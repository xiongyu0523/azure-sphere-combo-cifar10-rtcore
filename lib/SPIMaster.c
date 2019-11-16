/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include "SPIMaster.h"
#include "Common.h"
#include "NVIC.h"
#include "mt3620/spi.h"
#include "mt3620/dma.h"

// We currently disable half-duplex write transfers as a bug in the hardware causes
// the data to be bitshifted, we believe it's sending 1-bit of the opcode even in
// half-duplex mode.
// This define is left here to allow us to easily test for workarounds.
#undef SPI_ALLOW_TRANSFER_WRITE

// This is the maximum number of globbed transactions we allow to queue
#define SPI_MASTER_TRANSFER_COUNT_MAX 16

typedef enum {
    SPI_MASTER_TRANSFER_WRITE,
    SPI_MASTER_TRANSFER_READ,
    SPI_MASTER_TRANSFER_FULL_DUPLEX,
} SpiMaster_TransferType;

typedef struct {
    uint8_t            type;
    uint8_t            opcodeLen;
    uint8_t            payloadLen;
    uint8_t            transferCount;
    const SpiTransfer *transfer;
} SpiMaster_TransferGlob;

struct SpiMaster {
    uint32_t               id;
    bool                   open;
    bool                   dma;
    void                 (*callback)(int32_t, uintptr_t);
    SpiMaster_TransferGlob glob[SPI_MASTER_TRANSFER_COUNT_MAX];
    unsigned               globCount, globTransferred;
    int32_t                dataCount;
};

static SpiMaster spiContext[MT3620_SPI_COUNT] = { 0 };

// TODO: Reduce sysram usage by providing a more limited set of buffers?
// Each interface has two buffers for double buffering.
static __attribute__((section(".sysram"))) mt3620_spi_dma_cfg_t SpiMaster_DmaConfig[MT3620_SPI_COUNT] = { 0 };

#define SPI_PRIORITY 2

int32_t SpiMaster_Select(SpiMaster *handle, unsigned csLine)
{
    if (!handle->id) {
        return ERROR_PARAMETER;
    }
    if (!handle->open) {
        return ERROR_HANDLE_CLOSED;
    }

    switch (csLine) {
    case CS_LINE0:
    case CS_LINE1:
    case CS_NONE:
        break;
    default:
        return ERROR_UNSUPPORTED;
    }

    // Set the chip select line.
    SpiMaster_DmaConfig[handle->id].smmr.rs_slave_sel = csLine;

    return ERROR_NONE;
}

int32_t SpiMaster_Configure(SpiMaster *handle, bool cpol, bool cpha, uint32_t busSpeed)
{
    if (!handle) {
        return ERROR_PARAMETER;
    }
    if (!handle->open) {
        return ERROR_HANDLE_CLOSED;
    }

    // There's an errata for low busSpeed values when CPOL and CPHA are 0, so we increase the minimum.
    if ((cpol == 0) && (cpha == 0) && (busSpeed < MT3620_SPI_BUS_SPEED_250K)) {
        return ERROR_UNSUPPORTED;
    }

    // We round up the clock-speed division here to get the closest speed below the target.
    unsigned rs_clk_sel = ((MT3620_SPI_HCLK + (busSpeed - 1)) / busSpeed);
    rs_clk_sel = (rs_clk_sel < 2 ? 0 : (rs_clk_sel - 2));

    // Check we're not below the minimum speed.
    if (rs_clk_sel > 4095) {
        return ERROR_UNSUPPORTED;
    }

    unsigned rs_cl_sel_min = MT3620_RS_CLK_SEL_MIN;
    if (rs_clk_sel < rs_cl_sel_min) {
        rs_clk_sel = rs_cl_sel_min;
    }

    mt3620_spi_dma_cfg_t *cfg = &SpiMaster_DmaConfig[handle->id];
    cfg->smmr.cpol          = cpol;       // Set polarity for CPOL setting.
    cfg->smmr.cpha          = cpha;       // Set polarity for CPHA setting.
    cfg->smmr.rs_clk_sel    = rs_clk_sel; // Set serial clock SPI_CLK.
    cfg->smmr.more_buf_mode = 1;          // Select SPI buffer size.
    cfg->smmr.lsb_first     = false;      // Select MSB first.
    cfg->smmr.int_en        = true;       // Enable interrupts.

    return ERROR_NONE;
}

int32_t SpiMaster_ConfigDMA(SpiMaster *handle, bool enable)
{
    if (!handle) {
        return ERROR_PARAMETER;
    }
    if (!handle->open) {
        return ERROR_HANDLE_CLOSED;
    }

    if (handle->dma == enable) {
        return ERROR_NONE;
    }

    if (!MT3620_DMA_FIELD_READ(MT3620_SPI_DMA_TX(handle->id), start, str)) {
        return ERROR_BUSY;
    }

    MT3620_SPI_FIELD_WRITE(handle->id, cspol, dma_mode, enable);
    handle->dma = enable;
    return ERROR_NONE;
}


static inline unsigned SpiMaster_UnitToID(Platform_Unit unit)
{
    if ((unit < MT3620_UNIT_ISU0) || (unit > MT3620_UNIT_ISU5)) {
        return MT3620_SPI_COUNT;
    }
    return (unit - MT3620_UNIT_ISU0);
}

SpiMaster *SpiMaster_Open(Platform_Unit unit)
{
    unsigned id = SpiMaster_UnitToID(unit);
    if ((id >= MT3620_SPI_COUNT) || (spiContext[id].open)) {
        return NULL;
    }

    spiContext[id].id              = id;
    spiContext[id].open            = true;
    spiContext[id].dma             = true;
    spiContext[id].callback        = NULL;
    spiContext[id].globCount       = 0;
    spiContext[id].globTransferred = 0;
    spiContext[id].dataCount       = 0;

    // Select the CS line.
    int32_t status = SpiMaster_Select(&spiContext[id], 0);
    if (status != ERROR_NONE) {
        return NULL;
    }

    // Call SpiMaster_Configure to set up the chip for the SPI to 2 MHz by default.
    status = SpiMaster_Configure(&spiContext[id], CPOL0, CPHA0, MT3620_SPI_BUS_SPEED_2M);
    if (status != ERROR_NONE) {
        return NULL;
    }

    // Enable and Set the NVIC interrupt priority.
    NVIC_EnableIRQ(MT3620_SPI_INTERRUPT(id), SPI_PRIORITY);

    // Hard-code start to true in DMA transfers so transfer starts.
    mt3620_spi_dma_cfg_t *cfg = &SpiMaster_DmaConfig[id];
    cfg->stcsr.spi_master_start = true;

    volatile mt3620_dma_t * const tx_dma = &mt3620_dma[MT3620_SPI_DMA_TX(id)];
    mt3620_dma_global->ch_en_set = (1U << MT3620_SPI_DMA_TX(id));
    MT3620_DMA_FIELD_WRITE(MT3620_SPI_DMA_TX(id), start, str, false);
    mt3620_dma_con_t dma_con_tx = { .mask = tx_dma->con };
    dma_con_tx.dir   = 0;
    dma_con_tx.wpen  = false;
    dma_con_tx.wpsd  = 0;
    dma_con_tx.iten  = false;
    dma_con_tx.hiten = false;
    dma_con_tx.dreq  = false;
    dma_con_tx.dinc  = 0;
    dma_con_tx.sinc  = 1;
    dma_con_tx.size  = 2;
    tx_dma->con = dma_con_tx.mask;
    tx_dma->fixaddr = (uint32_t *)&mt3620_spi[id]->dataport;
    tx_dma->pgmaddr = (uint32_t *)&SpiMaster_DmaConfig[id];

    // Enable DMA mode.
    MT3620_SPI_FIELD_WRITE(id, cspol, dma_mode, true);

    // We have to set all buffers to 0xFF to ensure the line idles high.
    // This is due to a hardware bug in the SPI adapter on the MT3620.
    unsigned i;
    for (i = 0; i < (MT3620_SPI_BUFFER_SIZE_HALF_DUPLEX / 4); i++) {
        mt3620_spi[id]->sdor[i] = 0xFFFFFFFF;
    }
    mt3620_spi[id]->soar = 0xFFFFFFFF;

    MT3620_SPI_FIELD_WRITE(id, stcsr, spi_master_start, false);

    // Clear interrupt flag
    (void)mt3620_spi[id]->scsr;

    return &spiContext[id];
}

void SpiMaster_Close(SpiMaster *handle)
{
    if (!handle || !handle->open) {
        return;
    }

    mt3620_dma_global->ch_en_clr = (1U << MT3620_SPI_DMA_TX(handle->id));
    MT3620_SPI_FIELD_WRITE(handle->id, stcsr, spi_master_start, false); // Stop transfers
    MT3620_SPI_FIELD_WRITE(handle->id, smmr , int_en          , false); // Disable interrupts.
    MT3620_SPI_FIELD_WRITE(handle->id, cspol, dma_mode        , false); // Disable DMA mode.

    // Disable NVIC interrupts.
    NVIC_DisableIRQ(MT3620_SPI_INTERRUPT(handle->id));

    handle->open = false;
}


static inline void SpiMaster_WordCopy(volatile void *dst, volatile void *src, uintptr_t count)
{
    volatile uint32_t *udst = dst;
    volatile uint32_t *usrc = src;
    uintptr_t i;
    for (i = 0; i < count; i++) {
        udst[i] = usrc[i];
    }
}

static int32_t SpiMaster_TransferGlobQueue(SpiMaster *handle, SpiMaster_TransferGlob *glob)
{
    mt3620_spi_dma_cfg_t *cfg = &SpiMaster_DmaConfig[handle->id];

    switch (glob->type) {
#ifdef SPI_ALLOW_TRANSFER_WRITE
    case SPI_MASTER_TRANSFER_WRITE:
        cfg->smmr.both_directional_data_mode = false;

        cfg->smbcr.mosi_bit_cnt = (glob->payloadLen * 8);
        cfg->smbcr.miso_bit_cnt = 0;
        cfg->smbcr.cmd_bit_cnt  = 0;
        break;
#endif

    case SPI_MASTER_TRANSFER_READ:
        cfg->smmr.both_directional_data_mode = false;

        cfg->smbcr.mosi_bit_cnt = 0;
        cfg->smbcr.miso_bit_cnt = (glob->payloadLen * 8);
        cfg->smbcr.cmd_bit_cnt  = 0;
        break;

    case SPI_MASTER_TRANSFER_FULL_DUPLEX:
        cfg->smmr.both_directional_data_mode = true;

        cfg->smbcr.mosi_bit_cnt = (glob->payloadLen * 8);
        cfg->smbcr.miso_bit_cnt = (glob->payloadLen * 8);
        cfg->smbcr.cmd_bit_cnt  = (glob->opcodeLen  * 8);
        break;

    default:
        // This should never happen.
        return ERROR;
    }

    unsigned p = 0;
    unsigned t = 0;
    if (glob->opcodeLen >= 0) {
        const SpiTransfer *transfer = &glob->transfer[t++];

        // Reverse byte order of opcode as SPI is big-endian.
        uint32_t mask = 0xFFFFFFFF;
        const uint8_t *writeData = transfer->writeData;
        unsigned i;
        for (i = 0; i < glob->opcodeLen; i++) {
            mask <<= 8;
            mask |= writeData[i];
        }
        cfg->soar.mask = mask;

        p += (transfer->length - glob->opcodeLen);
        __builtin_memcpy(cfg->sdor, &writeData[glob->opcodeLen], p);
    }

    uint8_t *sdor = (uint8_t *)cfg->sdor;
    for (; t < glob->transferCount; t++) {
        const SpiTransfer *transfer = &glob->transfer[t];

        if (transfer->writeData) {
            __builtin_memcpy(&sdor[p], transfer->writeData, transfer->length);
        } else {
            __builtin_memset(&sdor[p], 0xFF, transfer->length);
        }
        p += transfer->length;
    }
    // This workaround is required to make the MOSI line idle high due to SPI bug.
    sdor[p] = 0xFF;

    if (handle->dma) {
        mt3620_dma[MT3620_SPI_DMA_TX(handle->id)].pgmaddr = (uint32_t *)&SpiMaster_DmaConfig[handle->id];
        mt3620_dma[MT3620_SPI_DMA_TX(handle->id)].count = (sizeof(mt3620_spi_dma_cfg_t) / 4);
        MT3620_DMA_FIELD_WRITE(MT3620_SPI_DMA_TX(handle->id), start, str, true);
    } else {
        SpiMaster_WordCopy(&mt3620_spi[handle->id]->soar, &SpiMaster_DmaConfig[handle->id], 11);
        mt3620_spi[handle->id]->stcsr = SpiMaster_DmaConfig[handle->id].stcsr.mask;
    }

    return ERROR_NONE;
}

static int32_t SpiMaster_TransferSequentialAsyncGlob(SpiMaster *handle, SpiMaster_TransferGlob *glob, uint32_t count, void(*callback)(int32_t status, uintptr_t dataCount))
{
    if (!callback) {
        return ERROR_PARAMETER;
    }
    if (!handle->open) {
        return ERROR_HANDLE_CLOSED;
    }
    if (count == 0) {
        return ERROR_PARAMETER;
    }

    handle->callback        = callback;
    handle->globCount       = count;
    handle->globTransferred = 0;
    handle->dataCount       = 0;

    int32_t status = SpiMaster_TransferGlobQueue(handle, &glob[0]);
    if (status != ERROR_NONE) {
        handle->callback  = NULL;
        handle->globCount = 0;
        return status;
    }

    return ERROR_NONE;
}

static int32_t SpiMaster_TransferGlobNew(const SpiTransfer *transfer, SpiMaster_TransferGlob *glob)
{
    if (transfer->writeData && transfer->readData) {
        return ERROR_UNSUPPORTED;
    }

    if (transfer->writeData) {
        if (transfer->length > (MT3620_SPI_BUFFER_SIZE_FULL_DUPLEX + MT3620_SPI_OPCODE_SIZE_FULL_DUPLEX)) {
            return ERROR_UNSUPPORTED;
        } else {
            glob->type       = SPI_MASTER_TRANSFER_FULL_DUPLEX;
            glob->opcodeLen  = (transfer->length > MT3620_SPI_OPCODE_SIZE_FULL_DUPLEX
                ? MT3620_SPI_OPCODE_SIZE_FULL_DUPLEX : transfer->length);
            glob->payloadLen = (transfer->length - glob->opcodeLen);
        }
    } else {
        if (transfer->length > MT3620_SPI_BUFFER_SIZE_HALF_DUPLEX) {
            return ERROR_UNSUPPORTED;
        }

        glob->type       = SPI_MASTER_TRANSFER_READ;
        glob->opcodeLen  = 0;
        glob->payloadLen = transfer->length;
    }

    glob->transferCount = 1;
    glob->transfer      = transfer;
    return ERROR_NONE;
}

static bool SpiMaster_TransferGlobAppend(const SpiTransfer *transfer, uint32_t count, SpiMaster_TransferGlob *glob)
{
    // If next transfer is a write but the one after involves a read, we don't glob it
    // as we need to glob a read onto a write due to hardware limitations.
    if (transfer[0].writeData && !transfer[0].readData && (count >= 2)) {
        if (transfer[1].writeData && transfer[1].readData) {
            return false;
        }
    }

    // We can't append a write/duplex to a half-duplex read.
    if ((glob->type == SPI_MASTER_TRANSFER_READ) && transfer->writeData) {
        return false;
    }

#ifdef SPI_ALLOW_TRANSFER_WRITE
    // We can't append a read/duplex to a half-duplex write.
    if ((glob->type == SPI_MASTER_TRANSFER_WRITE) && transfer->readData) {
        return false;
    }
#endif // #ifdef SPI_ALLOW_TRANSFER_WRITE

    unsigned payloadLimit;
    switch (glob->type) {
    case SPI_MASTER_TRANSFER_FULL_DUPLEX:
        payloadLimit = MT3620_SPI_BUFFER_SIZE_FULL_DUPLEX;
        break;

    case SPI_MASTER_TRANSFER_READ:
        payloadLimit = MT3620_SPI_BUFFER_SIZE_HALF_DUPLEX;
        break;

    case SPI_MASTER_TRANSFER_WRITE:
        // We must reserve the last byte to set the MOSI idle level high.
        // This is due to a bug in the MT3620 SPI interface.
        payloadLimit = MT3620_SPI_BUFFER_SIZE_HALF_DUPLEX - 1;
        break;

    default:
        return false;
    }

    if ((glob->payloadLen + transfer->length) > payloadLimit) {
#ifdef SPI_ALLOW_TRANSFER_WRITE
        if ((glob->type == SPI_MASTER_TRANSFER_FULL_DUPLEX) && !transfer->readData) {
            // Check if this transfer would overflow half-duplex glob.
            if ((glob->payloadLen + glob->opcodeLen + transfer->length) > (MT3620_SPI_BUFFER_SIZE_HALF_DUPLEX - 1)) {
                return false;
            }

            // Check if full-duplex glob contains reads.
            unsigned t;
            for (t = 0; t < glob->transferCount; t++) {
                if (glob->transfer[t].readData) {
                    return false;
                }
            }

            // If a full-duplex glob only contains writes, make it a write glob.
            glob->type = SPI_MASTER_TRANSFER_WRITE;
            glob->payloadLen += glob->opcodeLen;
            glob->opcodeLen = 0;
        } else
#endif //#ifdef SPI_ALLOW_TRANSFER_WRITE
        {
            return false;
        }
    }

    glob->transferCount++;
    glob->payloadLen += transfer->length;
    return true;
}

static void SpiMaster_TransferGlobFinalize(SpiMaster_TransferGlob *glob)
{
#ifdef SPI_ALLOW_TRANSFER_WRITE
    if (glob->type != SPI_MASTER_TRANSFER_FULL_DUPLEX) {
        return;
    }

    unsigned i;
    for (i = 0; i < glob->transferCount; i++) {
        if (glob->transfer[i].readData) {
            return;
        }
    }

    // If a full duplex glob only contains writes, make it a write glob.
    glob->type = SPI_MASTER_TRANSFER_WRITE;
    glob->payloadLen += glob->opcodeLen;
    glob->opcodeLen = 0;
#endif // #ifdef SPI_ALLOW_TRANSFER_WRITE
}

int32_t SpiMaster_TransferSequentialAsync(SpiMaster *handle, SpiTransfer *transfer, uint32_t count, void(*callback)(int32_t status, uintptr_t data_count))
{
    if (!handle) {
        return ERROR_PARAMETER;
    }
    if (!transfer || (count == 0)) {
        return ERROR_PARAMETER;
    }

    if (handle->callback) {
        return ERROR_BUSY;
    }

    SpiMaster_TransferGlob *glob = handle->glob;

    unsigned t = 0, g = 0;
    int32_t status;
    status = SpiMaster_TransferGlobNew(&transfer[t++], &glob[g]);
    if (status != ERROR_NONE) {
        return status;
    }

    for (; t < count; t++) {
        if (!SpiMaster_TransferGlobAppend(&transfer[t], (count - t), &glob[g])) {
            SpiMaster_TransferGlobFinalize(&glob[g]);

            if (++g >= SPI_MASTER_TRANSFER_COUNT_MAX) {
                return ERROR_UNSUPPORTED;
            }

            status = SpiMaster_TransferGlobNew(&transfer[t], &glob[g]);
            if (status != ERROR_NONE) {
                return status;
            }
        }
    }
    SpiMaster_TransferGlobFinalize(&glob[g]);

    handle->globCount = (g + 1);
    return SpiMaster_TransferSequentialAsyncGlob(handle, handle->glob, handle->globCount, callback);
}

static volatile bool SpiMaster_TransferSequentialSync_Ready = false;
static int32_t       SpiMaster_TransferSequentialSync_Status;
static int32_t       SpiMaster_TransferSequentialSync_Count;

static void SpiMaster_TransferSequentialSync_Callback(int32_t status, uintptr_t data_count)
{
    SpiMaster_TransferSequentialSync_Status = status;
    SpiMaster_TransferSequentialSync_Count = data_count;
    SpiMaster_TransferSequentialSync_Ready = true;
}

int32_t SpiMaster_TransferSequentialSync(SpiMaster *handle, SpiTransfer *transfer, uint32_t count)
{
    SpiMaster_TransferSequentialSync_Ready = false;
    int32_t status = SpiMaster_TransferSequentialAsync(handle, transfer, count, SpiMaster_TransferSequentialSync_Callback);
    if (status != ERROR_NONE) {
        return status;
    }

    while (!SpiMaster_TransferSequentialSync_Ready) {
        __asm__("wfi");
    }

    return SpiMaster_TransferSequentialSync_Status;
}

static void SpiMaster_IRQ(Platform_Unit unit)
{
    unsigned id = SpiMaster_UnitToID(unit);
    if (id >= MT3620_SPI_COUNT) {
        return;
    }

    SpiMaster *handle = &spiContext[id];

    // This should never happen
    if (!handle->open) {
        return;
    }

    if (handle->dma) {
        MT3620_DMA_FIELD_WRITE(MT3620_SPI_DMA_TX(id), start, str, false);
    }

    int32_t status = ERROR_NONE;

    // Clear interrupt flag and the status of the SPI transaction.
    if (!MT3620_SPI_FIELD_READ(id, scsr, spi_ok)) {
        status = ERROR_SPI_TRANSFER_FAIL;
    } else {
        SpiMaster_TransferGlob *glob = &handle->glob[handle->globTransferred];
        if (handle->dma && (mt3620_dma[MT3620_SPI_DMA_TX(id)].rlct != 0)){
            status = ERROR_SPI_TRANSFER_FAIL;
        } else {
            handle->dataCount += (glob->opcodeLen + glob->payloadLen);
        }
    }

    bool final = (status != ERROR_NONE);
    if (status == ERROR_NONE) {
        unsigned g;
        for (g = 0; g < handle->globCount; g++) {
            SpiMaster_TransferGlob *glob = &handle->glob[g];

            unsigned offset = 0;
            unsigned t;
            for (t = 0; t < glob->transferCount; t++) {
                const SpiTransfer *transfer = &glob->transfer[t];

                if (transfer->readData) {
                    unsigned sdirOffset = 0;
                    if (glob->type == SPI_MASTER_TRANSFER_FULL_DUPLEX) {
                        sdirOffset = 4;
                    }

                    // Read data out of SDIR.
                    uint8_t *src = (uint8_t*)&mt3620_spi[handle->id]->sdir[sdirOffset];
                    __builtin_memcpy(transfer->readData, &src[offset], transfer->length);
                }

                offset += transfer->length;
                if (t == 0) offset -= glob->opcodeLen;
            }
        }

        handle->globTransferred++;
        final = (handle->globTransferred >= handle->globCount);
        if (!final) {
            status = SpiMaster_TransferGlobQueue(
                handle, &handle->glob[handle->globTransferred]);
            final = (status != ERROR_NONE);
        }
    }

    if (final && handle->callback) {
        handle->callback(status, handle->dataCount);
        handle->callback  = NULL;
    }
}

void isu_g0_spim_irq(void) { SpiMaster_IRQ(MT3620_UNIT_ISU0); }
void isu_g1_spim_irq(void) { SpiMaster_IRQ(MT3620_UNIT_ISU1); }
void isu_g2_spim_irq(void) { SpiMaster_IRQ(MT3620_UNIT_ISU2); }
void isu_g3_spim_irq(void) { SpiMaster_IRQ(MT3620_UNIT_ISU3); }
void isu_g4_spim_irq(void) { SpiMaster_IRQ(MT3620_UNIT_ISU4); }
void isu_g5_spim_irq(void) { SpiMaster_IRQ(MT3620_UNIT_ISU5); }

#ifndef __MT3620_I2S_H__
#define __MT3620_I2S_H__

#include <stdbool.h>
#include <stdint.h>

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           en                        : 1;
        bool           dlfifo_en                 : 1;
        bool           ulfifo_en                 : 1;
        bool           engen_en                  : 1;
        bool           ext_io_ck                 : 1;
        bool           ext                       : 1;
        bool           ext_lrsw                  : 1;
        bool           dl_lrsw                   : 1;
        bool           dl_mono                   : 1;
        bool           dl_mono_dup               : 1;
        bool           mclk_output_e             : 1;
        bool           i2s_in_clk_en             : 1;
        const unsigned global_control_rsv_b17_13 : 5;
        bool           x26m_sel                  : 1;
        bool           ext_bclk_inv              : 1;
        bool           neg_cap                   : 1;
        const bool     global_control_rsv_b21    : 1;
        bool           dl_empty_value_en         : 1;
        bool           ul_empty_value_en         : 1;
        unsigned       clk_sel_in                : 2;
        unsigned       clk_sel_out               : 2;
        unsigned       ext_mclk_sel              : 2;
        const bool     global_control_rsv_b30    : 1;
        bool           loopback                  : 1;
    };

    uint32_t mask;
} mt3620_i2s_global_control_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           wlen                  : 1;
        bool           src                   : 1;
        bool           fmt                   : 1;
        const bool     dl_control_rsv_b4     : 1;
        bool           wsinv                 : 1;
        const bool     dl_control_rsv_b6     : 1;
        bool           dlfifo_2deq           : 1;
        unsigned       sr                    : 4;
        const bool     dl_control_rsv_b12    : 1;
        unsigned       bit_per_s             : 2;
        bool           ws_rsync              : 1;
        const bool     dl_control_rsv_b16    : 1;
        unsigned       msb_offset            : 7;
        const unsigned dl_control_rsv_b28_24 : 5;
        unsigned       ch_per_s              : 2;
        const bool     dl_control_rsv_b31    : 1;
    };

    uint32_t mask;
} mt3620_i2s_dl_control_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           en           : 1;
        bool           wlen         : 1;
        bool           src          : 1;
        unsigned       fmt          : 1;
        unsigned       dir          : 1;
        bool           wsinv        : 1;
        const unsigned res_7_6      : 2;
        unsigned       sr           : 4;
        bool           hden         : 1;
        unsigned       bit_per_s    : 2;
        bool           ws_rsync     : 1;
        bool           down_rate    : 1;
        unsigned       msb_offset   : 7;
        unsigned       update_world : 5;
        unsigned       ch_per_s     : 2;
        bool           lr_swap      : 1;
    };

    uint32_t mask;
} mt3620_i2s_ul_control_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           soft_rst     :  1;
        bool           glb_soft_rst :  1;
        const unsigned res_3_2      :  2;
        bool           dl_soft_rst  :  1;
        const unsigned res_7_5      :  3;
        bool           ul_soft_rst  :  1;
        const unsigned res_31_9     : 23;
    };

    uint32_t mask;
} mt3620_i2s_soft_reset_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           afull     : 1;
        bool           full      : 1;
        bool           w_clear   : 1;
        const unsigned res_7_3   : 5;
        unsigned       wfifo_cnt : 8;
        unsigned       w_thr     : 8;
        bool           w_ready   : 1;
        const unsigned res_31_25 : 7;
    };

    uint32_t mask;
} mt3620_i2s_dl_fifo_w_control_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           aempty    : 1;
        bool           empty     : 1;
        bool           r_clear   : 1;
        const unsigned res_7_3   : 5;
        unsigned       rfifo_cnt : 8;
        unsigned       r_thr     : 8;
        bool           r_ready   : 1;
        const unsigned res_31_25 : 7;
    };

    uint32_t mask;
} mt3620_i2s_dl_fifo_r_control_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           afull     : 1;
        bool           full      : 1;
        bool           w_clear   : 1;
        const unsigned res_7_3   : 5;
        unsigned       wfifo_cnt : 8;
        unsigned       w_thr     : 8;
        bool           w_ready   : 1;
        const unsigned res_31_25 : 7;
    };

    uint32_t mask;
} mt3620_i2s_ul_fifo_w_control_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           aempty    : 1;
        bool           empty     : 1;
        bool           r_clear   : 1;
        const unsigned res_7_3   : 5;
        unsigned       rfifo_cnt : 8;
        unsigned       r_thr     : 8;
        bool           r_ready   : 1;
        const unsigned res_31_25 : 7;
    };

    uint32_t mask;
} mt3620_i2s_ul_fifo_r_control_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        unsigned  dl_empty_value_r : 16;
        unsigned  dl_empty_value_l : 16;
    };

    uint32_t mask;
} mt3620_i2s_dl_empty_value_lr_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        unsigned  dl_empty_value_r1 : 16;
        unsigned  dl_empty_value_l1 : 16;
    };

    uint32_t mask;
} mt3620_i2s_dl_empty_value_l1r1_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        unsigned       dbg_sel     :  4;
        const unsigned res_5_4     :  2;
        bool           dbg_swap    :  1;
        bool           dbg_sel_src :  1;
        unsigned       res_31_8    : 24;
    };

    uint32_t mask;
} mt3620_i2s_debug_control_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        unsigned       dl_dmareq_mi_num :  2;
        const unsigned res_3_2          :  2;
        bool           dl_ahb_early_en  :  1;
        const unsigned res_14_5         : 10;
        bool           dl_dma_mode_sel  :  1;
        unsigned       ul_dmareq_mi_num :  2;
        bool           ul_ahb_early_en  :  1;
        const unsigned res_30_21        : 10;
        bool           ul_dma_mode_sel  :  1;
    };

    uint32_t mask;
} mt3620_i2s_dma_if_control_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           write_dl_fifo_when_full                    :  1;
        bool           read_dl_fifo_when_ready                    :  1;
        bool           dl_fifow_ready_asserted                    :  1;
        bool           dl_fifor_ready_asserted                    :  1;
        const unsigned res_7_6                                    :  2;
        bool           write_ul_fifo_when_full                    :  1;
        bool           read_ul_fifo_when_ready                    :  1;
        const unsigned res_11_10                                  :  2;
        bool           ul_fifow_ready_asserted                    :  1;
        bool           ul_fifor_ready_asserted                    :  1;
        const unsigned res_15_14                                  :  2;
        bool           dl_dma_request_multi_issue_count_overflow  :  1;
        bool           dl_dma_request_multi_issue_count_underflow :  1;
        bool           ul_dma_request_multi_issue_count_overflow  :  1;
        bool           ul_dma_request_multi_issue_count_underflow :  1;
        const unsigned res_31_20                                  : 12;
    };

    uint32_t mask;
} mt3620_i2s_global_int_en_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           write_dl_fifo_when_full                    :  1;
        bool           read_dl_fifo_when_ready                    :  1;
        bool           dl_fifow_ready_asserted                    :  1;
        bool           dl_fifor_ready_asserted                    :  1;
        const unsigned res_7_6                                    :  2;
        bool           write_ul_fifo_when_full                    :  1;
        bool           read_ul_fifo_when_ready                    :  1;
        const unsigned res_11_10                                  :  2;
        bool           ul_fifow_ready_asserted                    :  1;
        bool           ul_fifor_ready_asserted                    :  1;
        const unsigned res_15_14                                  :  2;
        bool           dl_dma_request_multi_issue_count_overflow  :  1;
        bool           dl_dma_request_multi_issue_count_underflow :  1;
        bool           ul_dma_request_multi_issue_count_overflow  :  1;
        bool           ul_dma_request_multi_issue_count_underflow :  1;
        const unsigned res_31_20                                  : 12;
    };

    uint32_t mask;
} mt3620_i2s_global_sts_clr_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           write_dl_fifo_when_full                    :  1;
        bool           read_dl_fifo_when_ready                    :  1;
        bool           dl_fifow_ready_asserted                    :  1;
        bool           dl_fifor_ready_asserted                    :  1;
        const unsigned res_7_6                                    :  2;
        bool           write_ul_fifo_when_full                    :  1;
        bool           read_ul_fifo_when_ready                    :  1;
        const unsigned res_11_10                                  :  2;
        bool           ul_fifow_ready_asserted                    :  1;
        bool           ul_fifor_ready_asserted                    :  1;
        const unsigned res_15_14                                  :  2;
        bool           dl_dma_request_multi_issue_count_overflow  :  1;
        bool           dl_dma_request_multi_issue_count_underflow :  1;
        bool           ul_dma_request_multi_issue_count_overflow  :  1;
        bool           ul_dma_request_multi_issue_count_underflow :  1;
        const unsigned res_31_20                                  : 12;
    };

    uint32_t mask;
} mt3620_i2s_global_int_sts_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_din_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_dout_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_dout_se_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           ad_i2s0_bclk  :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_dout_reset_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_oe_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_oe_set_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_oe_reset_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_pu_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_pu_set_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_pu_reset_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_pd_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_pd_set_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_pd_reset_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_sr_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   : 1;
        bool           pad_i2s0_mclk : 1;
        bool           pad_i2s0_fs   : 1;
        bool           pad_i2s0_rx   : 1;
        bool           pad_i2s0_bclk : 1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_sr_set_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_sr_reset_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_ies_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_ies_set_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        bool           pad_i2s0_tx   :  1;
        bool           pad_i2s0_mclk :  1;
        bool           pad_i2s0_fs   :  1;
        bool           pad_i2s0_rx   :  1;
        bool           pad_i2s0_bclk :  1;
        const unsigned res_31_5      : 27;
    };

    uint32_t mask;
} mt3620_i2s_gpio_ies_reset_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        unsigned       pad_i2s0_tx   :  2;
        unsigned       pad_i2s0_mclk :  2;
        unsigned       pad_i2s0_fs   :  2;
        unsigned       pad_i2s0_rx   :  2;
        unsigned       pad_i2s0_bclk :  2;
        const unsigned res_31_10     : 22;
    };

    uint32_t mask;
} mt3620_i2s_gpio_paddrv_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        unsigned pad_i2s0_tx   :  2;
        unsigned       pad_i2s0_mclk :  2;
        unsigned       pad_i2s0_fs   :  2;
        unsigned       pad_i2s0_rx   :  2;
        unsigned       pad_i2s0_bclk :  2;
        const unsigned res_31_10     : 22;
    };

    uint32_t mask;
} mt3620_i2s_gpio_pdsel_t;

typedef union __attribute__((__packed__)) {
    struct __attribute__((__packed__)) {
        unsigned       pad_i2s0_tx   :  4;
        unsigned       pad_i2s0_mclk :  4;
        unsigned       pad_i2s0_fs   :  4;
        unsigned       pad_i2s0_rx   :  4;
        unsigned       pad_i2s0_bclk :  4;
        const unsigned res_31_20     : 12;
    };

    uint32_t mask;
} mt3620_i2s_gpio_tdsel_t;

typedef struct {
    volatile uint32_t       global_control;
    volatile uint32_t       dl_control;
    volatile uint32_t       ul_control;
    volatile uint32_t       soft_reset;
    volatile uint32_t       dl_fifo_w_control;
    volatile uint32_t       dl_fifo_r_control_t;
    volatile uint32_t       dl_fifo_w_control;
    volatile uint32_t       dl_fifo_r_control_t;
    volatile uint32_t       dl_empty_value_lr;
    volatile uint32_t       dl_empty_value_l1r1;
    volatile uint32_t       debug_control;
    volatile const uint32_t debug_probe;
    volatile uint32_t       dma_if_cobntrol;
    volatile const uint32_t res_16_14[3];
    volatile uint32_t       global_int_en;
    volatile uint32_t       global_sts_clear;
    volatile uint32_t       global_int_sts;
    volatile const uint32_t res_44_20[45];
    volatile uint32_t       gpio_din;
    volatile const uint32_t res_48_46[3];
    volatile uint32_t       gpio_dout;
    volatile uint32_t       gpio_dout_set;
    volatile uint32_t       gpio_dout_reset;
    volatile const uint32_t res_52;
    volatile uint32_t       gpio_oe;
    volatile uint32_t       gpio_oe_set;
    volatile uint32_t       gpio_oe_reset;
    volatile const uint32_t res_56;
    volatile uint32_t       gpio_pu;
    volatile uint32_t       gpio_pu_set;
    volatile uint32_t       gpio_pu_reset;
    volatile const uint32_t res_60;
    volatile uint32_t       gpio_pd;
    volatile uint32_t       gpio_pd_set;
    volatile uint32_t       gpio_pd_reset;
    volatile const uint32_t res_64;
    volatile uint32_t       gpio_sr;
    volatile uint32_t       gpio_sr_set;
    volatile uint32_t       gpio_sr_reset;
    volatile const uint32_t res_68;
    volatile uint32_t       gpio_ies;
    volatile uint32_t       gpio_ies_set;
    volatile uint32_t       gpio_ies_reset;
    volatile const uint32_t res_72;
    volatile uint32_t       paddrv;
    volatile const uint32_t res_76_74[3];
    volatile uint32_t       rdsel;
    volatile const uint32_t res_80_78[3];
    volatile uint32_t       tdsel;
} mt3620_i2s_t;

#define MT3620_I2S_INTERRUPT(x) (68 + (x))

#define MT3620_I2S_COUNT 2
static volatile mt3620_i2s_t * const mt3620_i2s[MT3620_I2S_COUNT] = {
    (volatile mt3620_i2s_t *)0x380d0000,
    (volatile mt3620_i2s_t *)0x380e0000
};

#define MT3620_I2S_FIELD_READ(index, reg, field) \
        ((mt3620_i2s_##reg##_t)mt3620_i2s[index]->reg).field
#define MT3620_I2S_FIELD_WRITE(index, reg, field, value) \
        do { mt3620_i2s_##reg##_t reg = { .mask = mt3620_i2s[index]->reg }; reg.field = value; /
        mt3620_i2s[index]->reg = reg.mask; } while (0)

#endif

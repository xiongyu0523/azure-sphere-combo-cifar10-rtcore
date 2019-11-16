#include "Test.h"
#include "Pattern.h"
class FastMathF32:public Client::Suite
    {
        public:
            FastMathF32(Testing::testID_t id);
            void setUp(Testing::testID_t,std::vector<Testing::param_t>& params,Client::PatternMgr *mgr);
            void tearDown(Testing::testID_t,Client::PatternMgr *mgr);
        private:
            #include "FastMathF32_decl.h"
            Client::Pattern<float32_t> samples;

            Client::LocalPattern<float32_t> output;
            
            int nbSamples;

            float32_t *pSrc;
            float32_t *pDst;
            
            
    };
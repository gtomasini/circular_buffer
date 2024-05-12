#include <iostream>
#include <thread>
#include <cassert>
#include "Module1.h"

using namespace modules;

#undef DEBUG

static const auto MaxBuffers2Sent = 
#ifdef DEBUG
//just for debug
20;
#else
100;
#endif

void Module1::doMyJob() {
    for(;!_running;)std::this_thread::sleep_for(std::chrono::seconds(10));

	std::cout << std::this_thread::get_id()
	        << "-"<<__PRETTY_FUNCTION__ << std::endl;
    for(;_running;){
    #ifndef DEBUG
        uint8_t outBuff[ArrayLenMax];
    #else
        //just for debug
        uint8_t outBuff[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
                            17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
    #endif

        unsigned totalLen = 0;
        for (auto i=0; i< MaxBuffers2Sent; i++){
    #ifndef DEBUG
            memset (outBuff, 0x00, ArrayLenMax);
            const auto len = rand() % ArrayLenMax;//array len 0-99
            if (len==0) continue;
            // fill  the array with random values
            for (auto offset = 0; offset< len; ++offset)
                outBuff[offset] = rand() % ArrayValMax;//0-254
    #else
            //just for debug
            const auto len = sizeof(outBuff)/sizeof(outBuff[0]);
    #endif
            std::cout << "mod1, writes " << std::dec<<(unsigned)len << " bytes\n";
            uint8_t st(0);
            auto buf_1_to_2_ptr=_mod2.getBufferPtr();
            assert(buf_1_to_2_ptr);
            do {
                st = buf_1_to_2_ptr->write(outBuff, len);
            } while (st == 0);
            totalLen += len;
        }
        std::cout << "mod1, total written bytes: " << std::dec<<totalLen << std::endl;
    }
	std::cout << "mod1, ends...\n";
}
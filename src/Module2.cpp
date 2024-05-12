#include <iostream>
#include <cassert>
#include "Module2.h"
#include "Color.h"

using namespace modules;

void Module2::setMatch (const uint8_t* const matchPtr, uint8_t len) {
    if (len > ArrayLenMax)	len = ArrayLenMax;
    _matchPtr = matchPtr;
    _matchLen = len;
    std::cout << Clr::ma<<"mod2, match[]: "<<Clr::def;
    for (int i = 0; i < len; i++)
        std::cout << (int)_matchPtr[i] << ",";
    std::cout<<std::endl;
}

bool Module2::processInBuf (const uint8_t* buf, uint8_t len) const {
	for (auto offset=0; offset < (len-_matchLen); offset++)
		if (!memcmp (buf + offset, _matchPtr, _matchLen))
			return true;
	return false;
}

void Module2::doMyJob() {
    for(;!_running;)std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << Clr::ma<<std::this_thread::get_id()
	        << "-"<<__PRETTY_FUNCTION__ << std::endl<<Clr::def;

    for(;_running;){
        uint8_t inBuff[ArrayLenMax];
        uint8_t n = 0;//len of read buffer
        unsigned totalLen = 0;
        auto buf_2_to_3_ptr=_mod3.getBufferPtr();
        assert(buf_2_to_3_ptr != nullptr);
        do {
            memset (inBuff, 0x00, ArrayLenMax);//just for debug
            n = _buf.read (inBuff, ArrayLenMax);
            std::cout << Clr::ma<<"mod2, reads " << std::dec<<(unsigned)n << " bytes, "<<Clr::def;
            const auto st=processInBuf (inBuff, n);
            if (st) {
    #ifdef DEBUG
                std::cout << " matchs pattern, resend to thr3!\n";
    #endif
                uint8_t st(0);
                do {
                    st = buf_2_to_3_ptr->write(inBuff, n);
                } while (st == 0);

            }
            else
    #ifdef DEBUG
                std::cout << "no matchs.\n";
    #endif
            totalLen += n;
        } while (n > 0);
        std::cout << Clr::ma<<"mod2, total read bytes: " << std::dec<<totalLen << std::endl<<Clr::def;
    }
	std::cout << Clr::ma<<"mod2, ends...\n"<<Clr::def;
}

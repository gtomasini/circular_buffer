#ifndef IMODULE
#define IMODULE

#include <thread>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>      /* puts, snprintf */
#include <time.h> 
#include "WrapBuffer.h"

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace Modules {
	const uint8_t ArrayLenMax = 100;
	const uint8_t ArrayValMax = 0xff;
	const uint16_t BufferLen = 1024;

	struct iModule {
		virtual void doJob() const = 0;
		virtual void setWrapBuf_1_to_2 (buffers::WrapBuffer<BufferLen, true>* wrapBufPtr) {
			_wrapBuf_1_to_2_Ptr = wrapBufPtr;
		}
		virtual void setWrapBuf_2_to_3 (buffers::WrapBuffer<BufferLen, true>* wrapBufPtr) {
			_wrapBuf_2_to_3_Ptr = wrapBufPtr;
		}

	protected:
		buffers::WrapBuffer<BufferLen, true>* _wrapBuf_1_to_2_Ptr = nullptr;
		buffers::WrapBuffer<BufferLen, true>* _wrapBuf_2_to_3_Ptr = nullptr;
	};

	struct Module1 :iModule {
		void doJob() const;
		~Module1() {};
	};

	struct Module2 :iModule {
		void doJob () const;
		void setMatch (const uint8_t* const matchPtr, uint8_t len) {
			if (len > ArrayLenMax)	len = ArrayLenMax;
			_matchPtr = matchPtr;
			_matchLen = len;
			std::cout << "mod2, match[]: ";
			for (int i = 0; i < len; i++)
				std::cout << (int)_matchPtr[i] << ",";
			std::cout<<std::endl;
		}
		bool processInBuf (const uint8_t* buf, uint8_t len) const;

		~Module2() {};
	private:
		uint8_t const *_matchPtr;
		uint8_t _matchLen = 0;
	};

	struct Module3 :iModule {
		void doJob() const;
		~Module3() {};
	};

	using mapType = std::map<uint64_t, std::vector<uint8_t>>;
	const mapType *getfinalTimeVecMap();
}
#endif

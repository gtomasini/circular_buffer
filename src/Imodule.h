#ifndef IMODULE
#define IMODULE

#include <thread>
#include <string>
#include <map>
#include <vector>
#include <cstdio>      /* puts, snprintf */
#include <ctime> 
#include "RingBuffer.h"


namespace Modules {
const uint8_t ArrayLenMax = 100;
const uint8_t ArrayValMax = 0xff;
const uint16_t BufferLen = 100;

struct iModule {
	virtual void doJob() const = 0;
	
	virtual void setWrapBuf_1_to_2 (buffers::RingBuffer<uint8_t, BufferLen, true>* ptr) {
		_buf_1_to_2_ptr = ptr;
	}
	virtual void setWrapBuf_2_to_3 (buffers::RingBuffer<uint8_t, BufferLen, true>* ptr) {
		_buf_2_to_3_ptr = ptr;
	}

protected:
	buffers::RingBuffer<uint8_t, BufferLen, true>* _buf_1_to_2_ptr = nullptr;
	buffers::RingBuffer<uint8_t, BufferLen, true>* _buf_2_to_3_ptr = nullptr;
};

struct Module1 :iModule {
	void doJob() const override;
	virtual ~Module1() = default;
};

struct Module2 :iModule {
	void doJob () const override;
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

	virtual ~Module2() = default;
private:
	uint8_t const *_matchPtr=nullptr;
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

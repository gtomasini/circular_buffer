#pragma once
#include "Imodule.h"
#include "RingBuffer.h"

namespace modules {

struct Module2:iModule{
    Module2(iModule &mod3):_mod3(mod3){}

    void doMyJob() override;
    inline BufferType* getBufferPtr() override{
        return &_buf;
    }
	void setMatch (const uint8_t* const matchPtr, uint8_t len);
    virtual ~Module2() = default;

private:
	bool processInBuf (const uint8_t* buf, uint8_t len) const;
    iModule &_mod3;
	//buffers::RingBuffer<uint8_t, BufferLen, true>* _buf_1_to_2_ptr = nullptr;
    BufferType _buf;
    uint8_t const *_matchPtr=nullptr;
	uint8_t _matchLen = 0;
};
}
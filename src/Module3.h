#pragma once
#include "Imodule.h"
#include "RingBuffer.h"

namespace modules {

struct Module3:iModule{
    void doMyJob() override;
    inline BufferType* getBufferPtr() override{
        return &_buf;
    }
    virtual ~Module3() = default;

protected:
	//buffers::RingBuffer<uint8_t, BufferLen, true>* _buf_2_to_3_ptr = nullptr;   
    BufferType _buf;
};
}
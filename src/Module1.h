#pragma once
#include "Imodule.h"
#include "RingBuffer.h"

namespace modules {

using BufferType=buffers::RingBuffer<uint8_t, BufferLen, true>;
struct Module1:iModule{
    Module1(iModule &mod2):_mod2(mod2){}

    void doMyJob() override;
    BufferType *getBufferPtr(){return nullptr;};//not used!!!
    virtual ~Module1() = default;

protected:
    iModule &_mod2;//my consumer
};
}
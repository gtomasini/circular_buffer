#pragma once
#include <cstdint>
#include <map>
#include <vector>
#include <atomic>
#include "RingBuffer.h"

namespace modules {
const uint8_t ArrayLenMax = 100;
const uint8_t ArrayValMax = 0xff;
const uint16_t BufferLen = 100;

using BufferType=buffers::RingBuffer<uint8_t, BufferLen, true>;

struct iModule {
	virtual void doMyJob() = 0;
	virtual BufferType *getBufferPtr()=0;
	virtual void start(){_running=true;};
	virtual void stop(){_running=false;};
protected:
	std::atomic_bool _running = false;

};

using mapType = std::map<uint64_t, std::vector<uint8_t>>;
const mapType *getfinalTimeVecMap();
}


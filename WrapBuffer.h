#pragma once
#include "RingBuffer.h"
#include <cassert>
#include <iostream>

//this uses RingBuffer with sincronization

namespace buffers {
const std::chrono::seconds  _timeout = std::chrono::seconds(1);

template <size_t N = 100, bool DEBUG=false>

struct WrapBuffer {
    WrapBuffer () = default;

    inline auto getUsedSlots() const { return _ring_buffer.getUsedSlots(); }
    inline auto getUnusedSlots() const { return _ring_buffer.getFreeSlots(); }

    // n: max to write 255 bytes
    // ptr: pointer to data
    //if timeout return 0
    uint8_t write (const uint8_t* ptr, uint8_t len) {
        if (len==0 ||ptr==nullptr) return 0;

        const auto n =_ring_buffer.write (ptr, len);
        assert(len == n);
        return len;
    }

    //return read bytes 
    //ptr: pointer to buffer (warning with its size)
    //if timeout return 0
    uint8_t read (uint8_t* ptr) {
        const auto n = _ring_buffer.read (ptr, N);
        assert (n > 0);
        return n;
    }

private:
    RingBuffer <uint8_t, N, DEBUG> _ring_buffer;
};
}

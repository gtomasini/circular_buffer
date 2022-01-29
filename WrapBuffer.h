#ifndef WRAPBUFFER
#define WRAPBUFFER
#include "RingBuffer.h"
#include <cassert>

namespace buffers {

    //TODO: mutex it
    template <size_t N = 1000, bool DEBUG=false>
    struct WrapBuffer {
        RingBuffer <uint8_t, N, DEBUG> _buffer;
        auto used() { return _buffer.used(); }
        auto unused() { return _buffer.unused(); }

        //max to write 255 bytes
        uint8_t write (uint8_t* ptr, uint8_t n) {
            if (_buffer.unused() + sizeof(n) < n) return 0;
            _buffer.write (&n, sizeof(n));
            _buffer.write (ptr, n);
            return n;
        }

        //return read bytes 
        uint8_t read (uint8_t* ptr) {
            if (_buffer.used () == 0) return 0;
            uint8_t n;
            _buffer.read (&n, sizeof(n));
            auto k = _buffer.read (ptr, n);
            assert (k == n);
            return n;
        }
    };

}
#endif
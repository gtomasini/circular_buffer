#ifndef WRAPBUFFER
#define WRAPBUFFER
#include "RingBuffer.h"
#include <cassert>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace buffers {

    template <size_t N = 1000, bool DEBUG=false>
    struct WrapBuffer {
    private:
        RingBuffer <uint8_t, N, DEBUG> _buffer;
        std::mutex *_mtx;
        std::condition_variable _cv_item_available;
    public:
        WrapBuffer(std::mutex& mtx_ptr) 
            :_mtx(&mtx_ptr) {}

        auto used() { return _buffer.used(); }
        auto unused() { return _buffer.unused(); }

        //max to write 255 bytes
        uint8_t write (uint8_t* ptr, uint8_t n) {
            std::lock_guard<std::mutex> lk(*_mtx);

            if (n + sizeof(n) > _buffer.unused()) {
                std::cout << std::this_thread::get_id();
                std::cout << "write() buffer full..\n";
                //_cv.wait(lk, [&]{return n + sizeof(n) <= _buffer.unused() ; });
                //std::cout << "write() notified...\n";
                return 0;
            }

            auto n1=_buffer.write (&n, sizeof(n));
            auto n2=_buffer.write (ptr, n);
            assert(n1 + n2 == n+sizeof(n));
            _cv_item_available.notify_one();
            return n;
        }

        //return read bytes 
        uint8_t read (uint8_t* ptr) {
            std::unique_lock<std::mutex> lk(*_mtx);

            while (_buffer.used() == 0) {
                std::cout << std::this_thread::get_id();
                std::cout << " read() buffer empty...\n";
                _cv_item_available.wait (lk, [&] {return _buffer.used() > 0; });
                //std::cout << "read() notified...\n";
                //return 0;
            }
            uint8_t n;
            _buffer.read (&n, sizeof(n));
            auto k = _buffer.read (ptr, n);
            assert (k == n);
            //_cv.notify_one();
            return n;
        }
    };
}
#endif
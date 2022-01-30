#ifndef WRAPBUFFER
#define WRAPBUFFER
#include "RingBuffer.h"
#include <cassert>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <chrono>

namespace buffers {
    template <size_t N = 1000, bool DEBUG=false>
    struct WrapBuffer {
    private:
        RingBuffer <uint8_t, N, DEBUG> _buffer;
        std::mutex *_mtx;
        std::condition_variable _item_available;
        std::condition_variable _space_available;
        std::chrono::seconds    _timeout=std::chrono::seconds(3);

    public:
        WrapBuffer(std::mutex& mtx_ptr) 
            :_mtx(&mtx_ptr) {}

        auto used() { return _buffer.used(); }
        auto unused() { return _buffer.unused(); }

        //max to write 255 bytes
        uint8_t write (uint8_t* ptr, uint8_t n) {
            std::unique_lock<std::mutex> lk(*_mtx);
            while (n + sizeof(n) > _buffer.unused()) {
                std::cout << std::this_thread::get_id();
                std::cout << " write() buffer full..\n";
                if (_space_available.wait_for(lk, _timeout, 
                        [&] {return n + sizeof(n) <= _buffer.unused(); }) == false) {
                    std::cout << " write() timeout!!\n";
                    return 0;
                }
            }
            std::cout << std::this_thread::get_id();
            std::cout << " write() gets lock...\n";
            auto n1=_buffer.write (&n, sizeof(n));
            auto n2=_buffer.write (ptr, n);
            assert(n1 + n2 == n+sizeof(n));
            lk.unlock();
            _item_available.notify_one();
            return n;
        }

        //return read bytes 
        uint8_t read (uint8_t* ptr) {
            std::unique_lock<std::mutex> lk(*_mtx);
            while (_buffer.used() == 0) {
                std::cout << std::this_thread::get_id();
                std::cout << " read() buffer empty...\n";
                if (_item_available.wait_for(lk, _timeout,
                        [&] {return _buffer.used() > 0; }) == false) {
                    std::cout << " read() timeout!!\n";
                    return 0;
                }
            }
            std::cout << std::this_thread::get_id();
            std::cout << " read() gets lock...\n";
            uint8_t n;
            _buffer.read (&n, sizeof(n));
            auto k = _buffer.read (ptr, n);
            assert (k == n);
            lk.unlock();
            _space_available.notify_one();
            return n;
        }
    };
}
#endif
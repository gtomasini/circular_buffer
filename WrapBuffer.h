#ifndef WRAPBUFFER
#define WRAPBUFFER
#include "RingBuffer.h"
#include <cassert>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <chrono>

//this uses RingBuffer with sincronization

namespace buffers {
    template <size_t N = 1000, bool DEBUG=false>
    struct WrapBuffer {
        const std::chrono::seconds  _timeout = std::chrono::seconds(5);
    private:
        RingBuffer <uint8_t, N, DEBUG> _buffer;
        std::mutex *_mtx;
        std::condition_variable _item_available;
        std::condition_variable _space_available;

    public:
        WrapBuffer (std::mutex& mtx_ptr) 
            :_mtx(&mtx_ptr) {}

        auto used() const { return _buffer.used(); }
        auto unused() const { return _buffer.unused(); }

        //n: max to write 255 bytes
        //ptr: pointer to data
        uint8_t write (const uint8_t* ptr, uint8_t len) {
            std::unique_lock<std::mutex> lk(*_mtx);
            while (len + sizeof(len) > _buffer.unused()) {
                std::cout << std::this_thread::get_id();
                std::cout << " write() buffer almost full ("<<_buffer.unused()<<")\n";
                if (_space_available.wait_for(lk, _timeout, 
                        [&] {return len + sizeof(len) <= _buffer.unused(); }) == false) {
                    std::cout << std::this_thread::get_id();
                    std::cout << " write() timeout!!\n";
                    return 0;
                }
            }
            std::cout << std::this_thread::get_id();
            std::cout << " write() gets lock...\n";
            auto len1=_buffer.write (&len, sizeof(len));
            auto len2=_buffer.write (ptr, len);
            assert(len1 + len2 == len+sizeof(len));
            lk.unlock();
            _item_available.notify_one();
            return len;
        }

        //return read bytes 
        //ptr: pointer to buffer (warning with its size)
        uint8_t read (uint8_t* ptr) {
            std::unique_lock<std::mutex> lk(*_mtx);
            while (_buffer.used() == 0) {
                std::cout << std::this_thread::get_id();
                std::cout << " read() buffer empty...\n";
                if (_item_available.wait_for(lk, _timeout,
                        [&] {return _buffer.used() > 0; }) == false) {
                    std::cout << std::this_thread::get_id();
                    std::cout << " read() timeout!!\n";
                    return 0;
                }
            }
            std::cout << std::this_thread::get_id();
            std::cout << " read() gets lock...\n";
            uint8_t len;
            _buffer.read (&len, sizeof(len));
            auto n = _buffer.read (ptr, len);
            assert (n == len);
            lk.unlock();
            _space_available.notify_one();
            return len;
        }
    };
}
#endif
#pragma once

#include <algorithm> // for std::min
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <thread>
// generic circular buffer implementation

namespace buffers {
// N is size of buffer
// T data type
template <class T, size_t N=1000, bool DEBUG=true>
struct RingBuffer {
    const std::chrono::seconds  _timeout = std::chrono::seconds(3);
    RingBuffer() {
        std::cout<<__PRETTY_FUNCTION__<<std::endl;
        //_pdata= new T[N];
        if (DEBUG)   memset (_pdata, 0x00, sizeof(T)*N);
    }

    ~RingBuffer(){
        //delete [] _pdata;
    }

    inline size_t getUsedSlots () const { return _used_slots; }
    constexpr inline size_t getTotalSize () const { return N; }
    inline size_t getFreeSlots () const { return N - _used_slots; }

    // writes n items from ptr_data
    // return number of items written.
    size_t write (const T* ptr_data, size_t n) {
        if (n == 0 || ptr_data==nullptr
            ||n > N) return 0;
        std::unique_lock<std::mutex> lk(_mtx);
        //const auto n = std::min (len, N - _used_slots);// #items to write

        if (_space_available.wait_for(lk, _timeout, 
                    [&] {return n <= getFreeSlots(); }) == false) {
            if (DEBUG){		
                std::cerr << std::this_thread::get_id();
                std::cerr << " write() timeout !!\n";
            }
            lk.unlock();
            return 0;
        }
        if (n <= N - _end_offset) {// one step write because there is room
            memcpy (_pdata + _end_offset, ptr_data, n*sizeof(T));
            _end_offset += n;
            if (_end_offset == N) _end_offset = 0;
        }
        else {// 2 steps write because because the available buffer flips
            const size_t len1{ N - _end_offset };
            memcpy (_pdata + _end_offset, ptr_data, len1*sizeof(T));
            const size_t len2{ n - len1 };
            memcpy (_pdata, ptr_data + len1 * sizeof(T), len2*sizeof(T));
            _end_offset = len2;
        }
        _used_slots += n;
        lk.unlock();
        _item_available.notify_one();// notify to reader
        return n;
    }

    // reads maximum n items
    // return number of items read
    // if max>used_items returns 0
    size_t read (T* ptr_data, size_t max) {
        std::unique_lock<std::mutex> lk(_mtx);
        //const size_t n = std::min(len, _used_slots);// #items to read
        if (max == 0 || ptr_data==nullptr
            ||_used_slots > max) return 0;

        if (_item_available.wait_for(lk, _timeout,
                [&] {return _used_slots > 0; }) == false) {
            if (DEBUG){
                std::cerr << std::this_thread::get_id();
                std::cerr << " read() timeout!!\n";
            }
            lk.unlock();
            return 0;
        }
        const auto n=_used_slots;
        if (n <= N - _begin_offset) {// one step read
            memcpy (ptr_data, _pdata + _begin_offset, n*sizeof(T));
            if (DEBUG) memset (_pdata + _begin_offset, 0x00, n*sizeof(T));
            if (_begin_offset += n; _begin_offset == N) _begin_offset = 0;
        }
        else {// 2 steps read because the used buffer flips
            const size_t len1{ N - _begin_offset };
            memcpy (ptr_data, _pdata + _begin_offset, len1*sizeof(T));
            const size_t len2{ n - len1 };
            memcpy (ptr_data+len1*sizeof(T), _pdata, len2*sizeof(T));
            if (DEBUG) {
                memset (_pdata + _begin_offset, 0x00, len1 * sizeof(T));
                memset (_pdata, 0xff, len2 * sizeof(T));
            }
            _begin_offset = len2;
        }
        _used_slots -= n;
        lk.unlock();
        _space_available.notify_one();

        return n;
    }

protected:
    size_t _begin_offset = 0, _end_offset = 0;// begin and end offsets
    size_t _used_slots = 0;// used entries
    T _pdata[N];//=nullptr;// ptr to buffer
    std::mutex _mtx;
    std::condition_variable _item_available;
    std::condition_variable _space_available;
};
}


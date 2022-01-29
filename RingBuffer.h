#ifndef RINGBUFFER
#define RINGBUFFER

#include <algorithm> // for std::min
#include <string.h>

//generic circular buffer implementation
//I thin I will use here with T as uint8_t type

namespace buffers {
    template <class T, size_t N=1000, bool DEBUG=false>
    struct RingBuffer {
    private:
        size_t _begin_offset, _end_offset;//begin and end ingexes
        size_t _used;//used entries
        T _data[N];

    public:
        RingBuffer() :
            _begin_offset(0), _end_offset(0), _used(0) {
            if (DEBUG)
                memset (_data, 0x00, sizeof(T)*N);
        }

        size_t used() const { return _used; }
        size_t capacity() const { return N; }
        size_t unused() const { return N - _used; }

        //write min(n, _capacity-_used) items
        //return number of items written.
        size_t write (const T* ptr_data, size_t n) {
            const size_t items2write = std::min(n, N - _used);//items to write
            if (items2write == 0) return 0;

            if (items2write <= N - _end_offset) {//one step
                memcpy (_data + _end_offset, ptr_data, items2write*sizeof(T));
                _end_offset += items2write;
                if (_end_offset == N) _end_offset = 0;
            }
            else {//2 steps
                const size_t size_1{ N - _end_offset };
                const size_t size_2{ items2write - size_1 };
                memcpy (_data + _end_offset, ptr_data, size_1*sizeof(T));
                memcpy (_data, ptr_data + size_1 * sizeof(T), size_2*sizeof(T));
                _end_offset = size_2;
            }

            _used += items2write;
            return items2write;
        }

        //read min(n, _used) items
        //return number of items read
        size_t read (T* ptr_data, size_t n) {
            const size_t items2read = std::min(n, _used);//items to read
            if (items2read == 0) return 0;

            if (items2read <= N - _begin_offset) {//one step
                memcpy (ptr_data, _data + _begin_offset, items2read*sizeof(T));
                if (DEBUG)
                    memset (_data + _begin_offset, 0x00, items2read*sizeof(T));
                if (_begin_offset += items2read; _begin_offset == N) _begin_offset = 0;
            }
            else {//2 steps
                const size_t size_1{ N - _begin_offset };
                const size_t size_2{ items2read - size_1 };
                memcpy (ptr_data, _data + _begin_offset, size_1*sizeof(T));
                memcpy (ptr_data+size_1*sizeof(T), _data, size_2*sizeof(T));
                if (DEBUG) {
                    memset(_data + _begin_offset, 0x00, size_1 * sizeof(T));
                    memset(_data, 0x00, size_2 * sizeof(T));
                }
                _begin_offset = size_2;
            }

            _used -= items2read;
            return items2read;
        }
    };
}
#endif

#ifndef RINGBUFFER
#define RINGBUFFER

#include <algorithm> // for std::min
#include <string.h>

//generic circular buffer implementation
//I will use this with T as uint8_t type

namespace buffers {
    template <class T, size_t N=1000, bool DEBUG=false>
    struct RingBuffer {
    protected:
        size_t _begin_offset, _end_offset;//begin and end offsets
        size_t _used;//used entries
        T _data[N];

    public:
        RingBuffer() :
            _begin_offset(0), _end_offset(0), _used(0) {
            if (DEBUG)
                memset (_data, 0x00, sizeof(T)*N);
        }

        size_t used () const { return _used; }
        size_t size () const { return N; }
        size_t unused () const { return N - _used; }

        //writes min(len, _capacity-_used) items
        //return number of items written.
        size_t write (const T* ptr_data, size_t len) {
            const size_t n = std::min(len, N - _used);//items to write
            if (n == 0) return 0;

            if (n <= N - _end_offset) {//one step write
                memcpy (_data + _end_offset, ptr_data, n*sizeof(T));
                _end_offset += n;
                if (_end_offset == N) _end_offset = 0;
            }
            else {//2 steps write
                const size_t len1{ N - _end_offset };
                const size_t len2{ n - len1 };
                memcpy (_data + _end_offset, ptr_data, len1*sizeof(T));
                memcpy (_data, ptr_data + len1 * sizeof(T), len2*sizeof(T));
                _end_offset = len2;
            }
            _used += n;
            return n;
        }

        //reads min(len, _used) items
        //return number of items read
        size_t read (T* ptr_data, size_t len) {
            const size_t n = std::min(len, _used);//items to read
            if (n == 0) return 0;

            if (n <= N - _begin_offset) {//one step read
                memcpy (ptr_data, _data + _begin_offset, n*sizeof(T));
                if (DEBUG)
                    memset (_data + _begin_offset, 0x00, n*sizeof(T));
                if (_begin_offset += n; _begin_offset == N) _begin_offset = 0;
            }
            else {//2 steps read
                const size_t size_1{ N - _begin_offset };
                const size_t size_2{ n - size_1 };
                memcpy (ptr_data, _data + _begin_offset, size_1*sizeof(T));
                memcpy (ptr_data+size_1*sizeof(T), _data, size_2*sizeof(T));
                if (DEBUG) {
                    memset (_data + _begin_offset, 0x00, size_1 * sizeof(T));
                    memset (_data, 0x00, size_2 * sizeof(T));
                }
                _begin_offset = size_2;
            }
            _used -= n;
            return n;
        }
    };
}
#endif

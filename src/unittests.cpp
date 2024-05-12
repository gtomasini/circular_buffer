#include "RingBuffer.h"
#include <cassert>
#include <cstdint>
#include <string.h>
#include <iostream>


bool comp_mem(uint8_t *p1, uint8_t *p2, size_t n){
    return !memcmp(p1, p2, n);
}
//this is for kind of doing unit tests but I don't have time, sorry    
void someTests(){
    std::cout << "some utests.." << std::endl;
    uint8_t myData[100], x[100];
    for (int i = 0; i < 100; i++)  myData[i] = i+'A';
    memset (x, 0x00, sizeof(x)/(sizeof(x[0])));

    //just for debug
    buffers::RingBuffer<uint8_t, 30> ringbuf;
    assert(ringbuf.getTotalSize() == 30);

    auto n=ringbuf.write(myData, 10);
    assert(n==10);
    assert(ringbuf.getFreeSlots() == 20);
    assert(ringbuf.getUsedSlots() == 10);
    n=ringbuf.read(x, 100);
    assert(n==10);
    assert(comp_mem(myData, x, 10));

    ringbuf.write(myData, 10);
    n=ringbuf.write(&myData[10], 10);
    assert(n==10);
    assert(ringbuf.getFreeSlots() == 10);
    assert(ringbuf.getUsedSlots() == 20);
    n=ringbuf.read(x, 100);
    assert(n==20);
    assert(comp_mem(myData, x, 20));

    ringbuf.write(myData, 20);

    n=ringbuf.write(&myData[20], 5);
    assert(n==5);
    assert(ringbuf.getFreeSlots() == 5);
    assert(ringbuf.getUsedSlots() == 25);
    //assert(comp_mem(myData, x, 25));

    n=ringbuf.write(&myData[25], 10);
    assert(n==0);
    assert(ringbuf.getFreeSlots() == 5);
    assert(ringbuf.getUsedSlots() == 25);

    n=ringbuf.read(x, 100);
    assert(n==25);
    assert(ringbuf.getFreeSlots() == 30);
    assert(ringbuf.getUsedSlots() == 0);

    n=ringbuf.write(&myData[25], 10);
    assert(n==10);
    assert(ringbuf.getFreeSlots() == 20);
    assert(ringbuf.getUsedSlots() == 10);

    n=ringbuf.read(x, 10);
    assert(n==10);
    assert(ringbuf.getFreeSlots() == 30);
    assert(ringbuf.getUsedSlots() == 0);

    n=ringbuf.read(x, 5);
    assert(n==0);
    assert(ringbuf.getFreeSlots() == 30);
    assert(ringbuf.getUsedSlots() == 0);

    std::cout << "end of uts..." << std::endl;
}
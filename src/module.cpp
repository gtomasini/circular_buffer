#include <iostream>
#include <thread>
#include <chrono>  
#include <iterator>
#include <cassert>
#include "IModule.h"

using namespace Modules;

//#undef DEBUG
#define DEBUG

static const auto MaxBuffers2Sent = 
#ifdef DEBUG
//just for debug
20;
#else
100;
#endif

//here we save the final arrays from thread 3 (key: usecs since epoch)
static mapType finalTimeVecMap;

//in order to export data to main
const mapType *Modules::getfinalTimeVecMap() {
	return &finalTimeVecMap;
}

void Module1:: doJob() const {
	std::cout << std::this_thread::get_id();
	std::cout << " "<<__PRETTY_FUNCTION__ << std::endl;
	assert(_buf_1_to_2_ptr != nullptr);

#ifndef DEBUG
	uint8_t outBuff[ArrayLenMax];
#else
	//just for debug
	uint8_t outBuff[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
						17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
#endif

	unsigned totalLen = 0;
	for (auto i=0; i< MaxBuffers2Sent; i++){
#ifndef DEBUG
		memset (outBuff, 0x00, ArrayLenMax);
		const auto len = rand() % ArrayLenMax;//array len 0-99
		if (len==0) continue;
		// fill  the array with random values
		for (auto offset = 0; offset< len; ++offset)
			outBuff[offset] = rand() % ArrayValMax;//0-254
#else
		//just for debug
		const auto len = sizeof(outBuff)/sizeof(outBuff[0]);
#endif
		std::cout << "mod1, writes " << std::dec<<(unsigned)len << " bytes\n";
		uint8_t st(0);
		do {
			st = _buf_1_to_2_ptr->write(outBuff, len);
		} while (st == 0);
		totalLen += len;
	}
	std::cout << "mod1, total writen bytes: " << std::dec<<totalLen << std::endl;
	std::cout << "mod1, ends...\n";
}

bool Module2::processInBuf (const uint8_t* buf, uint8_t len) const {
	for (auto offset=0; offset < (len-_matchLen); offset++)
		if (!memcmp (buf + offset, _matchPtr, _matchLen))
			return true;
	return false;
}

void Module2::doJob() const {
	std::cout << std::this_thread::get_id();
	std::cout << " "<<__PRETTY_FUNCTION__ << std::endl;
	assert(_buf_1_to_2_ptr != nullptr);
	assert(_buf_2_to_3_ptr != nullptr);
	uint8_t inBuff[ArrayLenMax];
	uint8_t n = 0;//len of read buffer
	unsigned totalLen = 0;
	do {
		memset (inBuff, 0x00, ArrayLenMax);//just for debug
		n = _buf_1_to_2_ptr->read (inBuff, ArrayLenMax);
		std::cout << "mod2, reads " << std::dec<<(unsigned)n << " bytes, ";
		auto st=processInBuf (inBuff, n);
		if (st) {
#ifdef DEBUG
			std::cout << " matchs pattern, resend to thr3!\n";
#endif
			uint8_t st(0);
			do {
				st = _buf_2_to_3_ptr->write(inBuff, n);
			} while (st == 0);

		}
		else
#ifdef DEBUG
			std::cout << "no matchs.\n";
#endif
		totalLen += n;
	} while (n > 0);
	std::cout << "mod2, total read bytes: " << std::dec<<totalLen << std::endl;
	std::cout << "mod2, ends...\n";
}

//auxiliar
static const std::string getGMtime() {
#if defined(__GNUC__)	
	time_t t = time(NULL);
	struct tm ptm;
	char str[100];
	gmtime_r(&t, &ptm);
	snprintf(str, sizeof(str), "%2d-%02d-%02d %2d:%02d:%02d", 
		ptm.tm_year - 100, ptm.tm_mon + 1, ptm.tm_mday,
		ptm.tm_hour, ptm.tm_min, ptm.tm_sec);
	return str;
#else
	//TODO: implement this (compilator is not gnu here)
	return "hora";
#endif
}

//auxiliar functions *****************************************
static inline uint64_t get_msecsSinceEpoch() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

static inline uint64_t get_usecsSinceEpoch() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

template<typename T>  
void print_container(std::ostream& os, const T& container, const std::string& delimiter)  {  
    std::copy(std::begin(container),   
              std::end(container),   
              std::ostream_iterator<typename T::value_type>(os, delimiter.c_str())); 
}  


static void printVec (const std::vector<uint8_t>& vec) {
	std::cout << getGMtime() << ":";
	for (const auto& x : vec) {
		std::cout<<" "<<std::hex << (unsigned)x;
	}
	std::cout << std::endl;
}
//end auxiliar

void Module3::doJob() const {
	std::cout << std::this_thread::get_id()
			<< " "<<__PRETTY_FUNCTION__ << std::endl;
	uint8_t inBuff[ArrayLenMax];
	assert (_buf_2_to_3_ptr != nullptr);
	uint8_t n = 0;//len of read buffer
	unsigned totalLen = 0;
	do {
		memset(inBuff, 0x00, ArrayLenMax);//just for debug
		n = _buf_2_to_3_ptr->read(inBuff, ArrayLenMax);
		std::cout << "mod3, reads " << std::dec<<(unsigned)n << " bytes\n";
		if (n > 0){
			const std::vector<uint8_t> finalArr (inBuff, inBuff + n);
			//record array vector in a map (key: usecs from epoch)
			//Warning: if the computer is enough fast entry key could be override
			const auto usecsSinceEpoch = get_usecsSinceEpoch();
			if (finalTimeVecMap.count(usecsSinceEpoch))
				std::cerr<<"used entry map, ERROR!!!"<<std::endl;
			finalTimeVecMap[usecsSinceEpoch] = finalArr;
			printVec (finalArr);
		}
		totalLen += n;
	} while (n > 0);
	std::cout << "mod3, total read bytes: " << std::dec<<totalLen << std::endl;
	std::cout << "mod3, ends...\n";
}
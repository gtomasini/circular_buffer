#include <iostream>
#include <thread>
#include <chrono>  
#include "IModule.h"

using namespace Modules;

#undef DEBUG
const auto MaxBuffersSent = 
#ifdef DEBUG
10;
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
	assert(_wrapBuf_1_to_2_Ptr != nullptr);

#ifndef DEBUG
	uint8_t outBuff[ArrayLenMax];
#else
	uint8_t outBuff[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
						17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
#endif

	unsigned totalLen = 0;
	for (auto i=0; i< MaxBuffersSent; i++){
#ifndef DEBUG
		memset (outBuff, 0x00, ArrayLenMax);//just for debug
		const auto n = rand() % ArrayLenMax;//array len 0-99
		for (auto ii = 0; ii < n; ++ii)
			outBuff[ii] = rand() % ArrayValMax;//0-254
#else
		const auto n = sizeof(outBuff)/sizeof(outBuff[0]);
		std::cout << "mod1, writes " << (unsigned)n << " bytes\n";
#endif
		uint8_t st(0);
		do {
			st = _wrapBuf_1_to_2_Ptr->write(outBuff, n);
		} while (st == 0);
		totalLen += n;
	}
	std::cout << "mod1, total writen bytes: " << totalLen << std::endl;
	std::cout << __PRETTY_FUNCTION__<<" ends...\n";
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
	assert(_wrapBuf_1_to_2_Ptr != nullptr);
	assert(_wrapBuf_2_to_3_Ptr != nullptr);
	uint8_t inBuff[ArrayLenMax];
	uint8_t n = 0;//len of read buffer
	unsigned totalLen = 0;
	do {
		memset (inBuff, 0x00, ArrayLenMax);//just for debug
		n = _wrapBuf_1_to_2_Ptr->read (inBuff);
#ifdef DEBUG
		std::cout << "mod2, reads " << (unsigned)n << " bytes, ";
#endif
		auto st=processInBuf (inBuff, n);
		if (st) {
#ifdef DEBUG
			std::cout << "matchs pattern, resend to thr3!\n";
#endif
			uint8_t st(0);
			do {
				st = _wrapBuf_2_to_3_Ptr->write(inBuff, n);
			} while (st == 0);

		}
		else
#ifdef DEBUG
			std::cout << "no matchs.\n";
#endif
		totalLen += n;
	} while (n > 0);
	std::cout << "mod2, total read bytes: " << totalLen << std::endl;
	std::cout << __PRETTY_FUNCTION__ << " ends...\n";
}

//auxiliar
const std::string getGMtime() {
#if defined(__GNUC__)	
	time_t t = time(NULL);
	struct tm ptm;
	char str[26];
	gmtime_r(&t, &ptm);
	snprintf(str, sizeof(str), "%2d-%02d-%02d %2d:%02d:%02d", 
		ptm.tm_year - 100, ptm.tm_mon + 1, ptm.tm_mday,
		ptm.tm_hour, ptm.tm_min, ptm.tm_sec);
	return std::string(str);
#else
	//TODO: implement this
	return "hora";
#endif
}

//auxiliar
inline uint64_t get_msecsSinceEpoch() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

inline uint64_t get_usecsSinceEpoch() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void printVec (const std::vector<uint8_t>& vec) {
	std::cout << getGMtime() << ":";
	for (const auto& x : vec) {
		std::cout<<" "<<std::hex << (unsigned)x;
	}
	std::cout << std::endl;
}

void Module3::doJob() const {
	std::cout << std::this_thread::get_id();
	std::cout << " "<<__PRETTY_FUNCTION__ << std::endl;
	uint8_t inBuff[ArrayLenMax];
	assert (_wrapBuf_2_to_3_Ptr != nullptr);
	uint8_t n = 0;//len of read buffer
	unsigned totalLen = 0;
	do {
		memset(inBuff, 0x00, ArrayLenMax);//just for debug
		n = _wrapBuf_2_to_3_Ptr->read(inBuff);
#ifdef DEBUG
		std::cout << "mod3, reads " << (unsigned)n << " bytes\n";
#endif
		if (n > 0){
			const std::vector<uint8_t> finalArr (inBuff, inBuff + n);
			//record array vector in a map (key: usecs from epoch)
			//Warning: if the computer is enough fast entry key could be override
			finalTimeVecMap[get_usecsSinceEpoch()] = finalArr;
			printVec (finalArr);
		}
		totalLen += n;
	} while (n > 0);
	std::cout << "mod3, total read bytes: " << totalLen << std::endl;
	
	std::cout << __PRETTY_FUNCTION__ << " ends...\n";
}
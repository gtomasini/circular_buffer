#include <iterator>
#include "Module3.h"

using namespace modules;

//here we save the final arrays from thread 3 (key: usecs since epoch)
static mapType finalTimeVecMap;

//in order to export data to main
const mapType *modules::getfinalTimeVecMap() {
	return &finalTimeVecMap;
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

template<typename T>
static void print_list (const T& list) {
	std::cout << getGMtime() << ":";
	for (const auto& x : list) {
		std::cout<<" "<<std::hex << (unsigned)x;
	}
	std::cout << std::endl;
}
//end auxiliar

void Module3::doMyJob() {
    for(;!_running;)std::this_thread::sleep_for(std::chrono::seconds(10));

	std::cout << std::this_thread::get_id()
			<< " "<<__PRETTY_FUNCTION__ << std::endl;
    for(;_running;){
        uint8_t inBuff[ArrayLenMax];

        uint8_t n = 0;//len of read buffer
        unsigned totalLen = 0;
        do {
            memset(inBuff, 0x00, ArrayLenMax);//just for debug
            n = _buf.read(inBuff, ArrayLenMax);
            std::cout << "mod3, reads " << std::dec<<(unsigned)n << " bytes\n";
            if (n > 0){
                const std::vector<uint8_t> finalArr (inBuff, inBuff + n);
                //record array vector in a map (key: usecs from epoch)
                //Warning: if the computer is enough fast entry key could be override
                const auto usecsSinceEpoch = get_usecsSinceEpoch();
                if (finalTimeVecMap.count(usecsSinceEpoch))
                    std::cerr<<"used entry map, ERROR!!!"<<std::endl;
                finalTimeVecMap[usecsSinceEpoch] = finalArr;
                print_list (finalArr);
            }
            totalLen += n;
        } while (n > 0);
        std::cout << "mod3, total read bytes: " << std::dec<<totalLen << std::endl;
    }
	std::cout << "mod3, ends...\n";
}
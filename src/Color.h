#pragma once
#include <iostream>

#define __PRETTY__   Clr::pretty(__PRETTY_FUNCTION__);

struct Clr{
   static constexpr const char* bk="\033[0;30m";
   static constexpr const char* def="\033[0m";
   static constexpr const char* yl="\033[0;33m";
   static constexpr const char* gr="\033[0;32m";
   static constexpr const char* cy=" \033[0;36m";
   static constexpr const char* wh="\033[0;37m";
   static constexpr const char* bl=" \033[0;34m";
   static constexpr const char* re="\033[91m";
   static constexpr const char* og="\033[93m";
   static constexpr const char* ma="\033[95m";

   static bool _print;
   static inline void pretty(const char *s) {
      if (_print)
         std::cout << Clr::yl << "\n**** "<<s<< " ****" << Clr::def<<std::endl;
   }
};
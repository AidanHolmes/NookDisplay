#ifndef __UTILITY_FUNCS
#define __UTILITY_FUNCS

#include <string>
#include <wchar.h>

std::wstring utf8_to_wide(const std::string &str);
const wchar_t* getmonth(int m) ;
long get_battery() ;

#endif

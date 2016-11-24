#ifndef __UTILITY_FUNCS
#define __UTILITY_FUNCS

#include <string>
#include <vector>
#include <wchar.h>

std::vector<std::wstring> get_inet_address();
std::wstring utf8_to_wide(const std::string &str);
const wchar_t* getmonth(int m) ;
long get_battery() ;

#endif

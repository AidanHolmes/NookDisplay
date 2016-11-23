#include "utility.hpp"
#include <locale.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

long get_battery()
{
  char *strp = NULL;
  const int size = 50 ;
  char szbuffer[size] ;
  int fd = open("/sys/class/power_supply/bq27510-0/capacity", O_RDONLY) ;
  if (fd < 0) return 0.0 ;
  
  int ret = read(fd, szbuffer, size) ;
  if (ret < 0) return 0.0 ;

  szbuffer[ret] = '\0' ;

  close(fd) ;

  return strtol(szbuffer, &strp,10) ;
}

std::wstring utf8_to_wide(const std::string &str)
{
  char *curloc = setlocale(LC_CTYPE, NULL);
  std::wstring out ;

  if (!setlocale(LC_CTYPE, "en_GB.UTF-8")){
    std::cerr << "Cannot change locale\n" ;
  }

  size_t len = mbstowcs(NULL, str.c_str(), 0);
  if (len < 0){
    std::cerr << "Cannot convert multibyte to wide characters\n" ;
    setlocale(LC_CTYPE, curloc) ; // Reset locale
    return out ;
  }

  wchar_t *wout = new wchar_t[len+2] ;
  mbstowcs(wout, str.c_str(), len+1) ;
  out = wout ;
  delete[] wout ;

  setlocale(LC_CTYPE, curloc) ; // Reset locale
  return out ;
}

const wchar_t* getmonth(int m)
{
  switch(m){
  case 0: return L"Jan" ;
  case 1: return L"Feb" ;
  case 2: return L"Mar" ;
  case 3: return L"Apr" ;
  case 4: return L"May" ;
  case 5: return L"Jun" ;
  case 6: return L"Jul" ;
  case 7: return L"Aug" ;
  case 8: return L"Sep" ;
  case 9: return L"Oct" ;
  case 10: return L"Nov" ;
  case 11: return L"Dec" ;
  };
  return L"???" ;
}


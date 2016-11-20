#include "json.hpp"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define P(x,y) x.push_back(y);
#define SET_PATH2(x,a,b) P(x,a) P(x,b)
#define SET_PATH3(x,a,b,c) P(x,a) P(x,b) P(x,c)
#define SET_PATH4(x,a,b,c,d) P(x,a) P(x,b) P(x,c) P(x,d)
#define SET_PATH5(x,a,b,c,d,e) P(x,a) P(x,b) P(x,c) P(x,d) P(x,e)
#define SET_PATH6(x,a,b,c,d,e,f) P(x,a) P(x,b) P(x,c) P(x,d) P(x,e) P(x,f)

int main(int argc, char **argv)
{
  FILE *f = NULL ;

  f=fopen("cache", "r") ;
  if (!f){
    std::cerr << "Cannot open cache\n" ;
    return 1;
  }

  fseek(f, 0, SEEK_END) ;
  long len = ftell(f) ;
  char *szBuff = new char[len+1] ;
  fseek(f,0,SEEK_SET);
  if (fread(szBuff, 1, len, f) != len){
    std::cerr << "Couldn't read all of stream\n" ;
    return 1 ;
  }
  szBuff[len] = '\0' ;

  JSONData json ;
  
  json.load(szBuff) ;
  std::vector<std::string> location, unixdt, temp, tmp_min, tmp_max, weatherid, weatherdesc, weathericon, clouds;
  SET_PATH2(location, "city", "name") ;
  SET_PATH3(unixdt, "list", "0", "dt") ;
  SET_PATH4(tmp_min, "list", "0", "main", "temp_min") ;
  SET_PATH4(tmp_max, "list", "0", "main", "temp_max") ;
  SET_PATH4(temp, "list", "0", "main", "temp") ;
  SET_PATH5(weatherid, "list", "0", "weather", "0", "id") ;
  SET_PATH5(weatherdesc, "list", "0", "weather", "0", "description") ;
  SET_PATH5(weathericon, "list", "0", "weather", "0", "icon") ;
  SET_PATH4(clouds, "list", "0", "clouds", "all") ;

  char *strtolp = NULL ;
  long weathertime = 0 ;
  struct tm *timeinfo ;
  char szIndex[5] ;

  for (int i=0; i < 5; i++){
    snprintf(szIndex, 4, "%d",i) ;

    unixdt[1] = szIndex ;
    tmp_min[1] = szIndex ;
    tmp_max[1] = szIndex ;
    weatherid[1] = szIndex ;
    weatherdesc[1] = szIndex ;
    weathericon[1] = szIndex ;
    clouds[1] = szIndex ;

    weathertime = strtol(json.read(unixdt).c_str(), &strtolp,10) ;
    timeinfo = localtime(&weathertime) ;

    std::cout << "Weather - " << json.read(location) 
	      << " at: " << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec
	      << ". Temperature: " << json.read(tmp_max) << " to " << json.read(tmp_min)
	      << ". Weather - " << json.read(weatherdesc) << ". ID : " << json.read(weatherid) << ", icon: " << json.read(weathericon)
	      << ". Cloud percentage " << json.read(clouds) << "%\n" ;
  }

  delete[] szBuff ;

  return 0 ;
}

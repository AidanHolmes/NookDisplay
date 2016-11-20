#include "json.hpp"
#include <string>
#include <iostream>
#include <stdio.h>

int main(int argc, char **argv)
{
  FILE *f = NULL ;

  f=fopen("cache", "r") ;
  if (!f){
    std::cerr << "Cannot open cache\n" ;
    return 1;
  }

  std::string data, out ;

  fseek(f, 0, SEEK_END) ;
  long len = ftell(f) ;
  char *szBuff = new char[len+1] ;
  fseek(f,0,SEEK_SET);
  if (fread(szBuff, 1, len, f) != len){
    std::cerr << "Couldn't read all of stream\n" ;
    return 1 ;
  }
  szBuff[len] = '\0' ;
  data = szBuff ;
  delete[] szBuff ;

  JSONData json ;
  
  json.load(data) ;
  std::vector<std::string> path;

  path.push_back("list") ;
  path.push_back("0") ;
  path.push_back("dt") ;
  
  out = json.read(path) ;

  std::cout << "Result: " << out << std::endl;

  return 0 ;
}

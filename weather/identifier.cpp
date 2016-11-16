#include "identifier.hpp"
#include <string.h>

Identifier::Identifier(const char *szIdentifier)
{
  findme = NULL;
  p = NULL ;
  count = 0 ;
  
  if (szIdentifier){
    int len = strlen(szIdentifier) ;
    if (len >= FIXEDBUFF){
      findme = new char[len+1] ;
    }else{
      findme = fixed ;
    }
    strcpy(findme, szIdentifier) ;
    p = findme ;
  }
}

Identifier::~Identifier()
{
  if (findme && findme != fixed){
    delete[] findme ;
  }
}

bool Identifier::add(char c)
{
  if (*p == c) p++ ;
  else p=findme ;
  if (*p == '\0'){ 
    p = findme ;
    count++ ;
    return true ;
  }

  return false ;
}

void Identifier::reset()
{
  if (findme){
    p = findme ;
  }
  count = 0;
}


#ifndef __IDENTIFIER_HPP
#define __IDENTIFIER_HPP

#define FIXEDBUFF 200

class Identifier{
public:
  Identifier(const char *szIdentifier);
  ~Identifier() ;
  bool add(char c) ;
  void reset() ;
  unsigned int get_count(){return count;} ;
protected:
  char *findme ;
  char *p ;
  char fixed[FIXEDBUFF] ;
  unsigned int count ;
};



#endif

#ifndef __TESTNOOKWND_HPP
#define __TESTNOOKWND_HPP

#include "nookwindow.hpp"
#include <time.h>

class TestNookWnd: public NookWindow{
public:
  TestNookWnd() ;
  bool draw() ;
  bool tick() ;

private:
  unsigned int m_x ;
  unsigned int m_y ;
  time_t m_last;
  time_t m_full ;
} ;



#endif

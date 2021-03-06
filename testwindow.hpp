#ifndef __TESTNOOKWND_HPP
#define __TESTNOOKWND_HPP

#include "nookwindow.hpp"
#include "nookinput.hpp"
#include <time.h>
#include "nookfont.hpp"
#include "nookbutton.hpp"
#include "nookmessagebox.hpp"

class WeatherIcon: public NookWindow{
public:
  virtual bool draw() ;
  virtual void initialise() ;
};

class TestNookWnd: public NookWindow{
public:
  TestNookWnd() ;
  virtual bool draw() ;
  virtual bool tick() ;
  virtual void key_event(KeyEvent &keys) ;
  virtual void touch_event(TouchEvent &touch, unsigned int x_offset, unsigned int y_offset);
  virtual void initialise() ;

private:
  unsigned int m_x ;
  unsigned int m_y ;
  time_t m_last;
  time_t m_full ;

  WeatherIcon iconwnd ;
  NookFont m_fnt ;
  NookButton m_btn ;
  NookMsgBox m_msgbox ;
} ;



#endif

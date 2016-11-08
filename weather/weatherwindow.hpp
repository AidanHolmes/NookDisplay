#ifndef __WEATHERWND_HPP
#define __WEATHERWND_HPP

#include "nookwindow.hpp"
#include "nookinput.hpp"
#include <time.h>
#include "nookfont.hpp"
#include <string>

class WeatherIcon: public NookWindow{
public:
  virtual bool draw() ;
  virtual void initialise() ;
};

class WeatherWnd: public NookWindow{
public:
  WeatherWnd() ;
  virtual bool draw() ;
  virtual bool tick() ;
  virtual void key_event(KeyEvent &keys) ;
  virtual void touch_event(TouchEvent &touch) ;
  virtual void initialise() ;

protected:
  void loadweather();

private:
  time_t m_last;
  time_t m_full ;

  WeatherIcon iconwnd ;
  NookFont m_fnt ;
  std::string m_day1, m_day2, m_day3 ;
} ;



#endif

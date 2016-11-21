#ifndef __WEATHERWND_HPP
#define __WEATHERWND_HPP

#include "nookwindow.hpp"
#include "nookinput.hpp"
#include <time.h>
#include "nookfont.hpp"
#include <string>
#include "summarywindow.hpp"

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

  WeatherSummaryWnd m_wnd1 ;
  WeatherSummaryWnd m_wnd2 ;
  WeatherSummaryWnd m_wnd3 ;
  WeatherSummaryWnd m_wnd4 ;

  NookFont m_fnt ;
  std::wstring m_day1, m_day2, m_day3 ;
} ;



#endif

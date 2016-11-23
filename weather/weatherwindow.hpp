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
  virtual void touch_event(TouchEvent &touch,
			   unsigned int x_offset, 
			   unsigned int y_offset);
  virtual void initialise() ;
  
protected:
  void loadweather();
  void load_background() ;

private:
  time_t m_last;
  time_t m_full ;

  long m_time ;
  float m_temp ;
  long m_code ;
  std::wstring m_desc ;
  std::wstring m_location ;

  WeatherSummaryWnd m_wnd[4] ;
  DisplayImage m_background ;

  NookFont m_fnt ;
} ;



#endif

#ifndef __SUMMARYWND_HPP
#define __SUMMARYWND_HPP

#include "nookwindow.hpp"
#include "nookfont.hpp"

class WeatherSummaryWnd: public NookWindow{
public:
  WeatherSummaryWnd() ;
  virtual bool draw();
  virtual void initialise();

  void set_code(int code) ;
  void set_temperature(int temp) ;
  void set_time(long t) ;

protected:
  int m_code ;
  NookFont m_fnt ;
  long m_time ;
  long m_temp;
  DisplayImage m_icon ;
};


#endif

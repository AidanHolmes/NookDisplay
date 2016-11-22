#ifndef __SUMMARYWND_HPP
#define __SUMMARYWND_HPP

#include "nookwindow.hpp"
#include "nookfont.hpp"
#include <string>

class WeatherSummaryWnd: public NookWindow{
public:
  WeatherSummaryWnd() ;
  virtual bool draw();
  virtual void initialise();

  void set_code(int code) ;
  void set_temperature(float temp) ;
  void set_time(long t) ;
  void set_description(std::wstring str) ;

protected:
  void load_icon();

protected:
  int m_code ;
  NookFont m_fnt ;
  long m_time ;
  float m_temp;
  DisplayImage m_icon ;
  std::wstring m_desc ;
};


#endif

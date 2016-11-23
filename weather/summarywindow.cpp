#include "summarywindow.hpp"
#include <iostream>
#include <stdio.h>
#include <wchar.h>
#include "utility.hpp"

WeatherSummaryWnd::WeatherSummaryWnd()
{
  m_code = 100;
  m_temp= 0.0 ;
  m_time = 0 ;
}

void WeatherSummaryWnd::load_icon()
{
  const char *szfile ; 
  if (m_code >=200 && m_code <= 232){
    szfile = "gfx/thunder.jpg" ;
  }else if (m_code >= 300 && m_code <= 321){
    szfile = "gfx/sunrain.jpg" ;
  }else if (m_code >= 500 && m_code <= 531){
    szfile = "gfx/rain.jpg" ;
  }else if (m_code == 600){
    szfile = "gfx/sunsnow.jpg" ;
  }else if (m_code >= 601 && m_code <= 622){
    szfile = "gfx/snow.jpg" ;
  }else if (m_code >= 701 && m_code <= 781){
    szfile = "gfx/heavycloud.jpg" ;
  }else if (m_code == 800 || m_code == 904 || m_code == 905){
    szfile = "gfx/clear.jpg" ;
  }else if (m_code >= 900 && m_code <= 902){
    szfile = "gfx/thunder.jpg" ;
  }else if (m_code == 903 || m_code == 906){
    szfile = "gfx/freezingrain.jpg" ;
  }else{
    szfile = "gfx/clear.jpg" ;
  }
  
  m_icon.loadJPG(szfile, 8) ;
}

void WeatherSummaryWnd::set_description(std::wstring str)
{
  m_desc = str ;
}

bool WeatherSummaryWnd::draw()
{
  wchar_t szNumber[15] ;
  swprintf(szNumber, 14, L"%.1f\u00B0C", m_temp) ;

  struct tm *ti ;
  ti = localtime(&m_time) ;
  wchar_t szTime[15] ;
  swprintf(szTime, 14, L"%02d:%02d %02d-%ls", ti->tm_hour, ti->tm_min, ti->tm_mday, getmonth(ti->tm_mon)) ;

  canvas.setBGGrey(255) ;
  canvas.setFGGrey(0) ;
  canvas.eraseBackground() ;

  canvas.copy(m_icon,0,0,0);
 
  canvas.drawRect(0,0, m_width-1, m_height-1) ;

  unsigned int pen_y = 5, pen_x = 5;

  DisplayImage txttemp ;
  txttemp = m_fnt.write_string(szTime, 8) ; 
  canvas.copy(txttemp, 2, pen_x, pen_y) ;
  pen_y += txttemp.get_height() + 10;

  txttemp = m_fnt.write_string(szNumber, 8) ; 
  canvas.copy(txttemp, 2, pen_x, pen_y) ;

  txttemp = m_fnt.write_string(m_desc, 8) ;
  canvas.copy(txttemp, 2, pen_x, m_height - txttemp.get_height() -1) ;

  return true ;
}

void WeatherSummaryWnd::initialise()
{
  m_fnt.load_font("/usr/share/fonts/ttf/LiberationSans-Regular.ttf") ;
  load_icon() ;
}

void WeatherSummaryWnd::set_code(int code)
{
  m_code = code ;
  load_icon() ;
}

void WeatherSummaryWnd::set_temperature(float temp)
{
  m_temp = temp ;
}

void WeatherSummaryWnd::set_time(long t)
{
  m_time = t ;
}




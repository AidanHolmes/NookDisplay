#include "summarywindow.hpp"
#include <stdio.h>
#include <wchar.h>

WeatherSummaryWnd::WeatherSummaryWnd()
{
  m_code = 100;
  m_temp= 0 ;
  m_time = 0 ;
}

bool WeatherSummaryWnd::draw()
{
  wchar_t szNumber[15] ;
  swprintf(szNumber, 14, L"%d", m_temp) ;

  canvas.setBGGrey(255) ;
  canvas.setFGGrey(0) ;
  canvas.eraseBackground() ;
  canvas.copy(m_icon,0,0,0);
 
  canvas.drawRect(0,0, m_width-1, m_height-1) ;

  DisplayImage txttemp = m_fnt.write_string(szNumber, 8) ; 
  canvas.copy(txttemp, 2, 5,5) ;

  return true ;
}

void WeatherSummaryWnd::initialise()
{
  m_fnt.load_font("/usr/share/fonts/ttf/LiberationSans-Regular.ttf") ;
  m_icon.loadJPG("gfx/rain.jpg",8) ;
}

void WeatherSummaryWnd::set_code(int code)
{
  m_code = code ;
}

void WeatherSummaryWnd::set_temperature(int temp)
{
  m_temp = temp ;
}

void WeatherSummaryWnd::set_time(long t)
{
  m_time = t ;
}




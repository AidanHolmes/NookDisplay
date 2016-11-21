#include "weatherwindow.hpp"
#include <iostream>
#include <time.h>
#include "nookfont.hpp"
#include "http.hpp"

WeatherWnd::WeatherWnd()
{
  NookWindow() ;
  m_last = 0 ;
  m_full = 0 ;
}

void WeatherWnd::initialise()
{
  const long wndsize=150 ;
  long x=m_width-1 ;
  long y=m_height-wndsize-1 ;

  x -= wndsize ;
  m_wnd4.create(x,y,wndsize,wndsize) ;
  x -= wndsize ;
  m_wnd3.create(x,y,wndsize,wndsize) ;
  x -= wndsize ;
  m_wnd2.create(x,y,wndsize,wndsize) ;
  x -= wndsize ;
  m_wnd1.create(x,y,wndsize,wndsize) ;

  add_window(m_wnd1) ;
  add_window(m_wnd2) ;
  add_window(m_wnd3) ;
  add_window(m_wnd4) ;

  m_fnt.load_font("/usr/share/fonts/ttf/LiberationSans-Regular.ttf") ;

  canvas.loadJPG("gfx/main/snow.jpg",8) ;
}

bool WeatherWnd::draw()
{

  return true ;
}

void WeatherWnd::key_event(KeyEvent &keys)
{
  std::string strState = "up";
  if (keys.key_down) strState = "down" ;
  std::cout << "Key " << strState << ", ID: " << (int)keys.code << std::endl;
}

void WeatherWnd::touch_event(TouchEvent &touch)
{
  std::string strState = "up";
  if (touch.touch_down) strState = "down" ;
  std::cout << "Touch X:" << touch.x << " Y: " << touch.y << " state: " << strState << std::endl;
}

void WeatherWnd::loadweather()
{

}

bool WeatherWnd::tick()
{
  time_t now = time(NULL) ;

  if (now - m_last > 3600){
    loadweather() ;
    
    invalidate_window(true);
    m_last = now ;
  }

  return true ;
}

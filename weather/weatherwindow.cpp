#include "weatherwindow.hpp"
#include <iostream>
#include <time.h>
#include "nookfont.hpp"
#include "bbcweather.hpp"
#include "http.hpp"

bool WeatherIcon::draw()
{
  return true ;
}

void WeatherIcon::initialise()
{
  canvas.loadJPG("weather.jpg",8) ;
  set_window_merge(true) ;
}

WeatherWnd::WeatherWnd()
{
  NookWindow() ;
  m_last = 0 ;
  m_full = 0 ;
}

void WeatherWnd::initialise()
{
  iconwnd.create(50,50,256,256) ;
  add_window(iconwnd) ;
  m_fnt.load_font("/usr/share/fonts/ttf/LiberationSans-Regular.ttf") ;
}

bool WeatherWnd::draw()
{
  canvas.setFGGrey(0) ;
  canvas.setBGGrey(255) ;
  canvas.eraseBackground();

  DisplayImage fntstr = m_fnt.write_string(m_day1, 8) ;
  canvas.copy(fntstr, 2, 10, 320) ;

  canvas.copy(m_fnt.write_string(m_day2, 8), 2, 10, 360) ;
  canvas.copy(m_fnt.write_string(m_day3, 8), 2, 10, 400) ;

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

  iconwnd.set_origin(touch.x, touch.y) ;
}

void WeatherWnd::loadweather()
{
  HTTPConnection con ;
  con.set_urn("/weather/feeds/en/2641170/3dayforecast.rss") ;
  if (con.send_get("open.live.bbc.co.uk", 80)){
    BBCWeather w ;
    if (w.load(con.get_data())){
      m_day1 = w.get_day(0).get_title() ;
      m_day2 = w.get_day(1).get_title() ;
      m_day3 = w.get_day(2).get_title() ;
    }
  }
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

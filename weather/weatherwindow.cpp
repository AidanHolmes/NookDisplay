#include "weatherwindow.hpp"
#include <iostream>
#include <time.h>
#include <vector>
#include <string>
#include "nookfont.hpp"
#include "http.hpp"
#include "json.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <locale.h>

std::wstring utf8_to_wide(const std::string &str)
{
  char *curloc = setlocale(LC_CTYPE, NULL);
  std::wstring out ;

  if (!setlocale(LC_CTYPE, "en_GB.UTF-8")){
    std::cerr << "Cannot change locale\n" ;
  }

  size_t len = mbstowcs(NULL, str.c_str(), 0);
  if (len < 0){
    std::cerr << "Cannot convert multibyte to wide characters\n" ;
    setlocale(LC_CTYPE, curloc) ; // Reset locale
    return out ;
  }

  wchar_t *wout = new wchar_t[len+2] ;
  mbstowcs(wout, str.c_str(), len+1) ;
  out = wout ;
  delete[] wout ;

  setlocale(LC_CTYPE, curloc) ; // Reset locale
  return out ;
}

WeatherWnd::WeatherWnd()
{
  NookWindow() ;
  m_last = 0 ;
  m_full = 0 ;
  m_time = 0;
  m_temp = 0.0;
  m_code = 0;
}

void WeatherWnd::initialise()
{
  const long wndsize=150 ;
  long x=m_width-1 ;
  long y=m_height-wndsize-1 ;

  x -= wndsize ;
  m_wnd[3].create(x,y,wndsize,wndsize) ;
  x -= wndsize ;
  m_wnd[2].create(x,y,wndsize,wndsize) ;
  x -= wndsize ;
  m_wnd[1].create(x,y,wndsize,wndsize) ;
  x -= wndsize ;
  m_wnd[0].create(x,y,wndsize,wndsize) ;

  add_window(m_wnd[0]) ;
  add_window(m_wnd[1]) ;
  add_window(m_wnd[2]) ;
  add_window(m_wnd[3]) ;

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
  JSONData json ;
  char* szbuffer = NULL ;
  int fd = -1, ret = 0, size = 0 ;

  if ((fd=open("cache", O_RDONLY)) < 0){
    std::cerr << "Cannot open cache file\n" ;
    return ;
  }

  size = lseek(fd, 0, SEEK_END);
  if (size < 0){
    std::cerr << "Cannot seek to end of cache file\n" ;
    return ;
  }
  szbuffer = new char[size+1] ;
  if (!szbuffer){
    std::cerr << "Cannot allocate cache buffer\n" ;
    return ;
  }

  lseek(fd, 0, SEEK_SET);

  ret = read(fd, szbuffer, size);
  if (ret < 0){
    std::cerr << "Cannot read cache file\n" ;
    delete[] szbuffer ;
    return ;
  }
  
  szbuffer[size] = '\0' ;

  std::vector<std::string> location, unixdt, temp, tmp_min, tmp_max, weatherid, weatherdesc, weathericon, clouds;
  SET_PATH2(location, "city", "name") ;
  SET_PATH3(unixdt, "list", "0", "dt") ;
  SET_PATH4(tmp_min, "list", "0", "main", "temp_min") ;
  SET_PATH4(tmp_max, "list", "0", "main", "temp_max") ;
  SET_PATH4(temp, "list", "0", "main", "temp") ;
  SET_PATH5(weatherid, "list", "0", "weather", "0", "id") ;
  SET_PATH5(weatherdesc, "list", "0", "weather", "0", "description") ;
  SET_PATH5(weathericon, "list", "0", "weather", "0", "icon") ;
  SET_PATH4(clouds, "list", "0", "clouds", "all") ;

  char *strtop = NULL ;
  long weathertime = 0, code = 0 ;
  float temperature = 0.0 ;
  //struct tm *timeinfo ;
  char szIndex[5] ;
  std::wstring desc ;

  json.load(szbuffer) ;
  for (int i=0; i < 5; i++){
    snprintf(szIndex, 4, "%d",i) ;

    unixdt[1] = szIndex ;
    tmp_min[1] = szIndex ;
    tmp_max[1] = szIndex ;
    weatherid[1] = szIndex ;
    weatherdesc[1] = szIndex ;
    weathericon[1] = szIndex ;
    clouds[1] = szIndex ;

    weathertime = strtol(json.read(unixdt).c_str(), &strtop,10) ;
    code = strtol(json.read(weatherid).c_str(), &strtop,10) ;
    temperature = strtof(json.read(temp).c_str(), &strtop) ;
    desc = utf8_to_wide(json.read(weatherdesc));
    //timeinfo = localtime(&weathertime) ;

    //std::cout << "Time: " << json.read(unixdt) << ", code: " << json.read(weatherid) << ", temperature: " << json.read(temp) << std::endl;
    //std::cout << "Time: " << weathertime << ", code: " << code << ", temperature: " << temperature << std::endl;

    if (i > 0){
      m_wnd[i-1].set_code(code) ;
      m_wnd[i-1].set_temperature(temperature) ;
      m_wnd[i-1].set_time(weathertime) ;
      m_wnd[i-1].set_description(desc) ;
    }else{
      m_time = weathertime ;
      m_temp = temperature ;
      m_code = code ;
      m_desc = desc ;
    }
  }

  delete[] szbuffer ;
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

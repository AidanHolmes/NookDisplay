#include "weatherwindow.hpp"
#include <iostream>
#include <time.h>
#include <vector>
#include <string>
#include "nookfont.hpp"
#include "http.hpp"
#include "json.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "utility.hpp"

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
  load_background();
}

void WeatherWnd::load_background()
{
  const char *szfile ; 
  if (m_code >=200 && m_code <= 232){
    szfile = "gfx/main/thunderstorm.jpg" ;
  }else if (m_code >= 300 && m_code <= 321){
    szfile = "gfx/main/drizzle.jpg" ;
  }else if (m_code >= 500 && m_code <= 531){
    szfile = "gfx/main/rain.jpg" ;
  }else if (m_code >= 600 && m_code <= 622){
    szfile = "gfx/main/snow.jpg" ;
  }else if (m_code >= 701 && m_code <= 781){
    szfile = "gfx/main/fog.jpg" ;
  }else if (m_code == 800 || m_code == 904 || m_code == 905){
    szfile = "gfx/main/clear.jpg" ;
  }else if (m_code >= 900 && m_code <= 902){
    szfile = "gfx/main/extreme.jpg" ;
  }else if (m_code == 906){ // Hail
    szfile = "gfx/main/rain.jpg" ;
  }else if (m_code == 801){ // Few clouds
    szfile = "gfx/main/scatteredcloud.jpg" ;
  }else if (m_code == 802){ // Scattered clouds
    szfile = "gfx/main/scatteredcloud2.jpg" ;
  }else if (m_code == 803){ // Broken clouds
    szfile = "gfx/main/brokencloud.jpg" ;
  }else if (m_code == 804){ // Overcast clouds
    szfile = "gfx/main/overcast.jpg" ;
  }else{
    szfile = "gfx/main/clear.jpg" ;
  }
  
  m_background.loadJPG(szfile, 8) ;
}

bool WeatherWnd::draw()
{
  wchar_t szTemperature[15];
  swprintf(szTemperature, 14, L"%.1f\u00B0C", m_temp);
  struct tm *ti ;
  ti = localtime(&m_time) ;
  wchar_t szTime[15] ;
  swprintf(szTime, 14, L"%02d:%02d %02d-%ls", ti->tm_hour, ti->tm_min, ti->tm_mday, getmonth(ti->tm_mon)) ;

  wchar_t szBatt[15] ;
  swprintf(szBatt, 14, L"Batt: %d%%", get_battery()) ;

  // Set back and foreground pens
  canvas.setBGGrey(255) ;
  canvas.setFGGrey(0) ;

  // Write the background image
  canvas.copy(m_background,0,0,0) ;

  unsigned int pen_y = 5, pen_x = 5 ;

  DisplayImage txt ;
  // Write location
  canvas.setFGGrey(128) ;
  txt = m_fnt.write_string(m_location, 30) ;
  canvas.copy(txt, 8, pen_x, pen_y) ;
  pen_y += txt.get_height() + 10 ;
  pen_x = 20 ;

  canvas.setFGGrey(0) ;
  // Write time
  txt = m_fnt.write_string(szTime, 12) ;
  canvas.copy(txt, 2, pen_x, pen_y) ;
  pen_y += txt.get_height() + 30 ;

  // Write temperature
  txt = m_fnt.write_string(szTemperature, 40) ;
  canvas.copy(txt, 2, pen_x, pen_y) ;
  pen_y += txt.get_height() + 10 ;

  // Write description
  txt = m_fnt.write_string(m_desc, 20) ;
  canvas.copy(txt, 2, pen_x, pen_y) ;
  pen_y += txt.get_height() + 15 ;

  // Write battery
  txt = m_fnt.write_string(szBatt, 10) ;
  canvas.copy(txt, 2, m_width - txt.get_width() - 5, 5) ;

  return true ;
}

void WeatherWnd::key_event(KeyEvent &keys)
{
  // Nook key to refresh display before timeout
  if (keys.code == 102 && !keys.key_down) m_last = 0;

  std::string strState = "up";
  if (keys.key_down) strState = "down" ;
  std::cout << "Key " << strState << ", ID: " << (int)keys.code << std::endl;
}

void WeatherWnd::touch_event(TouchEvent &touch,unsigned int x_offset, unsigned int y_offset)
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
  char szIndex[5] ;
  std::wstring desc, strLocation ;

  json.load(szbuffer) ;
  for (int i=0; i < 5; i++){
    snprintf(szIndex, 4, "%d",i) ;

    unixdt[1] = szIndex ;
    temp[1] = szIndex ;
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
    strLocation = utf8_to_wide(json.read(location));

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
      m_location = strLocation ;
      load_background();
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

#ifndef __OPEN_WEATHER_HPP
#define __OPEN_WEATHER_HPP

#include <string>

std::wstring utf8_to_wide(std::string &str);

class OpenWeather ;

class OpenWeatherDay{
public:
  friend OpenWeather;
  std::wstring get_title(){return utf8_to_wide(m_title);} ;
  std::wstring get_description(){return utf8_to_wide(m_description);} ;
protected:
  std::string m_title ;
  std::string m_description ;
};

class OpenWeather{
public:
  bool load(std::string data);
  OpenWeatherDay& get_day(unsigned int day) ;
  void reset() ;

protected:
  OpenWeatherDay m_5day[5];
};


#endif

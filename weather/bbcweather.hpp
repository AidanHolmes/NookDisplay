#ifndef __BBC_WEATHER_RSS_HPP
#define __BBC_WEATHER_RSS_HPP

#include <string>

std::wstring utf8_to_wide(std::string &str);

class BBCWeather ;

class BBCWeatherDay{
public:
  friend BBCWeather;
  std::wstring get_title(){return utf8_to_wide(m_title);} ;
  std::wstring get_description(){return utf8_to_wide(m_description);} ;
protected:
  std::string m_title ;
  std::string m_description ;
};

class BBCWeather{
public:
  bool load(std::string data);
  BBCWeatherDay& get_day(unsigned int day) ;
  void reset() ;

protected:
  BBCWeatherDay m_3day[3];
};


#endif

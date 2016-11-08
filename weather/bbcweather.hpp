#ifndef __BBC_WEATHER_RSS_HPP
#define __BBC_WEATHER_RSS_HPP

#include <string>

class BBCWeather ;

class BBCWeatherDay{
public:
  friend BBCWeather;
  std::string get_title(){return m_title;} ;
  std::string get_description(){return m_description;} ;
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

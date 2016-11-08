#include "bbcweather.hpp"
#include <string>
#include <iostream>

void BBCWeather::reset()
{
  for (int i=0; i < 3; i++){
    m_3day[i].m_title.clear() ;
    m_3day[i].m_description.clear() ;
  }
}

BBCWeatherDay& BBCWeather::get_day(unsigned int day)
{
  if (day > 2) return m_3day[2] ;
  
  return m_3day[day] ;
}

bool BBCWeather::load(std::string data)
{
  int state = 0 ;
  int day = -1 ;

  reset() ;

  char szItemOpen[] = "<item>", *pio = szItemOpen;
  char szItemClosed[] = "</item>", *pic = szItemClosed;

  char szTitleOpen[] = "<title>", *tio = szTitleOpen;
  char szTitleClosed[] = "</title>", *tic = szTitleClosed;

  char szDescOpen[] = "<description>", *dio = szDescOpen;
  char szDescClosed[] = "</description>", *dic = szDescClosed;

  bool bOpenTag = false ;

  for (std::string::iterator i= data.begin(); i < data.end(); i++){
    // Search for the item tags and then populate the title and descriptions
    if (*i == '<') bOpenTag = true ;

    pio = (*i == *pio)?pio+1:szItemOpen ;
    pic = (*i == *pic)?pic+1:szItemClosed ;
    
    if (*pio == '\0'){ 
      state = 1 ; // reading item
      day++ ; // increment day
      if (day > 2) return true ; // Cannot process more items
    }
    if (*pic == '\0') state = 0 ; // closed item

    if (state >= 1){
      tio = (*i == *tio)?tio+1:szTitleOpen ;
      tic = (*i == *tic)?tic+1:szTitleClosed ;
      dio = (*i == *dio)?dio+1:szDescOpen ;
      dic = (*i == *dic)?dic+1:szDescClosed ;

      if (state == 2 && !bOpenTag) m_3day[day].m_title += *i ;
      if (state == 3 && !bOpenTag) m_3day[day].m_description += *i ;

      if (*tio == '\0') state = 2 ; // read title
      if (*dio == '\0') state = 3 ; // read description
      if (*dic == '\0' || *tic == '\0') state = 1 ; // Closed
    }

    if (*i == '>') bOpenTag = false ;
  } 

  if (day != 2) return false ;
  return true ;
}

#include "http.hpp"
#include "bbcweather.hpp"
#include <vector>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
  HTTPConnection con ;
  //  std::string urn = "/v1/public/yql?q=select%20item.condition%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text%3D%22nottingham%2C%20uk%22)&format=json" ;
  //std::string host = "query.yahooapis.com" ;

  //std::string host = "weather.yahooapis.com" ;
  //std::string urn = "/forecastrss?w=26465977&u=c";

  std::string host = "open.live.bbc.co.uk" ;
  std::string locationid = "2641170" ; // Nottingham
  std::string urn = "/weather/feeds/en/" + locationid + "/3dayforecast.rss";

  /*
  std::vector<std::string> address = HTTPConnection::whatsmyip();
  if (address.size() > 0){
    std::cout << "Machine has the following addresses:\n" ;
    for (int i=0; i < address.size(); i++){
      std::cout << "Address " << i << ": " << address[i] << std::endl;
    }
  }
  */

  if (argc > 1){
    urn = argv[1] ;
  }
    
  con.set_urn(urn) ;
  if (!con.send_get(host, 80)){
    std::cout << "Cannot process the GET data\n" ;
  }

  std::string str = con.get_data() ;
  
  std::cout << "Return code: " << con.get_http_code() << std::endl;
  std::cout << "<<DATA>>\n" << str << "<<DATA>>\n" ;

  BBCWeather w ;
  if (!w.load(str)){
    std::cout << "Failed to parse weather RSS feed\n" ;
    return 0 ;
  }

  for (int i=0; i < 3; i++){
    BBCWeatherDay d = w.get_day(i) ;
    std::cout << d.get_title() << "\n" << d.get_description() << "\n\n" ;
  }

  return 0 ;  
}

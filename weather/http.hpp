#ifndef __HTTP_SIMPLE_HPP
#define __HTTP_SIMPLE_HPP

#include <string>
#include <vector>

class HTTPConnection{
public:
  HTTPConnection() ;

  void set_urn(const std::string urn) ;
  void set_client(const std::string client_name) ;
  bool send_get(const std::string strServer, const unsigned int port);
  int get_http_code() ;
  std::string get_data() ;
  static std::vector<std::string> whatsmyip();

  void reset() ;

protected:
  void tcp_disconnect(int fd);
  int tcp_connect(const std::string strServer, const unsigned int port) ;
  std::string m_urn ;
  std::string m_client_name ;
  int m_last_http_code ;
  std::string m_data ;
};


#endif

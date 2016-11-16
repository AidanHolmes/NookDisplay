#ifndef __HTTP_SIMPLE_HPP
#define __HTTP_SIMPLE_HPP

#include <string>
#include <vector>


class HTTPConnection{
public:
  HTTPConnection() ;

  static std::vector<std::string> whatsmyip();
  void reset() ;

  void set_urn(const std::string urn) ;
  void set_client(const std::string client_name) ;
  bool send_get(const std::string strServer, const unsigned int port);

  // Query functions for returned data 
  int get_http_code() ;
  std::string get_response_str(){return m_str_status;} ;
  std::string get_data() ;
  int get_protocol_major(){return m_http_major;};
  int get_protocol_minor(){return m_http_minor;};
  std::string get_media_type(){return m_mediatype;};
  std::string get_charset() ;


protected:
  void parse_content_type(std::string str) ;
  void process_headers(std::string id, std::string val);
  const char* parse_status_header(const char *szBuffer) ;
  const char* parse_entities(const char *szBuffer) ;
  void tcp_disconnect(int fd);
  int tcp_connect(const std::string strServer, const unsigned int port) ;
  unsigned long read_hex(char c, unsigned long val);
  unsigned long read_chunk(const char *szBuffer);
  bool read_data(int sfd, unsigned long size, std::string *str);
  bool read_line(int sfd, std::string *str);


  // Request parameters
  std::string m_urn ;
  std::string m_client_name ;

  // Response parameters
  std::string m_data ;
  std::vector<std::string> m_headers ;
  std::vector<std::string> m_values ;
  std::vector<std::string> m_content_attr;
  std::vector<std::string> m_content_value;
  bool m_is_chunked ;
  int m_request_status ;
  std::string m_str_status ;
  unsigned int m_http_major;
  unsigned int m_http_minor ;
  int m_content_length ;
  std::string m_mediatype ;
};


#endif

#ifndef __JSON__H__
#define __JSON__H__

#include <string>
#include <vector>


class JSONData{
public:
  JSONData();

  void load(std::string data){m_data = data;}
  std::string read(const std::vector<std::string> path);
  
protected:
  const char* parse_object(const char *p);
  const char* parse_array(const char *p);
  bool parse_root(const char *buff);

protected:
  std::string m_data ;
  std::string m_last_value ;
  std::vector<std::string> m_search_path ;
  std::vector<std::string>::iterator m_path_it ;
  bool m_found ;
};



#endif

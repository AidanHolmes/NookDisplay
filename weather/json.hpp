#ifndef __JSON__H__
#define __JSON__H__

#include <string>
#include <vector>

class JSONData{
public:
  JSONData();

  // Add data as char or string data. string data will
  // take preference over zero terminated char strings
  void load(std::string data){m_data = data;};
  void load(const char *szdata){m_szdata = szdata;};

  // Specify a value to return from the JSON data set by load(...)
  std::string read(const std::vector<std::string> path);
  
protected:
  // Read objects. This can call array and other object functions and
  // recurrence is used.
  const char* parse_object(const char *p);
  const char* parse_array(const char *p);

  // First call to parse a data set. Initiates object or array
  // parsing depending on first identfier found.
  bool parse_root(const char *buff);

protected:
  std::string m_data ;
  const char *m_szdata ;
  std::string m_last_value ;
  std::vector<std::string> m_search_path ;
  std::vector<std::string>::iterator m_path_it ;
  bool m_found ;
};



#endif

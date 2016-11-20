#include "json.hpp"
#include <string>
#include "identifier.hpp"
#include <stdlib.h>
#include <iostream>

#define WS(x) (x == ' ' || x == '\t' || x == '\r' || x == '\n')

JSONData::JSONData()
{
  m_found = false ;
}

std::string JSONData::read(const std::vector<std::string> path)
{
  m_last_value.clear() ;
  if (path.size() == 0){
    std::cerr << "JSON path size is zero\n" ;
    return m_last_value ;
  }

  m_search_path = path ;
  m_path_it = m_search_path.begin() ;
  m_found = false ;

  if (!parse_root(m_data.c_str())){
    std::cout << "Parse root failed\n" ;
    m_last_value.clear() ;
  }

  return m_last_value ;
}

bool JSONData::parse_root(const char *buff)
{
  const char *p = NULL, *ret = NULL ;

  for (p = buff; *p != '\0'; p++){
    if (WS(*p)) continue ;
    switch(*p){
    case '{':
      parse_object(++p) ;
      return m_found;
    case '[':
      parse_array(++p) ;
      return m_found;
    default:
      std::cerr << "JSON: Unexpected character " << *p << std::endl;
      return false ; //unexpected character
    }
  }
}

const char* JSONData::parse_array(const char *p)
{
  std::string strval ;
  bool esc = false, quoted = false;
  if (m_path_it == m_search_path.end()) return NULL ;
  char *strtolptr ;
  long array_index = 0, find_index = 0 ;

  std::cout << "Seeking path: " << *m_path_it << std::endl ;

  find_index = strtol(m_path_it->c_str(),&strtolptr,10) ;
  if (strtolptr == m_path_it->c_str()) return NULL ; // no index in path

  for ( ; *p != '\0'; p++){
    if (WS(*p) && !quoted) continue ;
    
    if (*p == '\"' && !esc){
      quoted = !quoted ;
      continue ;
    }else if (*p == '\\' && quoted && !esc){
      esc = true ;
      continue ;
    }else if ((*p == ',' || *p == ']') && !quoted){
      std::cout << "Value at index [" << array_index << "] " << strval << std::endl ;
      if (find_index == array_index){
	m_path_it++ ;
	if (m_path_it == m_search_path.end()){
	  m_last_value = strval ;
	  m_found = true ;
	  return NULL ;
	}else{
	  // Not the last item but no other objs/arrays
	  m_found = false ;
	  return NULL ;
	}
      }
      if (*p == ','){
	// Next array value
	strval.clear() ;
	array_index++ ;
	continue ;
      }else if (*p == ']'){
	return p ; // Finished processing array
      }
    }

    esc = false ;
    if ((*p == '{' || *p == '[') && !quoted && strval.length() == 0){
      if (find_index == array_index){
	m_path_it++ ; // increment path
	// Cannot be the last element as this isn't a value itself
	if (m_path_it == m_search_path.end()) return NULL ;
      }
      if (*p == '{'){
	if (!(p=parse_object(++p))) return NULL ;
	continue ;
      }else if (*p == '['){
	if (!(p=parse_array(++p))) return NULL ;
	continue ;
      }
    }else{
      strval += *p ;
    }
  }
  return NULL ; // Out of data to parse
}

const char* JSONData::parse_object(const char *p)
{
  std::string strval ;
  bool esc = false, quoted = false, returnval = false ;
  if (m_path_it == m_search_path.end()) return NULL ;
  Identifier findme(m_path_it->c_str()) ;
  enum state{attribute, value} s = attribute ;

  std::cout << "Seeking path: " << *m_path_it << std::endl ;

  for ( ; *p != '\0'; p++){
    //std::cout << *p << "(" << s << (quoted?"-q) ":") ") ;
    if (WS(*p) && !quoted) continue ;
    
    if (*p == '\"' && !esc){
      quoted = !quoted ;
      continue ;
    }else if (*p == ':' && !quoted && s == attribute){
      if (findme.get_count() > 0){
	std::cout << "Found key " << *m_path_it << std::endl;
	m_path_it++ ;
 	if (m_path_it == m_search_path.end()){ 
	  std::cout << "Search end\n" ;
	  returnval = true ;
	}
      }
      s = value ;
      continue ;
    }else if (*p == '\\' && quoted && !esc){
      esc = true ;
      continue ;
    }else if ((*p == ',' || *p == '}') && !quoted){
      std::cout << "Read value [" << strval << "]\n";
      if (findme.get_count() > 0 && returnval){
	m_last_value = strval ;
	m_found = true ;
	return NULL ;
      }else if (findme.get_count() > 0){
	// There's no further arrays or objects. Failed to find
	m_found = false ;
	return NULL ;
      }

      if (*p == ','){
	// Next attribute
	std::cout << "Processing next attribute in object\n" ;
	s = attribute;
	findme.reset() ;
	strval.clear() ;
	continue ;
      }else if (*p == '}'){
	return p ; // Finished processing object
      }
    }

    if (s==attribute){
      findme.add(*p);
    }else if (s==value){
      esc = false ;
      if (*p == '{' && !quoted && strval.length() == 0){
	if (!(p=parse_object(++p))) return NULL ;
	continue ;
      }else if (*p == '[' && !quoted && strval.length() == 0){
	if (!(p=parse_array(++p))) return NULL ;
	continue ;
      }else{
	strval += *p ;
      }
    }
  }
  std::cout << "Ran out of text. Hit terminator\n" ;
  return NULL ; // No more text to parse
}

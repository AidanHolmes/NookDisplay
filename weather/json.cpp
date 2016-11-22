#include "json.hpp"
#include <string>
#include "identifier.hpp"
#include <stdlib.h>
#include <iostream>

#define WS(x) (x == ' ' || x == '\t' || x == '\r' || x == '\n')

#ifndef DEBUG
//#define DEBUG
#endif

#ifdef DEBUG
#define DPRINT(x) std::cout << std::string("DBG: ") << x ;
#else
#define DPRINT(x)
#endif


JSONData::JSONData()
{
  m_found = false ;
  m_szdata = NULL ;
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

  const char *buff = m_szdata;
  if (m_data.length() > 0) buff = m_data.c_str() ;
  if (!parse_root(buff)){
    m_last_value.clear() ;
  }

  return m_last_value ;
}

bool JSONData::parse_root(const char *buff)
{
  const char *p = NULL;

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
  return false;
}

const char* JSONData::parse_array(const char *p)
{
  std::string strval ;
  bool esc = false, quoted = false;
  if (m_path_it == m_search_path.end()) return NULL ;
  char *strtolptr ;
  long array_index = 0, find_index = -1 ;

  DPRINT("Seeking path: " << *m_path_it << std::endl) ;

  find_index = strtol(m_path_it->c_str(),&strtolptr,10) ;
  if (strtolptr == m_path_it->c_str()) find_index = -1;

  for ( ; *p != '\0'; p++){
    if (WS(*p) && !quoted) continue ;
    
    if (*p == '\"' && !esc){
      quoted = !quoted ;
      continue ;
    }else if (*p == '\\' && quoted && !esc){
      esc = true ;
      continue ;
    }else if ((*p == ',' || *p == ']') && !quoted){
      DPRINT("Value at index [" << array_index << "] " << strval << std::endl) ;
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
  std::string strval, strkey ;
  bool esc = false, quoted = false, returnval = false ;
  if (m_path_it == m_search_path.end()) return NULL ;
  enum state{attribute, value} s = attribute ;

  DPRINT("Seeking path: " << *m_path_it << std::endl) ;

  for ( ; *p != '\0'; p++){
    if (WS(*p) && !quoted) continue ;
    
    if (*p == '\"' && !esc){
      quoted = !quoted ;
      continue ;
    }else if (*p == ':' && !quoted && s == attribute){
      if (strkey == *m_path_it){
	DPRINT("Found key " << *m_path_it << std::endl);
	m_path_it++ ;
 	if (m_path_it == m_search_path.end()){ 
	  DPRINT("Search end\n") ;
	  returnval = true ;
	}
      }
      s = value ;
      continue ;
    }else if (*p == '\\' && quoted && !esc){
      esc = true ;
      continue ;
    }else if ((*p == ',' || *p == '}') && !quoted){
      DPRINT("Read key [" << strkey << "] value \"" << strval << "\"\n");
      if (returnval){
	m_last_value = strval ;
	m_found = true ;
	return NULL ;
      }else if (strkey == *m_path_it){
	// There's no further arrays or objects. Failed to find
	m_found = false ;
	return NULL ;
      }

      if (*p == ','){
	// Next attribute
	DPRINT("Processing next attribute in object\n") ;
	s = attribute;
	strval.clear() ;
	strkey.clear() ;
	continue ;
      }else if (*p == '}'){
	return p ; // Finished processing object
      }
    }

    if (s==attribute){
      strkey += *p ;
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
  DPRINT("Ran out of text. Hit terminator\n") ;
  return NULL ; // No more text to parse
}

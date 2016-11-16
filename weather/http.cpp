#include "http.hpp"

#include <string>
#include <iostream>
#include <sys/utsname.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include "identifier.hpp"

#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

#define MAXLINE 4096


HTTPConnection::HTTPConnection()
{
  m_client_name = "Homebrew/1.0" ;
  reset() ;
}

void HTTPConnection::reset()
{
  m_data.clear() ;
  m_urn.clear() ;
  m_headers.clear() ;
  m_values.clear() ;
  m_is_chunked = false ;
  m_request_status = 0 ;
  m_str_status.clear() ;
  m_http_minor = m_http_major = 0 ;
  m_content_length = 0;
}

std::string HTTPConnection::get_data()
{
  return m_data ;
}

int HTTPConnection::get_http_code()
{
  return m_request_status ;
}

void HTTPConnection::set_client(const std::string client_name)
{
  m_client_name = client_name ;
}

void HTTPConnection::set_urn(const std::string urn)
{
  m_urn = urn ;
}

unsigned long HTTPConnection::read_hex(char c, unsigned long val)
{
  unsigned long num = 0 ;
  if (c >= '0' && c <= '9') num = c - '0' ;
  if (c >= 'A' && c <= 'F') num = c - 'A' + 10 ;
  if (c >= 'a' && c <= 'f') num = c - 'a' + 10 ;
  
  return (val * 16) + num ;
}

unsigned long HTTPConnection::read_chunk(const char *szBuffer)
{
  Identifier term("\r\n") ;
  bool end = false ;
  const char *p = szBuffer ;
  unsigned long size = 0;
  while (*p != '\0'){
    if (term.add(*p)) break ;
    if (*p == ';'){ end = true ;continue ;}
    if (!end) size = read_hex(*p, size);
    p++ ;
  }
  return size ;
}

bool HTTPConnection::read_line(int sfd, std::string *str)
{
  Identifier term ("\r\n") ;
  char c = 0;
  int nres = 0;
  str->clear() ;
  do{
    if ( (nres = read(sfd, &c, 1)) < 0){
      if (errno == EINTR) continue ;
      else{
	std::cerr << "Failed to read HTTP data stream\n" ;
	tcp_disconnect(sfd) ;
	return false ;
      }
    }
    *str += c ;
  }while (term.add(c));

  return true ;
}

const char* HTTPConnection::parse_status_header(const char *szBuffer)
{
  Identifier http("HTTP/") ;
  Identifier term("\r\n") ;
  unsigned int major = 0, minor = 0, status = 0, state = 0;
  // Read the status header
  const char *p = szBuffer ;
  while (*p != '\0'){
    http.add(*p) ;
    if (term.add(*p)){ 
      if (status == 0) return NULL ; // cannot find status
      m_request_status = status ;
      m_http_major = major ;
      m_http_minor = minor ;
      return p+1 ; // Return updated pointer to end of header
    }
    if (http.get_count() > 0){
      // Found the HTTP header
      if (*p >= '0' && *p <= '9' && state <= 2){
	if (state == 0){
	  major = (major * 10) + (*p - '0') ;
	}else if (state == 1){
	  minor = (minor * 10) + (*p - '0') ;
	}else if (state == 2){
	  status = (status * 10) + (*p - '0') ;
	}
      }else if (*p == '.'){
	state = 1;
      }else if (state == 1 && *p == ' '){ 
	state = 2 ;
      }else if (state == 2 && *p == ' '){
	state = 3 ;
      }else if (state == 3){
	m_str_status.push_back(*p) ;
      }
    }
    p++ ;
  }
  return NULL ; // ran out of characters, didn't find header
}

void HTTPConnection::process_headers(std::string id, std::string val)
{
  std::cout << "Found header: (" << id << ") value: (" << val << ")\n";
  if (id == "Content-Length") m_content_length = atoi(val.c_str()) ;
  if (id == "Transfer-Encoding" && val == "chunked") m_is_chunked = true;
  if (id == "Content-Type") parse_content_type (val) ;
}

void HTTPConnection::parse_content_type(std::string str)
{
  Identifier term("\r\n") ;
  const char *p = NULL ;
  int state = 0 ;
  std::string attribute, value ;
  bool esc = false ;
  
  for(p = str.c_str() ;*p != '\0'; p++){
    if (term.add(*p)) break ;

    if (state !=3){
      if (*p == ' ' || *p == '\t') continue ;
      if (*p == ';'){ 
	if (attribute.length() > 0){
	  m_content_attr.push_back(attribute) ; attribute.clear();
	  m_content_value.push_back(value) ; value.clear() ;
	}
	state = 1; 
	continue;
      }
      else if (*p == '=' && state == 1){state = 2; continue;}
      else if (*p == '\"' && state == 2){state = 3 ; continue;}
    }else if (*p == '\"' && !esc && state == 3){state=1;continue;}
    else if (*p == '\\' && state == 3){ esc = true ;continue ;}

    if (state == 0) m_mediatype.push_back(*p) ;
    else if (state == 1) attribute.push_back(*p) ;
    else if (state == 2 || state == 3) value.push_back(*p) ;
    
    esc = false ;
  }

  if (state > 0 && attribute.length() > 0){
    m_content_attr.push_back(attribute) ;
    m_content_value.push_back(value) ;
  }

}

const char* HTTPConnection::parse_entities(const char *szBuffer)
{
  Identifier term("\r\n") ;
  Identifier hdr_term("\r\n\r\n") ;
  std::string strId, strVal ;
  const char *p = NULL ;
  int state = 0 ;
  for (p=szBuffer; *p != '\0'; p++){
    if ((*p == ' ' || *p == '\t') && state != 2) continue ;
    if (hdr_term.add(*p)) return p+1 ;
    if (term.add(*p)){
      if (state >= 1){
	m_headers.push_back(strId) ;
	m_values.push_back(strVal) ;
	process_headers(strId, strVal);
      }
      state = 0 ;
      strId.clear() ;
      strVal.clear() ;
      continue ;
    }
    if (*p == ':'){
      state = 1 ;
      continue ;
    }
    if (*p != '\r' && *p != '\n'){
      if (state == 0) strId.push_back(*p) ;
      if (state == 1 && strVal.length() == 0) state = 2;
      if (state == 2) strVal.push_back(*p);
    }
  }
  return NULL ; // Header didn't terminate
}

bool HTTPConnection::read_data(int sfd, unsigned long size, std::string *str)
{
  char fbuffer[MAXLINE + 1] ;
  char *buffer = fbuffer ;
  int nres = 0 ;

  if (size == 0) return true ;

  if (size >= MAXLINE){
    buffer = new char[size + 1] ;
    if (!buffer) return false ;
  }

  while (size > 0){
    if ( (nres = read(sfd, buffer, size)) < 0){
      if (errno == EINTR) nres = 0 ;
      else{
	std::cerr << "Failed to read HTTP data stream\n" ;
	tcp_disconnect(sfd) ;
	if (buffer != fbuffer) delete[] buffer ;
	return false ;
      }
    }
    buffer[nres] = '\0' ;
    *str += buffer ;
    size -= nres ;
  }

  if (buffer != fbuffer) delete[] buffer ;

  return true ;
}

bool HTTPConnection::send_get(const std::string strServer, const unsigned int port)
{
  int sfd = -1, nres ;
  char buffer[MAXLINE + 1], szPort[10] ;
  std::string client_request ;

  if ( (sfd = tcp_connect(strServer, port)) < 0){
    return false ;
  }

  client_request = "GET " + m_urn + " HTTP/1.1\r\nHost: " + strServer ;
  if (port != 80){
    snprintf(szPort, 10, ":%u", port) ;
    client_request += szPort ;
  }
  client_request += "\r\nUser-Agent: " + m_client_name + "\r\n\r\n" ;

  //std::cout << "Sending:\n" << client_request << std::endl; 
  nres = write(sfd, client_request.c_str(), client_request.length()) ;
  if (nres != (ssize_t)client_request.length()){
    tcp_disconnect(sfd) ;
    return false ;
  }
  
  nres = read(sfd, buffer, MAXLINE) ;
  if (nres < 0){
    if (errno == EINTR) nres = 0 ;
    else{
      std::cerr << "An error occurred reading from the socket\n" ;
      tcp_disconnect(sfd) ;
      return false;
    }
  }
  buffer[nres] = '\0' ;

  std::cout << "<<DEBUG>>\n" << buffer << "<<DEBUG>>\n" ;

  const char *p = parse_status_header(buffer) ;
  if (!p){
    std::cerr << "An error occurred reading the status line\n" ;
    tcp_disconnect(sfd) ;
    return false;
  }

  p = parse_entities(p) ;
  if (!p){
    std::cerr << "An error occurred reading the headers\n" ;
    tcp_disconnect(sfd) ;
    return false;
  }

  std::cout << "Parsing body...\n" ; 
  if (m_is_chunked){
    std::string line ;
    unsigned long size = read_chunk(p) ;
    m_data = p ;
    size -= m_data.length() ;
    
    while (size > 0){
      std::cout << "reading bytes " << size << " from stream\n" ;
      if (!read_data(sfd, size, &m_data)){
	std::cerr << "An error occurred reading chunked data\n" ;
	tcp_disconnect(sfd) ;
	return false;
      }

      if (!read_line(sfd, &line)){
	std::cerr << "An error occurred reading chunked data line\n" ;
	tcp_disconnect(sfd) ;
	return false;
      }

      size = read_chunk(line.c_str()) ;
      std::cout << "Reading chuck data of size " << size << std::endl;
    }
  }else{
    m_data = p ;
    // Get remaining data if any
    long remaining = m_content_length - m_data.length() ;
    std::cout << "Content length: " << m_content_length << std::endl;
    std::cout << "Reading remaining data of length: " << remaining << std::endl;
    if (remaining > 0){
      if (!read_data(sfd, remaining, &m_data)){
	std::cerr << "An error occurred reading content\n" ;
	tcp_disconnect(sfd) ;
	return false;
      }
    }
  }

  tcp_disconnect(sfd) ;
  return true ;
}
void HTTPConnection::tcp_disconnect(int fd)
{
  if (fd < 0) return ;

  shutdown(fd, SHUT_RDWR) ;
}

int HTTPConnection::tcp_connect(const std::string strServer, const unsigned int port)
{
  int sfd = -1 ;

  struct sockaddr_in servaddr ;
  struct hostent *phost=NULL ;

  // Resolve IPv4 addresses by hostname
  phost = gethostbyname(strServer.c_str()) ;
  if (!phost){
    std::cerr << "Cannot resolve host name " << strServer << std::endl;
    return -1 ;
  }

  memset(&servaddr, 0, sizeof(servaddr)) ;
  servaddr.sin_family = phost->h_addrtype ;
  servaddr.sin_port = htons(port) ;
  servaddr.sin_addr = *(in_addr*)(phost->h_addr_list[0]) ;
  
  if ((sfd = socket(phost->h_addrtype, SOCK_STREAM, 0)) < 0){
    std::cerr << "Cannot open socket to " << strServer << " on port " << port << std::endl;
    return -1 ;
  }

  if (connect(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
    std::cerr << "Cannot connect to socket on server " << strServer << " on port " << port << std::endl;
    shutdown(sfd, SHUT_RDWR) ;
    return -1 ;
  }

  return sfd ;
}

std::vector<std::string> HTTPConnection::whatsmyip()
{
  std::vector<std::string> return_vec ;
  struct hostent *hptr = NULL ;
  struct utsname hostname ;
  char ipstr[INET6_ADDRSTRLEN] ;

  if (uname(&hostname) < 0){
    return return_vec ; // empty list indicates error
  }

  // This will resolve just IPv4 addresses. 
  if (!(hptr = gethostbyname(hostname.nodename))){
    return return_vec ; // empty list on error
  }

  char **ppaddr = hptr->h_addr_list ;
  while (*ppaddr){
    
    return_vec.push_back(inet_ntop(hptr->h_addrtype, *ppaddr, ipstr, sizeof(ipstr))) ;
    ppaddr++ ;
  }

  return return_vec ;
}

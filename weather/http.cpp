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

#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

#define MAXLINE 4096

HTTPConnection::HTTPConnection()
{
  m_client_name = "homebrew/1.0" ;
  reset() ;
}

void HTTPConnection::reset()
{
  m_last_http_code = 0 ;
  m_data.clear() ;
  m_urn.clear() ;
}

std::string HTTPConnection::get_data()
{
  return m_data ;
}

int HTTPConnection::get_http_code()
{
  return m_last_http_code ;
}

void HTTPConnection::set_client(const std::string client_name)
{
  m_client_name = client_name ;
}

void HTTPConnection::set_urn(const std::string urn)
{
  m_urn = urn ;
}

bool HTTPConnection::send_get(const std::string strServer, const unsigned int port)
{
  int sfd = -1, nres ;
  char buffer[MAXLINE + 1], szPort[10] ;
  std::string client_request ;
  std::string server_response ;

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
  if (nres != client_request.length()){
    tcp_disconnect(sfd) ;
    return false ;
  }
  
  while ( (nres = read(sfd, buffer, MAXLINE)) > 0){
    buffer[nres] = '\0' ;
    server_response += buffer ;
    if (nres < MAXLINE){
      break;
    }
  }

  if (nres < 0){
    std::cerr << "An error occurred reading from the socket\n" ;
    tcp_disconnect(sfd) ;
    return false;
  }

  //std::cout << "<<DEBUG>>\n" << server_response << "<<DEBUG>>\n" ;
  // Parse header result
  const int lenbuf = 10 ;
  bool bChunked = false ;
  int nstate = -1, i =0;
  char szStatus[] = "000" ;
  char szLength[lenbuf] ;
  szLength[lenbuf-1] = '\0' ;
  char szTerminator[] = "\r\n\r\n", *p=szTerminator ;
  char szContentLen[] = "Content-Length", *pcl=szContentLen;
  char szChunked[] = "chunked", *pc=szChunked ;
  for ( ; i < server_response.length(); i++){
    char c = server_response[i] ;
    if (nstate < 0 && c == ' ') nstate = 0 ;
    else if (nstate >= 0 && nstate < 3) szStatus[nstate++] = c ;
    else if (nstate >=4 && nstate < (4+lenbuf-2)){
      if (c == '\r'){
	szLength[nstate-4] = '\0' ;
	nstate = 4+lenbuf-2;
      }
      if (c != ' ' && c != ':') szLength[(nstate++)-4] = c ;
    }

    pc = (*pc == c)?pc+1:szChunked ;
    if (*pc == '\0') bChunked = true ;
    pcl = (*pcl == c)?pcl+1:szContentLen ;
    if (*pcl == '\0') nstate = 4 ;
    p = (*p == c)?p+1:szTerminator ;
    if (*p == '\0') break ;
  }
  if (*p != '\0'){
    // Cannot find header terminator
    std::cerr << "No terminator found for response header\n" ;
    tcp_disconnect(sfd) ;
    return false ;
  }

  m_data = server_response.substr(i+1) ;

  int data_len = atoi(szLength) ;
  if (data_len == 0 && bChunked){
    // TO DO - read chunked data
    std::cerr << "Chunked data in response. Cannot read\n" ;
    tcp_disconnect(sfd) ;
    return false ;
  }

  m_last_http_code = atoi(szStatus) ;

  // Get remaining data if any
  int remaining = data_len - m_data.length() ;
  while (remaining > 0){
    if ( (nres = read(sfd, buffer, MAXLINE)) < 0){
      if (errno == EINTR) nres = 0 ;
      else{
	std::cerr << "Failed to read remaining of the HTTP body\n" ;
	tcp_disconnect(sfd) ;
	return false ;
      }
    }      
    buffer[nres] = '\0' ;
    m_data += buffer ;
    remaining -= nres ;
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

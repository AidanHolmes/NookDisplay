#include "nookinput.hpp"
#include <iostream>
#include <string>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

NookInput::NookInput()
{
  m_id = 0 ;
  m_fd = -1;
}

bool NookInput::create(std::string strDev, unsigned int eventid)
{
  char name[256], id[15] ;
  std::string strEvent ;
  std::string strEvtPath = "/event";
  int flags ;

  // std::to_string not playing. Use stdio to format a char string
  snprintf(id, 15, "%u", eventid) ;

  if (strDev.at(strDev.length()-1) == '/'){
    strEvtPath = strEvtPath.substr(1) ;
  }
  strEvent = strDev + strEvtPath + id ;

  if ((m_fd = open(strEvent.c_str(), O_RDONLY)) < 0){
    std::cerr << "Cannot open event: " << strEvent << std::endl;
    return false ;
  }

  // Non-blocking IO
  if (-1 == (flags = fcntl(m_fd, F_GETFL, 0)))
    flags = 0;
  fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);

  // Query the device name
  if (ioctl(m_fd, EVIOCGNAME(sizeof(name)), name) < 0){
    std::cerr << "EVIOCGNAME query failed for event " << eventid << std::endl;
    close(m_fd) ;
    m_fd = -1 ;
    return false ;
  }

  m_name = name ;

  m_id = eventid ;

  return true ;
}

bool NookInput::read_next_event()
{
  int bytes_read = 0 ;
  bytes_read = read(m_fd, &m_event, sizeof(struct input_event)) ;
  if (bytes_read < (int)sizeof(struct input_event)){
    return false ;
  }
  return true ;
}

Event& NookInput::get_next()
{
  m_evtCache.valid = false ;
    
  if (read_next_event()){
    m_evtCache.valid = true ;
    m_evtCache.type = m_event.type ;
    m_evtCache.code = m_event.code ;
    m_evtCache.value = m_event.value ;
  }
  return m_evtCache ;
}


//////////////////////////////////////////////
//
// NookKeys
//
//

NookKeys::NookKeys()
{
  NookInput();
  m_evtCache.key_down = false ;
}

KeyEvent& NookKeys::get_next_key()
{
  m_evtCache.valid = false ;

  while (read_next_event()){
    // Filter to only return EV_KEY
    if (m_event.type == EV_KEY){
      m_evtCache.valid = true ;
      m_evtCache.type = m_event.type ;
      m_evtCache.code = m_event.code ;
      m_evtCache.value = m_event.value ;
      m_evtCache.key_down = (m_event.value==1)?true:false ;
      return m_evtCache ;
    }
  }
  return m_evtCache ;
}

//////////////////////////////////////////////
//
// NookTouch
//
//

NookTouch::NookTouch()
{
  NookInput() ;
  m_evtCache.touch_down = false ;
  m_evtCache.x = 0;
  m_evtCache.y = 0;
  m_tmp_x = m_tmp_y = 0 ;

  m_got_x = false ;
  m_got_y = false ;

  m_min_x = m_max_x = m_min_y = m_max_y = 0 ;

  m_rotated = false ;
}

bool NookTouch::create(std::string strDev, unsigned int eventid)
{
  int abs[5] ;

  bool bret = NookInput::create(strDev, eventid);
  if (!bret) return false;

  // Query X
  if (ioctl(m_fd, EVIOCGABS(ABS_X), abs) < 0){
    std::cerr << "EVIOCGABS query failed for absolute X\n";
    close(m_fd) ;
    m_fd = -1 ;
    return false ;
  }
  m_min_x = abs[1] ;
  m_max_x = abs[2] ;

  // Query Y
  if (ioctl(m_fd, EVIOCGABS(ABS_Y), abs) < 0){
    std::cerr << "EVIOCGABS query failed for absolute Y\n";
    close(m_fd) ;
    m_fd = -1 ;
    return false ;
  }
  m_min_y = abs[1] ;
  m_max_y = abs[2] ;

  std::cout << "ABS X Min: " << m_min_x << " Max: " << m_max_x << std::endl;
  std::cout << "ABS Y Min: " << m_min_y << " Max: " << m_max_y << std::endl;

  return true ;
}

TouchEvent& NookTouch::get_next_touch()
{
  m_evtCache.valid = false ;
  bool btmp = false ;

  while (read_next_event()){
    // Not really relevent for this event
    // but they are set anyway
    m_evtCache.type = m_event.type ;
    m_evtCache.code = m_event.code ;
    m_evtCache.value = m_event.value ;

    // Keys and absolute values returned from z-force
    switch(m_event.type){
    case EV_KEY:
      // The only code is 330 for z-force touch
      // Return the key. The X & Y may relate to the key touch as the 
      // key code seems to follow the coordinates
      btmp = (m_event.value==1)?true:false;
      if (m_evtCache.touch_down != btmp){
	m_evtCache.touch_down = btmp;
	m_evtCache.valid = true ;
	return m_evtCache ;
      }
      break ;
    case EV_ABS:
      if (m_event.code == ABS_X){
	m_tmp_x = m_event.value;
	m_got_x = true ;
      }else if (m_event.code == ABS_Y){
	m_tmp_y = m_event.value ;
	m_got_y = true ;
      }
      break ;
    default:
      // Do nothing and check for more events
      break;
    };
    if (m_got_x && m_got_y){
      m_got_x = m_got_y = false ;
      m_evtCache.valid = true ;
      m_evtCache.touch_down = true ; // Touch must be down
      if (m_rotated){
	// Transform x and y to landscape
	m_evtCache.x = m_tmp_y;
	m_evtCache.y = m_max_x - m_tmp_x ;
      }else{
	m_evtCache.x = m_tmp_x ;
	m_evtCache.y = m_tmp_y ;
      }
      return m_evtCache;
    }
    
  }
  return m_evtCache ;
}

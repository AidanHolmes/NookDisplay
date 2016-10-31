#ifndef __NOOKINPUT_HPP
#define __NOOKINPUT_HPP

#include <string>
#include <vector>
#include <memory>
#include <linux/input.h>

class Event{
public:
  bool valid ;
  unsigned int type ;
  unsigned int code ;
  long value ;
};

class NookInput{
public:

  NookInput();

  // Setup the event
  bool create(std::string strDev, unsigned int eventid);
  unsigned int get_ID(){return m_id;};

  unsigned int queue_len() ;
  Event& get_next() ;
  std::string get_name(){return m_name ;};

protected:
  // true if event can be read, otherwise false
  bool read_next_event() ;

  struct input_event m_event ;
  unsigned int m_id ;
  int m_fd ;
  std::string m_name ;

private:
  Event m_evtCache ;

};

class KeyEvent : public Event{
public:
  bool key_down;
};

class NookKeys : public NookInput{
public:
  NookKeys() ;
  KeyEvent& get_next_key() ;
private:
  KeyEvent m_evtCache ;
};

class TouchEvent : public Event{
public:
  bool touch_down;
  long x;
  long y;
} ;

class NookTouch : public NookInput{
public:
  NookTouch() ;
  TouchEvent& get_next_touch() ;
protected:
  bool m_got_x ;
  bool m_got_y ;
private:
  TouchEvent m_evtCache ;
};

#endif

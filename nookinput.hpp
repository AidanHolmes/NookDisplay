#ifndef __NOOKINPUT_HPP
#define __NOOKINPUT_HPP

#include <string>
#include <vector>
#include <memory>
#include <linux/input.h>

class Event{
public:
  bool valid ;
  char type ;
  char code ;
  char value ;
};

class NookInput{
public:

  NookInput();

  // Setup the event
  bool create(std::string strDev, unsigned int eventid);
  unsigned int get_ID(){return m_id;};

  unsigned int queue_len() ;
  Event& get_next() ;

  Event m_evtCache ;

protected:
  // true if event can be read, otherwise false
  bool read_next_event() ;

  struct input_event m_event ;
  unsigned int m_id ;
  int m_fd ;
  std::string m_name ;

private:

};

class KeyEvent : public Event{
public:
  bool key_down;
};

class NookKeys : public NookInput{
public:
  NookKeys() ;
  KeyEvent& get_next_key() ;
  KeyEvent m_evtCache ;
protected:

private:

};


#endif

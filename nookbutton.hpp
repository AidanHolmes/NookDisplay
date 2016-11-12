#ifndef __NOOKWINDOW_BTN_HPP
#define __NOOKWINDOW_BTN_HPP

#include "nookwindow.hpp"
#include "nookfont.hpp"
#include <string>

class NookButton : public NookWindow{
public:
  // Define text for the button. Text will be sized to fit the button
  // as well as can be. 
  void add_text(std::wstring strtxt, long pt_size = 0) ;

  void initialise() ;

  // Draw the button.
  virtual bool draw();

  // Manage touch screen events
  virtual void touch_event(TouchEvent &touch, unsigned int x_offset, unsigned int y_offset);
  
  // These do nothing in the base class. These can optionally be 
  // overridden in the derived class to handle the events
  virtual void button_down(){};
  virtual void button_up(){} ;

protected:
  std::wstring m_text ;
  bool m_btndown ;
  NookFont m_fnt ;
  long m_txtsize ;
} ;


#endif

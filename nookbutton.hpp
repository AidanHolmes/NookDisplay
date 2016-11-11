#ifndef __NOOKWINDOW_BTN_HPP
#define __NOOKWINDOW_BTN_HPP

#include "nookwindow.hpp"
#include "nookfont.hpp"
#include <string>

class NookButton : public NookWindow{
public:
  // Define text for the button. Text will be sized to fit the button
  // as well as can be. 
  void add_text(std::wstring strtxt) ;

  void initialise() ;

  // Draw the button.
  virtual bool draw();

  // Manage touch screen events
  virtual void touch_event(TouchEvent &touch);

  // These do nothing in the base class. These can optionally be 
  // overridden in the derived class to handle the events
  virtual void button_down(){};
  virtual void button_up(){} ;

protected:
  std::wstring m_text ;
  bool m_btndown ;
  NookFont m_fnt ;
} ;


#endif

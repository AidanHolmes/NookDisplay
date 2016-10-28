#ifndef __NOOKWINDOW_HPP
#define __NOOKWINDOW_HPP

#include <vector>
#include "displayimage.hpp"

class NookWindow{
public:
  NookWindow();
  enum state{clean,dirty,verydirty} ;

  // Create the screen for the window
  bool create(unsigned int x, unsigned int y, unsigned int width, unsigned int height) ;

  // Add window as a child window
  void add_window(NookWindow &wnd) ;

  // Virtual. Called to send a redraw request to all windows
  virtual void redraw();

  // Virtual. Call to redraw the window
  virtual bool draw(){return true;};

  // Virtual. Tick the window. This doesn't dictate the amount of time passed
  virtual bool tick(){return true;};

  // Activate function to do any initialisation on the window if activated
  // Optional to implement. Base function does nothing
  virtual bool activate(){return true;};

  // Deactivate function to clean up the window if deactivated.
  // Optional, does nothing if baseclass is used
  virtual bool deactivate(){return true;};

  void set_origin(unsigned int x, unsigned int y){m_x_pos = x; m_y_pos = y;};
  unsigned int get_x_pos(){return m_x_pos;} ;
  unsigned int get_y_pos(){return m_y_pos;} ;

  bool is_hidden(){return m_bHidden;};
  void hide(bool bHide){m_bHidden = bHide;} ;

  state get_state() ;
  void invalidate_window(bool bFull = false);

  // Access is required to this object externally. This won't mess around with
  // friend classes to make access work.
  DisplayImage canvas;
  

protected:
  std::vector<NookWindow> m_children ;
  bool m_bHidden ;
  state m_enState ;
  unsigned int m_x_pos, m_y_pos ;
  unsigned int m_width, m_height ;

private:

};


#endif

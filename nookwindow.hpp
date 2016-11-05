#ifndef __NOOKWINDOW_HPP
#define __NOOKWINDOW_HPP

#include <vector>
#include "displayimage.hpp"
#include "nookinput.hpp"

class NookWindow{
public:
  NookWindow();
  enum state{clean,dirty,verydirty} ; // Window states

  // Create the screen for the window
  bool create(unsigned int x, unsigned int y, unsigned int width, unsigned int height) ;

  // Add window as a child window
  void add_window(NookWindow &wnd) ;

  // Defines the offset from the parent canvas. get_* calls return the offset values
  void set_origin(unsigned int x, unsigned int y){m_x_pos = x; m_y_pos = y;};
  unsigned int get_x_pos(){return m_x_pos;} ;
  unsigned int get_y_pos(){return m_y_pos;} ;

  void set_window_merge(bool bSet){m_copy_mode = bSet?2:0 ;};
  void set_white_transparency(bool bSet){m_copy_mode = bSet?4:0;};

  // Check status of window with is_hidden call. Hide a window with hide(true/false)
  bool is_hidden(){return m_bHidden;};
  void hide(bool bHide){m_bHidden = bHide;} ;

  // Return state of window which is defined in the enum in this header
  state get_state() ;
  // Set the state to dirty or verydirty if bFull is true. 
  // dirty and verydirty will trigger a refresh. verydirty causes a full screen refresh
  void invalidate_window(bool bFull = false);

  // Virtual. Called to send a redraw request to all windows
  virtual void redraw();

  // Virtual. Call to redraw the window
  virtual bool draw(){return true;};

  // Virtual. Tick the window. This doesn't dictate the amount of time passed
  virtual bool tick(){return true;};

  // Virtual call for windows to create any additional setup when created. Called from
  // create after graphics canvas has been created. 
  virtual void initialise(){};

  // Activate function to do any initialisation on the window if activated
  // Optional to implement. Base function does nothing
  virtual bool activate(){return true;};

  // Deactivate function to clean up the window if deactivated.
  // Optional, does nothing if baseclass is used
  virtual bool deactivate(){return true;};

  // Called for key presses for the 4 buttons on sides of Nook or through
  // the GPIO home and power keys
  virtual void key_event(KeyEvent &keys) ;

  // Called for touchscreen events
  virtual void touch_event(TouchEvent &touch) ;

  // Access is required to this object externally. This won't mess around with
  // friend classes to make access work.
  DisplayImage canvas;
  

protected:
  std::vector<NookWindow*> m_children ;
  bool m_bHidden ;
  state m_enState ;
  unsigned int m_x_pos, m_y_pos ;
  unsigned int m_width, m_height ;
  int m_copy_mode ;

private:

};


#endif

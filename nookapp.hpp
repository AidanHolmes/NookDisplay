#ifndef __NOOKAPP_HPP
#define __NOOKAPP_HPP

#include <string>
#include <vector>
#include "nookwindow.hpp"
#include <linux/fb.h>

class NookApp{
public:
  NookApp();
  ~NookApp() ;

  // Window functions
  void register_window(NookWindow &wnd);
  bool activate_window_next();
  bool activate_window(std::string strName);
  NookWindow *get_active_window();

  // Input functions
  enum input{touchscreen, key, gpio} ;
  bool init_inputs(std::string strEventBase);
  // setup an input event for use by the application

  // Dispatch drawing and input events to active window
  // Should be called from run()
  void dispatch_app_events();

  // Display functions
  bool init_display(std::string strFrameBuffer, std::string strrefresh) ; 
  bool write_to_nook(DisplayImage &img, bool bFull = false) ;
  
  // Start the application. Function returns when app closes
  // This needs implementing by an application object derived from this
  // class
  virtual bool run() = 0 ;

private:

protected:
  std::vector<NookWindow*> m_windows; 
  std::string m_fileRefresh ;
  NookWindow *m_pactivewindow ;
  struct fb_var_screeninfo m_vi;
  struct fb_fix_screeninfo m_fi;
  int m_iofb ;
  unsigned char* m_framebuffer;
  uint16_t* m_imgout ;
  int m_fb_page ;
  unsigned int m_fbw, m_fbh ;

  NookKeys m_inputkeys ;
  bool m_binputkeys_enabled ;

  NookKeys m_inputgpio;
  bool m_binputgpio_enabled ;

  NookTouch m_inputtouch;
  bool m_binputtouch_enabled ;
};

#endif

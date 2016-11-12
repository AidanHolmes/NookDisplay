#include "nookapp.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "nookinput.hpp"

using namespace std;

NookApp::NookApp()
{
  m_pactivewindow = NULL ;
  m_iofb = -1 ;
  m_framebuffer = NULL ;
  m_fb_page = 1;
  m_imgout = NULL ;
  m_fbw = m_fbh = 0 ;
  m_binputkeys_enabled = false;
  m_binputgpio_enabled = false;
  m_binputtouch_enabled = false;
  m_bLandscape = false ;
}

NookApp::~NookApp()
{
  if (m_imgout) delete[] m_imgout ;
  m_imgout = NULL ;
  // delete the memory mapped frame buffer
  if (m_framebuffer){
    munmap(m_framebuffer, m_fi.smem_len);
    close(m_iofb) ;
    m_framebuffer = NULL ;
    m_iofb = -1;
  }
}

bool NookApp::init_inputs(string strEventBase)
{
  bool bRet = false ;
  bRet = m_inputkeys.create(strEventBase.c_str(), 0) ;
  if (bRet) m_binputkeys_enabled = true ;

  bRet = m_inputgpio.create(strEventBase.c_str(), 1) ;
  if (bRet) m_binputgpio_enabled = true ;

  bRet = m_inputtouch.create(strEventBase.c_str(), 2) ;
  if (bRet) m_binputtouch_enabled = true ;

  return bRet ;
}

void NookApp::dispatch_app_events()
{
  NookWindow::state s = get_active_window()->get_state() ;
  bool bFull = s == NookWindow::verydirty ;
  if (s != NookWindow::clean){
    get_active_window()->redraw();
    if (m_bLandscape){
      // Rotate the canvas
      DisplayImage rotated_canvas;
      rotated_canvas.copy_rotate90_right(get_active_window()->canvas) ;
      write_to_nook(rotated_canvas, bFull) ;
    }else{
      write_to_nook(get_active_window()->canvas, bFull);
    }
  }

  // Process keys
  if (m_binputkeys_enabled){
    KeyEvent key = m_inputkeys.get_next_key() ;
    if (key.valid){
      get_active_window()->key_event(key);
    }
  }

  // Process gpio keys
  if (m_binputgpio_enabled){
    KeyEvent gkey = m_inputgpio.get_next_key() ;
    if (gkey.valid){
      get_active_window()->key_event(gkey);
    }
  }

  // Process touch screen
  if (m_binputtouch_enabled){
    TouchEvent touch = m_inputtouch.get_next_touch() ;
    if (touch.valid){
      get_active_window()->touch_event(touch, get_active_window()->get_x_pos(), get_active_window()->get_y_pos());
    }
  }

  get_active_window()->tick() ;
}

void NookApp::rotate_display(bool bLandscape)
{
  if (m_bLandscape == bLandscape) return ; // nothing to do

  m_bLandscape = bLandscape ;

  if (m_binputtouch_enabled)
    m_inputtouch.rotate_90_right(bLandscape) ;

  if (bLandscape){
    m_fbw = m_vi.yres ;
    m_fbh = m_vi.xres ;
  }else{
    m_fbh = m_vi.yres ;
    m_fbw = m_vi.xres ;
  }

  // Send a resize to all top-level windows
  for (vector<NookWindow*>::iterator i=m_windows.begin(); i != m_windows.end(); i++){
    (*i)->resize(m_fbw, m_fbh) ;
  }
}

bool NookApp::init_display(string strfb, string strrefresh)
{
  m_iofb = open(strfb.c_str(), O_RDWR);
  if (m_iofb < 0) return false ;

    // Get screen information for width and height
  if (ioctl(m_iofb, FBIOGET_VSCREENINFO, &m_vi) < 0){
    close(m_iofb);
    m_iofb = -1 ;
    return false ;
  }

  if(ioctl(m_iofb, FBIOGET_FSCREENINFO, &m_fi) < 0) {
    close(m_iofb);
    m_iofb = -1 ;
    return false ;
  }

  m_framebuffer = (unsigned char *)mmap(0, m_fi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, m_iofb, 0);
  if(m_framebuffer == MAP_FAILED) {
    close(m_iofb);
    m_iofb = -1 ;
    return false ;    
  }

  if (m_bLandscape){
    m_fbh = m_vi.xres ;
    m_fbw = m_vi.yres ;
  }else{
    m_fbw = m_vi.xres ;
    m_fbh = m_vi.yres ;
  }    
  m_fileRefresh = strrefresh ;

  return true ;
}
bool NookApp::write_to_nook(DisplayImage &img, bool bFull)
{
  unsigned char *fboff = NULL ;
  int f = 0 ;
  char refresh[2] = "1" ;

  if (m_iofb < 0 || !m_framebuffer){
    cerr << "Framebuffer not initialised\n" ;
    return false ;
  }

  if (!m_imgout) m_imgout = img.out565() ;
  else img.out565(m_imgout) ;

  if (!m_imgout){
    cerr << "Cannot allocate memory for 16bit display\n" ;
    return false ;
  }

  fboff = m_framebuffer + (m_fb_page * m_fbw * m_fbh * 2) ;
  memcpy(fboff, m_imgout, m_fbw * m_fbh * 2) ;
  m_vi.yres_virtual = m_vi.yres * 2 ;
  m_vi.yoffset = m_fb_page * m_vi.yres ;
  m_vi.activate |= FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE ;
  //cout << "ioctl - FBIOPUT_VSCREENINFO - YVirt:" << m_vi.yres_virtual << " - YOff: " << m_vi.yoffset << endl ;
  if (ioctl(m_iofb, FBIOPUT_VSCREENINFO, &m_vi) < 0) {
    return false ;
  }

  // Change the page
  m_fb_page = (m_fb_page > 0)?0:1 ;

  if (bFull){
    f = open(m_fileRefresh.c_str(), O_RDWR) ;
    if (f>=0){
      write(f,refresh, 1) ;
      close(f) ;
    }
  }

  return true ;
}

void NookApp::register_window(NookWindow &wnd)
{
  m_windows.push_back(&wnd) ;

  // Set the first window as active. This will remain the primary window
  // until changed with an alternative activate_window* call
  if (!m_pactivewindow) activate_window_next();
}

NookWindow* NookApp::get_active_window()
{
  return m_pactivewindow ;
}

bool NookApp::activate_window_next()
{
  // Check if we have windows to activate. Will fail if there are no
  // windows
  if (m_windows.size() == 0) return false;

  if (!m_pactivewindow || m_pactivewindow == m_windows.back()){
    if (m_pactivewindow){
      m_pactivewindow->deactivate() ; // ensure the window losing focus receives the deactivate
    }
    // pactivewindow is not set or the active window is the last item in the 
    // vector list
    m_pactivewindow = m_windows.front() ;
    m_pactivewindow->activate() ; // ensure the window gaining focus is activated
    return true ;
  }

  for (vector<NookWindow*>::iterator i=m_windows.begin(); i != m_windows.end(); i++){
    if (m_pactivewindow == *i){
      m_pactivewindow->deactivate() ; // ensure the window losing focus receives the deactivate
      m_pactivewindow = *(i+1) ; // Store the object pointer.
      m_pactivewindow->activate() ; // ensure the window gaining focus is activated
      return true ;
    }
  }

  // If here then the active window was not found. Cannot active the next
  m_pactivewindow = m_windows.front() ;
  m_pactivewindow->activate() ; // ensure the window gaining focus is activated

  return true ;
}

bool NookApp::activate_window(string strName)
{
  // To do. Ensure activate & deactivate are correctly called

  return true ;
}

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
  return bRet ;
}

void NookApp::dispatch_app_events()
{
  NookWindow::state s = get_active_window()->get_state() ;
  bool bFull = s == NookWindow::verydirty ;
  if (s != NookWindow::clean){
    get_active_window()->redraw();
    write_to_nook(get_active_window()->canvas, bFull);
  }
  if (m_binputkeys_enabled){
    KeyEvent key = m_inputkeys.get_next_key() ;
    if (key.valid){
      get_active_window()->key_event(key);
    }
  }
  get_active_window()->tick() ;
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

  m_fbw = m_vi.xres ;
  m_fbh = m_vi.yres ;

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
  m_windows.push_back(wnd) ;

  // Set the first window as active. This will remain the primary window
  // until changed with an alternative activate_window* call
  if (!m_pactivewindow) m_pactivewindow = &wnd ;
}

NookWindow* NookApp::get_active_window()
{
  return m_pactivewindow ;
}

bool NookApp::activate_window_next()
{
  NookWindow wnd ;
  // Check if we have windows to activate. Will fail if there are no
  // windows
  if (m_windows.size() == 0) return false;

  if (!m_pactivewindow || m_pactivewindow == &(m_windows.back())){
    // pactivewindow is not set or the active window is the last item in the 
    // vector list
    wnd = m_windows.front();
    m_pactivewindow = &wnd ;
    return true ;
  }

  for (vector<NookWindow>::iterator i=m_windows.begin(); i != m_windows.end(); i++){
    wnd = *i ;
    if (m_pactivewindow == &wnd){
      m_pactivewindow = &(*(i+1)) ; // Store the object pointer.
      return true ;
    }
  }

  // If here then the active window was not found. Cannot active the next
  m_pactivewindow = &(m_windows.front()) ;
  return true ;
}

bool NookApp::activate_window(string strName)
{
  // To do

  return true ;
}

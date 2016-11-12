#include "nookwindow.hpp"
#include <iostream>
using namespace std;

NookWindow::NookWindow()
{
  m_x_pos = 0;
  m_y_pos = 0 ;
  m_bHidden = false ;
  m_enState = dirty ; // Start off dirty for every new window
  m_width = m_height = 0 ;
  m_copy_mode = 0 ;
  m_pApp = NULL ;
  m_pParentWnd = NULL ;
}

NookApp* NookWindow::get_app()
{
  return m_pApp ;
}

bool NookWindow::create(unsigned int x, 
			unsigned int y, 
			unsigned int width, 
			unsigned int height, 
			NookWindow *pParent, 
			std::string strName, 
			NookApp *pApp)
{
  bool ret = false;

  m_strName = strName ;
  m_pApp = pApp ;
  m_x_pos = x;
  m_y_pos = y;
  m_width = width ;
  m_height = height;
  ret = canvas.createImage(width, height, 8) ;
  if (!ret) return false ;
  m_pParentWnd = pParent ;
  initialise() ; // Call the windows initialisation to finish any setup
  return true ;
}

void NookWindow::add_window(NookWindow &wnd)
{
  // Move to back of window stack
  m_children.push_back(&wnd) ;
}

NookWindow::state NookWindow::get_state()
{
  state st = m_enState ;

  if (m_enState == verydirty) return m_enState ; // Quick response if verydirty

  for (vector<NookWindow*>::iterator i=m_children.begin(); i != m_children.end(); i++){
    if (!(*i)->is_hidden()){
      state s = (*i)->get_state() ;
      if (s == verydirty) return s ; // Don't look further, found verydirty
      if (s == dirty) st = dirty ;
    }
  }

  return st; 
}

void NookWindow::invalidate_window(bool bFull)
{
  m_enState = dirty ;
  if (bFull) m_enState = verydirty ;
}

bool NookWindow::resize(unsigned int w, unsigned int h)
{
  bool ret ;
  if (w == m_width && h == m_height)
    return true ; // nothing to do

  // Create a greyscale canvas for the window.
  ret = canvas.createImage(w, h, 8) ;
  if (!ret) return false ;
  
  // Do a full window refresh to update and redraw
  // graphics.
  invalidate_window(true) ;
  
  return true ;
}

void NookWindow::redraw()
{
  if (draw()){ // parent draws before child
    for (vector<NookWindow*>::iterator i=m_children.begin(); i != m_children.end(); i++){
      if (!(*i)->is_hidden()){ // if child is hidden, skip drawing calls. 
	(*i)->redraw() ;
	// Call function in DisplayImage to copy and merge the canvas from children.
	if (!canvas.copy((*i)->canvas, (*i)->m_copy_mode, (*i)->m_x_pos, (*i)->m_y_pos)) 
	  cerr << "Canvas copy failed for window\n" ;
      }
    }
    m_enState = clean ; // just redrawn, so not dirty anymore
  }else{
    cout << "Draw returned false for window\n" ;
  }
}

void NookWindow::key_event(KeyEvent &keys)
{
  // Dispatch key stroke to all child windows
  for (vector<NookWindow*>::iterator i=m_children.begin(); i != m_children.end(); i++){
    (*i)->key_event(keys) ;
  }
}

void NookWindow::touch_event(TouchEvent &touch, unsigned int x_offset, unsigned int y_offset)
{
  // Dispatch touch events to all child windows
  for (vector<NookWindow*>::iterator i=m_children.begin(); i != m_children.end(); i++){
    (*i)->touch_event(touch, (*i)->get_x_pos()+x_offset, (*i)->get_y_pos()+y_offset) ;
  }
}

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
}

bool NookWindow::create(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
  m_x_pos = x;
  m_y_pos = y;
  m_width = width ;
  m_height = height;
  return canvas.createImage(width, height, 32) ;
}

void NookWindow::add_window(NookWindow &wnd)
{
  // Move to back of window stack
  m_children.push_back(wnd) ;
}

NookWindow::state NookWindow::get_state()
{
  state st = m_enState ;

  if (m_enState == verydirty) return m_enState ; // Quick response if verydirty

  for (vector<NookWindow>::iterator i=m_children.begin(); i != m_children.end(); i++){
    if (!i->is_hidden()){
      state s = i->get_state() ;
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

void NookWindow::redraw()
{
  if (draw()){ // parent draws before child
    for (vector<NookWindow>::iterator i=m_children.begin(); i != m_children.end(); i++){
      cout << "Iterating through child windows\n" ;
      if (!i->is_hidden()){ // if child is hidden, skip drawing calls. 
	i->redraw() ;
	// Call function in DisplayImage to copy and merge the canvas from children.
	if (!canvas.copy(i->canvas, 0, i->m_x_pos, i->m_y_pos)) 
	  cerr << "Canvas copy failed for window\n" ;
      }
    }
    m_enState = clean ; // just redrawn, so not dirty anymore
  }else{
    cout << "Draw returned false for window\n" ;
  }
}

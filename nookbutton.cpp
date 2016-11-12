#include "nookbutton.hpp"
#include <string>
#include "nookfont.hpp"
#include <iostream>

void NookButton::add_text(std::wstring strtxt, long pt_size)
{
  m_text = strtxt ;
  m_btndown = false ;
  m_txtsize = pt_size ;
}

void NookButton::initialise()
{
  m_fnt.load_font("/usr/share/fonts/ttf/LiberationSans-Regular.ttf") ;
  m_txtsize = 0 ;
}

bool NookButton::draw()
{
  const int inner_border = 2 ; // pixels
  // Draw the button outline
  canvas.setFGGrey(125);
  canvas.setBGGrey(255);
  // Draw button shadow
  canvas.eraseBackground() ;
  canvas.drawLine(1, m_height-1, m_width-1, m_height-1) ;
  canvas.drawLine(1, m_height-2, m_width-2, m_height-2) ;
  canvas.drawLine(m_width-1, m_height-1, m_width-1, 1) ;
  canvas.drawLine(m_width-2, m_height-2, m_width-2, 1) ;

  // Draw button box
  canvas.setFGGrey(0);
  canvas.drawRect(0, 0, m_width-3, m_height-3) ;
  if (m_btndown){
    // Fill the inside of the button with a grey shade
    canvas.setFGGrey(200);
    canvas.drawRect(1,1, m_width-4, m_height-4, true) ;
  }
  long pt = m_txtsize ;
  if (pt == 0){
    // One font pt is 1/72 of an inch. Nook is about 170dpi
    pt = (m_height - (inner_border *2)) * 72 / 170 ;
  }
  if (pt < 6) pt = 6 ; // going to look odd as button is so small

  DisplayImage txt = m_fnt.write_string(m_text, pt) ;
  unsigned int w = txt.get_width() ;
  unsigned int h = txt.get_height() ;
  int off_x = 0, off_y = 0 ;
  if (w > 0 && h > 0){
    // Try to centre the text on the button
    if (w < m_width) off_x = (int)((m_width - w) / 2) ;
    if (h < m_height) off_y = (int)((m_height - h) / 2);

    canvas.copy(txt, 2, off_x, off_y);
  }
  return true ;
}

void NookButton::touch_event(TouchEvent &touch, unsigned int x_offset, unsigned int y_offset)
{
  if (is_hidden()) return ; // cannot process if hidden
  if (touch.x >= (int)x_offset && touch.x <= (int)(x_offset+m_width) &&
      touch.y >= (int)y_offset && touch.y <= (int)(y_offset+m_height)){
    // Touch is within the button window area
    if (touch.touch_down != m_btndown){
      // State has changed. Call the events first
      // incase these functions need to influence the draw
      // call.
      if (touch.touch_down) button_down() ;
      else button_up() ;
      invalidate_window(false) ;
    }
    m_btndown = touch.touch_down ;
  }else{
    // Outside window area
    m_btndown = false ;
  }
}


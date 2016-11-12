#include "nookmessagebox.hpp"
#include <string>
#include <iostream>

NookMsgBox::NookMsgBox()
{
  m_response = none ;
}

void MsgBtn::button_up()
{
  if (m_pParentWnd){
    ((NookMsgBox *)m_pParentWnd)->message_response(this) ;
  }
}

void NookMsgBox::set_text(std::wstring str, long size_pt)
{
  m_message = str ;
  m_fntsize = size_pt ;
}

bool NookMsgBox::draw()
{
  const int shadow_size = 4 ; //pixels

  // Draw the window outline
  canvas.setFGGrey(125);
  canvas.setBGGrey(255);
  // Draw window shadow
  canvas.eraseBackground() ;
  canvas.drawRect(shadow_size, m_height - shadow_size - 1, m_width -1, m_height-1, true) ;
  canvas.drawRect(m_width - shadow_size - 1, shadow_size, m_width -1, m_height -1, true) ;

  // Draw window outline
  canvas.setFGGrey(0);
  canvas.drawRect(0, 0, m_width-shadow_size-2, m_height-shadow_size-2) ;
  canvas.drawRect(1, 1, m_width-shadow_size-3, m_height-shadow_size-3) ;

  DisplayImage txt = m_fnt.write_string(m_message, m_fntsize) ;
  unsigned int w = txt.get_width() ;
  unsigned int h = txt.get_height() ;
  int off_x = 0, off_y = 0 ;
  if (w > 0 && h > 0){
    // Try to centre the text on the window
    if (w < m_width) off_x = (int)((m_width - w) / 2) ;
    if (h < m_height) off_y = (int)((m_height - h) / 2);

    canvas.copy(txt, 2, off_x, off_y);
  }
  return true ;

}

void NookMsgBox::message_response(MsgBtn *pBtn)
{
  if (pBtn){
    hide(true) ;
    invalidate_window(true) ; // request redraw to hide
    if (pBtn->get_name() == "ok") m_response = ok ;
    else if (pBtn->get_name() == "cancel") m_response = cancel ;
    else std::cerr << "Unknown response from message box. Button " << pBtn->get_name() << " pressed\n" ;
  }
}

void NookMsgBox::initialise()
{
  const unsigned int btnwidth = 70 ;
  const unsigned int btnheight = 40 ;
  const unsigned int border = 10 ;
  m_fnt.load_font("/usr/share/fonts/ttf/LiberationSans-Regular.ttf") ;
  m_btnok.create(m_width - btnwidth - border,
		 m_height - btnheight - border,
		 btnwidth,
		 btnheight,
		 this,
		 "ok",
		 m_pApp) ;
  m_btnok.add_text(L"OK", 8) ;
  add_window(m_btnok) ;

  m_btncancel.create(m_width - btnwidth*2 - border*2,
		     m_height - btnheight - border,
		     btnwidth,
		     btnheight,
		     this,
		     "cancel",
		     m_pApp) ;
  m_btncancel.add_text(L"Cancel", 8) ;
  add_window(m_btncancel);
}

bool NookMsgBox::activate()
{
  m_response = none ;
  return true ;
}

#include "testwindow.hpp"
#include <iostream>
#include <time.h>
#include "nookfont.hpp"

bool WeatherIcon::draw()
{
  return true ;
}

void WeatherIcon::initialise()
{
  std::cout << "Initialising Weather Icon\n" ;
  canvas.loadJPG("weather.jpg",8) ;
  //set_window_merge(true) ;
  set_white_transparency(true) ;
}

TestNookWnd::TestNookWnd()
{
  NookWindow() ;
  m_x = 0;
  m_y = 0;
  m_last = 0 ;
  m_full = 0 ;
}

void TestNookWnd::initialise()
{
  iconwnd.create(50,50,256,256) ;
  add_window(iconwnd) ;
  m_fnt.load_font("/usr/share/fonts/ttf/LiberationSans-Regular.ttf") ;
  m_btn.create(100,600,70,30) ;
  m_btn.add_text(L"Test") ;
  add_window(m_btn) ;

  m_msgbox.create(200,300,300, 200) ;
  m_msgbox.set_text(L"Message to the world", 8) ;
  add_window(m_msgbox) ;

}

bool TestNookWnd::draw()
{
  //canvas.setFGCol(0,0,0,0);
  //canvas.setBGCol(255,255,255,255) ;
  canvas.setFGGrey(0) ;
  canvas.setBGGrey(255) ;
  canvas.eraseBackground();
  if (!canvas.drawLine(m_width/2,m_height/2, m_x, m_y))
    std::cerr << "Failed to draw line onto canvas\n" ;

  if (!canvas.drawRect(10,10,m_width-20, m_height-20))
    std::cerr << "Failed to draw rectangle onto canvas\n" ;

  //DisplayImage fntstr = m_fnt.write_string(m_fnt.get_family_name(), 16) ;
  DisplayImage fntstr = m_fnt.write_string(L"glyph justify", 16) ;
  canvas.copy(fntstr, 2, 10, 400) ;

  return true ;
}

void TestNookWnd::key_event(KeyEvent &keys)
{
  std::string strState = "up";
  if (keys.key_down) strState = "down" ;
  std::cout << "Key " << strState << ", ID: " << (int)keys.code << std::endl;
}

void TestNookWnd::touch_event(TouchEvent &touch, unsigned int x_offset, unsigned int y_offset)
{
  if (!m_msgbox.is_hidden()){
    // Message box is modal so just send touch messages there
    m_msgbox.touch_event(touch,x_offset+m_msgbox.get_x_pos(), y_offset + m_msgbox.get_y_pos()) ;
    return ;
  }
  
  NookWindow::touch_event(touch, x_offset, y_offset) ;
}

bool TestNookWnd::tick()
{
  time_t now = time(NULL) ;
  bool full = false ;

  if (now - m_last > 1){
    if (now - m_full > 20){
      full = true ;
      m_full = now ;
    }
    invalidate_window(full);
    m_last = now ;
  }

  if(m_x < m_width && m_y == 0) m_x++ ;
  else if (m_x == m_width && m_y < m_height) m_y++;
  else if (m_y == m_height && m_x > 0) m_x-- ;
  else if (m_x == 0 && m_y > 0) m_y-- ;

  return true ;
}

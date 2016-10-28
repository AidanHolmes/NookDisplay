#include "testwindow.hpp"
#include <iostream>
#include <time.h>

TestNookWnd::TestNookWnd()
{
  NookWindow() ;
  m_x = 0;
  m_y = 0;
  m_last = 0 ;
  m_full = 0 ;
}
bool TestNookWnd::draw()
{
  canvas.setFGCol(0,0,0,0);
  canvas.setBGCol(255,255,255,255) ;
  canvas.eraseBackground();
  std::cout << "Drawing to window, width: " << m_width << ", height: " << m_height << std::endl ;
  if (!canvas.drawLine(m_width/2,m_height/2, m_x, m_y))
    std::cerr << "Failed to draw line onto canvas\n" ;

  if (!canvas.drawRect(10,10,m_width-20, m_height-20))
    std::cerr << "Failed to draw rectangle onto canvas\n" ;

  return true ;
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

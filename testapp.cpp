#include "testapp.hpp"
#include "testwindow.hpp"
#include <time.h>
#include <iostream>
#include <string>

bool TestNookApp::run()
{
  const struct timespec tenthsec = {0, 100000000} ;
  std::string strfilefb("/dev/graphics/fb0") ;
  std::string strRefresh("/sys/class/graphics/fb0/epd_refresh") ;

  if (!init_display(strfilefb,strRefresh)){
    std::cerr << "Cannot open display " << strfilefb << std::endl;
    return false ;
  }

  TestNookWnd wnd ;
  std::cout << "Creating window, width: " << m_fbw << ", height: " << m_fbh << std::endl ;
  if (!wnd.create(0,0,m_fbw, m_fbh)){
    std::cerr << "Cannot create window\n";
    return false ;
  }

  register_window(wnd) ;

  while(1){
    
    NookWindow::state s = get_active_window()->get_state() ;
    bool bFull = s == NookWindow::verydirty ;
    if (s != NookWindow::clean){
      get_active_window()->redraw();
      std::cout << "write_to_nook(get_active_window()->canvas)\n" ;
      write_to_nook(get_active_window()->canvas, bFull);
    }
    
    nanosleep(&tenthsec,NULL) ;
    get_active_window()->tick() ;
  }
}


int main()
{
  TestNookApp app ;

  bool ret = app.run() ;

  std::cout << "Application exit: " << ret << std::endl ;

  return 0;
}

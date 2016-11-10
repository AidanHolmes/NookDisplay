#include "weatherapp.hpp"
#include "weatherwindow.hpp"
#include <time.h>
#include <iostream>
#include <string>

bool WeatherApp::run()
{
  const struct timespec tenthsec = {0, 100000000} ;
  std::string strfilefb("/dev/graphics/fb0") ;
  std::string strRefresh("/sys/class/graphics/fb0/epd_refresh") ;
  std::string strInputs("/dev/input") ;

  if (!init_display(strfilefb,strRefresh)){
    std::cerr << "Cannot open display " << strfilefb << std::endl;
    return false ;
  }

  if (!init_inputs(strInputs)){
    std::cerr << "Failed to initialise the input events\n" ;
    return false ;
  }

  rotate_display(true) ;

  WeatherWnd wnd ;
  if (!wnd.create(0,0,m_fbw, m_fbh)){
    std::cerr << "Cannot create main window\n";
    return false ;
  }

  register_window(wnd) ;

  while(1){
    dispatch_app_events() ;
    nanosleep(&tenthsec,NULL) ;
  }
}


int main()
{
  WeatherApp app ;

  bool ret = app.run() ;

  std::cout << "Application exit: " << ret << std::endl ;

  return 0;
}

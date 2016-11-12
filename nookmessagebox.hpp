#ifndef __NOOKWINDOW_MSGBOX_HPP
#define __NOOKWINDOW_MSGBOX_HPP

#include "nookwindow.hpp"
#include "nookbutton.hpp"
#include "nookfont.hpp"
#include <string>
#include "nookinput.hpp"

class NookMsgBox ;

class MsgBtn : public NookButton 
{
public:
  virtual void button_up();

};

class NookMsgBox : public NookWindow
{
public:
  NookMsgBox() ;
  enum msg{none, ok, cancel};
  msg get_response(){ return m_response ;};
  void message_response(MsgBtn *pBtn) ;

  void set_text(std::wstring str, long size_pt) ;
  virtual bool draw() ;
  virtual void initialise() ;
  virtual bool activate() ;

protected:
  MsgBtn m_btnok ;
  MsgBtn m_btncancel ;
  NookFont m_fnt ;
  std::wstring m_message ;
  long m_fntsize ;
  msg m_response ;
};

#endif

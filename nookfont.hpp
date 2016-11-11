#ifndef __NOOKFONT_HPP
#define __NOOKFONT_HPP

#include "displayimage.hpp"
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

class NookFont{
public:
  NookFont() ;
  ~NookFont() ;

  // Specify the font to load from file. Strfont is the path to the font
  // Returns false if the font cannot be loaded
  bool load_font(std::string strfont);

  // If the font has multiple faces then this will return > 1
  long get_num_faces();

  // Change the face ID. By default ID 0 is used in a font file
  bool set_face_id(long id) ;

  // Return the font family name and style for a face
  std::string get_family_name(){return m_familyname;} ;
  std::string get_style_name(){return m_stylename;};

  // Set screen DPI. Defaults to 170x170 for Nook.
  // This will dictate the interpretation of font size in points
  void set_dpi(long w, long h){m_dpiwidth = w; m_dpiheight = h;};

  // Specify the string to create and the size in points.
  // Returns an image of the string to merge with another image. 
  DisplayImage& write_string(std::wstring str, long size) ;

  // Reference to the offset after calling write_string. This can be used
  // to line up with other words on screen.
  long get_baseline_offset(){return m_baseline_offset;};

  bool get_string_size(std::wstring str, long size, long *width, long *yMax, long *yMin);

protected:
  long m_baseline_offset ;
  std::string m_strfont ;
  long m_faceid ;
  std::string m_familyname ;
  std::string m_stylename ;
  DisplayImage m_img;
  FT_Library m_library ;
  FT_Face m_face ;
  long m_dpiwidth ;
  long m_dpiheight ;
};

#endif

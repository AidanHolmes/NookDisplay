#include "nookfont.hpp"
#include <iostream>

NookFont::NookFont()
{
  m_faceid = 0;
  m_library = NULL ;
  m_face = NULL ;
  m_dpiwidth = m_dpiheight = 170 ; // Nook defaults
}

NookFont::~NookFont()
{
  //if (m_face) FT_Done_Face(m_face) ; // is this necessary?
  if (m_library) FT_Done_FreeType(m_library) ;
}

bool NookFont::load_font(std::string strfont)
{
  // Initialise the library if not already created
  if (!m_library){
    if (FT_Init_FreeType(&m_library) != 0){
      std::cerr << "Failed to initialise font library\n" ;
      return false ;
    }
  }

  // Set members
  m_strfont = strfont ;

  // Set font to use the first face
  // This can be changed in subsequent calls to set_face_id
  return set_face_id(0) ;
}

long NookFont::get_num_faces()
{
  if (m_face) 
    return m_face->num_faces; 
  return 0;
}

bool NookFont::set_face_id(long id)
{
  long ret = 0;

  // Check if library has been opened
  if (!m_library) return false ;

  // Check if the font library name has been set
  if (m_strfont.length() == 0) return false ;

  // Close resources for any previous face objects
  if (m_face){
    FT_Done_Face(m_face);
    m_face = NULL ;
  }

  // Select the font face
  ret = FT_New_Face(m_library, m_strfont.c_str(), id, &m_face) ;
  if (ret == FT_Err_Unknown_File_Format){
    std::cerr << "Font file " << m_strfont << " has an unknown format\n" ;
    return false ;
  }else if (ret != 0){
    std::cerr << "Loading " << m_strfont << " failed\n" ;
    return false ;
  }
  m_familyname = m_face->family_name ;
  m_stylename = m_face->style_name ;
  m_faceid = id ;

  return true ;
}

bool NookFont::get_string_size(std::wstring str, long size, long *width, long *yMax, long *yMin)
{
  long ret = 0 ;
  if (!m_face) return false ;
  FT_GlyphSlot gs ;

  *width = 0;
  *yMax = 0;
  *yMin = 0;

  ret = FT_Set_Char_Size(m_face, 0, size * 64, m_dpiwidth, m_dpiheight) ;
  if (ret != 0){
    std::cerr << "Cannot set font size to " << size << "pt \n" ;
    return false ;
  }

  for (std::wstring::iterator i = str.begin(); i != str.end(); i++){
    ret = FT_Load_Char(m_face, *i, FT_LOAD_RENDER) ;
    if (ret != 0){
      std::cerr << "Failed to load char " << (long)*i << std::endl;
      return false;
    }

    gs = m_face->glyph ;

    /*
    if (*i == ' '){
      *width += gs->advance.x / 64 ;
      continue ;
    }
    */

    if ((gs->advance.x / 64) > (gs->bitmap.width + gs->bitmap_left))
      *width += gs->advance.x / 64;
    else
      *width += gs->bitmap.width + gs->bitmap_left;

    // Work out the max height needed. Rows will be more that bitmap_top when the letter drops below the 
    // baseline. Add this difference on top of the rows to work out total bitmap height for this word. 
    if (gs->bitmap_top > *yMax) *yMax = gs->bitmap_top;
    if ((signed long)(gs->bitmap_top - gs->bitmap.rows) < *yMin) *yMin = (signed long)gs->bitmap_top - gs->bitmap.rows ;
  }

  return true;
}

DisplayImage& NookFont::write_string(std::wstring str, long size)
{
  long ret = 0;
  long canvas_width = 0, canvas_height = 0, yMax = 0, yMin = 0;
  unsigned long pen_x = 0, pen_y = 0 ;

  ret = FT_Set_Char_Size(m_face, 0, size * 64, m_dpiwidth, m_dpiheight) ;

  if (ret != 0){
    std::cerr << "Failed to set the required size: " << size << std::endl;
    m_img.createImage(0,0,0) ;
    return m_img;
  }

  if (!get_string_size(str, size, &canvas_width, &yMax, &yMin)){
    m_img.createImage(0,0,0) ;
    return m_img;
  }
  canvas_height = yMax - yMin ;

  if (!m_img.createImage(canvas_width, canvas_height, 8)){
    m_img.createImage(0,0,0) ;
    return m_img;
  }
  m_img.setBGGrey(255) ;
  m_img.eraseBackground() ;

  for (std::wstring::iterator i = str.begin(); i != str.end(); i++){
    ret = FT_Load_Char(m_face, *i, FT_LOAD_RENDER) ;
    if (ret != 0){
      std::cerr << "Failed to load char " << (long)*i << std::endl;
      m_img.createImage(0,0,0) ; // Clear the image
      return m_img;
    }

    FT_Bitmap bm = m_face->glyph->bitmap ;
    if (bm.width == 0){
      // No bitmap to print. May be a space character
      pen_x += m_face->glyph->advance.x / 64 ;
      continue ;
    }

    pen_x += m_face->glyph->bitmap_left;
    pen_y = yMax - m_face->glyph->bitmap_top ;


    if (bm.pixel_mode != FT_PIXEL_MODE_GRAY){
      // Expecting 8bit greyscale
      std::cerr << "Bitmap bit depth not 8bit grey. Pixel mode: " << bm.pixel_mode << std::endl;
      m_img.createImage(0,0,0) ; // Clear the image
      return m_img;
    }
    if (bm.num_grays != 256){
      // Not an error, but will create a compressed grey result
      std::cerr << "Num greys is not 256, characters are " << bm.num_grays << " greys per pixel\n" ;
    }
    // Assertion check
    if (bm.rows > canvas_height){
      std::cerr << "Bitmap height of " << bm.rows << " doesn't match expected height allocated of " << canvas_height << std::endl;
    }

    //std::cout << "Drawing character: " << (char)*i <<  " rows: " << bm.rows << " width: " << bm.width << " advancex: " << (m_face->glyph->advance.x /64) << std::endl;
    // Copy to the internal image and return a reference to the internal object
    for (unsigned long row=0; row < bm.rows; row++){
      for (unsigned long col=0; col < bm.width; col++){
	// Use the colour and pixel setting calls. 
	// This won't be fast but it's safer than direct writing 
	// to the image buffer.
	// Invert the values to become 0 black to 255 white
	unsigned char grey = 255 - bm.buffer[col+(row*bm.width)] ;
	m_img.setFGGrey(grey);
	m_img.setPixel(col+pen_x, row+pen_y, true) ;
      }
    }
    pen_x += bm.width;
  }

  return m_img ;
}

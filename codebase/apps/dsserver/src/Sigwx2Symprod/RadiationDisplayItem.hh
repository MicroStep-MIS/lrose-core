// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
/**
 *
 * @file RadiationDisplayItem.hh
 *
 * @class RadiationDisplayItem
 *
 * Class representing a radiation display item.
 *  
 * @date 10/21/2009
 *
 */

#ifndef RadiationDisplayItem_HH
#define RadiationDisplayItem_HH

#include <string>

#include <Spdb/Symprod.hh>

#include "BoundingBox.hh"
#include "DisplayItem.hh"

using namespace std;

/** 
 * @class RadiationDisplayItem
 */

class RadiationDisplayItem : public DisplayItem
{
 public:

  ////////////////////
  // Public methods //
  ////////////////////

  //////////////////////////////
  // Constructors/Destructors //
  //////////////////////////////

  /**
   * @brief Constructor
   *
   * @param[in] lat Cyclone latitude.
   * @param[in] lon Cyclone longitude.
   * @param[in] name Cyclone name.
   * @param[in] line_color Line color to use for rendering.
   * @param[in] line_width Line width to use for rendering.
   * @param[in] text_color Text color to use for rendering.
   * @param[in] text_bg_color Text background color to use for rendering.
   * @param[in] font_name Font name to use for rendering.
   * @param[in] font_size Font size to use for rendering.
   * @param[in] debug_level Debug level.
   */

  RadiationDisplayItem(const double lat, const double lon,
		       const string &name,
		       const string &line_color,
		       const int line_width,
		       const string &text_color,
		       const string &text_bg_color,
		       const string &font_name,
		       const int font_size,
		       const int debug_level);
  

  /**
   * @brief Destructor
   */

  virtual ~RadiationDisplayItem(void);
  

  virtual void draw(Symprod &symprod,
		    const BoundingBox &bbox,
		    const int icon_size) const;
  

protected:

  ///////////////////////
  // Protected members //
  ///////////////////////

  /**
   * @brief Name of cyclone.
   */

  string _name;
  

  /**
   * @brief Line color to use for rendering.
   */

  string _lineColor;
  

  /**
   * @brief Line width to use for rendering.
   */

  int _lineWidth;
  

  /**
   * @brief Text color to use for rendering.
   */

  string _textColor;
  

  /**
   * @brief Text background color to use for rendering.
   */

  string _textBgColor;
  

  /**
   * @brief Font name to use for rendering.
   */

  string _fontName;
  

  /**
   * @brief Font size to use for rendering.
   */

  int _fontSize;
  

};


#endif

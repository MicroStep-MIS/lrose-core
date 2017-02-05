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
#ifndef RhiWidget_HH
#define RhiWidget_HH

#include "PolarWidget.hh"

// Widget representing an RHI scan.  Beams are added to the scan as they
// are received.  The widget can be set up to display the RHI in a 90 degree
// view or in a 180 degree view.

class DLL_EXPORT RhiWidget : public PolarWidget
{

  // must include this if you use Qt signals/slots
  Q_OBJECT

 public:

  ////////////////////
  // Public methods //
  ////////////////////

  /**
   * @brief Constructor.
   *
   * @param[in] parent   Parent widget.
   * @param[in] params   TDRP parameters.
   */

  RhiWidget(QWidget* parent, 
            const PolarManager &manager,
            const Params &params,
            size_t n_fields);

  /**
   * @brief Destructor.
   */

  virtual ~RhiWidget();

  /**
   * @brief Configure for range.
   */

  virtual void configureRange(double max_range);

  /**
   * @brief Add a new beam to the display. Data for all fields and all
   *        gates are provided, as well as color maps for all fields.
   *        addBeam() will map the field values to  the correct color, and
   *        render the beam for each field in the appropriate pixamp. The
   *        existing wedge for this beam will be discarded.
   *
   * @param[in] start_angle    The starting angle for the beam.
   * @param[in] stop_angle     The ending angle for the beam.
   * @param[in] gates          The number of gates (must match beam_data vector
   *                             sizes).
   * @param[in] beam_data      Vectors of data, one for each field.
   * @param[in] maps           Colormaps, one for each field.
   */

  virtual void addBeam(const RadxRay *ray,
                       const float start_angle, const float stop_angle,
		       const std::vector< std::vector< double > > &beam_data,
		       const std::vector< DisplayField* > &fields);

signals:

  ////////////////
  // Qt signals //
  ////////////////

  /**
   * @brief Signal emitted when we have processed several beams.  This signal
   *        tells the RHI window that it can do a resize to fix the widget
   *        sizing problem that we have on startup.
   */

  void severalBeamsProcessed();
  
protected:

  ///////////////////////
  // Protected members //
  ///////////////////////

  /**
   * @brief The number of RHI beams processed so far.  I have to keep track of
   *        this so that I can automatically resize the window after processing
   *        a few beams to get rid of a problem with widget size on startup.
   */

  int _beamsProcessed;
  
  // get ray closest to click point

  virtual const RadxRay *_getClosestRay(double x_km, double y_km);

  /**
   * @brief Render the rings and grid. The current value of _ringsGridColor
   *        will be used for the color.
   *
   * @param[in] painter    Painter to use for rendering.
   */

  virtual void _drawOverlays(QPainter &painter);

  /**
   * @brief Determine a ring spacing which will give even distances, and
   *        fit a reasonable number of rings in the display.
   *
   * @return Returns the ring spacing in kilometers.
   */

  virtual void _setRingSpacing();

};

#endif

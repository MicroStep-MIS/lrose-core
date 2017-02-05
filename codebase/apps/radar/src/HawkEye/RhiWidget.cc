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

#include "RhiWidget.hh"
#include "PolarManager.hh"
#include <toolsa/toolsa_macros.h>

using namespace std;


RhiWidget::RhiWidget(QWidget* parent,
                     const PolarManager &manager,
                     const Params &params,
                     size_t n_fields) :
        PolarWidget(parent, manager, params, n_fields),
        _beamsProcessed(0)
{

  if (_params.rhi_display_180_degrees) {
    _aspectRatio = _params.rhi_aspect_ratio * 2.0;
  } else {
    _aspectRatio = _params.rhi_aspect_ratio;
  }
  _colorScaleWidth = _params.rhi_color_scale_width;

  // initialize world view

  configureRange(_params.max_range_km);

}


/*************************************************************************
 * Destructor
 */

RhiWidget::~RhiWidget()
{
}


/*************************************************************************
 * addBeam()
 */

void RhiWidget::addBeam(const RadxRay *ray,
                        const float start_angle,
			const float stop_angle,
			const std::vector< std::vector< double > > &beam_data,
			const std::vector< DisplayField* > &fields)
{
  // After processing 10 beams, send a signal to resize the widget.  This
  // gets rid of the problem with the widget frame being cut off at the
  // bottom until the user explicitly resizes the window.  I know this is
  // screwy, but I couldn't find any other way that worked.  Note that I
  // tried setting this to 3 instead of 10, and it seemed to be too soon to
  // work.  If you can find the right way to get the widgets to size correctly,
  // please change this!

  if (_beamsProcessed == 10) {
    emit severalBeamsProcessed();
  }
  
  PolarWidget::addBeam(ray, start_angle, stop_angle, beam_data, fields);

  _beamsProcessed++;

}

/*************************************************************************
 * configureRange()
 */

void RhiWidget::configureRange(double max_range)
{

  // Save the specified values

  _maxRange = max_range;

  // Set the ring spacing.  This is dependent on the value of _maxRange.

  _setRingSpacing();
  
  // set world view

  int leftMargin = 0;
  int rightMargin = 0;
  int topMargin = 0;
  int bottomMargin = 0;
  int colorScaleWidth = _params.rhi_color_scale_width;
  int axisTickLen = 7;
  int nTicksIdeal = 7;
  int textMargin = 5;

  if (_params.rhi_display_180_degrees) {
    _fullWorld.set(width(), height(),
                   leftMargin, rightMargin, topMargin, bottomMargin, colorScaleWidth,
                   -_maxRange, 0.0,
                   _maxRange, _maxRange,
                   axisTickLen, nTicksIdeal, textMargin);
    // _setWindow(QRect(-Beam::RENDER_PIXELS, -Beam::RENDER_PIXELS,
    //     	     Beam::RENDER_PIXELS * 2, Beam::RENDER_PIXELS));
  } else {
    _fullWorld.set(width(), height(),
                   leftMargin, rightMargin, topMargin, bottomMargin, colorScaleWidth,
                   0.0, 0.0,
                   _maxRange, _maxRange,
                   axisTickLen, nTicksIdeal, textMargin);
    // _setWindow(QRect(0, -Beam::RENDER_PIXELS,
    //     	     Beam::RENDER_PIXELS, Beam::RENDER_PIXELS));
  }

  _zoomWorld = _fullWorld;
  _isZoomed = false;
  _setTransform(_zoomWorld.getTransform());

  _setRingSpacing();

  // Initialize the images used for double-buffering.  For some reason,
  // the window size is incorrect at this point, but that will be corrected
  // by the system with a call to resize().

  _refreshImages();
  
}

////////////////////////////////////////////////////////////////////////////
// get ray closest to click point

const RadxRay *RhiWidget::_getClosestRay(double x_km, double y_km)

{

  double clickEl = atan2(y_km, x_km) * DEG_TO_RAD;
  
  double minDiff = 1.0e99;
  const RadxRay *closestRay = NULL;
  for (size_t ii = 0; ii < _beams.size(); ii++) {
    const RadxRay *ray = _beams[ii]->getRay();
    double rayEl = ray->getElevationDeg();
    double diff = fabs(clickEl - rayEl);
    if (diff > 180.0) {
      diff = fabs(diff - 360.0);
    }
    if (diff < minDiff) {
      closestRay = ray;
      minDiff = diff;
    }
  }

  return closestRay;

}

/*************************************************************************
 * _setRingSpacing()
 */

void RhiWidget::_setRingSpacing()
{

  double xRange = _zoomWorld.getXMaxWorld() - _zoomWorld.getXMinWorld();
  double yRange = _zoomWorld.getYMaxWorld() - _zoomWorld.getYMinWorld();
  double diagonal = sqrt(xRange * xRange + yRange * yRange);

  if (diagonal <= 1.0) {
    _ringSpacing = 0.05;
  } else if (diagonal <= 2.0) {
    _ringSpacing = 0.1;
  } else if (diagonal <= 5.0) {
    _ringSpacing = 0.2;
  } else if (diagonal <= 10.0) {
    _ringSpacing = 0.5;
  } else if (diagonal <= 20.0) {
    _ringSpacing = 1.0;
  } else if (diagonal <= 50.0) {
    _ringSpacing = 2.0;
  } else if (diagonal <= 100.0) {
    _ringSpacing = 5.0;
  } else if (diagonal <= 200.0) {
    _ringSpacing = 10.0;
  } else {
    _ringSpacing = 20.0;
  }

}


/*************************************************************************
 * _drawOverlays()
 */

void RhiWidget::_drawOverlays(QPainter &painter)
{

  // Don't try to draw rings if we haven't been configured yet or if the
  // rings or grids aren't enabled.
  
  if (!_ringsEnabled && !_gridsEnabled && !_azLinesEnabled) {
    return;
  }

  // save painter state

  painter.save();
  
  // store font
  
  QFont origFont = painter.font();
  
  // Set the painter to use the right color and font

  // painter.setWindow(_zoomWindow);
  
  painter.setPen(_gridRingsColor);

  // Draw rings

  if (_ringSpacing > 0.0 && _ringsEnabled) {

    // Draw the rings

    painter.save();
    painter.setTransform(_zoomTransform);
    double ringRange = _ringSpacing;
    while (ringRange <= _maxRange) {
      QRectF rect(-ringRange, -ringRange, ringRange * 2.0, ringRange * 2.0);
      painter.drawEllipse(rect);
      ringRange += _ringSpacing;
    }
    painter.restore();

    // Draw the labels
    
    QFont font = painter.font();
    font.setPointSizeF(_params.range_ring_label_font_size);
    painter.setFont(font);
    // painter.setWindow(0, 0, width(), height());
    
    ringRange = _ringSpacing;
    while (ringRange <= _maxRange) {
      double labelPos = ringRange * SIN_45;
      const string &labelStr = _scaledLabel.scale(ringRange);
      _zoomWorld.drawText(painter, labelStr, labelPos, labelPos, Qt::AlignCenter);
      _zoomWorld.drawText(painter, labelStr, -labelPos, labelPos, Qt::AlignCenter);
      _zoomWorld.drawText(painter, labelStr, labelPos, -labelPos, Qt::AlignCenter);
      _zoomWorld.drawText(painter, labelStr, -labelPos, -labelPos, Qt::AlignCenter);
      ringRange += _ringSpacing;
    }

  } /* endif - draw rings */
  
  // Draw the grid

  if (_ringSpacing > 0.0 && _gridsEnabled)  {

    double ringRange = _ringSpacing;
    double maxRingRange = ringRange;
    while (ringRange <= _maxRange) {

      _zoomWorld.drawLine(painter, ringRange, -_maxRange, ringRange, _maxRange);
      _zoomWorld.drawLine(painter, -ringRange, -_maxRange, -ringRange, _maxRange);
      _zoomWorld.drawLine(painter, -_maxRange, ringRange, _maxRange, ringRange);
      _zoomWorld.drawLine(painter, -_maxRange, -ringRange, _maxRange, -ringRange);
      
      maxRingRange = ringRange;
      ringRange += _ringSpacing;
    }

    _zoomWorld.setSpecifyTicks(true, -maxRingRange, _ringSpacing);

    _zoomWorld.drawAxisLeft(painter, "km", true, true, true);
    _zoomWorld.drawAxisRight(painter, "km", true, true, true);
    _zoomWorld.drawAxisTop(painter, "km", true, true, true);
    _zoomWorld.drawAxisBottom(painter, "km", true, true, true);
    
    _zoomWorld.setSpecifyTicks(false);

  }
  
  // Draw the azimuth lines

  if (_azLinesEnabled) {

    // Draw the lines along the X and Y axes

    _zoomWorld.drawLine(painter, 0, -_maxRange, 0, _maxRange);
    _zoomWorld.drawLine(painter, -_maxRange, 0, _maxRange, 0);

    // Draw the lines along the 30 degree lines

    double end_pos1 = SIN_30 * _maxRange;
    double end_pos2 = COS_30 * _maxRange;
    
    _zoomWorld.drawLine(painter, end_pos1, end_pos2, -end_pos1, -end_pos2);
    _zoomWorld.drawLine(painter, end_pos2, end_pos1, -end_pos2, -end_pos1);
    _zoomWorld.drawLine(painter, -end_pos1, end_pos2, end_pos1, -end_pos2);
    _zoomWorld.drawLine(painter, end_pos2, -end_pos1, -end_pos2, end_pos1);

  }
  
  // click point cross hairs
  
  if (_pointClicked) {

    int startX = _mouseReleaseX - _params.click_cross_size / 2;
    int endX = _mouseReleaseX + _params.click_cross_size / 2;
    int startY = _mouseReleaseY - _params.click_cross_size / 2;
    int endY = _mouseReleaseY + _params.click_cross_size / 2;

    painter.drawLine(startX, _mouseReleaseY, endX, _mouseReleaseY);
    painter.drawLine(_mouseReleaseX, startY, _mouseReleaseX, endY);

  }

  // reset painter state
  
  painter.restore();

  // draw the color scale

  const DisplayField &field = _manager.getSelectedField();
  _zoomWorld.drawColorScale(field.getColorMap(), painter);
  
}

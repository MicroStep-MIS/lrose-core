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
/////////////////////////////////////////////////////////////
// PolarManager.hh
//
// PolarManager object
//
// Mike Dixon, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Oct 2014
//
///////////////////////////////////////////////////////////////
//
// PolarManager manages polar rendering - PPIs and RHIs
//
///////////////////////////////////////////////////////////////

#ifndef PolarManager_HH
#define PolarManager_HH

#include <string>
#include <vector>

#include "Args.hh"
#include "Params.hh"
#include "DisplayManager.hh"
#include <QMainWindow>
#include <euclid/SunPosn.hh>
#include <Radx/RadxRay.hh>

class QApplication;
class QActionGroup;
class QButtonGroup;
class QRadioButton;
class QFrame;
class QDialog;
class QLabel;
class QGroupBox;
class QGridLayout;
class QDateTime;
class QDateTimeEdit;
class ColorBar;
class DisplayField;
class PpiWidget;
class RhiWidget;
class RhiWindow;
class Reader;
class RadxPlatform;

class PolarManager : public DisplayManager {
  
  Q_OBJECT

public:

  // constructor
  
  PolarManager(const Params &params,
               Reader *reader,
               const vector<DisplayField *> &fields,
               bool haveFilteredFields);
  
  // destructor
  
  ~PolarManager();

  // run 

  int run(QApplication &app);

  // enable the zoom button - called by PolarWidget

  void enableZoomButton() const;

  // inner class for ray locator

  class RayLoc {
  public:
    int startIndex;
    int endIndex;
    bool master;
    bool active;
    const RadxRay *ray;
    RayLoc() {
      master = false;
      active = false;
      ray = NULL;
    }
    void clear() {
      if (master && ray) {
        if (ray->removeClient() == 0) {
          delete ray;
        }
      }
      ray = NULL;
      master = false;
      active = false;
    }
  };

signals:

private:

  // beam geometry
  
  int _nGates;
  double _maxRange;

  // ray locator
  // we use a locator with data every 1/100th degree
  // around the 360 degree circle

  static const int RAY_LOC_RES = 10;
  static const int RAY_LOC_N = 4000;
  static const int RAY_LOC_OFFSET = 300;
 
  RayLoc* _ppiRayLoc; // for use, allows negative indices at north line
  RayLoc* _ppiRays; // for new and delete

  RayLoc* _rhiRayLoc; // for use, allows negative indices at north line
  RayLoc* _rhiRays; // for new and delete

  // input data
  
  RadxTime _readerRayTime;
  RadxVol _vol;
  int _sweepNum;
  bool _movingToTopSweep;
  bool _keepSweepNumber;

  // windows

  QFrame *_ppiFrame;
  PpiWidget *_ppi;

  QFrame *_rhiFrame;
  RhiWindow *_rhiWindow;
  RhiWidget *_rhi;
  bool _rhiWindowDisplayed;
  bool _rhiMode;
  
  // azimuths for current ray

  double _prevAz;
  double _prevEl;
  double _startAz;
  double _endAz;

  // times for rays

  RadxTime _plotStartTime;
  RadxTime _plotEndTime;
  RadxTime _prevRayTime;
  
  // menus

  QMenu *_fileMenu;
  QMenu *_viewMenu;
  QMenu *_helpMenu;

  // actions

  QAction *_ringsAct;
  QAction *_gridsAct;
  QAction *_azLinesAct;
  QAction *_showRhiAct;

  QAction *_timeControllerAct;
  QAction *_saveImageAct;

  // time controller settings dialog
  
  QDialog *_timeControllerDialog;
  QLabel *_timeControllerInfo;

  QLineEdit *_archiveScanIntervalEdit;
  double _archiveScanIntervalSecs;

  QLineEdit *_nArchiveScansEdit;
  int _nArchiveScans;

  bool _archiveMode;
  bool _archiveRetrievalPending;
  QRadioButton *_realtimeModeButton;
  QRadioButton *_archiveModeButton;

  QGroupBox *_archiveTimeBox;
  QDateTimeEdit *_archiveStartTimeEdit;
  RadxTime _archiveStartTime;
  int _archiveMarginSecs;

  QLabel *_archiveEndTimeEcho;
  RadxTime _archiveEndTime;
  
  RadxTime _archivePeriodStartTime;
  RadxTime _archivePeriodEndTime;

  // saving images in real time mode

  RadxTime _imagesScheduledTime;

  // override event handling

  virtual void timerEvent (QTimerEvent * event);
  virtual void resizeEvent (QResizeEvent * event);
  virtual void keyPressEvent(QKeyEvent* event);

  // set top bar

  virtual void _setTitleBar(const string &radarName);
  
  // local methods

  void _setupWindows();
  void _createRhiWindow();
  void _createActions();
  void _createMenus();

  // data retrieval

  void _handleRealtimeData(QTimerEvent * event);
  void _handleArchiveData(QTimerEvent * event);
  int _getArchiveData();
  void _plotArchiveData();
  void _setupVolRead(RadxFile &file);

  // draw beam

  void _handleRay(RadxPlatform &platform, RadxRay *ray);

  // ray handling for display

  void _storeRayLoc(const RadxRay *ray, const double az,
		    const double beam_width, RayLoc *ray_loc);
  void _clearRayOverlap(const int start_index, const int end_index,
			RayLoc *ray_loc);

  // images

  void _createRealtimeImageFiles();
  void _createArchiveImageFiles();
  void _createImageFiles();
  void _saveImageToFile(bool interactive);

  // override howto

  void _howto();

private slots:

  //////////////
  // Qt slots //
  //////////////

  // override

  virtual void _freeze();
  virtual void _unzoom();
  virtual void _refresh();
  virtual void _changeField(int fieldId, bool guiMode = true);

  // local

  void _ppiLocationClicked(double xkm, double ykm,
                           const RadxRay *closestRay);
  void _rhiLocationClicked(double xkm, double ykm,
                           const RadxRay *closestRay);
  void _locationClicked(double xkm, double ykm,
                        RayLoc *ray_loc, const RadxRay *closestRay);

  void _cancelTimeControllerChanges();

  // archive mode
  
  void _setDataRetrievalMode();
  void _setArchiveScanConfig();
  void _resetArchiveScanConfigToDefault();
  void _setStartTimeFromGui(const QDateTime &datetime1);
  void _setGuiFromStartTime();
  void _setArchiveStartTimeToDefault();
  void _setArchiveStartTime(const RadxTime &rtime);
  void _computeArchiveEndTime();
  void _goBack1();
  void _goFwd1();
  void _goBackNScans();
  void _goFwdNScans();

  void _performArchiveRetrieval();

  // time controller

  void _createTimeControllerDialog();
  void _refreshTimeControllerDialog();
  void _showTimeControllerDialog();

};

#endif



//////////////////////////////////////////////////////////////
//
// Header file for StnSelFormClassUI
//
//    This file is generated by RapidApp 1.2
//
//    This class implements the user interface portion of a class
//    Normally it is not used directly.
//    Instead the subclass, StnSelFormClass is instantiated
//
//    To extend or alter the behavior of this class, you should
//    modify the StnSelFormClass files
//
//    Restrict changes to those sections between
//    the "//--- Start/End editable code block" markers
//
//    This will allow RapidApp to integrate changes more easily
//
//    This class is a ViewKit user interface "component".
//    For more information on how components are used, see the
//    "ViewKit Programmers' Manual", and the RapidApp
//    User's Guide.
//
//////////////////////////////////////////////////////////////
#ifndef STNSELFORMCLASSUI_H
#define STNSELFORMCLASSUI_H
#include <Vk/VkComponent.h>


//---- Start editable code block: headers and declarations


//---- End editable code block: headers and declarations


class StnSelFormClassUI : public VkComponent
{ 

  public:

    StnSelFormClassUI ( const char *, Widget );
    StnSelFormClassUI ( const char * );
    ~StnSelFormClassUI();
    void create ( Widget );
    const char *  className();

    //---- Start editable code block: StnSelFormClass public


    //---- End editable code block: StnSelFormClass public


  protected:


    // Widgets created by this class

    Widget  _knowntoggle1;
    Widget  _label30;
    Widget  _scrolledWindow3;
    Widget  _selStnscrolledList2;
    Widget  _stnSelFormClassUI;


    // These virtual functions are called from the private callbacks (below)
    // Intended to be overriden in derived classes to define actions

    virtual void browseselCB ( Widget, XtPointer );
    virtual void defaultCB ( Widget, XtPointer );
    virtual void uifKnownOnlyChanged ( Widget, XtPointer );

    //---- Start editable code block: StnSelFormClass protected


    //---- End editable code block: StnSelFormClass protected


  private: 

    // Array of default resources

    static String      _defaultStnSelFormClassUIResources[];


    // Callbacks to interface with Motif

    static void browseselCBCallback ( Widget, XtPointer, XtPointer );
    static void defaultCBCallback ( Widget, XtPointer, XtPointer );
    static void uifKnownOnlyChangedCallback ( Widget, XtPointer, XtPointer );

    //---- Start editable code block: StnSelFormClass private


    //---- End editable code block: StnSelFormClass private
};
//---- Start editable code block: End of generated code


//---- End editable code block: End of generated code

#endif


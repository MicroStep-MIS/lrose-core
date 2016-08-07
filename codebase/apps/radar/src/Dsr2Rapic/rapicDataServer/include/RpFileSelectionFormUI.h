
//////////////////////////////////////////////////////////////
//
// Header file for RpFileSelectionFormUI
//
//    This file is generated by RapidApp 1.2
//
//    This class implements the user interface portion of a class
//    Normally it is not used directly.
//    Instead the subclass, RpFileSelectionForm is instantiated
//
//    To extend or alter the behavior of this class, you should
//    modify the RpFileSelectionForm files
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
#ifndef RPFILESELECTIONFORMUI_H
#define RPFILESELECTIONFORMUI_H
#include <Vk/VkComponent.h>


//---- Start editable code block: headers and declarations


//---- End editable code block: headers and declarations


class RpFileSelectionFormUI : public VkComponent
{ 

  public:

    RpFileSelectionFormUI ( const char *, Widget );
    RpFileSelectionFormUI ( const char * );
    ~RpFileSelectionFormUI();
    void create ( Widget );
    const char *  className();

    //---- Start editable code block: RpFileSelectionForm public


    //---- End editable code block: RpFileSelectionForm public


  protected:


    // Widgets created by this class

    Widget  _applyButton;
    Widget  _cancelButton;
    Widget  _editselectionlabel;
    Widget  _fileNamescrolledList;
    Widget  _fileselectlabel;
    Widget  _fileselecttextfield;
    Widget  _oKButton;
    Widget  _rpFileSelectionForm;
    Widget  _scrolledWindow1;


    // These virtual functions are called from the private callbacks (below)
    // Intended to be overriden in derived classes to define actions

    virtual void doApplyButton ( Widget, XtPointer );
    virtual void doCancelButton ( Widget, XtPointer );
    virtual void doOKButton ( Widget, XtPointer );
    virtual void listactioncallback ( Widget, XtPointer );
    virtual void listbrowsecallback ( Widget, XtPointer );
    virtual void listsinglecallback ( Widget, XtPointer );
    virtual void textactioncallback ( Widget, XtPointer );

    //---- Start editable code block: RpFileSelectionForm protected


    //---- End editable code block: RpFileSelectionForm protected


  private: 

    // Array of default resources

    static String      _defaultRpFileSelectionFormUIResources[];


    // Callbacks to interface with Motif

    static void doApplyButtonCallback ( Widget, XtPointer, XtPointer );
    static void doCancelButtonCallback ( Widget, XtPointer, XtPointer );
    static void doOKButtonCallback ( Widget, XtPointer, XtPointer );
    static void listactioncallbackCallback ( Widget, XtPointer, XtPointer );
    static void listbrowsecallbackCallback ( Widget, XtPointer, XtPointer );
    static void listsinglecallbackCallback ( Widget, XtPointer, XtPointer );
    static void textactioncallbackCallback ( Widget, XtPointer, XtPointer );

    //---- Start editable code block: RpFileSelectionForm private


    //---- End editable code block: RpFileSelectionForm private
};
//---- Start editable code block: End of generated code


//---- End editable code block: End of generated code

#endif

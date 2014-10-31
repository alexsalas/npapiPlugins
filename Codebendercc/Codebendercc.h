/**********************************************************\

  Auto-generated Codebendercc.h

  This file contains the auto-generated main plugin object
  implementation for the Codebender.cc project

\**********************************************************/
#ifndef H_CodebenderccPLUGIN
#define H_CodebenderccPLUGIN

#include "PluginWindow.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/AttachedEvent.h"

#include "PluginCore.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

extern vector <string> vectorPortsInUseList;
extern vector< string >::const_iterator iter;
extern boost::mutex mtxPort;
bool AddtoPortList(string port);
void RemovePortFromList(string port);
bool CanBeUsed(string port);

extern boost::mutex mtxAvrdudeFlag;
extern bool isAvrdudeRunning;

extern std::map<int, FB::JSAPIWeakPtr> apiMap;
void getJSAPIObjectById(int id);
FB_FORWARD_PTR(Codebendercc)
class Codebendercc : public FB::PluginCore
{

public:
    static void StaticInitialize();
    static void StaticDeinitialize();

public:
    Codebendercc();
    virtual ~Codebendercc();

public:
    void onPluginReady();
    void shutdown();
    virtual FB::JSAPIPtr createJSAPI();
    // If you want your plugin to always be windowless, set this to true
    // If you want your plugin to be optionally windowless based on the
    // value of the "windowless" param tag, remove this method or return
    // FB::PluginCore::isWindowless()
    virtual bool isWindowless() { return false; }

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *);
    virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *);
    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/

};



#endif


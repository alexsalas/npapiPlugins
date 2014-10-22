/**********************************************************\

  Auto-generated Codebendercc.cpp

  This file contains the auto-generated main plugin object
  implementation for the Codebender.cc project

\**********************************************************/

#include "CodebenderccAPI.h"

#include "Codebendercc.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> 
 
using namespace std;


///////////////////////////////////////////////////////////////////////////////
/// @fn Codebendercc::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void Codebendercc::StaticInitialize()
{
    // Place one-time initialization stuff here; As of FireBreath 1.4 this should only
    // be called once per process
}

///////////////////////////////////////////////////////////////////////////////
/// @fn Codebendercc::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void Codebendercc::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
    // always be called just before the plugin library is unloaded
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Codebendercc constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
Codebendercc::Codebendercc()
{
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Codebendercc destructor.
///////////////////////////////////////////////////////////////////////////////
Codebendercc::~Codebendercc()
{
    // This is optional, but if you reset m_api (the shared_ptr to your JSAPI
    // root object) and tell the host to free the retained JSAPI objects then
    // unless you are holding another shared_ptr reference to your JSAPI object
    // they will be released here.
    releaseRootJSAPI();
    m_host->freeRetainedObjects();
}

void Codebendercc::onPluginReady()
{
    // When this is called, the BrowserHost is attached, the JSAPI object is
    // created, and we are ready to interact with the page and such.  The
    // PluginWindow may or may not have already fire the AttachedEvent at
    // this point.
}

void Codebendercc::shutdown()
{
    // This will be called when it is time for the plugin to shut down;
    // any threads or anything else that may hold a shared_ptr to this
    // object should be released here so that this object can be safely
    // destroyed. This is the last point that shared_from_this and weak_ptr
    // references to this object will be valid
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Creates an instance of the JSAPI object that provides your main
///         Javascript interface.
///
/// Note that m_host is your BrowserHost and shared_ptr returns a
/// FB::PluginCorePtr, which can be used to provide a
/// boost::weak_ptr<Codebendercc> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr Codebendercc::createJSAPI()
{
    // m_host is the BrowserHost
    return boost::make_shared<CodebenderccAPI>(FB::ptr_cast<Codebendercc>(shared_from_this()), m_host);
}

bool Codebendercc::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool Codebendercc::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool Codebendercc::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
bool Codebendercc::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *)
{
    // The window is attached; act appropriately
    return false;
}

bool Codebendercc::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
    // The window is about to be detached; act appropriately
    return false;
}

vector <string> vectorPortsInUseList;
vector< string >::const_iterator iter;
boost::mutex mtxPort;

std::map<int, FB::JSAPIWeakPtr> apiMap;

void getJSAPIObjectById(int id) {
    apiMap.erase(id);
}
bool CanBeUsed(string port)
{
    if (vectorPortsInUseList.empty()){
        return true;
    }else{
        if (std::find(vectorPortsInUseList.begin(), vectorPortsInUseList.end(), port) != vectorPortsInUseList.end()){
            return false;
        }else{
            return true;
        }
    }
}

bool AddtoPortList(string port)
{
    #ifdef _WIN32
        if (port.find("\\\\.\\") == string::npos) {
            port = "\\\\.\\" + port;
        }
    #endif
    if (vectorPortsInUseList.empty()){
        mtxPort.lock();
        vectorPortsInUseList.push_back(port);
        mtxPort.unlock();
        return true;
    }else{
        if (std::find(vectorPortsInUseList.begin(), vectorPortsInUseList.end(), port) != vectorPortsInUseList.end()){
            return false;
        }else{
            mtxPort.lock();
            vectorPortsInUseList.push_back(port);
            mtxPort.unlock();
            return true;
        }
    }
}

void RemovePortFromList(string port)
{
    #ifdef _WIN32
        if (port.find("\\\\.\\") == string::npos) {
            port = "\\\\.\\" + port;
        }
    #endif
    mtxPort.lock();
    if (std::find(vectorPortsInUseList.begin(), vectorPortsInUseList.end(), port) != vectorPortsInUseList.end())
        vectorPortsInUseList.erase(std::remove(vectorPortsInUseList.begin(), vectorPortsInUseList.end(), port));
    mtxPort.unlock();
}

boost::mutex mtxAvrdudeFlag;
bool isAvrdudeRunning = false;



#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for Codebender.cc
#
#\**********************************************************/

set(PLUGIN_NAME "Codebendercc")
set(PLUGIN_PREFIX "COD")
set(COMPANY_NAME "codebendercc")

# ActiveX constants:
set(FBTYPELIB_NAME CodebenderccLib)
set(FBTYPELIB_DESC "Codebendercc 1.0 Type Library")
set(IFBControl_DESC "Codebendercc Control Interface")
set(FBControl_DESC "Codebendercc Control Class")
set(IFBComJavascriptObject_DESC "Codebendercc IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "Codebendercc ComJavascriptObject Class")
set(IFBComEventSource_DESC "Codebendercc IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID f224104d-21ae-5e17-8685-5aac9d48a5b3)
set(IFBControl_GUID 06bb4f54-3d31-5e02-8e77-7ee9dad4ded7)
set(FBControl_GUID b640bb42-591a-51dc-bc7a-cb0f2812296a)
set(IFBComJavascriptObject_GUID 0d7df234-7b39-5504-96b0-b1bb5a66aa45)
set(FBComJavascriptObject_GUID 4c6788d4-0ddd-5eed-9c05-2ff8c3bc5b42)
set(IFBComEventSource_GUID bce32abc-e7f1-549e-a12e-8fdf6b2fb722)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "codebendercc.Codebendercc")
set(MOZILLA_PLUGINID "codebender.cc/Codebendercc")

# strings
set(FBSTRING_CompanyName "codebendercc")
set(FBSTRING_FileDescription "Plugin for the Codebender.cc Arduino - Browser Communication")
set(FBSTRING_PLUGIN_VERSION "1.6.0.8")
set(FBSTRING_LegalCopyright "Copyright 2014 codebender.cc")
if(APPLE)
set(FBSTRING_PluginFileName "${PLUGIN_NAME}")
else(NOT APPLE)
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
endif(APPLE)
set(FBSTRING_ProductName "Codebender.cc")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "Codebendercc")
set(FBSTRING_MIMEType "application/x-codebendercc")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 0)
set(FBMAC_USE_COCOA 0)
set(FBMAC_USE_COREGRAPHICS 0)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)


add_boost_library(regex)
add_boost_library(thread)
add_boost_library(system)
add_boost_library(filesystem)


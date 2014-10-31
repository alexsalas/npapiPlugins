/*! \mainpage Codebender Browser NPAPI Plugin Documentation Pages
 *
 * \section intro_sec Introduction
 *
 * These pages give a better understanding to the people that want to work on or use the Codebender Browser plugin.\n  
 *
 * \section install_sec Installation
 * The generated library file should be placed in the suitable directory for each platform/browser.
 * 
 * \author Dimitrios Amaxilatis {d.amaxilatis at gmail}
 * 
 * \todo fix the error messages add full-stops and rest
 * @see http://codebender.cc
 * @see http://www.firebreath.org/display/documentation/FireBreath+Home
 */


#ifdef _WIN32
    #define MAX_KEY_LENGTH 255
    #define WIN32_LEAN_AND_MEAN 
    #include <SDKDDKVer.h>
    #include "dirent.h"
    #include <windows.h>
    #include <tchar.h>
    #include <Shellapi.h>
    #include <Tchar.h>
    #include <Iepmapi.h>
    #include <stdio.h>
    #include <iostream>
    #include <fstream>
    #include <string>
    #include <stdlib.h>
    #include <string.h>
    #include <tchar.h>
    #include <tlhelp32.h>
    #include <comdef.h>
#else
    #include <dirent.h>
    #include <sys/file.h>
    #include <sys/syscall.h>
    #include <unistd.h>
    #include <stddef.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <signal.h>
    #include <stdio.h>
    #include <string.h>
    #include <errno.h>
#endif

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp> 
#include <boost/date_time.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/array.hpp>
#include <fstream>
#include <vector>
#include <time.h>
#include <sys/stat.h>
#include <algorithm>
#include <numeric>
#include <sys/types.h>
#include <fcntl.h>

#include "BrowserHost.h"
#include "Codebendercc.h"
#include "DOM/Window.h"
#include "DOM/Document.h"
#include "global/config.h"
#include "JSAPIAuto.h"
#include "JSObject.h"
#include "src/3rdParty/boost/boost/thread/detail/thread.hpp"
#include "SimpleStreamHelper.h"
#include "variant_list.h"
#include "SimpleSerial.h"
#include <exception> 
/**
 * Wjwwod serial library. 
 * https://github.com/wjwwood/serial
 **/
#include "serial/include/serial/serial.h"
#include "json/writer.h"
#include "json/reader.h"
#include "json/elements.h"
using namespace serial;
using namespace json;

#ifdef __APPLE__
#import <Security/Security.h>
#endif

#ifndef H_CodebenderccAPI
#define H_CodebenderccAPI

class CodebenderccAPI : public FB::JSAPIAuto {
public:
    ////////////////////////////////////////////////////////////////////////////
    ///
    ////////////////////////////////////////////////////////////////////////////
#define MSG_LEONARD_AUTORESET "Trying to auto-reset your device. If it does not reset automatically, please reset your device manually!"
#define THREAD_INTERRUPTED -1234
    /**
     * Constructor for your JSAPI object.
     * You should register your methods, properties, and events
     * that should be accessible to Javascript from here.
     * 
     * @see FB::JSAPIAuto::registerMethod
     * @see FB::JSAPIAuto::registerProperty
     * @see FB::JSAPIAuto::registerEvent
     * @param plugin
     * @param host
     */
    CodebenderccAPI(const CodebenderccPtr& plugin, const FB::BrowserHostPtr& host) :
    m_plugin(plugin), m_host(host), io() {

        // Retrieve a reference to the DOM Window
        FB::DOM::WindowPtr window = m_host->getDOMWindow();

        //Register all JS callbacks
        registerMethod("probeUSB", make_method(this, &CodebenderccAPI::probeUSB));
        registerMethod("getPorts", make_method(this, &CodebenderccAPI::getPorts));
        registerMethod("availablePorts", make_method(this, &CodebenderccAPI::availablePorts));
        registerMethod("download", make_method(this, &CodebenderccAPI::download));
        registerMethod("flash", make_method(this, &CodebenderccAPI::flash));
        registerMethod("flashWithProgrammer", make_method(this, &CodebenderccAPI::flashWithProgrammer));
        registerMethod("flashBootloader", make_method(this, &CodebenderccAPI::flashBootloader));
        registerMethod("saveToHex", make_method(this, &CodebenderccAPI::saveToHex));
        
        registerMethod("openPort", make_method(this, &CodebenderccAPI::openPort));
        registerMethod("serialRead", make_method(this, &CodebenderccAPI::serialRead));
        registerMethod("disconnect", make_method(this, &CodebenderccAPI::disconnect));
        registerMethod("setCallback", make_method(this, &CodebenderccAPI::setCallback));
        registerMethod("setErrorCallback", make_method(this, &CodebenderccAPI::setErrorCallback));
        registerMethod("serialWrite", make_method(this, &CodebenderccAPI::serialWrite));
        registerMethod("enableDebug", make_method(this, &CodebenderccAPI::enableDebug));
        registerMethod("disableDebug", make_method(this, &CodebenderccAPI::disableDebug));
        registerMethod("getFlashResult", make_method(this, &CodebenderccAPI::getFlashResult));
        registerMethod("serialMonitorSetStatus", make_method(this, &CodebenderccAPI::serialMonitorSetStatus));
        registerMethod("closeTab", make_method(this, &CodebenderccAPI::closeTab));
        registerMethod("init", make_method(this, &CodebenderccAPI::init));
        registerMethod("printMap", make_method(this, &CodebenderccAPI::printMap));
        registerMethod("deleteMap", make_method(this, &CodebenderccAPI::deleteMap));
        //Register all JS read-only properties
        registerProperty("version", make_property(this, &CodebenderccAPI::get_version));
        registerProperty("command", make_property(this, &CodebenderccAPI::getLastCommand));
        registerProperty("retVal", make_property(this, &CodebenderccAPI::getRetVal));
        registerProperty("instance_id", make_property(this, &CodebenderccAPI::get_instId));

        instance_id = counter++;
        serialMonitorStatus=false;
        debug_ = false;
        lastPortCount=0;
        probeFlag=false;
        usedPort="";

        thr=NULL;

        //Returns the string name of the current operating system. 
        std::string os = getPlugin().get()->getOS();
        //Returns the path and filename of the current plugin module. 
        path = getPlugin().get()->getFSPath();
        //Finds the last / and returns the rest of the path.
        path = path.substr(0, path.find_last_of("/\\") + 1);

        std::string arch = "32";
        #ifdef __x86_64
                arch = "64";
        #endif

        // paths to files
        
#ifdef _WIN32
    current_dir = getShortPaths(path);
    std::wstring wchdir(current_dir);
        if (os == "Windows")
        {
            // WINDOWS
            // .exe for windows
            // no path is appended to avrdude.exe or its config file, since both are used in a batch file
            // that executes the avrdude command
            avrdude = "avrdude.exe";
            avrdudeConf = os + ".avrdude.conf";

            batchFile = wchdir + L"command.bat";
            binFile = wchdir + L"file.bin";
            hexFile = wchdir + L"bootloader.hex";
            outfile = wchdir + L"out";
            debugFilename = wchdir + L"debugging.txt";
        }
#else
        binFile = path + "file.bin";
        hexFile = path + "bootloader.hex";
        outfile = path + "out";
        debugFilename = path + "debugging.txt";
            if (os == "X11") {
                // LINUX
                avrdude = path + os + "." + arch + ".avrdude";
                avrdudeConf = path + os + "." + arch + ".avrdude.conf";
                             } 
            else {
                // MAC
                path = path + "../../";
                avrdude = path + os + ".avrdude";
                avrdudeConf = path + os + ".avrdude.conf";
                    #ifdef __APPLE__        //added to avoid messing up compilation process
                        binFile = path + "file.bin";
                        outfile = path + "out";
                    #endif
                 }
#endif

        _retVal = 9999;
    }

    /**
     * Destructor.
     */
    virtual ~CodebenderccAPI() {
    };

    /**
     * Returns a reference to the Plugin Object.
     * Gets a reference to the plugin that was passed in when the object
     * was created.  If the plugin has already been released then this
     * will throw a FB::script_error that will be translated into a
     * javascript exception in the page.
     * @return a reference to the plugin.
     */
    CodebenderccPtr getPlugin();

    /**
     * Gets the plugin version.
     * @return the version string.
     */
    std::string get_version();

    /**
     * Used to Download Avrdude Objects.
     * @deprecated
     * This function is now available only for compatibility. No functionality here.
     * 
     * @return "deprecated".
     */
    FB::variant download();

    /**
     * Used to flash a binary file to a connected Arduino/device.
     * The flash operation is initiated in a @b new @b thread
     * @param device The port of the device as a string. @see validate_device
     * @param code A base64 encoded string of the binary file to be flashed to the device. 
     * @param maxsize The maximum size of a binary file that can be flashed to the specific Arduino/other Board.
     * @param protocol The protocol to be used for Avrdude.
     * @param disable_flushing Specify whether the serial port buffer should be flushed or not.
     * @param speed The baudrate to be used with Avrdude.
     * @param mcu The mcu to be used with Avrdude.
     * @param cback A callback used to report the flash result.
     * @return 0 if the flash process is started. Anything else is an error value.
     */
    FB::variant flash(const std::string& device,
                      const std::string& code,
                      const std::string& maxsize,
                      const std::string& protocol,
                      const std::string& disable_flushing,
                      const std::string& speed,
                      const std::string& mcu,
                      const FB::JSObjectPtr & cback);

    /**
     * Alternative flash function. Used to flash a binary using a programmer.
     * The flash operation is initiated in a new @b thread
     * @param device The port of the device as a string. @see validate_device
     * @param code A base64 encoded string of the binary file to be flashed to the device connected to the programmer. 
     * @param maxsize The maximum size of a binary file that can be flashed to the specific device.
     * @param programmerProtocol The protocol to be used for Avrdude.
     * @param programmerCommunication The communication method used when programming the device. 
     * @param programmerSpeed The baudrate to be used with Avrdude, when the programmer imlements serial communication.
     * @param programmerForce Specifies whether or not -F flag should be used for Avrdude.
     * @param programmerDelay The delay applied when using parallel programmer.
     * @param mcu The mcu to be used with Avrdude.
     * @param cback A callback used to report the flash result.
     * @return 0 if the flash process is started. Anything else is an error value.
     */
    FB::variant flashWithProgrammer(const std::string& device,
                                    const std::string& code,
                                    const std::string& maxsize,
                                    const std::string& programmerProtocol,
                                    const std::string& programmerCommunication,
                                    const std::string& programmerSpeed,
                                    const std::string& programmerForce,
                                    const std::string& programmerDelay,
                                    const std::string& mcu,
                                    const FB::JSObjectPtr & cback);

    /**
     * Bootloader burn function. Used to burn a hex bootloader file to the device.
     * The burn operation is initiated in a new @b thread
     * @param hexContent the content of the hex bootloader file
     * @param device The port of the device as a string.
     * @param programmerProtocol The protocol to be used for Avrdude.
     * @param programmerCommunication The communication method used when programming the device. 
     * @param programmerSpeed The baudrate to be used with Avrdude, when the programmer imlements serial communication.
     * @param programmerForce Specifies whether or not -F flag should be used for Avrdude.
     * @param programmerDelay The delay applied when using parallel programmer.
     * @param bootloaderHighFuses bootloader parameter used to erase the device.
     * @param bootloaderLowFuses bootloader parameter used to erase the device.
     * @param bootloaderExtendedFuses bootloader parameter used to erase the device.
     * @param bootloaderUnlockBits bootloader parameter used to erase the device.
     * @param bootloaderLockBits bootloader parameter used to upload the bootloader.
     * @param mcu The mcu to be used with Avrdude.
     * @param cback A callback used to report the flash result.
     * @return 0 if the flash process is started. Anything else is an error value.
     */
     FB::variant flashBootloader(const std::string& device,
                                const std::string& programmerProtocol,
                                const std::string& programmerCommunication,
                                const std::string& programmerSpeed,
                                const std::string& programmerForce,
                                const std::string& programmerDelay,
                                const std::string& bootloaderHighFuses,
                                const std::string& bootloaderLowFuses,
                                const std::string& bootloaderExtendedFuses,
                                const std::string& bootloaderUnlockBits,
                                const std::string& bootloaderLockBits,
                                const std::string& mcu,
                                const FB::JSObjectPtr & cback);

    /**
     * When on Windows OS, finds all available usb ports.
     * @return a comma separated list of the detected devices.
     */
#ifdef _WIN32
    std::string CodebenderccAPI::QueryKey(HKEY hKey);
#endif

    /**
     * Checks for all available USB Arduino devices.
     * @return a comma separated list of the detected devices.
     */
    std::string probeUSB();

    /**
     * Checks for all available USB Arduino devices.
     * @return a json array with the detected devices (port, description, hardware).
     */
    std::string getPorts();

    std::string availablePorts();

    /**
     * Returns the last avrdude's output.
     * @return the output recorded from avrdude.
     */
    std::string getFlashResult();

    /**
     * The last avrdude command executed.
     * @return the last avrdude command executed.
     */
    FB::variant getLastCommand();

    FB::variant getRetVal() {
        return _retVal;
    }

    /**
     * Sets a callback to notify the web page about changes.
     * A number of operational changes and exceptions are reported from here:
     * @param callback a javaScript callback function.
     * @return true if the callback is set, false in any error.
     */
    bool setCallback(const FB::JSObjectPtr &callback);

    /**
     * Sets an error callback to notify the web page about unknown exceptions
     *
     * @param callback a javaScript callback function.
     * @return true if the callback is set, false in any error.
     */
    bool setErrorCallback(const FB::JSObjectPtr &error_callback);

    /**
     * Opens a Serial Port and Reads input from it. 
     * The connection is maintained in a @b new @b thread.
     * 
     * @see serialRead
     * @param port The port of the device as a string. @see validate_device
     * @param baudrate The baudrate to use for the connection as a string.
     * @param callback A callback function to report all characters read from the Serial Port.
     * @param valHandCallback A callback function to display exception thrown by openPort.
     * @return true if connection was attempted, false otherwise.
     */
    bool serialRead(const std::string &port, const std::string &baudrate, const FB::JSObjectPtr &callback, const FB::JSObjectPtr &valHandCallback);
    /**
     * Write String to the open serial port.
     * @param the string to write.
     */
    void serialWrite(const std::string &);
    /**
     * Disconnects from serial port.
     * @return 1 when disconnected. Any other value is an error value. 
     */
    FB::variant disconnect();
    /**
     * Checks for the correct permissions under linux.
     * @param port The port to check for. @see validate_device
     * @return the group needed (if needed) for the user to be added.
     */
    FB::variant checkPermissions(const std::string &port);

    /**
     * Creates an instance of the serial library and opens it.
     **/
    int openPort(const std::string &port, const unsigned int &baudrate, bool flushFlag,const std::string &functionPrefix);

    /**
     * Closes the current port connection.
     **/
    void closePort(bool flushFlag);

    /**
     * Functions to check and enable or disable debugging.
     **/
    void enableDebug(int debugLevel);

    void disableDebug();

    bool checkDebug();

    /**
     * Functions to check and modify serial monitor status.
     **/
    void serialMonitorSetStatus ();

    bool checkSerialMonitorStatus();

    /**
     * Functions that print debugging messages depending on the level.
     **/
    void debugMessage(const char * messageDebug, int minimumLevel); 

    void debugMessageProbe(const char * messageDebug, int minimumLevel);

    /**
     * Debugging variables.
     **/
    std::ofstream debugFile;
#ifdef _WIN32
    std::wstring debugFilename;
#else
    std::string debugFilename;
#endif

    int lastPortCount;
    bool probeFlag;
    bool debug_;
    int currentLevel;
    std::string usedPort;
    boost::mutex serialMonitor;
    bool serialMonitorStatus;

    /**
     * Function that interrupts and joins a thread when a tab is closed.
     **/
    void closeTab();

    boost::thread* thr;

    static int counter;
    int instance_id;
    int get_instId() ;
    void init() ;
    void printMap();
    void deleteMap();
    bool JSAPIWeakPtrExists();

private:

    /**
     * Validate Device Name.
     * Devices in Linux are /dev/tty{USB??,ACM??}.
     * Devices in Mac are /dev/cu.{}.
     * Devices in Windows are COM??.
     * 
     * @param input the device string.
     * @return true if valid, false else.
     */
    bool validate_device(const std::string &input);

    /**
     * Exec system command.
     * @param filename
     * @return 
     */
    std::string exec(const char * cmd);

    /**
     * Check if the file exists.
     * @param filename the file to check.
     * @return true if it exists.
     */
    bool fileExists(const std::string& filename);
    /**
     * 
     * @param source
     * @param target
     * @param targetlen
     * @return 
     */
    size_t base64_decode(const char *source, unsigned char *target, size_t targetlen);
    /**
     * 
     * @param base64char
     * @return 
     */
    int _base64_char_value(char base64char);
    /**
     * 
     * @param quadruple
     * @param result
     * @return 
     */
    int _base64_decode_triple(char quadruple[4], unsigned char *result);
    /**
     * Saves a Binary file to disk.
     * @param data the data to write.
     * @param size the size of the buffer.
     */
    void saveToBin(unsigned char *, size_t);

    /**
     * Saves a bootloader hex file to disk.
     * @param bootloaderContent the contents of the hex file to write.
     */
    void saveToHex(const std::string& bootloaderContent);

    /**
      * Detects which port was added or removed.
      */
    void detectNewPort(const std::string& portString);

    /**
      * Validate hex string number.
      * @param input the string hex number to validate
      * $return true if the string is a hex value, false else.
      **/
    bool validate_hex(const std::string &);

    /**
     * Validate a number string.
     * @param input the string to validate.
     * @return true if the string contains a number, false else.
     */
    bool validate_number(const std::string &);
    /**
     * Validate a base64 file string.
     * @param input the code for the Arduino in base64 format.
     * @return true if the string is valid, false else.
     */
    bool validate_code(const std::string &);
    /**
     * Validate a string that contains digits and characters.
     * @param input the string to validate.
     * @return true if valid,false else.
     */
    bool validate_charnum(const std::string &);

    /**
     * Sends a notification to the default callback.
     */
    void notify(const std::string &message);

    /**
     * Sends a error notification to the default callback.
     */
    void error_notify(const std::string &message, int optionalWarningFlag = 0);

    void Invoke(const FB::JSObjectPtr& flash_callback, const int &value);

    void Invoke(const FB::JSObjectPtr& flash_callback, const string &value);
    /**
      * Validates the input and creates a map with the parameters of the programmer.
      * Returns zero upon success. All other return codes represent validation errors.
      * @param device port to be used when uploading with programmer or burning bootloader.
      * @param programmerProtocol the protocol used by the selected programmer.
      * @param programmerSpeed the speed specified by the programmer protocol to be used with avrdude. 
      * @param programmerCommunication the communication method specified by the programmer protocol.
      * @param programmerForce the flage specifying whether or not to used -F flag with avrdude
      * @param programmerDelay the delay applied when writing data to the device.
      * @param mcu the device microcontroller unit.
      * @param programmerData a map containing the programmer parameters.
      **/
    int programmerPrefs(const std::string& port, 
        const std::string& programmerProtocol, 
        const std::string& programmerSpeed, 
        const std::string& programmerCommunication, 
        const std::string& programmerForce, 
        const std::string& programmerDelay, 
        const std::string& mcu, 
        std::map<std::string, std::string>& programmerData);

    /**
      * Validates the bootloader parameters and creates a map containing them.
      * Returns zero upon success. All other return codes represent validation errors.
      * @param lowFuses
      * @param highFuses
      * @param extendedFuses
      * @param unLockBits
      * @param lockBits
      * @param bootloaderData
      **/
    int bootloaderPrefs(const std::string& lowFuses, 
        const std::string& highFuses, 
        const std::string& extendedFuses, 
        const std::string& unLockBits, 
        const std::string& lockBits, 
        std::map<std::string, std::string>& bootloaderData);
    
    /**
      * Creates the first part of the avrdude command for uploading with
      * a programmer or flashing a bootloader. 
      * @param programmerData a map including the settings of the selected programmer.
      */
    const std::string setProgrammerCommand(std::map<std::string, std::string>& data);

    /**
      * Executes a command with avrdude.
      * When on Widnows, the functions creates a batch file and then 
      * calls CodebenderccAPI::execAvrdude function to execute the batch file,
      * else calls CodebenderccAPI::unixExecAvrdude function.
      * If appendFlag is true append the output of the avrdude command to the output file, if one exists.
      */
    int runAvrdude(const std::string& command, bool append);

    int unixExecAvrdude(const std::string &unixExecCommand, bool unixAppendFlag);

    long filesize(const char *filename);

    /**
     * 
     * @param device The port of the device as a string. @see validate_device
     * @param code A base64 encoded string of the binary file to be flashed to the device. 
     * @param maxsize The maximum size of a binary file that can be flashed to the specific Arduino/other Board.
     * @param protocol The protocol to be used for Avrdude.
     * @param disable_flushing Specify whether the serial port buffer should be flushed or not.
     * @param speed The baudrate to be used with Avrdude.
     * @param mcu The mcu to be used with Avrdude.
     * @param cback A callback used to report the flash result.
     */
    void doflash(const std::string&,
                const std::string&,
                const std::string&,
                const std::string&,
                const std::string&,
                const std::string&,
                const std::string&,
                const FB::JSObjectPtr &);

    int resetLeonardo(std::string& fdevice);

    void LeonardoSketchControl(const std::string& fdevice);
    std::string createCommand(const std::string& fdevice, 
                              const std::string& protocol, 
                              const std::string& speed, 
                              const std::string& mcu);

    /**
     * 
     * @param 
     * @param 
     * @param 
     * @param 
     * @param 
     * @param 
     */
    void doflashWithProgrammer(const std::string&,
                              const std::string&,
                              const std::string&,
                              std::map<std::string, std::string>&,
                              const std::string&,
                              const FB::JSObjectPtr &);

    /**
      *
      * @param
      * @param
      * @param
      * @param
      * @param
      */
    void doflashBootloader(const std::string&,
                           std::map<std::string, std::string>&,
                           std::map<std::string, std::string>&,
                           const std::string&,
                           const FB::JSObjectPtr &);

    /**
     * 
     * @param 
     * @param 
     * @param 
     * @param
     */
    void serialReader(const std::string &, const unsigned int &, const FB::JSObjectPtr &, const FB::JSObjectPtr &);

    int PortNotOpenedException(std::string);

    int SerialException(std::string);

    int invalid_argument(std::string);

    int IOException(std::string);

    int checkIfIsDigit(std::string);

    int GetTag(std::string);

    int GetNumberBetween(std::string);

    /**
     * Creates a separate process to run the avrdude command when on Windows OS.
     * Thus, one can get both the output of the command (the output that would originally be printed on a 
     * command prompt) and the value returned by the process.
     * If appendFlag is true, append the output to the existing output file.
     * @return a code (integer) that indicates whether the command was successful or not
     */
    int winExecAvrdude(const std::wstring & cmd, bool appendFlag);

    /**
     * Kills avrdude process if it is still running on Windows OS.
     **/

    #ifdef _WIN32 
        void winKillAvrdude(DWORD dwPid);
    #endif

    /** Flushes the contents of the serial port and toggles the DTR and RTS signal values. **/
    int flushBuffer(const std::string &);

    /**
     */
    CodebenderccWeakPtr m_plugin;
    /**
     */
    FB::BrowserHostPtr m_host;
    /**
     */
    std::vector<std::string> portsList;
    /**
     */

    #ifdef _WIN32
        std::string avrdude, avrdudeConf;
        std::wstring binFile, hexFile, outfile, batchFile;
        const wchar_t * current_dir;
    #else
        std::string avrdude, avrdudeConf, binFile, hexFile, outfile;
    #endif
    /**
     */
    std::string lastcommand;
    int _retVal;
    /**
    */

    FB::JSObjectPtr callback_;
    FB::JSObjectPtr error_callback_;
    
    /**
     * Serial library and timeout objects
     **/
    serial::Serial serialPort;
    Timeout portTimeout;

    boost::asio::io_service io;
    boost::array<char, 1 > buf;
    std::string path;

    void delay(int duration) {
#ifdef _WIN32
        Sleep(duration);
#else
        usleep(duration * 1000);
#endif
    }

#ifdef _WIN32

    const wchar_t * getShortPaths(std::string  &longpath) {

        std::wstring wstrpath = FB::utf8_to_wstring(longpath);
        long length = 0;
        LPCWSTR szlongpath = wstrpath.c_str();
        TCHAR* buffer = NULL;

        length = GetShortPathName(szlongpath, NULL, 0);

        if (length != 0) {
            buffer = new TCHAR[length];
            length = GetShortPathName(szlongpath, buffer, length);
            if (length != 0){
                return buffer;
            }
        }
        return L"";
    }
#endif

    DIR *opendir(const char *name);

    struct dirent *readdir(DIR *dirp);

    void closedir(DIR *dirp);

    FILE *fopen(const char *path, const char *mode);

    FILE *freopen(const char *path, const char *mode, FILE *stream);

    size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

    char *fgets(char *s, int size, FILE *stream);

    void fclose(FILE *fp);

#ifndef _WIN32
    FILE *popen(const char *command, const char *type);

    void pclose(FILE *stream);

    int stat(const char *path, struct stat *buf);

    pid_t fork(void);

    int execvp(const char *file, char *const argv[]);

    pid_t waitpid(pid_t pid, int *status, int options);
#endif

    int system(const char *command);

#ifdef _WIN32
    LONG RegQueryInfoKey(HKEY hKey,
                         LPTSTR lpClass,
                         LPDWORD lpcClass,
                         LPDWORD lpReserved,
                         LPDWORD lpcSubKeys,
                         LPDWORD lpcMaxSubKeyLen,
                         LPDWORD lpcMaxClassLen,
                         LPDWORD lpcValues,
                         LPDWORD lpcMaxValueNameLen,
                         LPDWORD lpcMaxValueLen,
                         LPDWORD lpcbSecurityDescriptor,
                         PFILETIME lpftLastWriteTime);

    LONG RegEnumValue(HKEY hKey,
                      DWORD dwIndex,
                      LPTSTR lpValueName,
                      LPDWORD lpcchValueName,
                      LPDWORD lpReserved,
                      LPDWORD lpType,
                      LPBYTE lpData,
                      LPDWORD lpcbData);

    LONG RegQueryValueEx(HKEY hKey,
                         LPCTSTR lpValueName,
                         LPDWORD lpReserved,
                         LPDWORD lpType,
                         LPBYTE lpData,
                         LPDWORD lpcbData);

    LONG RegOpenKeyEx(HKEY hKey,
                      LPCTSTR lpSubKey,
                      DWORD ulOptions,
                      REGSAM samDesired,
                      PHKEY phkResult);

    LONG RegCloseKey(HKEY hKey);

    HANDLE CreateFile(LPCTSTR lpFileName,
                      DWORD dwDesiredAccess,
                      DWORD dwShareMode,
                      LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                      DWORD dwCreationDisposition,
                      DWORD dwFlagsAndAttributes,
                      HANDLE hTemplateFile);

    BOOL CreateProcess(LPCTSTR lpApplicationName,
                       LPTSTR lpCommandLine,
                       LPSECURITY_ATTRIBUTES lpProcessAttributes,
                       LPSECURITY_ATTRIBUTES lpThreadAttributes,
                       BOOL bInheritHandles,
                       DWORD dwCreationFlags,
                       LPVOID lpEnvironment,
                       LPCTSTR lpCurrentDirectory,
                       LPSTARTUPINFO lpStartupInfo,
                       LPPROCESS_INFORMATION lpProcessInformation);

    DWORD WaitForSingleObject(HANDLE hHandle,
                              DWORD dwMilliseconds);


    BOOL GetExitCodeProcess(HANDLE hProcess,
                            LPDWORD lpExitCode);

    BOOL TerminateProcess(HANDLE hProcess,
                          UINT uExitCode);

    BOOL CloseHandle(HANDLE hObject);

    HANDLE OpenProcess(DWORD dwDesiredAccess,
                       BOOL bInheritHandle,
                       DWORD dwProcessId);

    HANDLE CreateToolhelp32Snapshot(DWORD dwFlags,
                                    DWORD th32ProcessID);

    BOOL Process32First(HANDLE hSnapshot,
                        LPPROCESSENTRY32 lppe);

    BOOL Process32Next(HANDLE hSnapshot,
                       LPPROCESSENTRY32 lppe);

#endif

};

#endif // H_CodebenderccAPI


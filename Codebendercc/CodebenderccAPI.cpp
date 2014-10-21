/**********************************************************\

  Auto-generated CodebenderccAPI.cpp

\**********************************************************/
#include "CodebenderccAPI.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////deprecated////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

FB::variant CodebenderccAPI::download() {
    return "deprecated";
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////public//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int CodebenderccAPI::openPort(const std::string &port,
                              const unsigned int &baudrate,
                              bool flushFlag,
                              const std::string &functionPrefix) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::openPort",3);
    std::string device;
    device = port;
    #ifdef _WIN32
        device = "\\\\.\\" + port;
    #endif
    if(flushFlag || AddtoPortList(device)){ //Check if device is used by someone else
        try{
            usedPort=device;
            if (serialPort.isOpen() == false){
                uint32_t TotalTimeoutConstantDivider = ((baudrate/9)/100 == 0)? 1 : (baudrate/9)/100;
                uint32_t readTotalTimeoutConst = 1000/TotalTimeoutConstantDivider;
                portTimeout = Timeout(10, readTotalTimeoutConst, 0, 0, 10);
                serialPort.setPort(device);                  //set port name
                serialPort.setBaudrate(baudrate);            //set port baudrate
                serialPort.setTimeout(portTimeout);          //set the read/write timeout of the port
                serialPort.open();                           //open the port
                serialPort.setDTR(true);                     //set Data Transfer signal, needed for Arduino leonardo
                serialPort.setRTS(false);                    //set Request to Send signal to false, needed for Arduino leonardo
            }
        }catch(serial::PortNotOpenedException& pno){
            CodebenderccAPI::debugMessage(pno.what(),2);
            std::string err_mess = boost::lexical_cast<std::string>(pno.what());
            std::string result = functionPrefix + err_mess;
            if (functionPrefix.find("flushBuffer")!=std::string::npos) {error_notify(result, 1);}
            else {error_notify(result);}
            if(!flushFlag)
                RemovePortFromList(usedPort);
            int return_value= CodebenderccAPI::PortNotOpenedException(err_mess);
            return return_value;
        }
        catch(serial::SerialException& se){
            CodebenderccAPI::debugMessage(se.what(),2);
            std::string err_mess = boost::lexical_cast<std::string>(se.what());
            std::string result = functionPrefix + err_mess;
            if (functionPrefix.find("flushBuffer")!=std::string::npos) {error_notify(result, 1);}
            else {error_notify(result);}
            if(!flushFlag)
                RemovePortFromList(usedPort);
            int return_value= CodebenderccAPI::SerialException(err_mess);
            return return_value;
        }
        catch(std::invalid_argument& inv_arg){
            CodebenderccAPI::debugMessage(inv_arg.what(),2);
            std::string err_mess = boost::lexical_cast<std::string>(inv_arg.what());
            std::string result = functionPrefix + err_mess;
            if (functionPrefix.find("flushBuffer")!=std::string::npos) {error_notify(result, 1);}
            else {error_notify(result);}
            if(!flushFlag)
                RemovePortFromList(usedPort);
            int return_value= CodebenderccAPI::invalid_argument(err_mess);
            return return_value;
        }
        catch(serial::IOException& IOe){
            CodebenderccAPI::debugMessage(IOe.what(),2);
            std::string err_mess = boost::lexical_cast<std::string>(IOe.what());
            std::string result = functionPrefix + err_mess;
            if (functionPrefix.find("flushBuffer")!=std::string::npos) {error_notify(result, 1);}
            else {error_notify(result);}
            if(!flushFlag)
                RemovePortFromList(usedPort);
            #ifndef _WIN32
                if (result.find("IO Exception (16)")!=std::string::npos)
                    return -55;
                else if (result.find("IO Exception (13)")!=std::string::npos)
                    return -56;
                else if (result.find("IO Exception (2)")!=std::string::npos)
                    return -57;
                else{
                    int return_value= CodebenderccAPI::IOException(err_mess);
                    return return_value;}
            #endif
            #ifdef _WIN32
                if (result.find("Can't open device,")!=std::string::npos)
                    return -56;
                else if (result.find("Specified port,")!=std::string::npos)
                    return -57;
                else{
                    int return_value= CodebenderccAPI::IOException(err_mess);
                return return_value;}
                #endif
        }
    }else{
        CodebenderccAPI::debugMessage("CodebenderccAPI::Port is already in use.",3);
        return -22;
    }
    CodebenderccAPI::debugMessage("CodebenderccAPI::openPort ended",3);
    return 1;
} catch (...) {
    error_notify("CodebenderccAPI::openPort() threw an unknown exception");
    return -54;
}

void CodebenderccAPI::closePort(bool flushFlag) try {
	CodebenderccAPI::debugMessage("CodebenderccAPI::closePort",3);
	if(serialPort.isOpen())
		serialPort.close();
	if(!flushFlag)
		RemovePortFromList(usedPort);
	CodebenderccAPI::debugMessage("CodebenderccAPI::closePort ended",3);
}catch(serial::IOException& IOe){
	CodebenderccAPI::debugMessage(IOe.what(),2);
	std::string err_mess = boost::lexical_cast<std::string>(IOe.what());
	error_notify(err_mess);
	if(!flushFlag)
		RemovePortFromList(usedPort);
}catch (...){
	CodebenderccAPI::debugMessage("CodebenderccAPI::closePort exception",2);
    error_notify("CodebenderccAPI::closePort() threw an unknown exception");
    if(!flushFlag)
		RemovePortFromList(usedPort);
}

#ifdef _WIN32
std::string CodebenderccAPI::probeUSB() try {
    CodebenderccAPI::debugMessageProbe("CodebenderccAPI::probeUSB probing USB ports",3);
    std::string ports ="";
    HKEY hKey;
    /*
    * Open the registry key where serial port key-value pairs are stored.
    */
    if( CodebenderccAPI::RegOpenKeyEx(HKEY_LOCAL_MACHINE,                         // The name of the registry key handle is always the same.
                                    TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM\\"),    // The same applies to the subkey, since we are looking for serial ports only.
                                    0,
                                    KEY_READ,                                     // Set the access rights, before reading the key contents.
                                    &hKey) == ERROR_SUCCESS                       // Set the variable that retrieves the open key handle.
        ){
        ports.append(CodebenderccAPI::QueryKey(hKey));                            // Call QueryKey function to retrieve the available ports.

        if(lastPortCount!=ports.length()){
            lastPortCount=ports.length();
            CodebenderccAPI::detectNewPort(ports);
        }
        CodebenderccAPI::RegCloseKey(hKey);                                       // Need to close the key handle after the task is completed.
    }
    return ports;
} catch (...) {
    error_notify("[Windows] CodebenderccAPI::probeUSB() threw an unknown exception");
    return "";
}
#else
std::string CodebenderccAPI::probeUSB() try {
    CodebenderccAPI::debugMessageProbe("CodebenderccAPI::probeUSB probing USB ports",3);
    DIR *dp;
    std::string dirs = "";
    struct dirent *ep;
    dp = CodebenderccAPI::opendir("/dev/");
    if (dp != NULL) {
        while (ep = CodebenderccAPI::readdir(dp)) {
            //UNIX ARDUINO PORTS
            if (boost::contains(ep->d_name, "ttyACM") || boost::contains(ep->d_name, "ttyUSB")) {
                dirs += "/dev/";
                dirs += ep->d_name;
                dirs += ",";
            } else if (boost::contains(ep->d_name, "cu.")) {
                dirs += "/dev/";
                dirs += ep->d_name;
                dirs += ",";
            }
        }
        CodebenderccAPI::closedir(dp);
    } else{
        CodebenderccAPI::debugMessage("CodebenderccAPI::probeUSB could not open directory",2);
    }
    if((size_t)lastPortCount!=dirs.length()){
        lastPortCount=dirs.length();
        CodebenderccAPI::detectNewPort(dirs);
        return dirs;
    }
    return dirs;
} catch (...) {
    error_notify("CodebenderccAPI::probeUSB() threw an unknown exception");
    return "";
}
#endif

std::string CodebenderccAPI::getPorts() try {

    CodebenderccAPI::debugMessageProbe("CodebenderccAPI::getPorts",3);

    std::vector<serial::PortInfo> devices = serial::list_ports();
    json::Array json_object_array;
    json::Object json_object;
    std::string json_string;
    
    for(unsigned int i = 0; i < devices.size(); i++){
    
        std::string port=devices.at(i).port;
        std::string description=devices.at(i).description;
        std::string hardware_id = devices.at(i).hardware_id;

        json_object["port"]=String(port);
        json_object["description"]=String(description);
        json_object["hardware"]=String(hardware_id);
         
        json_object_array.Insert(json_object);
        }

    std::stringstream json_ss;
    Writer::Write(json_object_array, json_ss);
    json_string = json_ss.str();
    return json_string;

} catch (...) {
    error_notify("CodebenderccAPI::getPorts() threw an unknown exception");
    return "";
}    

std::string CodebenderccAPI::availablePorts() try {
    CodebenderccAPI::debugMessageProbe("CodebenderccAPI::availablePorts()",3);

    std::vector<serial::PortInfo> devices = serial::list_ports();
    std::string ports;
    std::vector<std::string> portsVector;

    for(unsigned int i = 0; i < devices.size(); i++){
        std::string port=devices.at(i).port;
        portsVector.push_back(port);
        }

    std::vector<std::string>::iterator it=portsVector.begin();
    for(;it!=portsVector.end();it++){
        ports.append(*it);
        if (it != (portsVector.end()-1))
            ports.append(",");
    }
    return ports;
} catch (...) {
    error_notify("CodebenderccAPI::availablePorts() threw an unknown exception");
    return "";
}

#ifdef _WIN32

int CodebenderccAPI::winExecAvrdude(const std::wstring & command,
                                    bool appendFlag)
try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::winExecAvrdude",3);
    DWORD dwExitCode = -1;
    DWORD APPEND;
    DWORD CREATE;
    if (appendFlag){
        APPEND = FILE_APPEND_DATA;
        CREATE = OPEN_ALWAYS;
    }
    else{
        APPEND = GENERIC_WRITE;
        CREATE = CREATE_ALWAYS;
    }
    std::string strResult;    // Contains the result of the child process created below.
    BOOL success;

    // Create security attributes to create pipe.
    SECURITY_ATTRIBUTES sa    = {sizeof(SECURITY_ATTRIBUTES)} ;
    sa.bInheritHandle        = TRUE;                                    // Set the bInheritHandle flag so pipe handles are inherited by child process. Required.
    sa.lpSecurityDescriptor = NULL;                                     // Specify a security descriptor. Required.

    STARTUPINFO si    = { sizeof(STARTUPINFO) };                        // Specify the necessary parameters for child process.
    si.dwFlags        = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;    // STARTF_USESTDHANDLES is required.
    si.wShowWindow    = SW_HIDE;                                        // Prevent cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.

    PROCESS_INFORMATION pi    = { 0 };                                  // Create an empty process information struct. Needed to get the return value of the command.

    try{
        boost::this_thread::interruption_point();

        // Create a file handle pointing to the output file, in order to capture the output.
        HANDLE fh = CodebenderccAPI::CreateFile(&outfile[0],
                                                APPEND,
                                                FILE_SHARE_READ|FILE_SHARE_WRITE,
                                                &sa,
                                                CREATE,
                                                FILE_FLAG_SEQUENTIAL_SCAN,
                                                0);
        // Bind the stdinput, stdoutput and stderror to the output file in order to capture all the output of the command.
        si.hStdOutput = fh;
        si.hStdError = fh;
        si.hStdInput = fh;

        boost::this_thread::interruption_point();

        // Create the child process. The command simply executes the contents of the batch file, which is the actual command.
        success = CodebenderccAPI::CreateProcess(NULL,
                                                (LPWSTR)command.c_str(),            // command line
                                                NULL,                               // process security attributes
                                                NULL,                               // primary thread security attributes
                                                TRUE,                               // Inherit pipe handles from parent process
                                                CREATE_NEW_CONSOLE,                 // creation flags
                                                NULL,                               // use parent's environment
                                                current_dir,                        // use the plugin's directory
                                                &si,                                // __in, STARTUPINFO pointer
                                                &pi);                               // __out, receives PROCESS_INFORMATION

        if (! success){
            CodebenderccAPI::debugMessage("Failed to create child process.", 1);
            return -200;
        }

        DWORD dwFileSizeOld=0;
        DWORD dwFileSizeNew=0;
        int counter = 0;

        do {
            CodebenderccAPI::GetExitCodeProcess(pi.hProcess, &dwExitCode);

            boost::this_thread::interruption_point();

            // Check if created process is still active.
            if(dwExitCode==STILL_ACTIVE){
                delay(10);
                dwFileSizeNew=GetFileSize( fh, NULL );
                // Check if output file changes.
                if (dwFileSizeOld == dwFileSizeNew)
                    counter++;
                else{
                    dwFileSizeOld = dwFileSizeNew;
                    counter = 0;}
            }else
                break;
        }while(counter <= 2000);

        if(dwExitCode == STILL_ACTIVE){
            // Kill child & main process if it is still running.
            DWORD dwPid = GetProcessId(pi.hProcess);
            CodebenderccAPI::winKillAvrdude(dwPid);
            dwExitCode = -204;
        }

        CodebenderccAPI::CloseHandle(fh);
        // CreateProcess docs specify that these must be closed.
        CodebenderccAPI::CloseHandle( pi.hProcess );
        CodebenderccAPI::CloseHandle( pi.hThread );
        CodebenderccAPI::debugMessage("CodebenderccAPI::winExecAvrdude ended",3);
        return dwExitCode;
    }catch(boost::thread_interrupted&){
        DWORD dwPid = GetProcessId(pi.hProcess);
        CodebenderccAPI::winKillAvrdude(dwPid);
        return -1234;
    }
} catch (...) {
    error_notify("CodebenderccAPI::winExecAvrdude() threw an unknown exception");
    return 0;
}
#endif

#ifdef _WIN32
void CodebenderccAPI::winKillAvrdude( DWORD dwPid) try {
CodebenderccAPI::debugMessage("CodebenderccAPI::winKillAvrdude",3);

PROCESSENTRY32 pe;
memset(&pe, 0, sizeof(PROCESSENTRY32));
pe.dwSize = sizeof(PROCESSENTRY32);

HANDLE hSnap = CodebenderccAPI::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (CodebenderccAPI::Process32First(hSnap, &pe))
    {
    BOOL bContinue = TRUE;

    // kill child processes
    while (bContinue){
        // only kill child processes
        if (pe.th32ParentProcessID == dwPid){
            HANDLE hChildProc = CodebenderccAPI::OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
            if (hChildProc){
                CodebenderccAPI::TerminateProcess(hChildProc, 1);
                CodebenderccAPI::CloseHandle(hChildProc);
            }
            else
                return;
        }

        bContinue = CodebenderccAPI::Process32Next(hSnap, &pe);
    }

    // kill the main process
    HANDLE hProc = CodebenderccAPI::OpenProcess(PROCESS_TERMINATE, FALSE, dwPid);

    if (hProc){
        CodebenderccAPI::TerminateProcess(hProc, 1);
        CodebenderccAPI::CloseHandle(hProc);
    }

    CodebenderccAPI::debugMessage("CodebenderccAPI::winKillAvrdude ended",3);
    }
CodebenderccAPI::CloseHandle(hSnap);
} catch (...) {
    error_notify("CodebenderccAPI::winKillAvrdude() threw an unknown exception");
    return;
}
#endif

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////PRIVATE////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int CodebenderccAPI::counter(0);

int CodebenderccAPI::get_instId() {
    return instance_id;
}

void CodebenderccAPI::init() {
    apiMap[instance_id] = shared_from_this();
}

void CodebenderccAPI::printMap() {
    std::map<int, FB::JSAPIWeakPtr>::iterator p;
    p= apiMap.begin();

    for(; p!=apiMap.end(); ++p){
        std::cout << "int is: " << p->first << endl;
        FB::JSAPIPtr cur(p->second.lock());
        if (cur) {
            std::cout << "FB::JSAPIWeakPtr is valid: " << cur.get() << endl;
        }
        else {
            std::cout << "FB::JSAPIWeakPtr is invalid" << endl;
        }
    }
}

void CodebenderccAPI::deleteMap() {
    int my_id =  CodebenderccAPI::get_instId();
    getJSAPIObjectById(my_id);
}

bool CodebenderccAPI::JSAPIWeakPtrExists() {
    int my_id =  CodebenderccAPI::get_instId();
    std::map<int, FB::JSAPIWeakPtr>::iterator p;
    p= apiMap.begin();
    for(; p!=apiMap.end(); ++p){
        if (p->first==my_id){
            FB::JSAPIPtr cur(p->second.lock());
            if (cur){
                return true; }
            else{
                return false;}
        }
    }
}

void CodebenderccAPI::doflash(const std::string& device,
                              const std::string& code,
                              const std::string& maxsize,
                              const std::string& protocol,
                              const std::string& disable_flushing,
                              const std::string& speed,
                              const std::string& mcu,
                              const FB::JSObjectPtr & flash_callback) try {

    CodebenderccAPI::debugMessage("CodebenderccAPI::doflash",3);

    mtxAvrdudeFlag.lock();
    if(isAvrdudeRunning){
        CodebenderccAPI::Invoke(flash_callback, -23);
        mtxAvrdudeFlag.unlock();
        return;}
    isAvrdudeRunning=true;
    mtxAvrdudeFlag.unlock();

    if(mcu == "atmega32u4" || AddtoPortList(device)){
        #ifndef _WIN32
            chmod(avrdude.c_str(), S_IRWXU);
        #endif
        unsigned char buffer [150000];
        size_t size = base64_decode(code.c_str(), buffer, 150000);
        saveToBin(buffer, size);
        std::string fdevice = device;
        std::string initialDevice = device;
        std::string fprotocol = protocol;
        std::string fspeed = speed;
        std::string fmcu = mcu;
        try {
            try{
                boost::this_thread::interruption_point();
                if (mcu == "atmega32u4") {
                    notify(MSG_LEONARD_AUTORESET);
                    int LeonardoPortStatus = CodebenderccAPI::resetLeonardo(fdevice);
                    if (LeonardoPortStatus!=1){
                        CodebenderccAPI::Invoke(flash_callback, LeonardoPortStatus);
                        isAvrdudeRunning=false;
                        return;
                    }
                    AddtoPortList(fdevice);
                }

                boost::this_thread::interruption_point();

                /* Flush the buffer of the serial port before uploading,
                unless the board definition specifies not to do so.*/

                int finalRetVal=0;
                if (disable_flushing == "" || disable_flushing == "false"){
                    int flushBufferRetVal = CodebenderccAPI::flushBuffer(fdevice);
                    finalRetVal=flushBufferRetVal;
                    if (flushBufferRetVal == -55 || flushBufferRetVal == -56 || flushBufferRetVal == -57){
                        RemovePortFromList(fdevice);
                        isAvrdudeRunning=false;
                        CodebenderccAPI::Invoke(flash_callback, flushBufferRetVal);
                        return;
                    }
                }

                boost::this_thread::interruption_point();
                std::string command = CodebenderccAPI::createCommand(fdevice,
                                                                    fprotocol,
                                                                    fspeed,
                                                                    fmcu);
                if (command == ""){
                    CodebenderccAPI::Invoke(flash_callback, -100);
                    RemovePortFromList(fdevice);
                    isAvrdudeRunning=false;
                    return;
                }

                boost::this_thread::interruption_point();

                int retVal = 1;
                retVal = CodebenderccAPI::runAvrdude(command, false);
                
                if (retVal==THREAD_INTERRUPTED){
                    RemovePortFromList(fdevice);
                    isAvrdudeRunning=false;
                    return;
                }

                if (retVal==1){
                    if(finalRetVal == 0)
                        retVal=retVal;
                    else
                        retVal= 30000 + finalRetVal;
                }

                _retVal = retVal;

                boost::this_thread::interruption_point();

                if (mcu == "atmega32u4")
                    CodebenderccAPI::LeonardoSketchControl(initialDevice);

                CodebenderccAPI::Invoke(flash_callback, retVal); 
                RemovePortFromList(fdevice);
                isAvrdudeRunning=false;

            }catch(boost::thread_interrupted&){
                RemovePortFromList(fdevice);
                isAvrdudeRunning=false;
            }
        } catch (...) {
            CodebenderccAPI::debugMessage("CodebenderccAPI::doflash exception",2);
            RemovePortFromList(fdevice);
            isAvrdudeRunning=false;
            CodebenderccAPI::Invoke(flash_callback, 9001);
        }
    }else{
        CodebenderccAPI::debugMessage("Port is in use, choose another port",3);
        isAvrdudeRunning=false;
        CodebenderccAPI::Invoke(flash_callback, -22);
    }

    CodebenderccAPI::debugMessage("CodebenderccAPI::doflash ended",3);

} catch (...) {
    error_notify("CodebenderccAPI::doFlash() threw an unknown exception");
    RemovePortFromList(device);
    isAvrdudeRunning=false;
    CodebenderccAPI::Invoke(flash_callback, 9002);
}

int CodebenderccAPI::resetLeonardo(std::string& fdevice)try {

    CodebenderccAPI::debugMessage("CodebenderccAPI::resetLeonardo",3);

    /* Get the initial list of ports before resetting the board */

    std::string oldports = availablePorts();
    std::istringstream oldStream(oldports);
    std::string token;
    std::vector<std::string> oldPorts;

    while(std::getline(oldStream, token, ','))
        oldPorts.push_back(token);

    CodebenderccAPI::debugMessage("Listing serial port changes..",2);
    std::string oldPortsMessage = "Initial ports : {" + oldports + "}";
    CodebenderccAPI::debugMessage(oldPortsMessage.c_str(),2);

    /* Open port and set the "magic" baudrate to force Leonardo reset */
    try{
        int openPortStatus=CodebenderccAPI::openPort(fdevice,
                                                    1200,
                                                    false,
                                                    "CodebenderccAPI::doflash Leonardo reset- ");
        if(openPortStatus!=1)
            return openPortStatus;

        boost::this_thread::interruption_point();
        delay(2000);
        CodebenderccAPI::closePort(false);
    }catch(boost::thread_interrupted&){
        CodebenderccAPI::closePort(false);
        return 0;
    }

    /* Delay for 300 ms so that the reset is complete */

    std::string os = getPlugin().get()->getOS();

    if ((os == "Windows") || (os == "X11"))
        delay(300);

    /* Get the new list of ports after resetting the board */

    int elapsed_time = 0;
    bool found = false;
    while(elapsed_time <= 10000){
        try{
            boost::this_thread::interruption_point();
            std::string newports = availablePorts();
            std::stringstream ss(newports);
            std::string item;
            std::vector<std::string> newPorts;

            while (std::getline(ss, item, ','))
                newPorts.push_back(item);

            std::string newPortsMessage = "New ports : {" + newports + "}";
            CodebenderccAPI::debugMessage(newPortsMessage.c_str(),2);

            /* Check if the new list of ports contains a port that did not exist in the initial ports list. */

            for (std::vector<std::string>::iterator it = newPorts.begin(); it != newPorts.end(); ++it){
                if (std::find(oldPorts.begin(), oldPorts.end(), *it) == oldPorts.end()){
                    fdevice = *it;
                    found = true;
                    break;
                }
            }

            /* If new list of ports contains a port that did not exist previously,
            Leonardo device is connected to that port. Save it and go on. */

            if (found){
                std::string leonardoDeviceMessage = "Found leonardo device on " + fdevice + " port";
                CodebenderccAPI::debugMessage(leonardoDeviceMessage.c_str(),2);
                break;
            }

            /* If new list of ports does not contain a new port, continue searching. */

            oldPorts = newPorts;
            delay(250);
            elapsed_time += 250;

            /* If a certain amount of time has gone by, and the initial port is in the list of ports,
            upload using this port. */

            if((( os != "Windows" && elapsed_time >= 500)||elapsed_time >=5000) && (std::find(oldPorts.begin(), oldPorts.end(), fdevice) != oldPorts.end()) ){
                std::string uploadingDeviceMessage = "Uploading using selected port: {" + fdevice +"}";
                CodebenderccAPI::debugMessage(uploadingDeviceMessage.c_str(),2);
                break;
            }

            if (elapsed_time == 10000){
                notify("Could not auto-reset or detect a manual reset!");
                std::string noResetMessage = "Could not auto reset device connected to port: {" + fdevice +"}";
                CodebenderccAPI::debugMessage(noResetMessage.c_str(),2);
                return -1;
            }
        }catch(boost::thread_interrupted&){
            return 0;
        }
    }

    CodebenderccAPI::debugMessage("CodebenderccAPI::resetLeonardo ended",3);

    return 1;

}catch (...) {
    CodebenderccAPI::debugMessage("CodebenderccAPI::resetLeonardo threw an unknown exception",2);
    return 0;
}

void CodebenderccAPI::LeonardoSketchControl(const std::string& fdevice)try{

    CodebenderccAPI::debugMessage("CodebenderccAPI::LeonardoSketchControl",3);

    /* If the current board is leonardo, wait for a few seconds 
    until the sketch actually takes control of the port. */

    delay(500);
    int timer = 0;
    while(timer < 2000){

        std::vector<std::string> portVector;
        std::string ports = availablePorts();
        std::stringstream chk(ports);
        std::string tok;

        while (std::getline(chk, tok, ','))
            portVector.push_back(tok);

        /* Check if the bootloader has finished and the
        sketch has taken control of the port. */

        if (std::find(portVector.begin(), portVector.end(), fdevice) != portVector.end()){
            delay(100);
            timer += 100;
            break;
        }

        delay(100);
        timer += 100;}

    CodebenderccAPI::debugMessage("CodebenderccAPI::LeonardoSketchControl ended",3);

}catch(...) {
    CodebenderccAPI::debugMessage("CodebenderccAPI::LeonardoSketchControl threw an unknown exception",2);
}

std::string CodebenderccAPI::createCommand(const std::string& fdevice,
                                          const std::string& protocol,
                                          const std::string& speed,
                                          const std::string& mcu)try{

    CodebenderccAPI::debugMessage("CodebenderccAPI::createCommand",3);
    std::string os = getPlugin().get()->getOS();

    #ifndef _WIN32
        std::string command = "\"" + avrdude + "\"" + " -C\"" + avrdudeConf + "\"";
    #else
        std::string command = avrdude + " -C" + avrdudeConf;
    #endif

    if (CodebenderccAPI::checkDebug() && currentLevel >= 2)
        command += " -v -v -v -v";

    command += " -V";
    command += " -P";
    command += (os == "Windows") ? "\\\\.\\" : "";

    #ifdef _WIN32
        command += fdevice + " -p" + mcu;
    #else
        command += "\"" + fdevice + "\"" + " -p" + mcu;
    #endif

    #ifdef _WIN32
        command += " -u -D -U flash:w:file.bin:r";
    #else
        command += " -u -D -U flash:w:\"" + binFile + "\":r";
    #endif

    command += " -c" + protocol + " -b" + speed + " -F";

    CodebenderccAPI::debugMessage("CodebenderccAPI::createCommand ended",3);
    return command;

}catch (...) {
    CodebenderccAPI::debugMessage("CodebenderccAPI::createCommand threw an unknown exception",2);
    return "";
}

void CodebenderccAPI::doflashWithProgrammer(const std::string& device,
                                            const std::string& code,
                                            const std::string& maxsize,
                                            std::map<std::string, std::string>& programmerData,
                                            const std::string& mcu,
                                            const FB::JSObjectPtr & flash_callback) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::doflashWithProgrammer",3);
    mtxAvrdudeFlag.lock();
    if(isAvrdudeRunning){
        CodebenderccAPI::Invoke(flash_callback, -23);
        mtxAvrdudeFlag.unlock();
        return;
        }
    isAvrdudeRunning=true;
    mtxAvrdudeFlag.unlock();

    std::string os = getPlugin().get()->getOS();

    try {
            if((programmerData["communication"] == "usb")||(programmerData["communication"] == "")||(AddtoPortList(device)))
            {
                #ifndef _WIN32
                        chmod(avrdude.c_str(), S_IRWXU);
                #endif

                unsigned char buffer [150000];
                size_t size = base64_decode(code.c_str(), buffer, 150000);
                saveToBin(buffer, size);

                std::string fdevice = device;
                int retVal = 1;
                try{
                    // Create the first part of the command, which includes the programmer settings.
                    programmerData["mcu"] = mcu.c_str();
                    programmerData["device"] = fdevice.c_str();
                    boost::this_thread::interruption_point();
                    std::string command = setProgrammerCommand(programmerData);

                    #ifdef _WIN32
                        command += " -Uflash:w:file.bin:r";
                    #else
                        command += " -Uflash:w:\"" + binFile + "\":r";
                    #endif
                    // Execute the upload command.
                    boost::this_thread::interruption_point();
                    retVal = CodebenderccAPI::runAvrdude(command, false);

                    if (retVal==THREAD_INTERRUPTED){
                        RemovePortFromList(fdevice);
                        isAvrdudeRunning=false;
                        return;
                        }

                    _retVal = retVal;
                    CodebenderccAPI::Invoke(flash_callback, retVal);
                    RemovePortFromList(device);

            }catch(boost::thread_interrupted&){
                    RemovePortFromList(device);
                    isAvrdudeRunning=false;
                    }
            }
            else
            {
                CodebenderccAPI::Invoke(flash_callback, -22);
                isAvrdudeRunning=false;
                CodebenderccAPI::debugMessage("CodebenderccAPI::Port is already in use.",3);
            }
    }catch(...){
        CodebenderccAPI::debugMessage("CodebenderccAPI::doflashWithProgrammer exception",2);
        isAvrdudeRunning=false;
        CodebenderccAPI::Invoke(flash_callback, 9001);
        RemovePortFromList(device);
    }
    CodebenderccAPI::debugMessage("CodebenderccAPI::doflashWithProgrammer ended",3);
    isAvrdudeRunning=false;
} catch (...) {
    error_notify("CodebenderccAPI::doflashWithProgrammer() threw an unknown exception");
}

void CodebenderccAPI::doflashBootloader(const std::string& device,
                                        std::map<std::string,std::string>& programmerData,
                                        std::map<std::string, std::string>& bootloaderData,
                                        const std::string& mcu,
                                        const FB::JSObjectPtr & flash_callback) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::doflashBootloader",3);
    mtxAvrdudeFlag.lock();
    if(isAvrdudeRunning){
        CodebenderccAPI::Invoke(flash_callback, -23);
        mtxAvrdudeFlag.unlock();
        return;}
    isAvrdudeRunning=true;
    mtxAvrdudeFlag.unlock();
    std::string os = getPlugin().get()->getOS();

    try {
        if((programmerData["communication"] == "usb")||(programmerData["communication"] == "")||(AddtoPortList(device))){

            try{
                #ifndef _WIN32
                    chmod(avrdude.c_str(), S_IRWXU);
                #endif

                std::string fdevice = device;
                int retVal = 1;

                // Create the first part of the command, which includes the programmer settings.
                programmerData["mcu"] = mcu.c_str();
                programmerData["device"] = fdevice.c_str();

                boost::this_thread::interruption_point();

                std::string programmerCommand = setProgrammerCommand(programmerData);

                // The first part of the command is very likely to be used again when flashing the hex file.
                std::string command = programmerCommand;

                /** Erase the chip, applying the proper values to the unlock bits and high/low/extended fuses.
                Note: Values for high and low fuses MUST exist. The other values are optional, depending on the chip. **/

                command += " -e";
                command += (bootloaderData["ulbits"] != "") ? " -Ulock:w:" + bootloaderData["ulbits"] + ":m" : "";
                command += (bootloaderData["efuses"] != "") ? " -Uefuse:w:" + bootloaderData["efuses"] + ":m" : "";
                command += " -Uhfuse:w:" + bootloaderData["hfuses"] + ":m"
                        + " -Ulfuse:w:" + bootloaderData["lfuses"] + ":m";

                boost::this_thread::interruption_point();

                retVal = CodebenderccAPI::runAvrdude(command, false);
                _retVal = retVal;

                if (retVal==THREAD_INTERRUPTED){
                    RemovePortFromList(fdevice);
                    isAvrdudeRunning=false;
                    return;}

                // If avrdude failed return the error code, else continue.
                if (retVal == 0){

                    // Apply a delay of one second.
                    delay(1000);

                    // Check if hex bootloader was sent from the server. If no bootloader exists, an empty file is created.
                    std::ifstream file (hexFile.c_str(), std::ifstream::binary);
                    if (file.is_open()){
                        file.seekg (0, file.end);
                        int length = file.tellg();

                        if (length == 0){
                            file.close();
                        }else{
                            command = programmerCommand;
                            #ifdef _WIN32
                                command += " -Uflash:w:bootloader.hex:i";
                            #else
                                command += " -Uflash:w:\"" + hexFile + "\":i";
                            #endif
                            command += (bootloaderData["lbits"] != "") ? " -Ulock:w:" + bootloaderData["lbits"] + ":m" : "";

                            boost::this_thread::interruption_point();

                            retVal = CodebenderccAPI::runAvrdude(command, true);
                            _retVal = retVal;

                            if (retVal==THREAD_INTERRUPTED){
                                RemovePortFromList(fdevice);
                                isAvrdudeRunning=false;
                                return;}
                        }
                    }
                }

                CodebenderccAPI::Invoke(flash_callback, retVal);
                RemovePortFromList(fdevice);
                isAvrdudeRunning=false;
                CodebenderccAPI::debugMessage("CodebenderccAPI::doflashBootloader ended",3);

            }catch(boost::thread_interrupted&){
                RemovePortFromList(device);
                isAvrdudeRunning=false;
                return;}

        }else{
            CodebenderccAPI::Invoke(flash_callback, -22);
            isAvrdudeRunning=false;
            CodebenderccAPI::debugMessage("CodebenderccAPI::Port is already in use:",3);}

    }catch(...){
        CodebenderccAPI::debugMessage("CodebenderccAPI::doflashBootloader exception",2);
        isAvrdudeRunning=false;
        RemovePortFromList(device);
        CodebenderccAPI::Invoke(flash_callback, 9001);}

}catch (...){
    isAvrdudeRunning=false;
    error_notify("CodebenderccAPI::doflashBootloader() threw an unknown exception");
}

const std::string CodebenderccAPI::setProgrammerCommand(std::map<std::string, std::string>& programmerData) try {
    
    CodebenderccAPI::debugMessage("CodebenderccAPI::setProgrammerCommand",3);
    std::string os = getPlugin().get()->getOS();
    #ifndef _WIN32
        std::string command = "\"" + avrdude + "\"" + " -C\"" + avrdudeConf + "\"";
    #else
        std::string command = avrdude + " -C" + avrdudeConf;
    #endif
    /**
    * Check if debugging is set to true. If the debug verbosity level is greater than 1,
    * add verbosity flags to the avrdude command.
    */
    if (CodebenderccAPI::checkDebug() && currentLevel >= 2){
        command += " -v -v -v -v";
    }else{
        command += " -V";
    }
    command += " -p" + programmerData["mcu"] + " -c" + programmerData["protocol"];
    if (programmerData["communication"] == "usb"){
        command += " -Pusb";
    }else if (programmerData["communication"] == "serial"){
        command += " -P";
        command += (os == "Windows") ? "\\\\.\\" : "";
        command += programmerData["device"];
        if (programmerData["speed"] != "0"){
            command += " -b" + programmerData["speed"];
        }
    }
    if (programmerData["force"] == "true")
        command += " -F";
    if (programmerData["delay"] != "0")
        command += " -i" + programmerData["delay"];

    CodebenderccAPI::debugMessage("CodebenderccAPI::setProgrammerCommand ended",3);
    return command;
} catch (...) {
    error_notify("CodebenderccAPI::setProgrammerCommand() threw an unknown exception");
    return "";
}

int CodebenderccAPI::runAvrdude(const std::string& command, bool append) try {

    CodebenderccAPI::debugMessage("CodebenderccAPI::runAvrdude",3);
    int retval = 1;
    #ifdef _WIN32
        // Ιf on Windows, create a batch file and save the command in that file.
        std::ofstream batchFd;
        try{
            batchFd.open(batchFile.c_str());
            batchFd << command;
            batchFd.close();
        }catch(...){
            CodebenderccAPI::debugMessage("Failed to write command to batch file!",1);
        }
        lastcommand = command;
        // Call winExecAvrdude, which creates a new process, runs the batch file and gets all the output.
        retval = winExecAvrdude(batchFile, append);
    #else
         /**
         * If on Unix-like system, call unixExecAvrdude which makes a duplicate of the current process (fork)
         * and replaces the entire new process with avrdude (exec).
         */
        lastcommand = command;

        retval = unixExecAvrdude(command, append);
    #endif

    /** Print the content of the output file, if debugging is on. **/
    if (CodebenderccAPI::checkDebug()){
        std::ifstream ifs(outfile.c_str());
        std::string content( (std::istreambuf_iterator<char>(ifs) ),
        (std::istreambuf_iterator<char>()    ) );
        CodebenderccAPI::debugMessage(content.c_str(),1);
    }
    CodebenderccAPI::debugMessage(lastcommand.c_str(),1);
    CodebenderccAPI::debugMessage("CodebenderccAPI::runAvrdude ended",3);
    return retval;
} catch (...) {
    error_notify("CodebenderccAPI::runAvrdude() threw an unknown exception");
    return 1;
}

#if !defined  _WIN32
int CodebenderccAPI::unixExecAvrdude (const std::string &command,
                                      bool appendFlag) try{
    std::string executionCommand = command;
    /* Convert string vector to char array */
    std::vector<char *> cmd_argv(4);
    cmd_argv[0] = "sh";
    cmd_argv[1] = "-c";
    cmd_argv[2] = &executionCommand[0];
    cmd_argv[3] = NULL;

    pid_t pid, w;

    /* make a duplicate of the current process */
    pid = CodebenderccAPI::fork();
    if (pid == -1)
        return -200; // fork() failed

    if (pid == 0) {
        pid_t cpid = getpid();
        setpgid(cpid, cpid);
        const char *frpathout = outfile.c_str();

        stdout = CodebenderccAPI::freopen(frpathout,
                                          appendFlag ? "a" : "w",
                                          stdout);
        stderr = CodebenderccAPI::freopen(frpathout,
                                          appendFlag ? "a" : "w",
                                          stderr);

        CodebenderccAPI::execvp(cmd_argv[0], cmd_argv.data());
        _exit(EXIT_FAILURE);
    }

    try{
        long oldSize=0;
        long newSize=0;
        int counter =0;

        do {

            boost::this_thread::interruption_point();
            int status = 0;

            w = CodebenderccAPI::waitpid(pid, &status, WNOHANG);
            if (w == -1)
                return -202; // waitpid() failed

            if (w == 0) {
                /* wait for 0.0001 sec before getting the size of the output file*/
                delay(1);
                /* Check if the file exists */
                boost::filesystem::path out(outfile);
                if(boost::filesystem::exists(out)){
                    /* the child's state has not changed */
                    newSize = CodebenderccAPI::filesize(outfile.c_str());
                    if (newSize == -1)
                        break;
                    if (newSize == oldSize)
                        counter++;
                    else{
                        oldSize = newSize;
                        counter = 0;
                    }
                }
                /* If file doesn't exist, increase counter */
                else {
                    counter++;
                }
            }
            else if(WIFSIGNALED(status)){
                return -203;
            }
            else if (WIFEXITED(status)){
                return WEXITSTATUS(status);
            }
        } while (counter != 20000);

        killpg(pid, SIGKILL);

        return -204; // child process was killed

    }catch(boost::thread_interrupted&){
        return -1234;
    }
} catch (...){
    error_notify("CodebenderccAPI::unixExecAvrdude() threw an unknown exception");
    return -205;
}
#endif

#if !defined  _WIN32
long CodebenderccAPI::filesize(const char *filename) try{
    struct stat buf;

    if (CodebenderccAPI::stat(filename, &buf) == -1)
        return -1;

    return buf.st_size;
} catch (...){
    error_notify("CodebenderccAPI::filesize() threw an unknown exception");
    return -1;
}
#endif

/**
 * Save the binary data to the binary file specified in the constructor.
 */
void CodebenderccAPI::saveToBin(unsigned char * data, size_t size) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::saveToBin",3);
    #if !defined _WIN32 ||_WIN34
        FILE *fp = CodebenderccAPI::fopen(binFile.c_str(), "wb");
    #else
        const std::string stbinFile(binFile.begin(),binFile.end());
        FILE *fp = CodebenderccAPI::fopen(stbinFile.c_str(), "wb");
    #endif
    if (!fp)
        return;

    CodebenderccAPI::fwrite(data, size, 1, fp);
    CodebenderccAPI::fclose(fp);

    CodebenderccAPI::debugMessage("CodebenderccAPI::saveToBin ended",3);
} catch (...) {
    error_notify("CodebenderccAPI::saveToBin() threw an unknown exception");
}

/**
 * Save the hex data of the bootloader file to the hex file specified in the constructor.
 */
void CodebenderccAPI::saveToHex(const std::string& hexContent) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::saveToHex",3);
    #if !defined _WIN32
        FILE *fp = CodebenderccAPI::fopen(hexFile.c_str(), "wb");
    #else
        const std::string sthexFile(hexFile.begin(),hexFile.end());
        FILE *fp = CodebenderccAPI::fopen(sthexFile.c_str(), "wb");
    #endif
    if (!fp)
        return;

    CodebenderccAPI::fwrite(hexContent.c_str(), hexContent.length(), hexContent.length() ? 1 : 0, fp);
    CodebenderccAPI::fclose(fp);

    CodebenderccAPI::debugMessage("CodebenderccAPI::saveToHex ended",3);
} catch (...) {
    error_notify("CodebenderccAPI::saveToHex() threw an unknown exception");
}

void CodebenderccAPI::detectNewPort(const std::string& portString) try {
    
    std::vector<std::string> portVector;
    std::vector< std::string >::iterator externalIterator = portsList.begin();

    std::string mess;

    std::stringstream chk(portString);
    std::string tok;
    while (std::getline(chk, tok, ',')) {
        portVector.push_back(tok);
    }
    
    std::vector< std::string >::iterator internalIterator = portVector.begin();
    
    while (externalIterator != portsList.end()){
        if (std::find(portVector.begin(), portVector.end(), *externalIterator) == portVector.end()){
            mess = "Device removed from port : " + *externalIterator;
            CodebenderccAPI::debugMessage(mess.c_str(),1);
        }
        externalIterator++;
    }
    while (internalIterator != portVector.end()){
        if (std::find(portsList.begin(), portsList.end(), *internalIterator) == portsList.end()){
            mess = "Device added to port : " + *internalIterator;
            CodebenderccAPI::debugMessage(mess.c_str(),1);
        }
        internalIterator++;
    }
    
    portsList = portVector;
} catch (...) {
    error_notify("CodebenderccAPI::detectNewPort() threw an unknown exception");
}

void CodebenderccAPI::serialReader(const std::string &port,
                                   const unsigned int &baudrate,
                                   const FB::JSObjectPtr & callback,
                                   const FB::JSObjectPtr & valHandCallback) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::serialReader",3);
    
    int openPortStatus=CodebenderccAPI::openPort(port, baudrate, false, "CodebenderccAPI::serialReader - ");

    if(openPortStatus!=1){
        CodebenderccAPI::Invoke(valHandCallback, openPortStatus);
        notify("disconnect");
        CodebenderccAPI::disconnect();
        return;
    }

    try {

        std::string rcvd;
        serialPort.flushInput();
        serialPort.flushOutput();
        serialMonitor.lock();
        serialMonitorStatus=true;
        serialMonitor.unlock();
        try{
            for (;;) {

                 boost::this_thread::interruption_point();

                if (CodebenderccAPI::checkSerialMonitorStatus()==0)
                    break;

                if (!serialPort.isOpen())
                    break;

                if(!serialPort.available())
                    continue;
                
                rcvd = "";
                rcvd = serialPort.read((size_t) 100);
                
                if (rcvd != "")
                    CodebenderccAPI::Invoke(callback, rcvd);

            }
        }catch(boost::thread_interrupted&){
            CodebenderccAPI::serialMonitorSetStatus();
            CodebenderccAPI::disconnect();
        }        
    }catch (...) {
        CodebenderccAPI::debugMessage("CodebenderccAPI::serialReader loop interrupted",1);
         error_notify("CodebenderccAPI::serialReader loop interrupted", 1);
        notify("disconnect");
        CodebenderccAPI::serialMonitorSetStatus();
        CodebenderccAPI::disconnect();
    }
    CodebenderccAPI::disconnect();
    CodebenderccAPI::debugMessage("CodebenderccAPI::serialReader ended",3);
} catch (...) {
    error_notify("CodebenderccAPI::serialReader() threw an unknown exception");
    notify("disconnect");
    CodebenderccAPI::serialMonitorSetStatus();
    CodebenderccAPI::disconnect();
}

void CodebenderccAPI::serialMonitorSetStatus(){
    serialMonitor.lock();
    serialMonitorStatus= false;
    serialMonitor.unlock();
}

bool CodebenderccAPI::checkSerialMonitorStatus(){
    return serialMonitorStatus;
}

int CodebenderccAPI::PortNotOpenedException(std::string err_mess)try{
    if (err_mess.find("Serial::setDTR")!=std::string::npos)
        return 1001;
    else if    (err_mess.find("Serial::setRTS")!=std::string::npos)
        return 1002;
    else
        return 1000;
}catch (...) {
error_notify("CodebenderccAPI::PortNotOpenedException threw an unknown exception.");
    return -1;
}

int CodebenderccAPI::SerialException(std::string err_mess)try{
    if (err_mess.find("Serial port already open.")!=std::string::npos)
        return 2001;
    int error_code=CodebenderccAPI::checkIfIsDigit(err_mess);
    if (error_code == 10000)
        return error_code;
    if (err_mess.find("setDTR failed on a call to ioctl(TIOCMBIC):")!=std::string::npos)
        return (2100+error_code);
    else if    (err_mess.find("setDTR failed on a call to ioctl(TIOCMBIS):")!=std::string::npos)
        return (2300+error_code);
    else if    (err_mess.find("setRTS failed on a call to ioctl(TIOCMBIS):")!=std::string::npos)
        return (2500+error_code);
    else if    (err_mess.find("setRTS failed on a call to ioctl(TIOCMBIC):")!=std::string::npos)
        return (2700+error_code);
    else
        return 2000;
    }catch (...) {
    error_notify("CodebenderccAPI::SerialException threw an unknown exception.");
        return -1;
    }

int CodebenderccAPI::invalid_argument(std::string err_mess)try{
    if (err_mess.find("Empty port is invalid")!=std::string::npos)
        return 3001;
    else if    (err_mess.find("invalid char len")!=std::string::npos)
        return 3002;
    else if    (err_mess.find("invalid stop bit")!=std::string::npos)
        return 3003;
    else if    (err_mess.find("invalid parity")!=std::string::npos)
        return 3004;
    else if    (err_mess.find("OS does not currently support custom bauds")!=std::string::npos)
        return 3005;
    else
        return 3000;
}catch (...) {
error_notify("CodebenderccAPI::invalid_argument threw an unknown exception.");
    return -1;
}

#ifndef _WIN32
    int CodebenderccAPI::IOException(std::string err_mess)try{
        if (err_mess.find("Too many file handles open.")!=std::string::npos)
            return 4001;
        else if    (err_mess.find("Invalid file descriptor, is the serial port open?")!=std::string::npos)
            return 4002;
        else if    (err_mess.find("::tcgetattr")!=std::string::npos)
            return 4003;
        int error_code=CodebenderccAPI::GetTag(err_mess);
        if (error_code == 20000)
            return error_code;
        if    (err_mess.find("IO Exception (")!=std::string::npos)
            return (4100+error_code);
        else
            return 4000;
    }catch (...) {
    error_notify("CodebenderccAPI::IOException threw an unknown exception.");
        return -1;
    }
#endif
#ifdef _WIN32
    int CodebenderccAPI::IOException(std::string err_mess)try{
        if (err_mess.find("Error setting timeouts.")!=std::string::npos)
            return 5001;
        else if    (err_mess.find("Error setting serial port settings")!=std::string::npos)
            return 5002;
        else if    (err_mess.find("Invalid file descriptor, is the serial port open?")!=std::string::npos)
            return 5003;
        else if    (err_mess.find("Error getting the serial port state")!=std::string::npos)
            return 5004;
        int error_code=CodebenderccAPI::GetNumberBetween(err_mess);
        if (error_code == 20000)
            return error_code;
        if    (err_mess.find("Unknown error opening the serial port:")!=std::string::npos)
            return (6000+error_code);
        else
            return 5000;
    }catch (...) {
    error_notify("CodebenderccAPI::IOException threw an unknown exception.");
        return -1;
    }
#endif

int CodebenderccAPI::checkIfIsDigit(std::string err_mess)try{
    int err_code;
    int nIndex;
    stringstream strStream;
       for (nIndex=0; nIndex < err_mess.length(); nIndex++){
               if (isdigit(err_mess[nIndex]))
                strStream << err_mess[nIndex];
       }
    strStream >> err_code;
    return err_code;
    }catch (...) {
    error_notify("CodebenderccAPI::checkIfIsDigit threw an unknown exception.");
        return 10000;
    }

int CodebenderccAPI::GetTag(std::string str)try{
    int err_code;
    std::string retVal;
    std::string::size_type start = str.find('(');
    if (start != str.npos)
    {
        std::string::size_type end = str.find(')', start + 1);
        if (end != str.npos)
        {
            ++start;
            std::string::size_type count = end - start;
            retVal=str.substr(start, count);
            err_code=atoi(retVal.c_str());
            return err_code;
        }
    }
    return -1;
    }catch (...) {
    error_notify("CodebenderccAPI::GetTag threw an unknown exception.");
        return 20000;
    }

int CodebenderccAPI::GetNumberBetween(std::string str)try{
    int err_code;
    std::string::size_type start = str.find_last_of(':');
    if (start != str.npos)
    {
        std::string::size_type end = str.find_first_of(',', start + 1);
        if (end != str.npos){
            ++start;
            std::string::size_type count = end - start;
            err_code=atoi((str.substr(start, count)).c_str());
            return err_code;
        }
    }
    return -1;
    }catch (...) {
    error_notify("CodebenderccAPI::GetNumberBetween threw an unknown exception.");
        return 20000;
    }

std::string CodebenderccAPI::exec(const char * cmd) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::exec",3);
    std::string result = "";
    #ifndef _WIN32
        FILE* pipe = CodebenderccAPI::popen(cmd, "r");
        if (!pipe) return "ERROR";
        char buffer[128];
        while (CodebenderccAPI::fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
        }
        CodebenderccAPI::pclose(pipe);
    #endif
    CodebenderccAPI::debugMessage("CodebenderccAPI::exec ended",3);
    return result;
} catch (...) {
    error_notify("CodebenderccAPI::exec() threw an unknown exception");
    return "";
}

void CodebenderccAPI::Invoke(const FB::JSObjectPtr &flash_callback, const int &value) {
    CodebenderccAPI::debugMessage("CodebenderccAPI::Invoke",3);
    if (CodebenderccAPI::JSAPIWeakPtrExists()==true)
        flash_callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(value));
}

void CodebenderccAPI::Invoke(const FB::JSObjectPtr &flash_callback, const string &value) {
    CodebenderccAPI::debugMessage("CodebenderccAPI::Invoke",3);
    if (CodebenderccAPI::JSAPIWeakPtrExists()==true)
        flash_callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(value));
}

void CodebenderccAPI::notify(const std::string &message) {
    CodebenderccAPI::debugMessage("CodebenderccAPI::notify",3);
    if (CodebenderccAPI::JSAPIWeakPtrExists()==true)
        callback_->InvokeAsync("", FB::variant_list_of(shared_from_this())(message.c_str()));
}

void CodebenderccAPI::error_notify(const std::string &message, int warningFlag) {
    CodebenderccAPI::debugMessage("CodebenderccAPI::error_notify",3);
    if (CodebenderccAPI::JSAPIWeakPtrExists()==true)
        error_callback_->InvokeAsync("", FB::variant_list_of(shared_from_this())(message.c_str())(warningFlag));
}

DIR *
CodebenderccAPI::opendir(const char *name){
    DIR *dp;

    dp = ::opendir(name);
    if (dp)
        return dp;

    std::string err_msg = "CodebenderccAPI::opendir() - ";

    switch (errno) {
        case EACCES:
            err_msg += "EACCES: Permission denied.";
            break;
        case EMFILE:
            err_msg += "EMFILE: Too many file descriptors in use by process.";
            break;
        case ENFILE:
            err_msg += "ENFILE: Too many files are currently open in the system.";
            break;
        case ENOENT:
            err_msg += "ENOENT: Directory does not exist, or name is an empty string.";
            break;
        case ENOMEM:
            err_msg += "ENOMEM: Insufficient memory to complete the operation.";
            break;
        case ENOTDIR:
            err_msg += "ENOTDIR: name is not a directory.";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
    return NULL;
}

struct dirent *
CodebenderccAPI::readdir(DIR *dirp)
{
    struct dirent *dent;

    errno = 0;
    dent = ::readdir(dirp);

    if (errno == 0)
        return dent;

    std::string err_msg = "CodebenderccAPI::readdir() - ";

    switch (errno) {
        case EBADF:
            err_msg += "EBADF: Invalid directory stream descriptor dirp.";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
    return NULL;
}

void
CodebenderccAPI::closedir(DIR *dirp)
{
    int rc;

    rc = ::closedir(dirp);
    if (rc == 0)
        return;

    std::string err_msg = "CodebenderccAPI::closedir() - ";

    switch (errno) {
        case EBADF:
            err_msg += "EBADF: Invalid directory stream descriptor dirp.";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
}

FILE *
CodebenderccAPI::fopen(const char *path, const char *mode)
{
    FILE *fp;

    fp = ::fopen(path, mode);
    if (fp)
        return fp;

    std::string err_msg = "CodebenderccAPI::fopen() - ";

    switch (errno) {
        case EINVAL:
            err_msg += "EINVAL: The mode provided to fopen() was invalid.";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
    return NULL;
}

FILE *
CodebenderccAPI::freopen(const char *path, const char *mode, FILE *stream)
{
    FILE *fp;

    fp = ::freopen(path, mode, stream);
    if (fp != NULL)
        return fp;

    std::string err_msg = "CodebenderccAPI::freopen() - ";

    switch (errno) {
        case EINVAL:
            err_msg += "EINVAL: The mode provided to freopen() was invalid.";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
    return NULL;
}

size_t
CodebenderccAPI::fwrite(const void *ptr,
                        size_t size,
                        size_t nmemb,
                        FILE *stream)
{
    std::string err_msg = "CodebenderccAPI::fwrite() - ";
    size_t n;

    clearerr(stream);

    n = ::fwrite(ptr, size, nmemb, stream);
    if (n == nmemb)
        return n;

    err_msg += (ferror(stream) != 0) ? "errno set" : "bad I/O";
    error_notify(err_msg);
    return n;
}

char *
CodebenderccAPI::fgets(char *s, int size, FILE *stream)
{
    char *ret;

    clearerr(stream);
    ret = ::fgets(s, size, stream);

    if (ferror(stream)  == 0)
        return ret;

    std::string err_msg = "CodebenderccAPI::fgets() failed";

    error_notify(err_msg);
    return NULL;
}

void
CodebenderccAPI::fclose(FILE *fp)
{
    if (::fclose(fp) == 0)
        return;

    std::string err_msg = "CodebenderccAPI::fclose() - ";

    switch (errno) {
        case EBADF:
            err_msg += "EBADF  The file descriptor underlying fp is not valid.";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
}

#ifndef _WIN32
FILE *
CodebenderccAPI::popen(const char *command, const char *type)
{
    FILE *fp;

    fp = ::popen(command, type);
    if (fp)
        return fp;

    std::string err_msg = "CodebenderccAPI::popen() - ";

    switch (errno) {
        case EINVAL:
            err_msg += "EINVAL: invalid type argument";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
    return NULL;
}

void
CodebenderccAPI::pclose(FILE *stream)
{
    if (::pclose(stream) != -1)
        return;

    std::string err_msg = "CodebenderccAPI::pclose() - ";

    switch (errno) {
        case ECHILD:
            err_msg += "ECHILD: could not obtain the child status";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
}

int
CodebenderccAPI::stat(const char *path, struct stat *buf)
{
    int rc;

    rc = ::stat(path, buf);
    if (rc == 0)
        return rc;

    std::string err_msg = "CodebenderccAPI::stat() - ";

    switch (errno) {
        case EACCES:
            err_msg += "EACCES: Search permission is denied for one of the " \
                       "directories in the path prefix of path.";
            break;
        case EFAULT:
            err_msg += "EFAULT: Bad address";
            break;
        case ELOOP:
            err_msg += "ELOOP: Too many symbolic links encountered while traversing the path";
            break;
        case ENAMETOOLONG:
            err_msg += "ENAMETOOLONG: path is too long";
            break;
        case ENOENT:
            err_msg += "ENOENT: A component of path does not exist, or path is an empty string";
            break;
        case ENOMEM:
            err_msg += "ENOMEM: Out of kernel memory";
            break;
        case ENOTDIR:
            err_msg += "ENOTDIR: A component of the path prefix of path is not a directory.";
            break;
        case EOVERFLOW:
            err_msg += "EOVERFLOW: path or fd refers to a file whose "                          \
                       "size, inode number, or number of blocks cannot  be  represented  in, "  \
                       "respectively, the types off_t, ino_t, or blkcnt_t.";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
    return -1;
}

pid_t
CodebenderccAPI::fork(void)
{
    pid_t pid;

    pid = ::fork();
    if (pid != -1)
        return pid;

    std::string err_msg = "CodebenderccAPI::fork() - ";

    switch (errno) {
        case EAGAIN:
            err_msg += "EAGAIN: cannot allocate sufficient memory to copy the " \
                       "parent's page tables and allocate a  task  structure for the child.";
            break;

        case ENOMEM:
            err_msg += "ENOMEM: failed to allocate the necessary kernel structures" \
                       " because memory is tight.";
            break;

        case ENOSYS:
            err_msg += "ENOSYS: fork() is not supported on this platform.";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
    return pid;
}

int
CodebenderccAPI::execvp(const char *file, char *const argv[])
{
    int rc;

    rc = ::execvp(file, argv);

    std::string err_msg = "CodebenderccAPI::execvp() - ";

    if (rc == -1)
        err_msg += "Unknown error!";
    else
        err_msg += "execv() returned != -1!!!";

    error_notify(err_msg);
    return rc;
}

pid_t
CodebenderccAPI::waitpid(pid_t pid, int *status, int options)
{
    pid_t cpid;

    cpid = ::waitpid(pid, status, options);
    if (cpid != -1)
        return cpid;

    std::string err_msg = "CodebenderccAPI::waitpid() - ";

    switch (errno) {
        case ECHILD:
            err_msg += "ECHILD: The process specified by pid does not exist or " \
                       "is not a child of the calling process.";
            break;

        case EINTR:
            err_msg += "EINTR: WNOHANG was not set and an unblocked signal or " \
                       "a SIGCHLD was caught; see signal(7).";
            break;

        case EINVAL:
            err_msg += "EINVAL: The option argument was invalid";
            break;

        default:
            err_msg += "Unknown error!";
    }

    error_notify(err_msg);
    return cpid;
}

#endif

/** TODO: we should return the result of WEXITSTATUS(rc) */
int
CodebenderccAPI::system(const char *command)
{
    std::string err_msg = "CodebenderccAPI::system() - ";
    int rc;

    rc = ::system(NULL);
    if (rc == 0)
        error_notify(err_msg + "can not access the shell");

    rc = ::system(command);
    if (rc == -1)
        err_msg += "Unknown error";
#ifndef _WIN32
    else if (WEXITSTATUS(rc) == 127)
        err_msg += "/bin/sh could not be executed";
#endif
    else
        return rc;

    error_notify(err_msg);
    return rc;
}

#ifdef _WIN32
LONG
CodebenderccAPI::RegQueryInfoKey(HKEY hKey,
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
                                 PFILETIME lpftLastWriteTime)
{
    LONG rc;

    rc = ::RegQueryInfoKey(hKey,
                           lpClass,
                           lpcClass,
                           lpReserved,
                           lpcSubKeys,
                           lpcMaxSubKeyLen,
                           lpcMaxClassLen,
                           lpcValues,
                           lpcMaxValueNameLen,
                           lpcMaxValueLen,
                           lpcbSecurityDescriptor,
                           lpftLastWriteTime);
    if (rc == ERROR_SUCCESS)
        return rc;

    std::string err_msg = "CodebenderccAPI::RegQueryInfoKey() - ";

    switch (rc) {
        case ERROR_MORE_DATA:
            err_msg += "lpClass buffer is too small to receive the name of the class";
            break;

        default:
            err_msg += "System error code: ";
            err_msg += boost::lexical_cast<std::string>(rc);
    }

    error_notify(err_msg);
    return rc;
}

LONG
CodebenderccAPI::RegEnumValue(HKEY hKey,
                              DWORD dwIndex,
                              LPTSTR lpValueName,
                              LPDWORD lpcchValueName,
                              LPDWORD lpReserved,
                              LPDWORD lpType,
                              LPBYTE lpData,
                              LPDWORD lpcbData)
{
    LONG rc;

    rc = ::RegEnumValue(hKey,
                        dwIndex,
                        lpValueName,
                        lpcchValueName,
                        lpReserved,
                        lpType,
                        lpData,
                        lpcbData);
    if (rc == ERROR_SUCCESS)
        return rc;

    std::string err_msg = "CodebenderccAPI::RegEnumValue() - ";

    switch (rc) {
        case ERROR_MORE_DATA:
            err_msg += "lpData buffer is too small to receive the value";
            break;

        case ERROR_NO_MORE_ITEMS:
            err_msg += "There are no more values available";
            break;

        default:
            err_msg += "System error code: ";
            err_msg += boost::lexical_cast<std::string>(rc);
    }

    error_notify(err_msg);
    return rc;
}

LONG
CodebenderccAPI::RegQueryValueEx(HKEY hKey,
                                 LPCTSTR lpValueName,
                                 LPDWORD lpReserved,
                                 LPDWORD lpType,
                                 LPBYTE lpData,
                                 LPDWORD lpcbData)
{
    LONG rc;

    rc = ::RegQueryValueEx(hKey,
                           lpValueName,
                           lpReserved,
                           lpType,
                           lpData,
                           lpcbData);
    if (rc == ERROR_SUCCESS)
        return rc;

    std::string err_msg = "CodebenderccAPI::RegQueryValueEx() - ";

    switch (rc) {
        case ERROR_MORE_DATA:
            err_msg += "lpData buffer is too small to receive the data";
            break;

        case ERROR_FILE_NOT_FOUND:
            err_msg += "lpValueName registry value does not exist";
            break;

        default:
            err_msg += "System error code: ";
            err_msg += boost::lexical_cast<std::string>(rc);
    }

    error_notify(err_msg);
    return rc;
}


LONG
CodebenderccAPI::RegOpenKeyEx(HKEY hKey,
                              LPCTSTR lpSubKey,
                              DWORD ulOptions,
                              REGSAM samDesired,
                              PHKEY phkResult)
{
    LONG rc;
    static bool errorFlag;

    rc = ::RegOpenKeyEx(hKey,
                        lpSubKey,
                        ulOptions,
                        samDesired,
                        phkResult);
    if (rc == ERROR_SUCCESS){
        errorFlag = false;
        return rc;
    }

    if (rc != ERROR_SUCCESS && errorFlag == false){
        errorFlag = true;
        /* TODO: Print error code description with FormatMessage.
         http://msdn.microsoft.com/en-us/library/windows/desktop/ms724897%28v=vs.85%29.aspx */
        std::string err_msg = "CodebenderccAPI::RegOpenKeyEx() - Winerror.h error code: ";
        err_msg += boost::lexical_cast<std::string>(rc);

        if(rc==2)
            error_notify(err_msg,1);
        else
            error_notify(err_msg);
        return rc;
    }
    
    return rc;
}

LONG
CodebenderccAPI::RegCloseKey(HKEY hKey)
{
    LONG rc;
    static bool errorFlag;

    rc = ::RegCloseKey(hKey);
    if (rc == ERROR_SUCCESS){
        errorFlag = false;
        return rc;
    }

    if (rc != ERROR_SUCCESS && errorFlag == false){
        errorFlag = true;
     /* TODO: Print error code description with FormatMessage.
       http://msdn.microsoft.com/en-us/library/windows/desktop/ms724837%28v=vs.85%29.aspx */
        std::string err_msg = "CodebenderccAPI::RegCloseKey() - Winerror.h error code: ";
        err_msg += boost::lexical_cast<std::string>(rc);
        if(rc==6)
            error_notify(err_msg,1);
        else
            error_notify(err_msg);
        return rc;
    }

    return rc;
}

HANDLE
CodebenderccAPI::CreateFile(LPCTSTR lpFileName,
                            DWORD dwDesiredAccess,
                            DWORD dwShareMode,
                            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                            DWORD dwCreationDisposition,
                            DWORD dwFlagsAndAttributes,
                            HANDLE hTemplateFile)
{
    HANDLE rc;

    rc = ::CreateFile(lpFileName,
                      dwDesiredAccess,
                      dwShareMode,
                      lpSecurityAttributes,
                      dwCreationDisposition,
                      dwFlagsAndAttributes,
                      hTemplateFile);
    if (rc != INVALID_HANDLE_VALUE)
        return rc;

    std::string err_msg = "CodebenderccAPI::CreateFile() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

BOOL
CodebenderccAPI::CreateProcess(LPCTSTR lpApplicationName,
                               LPTSTR lpCommandLine,
                               LPSECURITY_ATTRIBUTES lpProcessAttributes,
                               LPSECURITY_ATTRIBUTES lpThreadAttributes,
                               BOOL bInheritHandles,
                               DWORD dwCreationFlags,
                               LPVOID lpEnvironment,
                               LPCTSTR lpCurrentDirectory,
                               LPSTARTUPINFO lpStartupInfo,
                               LPPROCESS_INFORMATION lpProcessInformation)
{
    BOOL rc;

    rc = ::CreateProcess(lpApplicationName,
                         lpCommandLine,
                         lpProcessAttributes,
                         lpThreadAttributes,
                         bInheritHandles,
                         dwCreationFlags,
                         lpEnvironment,
                         lpCurrentDirectory,
                         lpStartupInfo,
                         lpProcessInformation);
    if (rc != 0)
        return rc;

    std::string err_msg = "CodebenderccAPI::CreateProcess() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

DWORD
CodebenderccAPI::WaitForSingleObject(HANDLE hHandle,
                                     DWORD dwMilliseconds)
{
    DWORD rc;

    rc = ::WaitForSingleObject(hHandle,
                               dwMilliseconds);
    if (rc != WAIT_FAILED)
        return rc;

    std::string err_msg = "CodebenderccAPI::WaitForSingleObject() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

BOOL
CodebenderccAPI::GetExitCodeProcess(HANDLE hProcess,
                                    LPDWORD lpExitCode)
{
    BOOL rc;

    rc = ::GetExitCodeProcess(hProcess,
                              lpExitCode);
    if (rc != 0)
        return rc;

    std::string err_msg = "CodebenderccAPI::GetExitCodeProcess() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

BOOL
CodebenderccAPI::TerminateProcess(HANDLE hProcess,
                                  UINT uExitCode)
{
    BOOL rc;

    rc = ::TerminateProcess(hProcess,
                            uExitCode);
    if (rc != 0)
        return rc;

    std::string err_msg = "CodebenderccAPI::TerminateProcess() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

BOOL
CodebenderccAPI::CloseHandle(HANDLE hObject)
{
    BOOL rc;

    rc = ::CloseHandle(hObject);
    if (rc != 0)
        return rc;

    std::string err_msg = "CodebenderccAPI::CloseHandle() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

HANDLE
CodebenderccAPI::OpenProcess(DWORD dwDesiredAccess,
                            BOOL bInheritHandle,
                            DWORD dwProcessId)
{
    HANDLE rc;

    rc = ::OpenProcess(dwDesiredAccess,
                      bInheritHandle,
                      dwProcessId);
    if (rc != NULL)
        return rc;

    std::string err_msg = "CodebenderccAPI::OpenProcess() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

HANDLE
CodebenderccAPI::CreateToolhelp32Snapshot(DWORD dwFlags,
                                         DWORD th32ProcessID)
{
    HANDLE rc;

    rc = ::CreateToolhelp32Snapshot(dwFlags,
                                     th32ProcessID);
    if (rc != INVALID_HANDLE_VALUE)
        return rc;

    std::string err_msg = "CodebenderccAPI::CreateToolhelp32Snapshot() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

BOOL
CodebenderccAPI::Process32First(HANDLE hSnapshot,
                              LPPROCESSENTRY32 lppe)
{
    BOOL rc;

    rc = ::Process32First(hSnapshot,
                         lppe);
    if (rc != 0)
        return rc;

    std::string err_msg = "CodebenderccAPI::Process32First() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

BOOL
CodebenderccAPI::Process32Next(HANDLE hSnapshot,
                              LPPROCESSENTRY32 lppe)
{
    BOOL rc;

    rc = ::Process32Next(hSnapshot,
                         lppe);
    if (rc != 0)
        return rc;

    DWORD err = GetLastError();

    if(err == ERROR_NO_MORE_FILES)
        return rc;

    std::string err_msg = "CodebenderccAPI::Process32Next() - extended error information: ";
    err_msg += boost::lexical_cast<std::string>(GetLastError());

    error_notify(err_msg);
    return rc;
}

#endif

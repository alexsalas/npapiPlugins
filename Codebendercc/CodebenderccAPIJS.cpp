#include "CodebenderccAPI.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////public//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

FB::variant CodebenderccAPI::flash(const std::string& device,
                                   const std::string& code,
                                   const std::string& maxsize,
                                   const std::string& protocol,
                                   const std::string& disable_flushing,
                                   const std::string& speed,
                                   const std::string& mcu,
                                   const FB::JSObjectPtr &flash_callback) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::flash",3);
    #ifdef _WIN32 // Check if finding the short path of the plugin failed.
        if (current_dir == L""){
            flash_callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(-2));
        return 0;}
    #endif

    int error_code = 0;
    if (!validate_device(device)) error_code = -4;
    if (!validate_code(code)) error_code = -5;
    if (!validate_number(maxsize)) error_code = -6;
    if (!validate_number(speed)) error_code = -7;
    if (!validate_charnum(protocol)) error_code = -8;
    if (!validate_charnum(mcu)) error_code = -9;
    if (!validate_charnum(disable_flushing)) error_code = -30;
    if (error_code != 0){
        flash_callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(error_code));
        return 0;}

    boost::thread* t = new boost::thread(boost::bind(&CodebenderccAPI::doflash,
                                        this,
                                        device,
                                        code,
                                        maxsize,
                                        protocol,
                                        disable_flushing,
                                        speed,
                                        mcu,
                                        flash_callback));
    thr = t;
    CodebenderccAPI::debugMessage("CodebenderccAPI::flash ended",3);
    return 0;
}catch (...) {
    error_notify("CodebenderccAPI::flash() threw an unknown exception");
    return 0;}

FB::variant CodebenderccAPI::flashWithProgrammer(const std::string& device,
                                                const std::string& code,
                                                const std::string& maxsize,
                                                const std::string& programmerProtocol,
                                                const std::string& programmerCommunication,
                                                const std::string& programmerSpeed,
                                                const std::string& programmerForce,
                                                const std::string& programmerDelay,
                                                const std::string& mcu,
                                                const FB::JSObjectPtr & cback) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::flashWithProgrammer",3);
    #ifdef _WIN32 // Check if finding the short path of the plugin failed.
        if (current_dir == L""){
            cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(-2));
            return 0;}
    #endif

    /** Input validation. The error codes returned correspond to
    messages printed by the javascript of the website **/
    if (!validate_code(code)) return -2;
    if (!validate_number(maxsize)) return -3;
    std::map<std::string, std::string> programmerData;
    int progValidation = programmerPrefs(device,
                                        programmerProtocol,
                                        programmerSpeed,
                                        programmerCommunication,
                                        programmerForce,
                                        programmerDelay,
                                        mcu,
                                        programmerData);
    if (progValidation != 0){
        cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(progValidation));
        return 0;}
    /**Validation end**/

   boost::thread* t = new boost::thread(boost::bind(&CodebenderccAPI::doflashWithProgrammer,
                                        this, 
                                        device, 
                                        code, 
                                        maxsize, 
                                        programmerData, 
                                        mcu, 
                                        cback));  
   thr=t;
   CodebenderccAPI::debugMessage("CodebenderccAPI::flashWithProgrammer ended",3);
   return 0;
}catch (...) {
   error_notify("CodebenderccAPI::flashWithProgrammer() threw an unknown exception");
   return 0;
}

FB::variant CodebenderccAPI::flashBootloader(const std::string& device,
                                            const std::string& programmerProtocol,
                                            const std::string& programmerCommunication,
                                            const std::string& programmerSpeed,
                                            const std::string& programmerForce,
                                            const std::string& programmerDelay,
                                            const std::string& highFuses, const std::string& lowFuses,
                                            const std::string& extendedFuses,
                                            const std::string& unlockBits,
                                            const std::string& lockBits,
                                            const std::string& mcu,
                                            const FB::JSObjectPtr & cback) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::flashBootloader",3);

    #ifdef _WIN32 // Check if finding the short path of the plugin failed.
    if (current_dir == L""){
        cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(-2));
        return 0;}
    #endif

    /**Input validation. The error codes returned correspond to
    messages printed by the javascript of the website **/
    std::map<std::string, std::string> programmerData;
    int progValidation = programmerPrefs(device,
                                        programmerProtocol,
                                        programmerSpeed,
                                        programmerCommunication,
                                        programmerForce,
                                        programmerDelay,
                                        mcu,
                                        programmerData);

    if (progValidation != 0){
        cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(progValidation));
        return 0;}

    std::map<std::string, std::string> bootloaderData;
    int bootValidation = bootloaderPrefs(lowFuses,
                                         highFuses,
                                         extendedFuses,
                                         unlockBits,
                                         lockBits,
                                         bootloaderData);
    if (bootValidation != 0){
        cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(bootValidation));
        return 0;}
    /**  Validation end **/

    boost::thread* t = new boost::thread(boost::bind(&CodebenderccAPI::doflashBootloader,
                                                    this,
                                                    device,
                                                    programmerData,
                                                    bootloaderData,
                                                    mcu,
                                                    cback));
    thr = t;
    CodebenderccAPI::debugMessage("CodebenderccAPI::flashBootloader ended",3);
    return 0;
}catch (...) {
    error_notify("CodebenderccAPI::flashBootloader() threw an unknown exception");
    return 0;
}

bool CodebenderccAPI::setCallback(const FB::JSObjectPtr &callback) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::setCallback",3);
    callback_ = callback;
    CodebenderccAPI::debugMessage("CodebenderccAPI::setCallback ended",3);
    return true;
}catch (...) {
    error_notify("CodebenderccAPI::setCallback() threw an unknown exception");
    return true;
}

bool CodebenderccAPI::setErrorCallback(const FB::JSObjectPtr &error_callback) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::setErrorCallback",3);
    error_callback_ = error_callback;
    CodebenderccAPI::debugMessage("CodebenderccAPI::setErrorCallback ended",3);
    return true;
} catch (...) {
    error_notify("CodebenderccAPI::setErrorCallback() threw an unknown exception");
    return true;
}

void CodebenderccAPI::serialWrite(const std::string & message) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::serialWrite",3);
    std::string mess = message;
    size_t bytes_read;
    if(serialPort.isOpen()){
        try{
            bytes_read = serialPort.write(mess);
            if(bytes_read != 0){
                std::string portMessage = "Wrote to port: " + mess + " ";
                CodebenderccAPI::debugMessage(portMessage.c_str(),1);}
        }catch(serial::PortNotOpenedException& pno){
            CodebenderccAPI::debugMessage(pno.what(),2);
            error_notify(pno.what());
            notify("disconnect");
            CodebenderccAPI::disconnect();
            return;}
        catch(serial::SerialException& se){
            CodebenderccAPI::debugMessage(se.what(),2);
            error_notify(se.what());
            notify("disconnect");
            CodebenderccAPI::disconnect();
            return;}
        catch(serial::IOException& IOe){
            CodebenderccAPI::debugMessage(IOe.what(),2);
            error_notify(IOe.what());
            notify("disconnect");
            CodebenderccAPI::disconnect();
            return;}
    }else {
        CodebenderccAPI::debugMessage("CodebenderccAPI::serialWrite port not open",1);
        perror("null");}
    CodebenderccAPI::debugMessage("CodebenderccAPI::serialWrite ended",3);
} catch (...) {
    error_notify("CodebenderccAPI::serialWrite() threw an unknown exception");
 }

FB::variant CodebenderccAPI::disconnect() try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::disconnect",3);
    if(!(serialPort.isOpen()))
        return 1;
    try{
        CodebenderccAPI::closePort(false);
    }catch(...){
        CodebenderccAPI::debugMessage("CodebenderccAPI::disconnect close port exception",2);
        error_notify("CodebenderccAPI::disconnect close port threw an unknown exception");
        return 0;}
    CodebenderccAPI::debugMessage("CodebenderccAPI::disconnect ended",3);
    return 1;
} catch (...) {
    error_notify("CodebenderccAPI::disconnect() threw an unknown exception");
    return 0;
}

bool CodebenderccAPI::serialRead(const std::string &port,
                                const std::string &baudrate,
                                const FB::JSObjectPtr
                                &callback,
                                const FB::JSObjectPtr &valHandCallback) try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::serialRead",3);
    std::string message = "connecting at ";
    message += baudrate;
    message += '\n';
    callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(message));
    unsigned int brate = boost::lexical_cast<unsigned int, std::string > (baudrate);
    boost::thread* t = new boost::thread(boost::bind(&CodebenderccAPI::serialReader,
                                                    this,
                                                    port,
                                                    brate,
                                                    callback,
                                                    valHandCallback));
    thr = t;
    CodebenderccAPI::debugMessage("CodebenderccAPI::serialRead ended",3);
    return true; // the thread is started
}catch (...) {
    error_notify("CodebenderccAPI::serialRead() threw an unknown exception");
    return true;
}

void CodebenderccAPI::closeTab(){
    if (thr!=NULL){
        thr->interrupt();
        thr->join();
        delete thr;
        thr=NULL;}
}
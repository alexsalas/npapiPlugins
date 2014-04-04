#include "CodebenderccAPI.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////public//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

FB::variant CodebenderccAPI::flash(const std::string& device, const std::string& code, const std::string& maxsize, const std::string& protocol, const std::string& speed, const std::string& mcu, const FB::JSObjectPtr &flash_callback) {
	CodebenderccAPI::debugMessage("CodebenderccAPI::flash",3);	
		#if defined _WIN32||_WIN64	// Check if finding the short path of the plugin failed.
			if (current_dir == L""){
				flash_callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(-2));
				return 0;
			}
		#endif
			int error_code = 0;
			if (!validate_device(device)) error_code = -4;
			if (!validate_code(code)) error_code = -5;
			if (!validate_number(maxsize)) error_code = -6;
			if (!validate_number(speed)) error_code = -7;
			if (!validate_charnum(protocol)) error_code = -8;
			if (!validate_charnum(mcu)) error_code = -9;
			if (error_code != 0){
				flash_callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(error_code));
				return 0;
			}	
		boost::thread* t = new boost::thread(boost::bind(&CodebenderccAPI::doflash, this, device, code, maxsize, protocol, speed, mcu, flash_callback));
	CodebenderccAPI::debugMessage("CodebenderccAPI::flash ended",3);		
	return 0;
}

FB::variant CodebenderccAPI::flashWithProgrammer(const std::string& device, const std::string& code, const std::string& maxsize, const std::string& programmerProtocol, const std::string& programmerCommunication, const std::string& programmerSpeed, const std::string& programmerForce, const std::string& programmerDelay, const std::string& mcu, const FB::JSObjectPtr & cback) {
	CodebenderccAPI::debugMessage("CodebenderccAPI::flashWithProgrammer",3);
		#if defined _WIN32||_WIN64	// Check if finding the short path of the plugin failed.
			if (current_dir == L""){
				cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(-2));
				return 0;
			}
		#endif
		/**
		  *  Input validation. The error codes returned correspond to 
		  *	 messages printed by the javascript of the website
		  **/
		if (!validate_code(code)) return -2;
		if (!validate_number(maxsize)) return -3;
		std::map<std::string, std::string> programmerData;
		int progValidation = programmerPrefs(device, programmerProtocol, programmerSpeed, programmerCommunication, programmerForce, programmerDelay, mcu, programmerData);
		if (progValidation != 0){
			cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(progValidation));
			return 0;
		}
		/**
		  * Validation end
		  **/		
		boost::thread* t = new boost::thread(boost::bind(&CodebenderccAPI::doflashWithProgrammer,
		this, device, code, maxsize, programmerData, mcu, cback));	
	CodebenderccAPI::debugMessage("CodebenderccAPI::flashWithProgrammer ended",3);
    return 0;
}

FB::variant CodebenderccAPI::flashBootloader(const std::string& device, const std::string& programmerProtocol, const std::string& programmerCommunication, const std::string& programmerSpeed, const std::string& programmerForce, const std::string& programmerDelay, const std::string& highFuses, const std::string& lowFuses, const std::string& extendedFuses, const std::string& unlockBits, const std::string& lockBits, const std::string& mcu, const FB::JSObjectPtr & cback) {
	CodebenderccAPI::debugMessage("CodebenderccAPI::flashBootloader",3);
	#if defined _WIN32||_WIN64	// Check if finding the short path of the plugin failed.
		if (current_dir == L""){
			cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(-2));
			return 0;
		}
	#endif
	/**
	  *  Input validation. The error codes returned correspond to 
	  *	 messages printed by the javascript of the website
	  **/
	std::map<std::string, std::string> programmerData;
	int progValidation = programmerPrefs(device, programmerProtocol, programmerSpeed, programmerCommunication, programmerForce, programmerDelay, mcu, programmerData);
	if (progValidation != 0){
		cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(progValidation));
        return 0;
	}

	std::map<std::string, std::string> bootloaderData;
	int bootValidation = bootloaderPrefs(lowFuses, highFuses, extendedFuses, unlockBits, lockBits, bootloaderData);
	if (bootValidation != 0){
		cback->InvokeAsync("", FB::variant_list_of(shared_from_this())(bootValidation));
        return 0;
	}
	/**
	  * Validation end
	  **/
	boost::thread* t = new boost::thread(boost::bind(&CodebenderccAPI::doflashBootloader,
		this, device, programmerData, bootloaderData, mcu, cback));
	CodebenderccAPI::debugMessage("CodebenderccAPI::flashBootloader ended",3);
	return 0;
}

bool CodebenderccAPI::setCallback(const FB::JSObjectPtr &callback) {
	CodebenderccAPI::debugMessage("CodebenderccAPI::setCallback",3);	
	callback_ = callback;
	CodebenderccAPI::debugMessage("CodebenderccAPI::setCallback ended",3);
    return true;
}

void CodebenderccAPI::serialWrite(const std::string & message) {
	CodebenderccAPI::debugMessage("CodebenderccAPI::serialWrite",3);
    std::string mess = message;
	size_t bytes_read;
	try
	{
		if(serialPort.isOpen()){
			
			bytes_read = serialPort.write(mess);
			if(bytes_read != 0){
				perror("Wrote to port");
				std::string portMessage = "Wrote to port: " + mess + " ";
				CodebenderccAPI::debugMessage(portMessage.c_str(),1);
			}
		}
		else {
			CodebenderccAPI::debugMessage("CodebenderccAPI::serialWrite port not open",1);
			perror("null");
		}
	}catch(...){
		CodebenderccAPI::debugMessage("CodebenderccAPI::serialWrite open serial port exception",1);
		notify("disconnect");
	}
	CodebenderccAPI::debugMessage("CodebenderccAPI::serialWrite ended",3);
}

FB::variant CodebenderccAPI::disconnect() {
	CodebenderccAPI::debugMessage("CodebenderccAPI::disconnect",3);
	if(!(serialPort.isOpen()))
		return 1;
	try{
			CodebenderccAPI::closePort();
		}catch(...){
		CodebenderccAPI::debugMessage("CodebenderccAPI::disconnect close port exception",2);
		}
	CodebenderccAPI::debugMessage("CodebenderccAPI::disconnect ended",3);
	return 1;	
}

bool CodebenderccAPI::serialRead(const std::string &port, const std::string &baudrate, const FB::JSObjectPtr &callback) {
	CodebenderccAPI::debugMessage("CodebenderccAPI::serialRead",3);
	std::string message = "connecting at ";
    message += baudrate;
    callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(message));
    unsigned int brate = boost::lexical_cast<unsigned int, std::string > (baudrate);
    boost::thread* t = new boost::thread(boost::bind(&CodebenderccAPI::serialReader, this, port, brate, callback));
    CodebenderccAPI::debugMessage("CodebenderccAPI::serialRead ended",3);
	return true; // the thread is started
}
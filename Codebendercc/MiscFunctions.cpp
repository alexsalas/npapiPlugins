#include "CodebenderccAPI.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////public//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

std::string CodebenderccAPI::QueryKey(HKEY hKey) try {

    DWORD    cValues;              // number of values for key
    DWORD    cchMaxValue;          // longest value name (characters)
    DWORD    cbMaxValueData;       // longest value data (bytes)
    
    DWORD i, retCode; 
 
    TCHAR    achValue[MAX_KEY_LENGTH];
    DWORD    cchValue = MAX_KEY_LENGTH;

    std::string ports = "";

    // Get the registry key value count.
    retCode = CodebenderccAPI::RegQueryInfoKey(
        hKey,                     // An open registry key handle.
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &cValues,                // A pointer to a variable that receives the number of values that are associated with the key.
        &cchMaxValue,            // A pointer to a variable that receives the size of the key's longest value name, in Unicode characters.
        &cbMaxValueData,         // A pointer to a variable that receives the size of the longest data component among the key's values, in bytes.
        NULL,
        NULL);

    // Enumerate the key values.

    BYTE* buffer = new BYTE[cbMaxValueData];

    if (cValues)                // cValues is the count of values found in the SERIALCOMM subkey
    {
        for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
        { 
            cchValue = MAX_KEY_LENGTH;
            achValue[0] = '\0'; 
            retCode = CodebenderccAPI::RegEnumValue(
                hKey,             // A handle to an open registry key
                i,                // The index of the value to be retrieved
                achValue,         // A pointer to a buffer that receives the name of the value as a null-terminated string
                &cchValue,        // A pointer to a variable that specifies the size of the buffer pointed to by the achValue parameter, in characters.
                NULL,             // lpReserved parameter is reserved and must be NULL
                NULL,
                NULL,
                NULL);

            if (retCode == ERROR_SUCCESS )
            { 
                DWORD lpData = cbMaxValueData;
                buffer[0] = '\0';
                LONG dwRes = CodebenderccAPI::RegQueryValueEx(hKey, achValue, 0, NULL, (LPBYTE)buffer, &lpData);
                if (dwRes == ERROR_SUCCESS)
                {
                    std::string str( reinterpret_cast<char const*>(buffer) , (int) lpData );
                    std::string tmp="";
                    for (int k=0;k<(int)lpData;k++){
                        if(buffer[k]!=0)             // Check the buffer for bytes that contain zeros.
                            tmp+=buffer[k];}
                    ports.append(tmp);               // Append port to the list of ports.
                    if (i != (cValues - 1))
                        ports.append(",");
                }
            }
        }
    }
    delete [] buffer;
    return ports;
}catch (...) {
    error_notify("CodebenderccAPI::QueryKey() threw an unknown exception");
    return "";
}
#endif

void CodebenderccAPI::enableDebug(int debugLevel) try{
    if(debugLevel >= 1 && debugLevel <= 3) {
        debug_ = true;
        currentLevel= debugLevel;
        if (currentLevel==3){
            if (!(debugFile.is_open()))
                debugFile.open(debugFilename.c_str());}
        else {
            if (debugFile.is_open())
                debugFile.close();}
    }
    else{
        if (debugFile.is_open())
            debugFile.close();
        m_host->htmlLog("Level set in enableDebug is not valid.");
        }
}catch (...) {
    error_notify("CodebenderccAPI::enableDebug() threw an unknown exception");
}

void CodebenderccAPI::disableDebug() try{
    debug_ = false;
    if (currentLevel==3){
        if (debugFile.is_open())
            debugFile.close();
    }
} catch (...) {
    error_notify("CodebenderccAPI::disableDebug() threw an unknown exception");
}


bool CodebenderccAPI::checkDebug() {
    return debug_;
}

void CodebenderccAPI::debugMessageProbe(const char * messageDebug, int minimumLevel) try {
    if (CodebenderccAPI::checkDebug() && minimumLevel <= currentLevel && probeFlag==false){
        probeFlag=true;
        m_host->htmlLog(messageDebug);
        if (currentLevel==3){
            if (debugFile.is_open()){
                debugFile << messageDebug;
                debugFile << "\n";
            }
        }
    }
}catch (...) {
    error_notify("CodebenderccAPI::debugMessageProbe() threw an unknown exception");
}

void CodebenderccAPI::debugMessage(const char * messageDebug, int minimumLevel) try {
    if (CodebenderccAPI::checkDebug() && minimumLevel <= currentLevel)
        m_host->htmlLog(messageDebug);
        probeFlag=false;
        if (currentLevel==3){
            if (debugFile.is_open()){
                debugFile << messageDebug;
                debugFile << "\n";
            }
        }
}catch (...) {
    error_notify("CodebenderccAPI::debugMessage() threw an unknown exception");
}

CodebenderccPtr CodebenderccAPI::getPlugin() {
    CodebenderccAPI::debugMessage("CodebenderccAPI::getPlugin",3);
    CodebenderccPtr plugin(m_plugin.lock());
    if (!plugin) {
        CodebenderccAPI::debugMessage("CodebenderccAPI::getPlugin invalid plugin",2);
        throw FB::script_error("The plugin is invalid");
    }
    CodebenderccAPI::debugMessage("CodebenderccAPI::getPlugin ended",3);
    return plugin;
}

// Read-only property version

std::string CodebenderccAPI::get_version() {
    CodebenderccAPI::debugMessage("CodebenderccAPI::get_version",3);
    return FBSTRING_PLUGIN_VERSION;
}

FB::variant CodebenderccAPI::getLastCommand() {
    CodebenderccAPI::debugMessage("CodebenderccAPI::getLastCommand",3);
    return lastcommand;
}

std::string CodebenderccAPI::getFlashResult() try {
    CodebenderccAPI::debugMessage("CodebenderccAPI::getFlashResult",3);
    FILE *pFile;
    #ifdef _WIN32
        std::string filename = FB::wstring_to_utf8(outfile);
        pFile = CodebenderccAPI::fopen(filename.c_str(), "r");
    #else
        pFile = CodebenderccAPI::fopen(outfile.c_str(), "r");
    #endif
    char buffer[128];
    std::string result = "";

    if (pFile == NULL)
        return result;

    while (!feof(pFile)) {
        if (CodebenderccAPI::fgets(buffer, 128, pFile) != NULL)
            result += buffer;
    }
    CodebenderccAPI::fclose(pFile);
    CodebenderccAPI::debugMessage("CodebenderccAPI::getFlashResult ended",3);
    return result;
}catch (...) {
    error_notify("CodebenderccAPI::getFlashResult() threw an unknown exception");
    return "";
}



////////////////////////////////////////////////////////////////////////////////
/////////////////////////////PRIVATE////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * decode base64 encoded data
 *
 * @param source the encoded data (zero terminated)
 * @param target pointer to the target buffer
 * @param targetlen length of the target buffer
 * @return length of converted data on success, -1 otherwise
 */
size_t CodebenderccAPI::base64_decode(const char *source, unsigned char *target, size_t targetlen) try {
    
    char *src, *tmpptr;
    char quadruple[4];
    unsigned char tmpresult[3];
    int i;
    size_t tmplen = 3;
    int converted = 0;

    /* concatinate '===' to the source to handle unpadded base64 data */
    src = (char *) malloc(strlen(source) + 5);
    if (src == NULL)
        return -1;
    strcpy(src, source);
    strcat(src, "====");
    tmpptr = src;

    /* convert as long as we get a full result */
    while (tmplen == 3) {
        /* get 4 characters to convert */
        for (i = 0; i < 4; i++) {
            /* skip invalid characters - we won't reach the end */
            while (*tmpptr != '=' && _base64_char_value(*tmpptr) < 0)
                tmpptr++;

            quadruple[i] = *(tmpptr++);
        }

        /* convert the characters */
        tmplen = _base64_decode_triple(quadruple, tmpresult);

        /* check if the fit in the result buffer */
        if (targetlen < tmplen) {
            free(src);
            return -1;
        }

        /* put the partial result in the result buffer */
        memcpy(target, tmpresult, tmplen);
        target += tmplen;
        targetlen -= tmplen;
        converted += tmplen;
    }

    free(src);

    return converted;
}catch (...) {
    error_notify("CodebenderccAPI::base64_decode() threw an unknown exception");
    return 0;
}

/**
 * determine the value of a base64 encoding character
 *
 * @param base64char the character of which the value is searched
 * @return the value in case of success (0-63), -1 on failure
 */
int CodebenderccAPI::_base64_char_value(char base64char) try {

    if (base64char >= 'A' && base64char <= 'Z')
        return base64char - 'A';
    if (base64char >= 'a' && base64char <= 'z')
        return base64char - 'a' + 26;
    if (base64char >= '0' && base64char <= '9')
        return base64char - '0' + 2 * 26;
    if (base64char == '+')
        return 2 * 26 + 10;
    if (base64char == '/')

        return 2 * 26 + 11;
    return -1;
}catch (...) {
    error_notify("CodebenderccAPI::_base64_char_value() threw an unknown exception");
    return -1;
}

/**
 * decode a 4 char base64 encoded byte triple
 *
 * @param quadruple the 4 characters that should be decoded
 * @param result the decoded data
 * @return length of the result (1, 2 or 3), 0 on failure
 */
int CodebenderccAPI::_base64_decode_triple(char quadruple[4], unsigned char *result) try {

    int i, triple_value, bytes_to_decode = 3, only_equals_yet = 1;
    int char_value[4];

    for (i = 0; i < 4; i++)
        char_value[i] = _base64_char_value(quadruple[i]);

    /* check if the characters are valid */
    for (i = 3; i >= 0; i--) {
        if (char_value[i] < 0) {
            if (only_equals_yet && quadruple[i] == '=') {
                /* we will ignore this character anyway, make it something
                 * that does not break our calculations */
                char_value[i] = 0;
                bytes_to_decode--;
                continue;
            }
            return 0;
        }
        /* after we got a real character, no other '=' are allowed anymore */
        only_equals_yet = 0;
    }

    /* if we got "====" as input, bytes_to_decode is -1 */
    if (bytes_to_decode < 0)
        bytes_to_decode = 0;

    /* make one big value out of the partial values */
    triple_value = char_value[0];
    triple_value *= 64;
    triple_value += char_value[1];
    triple_value *= 64;
    triple_value += char_value[2];
    triple_value *= 64;
    triple_value += char_value[3];

    /* break the big value into bytes */
    for (i = bytes_to_decode; i < 3; i++)
        triple_value /= 256;
    for (i = bytes_to_decode - 1; i >= 0; i--) {

        result[i] = triple_value % 256;
        triple_value /= 256;
    }

    return bytes_to_decode;
} catch (...) {
    error_notify("CodebenderccAPI::_base64_decode_triple() threw an unknown exception");
    return 0;
}

int CodebenderccAPI::programmerPrefs(const std::string& port,
                                    const std::string& programmerProtocol,
                                    const std::string&  programmerSpeed,
                                    const std::string& programmerCommunication,
                                    const std::string& programmerForce,
                                    const std::string& programmerDelay,
                                    const std::string& mcu, std::map<std::string, std::string>& programmerData) try {

    CodebenderccAPI::debugMessage("CodebenderccAPI::programmerPrefs",3);

    /** Validate the programmer parameters. **/
    if (!validate_number(programmerSpeed)) return -10;
    if (!validate_charnum(programmerProtocol)) return -11;
    if (!validate_charnum(mcu)) return -12;
    if (programmerProtocol != "usbtiny" && programmerProtocol != "dapa"){
        if (!validate_charnum(programmerCommunication)) return -13;
        if (programmerCommunication == "serial")
            if (!validate_device(port)) return -14;
    }
    if (!validate_charnum(programmerForce)) return -15;
    if (!validate_number(programmerDelay)) return -16;

    /** Pass the programmer parameters to a map.**/
    programmerData["protocol"] = programmerProtocol.c_str();
    programmerData["communication"] = programmerCommunication.c_str();
    programmerData["speed"] = programmerSpeed.c_str();
    programmerData["force"] = programmerForce.c_str();
    programmerData["delay"] = programmerDelay.c_str();

    CodebenderccAPI::debugMessage("CodebenderccAPI::programmerPrefs ended",3);

    return 0;
}catch (...) {
    error_notify("CodebenderccAPI::programmerPrefs() threw an unknown exception");
    return 0;
}

int CodebenderccAPI::bootloaderPrefs(const std::string& lFuses,
                                    const std::string& hFuses,
                                    const std::string& eFuses,
                                    const std::string& ulBits,
                                    const std::string& lBits, std::map<std::string, std::string>& data) try {

    CodebenderccAPI::debugMessage("CodebenderccAPI::bootloaderPrefs",3);

    /** Validate the bootloader parameters. **/
    if (lFuses == "" || !validate_hex(lFuses)) return -17;
    if (hFuses == "" || !validate_hex(hFuses)) return -18;
    if (eFuses != "" && !validate_hex(eFuses)) return -19;
    if (ulBits != "" && !validate_hex(ulBits)) return -20;
    if (lBits != "" && !validate_hex(lBits)) return -21;

    /** Pass the programmer parameters to a map. **/
    data["hfuses"] = hFuses.c_str();
    data["lfuses"] = lFuses.c_str();
    data["efuses"] = eFuses.c_str();
    data["ulbits"] = ulBits.c_str();
    data["lbits"] = lBits.c_str();

    CodebenderccAPI::debugMessage("CodebenderccAPI::bootloaderPrefs ended",3);

    return 0;
}catch (...) {
    error_notify("CodebenderccAPI::bootloaderPrefs() threw an unknown exception");
    return 0;
}

int CodebenderccAPI::flushBuffer(const std::string& port) try {

     CodebenderccAPI::debugMessage("CodebenderccAPI::flushBuffer",3);
    int openPortStatus=CodebenderccAPI::openPort(port, 9600, true, "CodebenderccAPI::flushBuffer - ");
    if(openPortStatus!=1)
        return openPortStatus;
     try{
         serialPort.flushInput();
         serialPort.flushOutput();

        serialPort.setDTR(false);
        serialPort.setRTS(false);

         delay(100);

         serialPort.setDTR(true);
         serialPort.setRTS(true);
     }
    catch(serial::SerialException& se){
        std::string err_msg = boost::lexical_cast<std::string>(se.what());
        std::string result = "flushBuffer exception: " + err_msg;
        error_notify(result, 1);
        return -1;
    }
    catch(serial::IOException& IOe){  
        std::string err_msg = boost::lexical_cast<std::string>(IOe.what());
        std::string result = "flushBuffer exception: " + err_msg;
        error_notify(result, 1);
        return -1;
    }
    catch(serial::PortNotOpenedException& pno){
        std::string err_msg = boost::lexical_cast<std::string>(pno.what());
        std::string result = "flushBuffer exception: " + err_msg;
        error_notify(result, 1);
        return -1;
    }
 
     CodebenderccAPI::closePort(true);

     CodebenderccAPI::debugMessage("CodebenderccAPI::flushBuffer ended",3);

    return 0;
 }catch(...) {
     error_notify("CodebenderccAPI::flushBuffer() threw an unknown exception");
     return -1;
 }

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////validations//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool CodebenderccAPI::validate_hex(const std::string & input) try {
    return (input.compare(0, 2, "0x") == 0
    && input.size() > 2 && input.size() <= 4
    && input.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos);
}catch (...) {
    error_notify("CodebenderccAPI::validate_hex() threw an unknown exception");
    return false;
}

bool CodebenderccAPI::validate_number(const std::string & input) try {

    try {
        boost::lexical_cast<double>(input);
        return true;
    } catch (boost::bad_lexical_cast &) {

        return false;
    }
}catch (...) {
    error_notify("CodebenderccAPI::validate_number() threw an unknown exception");
    return false;
}

bool CodebenderccAPI::validate_device(const std::string & input) try {

    static const boost::regex acm("\\/dev\\/ttyACM[[:digit:]]+");
    static const boost::regex usb("\\/dev\\/ttyUSB[[:digit:]]+");
    static const boost::regex com("COM[[:digit:]]+");
    static const boost::regex cu("\\/dev\\/cu.[0-9a-zA-Z\\-\\s(=>_)]+");

    return boost::regex_match(input, acm)
        || boost::regex_match(input, usb)
        || boost::regex_match(input, com)
        || boost::regex_match(input, cu)
        ;
}catch (...) {
    error_notify("CodebenderccAPI::validate_device() threw an unknown exception");
    return false;
}

bool CodebenderccAPI::validate_code(const std::string & input) try {

    static const boost::regex base64("[0-9a-zA-Z+\\/=\n]+");

    return boost::regex_match(input, base64);
}catch (...) {
    error_notify("CodebenderccAPI::validate_code() threw an unknown exception");
    return false;
}

bool CodebenderccAPI::validate_charnum(const std::string & input) try {

    static const boost::regex charnum("[0-9a-zA-Z]*");
    return boost::regex_match(input, charnum);
}catch (...) {
    error_notify("CodebenderccAPI::validate_charnum() threw an unknown exception");
    return false;
}

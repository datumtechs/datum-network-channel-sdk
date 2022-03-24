#pragma once
#include <iostream>
#include <stdarg.h>
#include <map>
#include <algorithm>
#include <regex>
#include <time.h>
#include "event_exception.h"
using namespace std;
const string C_Servant_Adapter_Name = "IoChannel";
const string C_Servant_Endpoints_Key = C_Servant_Adapter_Name + ".Endpoints";
const string C_Servant_AdapterId_Key = C_Servant_Adapter_Name + ".AdapterId";
const string C_Server_Proxy_Key = "IoChannel.Proxy";

// ServantId = C_Servant_Id_Prefix + "_" + NodeId
const string C_Servant_Id_Prefix = "IoChannel";
// ServantApapterId = C_Servant_Adapter_Id_Prefix + TaskId + "_" + NodeId
// 用于Glacier2寻址
const string C_Servant_Adapter_Id_Prefix = "ChannelAdapter_";

// Glacier2 Configuration
const string C_Glacier2_Router_Key = "Ice.Default.Router";

// IceGrid Configuration
const string C_IceGrid_Locator_Key = "Ice.Default.Locator";

const string C_MAX_SIZE_KEY = "Ice.MessageSizeMax";

const string C_EVENT_CODE_PARSE_FILE = "0402000";
const string C_EVENT_CODE_NO_FIND_NID = "0402001";
const string C_EVENT_CODE_NO_PUBLIC_IP = "0402002";
const string C_EVENT_CODE_NO_GLACIER2 = "0402003";
const string C_EVENT_CODE_NO_ICEGRID = "0402004";
const string C_EVENT_CODE_INVALID_CERT = "0402005";
const string C_EVENT_CODE_START_SERVICE = "0402006";
const string C_EVENT_CODE_CREATE_CLIENT = "0402007";
const string C_EVENT_CODE_INVALID_PROXY = "0402008";
const string C_EVENT_CODE_CONNECT_TIMEOUT = "0402009";
const string C_EVENT_CODE_SEND_DATA_TIMEOUT = "0402010";

const map<string, string> C_MAP_EVENTS = {
    {C_EVENT_CODE_PARSE_FILE, "Failed to parse the configuration file, Please check!"},
    {C_EVENT_CODE_NO_FIND_NID, "No found NodeID:%s, get node info failed!"},
    {C_EVENT_CODE_NO_PUBLIC_IP, "The service node:%s doesn't configure PUBLIC_IP, create io channel failed!"},
    {C_EVENT_CODE_NO_GLACIER2, "The service node:%s doesn't configure Glacier2 info, create io channel failed!"},
    {C_EVENT_CODE_NO_ICEGRID, "The service node:%s doesn't configure IceGrid info, create io channel failed!"},
    {C_EVENT_CODE_INVALID_CERT, "Invalid certificate, node id:%s, create io channel failed!"},
    {C_EVENT_CODE_START_SERVICE, "Start the service node:%s failed, reason:%s, create io channel failed!"},
    {C_EVENT_CODE_CREATE_CLIENT, "Create the client node:%s failed, reason:%s, create io channel failed!"},
    {C_EVENT_CODE_INVALID_PROXY, "Invalid proxy, node id:%s, create io channel failed!"},
    {C_EVENT_CODE_CONNECT_TIMEOUT, "Connect to remote nodeid:%s timeout, The timeout period is:%ldms, create io channel failed!"},
    {C_EVENT_CODE_SEND_DATA_TIMEOUT, "Self nodeid:%s send data to nodeid:%s timeout, The timeout period is: %ldms, send data failed!"}
};


static void HANDLE_EXCEPTION_EVENT(const string& strEventCode, const string& taskid, ...) {
    const int nMaxLen = 512;
	string strErrMsg(nMaxLen, 0);
    string strFmt = C_MAP_EVENTS.at(strEventCode);
	va_list args;
	va_start(args, strFmt);
	vsnprintf(&strErrMsg[0], nMaxLen, strFmt.c_str(), args);
	va_end(args);

    throw EventException(strErrMsg);
/*  std::regex newlines_re("\n+");
    strErrMsg = std::regex_replace(strErrMsg, newlines_re, "");
    
    // Event Struct
    const char* pEventInfo = "{\"type\":\"%s\", \"taskId\":\"%s\", \"content\":\"%s\", \"createTime\":\"%s\"}";
    // Get Current Time
    time_t t = time(0); 
    char timeTmp[32] = {0};
    strftime(timeTmp, sizeof(timeTmp), "%Y-%m-%d %H:%M:%S",localtime(&t)); 

    string strEventInfo(nMaxLen, 0);
    sprintf(&strEventInfo[0], pEventInfo, strEventCode.c_str(), taskid.c_str(), strErrMsg.c_str(), timeTmp);
    // cout << strEventInfo << endl;
    throw EventException(strEventInfo);
*/
}

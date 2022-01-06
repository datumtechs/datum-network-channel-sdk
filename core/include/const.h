#pragma once
#include <iostream>
const std::string C_Servant_Adapter_Name = "IoChannel";
const std::string C_Servant_Endpoints_Key = C_Servant_Adapter_Name + ".Endpoints";
const std::string C_Servant_AdapterId_Key = C_Servant_Adapter_Name + ".AdapterId";
const std::string C_Server_Proxy_Key = "IoChannel.Proxy";
const std::string C_Server_Tcp_Local = "tcp -h localhost";

// ServantId = C_Servant_Id_Prefix + "_" + NodeId
const std::string C_Servant_Id_Prefix = "IoChannel";
// ServantApapterId = C_Servant_Adapter_Id_Prefix + TaskId + "_" + NodeId
// 用于Glacier2寻址
const std::string C_Servant_Adapter_Id_Prefix = "ChannelAdapter_";

// Glacier2 Configuration
const std::string C_Glacier2_Router_Key = "Ice.Default.Router";

// IceGrid Configuration
const std::string C_IceGrid_Locator_Key = "Ice.Default.Locator";
const std::string C_IceGrid_Locator_Fmt = "%s/Locator:tcp -p %s -h %s";

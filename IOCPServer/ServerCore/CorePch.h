#pragma once

#define WIN32_LEAN_AND_MEAN

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "tinyxml2.h"
//#include "SimpleMath.h" 알아서 사용할 것

#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
using namespace std;

#include <assert.h>
#include "Lock.h"
#include "SocketUtils.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "SessionManager.h"
#include "Session.h"

#include "ThreadManager.h"
#include "GlobalQueue.h"
#pragma comment(lib, "ws2_32.lib")
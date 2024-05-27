#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

std::unique_ptr<ThreadManager> GThreadManager = make_unique<ThreadManager>();

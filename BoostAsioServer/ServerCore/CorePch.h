#pragma once
#include "Types.h"
#include "CoreMacro.h"

#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/bind/bind.hpp>

using SessionFunction = std::function<SessionRef(boost::asio::io_context&)>;
using boost::asio::ip::tcp;

#include "LockQueue.h"
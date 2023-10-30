#include "pch.h"
#include "TestManager.h"

shared_ptr<TestManager> Gtest = make_shared<TestManager>();

void TestManager::Send(string prints)
{
	cout << prints << endl;
}

void TestManager::SendByNow(string str)
{
	cout << str << endl;
}



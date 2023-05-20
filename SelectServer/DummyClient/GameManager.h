#pragma once
#include "DummySession.h"

class GameManager
{
	SINGLETON(GameManager)

public:
	void Init(int users = 1);
	void Start();

	ClientServiceRef GetService() { return service; }

private:
	ClientServiceRef service;
};


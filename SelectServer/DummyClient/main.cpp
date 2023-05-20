#include "pch.h"
#include <Service.h>
#include "GameManager.h"
#include "PacketHandler.h"

int main()
{
	PacketHandler::Init();

	this_thread::sleep_for(1s);

	GAME->Init(10);
}

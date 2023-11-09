#include "pch.h"
#include "Accepter.h"
#include "Session.h"

Accepter::Accepter(boost::asio::io_context& context, tcp::endpoint endPoint, SessionFunction func)
	: _ioContext(context), _accepter(context, endPoint), _sessionFunc(func)
{
}

Accepter::~Accepter()
{
	if (_accepter.is_open())
	{
		_accepter.cancel();
		_accepter.close();

		_session = nullptr;
	}
}

void Accepter::StartAccept()
{
	AsyncAccept();
}

void Accepter::AsyncAccept()
{
	_session = _sessionFunc(_ioContext);
	_accepter.async_accept(_session->GetSocket(),
		[=](const boost::system::error_code ec)
		{
			OnAccept(ec);
		});
}

void Accepter::OnAccept(boost::system::error_code ec)
{
	if (!ec)
	{
		std::cout << "Accept" << std::endl;
		_session->Start();
	}

	StartAccept();
}

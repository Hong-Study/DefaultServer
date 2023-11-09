#pragma once

using SessionFunction = std::function<SessionRef(boost::asio::io_context&)>;

class Accepter
{
public:
	Accepter(boost::asio::io_context& context, tcp::endpoint endPoint, SessionFunction func);
	~Accepter();

	void StartAccept();

private:
	void AsyncAccept();
	void OnAccept(boost::system::error_code ec);

private:
	tcp::acceptor				_accepter;
	boost::asio::io_context&	_ioContext;
	SessionFunction				_sessionFunc;
	SessionRef					_session;
};
# C++ Boost::Asio Server
- 기본적인 골자는 [IOCP Server](https://github.com/Hong-Study/DefaultServer/tree/main/IOCPServer)에서 가져왔습니다.
- 따라서 클라이언트가 접속하면 [ClientSession](https://github.com/Hong-Study/DefaultServer/blob/main/BoostAsioServer/Server/ClientSession.h)으로 관리하게 됩니다.
- 아직은 서버 코어만 완성된 상태의 코드입니다.

### 설명
- [Accepter](https://github.com/Hong-Study/DefaultServer/blob/main/BoostAsioServer/ServerCore/Accepter.h)를 통해 클라이언트의 Connect를 받게 되며, 비동기로 구현되어있습니다.
- [JobQueue](https://github.com/Hong-Study/DefaultServer/blob/main/BoostAsioServer/ServerCore/JobQueue.h)의 경우, Boost::asio 의 strand를 통해 Lock을 사용하지 않고 Queue가 돌아가며 Job을 수행할 수 있도록 구현하였습니다.
- [Session](https://github.com/Hong-Study/DefaultServer/blob/main/BoostAsioServer/ServerCore/Session.h)은 비동기 Read, Write 함수를 사용하여 클라이언트와 송,수신을 하게 됩니다.
- Buffer의 경우 [SendBuffer](https://github.com/Hong-Study/DefaultServer/blob/main/BoostAsioServer/ServerCore/SendBuffer.h)와 [RecvBuffer](https://github.com/Hong-Study/DefaultServer/blob/main/BoostAsioServer/ServerCore/RecvBuffer.h)가 존재합니다.
- 

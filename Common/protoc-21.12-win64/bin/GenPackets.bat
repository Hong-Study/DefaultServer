pushd %~dp0

protoc.exe -I=./ --cpp_out=./ ./Protocol.proto
protoc.exe -I=./ --csharp_out=./ ./ProtocolC.proto
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --csharp_out=./ ./Enum.proto

XCOPY /Y Protocol.pb.h "../../../IOCPServer/Server"
XCOPY /Y Protocol.pb.cc "../../../IOCPServer/Server"
XCOPY /Y Enum.pb.h "../../../IOCPServer/Server"
XCOPY /Y Enum.pb.cc "../../../IOCPServer/Server"

XCOPY /Y ProtocolC.cs "../../../CsharpClient/GameServer/Packet"
XCOPY /Y Enum.cs "../../../CsharpClient/GameServer/Packet"

IF ERRORLEVEL 1 PAUSE

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.cs


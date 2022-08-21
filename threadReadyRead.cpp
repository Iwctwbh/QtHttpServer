#include "threadReadyRead.h"

threadReadyRead::threadReadyRead(QObject *parent) : QObject(parent)
{
}

void threadReadyRead::setSocket(QTcpSocket *socket)
{
	threadReadyRead::t_socket = socket;
}

void threadReadyRead::runLogicFunction()
{
	//QByteArray v_byteArr[2];
	//v_byteArr[0] = "HTTP/1.1200OK\r\nServer:nginx\r\nContent-Type:application/json;text/plain,charset=utf-8\r\nConnection:keep-alive\r\nAccess-Control-Allow-Origin:*\r\nAccess-Control-Allow-Headers:Access-Control-Request-Headers,Authorization,Content-Type,Depth,User-Agent,X-File-Size,X-Requested-With,X-Requested-By,If-Modified-Since,X-File-Name,X-File-Type,Cache-Control,Origin,Access-Token\r\nAccess-Control-Allow-Credentials:true\r\naccess-control-expose-headers:Authorization\r\naccess-control-expose-headers:*\r\nAccess-Control-Max-Age:60\r\nVary:Accept-Encoding,Origin\r\nKeep-Alive:timeout=2,max=100\r\nContent-Length:150\r\n";
	//v_byteArr[1] = "{\"data\":[{\"ID\":1,\"IsActive\":\"\u0001\",\"Key\":\"a\",\"Value\":\"aaa\"}]}";

	//// Êä³öÏìÓ¦
	//t_socket->write(v_byteArr[0]);
	//t_socket->write(v_byteArr[1]);
	//t_socket->flush();
	////socket->waitForBytesWritten((*http).size());
	//t_socket->waitForBytesWritten((v_byteArr[0]).size() + (v_byteArr[1]).size());

	//// ¹Ø±Õ¼àÌý
	//t_socket->abort();
	//t_socket->close();

	//emit sendMessage("Success");
}

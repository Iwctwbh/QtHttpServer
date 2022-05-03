#include "httpserver.h"

HttpServer &HttpServer::instance()
{
	static HttpServer obj;
	return obj;
}

void HttpServer::run(const QHostAddress &address, const quint16 &port)
{
	m_httpServer->listen(address, port);
}

void HttpServer::newConnection()
{
	if (m_httpServer->hasPendingConnections())
	{
		QTcpSocket *m_socket = m_httpServer->nextPendingConnection();
		// 多线程
		QObject::connect(m_socket, &QTcpSocket::readyRead, this, &HttpServer::readyRead);
	}
}

void HttpServer::readyRead()
{
	static int count = 0;
	QScopedPointer<QTcpSocket> socket(qobject_cast<QTcpSocket *>(sender()));
	if (socket) {
		++count;
		//qDebug() << "newConnection " << count;

		// 初始化指针
		static QByteArray *byteArr_Request = new QByteArray();
		static QByteArray *byteArr_ResponseHttp = new QByteArray();
		static QByteArray *byteArr_ResponseData = new QByteArray();

		*byteArr_Request = socket->readAll().trimmed();
		byteArr_ResponseHttp->clear();
		byteArr_ResponseData->clear();

		// 中间函数，亦处理主函数 为函数指针，需指向所需函数
		LogicControl(byteArr_Request, byteArr_ResponseHttp, byteArr_ResponseData);
		
		// 输出响应
		socket->write(*byteArr_ResponseHttp);
		socket->write(*byteArr_ResponseData);
		socket->flush();
		//socket->waitForBytesWritten((*http).size());
		socket->waitForBytesWritten((*byteArr_ResponseHttp).size() + (*byteArr_ResponseData).size());

		// 关闭监听
		socket->abort();
		socket->close();

		// 释放内存 置空指针
		/*delete byteArr_Request;
		byteArr_Request = nullptr;
		delete byteArr_ResponseHttp;
		byteArr_ResponseHttp = nullptr;
		delete byteArr_ResponseData;
		byteArr_ResponseData = nullptr;*/
	}
}

HttpServer::HttpServer(QObject *parent) : QObject(parent)
{
	m_httpServer = new QTcpServer(this);
	m_httpServer->setMaxPendingConnections(10);//设置最大允许连接数
	QObject::connect(m_httpServer, &QTcpServer::newConnection, this, &HttpServer::newConnection);
}

HttpServer::~HttpServer()
{
	m_httpServer->close();
	delete m_httpServer;
	m_httpServer = nullptr;
}
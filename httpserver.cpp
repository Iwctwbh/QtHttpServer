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
		// ���߳�
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

		// ��ʼ��ָ��
		static QByteArray *byteArr_Request = new QByteArray();
		static QByteArray *byteArr_ResponseHttp = new QByteArray();
		static QByteArray *byteArr_ResponseData = new QByteArray();

		*byteArr_Request = socket->readAll().trimmed();
		byteArr_ResponseHttp->clear();
		byteArr_ResponseData->clear();

		// �м亯�����ദ�������� Ϊ����ָ�룬��ָ�����躯��
		LogicControl(byteArr_Request, byteArr_ResponseHttp, byteArr_ResponseData);
		
		// �����Ӧ
		socket->write(*byteArr_ResponseHttp);
		socket->write(*byteArr_ResponseData);
		socket->flush();
		//socket->waitForBytesWritten((*http).size());
		socket->waitForBytesWritten((*byteArr_ResponseHttp).size() + (*byteArr_ResponseData).size());

		// �رռ���
		socket->abort();
		socket->close();

		// �ͷ��ڴ� �ÿ�ָ��
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
	m_httpServer->setMaxPendingConnections(10);//�����������������
	QObject::connect(m_httpServer, &QTcpServer::newConnection, this, &HttpServer::newConnection);
}

HttpServer::~HttpServer()
{
	m_httpServer->close();
	delete m_httpServer;
	m_httpServer = nullptr;
}
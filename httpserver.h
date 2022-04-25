#pragma once
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QtNetwork>
class HttpServer : public QObject
{
	Q_OBJECT

public:
	static HttpServer &instance();
	QMap<QString, QString(*)(QString*)> *map_Parameter = new QMap<QString, QString(*)(QString*)>();
	void (*LogicControl)(QByteArray*, QByteArray*, QByteArray*);
	void run(const QHostAddress &address = QHostAddress::Any, const quint16 &port = 80);

signals:

public slots:

private slots:
	void newConnection();
	void readyRead();

private:
	explicit HttpServer(QObject *parent = nullptr);
	~HttpServer();
	Q_DISABLE_COPY(HttpServer)

private:
	QTcpServer *m_httpServer;
	QTime timer;
};

#endif // HTTPSERVER_H
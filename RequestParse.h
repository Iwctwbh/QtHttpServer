#pragma once
#ifndef REQUESTPARSING_H
#define REQUESTPARSING_H

#include <QObject>
#include <QtCore/QRegularExpression>

class RequestParse
{
public:
	RequestParse();

	QByteArray* SetRequest(QByteArray byteArr_Request);
	QByteArray* GetMethod(void);
	QByteArray* GetParameter(void);
	QByteArray* GetPostData(void);
	QMap<QByteArray, QByteArray>* GetCookies(void);

	~RequestParse(void);

private:
	QByteArray *_byteArr_Request = new QByteArray();
	QByteArray *_byteArr_Method = new QByteArray();
	QByteArray *_byteArr_Parameter = new QByteArray();
	QByteArray *_byteArr_PostData = new QByteArray();
	QByteArray *_byteArr_Cookies = new QByteArray();
	QMap<QByteArray, QByteArray> *_map_Cookies = new QMap<QByteArray, QByteArray>();
};
#endif // REQUESTPARSING_H
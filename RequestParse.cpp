#include "RequestParse.h"

RequestParse::RequestParse()
{

}

QByteArray *RequestParse::GetMethod()
{
	QScopedPointer<QRegularExpression> reg_Method(new QRegularExpression("^.*? "));
	//QScopedPointer<QRegularExpressionMatch> reg_Method_match(new QRegularExpressionMatch(reg_Method->match(Request)));
	*_byteArr_Method = reg_Method->match(*_byteArr_Request).captured(0).trimmed().toLocal8Bit();
	return _byteArr_Method;
}

QByteArray *RequestParse::GetParameter()
{
	if (_byteArr_Method->isEmpty())
	{
		GetMethod();
	}
	QScopedPointer<QRegularExpression> reg_Parameter(new QRegularExpression("^(" + *_byteArr_Method + ") \/(.*?) HTTP"));
	QScopedPointer<QRegularExpressionMatch> reg_Parameter_match(new QRegularExpressionMatch(reg_Parameter->match(*_byteArr_Request)));
	QScopedPointer<QList<QByteArray>> list_Parameter_byteArr(new QList<QByteArray>(reg_Parameter_match->captured(0).toUtf8().split(' ')));
	*_byteArr_Parameter = list_Parameter_byteArr->value(1);
	return _byteArr_Parameter;
}

QMap<QByteArray, QByteArray> *RequestParse::GetCookies()
{
	QScopedPointer<QRegularExpression> reg_Cookie(new QRegularExpression("Cookie: .*\n"));
	QScopedPointer<QRegularExpressionMatch> reg_Cookie_match(new QRegularExpressionMatch(reg_Cookie->match(*_byteArr_Request)));
	_map_Cookies->clear();
	if (reg_Cookie_match->hasMatch())
	{
		QScopedPointer<QList<QByteArray>> list_Cookies_byteArr(new QList<QByteArray>(reg_Cookie_match->captured(0).toUtf8().mid(7).split(';')));
		Q_FOREACH(auto i, *list_Cookies_byteArr)
		{
			_map_Cookies->insert(i.split('=')[0].trimmed(), QByteArray::fromPercentEncoding(i.split('=')[1]).trimmed());
		}
	}
	return _map_Cookies;
}

QByteArray *RequestParse::GetPostData()
{
	QScopedPointer<QList<QByteArray>> list_Request_byteArr(new QList<QByteArray>(_byteArr_Request->split(('\n'))));
	*_byteArr_PostData = list_Request_byteArr->last().trimmed();
	//QString *str_PostData = new QString();
	//QScopedPointer<QByteArray> byteArr_PostData(new QByteArray());
	//if ((*list_Request_byteArr)[list_Request_byteArr->count() - 2].trimmed() == "") // ÅÐ¶ÏÊÇ·ñÓÐ POST data;
	//{
	//	*byteArr_PostData = list_Request_byteArr->last().trimmed();
	//}
	return _byteArr_PostData;
}

QByteArray *RequestParse::SetRequest(QByteArray byteArr_Request)
{
	*_byteArr_Request = byteArr_Request;
	return _byteArr_Request;
}

RequestParse::~RequestParse()
{
	delete _byteArr_Request;
	_byteArr_Request = nullptr;
	delete _byteArr_Method;
	_byteArr_Method = nullptr;
	delete _byteArr_Parameter;
	_byteArr_Parameter = nullptr;
	delete _byteArr_PostData;
	_byteArr_PostData = nullptr;
}
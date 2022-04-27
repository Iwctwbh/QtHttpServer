#include <QtCore/QCoreApplication>
#include <QtSql>

#include<algorithm>

#include "httpserver.h"
#include "mysql.h"
#include "RequestParse.h"
#include "SimpleServers.h"

void LogicControl(QByteArray *byteArr_Request, QByteArray *byteArr_ResponseHttp, QByteArray *byteArr_ResponseData);

//QMap<QByteArray, QByteArray(*)(QByteArray *)> *map_Function = new QMap<QByteArray, QByteArray(*)(QByteArray *)>();

typedef struct
{
	QList<QByteArray> *Parameter;
	QByteArray *SQLString;

} SimpleServer;

//QMap<SimpleServer *, QByteArray(*)(QByteArray *)> *map_Function = new QMap<SimpleServer *, QByteArray(*)(QByteArray *)>();

Mysql *mysql = new Mysql("127.0.0.1", 3306, "Test", "Test", "AJK8d9sfjc90sd8f9cxSJD@09^8d#N*&DFDJDNDaJJJdsf090905", "dbcon1");

SimpleServers *sserv = new SimpleServers();

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QUuid id = QUuid::createUuid();

	QFile file{ "data.json" };
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Loading file error";
	}

	QByteArray filestream = file.readAll();

	QJsonDocument qjd = QJsonDocument::fromJson(filestream);
	bool flag = qjd.isObject();

	QJsonObject *QJsonObject_Temp = new QJsonObject(QJsonDocument::fromJson(filestream).object());
	QJsonObject *QJsonObject_MySQL_Temp = new QJsonObject(QJsonObject_Temp->take("MySQL").toObject());
	delete mysql;
	mysql = new Mysql(
		QJsonObject_MySQL_Temp->take("Host").toString(),
		QJsonObject_MySQL_Temp->take("Port").toString().toInt(),
		QJsonObject_MySQL_Temp->take("DataBase").toString(),
		QJsonObject_MySQL_Temp->take("UserName").toString(),
		QJsonObject_MySQL_Temp->take("Password").toString(),
		"dbcon1");

	QJsonArray *arr = new QJsonArray(QJsonObject_Temp->take("SimpleServers").toArray());
	qDebug() << *arr;
	sserv->InitSimpleServersFromJson(arr);

	quint16 HttpServerPort = QJsonObject_Temp->take("HttpServerPort").toString().toInt();

	// HttpServer启动
	HttpServer::instance().LogicControl = LogicControl; // 配置处理主函数
	HttpServer::instance().run(QHostAddress::Any, HttpServerPort); // 设置端口

	// 连接数据库
	mysql->connect();

	return a.exec();
}

QJsonObject ConvertStringToJson(QString *str_Json)
{
	QJsonObject json_Json;
	QScopedPointer<QList<QString>> list_Request_str(new QList<QString>(str_Json->split('&')));
	foreach(QString i, *list_Request_str)
	{
		json_Json.insert(QString(QUrl::fromPercentEncoding(i.split('=').first().toLocal8Bit())), QString(QUrl::fromPercentEncoding(i.split('=').last().toLocal8Bit())));
	}
	return json_Json;
}

void ConvertStringToJson_ptr(QString *str_Json, QJsonObject *json_Json)
{
	QScopedPointer<QList<QString>> list_Request_str(new QList<QString>(str_Json->split('&')));
	foreach(QString i, *list_Request_str)
	{
		json_Json->insert(QString(QUrl::fromPercentEncoding(i.split('=').first().toLocal8Bit())), QString(QUrl::fromPercentEncoding(i.split('=').last().toLocal8Bit())));
	}
}

QByteArray TestFunction(QByteArray *PostData, SimpleServers::SimpleServer *SServer)
{
	QJsonObject *JsonObject = new QJsonObject(QJsonDocument::fromJson(*PostData).object());
	QString *String_SQLParameters = new QString();
	for (auto s : *SServer->Parameters)
	{
		if (!String_SQLParameters->isEmpty())
		{
			*String_SQLParameters += ", ";
		}
		else
		{
			*String_SQLParameters += " (";
		}
		*String_SQLParameters += '\'' + JsonObject->take(s).toString() + '\'';
	}
	*String_SQLParameters += ')';
	QString *String_SQL = new QString(*SServer->SQLString + *String_SQLParameters);

	QSqlQuery *query_Sql = mysql->QueryExec(*String_SQL);
	QJsonObject *json_Test = new QJsonObject();
	QString *str_Json = new QString();

	*json_Test = {};

	QJsonArray JsonArray_Temp;

	query_Sql = mysql->QueryExec(*String_SQL);

	while (query_Sql->next())
	{
		for (int x{ 0 }; x < query_Sql->record().count(); ++x)
		{
			JsonArray_Temp.push_back(QJsonObject{ {query_Sql->record().fieldName(x), QJsonValue::fromVariant(query_Sql->value(x))} });
		}
	}

	return QJsonDocument{ JsonArray_Temp }.toJson();
}

// 处理主函数
void LogicControl(QByteArray *byteArr_Request, QByteArray *byteArr_ResponseHttp, QByteArray *byteArr_ResponseData)
{
	static RequestParse *requestParsing = new RequestParse();

	requestParsing->SetRequest(*byteArr_Request);

	//auto iter = map_Function->find(*requestParsing->GetParameter());
	QByteArray Parameter = *requestParsing->GetParameter();
	bool flag = false;
	SimpleServers::SimpleServer *simpleserver = new SimpleServers::SimpleServer();
	/*foreach(SimpleServer * s, map_Function)
	{
		if (Parameter.compare(*s->Control))
		{
			flag = true;
			simpleserver = s;
			break;
		}
	}*/

	for (auto iter_Temp = sserv->Map_SimpleServers->begin(); iter_Temp != sserv->Map_SimpleServers->end(); ++iter_Temp)
	{
		if (!Parameter.compare(*iter_Temp.key()))
		{
			flag = true;
			simpleserver = iter_Temp.value();
			break;
		}
	}

	if (flag)
	{
		*byteArr_ResponseData = TestFunction(requestParsing->GetPostData(), simpleserver);
		auto map_Cookies = requestParsing->GetCookies();
		for (auto iter_Temp = map_Cookies->begin(); iter_Temp != map_Cookies->end(); ++iter_Temp)
		{
			qDebug() << iter_Temp.key() << ": " << iter_Temp.value() << "\r\n";
		}
		*byteArr_ResponseHttp += "HTTP/1.1 200 OK\r\n";
	}
	else
	{
		*byteArr_ResponseHttp += "HTTP/1.1 404 NotFound\r\n";
	}

	*byteArr_ResponseHttp += "Server: nginx\r\n";
	//*byteArr_ResponseHttp += "Content-Type: text/html;charset=utf-8\r\n";
	*byteArr_ResponseHttp += "Content-Type: application/json; text/plain, charset=utf-8\r\n";
	*byteArr_ResponseHttp += "Connection: keep-alive\r\n";
	*byteArr_ResponseHttp += "Access-Control-Allow-Origin: http://10.11.12.6:8080\r\n";
	*byteArr_ResponseHttp += "Access-Control-Allow-Methods: POST, OPTIONS\r\n";
	*byteArr_ResponseHttp += "Access-Control-Allow-Header: Authorization, Content-Type, Depth, User-Agent, X-File-Size, X-Requested-With, X-Requested-By, If-Modified-Since, X-File-Name, X-File-Type, Cache-Control, Origin\r\n";
	*byteArr_ResponseHttp += "Access-Control-Allow-Credentials: true\r\n";
	*byteArr_ResponseHttp += "access-control-expose-headers: Authorization\r\n";
	*byteArr_ResponseHttp += "Access-Control-Max-Age: 60\r\n";
	*byteArr_ResponseHttp += "Vary: Accept-Encoding, Origin\r\n";
	*byteArr_ResponseHttp += "Keep-Alive: timeout=2, max=100\r\n";
	*byteArr_ResponseHttp += QString("Content-Length: %1\r\n\r\n").arg(QString::number((*byteArr_ResponseData).size())).toLocal8Bit();

	qDebug() << "\r\n" << "-------------------end------------------" << "\r\n";
}
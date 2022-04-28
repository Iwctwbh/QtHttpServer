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

Mysql *mysql;

SimpleServers *sserv = new SimpleServers();

int main(int argc, char *argv[])
{
	auto *a = new QCoreApplication{argc, argv};

	//QUuid id = QUuid::createUuid();

	QScopedPointer<QFile> t_pFile{new QFile{ "data.json" }};

	if (!t_pFile->open(QIODevice::ReadOnly))
	{
		qDebug() << "Loading file error";
	}

	QFileInfo t_pFileInfo{"data.json"};

	if (t_pFileInfo.exists())
	{
		qDebug() << "data.json exists";
		if (t_pFileInfo.isReadable())
		{
			qDebug() << "data.json is readable";
			QScopedPointer<QFile> t_pFile{new QFile{"data.json"}};
			if (t_pFile->open(QIODevice::ReadOnly))
			{
				qDebug() << "data.json open success";
				QByteArray filestream = t_pFile->readAll();
				QScopedPointer<QJsonDocument> t_pJsDoc{new QJsonDocument{ QJsonDocument::fromJson(filestream) }};
				if (t_pJsDoc->isObject())
				{
					qDebug() << "data.json format is correct";
					QScopedPointer<QJsonObject> t_pJsObj{new QJsonObject{t_pJsDoc->object()}};
					QScopedPointer<QJsonObject> t_pJsMySQL{new QJsonObject(t_pJsObj->value("MySQL").toObject())};
					mysql = new Mysql(
						t_pJsMySQL->value("Host").toString(),
						t_pJsMySQL->value("Port").toString().toInt(),
						t_pJsMySQL->value("DataBase").toString(),
						t_pJsMySQL->value("UserName").toString(),
						t_pJsMySQL->value("Password").toString(),
						"dbcon1");

					QScopedPointer<QJsonDocument> t_pJsDocSimpleServers{new QJsonDocument{t_pJsObj->value("SimpleServers").toArray()}};

					if (t_pJsDocSimpleServers->array().count() > 0)
					{
						qDebug() << "SimpleServers JsonAraay is correct";
						QSharedPointer<QJsonArray> t_pJsArrSimpleServers{new QJsonArray{t_pJsDocSimpleServers->array()}};
						sserv->InitSimpleServersFromJson(t_pJsArrSimpleServers);
						quint16 HttpServerPort = t_pJsObj->value("HttpServerPort").toString().toInt();

						// HttpServer启动
						HttpServer::instance().LogicControl = LogicControl; // 配置处理主函数
						HttpServer::instance().run(QHostAddress::Any, HttpServerPort); // 设置端口

						// 连接数据库
						mysql->connect();

						t_pJsArrSimpleServers.clear();

						return a->exec();
					}
					else
					{
						qDebug() << "SimpleServers JsonAraay is not correct";
						system("pause");
					}
				}
				else
				{
					qDebug() << "data.json format is not correct";
					system("pause");

				}
			}
			else
			{
				qDebug() << "data.json is not readable";
				system("pause");

			}

		}
		else
		{
			qDebug() << "data.json is not readable";
			system("pause");

		}
	}
	else
	{
		qDebug() << "data.json 不存在";
		system("pause");

	}
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
	QScopedPointer<QJsonObject> t_pJsonObject{new QJsonObject{QJsonDocument::fromJson(*PostData).object()}};
	QScopedPointer<QString> t_pstrSQLParameters{new QString{""}};
	for (auto s : *SServer->Parameters)
	{
		if (!t_pstrSQLParameters->isEmpty())
		{
			*t_pstrSQLParameters += ", ";
		}
		else
		{
			*t_pstrSQLParameters += " (";
		}
		*t_pstrSQLParameters += '\'' + t_pJsonObject->value(s).toString() + '\'';
	}
	*t_pstrSQLParameters += ')';
	QString *String_SQL = new QString(*SServer->SQLString + *t_pstrSQLParameters);

	QSqlQuery *query_Sql = mysql->QueryExec(*String_SQL);
	QJsonObject *json_Test = new QJsonObject();
	QString *str_Json = new QString();

	*json_Test = {};

	QJsonArray JsonArray_Temp;

	query_Sql = mysql->QueryExec(*String_SQL);

	while (query_Sql->next())
	{
		QJsonObject jsobjTemp = QJsonObject();
		for (int x{0}; x < query_Sql->record().count(); ++x)
		{
			jsobjTemp.insert(query_Sql->record().fieldName(x), QJsonValue::fromVariant(query_Sql->value(x)));
		}
		JsonArray_Temp.push_back(jsobjTemp);
	}
	return QJsonDocument{JsonArray_Temp}.toJson();
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
//#pragma execution_character_set("utf-8")

#include <QtCore/QCoreApplication>
#include <QtSql>
#include <crow.h>

#include "mysql.h"
#include "SimpleServers.h"

Mysql* mysql;

int main(int argc, char* argv[])
{
	[[maybe_unused]] const QCoreApplication* a = new QCoreApplication{ argc, argv };

	SimpleServers simple_servers{};
	simple_servers.Run();

	return QCoreApplication::exec();
}

QJsonObject ConvertStringToJson(QString* str_Json)
{
	QJsonObject json_Json;
	QList<QString> list_Request_str(str_Json->split('&'));
	foreach(QString i, list_Request_str)
	{
		json_Json.insert(QString(QUrl::fromPercentEncoding(i.split('=').first().toLocal8Bit())), QString(QUrl::fromPercentEncoding(i.split('=').last().toLocal8Bit())));
	}
	return json_Json;
}

QByteArray TestFunction(QByteArray* PostData, SimpleServers::SimpleServer* SServer)
{
	QJsonParseError t_pJsParseError{ QJsonParseError::NoError };
	QScopedPointer<QJsonDocument> t_pJsDoc{ new QJsonDocument{QJsonDocument::fromJson(*PostData, &t_pJsParseError)} };
	if (t_pJsParseError.error == QJsonParseError::NoError && t_pJsDoc->isObject())
	{
		QScopedPointer<QJsonObject> t_pJsonObject{ new QJsonObject{t_pJsDoc->object()} };
		QScopedPointer<QString> t_pstrSQLParameters{ new QString{""} };
		for (auto s : SServer->list_parameters)
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
		QScopedPointer<QString> String_SQL{ new QString(*SServer->bytearray_sql + *t_pstrSQLParameters) };

		QScopedPointer<QSqlQuery> query_Sql{ new QSqlQuery{*mysql->QueryExec(*String_SQL)} };
		QScopedPointer<QJsonObject> json_Test{ new QJsonObject() };
		QScopedPointer<QString> str_Json{ new QString() };

		*json_Test = {};

		QJsonArray JsonArray_Temp;

		//query_Sql = mysql->QueryExec(*String_SQL);

		while (query_Sql->next())
		{
			QJsonObject jsobjTemp = QJsonObject();
			for (int x{ 0 }; x < query_Sql->record().count(); ++x)
			{
				jsobjTemp.insert(query_Sql->record().fieldName(x), QJsonValue::fromVariant(query_Sql->value(x)));
			}
			JsonArray_Temp.push_back(jsobjTemp);
		}
		QScopedPointer<QJsonObject> t_pJsObj{ new QJsonObject };
		t_pJsObj->insert("data", JsonArray_Temp);
		return QJsonDocument{ *t_pJsObj }.toJson();
	}
	else
	{
		return QJsonDocument::fromJson("{\"Error\":\"Error\"}").toJson();
		qDebug() << "PostData format is not correct";
	}
}

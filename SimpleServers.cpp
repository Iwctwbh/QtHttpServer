#include "SimpleServers.h"

void SimpleServers::InitSimpleServers()
{
	this->Map_SimpleServers->clear();
}

void SimpleServers::InsertSimpleServer(QByteArray *arg_QByteArray_Control_Temp, QList<QByteArray> *arg_QList_Parameters, QByteArray *arg_QByteArray_SQLString_Temp)
{
	//QByteArray QByteArray_MD5_Temp = QCryptographicHash::hash(*QByteArray_Control_Temp, QCryptographicHash::Md5).toHex();
	SimpleServers::SimpleServer *SimpleServer_Temp = new SimpleServers::SimpleServer
	{
		arg_QList_Parameters,
		arg_QByteArray_SQLString_Temp
	};
	if (this->Map_SimpleServers->find(arg_QByteArray_Control_Temp) == this->Map_SimpleServers->end())
	{
		this->Map_SimpleServers->insert(arg_QByteArray_Control_Temp, SimpleServer_Temp);
	}
}

void SimpleServers::EraseSimpleServer()
{

}

void SimpleServers::InitSimpleServersFromJson(QJsonArray *arg_QjsonArray)
{
	this->InitSimpleServers();
	SimpleServers *ths = this;

	std::for_each(arg_QjsonArray->begin(), arg_QjsonArray->end(), [ths](QJsonValue obj)
		{
			QByteArray *QByteArray_Control_Temp = new QByteArray{ obj.toObject().take("Control").toString().toLocal8Bit() };

			QList<QByteArray> *QList_QString_Parameters = new QList<QByteArray>();

			Q_FOREACH(auto o, obj.toObject().take("Parameters").toString().split(','))
			{
				QList_QString_Parameters->emplace_back(o.trimmed().toLocal8Bit());
			}

			QByteArray *QByteArray_SQLString_Temp = new QByteArray{ obj.toObject().take("SQLString").toString().toLocal8Bit() };

			ths->InsertSimpleServer(QByteArray_Control_Temp, QList_QString_Parameters, QByteArray_SQLString_Temp);
		});
}
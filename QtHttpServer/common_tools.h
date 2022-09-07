#pragma once
#ifndef COMMON_TOOLS_H
#define COMMON_TOOLS_H

#include <QtCore/QtCore>

class QtCommonTools
{
public:
	static QByteArray ConvertImgToBase64(const QByteArray &string_path);
};

#endif // SIMPLE_SERVERS_H

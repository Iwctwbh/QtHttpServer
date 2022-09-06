#pragma once

#include "qt_common_tools_global.h"

#include <QtCore/QtCore>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>

class QTCOMMONTOOLS_EXPORT QtCommonTools
{
public:
	QtCommonTools();
	static QByteArray ConvertImgToBase64(const QString& string_path);
};
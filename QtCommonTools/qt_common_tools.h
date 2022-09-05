#pragma once

#include "qt_common_tools_global.h"

#include <QtCore/QtCore>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//QT_BEGIN_NAMESPACE
//class QConvert;
//QT_END_NAMESPACE


class QTCOMMONTOOLS_EXPORT QtCommonTools
{
public:
	QtCommonTools();
	QString ConvertImgToBase64(std::string string_path);
};
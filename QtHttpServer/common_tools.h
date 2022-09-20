#pragma once
#ifndef COMMON_TOOLS_H
#define COMMON_TOOLS_H

#include <QtCore/QtCore>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

class QtCommonTools
{
public:
	static QByteArray ConvertImgToBase64(const QByteArray &string_path);
	[[nodiscard]] QString GetMiCode() const;

private:
	static void MyMerge(cv::Mat src, cv::Mat &dst);
};

#endif // SIMPLE_SERVERS_H

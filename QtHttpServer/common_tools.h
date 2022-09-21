#pragma once
#ifndef COMMON_TOOLS_H
#define COMMON_TOOLS_H

#include <QtCore/QtCore>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

class CommonTools
{
public:
	static QByteArray ConvertImgToBase64(const QByteArray &string_path);
	static cv::Mat GetCaptchaImage();
	static QByteArray ConvertMatToBase64(const cv::Mat &arg_mat_image);

private:
	static void MergeImage(cv::Mat &src, cv::Mat &dst);
};

#endif // SIMPLE_SERVERS_H

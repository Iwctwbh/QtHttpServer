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
	static QByteArray ConvertImgToBase64(const QByteArray& string_path);
	static cv::Mat CreateCaptchaImage(std::string* out_string_result = nullptr);
	static QByteArray ConvertMatToBase64(const cv::Mat& arg_mat_image);

	/**
	* @param arg_json_object
	* 被插入的Json Object
	* @param arg_string_key
	* 被插入的key
	* @param arg_json_value
	* 待插入的value
	*/
	static void InsertJsonValueToJsonObject(QJsonObject& arg_json_object, const QString& arg_string_key, const QJsonValue& arg_json_value);

private:
	static void MergeImage(cv::Mat& src, cv::Mat& dst);
};

#endif  // SIMPLE_SERVERS_H

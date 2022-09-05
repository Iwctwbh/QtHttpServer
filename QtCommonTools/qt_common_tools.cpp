#include "qt_common_tools.h"

QtCommonTools::QtCommonTools()
{
}

QString QtCommonTools::ConvertImgToBase64(std::string string_path)
{
	//cv::Mat image = cv::imread(string_path, 1);
	std::vector<uint8_t> buffer;
	//cv::imencode(".png", image, buffer);
	//QByteArray byteArray = QByteArray::fromRawData((const char*)buffer.data(), buffer.size());
	////std::string a(buffer.begin(), buffer.end());
	////QString base64Image(byteArray.toBase64());
	return "string_path";
}

#include "qt_common_tools.h"

QtCommonTools::QtCommonTools()
{
}

QByteArray QtCommonTools::ConvertImgToBase64(const QString& string_path)
{
	QByteArray bytearray_image_base64{};
	QFile file(string_path);  //strFilePath 路径
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray bytearray_file = file.readAll();
		//const std::vector<char> vector_file_data(bytearray_file.data(), bytearray_file.data() + bytearray_file.size());
		//std::vector<uint8_t> buffer;
		//cv::Mat mat_image = cv::imdecode(cv::Mat(vector_file_data), 1);  //>0 Return a 3-channel color image
		bytearray_image_base64 = bytearray_file.toBase64();
		file.close();
	}

	return bytearray_image_base64;
}

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
		const QByteArray bytearray_file = file.readAll();
		bytearray_image_base64 = bytearray_file.toBase64();
		file.close();
	}

	return bytearray_image_base64;
}

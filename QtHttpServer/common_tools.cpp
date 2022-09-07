#include "common_tools.h"

QByteArray QtCommonTools::ConvertImgToBase64(const QByteArray& string_path)
{
	QByteArray bytearray_image_base64{};
	if (QFile file(string_path); file.open(QIODevice::ReadOnly))
	{
		const QByteArray bytearray_file = file.readAll();
		bytearray_image_base64 = bytearray_file.toBase64();
		file.close();
	}

	return bytearray_image_base64;
}

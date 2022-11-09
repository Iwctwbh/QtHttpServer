#include "common_tools.h"

void HanZiCaptchaImage(const QByteArray& bytearray_base64)
{
	cv::Mat cv_mat{};
}

QByteArray CommonTools::ConvertImgToBase64(const QByteArray& string_path)
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

//获取4位随机生成的图片验证码
//数字和字母组合，不重复。
//@return
void CommonTools::MergeImage(cv::Mat& src, cv::Mat& dst)
{
	for (int i = 0; i < src.rows; i++)
	{
		//遍历像素点
		for (int j = 0; j < src.cols; j++)
		{
			if (src.at<cv::Vec3b>(i, j)[0] < 255 && src.at<cv::Vec3b>(i, j)[0] > 20)
			{
				dst.at<cv::Vec3b>(i, j)[0] = src.at<cv::Vec3b>(i, j)[0];
				dst.at<cv::Vec3b>(i, j)[1] = src.at<cv::Vec3b>(i, j)[1];
				dst.at<cv::Vec3b>(i, j)[2] = src.at<cv::Vec3b>(i, j)[2];
			}
		}
	}
}

cv::Mat CommonTools::CreateCaptchaImage(std::string* out_string_result)
{
	uint16_t uint16_image_width = 300;
	uint16_t uint16_image_height = 70;
	std::default_random_engine random_engine_seed(static_cast<unsigned int>(time(nullptr)));
	std::uniform_int_distribution<> distribution_int(0, 32767);
	cv::Mat mat_image(uint16_image_height, uint16_image_width, CV_8UC3, cv::Scalar(255, 255, 255));

	std::string string_result{};
	//增加随机字符
	std::string string_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	std::string string_letter_code;
	string_letter_code.resize(6);
	for (char& i : string_letter_code)
	{
		i = string_chars[distribution_int(random_engine_seed) % string_chars.length()];
	}
	uint16_t uint16_pos_x{0};
	uint16_t uint16_pos_y{25};
	for (char char_char : string_letter_code)
	{
		cv::Mat mat_image_copy(uint16_image_height, uint16_image_width, CV_8UC3, cv::Scalar(255, 255, 255));
		std::string string_char{char_char};
		string_result += string_char;
		cv::Point point_origin;
		point_origin.x = uint16_pos_x + distribution_int(random_engine_seed) % 50;
		while (point_origin.x - uint16_pos_x < 15)
		{
			point_origin.x = uint16_pos_x + distribution_int(random_engine_seed) % 50;
		}
		point_origin.y = uint16_pos_y + distribution_int(random_engine_seed) % 35;
		uint16_t uint16_font_face = cv::FONT_HERSHEY_COMPLEX;
		double double_font_scale =
			distribution_int(random_engine_seed) % 2000 / 3000.0 + 1.0;
		uint16_t uint16_thickness = 1;
		int32_t int32_baseline;
		if (cv::Size size_text = cv::getTextSize(string_char, uint16_font_face, double_font_scale, uint16_thickness, &int32_baseline);
			point_origin.y < size_text.height)
		{
			point_origin.y = uint16_pos_y + distribution_int(random_engine_seed) % 35; //以免超出
		}
		putText(mat_image_copy, string_char, point_origin, uint16_font_face, double_font_scale,
		        cv::Scalar(distribution_int(random_engine_seed) % 255,
		                   distribution_int(random_engine_seed) % 255,
		                   distribution_int(random_engine_seed) % 255),
		        uint16_thickness, 8, false);
		uint16_pos_x += 45;
		cv::Point2f point2f_center(mat_image_copy.cols / 2, mat_image_copy.rows / 2); //定义旋转中心为图像的中心
		cv::Mat mat_rot_mat = getRotationMatrix2D(point2f_center, distribution_int(random_engine_seed) % 20 - 10, 1);
		warpAffine(mat_image_copy, mat_image_copy, mat_rot_mat, mat_image_copy.size());
		MergeImage(mat_image_copy, mat_image);
	}
	//增加随机字符干扰
	int char_num = 10;
	for (int i = 0; i < char_num; i++)
	{
		std::string temp_string = " ";
		temp_string[0] = string_chars[distribution_int(random_engine_seed) % string_chars.length()];
		cv::Point origin;
		origin.x = 50 + distribution_int(random_engine_seed) % 300;
		origin.y = 10 + distribution_int(random_engine_seed) % 70;
		putText(mat_image, temp_string, origin, cv::FONT_HERSHEY_COMPLEX, 0.4,
		        cv::Scalar(distribution_int(random_engine_seed) % 255,
		                   distribution_int(random_engine_seed) % 255,
		                   distribution_int(random_engine_seed) % 255),
		        1, 8, false);
	}
	if (out_string_result)
	{
		*out_string_result = string_result;
	}
	return mat_image;
}

QByteArray CommonTools::ConvertMatToBase64(const cv::Mat& arg_mat_image)
{
	std::vector<uint8_t> buffer;
	imencode(".png", arg_mat_image, buffer);
	const QByteArray bytearray_mat_stream = QByteArray::fromRawData(reinterpret_cast<const char*>(buffer.data()), buffer.size());
	return bytearray_mat_stream.toBase64();
}

void CommonTools::InsertJsonValueToJsonObject(QJsonObject& arg_json_object, const QString& arg_string_key, const QJsonValue& arg_json_value)
{
	if (arg_json_value.isObject())
	{
		arg_json_object.insert(arg_string_key, arg_json_value.toObject());
	}
	else if (arg_json_value.isArray())
	{
		arg_json_object.insert(arg_string_key, arg_json_value.toArray());
	}
	else
	{
		arg_json_object.insert(arg_string_key, arg_json_value);
	}
}

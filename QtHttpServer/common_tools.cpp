#include "common_tools.h"

QByteArray QtCommonTools::ConvertImgToBase64(const QByteArray &string_path)
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
void QtCommonTools::MyMerge(cv::Mat src, cv::Mat &dst)
{
	for (int i = 0; i < src.rows; i++) { //遍历像素点
		for (int j = 0; j < src.cols; j++)
		{
			if (src.at<cv::Vec3b>(i, j)[0] < 255 && src.at<cv::Vec3b>(i, j)[0] >  20) {
				dst.at<cv::Vec3b>(i, j)[0] = src.at<cv::Vec3b>(i, j)[0];
				dst.at<cv::Vec3b>(i, j)[1] = src.at<cv::Vec3b>(i, j)[1];
				dst.at<cv::Vec3b>(i, j)[2] = src.at<cv::Vec3b>(i, j)[2];
			}
		}
	}
}

QString QtCommonTools::GetMiCode() const
{
	int imageWidth = 300;
	int imageHeight = 70;
	srand(static_cast<unsigned>(time(nullptr)));
	double bgColor = rand() % 100 + 100;
	cv::Mat image(imageHeight, imageWidth, CV_8UC3, cv::Scalar(255, 255, 255));
	imshow("image1", image);

	std::string fileName = "";
	//增加随机字符
	std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char c = chars[rand() % 36];
	std::string letterCode;
	letterCode.resize(6);
	for (int i = 0; i < letterCode.size(); i++)
	{
		letterCode[i] = chars[rand() % 36];
	}
	//std::cout << letterCode << std::endl;
	int pos_x = 0;
	int pos_y = 25;
	for (int i = 0; i < 6; i++)
	{
		cv::Mat imageCopy(imageHeight, imageWidth, CV_8UC3, cv::Scalar(255, 255, 255));
		std::string tempChar = letterCode.substr(i, 1);
		fileName += tempChar;
		cv::Point origin;
		origin.x = pos_x + rand() % 50;
		while (origin.x - pos_x < 15)
			origin.x = pos_x + rand() % 50;
		origin.y = pos_y + rand() % 35;
		int font_face = cv::FONT_HERSHEY_COMPLEX;
		double font_scale = rand() % 2000 / 3000.0 + 1.0;
		int thickness = 1;
		int baseline;
		cv::Size text_size = cv::getTextSize(tempChar, font_face, font_scale, thickness, &baseline);
		if (origin.y < text_size.height)
			origin.y = pos_y + rand() % 35; //以免超出
		putText(imageCopy, tempChar, origin, font_face, font_scale, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), thickness, 8, 0);
		pos_x += 45;
		cv::Point2f  center(imageCopy.cols / 2, imageCopy.rows / 2);  //定义旋转中心为图像的中心
		cv::Mat rotMat = getRotationMatrix2D(center, rand() % 20 - 10, 1);
		warpAffine(imageCopy, imageCopy, rotMat, imageCopy.size());
		MyMerge(imageCopy, image);
	}
	imshow("image2", image);
	//增加随机字符干扰
	int char_num = 10;
	for (int i = 0; i < char_num; i++)
	{
		std::string s1 = " ";
		s1[0] = chars[rand() % 36];
		cv::Point origin;
		origin.x = 50 + rand() % 300;
		origin.y = 10 + rand() % 70;
		putText(image, s1, origin, cv::FONT_HERSHEY_COMPLEX, 0.4, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 1, 8, 0);
	}
	imshow("image3", image);
	//imwrite("result1/" + fileName + ".jpg", image);
	cv::waitKey(33);

	std::vector<uint8_t> buffer;
	cv::imencode(".png", image, buffer);
	QByteArray byteArray = QByteArray::fromRawData(reinterpret_cast<const char*>(buffer.data()), buffer.size());
	QString base64Image(byteArray.toBase64());

	return base64Image;
}

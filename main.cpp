#include "opencv2/opencv.hpp"

int main()
{
	cv::Mat src_img = cv::imread("test.jpg");

	std::vector<cv::Point2f> src_points;
	src_points.push_back(cv::Point(0, 0));
	src_points.push_back(cv::Point(1, 0));
	src_points.push_back(cv::Point(1, 1));
	src_points.push_back(cv::Point(0, 1));

	std::vector<cv::Point2f> dst_points;
	dst_points.push_back(cv::Point(0, 0));
	dst_points.push_back(cv::Point(1, 0));
	dst_points.push_back(cv::Point(2, 1));
	dst_points.push_back(cv::Point(0, 1));

	std::vector<cv::DMatch> matches;
	for (int i = 0; i < src_points.size(); ++i)
		matches.push_back(cv::DMatch(i, i, 0));

	cv::Ptr<cv::ThinPlateSplineShapeTransformer> tps 
		= cv::createThinPlateSplineShapeTransformer(0);
	tps->estimateTransformation(src_points, dst_points, matches);

	cv::Mat dst_img;
	tps->warpImage(src_img, dst_img);

	cv::pyrDown(src_img, src_img);
	cv::pyrDown(dst_img, dst_img);
	cv::imshow("source", src_img);
	cv::imshow("result", dst_img);
	cv::waitKey(0);
	return 0;
}
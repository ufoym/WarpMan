#include "opencv2/opencv.hpp"

void draw_bezigon(
	cv::Mat img,
	const std::vector<cv::Point2f> ctrl_pts,
	const cv::Scalar curve_color,
	const int thickness = 1,
	const float ctrl_alpha = 0.7f,
	const cv::Scalar ctrl_color = cv::Scalar(0, 255, 255),
	const int n_samples = 100)
{
	const int n_ctrl_pts = ctrl_pts.size();
	float prev_x = 0, prev_y = 0;
	for (int i = 0; i < n_ctrl_pts; i += 3) {
		for (float t = 0; t < 1; t += 1.0f / n_samples) {
			float t2 = t * t;
			float t3 = t2 * t;
			float nt = 1 - t;
			float nt2 = nt * nt;
			float nt3 = nt2 * nt;
			float x = ctrl_pts[i + 0].x * nt3
				+ ctrl_pts[i + 1].x * 3 * nt2 * t
				+ ctrl_pts[i + 2].x * 3 * nt * t2
				+ ctrl_pts[(i + 3) % n_ctrl_pts].x * t3;
			float y = ctrl_pts[i + 0].y * nt3
				+ ctrl_pts[i + 1].y * 3 * nt2 * t
				+ ctrl_pts[i + 2].y * 3 * nt * t2
				+ ctrl_pts[(i + 3) % n_ctrl_pts].y * t3;
			if (t > 0) {
				cv::line(img, cv::Point(x, y), cv::Point(prev_x, prev_y),
					curve_color, thickness, cv::LINE_AA);
			}
			prev_x = x;
			prev_y = y;
		}
	}
	if (ctrl_alpha > 0) {
		cv::Mat img_copy = img.clone();
		for (int i = 0; i < n_ctrl_pts; i += 3) {
			cv::line(img, ctrl_pts[i + 0], ctrl_pts[i + 1], ctrl_color, 1, cv::LINE_AA);
			cv::line(img, ctrl_pts[i + 2], ctrl_pts[(i + 3) % n_ctrl_pts], ctrl_color, 1, cv::LINE_AA);
		}
		for each (auto pt in ctrl_pts)
			cv::circle(img, pt, 3, ctrl_color, -1, cv::LINE_AA);
		img = img * ctrl_alpha + img_copy * (1 - ctrl_alpha);
	}
}


int main()
{
	cv::Mat src_img = cv::imread("test.jpg"); 
	
	std::vector<cv::Point2f> ctrl_pts;
	ctrl_pts.push_back(cv::Point2f(100, 100));
	ctrl_pts.push_back(cv::Point2f(200, 100));
	ctrl_pts.push_back(cv::Point2f(200, 200));
	ctrl_pts.push_back(cv::Point2f(100, 200));
	ctrl_pts.push_back(cv::Point2f(0, 200));
	ctrl_pts.push_back(cv::Point2f(0, 0));

	draw_bezigon(src_img, ctrl_pts, cv::Scalar(255, 0, 255), 2);

	std::vector<cv::Point2f> src_pts;
	src_pts.push_back(cv::Point2f(0, 0));
	src_pts.push_back(cv::Point2f(1, 0));
	src_pts.push_back(cv::Point2f(1, 1));
	src_pts.push_back(cv::Point2f(0, 1));

	std::vector<cv::Point2f> dst_pts;
	dst_pts.push_back(cv::Point2f(0, 0));
	dst_pts.push_back(cv::Point2f(1, 0));
	dst_pts.push_back(cv::Point2f(2, 1));
	dst_pts.push_back(cv::Point2f(0, 1));

	std::vector<cv::DMatch> matches;
	for (int i = 0; i < src_pts.size(); ++i)
		matches.push_back(cv::DMatch(i, i, 0));

	cv::Ptr<cv::ThinPlateSplineShapeTransformer> tps 
		= cv::createThinPlateSplineShapeTransformer(0);
	tps->estimateTransformation(src_pts, dst_pts, matches);

	cv::Mat dst_img;
	tps->warpImage(src_img, dst_img);

	cv::pyrDown(src_img, src_img);
	cv::pyrDown(dst_img, dst_img);
	cv::imshow("source", src_img);
	cv::imshow("result", dst_img);
	cv::waitKey(0);
	return 0;
}
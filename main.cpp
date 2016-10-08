#include "opencv2/opencv.hpp"

std::vector<cv::Point2f> init_bezigon() {
	std::vector<cv::Point2f> bezigon;
	bezigon.push_back(cv::Point2f(100, 100));
	bezigon.push_back(cv::Point2f(200, 200));
	bezigon.push_back(cv::Point2f(200, 200));
	bezigon.push_back(cv::Point2f(300, 100));
	bezigon.push_back(cv::Point2f(200, 200));
	bezigon.push_back(cv::Point2f(200, 200));
	bezigon.push_back(cv::Point2f(300, 300));
	bezigon.push_back(cv::Point2f(200, 200));
	bezigon.push_back(cv::Point2f(200, 200));
	bezigon.push_back(cv::Point2f(100, 300));
	bezigon.push_back(cv::Point2f(200, 200));
	bezigon.push_back(cv::Point2f(200, 200));
	return bezigon;
}

void draw_bezigon(
	cv::Mat img,
	const std::vector<cv::Point2f> bezigon,
	const cv::Scalar curve_color,
	const int thickness = 1,
	const float ctrl_alpha = 0.7f,
	const cv::Scalar ctrl_color = cv::Scalar(0, 255, 255),
	const int n_samples = 100)
{
	const int n_pts = bezigon.size();
	float prev_x = 0, prev_y = 0;
	for (int i = 0; i < n_pts; i += 3) {
		for (float t = 0; t < 1; t += 1.0f / n_samples) {
			float t2 = t * t;
			float t3 = t2 * t;
			float nt = 1 - t;
			float nt2 = nt * nt;
			float nt3 = nt2 * nt;
			float x = bezigon[i + 0].x * nt3
				+ bezigon[i + 1].x * 3 * nt2 * t
				+ bezigon[i + 2].x * 3 * nt * t2
				+ bezigon[(i + 3) % n_pts].x * t3;
			float y = bezigon[i + 0].y * nt3
				+ bezigon[i + 1].y * 3 * nt2 * t
				+ bezigon[i + 2].y * 3 * nt * t2
				+ bezigon[(i + 3) % n_pts].y * t3;
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
		for (int i = 0; i < n_pts; i += 3) {
			cv::line(img, bezigon[i + 0], bezigon[i + 1], ctrl_color, 1, cv::LINE_AA);
			cv::line(img, bezigon[i + 2], bezigon[(i + 3) % n_pts], ctrl_color, 1, cv::LINE_AA);
		}
		for each (auto pt in bezigon)
			cv::circle(img, pt, 3, ctrl_color, -1, cv::LINE_AA);
		img = img * ctrl_alpha + img_copy * (1 - ctrl_alpha);
	}
}

int find_nearest(const std::vector<cv::Point2f> & bezigon, int x, int y, float min_dist = 5.0f)
{
	int min_idx = -1;
	for (int i = 0; i < bezigon.size(); ++i) {
		float dx = bezigon[i].x - x;
		float dy = bezigon[i].y - y;
		float dist = sqrtf(dx * dx + dy * dy);
		if (dist < min_dist) {
			min_dist = dist;
			min_idx = i;
		}
	}
	return min_idx;
}

void redraw(const cv::Mat & src_img, const std::vector<cv::Point2f> & bezigon) {
	cv::Mat img = src_img.clone();
	draw_bezigon(img, bezigon, cv::Scalar(255, 0, 255), 2);
	cv::pyrDown(img, img);
	cv::imshow("source", img);
}


cv::Mat src_img = cv::imread("test.jpg");
std::vector<cv::Point2f> bezigon = init_bezigon();
int current_idx = -1;

void mouse_handler(int event, int x, int y, int flags, void* param)
{
	x *= 2;
	y *= 2;
	switch (event) {
	case cv::EVENT_LBUTTONDOWN:
		current_idx = find_nearest(bezigon, x, y);
		break;
	case cv::EVENT_MOUSEMOVE:
		if (current_idx != -1) {
			bezigon[current_idx].x = x;
			bezigon[current_idx].y = y;
		}
		break;
	case cv::EVENT_LBUTTONUP:
		current_idx = -1;
		break;
	}
	redraw(src_img, bezigon);
}


int main()
{
	cv::namedWindow("source");
	cv::setMouseCallback("source", mouse_handler, NULL);
	redraw(src_img, bezigon);

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

	cv::pyrDown(dst_img, dst_img);
	cv::imshow("result", dst_img);
	cv::waitKey(0);
	return 0;
}
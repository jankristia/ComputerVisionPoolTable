#include "BallDetector.hpp"

void BallDetector::detectBalls(cv::Mat frame) {
    cv::Mat gray, blurred, edges;
    std::vector<cv::Vec3f> circles;

    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 2, 2);

    cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1, blurred.rows / 8, 100, 30, 1, 20);

    cv::imshow("Blurred", blurred);

    this->detectedBalls = circles;


    for (size_t i = 0; i < circles.size(); i++) {
        cv::Vec3i c = circles[i];
        cv::Point center = cv::Point(c[0], c[1]);
        int radius = c[2];
        cv::circle(frame, center, radius, cv::Scalar(0, 255, 0), 3);
    }
    cv::imshow("Detected Balls", frame);
    cv::waitKey(0);
}
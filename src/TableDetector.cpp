#include "TableDetector.hpp"

void TableDetector::detectTable(cv::Mat frame) {
    // first color-based segmentation, then edge detection, then hough transform
    cv::Mat hsv, mask;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    // Choose refrence color from a neighbourhood in the middle of the image
    cv::Vec3b refColor = hsv.at<cv::Vec3b>(frame.rows/2, frame.cols/2);
    int hueTolerance = 40;
    int saturationTolerance = 100;
    int valueTolerance = 100;

    cv::inRange(hsv, cv::Scalar(refColor[0] - hueTolerance, refColor[1] - saturationTolerance, refColor[2] - valueTolerance), cv::Scalar(refColor[0] + hueTolerance, refColor[1] + saturationTolerance, refColor[2] + valueTolerance), mask);

    // Apply GaussianBlur to reduce noise
    cv::Mat blur;
    cv::GaussianBlur(mask, blur, cv::Size(5, 5), 0);

    // Morphological operations
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(blur, blur, cv::MORPH_CLOSE, kernel);

    // Apply Canny edge detection
    cv::Mat edges;
    cv::Canny(blur, edges, 75, 150);

    cv::imshow("Edges", edges);
    

    // Find lines using Hough Transform
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI/180, 100);

    // Choose one line from each bundle of parallel and close lines
    std::vector<cv::Vec2f> uniqueLines;
    for(size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0], theta = lines[i][1];
        bool add = true;
        for(size_t j = 0; j < uniqueLines.size(); j++) {
            float rho2 = uniqueLines[j][0], theta2 = uniqueLines[j][1];
            if(abs(rho - rho2) < 70 && abs(theta - theta2) < CV_PI/12) {
                add = false;
                break;
            }
        }
        if(add) {
            uniqueLines.push_back(cv::Vec2f(rho, theta));
        }
    }
    

    // Draw lines on the frame
    for(size_t i = 0; i < uniqueLines.size(); i++) {
        float rho = uniqueLines[i][0], theta = uniqueLines[i][1];
        cv::Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));
        cv::line(frame, pt1, pt2, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
        std::cout << "Line " << i << ": rho = " << rho << ", theta = " << theta << std::endl;
    }

    // Display the frame
    cv::imshow("Detected Table", frame);

    cv::waitKey(0);
}
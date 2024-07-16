// Main author: Jan Kristian Alstergren
#include "TableDetector.hpp"

void TableDetector::setTableLines(cv::Mat frame) {
    // first color-based segmentation, then edge detection, then hough transform
    cv::Mat hsv, mask;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    mask = removeTableColor(hsv);

    cv::Mat blur;
    cv::GaussianBlur(mask, blur, cv::Size(5, 5), 0);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(blur, blur, cv::MORPH_CLOSE, kernel);

    cv::Mat edges;
    cv::Canny(blur, edges, 75, 150);

    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI/180, 100);

    std::vector<cv::Vec2f> uniqueLines = chooseFourLines(lines);

    this->detectedLines = uniqueLines;
}

void TableDetector::setRoiTable(cv::Mat frame) {
    this->setTableLines(frame);

    std::vector<cv::Point> intersections = this->findIntersections(frame);

    cv::Point centroid = computeCentroid(intersections);

    std::vector<cv::Point> orderedIntersections = refineIntersections(intersections, centroid);
    this->tableCorners = orderedIntersections;
    // Make a mask of the table region
    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::fillConvexPoly(mask, orderedIntersections, cv::Scalar(255));

    // Apply the mask to the frame
    cv::Mat roiTable;
    frame.copyTo(roiTable, mask);

    this->roiTable = roiTable;
}

cv::Mat TableDetector::detectTable(cv::Mat frame) {
    this->setRoiTable(frame);
    return this->roiTable;
}

cv::Mat TableDetector::removeTableColor(cv::Mat hsvFrame) {

    // Choose refrence color from a neighbourhood in the middle of the image
    cv::Vec3b refColor = hsvFrame.at<cv::Vec3b>(hsvFrame.rows/2, hsvFrame.cols/2);
    int hueTolerance = 40;
    int saturationTolerance = 100;
    int valueTolerance = 100;
    cv::Mat mask;
    cv::inRange(hsvFrame, 
                cv::Scalar(refColor[0] - hueTolerance,
                refColor[1] - saturationTolerance,
                refColor[2] - valueTolerance),
                cv::Scalar(refColor[0] + hueTolerance,
                refColor[1] + saturationTolerance, refColor[2] + valueTolerance),
                 mask);
    return mask;
};

std::vector<cv::Vec2f> TableDetector::chooseFourLines(std::vector<cv::Vec2f> lines) {
    std::vector<cv::Vec2f> uniqueLines;
    for(size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0], theta = lines[i][1];
        bool add = true;
        for(size_t j = 0; j < uniqueLines.size(); j++) {
            float rho2 = uniqueLines[j][0], theta2 = uniqueLines[j][1];
            if(abs(rho - rho2) < 200 && abs(theta - theta2) < CV_PI/6) {
                add = false;
                break;
            }
        }
        if(add) {
            uniqueLines.push_back(cv::Vec2f(rho, theta));
        }
    }
    return uniqueLines;
}

void TableDetector::drawDetectedLines(cv::Mat frame) {
        // Draw lines on the frame
    for(size_t i = 0; i < this->detectedLines.size(); i++) {
        float rho = this->detectedLines[i][0], theta = this->detectedLines[i][1];
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
}

std::vector<cv::Point> TableDetector::findIntersections(cv::Mat frame) {
    // Find the intersections of the detected lines
    std::vector<cv::Point> intersections;
    for(size_t i = 0; i < this->detectedLines.size(); i++) {
        for(size_t j = i + 1; j < this->detectedLines.size(); j++) {
            float rho1 = this->detectedLines[i][0], theta1 = this->detectedLines[i][1];
            float rho2 = this->detectedLines[j][0], theta2 = this->detectedLines[j][1];
            double a1 = cos(theta1), b1 = sin(theta1);
            double a2 = cos(theta2), b2 = sin(theta2);
            double det = a1 * b2 - a2 * b1;
            if(det != 0) {
                cv::Point pt;
                pt.x = cvRound((b2 * rho1 - b1 * rho2) / det);
                pt.y = cvRound((a1 * rho2 - a2 * rho1) / det);
                if(pt.x >= 0 && pt.x < frame.cols && pt.y >= 0 && pt.y < frame.rows) {
                    intersections.push_back(pt);
                }
            }
        }
    }
    return intersections;
}

cv::Point TableDetector::computeCentroid(std::vector<cv::Point> intersections) {
    cv::Point centroid;
    for(size_t i = 0; i < intersections.size(); i++) {
        centroid.x += intersections[i].x;
        centroid.y += intersections[i].y;
    }
    centroid.x /= intersections.size();
    centroid.y /= intersections.size();
    return centroid;
}

std::vector<cv::Point> TableDetector::refineIntersections(std::vector<cv::Point> intersections, cv::Point centroid) {
   // Compute angle between centroin and each intersection
    std::vector<double> angles;
    for(size_t i = 0; i < intersections.size(); i++) {
        double angle = atan2(intersections[i].y - centroid.y, intersections[i].x - centroid.x);
        angles.push_back(angle);
    }

    // Order the intersections clockwise by angle, starting from the smallest angle
    std::vector<cv::Point> orderedIntersections;
    std::vector<double> orderedAngles;
    for(size_t i = 0; i < intersections.size(); i++) {
        double minAngle = CV_PI;
        size_t minIndex = 0;
        for(size_t j = 0; j < intersections.size(); j++) {
            if(angles[j] < minAngle) {
                minAngle = angles[j];
                minIndex = j;
            }
        }
        orderedIntersections.push_back(intersections[minIndex]);
        orderedAngles.push_back(angles[minIndex]);
        angles[minIndex] = CV_PI;
    }

    // move orderedIntersections[i] closer to centroid
    for (int i = 0; i < orderedIntersections.size(); i++) {
        cv::Point direction = orderedIntersections[i] - centroid;
        if(direction.x > 0) {
            orderedIntersections[i].x -= 10;
        } else {
            orderedIntersections[i].x += 10;
        }
        if(direction.y > 0) {
            orderedIntersections[i].y -= 10;
        } else {
            orderedIntersections[i].y += 10;
        }
    }
    return orderedIntersections;
}
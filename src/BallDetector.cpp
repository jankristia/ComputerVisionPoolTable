// Main author: Jan Kristian Alstergren
#include "BallDetector.hpp"
#include <iostream>


BallDetector::BallDetector(): colorToHueMap({
        {YELLOW, 20},
        {BLUE, 106},
        {RED, 170},
        {PURPLE, 119},
        {ORANGE, 7},
        {GREEN, 77},
        {MAROON, 9},
        {WHITE, 35},
        {BLACK, 90}
    }) { }

void BallDetector::setTableColor(cv::Mat frame) {
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    cv::Vec3f tableColor = cv::Vec3f(0, 0, 0);
    for (int i = frame.rows / 2 - 7; i < frame.rows / 2 + 8; i++) {
        for (int j = frame.cols / 2 - 7; j < frame.cols / 2 + 8; j++) {
            tableColor += hsvFrame.at<cv::Vec3b>(i, j);
        }
    }
    tableColor /= 225;

    this->tableColor = tableColor;
}

bool BallDetector::isInRange(cv::Vec3f testColor, cv::Vec3f refrenceColor, int threshold,  int indexToCheck) {
    double diff = std::abs(testColor[indexToCheck] - refrenceColor[indexToCheck]);
    return diff < threshold;
}


void BallDetector::detectBalls(cv::Mat frame) {    
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

    cv::Mat blurred;
    cv::GaussianBlur(grayFrame, blurred, cv::Size(21, 21), 2, 2);

    cv::Mat equalized;
    cv::equalizeHist(blurred, equalized);

    cv::imshow("Equalized", equalized);

    cv::Mat edges;
    cv::Canny(equalized, edges, 100, 300);

    // Show edges image
    cv::imshow("Canny Edges", edges);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(edges, circles, cv::HOUGH_GRADIENT, 1, 25, 1, 10, 7, 21);

    
    this->setTableColor(frame);
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);
   // Remove circles that have the same average color as the table
   // in the neighbourhood of the center of the circle
    for(int i = 0; i < circles.size(); i++) {
        int x = cvRound(circles[i][0]);
        int y = cvRound(circles[i][1]);
        int radius = 6;
        int tableColorTreshold = 4;
        int hueSum = 0;
        for(int j = x - radius; j < x + radius; j++) {
            for(int k = y - radius; k < y + radius; k++) {
                cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(k, j);
                hueSum += abs(pixel[0] - this->tableColor[0]);
            }
        }
        if(hueSum/(radius*radius) <= tableColorTreshold) {
            circles.erase(circles.begin() + i);
            i--;
        }
    }

    // Remove circles that have more than 50 pixels = 0, 0, 0 the square radius*radius
    for(int i = 0; i < circles.size(); i++) {
        int x = cvRound(circles[i][0]);
        int y = cvRound(circles[i][1]);
        int radius = cvRound(circles[i][2]);
        int blackCount = 0;
        int radiusTreshold = 50;
        for(int j = x - radius; j < x + radius; j++) {
            for(int k = y - radius; k < y + radius; k++) {
                cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(k, j);
                if(pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) {
                    blackCount++;
                }
            }
        }
        if(blackCount > radiusTreshold) {
            circles.erase(circles.begin() + i);
            i--;
        }
    }


    this->detectedBalls = circles;
    this->setBoundingBoxes();
}

void BallDetector::setBoundingBoxes() {
    for(int i = 0; i < this->detectedBalls.size(); i++) {
        cv::Rect2d boundingBox = cv::Rect2d(cvRound(this->detectedBalls[i][0] - this->detectedBalls[i][2]), 
                                            cvRound(this->detectedBalls[i][1] - this->detectedBalls[i][2]),
                                            cvRound(2*this->detectedBalls[i][2]), 
                                            cvRound(2*this->detectedBalls[i][2]));
        this->boundingBoxes.push_back(boundingBox);
    }
}

void BallDetector::detectWhiteBall(cv::Mat frame) {

    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    std::vector<cv::Vec3f> detectedBallsTemp = this->detectedBalls;

    // Thresholds for the white ball, found by inspection
    int hueL = 20;
    int hueH = 49;
    int satL = 20;
    int satH = 110;
    int valL = 200;
    int valH = 255;

    // Find the white ball
    int whiteIndex = 0;
    int largestWhitePart = 0;
    for(int i = 0; i < detectedBallsTemp.size(); i++) {
        int x = cvRound(detectedBallsTemp[i][0]);
        int y = cvRound(detectedBallsTemp[i][1]);
        int radius = cvRound(detectedBallsTemp[i][2]);
        int whiteCount = 0;
        for(int j = x - radius; j < x + radius; j++) {
            for(int k = y - radius; k < y + radius; k++) {
                cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(k, j);
                if(pixel[0] >= hueL && pixel[0] <= hueH && 
                    pixel[1] >= satL && pixel[1] <= satH && 
                    pixel[2] >= valL && pixel[2] <= valH) {
                    whiteCount++;
                }
            }
        }
        if(whiteCount > largestWhitePart) {
            largestWhitePart = whiteCount;
            whiteIndex = i;
        }
    }

    // Draw the white ball
    cv::circle(frame, cv::Point(cvRound(detectedBallsTemp[whiteIndex][0]), cvRound(detectedBallsTemp[whiteIndex][1])), cvRound(detectedBallsTemp[whiteIndex][2]), cv::Scalar(255, 255, 255), 2);

    // Display the frame
    cv::namedWindow("White ball", cv::WINDOW_AUTOSIZE);
    cv::imshow("White ball", frame);
}

void BallDetector::detectBlackBall(cv::Mat frame) {
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    std::vector<cv::Vec3f> detectedBallsTemp = this->detectedBalls;

    // Thresholds for the black ball, found by inspection
    int hueL = 80;
    int hueH = 105;
    int satL = 200;
    int satH = 255;
    int valL = 0;
    int valH = 70;

    // Find the black ball
    int blackIndex = 0;
    int largestBlackPart = 0;
    for(int i = 0; i < detectedBallsTemp.size(); i++) {
        int x = cvRound(detectedBallsTemp[i][0]);
        int y = cvRound(detectedBallsTemp[i][1]);
        int radius = cvRound(detectedBallsTemp[i][2]);
        int blackCount = 0;
        for(int j = x - radius; j < x + radius; j++) {
            for(int k = y - radius; k < y + radius; k++) {
                cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(k, j);
                if(pixel[0] >= hueL && pixel[0] <= hueH && 
                    pixel[1] >= satL && pixel[1] <= satH && 
                    pixel[2] >= valL && pixel[2] <= valH) {
                    blackCount++;
                }
            }
        }
        if(blackCount > largestBlackPart) {
            largestBlackPart = blackCount;
            blackIndex = i;
        }
    }

    // Draw the black ball
    cv::circle(frame, cv::Point(cvRound(detectedBallsTemp[blackIndex][0]), cvRound(detectedBallsTemp[blackIndex][1])), cvRound(detectedBallsTemp[blackIndex][2]), cv::Scalar(0, 0, 0), 2);

    // Display the frame
    cv::namedWindow("Black ball", cv::WINDOW_AUTOSIZE);
    cv::imshow("Black ball", frame);
}






























void BallDetector::detectSolidBalls(cv::Mat frame) {
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    cv::Mat coloredBallMask = cv::Mat::zeros(frame.size(), CV_8UC1);

    cv::Vec3f redBallColor = cv::Vec3f(colorToHueMap.at(RED), 0, 0);

    for( int i = 0; i < this->detectedBalls.size(); i++ ) {
        // Loop thorugh a 30x30 square around the ball center
        int x = cvRound(this->detectedBalls[i][0]);
        int y = cvRound(this->detectedBalls[i][1]);
        int redBallCount = 0;
        int whiteCount = 0;
        for(int j = x - 15; j < x + 15; j++) {
            for(int k = y - 15; k < y + 15; k++) {
                cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(k, j);
                if(this->isInRange(pixel, redBallColor, 20, 0)) {
                    coloredBallMask.at<uchar>(k, j) = 255;
                    redBallCount++;
                }
            }
        }
        if(redBallCount > 50) {
            cv::circle(frame, cv::Point(cvRound(this->detectedBalls[i][0]), cvRound(this->detectedBalls[i][1])), cvRound(this->detectedBalls[i][2]), cv::Scalar(0, 0, 255), 2);
        }
    }

    cv::namedWindow("Red ball mask", cv::WINDOW_AUTOSIZE);
    cv::imshow("Red ball mask", frame);

}

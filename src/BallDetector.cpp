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
}

int BallDetector::detectWhiteBallIndex(cv::Mat frame) {

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
    return whiteIndex;
}

int BallDetector::detectBlackBallIndex(cv::Mat frame) {
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
    return blackIndex;
}

void BallDetector::segmentBalls(cv::Mat frame) {
    this->detectBalls(frame);
    int whiteIndex = this->detectWhiteBallIndex(frame);
    int blackIndex = this->detectBlackBallIndex(frame);

    std::vector<cv::Vec3f> detectedBallsTemp = this->detectedBalls;

    // Add the white ball to segmentedBalls with ballType = 1
    BoundingBox whiteBox = {cvRound(detectedBallsTemp[whiteIndex][0] - detectedBallsTemp[whiteIndex][2]), 
                            cvRound(detectedBallsTemp[whiteIndex][1] - detectedBallsTemp[whiteIndex][2]), 
                            cvRound(detectedBallsTemp[whiteIndex][2]*2), 
                            cvRound(detectedBallsTemp[whiteIndex][2]*2), 1};
    this->segmentedBalls.push_back(whiteBox);
    BoundingBox blackBox = {cvRound(detectedBallsTemp[blackIndex][0] - detectedBallsTemp[blackIndex][2]), 
                            cvRound(detectedBallsTemp[blackIndex][1] - detectedBallsTemp[blackIndex][2]), 
                            cvRound(detectedBallsTemp[blackIndex][2]*2), 
                            cvRound(detectedBallsTemp[blackIndex][2]*2), 2};
    // Add the black ball to segmentedBalls with ballType = 2
    this->segmentedBalls.push_back(blackBox);
    
    // Delete the white from detectedBallsTemp
    detectedBallsTemp.erase(detectedBallsTemp.begin() + whiteIndex);
    if(whiteIndex < blackIndex) {
        blackIndex--;
    }
    detectedBallsTemp.erase(detectedBallsTemp.begin() + blackIndex);

    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);
    // Loop through the rest of the balls, if there are a thershold amount of white pixels in the radius ball,
    // add it to segmentedBalls with ballType = 4
    // If there are less than threshold amount of white pixels, add it to segmentedBalls with ballType = 3
    for(int i = 0; i < detectedBallsTemp.size(); i++) {
        int x = cvRound(detectedBallsTemp[i][0]);
        int y = cvRound(detectedBallsTemp[i][1]);
        int radius = cvRound(detectedBallsTemp[i][2]);
        int whiteCount = 0;
        int whiteThreshold = 20;
        for(int j = x - radius; j < x + radius; j++) {
            for(int k = y - radius; k < y + radius; k++) {
                cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(k, j);
                if(pixel[0] >= 20 && pixel[0] <= 49 && 
                    pixel[1] >= 20 && pixel[1] <= 110 && 
                    pixel[2] >= 200 && pixel[2] <= 255) {
                    whiteCount++;
                }
            }
        }
        std::cout << "White count: " << whiteCount << std::endl;
        if(whiteCount > whiteThreshold) {
            this->segmentedBalls.push_back(BoundingBox(cvRound(detectedBallsTemp[i][0] - detectedBallsTemp[i][2]), 
                                            cvRound(detectedBallsTemp[i][1] - detectedBallsTemp[i][2]), 
                                            cvRound(detectedBallsTemp[i][2]*2), 
                                            cvRound(detectedBallsTemp[i][2]*2), 4));
        } else {
            this->segmentedBalls.push_back(BoundingBox(cvRound(detectedBallsTemp[i][0] - detectedBallsTemp[i][2]), 
                                            cvRound(detectedBallsTemp[i][1] - detectedBallsTemp[i][2]), 
                                            cvRound(detectedBallsTemp[i][2]*2), 
                                            cvRound(detectedBallsTemp[i][2]*2), 3));
        }
    }
    // Finally display the bounding boxes in the frame
    // with different colors depending on the ballType
    for(int i = 0; i < this->segmentedBalls.size(); i++) {
        cv::Scalar color;
        if(this->segmentedBalls[i].ballType == 1) {
            color = cv::Scalar(255, 255, 255);
        } else if(this->segmentedBalls[i].ballType == 2) {
            color = cv::Scalar(0, 0, 0);
        } else if(this->segmentedBalls[i].ballType == 3) {
            color = cv::Scalar(0, 0, 255);
        } else if(this->segmentedBalls[i].ballType == 4) {
            color = cv::Scalar(255, 0, 0);
        }
        // Draw the bounding boxes on the frame
        cv::rectangle(frame, cv::Rect(this->segmentedBalls[i].x,
                                        this->segmentedBalls[i].y,
                                        this->segmentedBalls[i].width,
                                        this->segmentedBalls[i].height), 
                                        color, 2);
    }

    cv::imshow("Segmented Balls", frame);
}
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
    // this->setTableColor(frame);
    // // loop through the frame and sett all pixels with table color to white
    // cv::Mat hsvFrame;
    // cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    // // Assuming you have the hue, saturation, and value range for the table color
    // int lowH = this->tableColor[0] - 3;
    // int highH = this->tableColor[0] + 3;
    // int lowS = 0;        // this->tableColor[1] - 50;
    // int highS = 255;     // this->tableColor[1] + 50;
    // int lowV = 0;        // this->tableColor[2] - 50;
    // int highV = 255;     // this->tableColor[2] + 50;
    // cv::Mat mask;
    // cv::inRange(hsvFrame, cv::Scalar(lowH, lowS, lowV), cv::Scalar(highH, highS, highV), mask);
    // cv::bitwise_not(mask, mask);  // Invert mask to keep everything except the table color

    // cv::Mat noTableColor;
    // cv::bitwise_and(frame, frame, noTableColor, mask);

    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

    cv::Mat blurred;
    cv::GaussianBlur(grayFrame, blurred, cv::Size(7, 7), 2, 2);

    cv::Mat equalized;
    cv::equalizeHist(blurred, equalized);

    cv::Mat edges;
    cv::Canny(equalized, edges, 100, 200);

    // Show edges image
    cv::imshow("Canny Edges", edges);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1, 20, 1, 11, 7, 13);

    // Only keep circles that have colors in a range of the colorToHueMap

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

            // Find the white ball
            int whiteIndex = 0;
            int largestWhitePart = 0;
            for(int i = 0; i < detectedBallsTemp.size(); i++) {
                cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(cvRound(detectedBallsTemp[i][1]), cvRound(detectedBallsTemp[i][0]));

                cv::circle(frame, cv::Point(cvRound(detectedBallsTemp[i][0]), cvRound(detectedBallsTemp[i][1])), 1, cv::Scalar(0, 0, 255), 2);
                
                // Thresholds for calling pxel a color
                int whiteTreshold = 5;
                int colorTreshold = 5;
                int blackTreshold = 5;

                // Counter variables
                int whiteCount = 0;
                int colorCount = 0;
                int blackCount = 0;

                // Loop through a square around the pixel, go radius in each direction
                int radius = cvRound(detectedBallsTemp[i][2]);
                int x = cvRound(detectedBallsTemp[i][0]);
                int y = cvRound(detectedBallsTemp[i][1]);
                for(int j = x - radius - 3; j < x + radius + 3; j++) {
                    for(int k = y - radius - 3; k < y + radius + 3; k++) {
                        cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(k, j);
                        if(this->isInRange(pixel, cv::Vec3f(colorToHueMap.at(WHITE), 0, 0), whiteTreshold, 0)){
                            whiteCount++;
                        } else if(this->isInRange(pixel, cv::Vec3f(colorToHueMap.at(BLACK), 0, 0), blackTreshold)){
                            blackCount++;
                        } else if(this->isInRange(pixel, cv::Vec3f(colorToHueMap.at(RED), 0, 0), colorTreshold) ||
                                    this->isInRange(pixel, cv::Vec3f(colorToHueMap.at(BLUE), 0, 0), colorTreshold) ||
                                    this->isInRange(pixel, cv::Vec3f(colorToHueMap.at(ORANGE), 0, 0), colorTreshold) ||
                                    this->isInRange(pixel, cv::Vec3f(colorToHueMap.at(PURPLE), 0, 0), colorTreshold) ||
                                    this->isInRange(pixel, cv::Vec3f(colorToHueMap.at(YELLOW), 0, 0), colorTreshold) ||
                                    this->isInRange(pixel, cv::Vec3f(colorToHueMap.at(GREEN), 0, 0), colorTreshold) ||
                                    this->isInRange(pixel, cv::Vec3f(colorToHueMap.at(MAROON), 0, 0), colorTreshold)){
                            colorCount++;
                            colorCount++;
                        } 
                    }
                }

                if((whiteCount-blackCount-colorCount)/float(radius*radius) >= largestWhitePart ) {
                    whiteIndex = i;
                    largestWhitePart = (whiteCount)/float(radius*radius);
                }
            }
            // Draw the white ball in the frame
            cv::circle(frame, cv::Point(cvRound(detectedBallsTemp[whiteIndex][0]), cvRound(detectedBallsTemp[whiteIndex][1])), cvRound(detectedBallsTemp[whiteIndex][2]), cv::Scalar(0, 0, 255), 2);

            // Display the frame
            cv::namedWindow("Detected white ball", cv::WINDOW_AUTOSIZE);
            cv::imshow("Detected white ball", frame);

            this->whiteBall = detectedBallsTemp[whiteIndex];
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

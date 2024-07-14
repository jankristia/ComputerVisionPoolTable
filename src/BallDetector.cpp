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
        {WHITE, 42},
        {BLACK, 90}
    }) {}

void BallDetector::setTableColor(cv::Mat frame) {
    // Convert the frame to HSV
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    // Find the color of the table by averaging the color of the 15x15 middle pixels of the frame
    cv::Vec3f tableColor = cv::Vec3f(0, 0, 0);
    for (int i = frame.rows / 2 - 7; i < frame.rows / 2 + 8; i++) {
        for (int j = frame.cols / 2 - 7; j < frame.cols / 2 + 8; j++) {
            tableColor += hsvFrame.at<cv::Vec3b>(i, j);
        }
    }
    tableColor /= 225;

    this->tableColor = tableColor;

    std::cout << "Table color: " << tableColor << std::endl;
}

bool BallDetector::isInRange(cv::Vec3f testColor, cv::Vec3f refrenceColor, int threshold) {
    // Only testing hue
    double diff = std::abs(testColor[0] - refrenceColor[0]);
    return diff < threshold;
}

void BallDetector::segmentBallColors(cv::Mat frame) {
    // Convert the frame to HSV
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    // initialize image in same size as frame
    cv::Mat whiteBallMask = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::Mat coloredBallMask = cv::Mat::zeros(frame.size(), CV_8UC1);
    
    cv::Vec3f whiteBallColor = cv::Vec3f(colorToHueMap.at(WHITE), 0, 0);
    // Loop through the frame and find the white ball
    for (int i = 0; i < frame.rows; i++) {
        for (int j = 0; j < frame.cols; j++) {
            cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(i, j);
            if (isInRange(pixel, whiteBallColor, 20)) {
                whiteBallMask.at<uchar>(i, j) = 255;
            } else {
                whiteBallMask.at<uchar>(i, j) = 0;
            }
            if (isInRange(pixel, cv::Vec3f(colorToHueMap.at(RED), 0, 0), 30)) {
                coloredBallMask.at<uchar>(i, j) = 255;
            } else {
                coloredBallMask.at<uchar>(i, j) = 0;
            }
        }
    }

    // Mabye use morphological operations to remove noise

    cv::Mat detectedWhiteBallMask = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::Mat detectedColoredBallMask = cv::Mat::zeros(frame.size(), CV_8UC1);
    
    int windowSize = 29;
    int whiteThreshold = 50;
    int redThreshold = 50;
    for (int i = 0; i < whiteBallMask.rows - windowSize; i++) {
        for (int j = 0; j < whiteBallMask.cols - windowSize; j++) {
            int whiteCount = 0;
            int redCount = 0;
            for (int k = i - (windowSize-1)/2; k < i + (windowSize-1)/2; k++) {
                for (int l = j - (windowSize-1)/2; l < j + (windowSize-1)/2; l++) {
                    if (whiteBallMask.at<uchar>(k, l) == 255) {
                        whiteCount++;
                    }
                    if (coloredBallMask.at<uchar>(k, l) == 255) {
                        redCount++;
                    }
                }
            }
            if (whiteCount > whiteThreshold) {
                detectedWhiteBallMask.at<uchar>(i, j) = 255;
                frame.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
            } else {
                detectedWhiteBallMask.at<uchar>(i, j) = 0;
            }
            if (redCount > redThreshold) {
                detectedColoredBallMask.at<uchar>(i, j) = 255;
                frame.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            } else {
                detectedColoredBallMask.at<uchar>(i, j) = 0;
            }
        }
    }
    // Add some morphological operations to remove noise?


    cv::namedWindow("Frame", cv::WINDOW_AUTOSIZE);
    cv::imshow("Frame", frame);
}
#include "BallDetector.hpp"
#include <iostream>


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

void BallDetector::detectWhiteBall(cv::Mat frame) {
    // Convert the frame to HSV
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

    // initialize image in same size as frame
    cv::Mat whiteBallMask = cv::Mat::zeros(frame.size(), CV_8UC1);
    
    cv::Vec3f whiteBallColor = cv::Vec3f(42, 80, 200);
    // Loop through the frame and find the white ball
    for (int i = 0; i < frame.rows; i++) {
        for (int j = 0; j < frame.cols; j++) {
            cv::Vec3b pixel = hsvFrame.at<cv::Vec3b>(i, j);
            if (isInRange(pixel, whiteBallColor, 20)) {
                whiteBallMask.at<uchar>(i, j) = 255;
            } else {
                whiteBallMask.at<uchar>(i, j) = 0;
            }
        }
    }

    // Mabye use morphological operations to remove noise

    cv::namedWindow("White ball mask", cv::WINDOW_AUTOSIZE);
    cv::imshow("White ball mask", whiteBallMask);

    cv::namedWindow("White ball detection", cv::WINDOW_AUTOSIZE);
    cv::imshow("White ball detection", frame);

    cv::Mat detectedBallPointsMask = cv::Mat::zeros(frame.size(), CV_8UC1);
    // Slide 25*25 window over the whiteBallMask and count number of white pixels in each window
    int windowSize = 30;
    int whiteThreshold = 100;
    for (int i = 0; i < whiteBallMask.rows - windowSize; i++) {
        for (int j = 0; j < whiteBallMask.cols - windowSize; j++) {
            int whiteCount = 0;
            for (int k = i; k < i + windowSize; k++) {
                for (int l = j; l < j + windowSize; l++) {
                    if (whiteBallMask.at<uchar>(k, l) == 255) {
                        whiteCount++;
                    }
                }
            }
            if (whiteCount > whiteThreshold) {
                detectedBallPointsMask.at<uchar>(i, j) = 255;
            } else {
                detectedBallPointsMask.at<uchar>(i, j) = 0;
            }
        }
    }

    cv::namedWindow("Detected ball points", cv::WINDOW_AUTOSIZE);
    cv::imshow("Detected ball points", detectedBallPointsMask);
}

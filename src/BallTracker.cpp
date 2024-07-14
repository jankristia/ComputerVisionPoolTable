#include "BallTracker.hpp"

std::vector<cv::Rect> BallTracker::expandBoundingBoxes(std::vector<cv::Rect> inputBoxes) {
    std::vector<cv::Rect> expandedBoundingBoxes;
    for(const auto& box : inputBoxes) {
        cv::Rect expandedBox = cv::Rect(box.x - 12, box.y - 12, box.width + 24, box.height + 24);
        expandedBoundingBoxes.push_back(expandedBox);
    }
    return expandedBoundingBoxes;
}

void BallTracker::trackBalls(cv::VideoCapture video) {


    cv::Mat frame;
    int frameWidth = video.get(cv::CAP_PROP_FRAME_WIDTH);
    int frameHeight = video.get(cv::CAP_PROP_FRAME_HEIGHT);

    cv::VideoWriter videoWriter("output.avi", cv::VideoWriter::fourcc('M','J','P','G'), 10, cv::Size(frameWidth, frameHeight));
    
    video.read(frame);
    // Detect the pool table
    cv::Mat tableFrame;
    TableDetector tableDetector;
    tableDetector.setTableLines(frame);
    tableDetector.setRoiTable(frame);
    tableFrame = tableDetector.getRoiTable();
    tableDetector.drawDetectedLines(tableFrame);

    // Detect the balls and make bounding boxes
    BallDetector ballDetector;
    ballDetector.detectBalls(tableDetector.roiTable);
    ballDetector.setBoundingBoxes();
    
    cv::Mat gaussianFrame;
    cv::GaussianBlur(tableFrame, gaussianFrame, cv::Size(11, 11), 2, 2);

    std::vector<cv::Rect> expandedBoundingBoxes = this->expandBoundingBoxes(ballDetector.boundingBoxes);
    cv::Ptr<cv::MultiTracker> multiTracker = cv::MultiTracker::create();


    if(expandedBoundingBoxes.size() < 1) {
        std::cout << "There are no bounding boxes" << std::endl;
        return;
    }

    for(const auto& box : expandedBoundingBoxes) {
        multiTracker->add(cv::TrackerBoosting::create(), gaussianFrame, box);
    }
    int i = 0;
    while(video.read(frame)) {
        tableDetector.setRoiTable(frame);
        tableFrame = tableDetector.getRoiTable();
        cv::GaussianBlur(tableFrame, gaussianFrame, cv::Size(11, 11), 2, 2);

        multiTracker->update(gaussianFrame);
        std::cout << "Updating frame: " << i << std::endl;
        i++;
        for(const auto& box : multiTracker->getObjects()) {
            cv::rectangle(gaussianFrame, box, cv::Scalar(255, 0, 0), 2, 1);
        }
        cv::imshow("MultiTracker", gaussianFrame);
        videoWriter.write(gaussianFrame);
        
        if (cv::waitKey(75) == 'q') {
            break;
        }
    }
}
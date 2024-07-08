#include <opencv2/opencv.hpp>

class BallDetector {
    public:
        std::vector<cv::Vec3f> detectedBalls;
        std::vector<cv::Vec2f> tableBorders;
        
        void detectBalls(cv::Mat frame);
};
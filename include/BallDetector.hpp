#include <opencv2/opencv.hpp>

class BallDetector {
    public:
        std::vector<cv::Vec3f> detectedBalls;
        std::vector<cv::Vec2f> tableBorders;
        cv::Vec3f tableColor;

        void setTableColor(cv::Mat frame);
        bool isInRange(cv::Vec3f testColor, cv::Vec3f refrenceColor, int threshold);

        void detectWhiteBall(cv::Mat frame);
};
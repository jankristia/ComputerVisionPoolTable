// Main author: Jan Kristian Alstergren
#include "MeanAveragePrecision.hpp"

void MeanAveragePrecision::loadGroundTruth(const std::string& filepath) {
    std::ifstream
        file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return;
    }
    int x, y, width, height, ballType;
    while (file >> x >> y >> width >> height >> ballType) {
        BoundingBox b(x, y, width, height, ballType);
        this->groundTruth.push_back(b);
    }
}

void MeanAveragePrecision::setDetectedBoxes(std::vector<BoundingBox> detectedBoundingBoxes) {
    this->detectedBoxes = detectedBoundingBoxes;
}

double MeanAveragePrecision::calculateIoU(const BoundingBox& box1, const BoundingBox& box2) {
    int x1 = std::max(box1.x, box2.x);
    int y1 = std::max(box1.y, box2.y);
    int x2 = std::min(box1.x + box1.width, box2.x + box2.width);
    int y2 = std::min(box1.y + box1.height, box2.y + box2.height);

    int intersection = std::max(0, x2 - x1) * std::max(0, y2 - y1);
    int unionArea = box1.width * box1.height + box2.width * box2.height - intersection;

    return static_cast<double>(intersection) / static_cast<double>(unionArea);
}

std::vector<bool> MeanAveragePrecision::evaluateDetections(const std::vector<BoundingBox>& groundTruth, const std::vector<BoundingBox>& detectedBoxes, double threshold) {
    std::vector<bool> detections(detectedBoxes.size(), false);
    std::vector<bool> matched(groundTruth.size(), false);

    for (size_t i = 0; i < detectedBoxes.size(); ++i) {
        for (size_t j = 0; j < groundTruth.size(); ++j) {
            if (!matched[j] && calculateIoU(detectedBoxes[i], groundTruth[j]) > threshold) {
                detections[i] = true;
                matched[j] = true;
                break;
            }
        }
    }

    return detections;
}
void MeanAveragePrecision::calculatePrecisionRecall() {
    int truePositives = 0;
    int falsePositives = 0;
    int falseNegatives = 0;
    for(const auto& detected : this->evaluateDetections(this->groundTruth, this->detectedBoxes, 0.5)) {
        if(detected) {
            truePositives++;
        } else {
            falsePositives++;
        }
    }
    falseNegatives = this->groundTruth.size() - truePositives;
    float precision = static_cast<float>(truePositives) / static_cast<float>(truePositives + falsePositives);
    float recall = static_cast<float>(truePositives) / static_cast<float>(this->groundTruth.size());
    this->precisions.push_back(precision);
    this->recalls.push_back(recall);
}

double MeanAveragePrecision::calculateAveragePrecision() {
    // Sort by recall
    std::vector<std::pair<float, float>> prPairs;
    for (size_t i = 0; i < this->precisions.size(); i++) {
        prPairs.push_back(std::make_pair(this->recalls[i], this->precisions[i]));
    }
    std::sort(prPairs.begin(), prPairs.end());

    // Interpolate precision at each recall level
    double sumPrecision = 0.0;
    int recallLevels[11] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100}; // Recall levels from 0 to 1 with 0.1 steps
    for (int recallLevel : recallLevels) {
        float recall = recallLevel / 100.0;
        float maxPrecision = 0.0;
        for (const auto& prPair : prPairs) {
            if (prPair.first >= recall) {
                maxPrecision = std::max(maxPrecision, prPair.second);
            }
        }
        sumPrecision += maxPrecision;
    }
    return sumPrecision / 11.0; // Mean of the 11 interpolated precision values
}

double MeanAveragePrecision::averagePrecisionCalculation(cv::Mat frame, std::string groundTruthPath) {
    cv::Mat tableFrame;
    TableDetector tableDetector;
    tableFrame = tableDetector.detectTable(frame);

    // Detect the balls and make bounding boxes
    BallDetector ballDetector;
    ballDetector.segmentBalls(tableFrame);

    MeanAveragePrecision map;
    map.loadGroundTruth(groundTruthPath);
    map.setDetectedBoxes(ballDetector.segmentedBalls);
    map.calculatePrecisionRecall();
    double meanAveragePrecision = map.calculateAveragePrecision();
    return meanAveragePrecision;
}
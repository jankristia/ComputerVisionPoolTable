cmake_minimum_required(VERSION 2.8)
project(finalProject)

find_package(OpenCV REQUIRED)


include_directories( ${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable( ${PROJECT_NAME} src/main.cpp src/TableDetector.cpp src/BallDetector.cpp src/BallTracker.cpp src/MeanAveragePrecision.cpp)
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

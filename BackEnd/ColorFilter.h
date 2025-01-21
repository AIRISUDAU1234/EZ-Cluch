#ifndef PIXELSEARCH_H
#define PIXELSEARCH_H

#include <opencv2/opencv.hpp>

extern cv::Scalar base_color;

bool ColorFilter(const unsigned char* pixel, int width, int height, cv::Scalar base_color, int shadeVariation, int erode_iterations, int dilate_iterations);
bool IsColor(const unsigned char* pixel);

cv::Scalar hexToScalar(int hexColor);

#endif 
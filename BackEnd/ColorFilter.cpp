#include <opencv2/opencv.hpp>
#include <iostream>

#include "ColorFilter.h"
#include "Global.h"

bool IsColor(const unsigned char* pixel)
{
    const unsigned char blue = pixel[0];
    const unsigned char green = pixel[1];
    const unsigned char red = pixel[2];

    if (RED &&
        (red >= (0xB4 - shadeVariation) && red <= (0xB4 + shadeVariation)) &&
        (green >= (0x15 - shadeVariation) && green <= (0x15 + shadeVariation)) &&
        (blue >= (0x15 - shadeVariation) && blue <= (0x15 + shadeVariation)))
    {
        return true;
    }
    if (PURPLE &&
        (red >= (0xAF - shadeVariation) && red <= (0xAF + shadeVariation)) &&
        (green >= (0x2E - shadeVariation) && green <= (0x2E + shadeVariation)) &&
        (blue >= (0xAF - shadeVariation) && blue <= (0xAF + shadeVariation)))
    {
        return true;
    }
    if (YELLOW &&
        (red >= (0xC8 - shadeVariation) && red <= (0xC8 + shadeVariation)) &&
        (green >= (0xC7 - shadeVariation) && green <= (0xC7 + shadeVariation)) &&
        (blue >= (0x22 - shadeVariation) && blue <= (0x22 + shadeVariation)))
    {
        return true;
    }
    return false;
}

bool ColorFilter(const unsigned char* pixel, int width, int height, cv::Scalar base_color, int shadeVariation, int erode_iterations, int dilate_iterations)
{
    cv::Mat input_image(height, width, CV_8UC3, const_cast<unsigned char*>(pixel));

    cv::Mat hsv_image;
    cv::cvtColor(input_image, hsv_image, cv::COLOR_BGR2HSV);

    cv::Scalar lower_hsv(
        (base_color[0] - shadeVariation > 0.0) ? (base_color[0] - shadeVariation) : 0.0,  // H 下限
        (base_color[1] - shadeVariation > 0.0) ? (base_color[1] - shadeVariation) : 0.0,  // S 下限
        (base_color[2] - shadeVariation > 0.0) ? (base_color[2] - shadeVariation) : 0.0   // V 下限
    );

    cv::Scalar upper_hsv(
        (base_color[0] + shadeVariation <= 179.0) ? (base_color[0] + shadeVariation) : 179.0, // H 上限
        (base_color[1] + shadeVariation <= 255.0) ? (base_color[1] + shadeVariation) : 255.0, // S 上限
        (base_color[2] + shadeVariation <= 255.0) ? (base_color[2] + shadeVariation) : 255.0  // V 上限
    );

    cv::Mat mask;
    cv::inRange(hsv_image, lower_hsv, upper_hsv, mask);

    cv::Mat eroded_mask, dilated_mask;
    cv::erode(mask, eroded_mask, cv::Mat(), cv::Point(-1, -1), erode_iterations);
    cv::dilate(eroded_mask, dilated_mask, cv::Mat(), cv::Point(-1, -1), dilate_iterations);

    if (cv::countNonZero(dilated_mask) > 0) 
    {
        return true; 
    }
    return false; 
}

cv::Scalar hexToScalar(int hexColor) 
{
    int blue = hexColor & 0xFF;         // 低位8ビット (B)
    int green = (hexColor >> 8) & 0xFF; // 中間8ビット (G)
    int red = (hexColor >> 16) & 0xFF;  // 高位8ビット (R)
    return cv::Scalar(blue, green, red); 
}
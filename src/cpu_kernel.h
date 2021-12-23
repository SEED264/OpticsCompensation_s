#ifndef _OPTICSCOMPENSATION_S_SRC_CPU_KERNEL_H_
#define _OPTICSCOMPENSATION_S_SRC_CPU_KERNEL_H_

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <aut/AUL_Type.h>
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include "parameter.h"

template<typename T> cv::Vec<T, 4> SamplingPixel(const cv::Mat &img, int x, int y,
                                                 const aut::Size2D &image_size) {
    // Return all 0 if pt is out of range
    if (x < 0 || x >= image_size.w || y < 0 || y >= image_size.h)
        return cv::Vec<T, 4>(cv::Scalar::all(0));

    return img.ptr<cv::Vec<T, 4>>(y)[x];
}

template<typename T> cv::Vec4f SamplingPixel(const cv::Mat &img, float x, float y,
                                             const aut::Size2D &image_size) {
    float dummy;
    float dx = (x >= 0) ? std::fmod(x, 1.f) : 1.f - std::abs(std::fmod(x, 1.f));
    float dy = (y >= 0) ? std::fmod(y, 1.f) : 1.f - std::abs(std::fmod(y, 1.f));

    int fx = static_cast<int>(std::floor(x));
    int fy = static_cast<int>(std::floor(y));
    int cx = static_cast<int>(std::ceil(x));
    int cy = static_cast<int>(std::ceil(y));

    auto plt = cv::Vec4f(SamplingPixel<T>(img, fx, fy, image_size));
    auto prt = cv::Vec4f(SamplingPixel<T>(img, cx, fy, image_size));
    auto plb = cv::Vec4f(SamplingPixel<T>(img, fx, cy, image_size));
    auto prb = cv::Vec4f(SamplingPixel<T>(img, cx, cy, image_size));

    return (1 - dx) * (1 - dy) * plt +
                dx  * (1 - dy) * prt +
           (1 - dx) *      dy  * plb +
                dx  *      dy  * prb;
}

void PremultKernel(const cv::Mat &in_image, const cv::Mat &out_image);
void UnpremultKernel(const cv::Mat &in_image, const cv::Mat &out_image);

void SpoolCPUKernel(const cv::Mat &in_image, const cv::Mat &out_image,
                    const aut::Size2D &image_size,
                    OpticsCompensationParameter parameter);

void BarrelCPUKernel(const cv::Mat &in_image, const cv::Mat &out_image,
                     const aut::Size2D &image_size,
                     OpticsCompensationParameter parameter);

inline glm::vec2 CalcSpoolCoord(const glm::vec2 &coord,
                                const glm::vec2 &center_coord,
                                float focal_distance) {
    auto relative_coords = coord - center_coord;
    auto distance = glm::length(relative_coords);

    return relative_coords / distance * focal_distance *
           glm::atan(distance / focal_distance) + center_coord;
}

inline glm::vec2 CalcBarrelCoord(const glm::vec2 &coord,
                                 const glm::vec2 &center_coord,
                                 float focal_distance) {
    auto relative_coords = coord - center_coord;
    auto distance = glm::length(relative_coords);

    return relative_coords / distance * focal_distance *
           glm::tan(glm::clamp(distance / focal_distance,
                               static_cast<float>(-M_PI_2),
                               static_cast<float>(M_PI_2))) +
           center_coord;
}

inline glm::vec2 LinearInterpolation2D(const glm::vec2 &a, const glm::vec2 &b, float alpha) {
    return a + (b - a) * alpha;
}

inline glm::vec2 CalcAASampleCoords(const glm::vec2 &coords_lt, const glm::vec2 &coords_rt,
                                    const glm::vec2 &coords_lb, const glm::vec2 &coords_rb,
                                    const glm::vec2 &alpha) {
    glm::vec2 interpolated_top    = LinearInterpolation2D(coords_lt, coords_rt, alpha.x);
    glm::vec2 interpolated_bottom = LinearInterpolation2D(coords_lb, coords_rb, alpha.x);
    return LinearInterpolation2D(interpolated_top, interpolated_bottom, alpha.y);
}

#endif // _OPTICSCOMPENSATION_S_SRC_CPU_KERNEL_H_
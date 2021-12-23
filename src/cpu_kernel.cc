#include "cpu_kernel.h"
#include "debug_helper.h"
#define SAMPLE_NUM 2

void PremultKernel(const cv::Mat &in_image, const cv::Mat &out_image) {
    auto w = in_image.cols;
    auto h = in_image.rows;
    aut::Size2D image_size(w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            auto in_pixel = SamplingPixel<uchar>(in_image, x, y, image_size);
            auto out_pixel = reinterpret_cast<cv::Vec4f*>(out_image.data) + y * w + x;
            float alpha = in_pixel[3];
            (*out_pixel)[0] = in_pixel[0] * alpha;
            (*out_pixel)[1] = in_pixel[1] * alpha;
            (*out_pixel)[2] = in_pixel[2] * alpha;
            (*out_pixel)[3] = alpha;
        }
    }
}

void UnpremultKernel(const cv::Mat &in_image, const cv::Mat &out_image) {
    auto w = in_image.cols;
    auto h = in_image.rows;
    aut::Size2D image_size(w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            auto in_pixel = SamplingPixel<float>(in_image, x, y, image_size);
            auto out_pixel = reinterpret_cast<cv::Vec4b*>(out_image.data) + y * w + x;
            float alpha = in_pixel[3];
            if (alpha != 0) {
                (*out_pixel)[0] = static_cast<uchar>(in_pixel[0] / alpha);
                (*out_pixel)[1] = static_cast<uchar>(in_pixel[1] / alpha);
                (*out_pixel)[2] = static_cast<uchar>(in_pixel[2] / alpha);
                (*out_pixel)[3] = static_cast<uchar>(alpha);
            } else {
                (*out_pixel)[0] = 0;
                (*out_pixel)[1] = 0;
                (*out_pixel)[2] = 0;
                (*out_pixel)[3] = 0;
            }
        }
    }
}

void SpoolCPUKernel(const cv::Mat &in_image, const cv::Mat &out_image,
                    const aut::Size2D &image_size,
                    OpticsCompensationParameter parameter) {
    glm::vec2 center_coord(
        (image_size.w - 1) / 2.f - parameter.center_pos.x,
        (image_size.h - 1) / 2.f - parameter.center_pos.y
    );
    #pragma omp parallel for
    for (int y = 0; y < image_size.h; y++) {
        #pragma omp parallel for
        for (int x = 0; x < image_size.w; x++) {
            glm::vec2 coord(x, y);
            glm::vec2 sampling_coord = CalcSpoolCoord(coord, center_coord,
                                                       parameter.CalcFocalDistance());

            auto pixel = SamplingPixel<float>(in_image, sampling_coord.x, sampling_coord.y,
                                              image_size);

            auto out_pixel = reinterpret_cast<cv::Vec4f*>(out_image.data) + y * image_size.w + x;
            (*out_pixel) = pixel;
        }
    }
}

void BarrelCPUKernel(const cv::Mat &in_image, const cv::Mat &out_image,
                     const aut::Size2D &image_size,
                     OpticsCompensationParameter parameter) {
    if (parameter.amount == 1)
        return;

    aut::DebugPrint("AA : ", parameter.anti_aliasing);

    glm::vec2 center_coord(
        (image_size.w - 1) / 2.f - parameter.center_pos.x,
        (image_size.h - 1) / 2.f - parameter.center_pos.y
    );
    auto focal_distance = parameter.CalcFocalDistance();
    #pragma omp parallel for
    for (int y = 0; y < image_size.h; y++) {
        #pragma omp parallel for
        for (int x = 0; x < image_size.w; x++) {
            glm::vec2 coord(x, y);
            auto out_pixel = reinterpret_cast<cv::Vec4f*>(out_image.data) + y * image_size.w + x;
            if (parameter.anti_aliasing) {
                auto coord_lt = CalcBarrelCoord(glm::vec2(coord.x - 0.5f, coord.y - 0.5f),
                                                center_coord, focal_distance);
                auto coord_rt = CalcBarrelCoord(glm::vec2(coord.x + 0.5f, coord.y - 0.5f),
                                                center_coord, focal_distance);
                auto coord_lb = CalcBarrelCoord(glm::vec2(coord.x - 0.5f, coord.y + 0.5f),
                                                center_coord, focal_distance);
                auto coord_rb = CalcBarrelCoord(glm::vec2(coord.x + 0.5f, coord.y + 0.5f),
                                                center_coord, focal_distance);

                cv::Vec4f pixel(cv::Scalar::all(0));
                int sampled_num = 0;
                for (float sy = 1.f / SAMPLE_NUM / 2; sy < 1; sy += (1.f / SAMPLE_NUM)) {
                    for (float sx = 1.f / SAMPLE_NUM / 2; sx < 1; sx += (1.f / SAMPLE_NUM)) {
                        glm::vec2 alpha(sx, sy);
                        auto sampling_coord = CalcAASampleCoords(coord_lt, coord_rt,
                                                               coord_lb, coord_rb,
                                                               alpha);
                        auto sampled_pixel =
                            SamplingPixel<float>(in_image, sampling_coord.x, sampling_coord.y,
                                                 image_size);
                        pixel += sampled_pixel;
                        sampled_num++;
                    }
                }
                pixel /= sampled_num;
                (*out_pixel) = pixel;
            } else {
                glm::vec2 sampling_coord = CalcBarrelCoord(coord, center_coord,
                                                           parameter.CalcFocalDistance());

                auto pixel = SamplingPixel<float>(in_image, sampling_coord.x, sampling_coord.y,
                                                  image_size);

                (*out_pixel) = pixel;
            }
        }
    }
}
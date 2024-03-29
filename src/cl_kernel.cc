#include "cl_kernel.h"
#include <cmath>

SpoolKernelManager::SpoolKernelManager(const cl::Program *program, cl::CommandQueue *command_queue) :
    CLKernelManager(program, "Spool") {
    SetCommandQueue(command_queue);
}

void SpoolKernelManager::CallKernel(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h,
                                    OpticsCompensationParameter parameter) {
    cl_int2 image_size = {w, h};
    cl_float2 center_coords = {
        (w - 1) / 2.0f + parameter.center_pos.x,
        (h - 1) / 2.0f + parameter.center_pos.y
    };
    int block_size = 16;
    kernel_->setArg(0, in_image);
    kernel_->setArg(1, out_image);
    kernel_->setArg(2, image_size);
    kernel_->setArg(3, center_coords);
    kernel_->setArg(4, parameter.CalcFocalDistance());
    cl::NDRange global(
        static_cast<std::size_t>(std::ceil(w / static_cast<float>(block_size))) * block_size,
        static_cast<std::size_t>(std::ceil(h / static_cast<float>(block_size))) * block_size,
        1
    );
    cl::NDRange local(block_size, block_size, 1);
    command_queue_->enqueueNDRangeKernel(*kernel_, cl::NDRange(0, 0, 0), global, local);
}

BarrelKernelManager::BarrelKernelManager(const cl::Program *program, cl::CommandQueue *command_queue) :
    CLKernelManager(program, "Barrel") {
    SetCommandQueue(command_queue);
}

void BarrelKernelManager::CallKernel(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h,
                                    OpticsCompensationParameter parameter) {
    cl_int2 image_size = {w, h};
    cl_float2 center_coords = {
        (w - 1) / 2.0f + parameter.center_pos.x,
        (h - 1) / 2.0f + parameter.center_pos.y
    };
    int block_size = 16;
    kernel_->setArg(0, in_image);
    kernel_->setArg(1, out_image);
    kernel_->setArg(2, image_size);
    kernel_->setArg(3, center_coords);
    kernel_->setArg(4, parameter.CalcFocalDistance());
    cl::NDRange global(
        static_cast<std::size_t>(std::ceil(w / static_cast<float>(block_size))) * block_size,
        static_cast<std::size_t>(std::ceil(h / static_cast<float>(block_size))) * block_size,
        1
    );
    cl::NDRange local(block_size, block_size, 1);
    command_queue_->enqueueNDRangeKernel(*kernel_, cl::NDRange(0, 0, 0), global, local);
}

MSBarrelKernelManager::MSBarrelKernelManager(const cl::Program *program, cl::CommandQueue *command_queue) :
    CLKernelManager(program, "MultiSamplingBarrel") {
    SetCommandQueue(command_queue);
}

void MSBarrelKernelManager::CallKernel(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h,
                                    OpticsCompensationParameter parameter) {
    cl_int2 image_size = {w, h};
    cl_float2 center_coords = {
        (w - 1) / 2.0f + parameter.center_pos.x,
        (h - 1) / 2.0f + parameter.center_pos.y
    };
    int block_size = 16;
    kernel_->setArg(0, in_image);
    kernel_->setArg(1, out_image);
    kernel_->setArg(2, image_size);
    kernel_->setArg(3, center_coords);
    kernel_->setArg(4, parameter.CalcFocalDistance());
    kernel_->setArg(5, 4);
    cl::NDRange global(
        static_cast<std::size_t>(std::ceil(w / static_cast<float>(block_size))) * block_size,
        static_cast<std::size_t>(std::ceil(h / static_cast<float>(block_size))) * block_size,
        1
    );
    cl::NDRange local(block_size, block_size, 1);
    command_queue_->enqueueNDRangeKernel(*kernel_, cl::NDRange(0, 0, 0), global, local);
}

PremultKernelManager::PremultKernelManager(const::cl::Program *program, cl::CommandQueue *command_queue) :
    CLKernelManager(program, "Premult") {
    SetCommandQueue(command_queue);
}

void PremultKernelManager::CallPremult(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h) {
    cl_int2 image_size = {w, h};

    kernel_->setArg(0, in_image);
    kernel_->setArg(1, out_image);
    kernel_->setArg(2, image_size);

    int block_size = 16;
    cl::NDRange global(
        static_cast<std::size_t>(std::ceil(w / static_cast<float>(block_size))) * block_size,
        static_cast<std::size_t>(std::ceil(h / static_cast<float>(block_size))) * block_size,
        1
    );
    cl::NDRange local(block_size, block_size, 1);

    command_queue_->enqueueNDRangeKernel(*kernel_, cl::NDRange(0, 0, 0), global, local);
}

UnpremultKernelManager::UnpremultKernelManager(const::cl::Program *program, cl::CommandQueue *command_queue) :
    CLKernelManager(program, "Unpremult") {
    SetCommandQueue(command_queue);
}

void UnpremultKernelManager::CallUnpremult(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h) {
    cl_int2 image_size = {w, h};

    kernel_->setArg(0, in_image);
    kernel_->setArg(1, out_image);
    kernel_->setArg(2, image_size);

    int block_size = 16;
    cl::NDRange global(
        static_cast<std::size_t>(std::ceil(w / static_cast<float>(block_size))) * block_size,
        static_cast<std::size_t>(std::ceil(h / static_cast<float>(block_size))) * block_size,
        1
    );
    cl::NDRange local(block_size, block_size, 1);

    command_queue_->enqueueNDRangeKernel(*kernel_, cl::NDRange(0, 0, 0), global, local);
}
#ifndef _OPTICSCOMPENSATION_S_SRC_CL_KERNEL_H_
#define _OPTICSCOMPENSATION_S_SRC_CL_KERNEL_H_

#include "cl_manager.h"
#include "parameter.h"

class SpoolKernelManager : public CLKernelManager {
public:
    SpoolKernelManager(const cl::Program *program, cl::CommandQueue *command_queue);

    void CallKernel(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h,
                    OpticsCompensationParameter parameter);
};

class BarrelKernelManager : public CLKernelManager {
public:
    BarrelKernelManager(const cl::Program *program, cl::CommandQueue *command_queue);

    void CallKernel(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h,
                    OpticsCompensationParameter parameter);
};

class MSBarrelKernelManager : public CLKernelManager {
public:
    MSBarrelKernelManager(const cl::Program *program, cl::CommandQueue *command_queue);

    void CallKernel(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h,
                    OpticsCompensationParameter parameter);
};

class PremultKernelManager : public CLKernelManager {
public:
    PremultKernelManager(const cl::Program *program, cl::CommandQueue *command_queue);

    void CallPremult(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h);
};

class UnpremultKernelManager : public CLKernelManager {
public:
    UnpremultKernelManager(const cl::Program *program, cl::CommandQueue *command_queue);

    void CallUnpremult(cl::Image2D &in_image, cl::Image2D &out_image, int w, int h);
};

#endif // _OPTICSCOMPENSATION_S_SRC_CL_KERNEL_H_
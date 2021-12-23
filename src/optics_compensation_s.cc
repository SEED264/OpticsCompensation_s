#include <string>
#include <aut/AUL_Utils.h>
#include <CL/cl.hpp>
#include <lua.hpp>
#include <opencv2/opencv.hpp>
#include "cl_manager.h"
#include "cl_kernel.h"
#include "cpu_kernel.h"
#include "exception.h"
#include "out_debug.h"
#include "parameter.h"
#include "stopwatch.h"

#define CL_KERNEL_SOURCE(x) #x
static const std::string kernel_source =
#include "kernel.cl"
;

static OpenCLManager *opencl_manager = nullptr;
static SpoolKernelManager *spool_kernel_manager = nullptr;
static BarrelKernelManager *barrel_kernel_manager = nullptr;
static MSBarrelKernelManager *ms_barrel_kernel_manager = nullptr;
static PremultKernelManager *premult_kernel_manager = nullptr;
static UnpremultKernelManager *unpremult_kernel_manager = nullptr;

bool first_time = true;
bool use_opencl = true;

int OpticsCompensation(lua_State *L) {
    StopWatch sw(true);
    OpticsCompensationParameter parameter;
    int arg_num = static_cast<int>(lua_gettop(L));
    parameter.amount = static_cast<float>(lua_tonumber(L, 1) / 100);
    if (arg_num >=2)
        parameter.anti_aliasing = static_cast<bool>(lua_tointeger(L, 2));

    if (arg_num >= 4) {
        parameter.center_pos.x = static_cast<float>(lua_tonumber(L, 3));
        parameter.center_pos.y = static_cast<float>(lua_tonumber(L, 4));
    }

    if (!parameter.amount)
        return 0;
    if (parameter.amount < 0) {
        parameter.spool_mode = true;
        parameter.amount *= -1;
    }

    aut::PixelRGBA *image_data;
    aut::Size2D image_size;
    aut::getpixeldata(L, &image_data, &image_size);

    if (first_time) {
        try {
            OutDebugInfo("Init OpenCL");
            if (!opencl_manager)
                opencl_manager = new OpenCLManager(kernel_source);
            cl::Context *context = opencl_manager->GetContext();
            CLCommandQueueManager *cqman = opencl_manager->GetCommandQueueManager();

            spool_kernel_manager = new SpoolKernelManager(opencl_manager->GetProgram(), cqman->GetCommandQueue());
            barrel_kernel_manager = new BarrelKernelManager(opencl_manager->GetProgram(), cqman->GetCommandQueue());
            ms_barrel_kernel_manager = new MSBarrelKernelManager(opencl_manager->GetProgram(), cqman->GetCommandQueue());
            premult_kernel_manager = new PremultKernelManager(opencl_manager->GetProgram(), cqman->GetCommandQueue());
            unpremult_kernel_manager = new UnpremultKernelManager(opencl_manager->GetProgram(), cqman->GetCommandQueue());

            CLDeviceManager *dman = opencl_manager->GetDeviceManager();
            for (unsigned int i = 0; i < dman->GetDeviceNum(); i++) {
                aut::DebugPrint("Device ", i, " : ", dman->GetDeviceName(i));
            }
            OutDebugInfo("Build Log : ",
                         opencl_manager->GetProgramManager()->GetBuildLog());
            use_opencl = true;
            OutDebugInfo("Init OpenCL complete");
        } catch (InitOpenCLManagerException &e) {
            aut::DebugPrint(e.message());
            use_opencl = false;
        }
        first_time = false;
    }

    if (use_opencl) {
        auto *context = opencl_manager->GetContext();
        auto *command_queue_manager = opencl_manager->GetCommandQueueManager();
        cl::ImageFormat fmt;
        fmt.image_channel_data_type = CL_UNORM_INT8;
        fmt.image_channel_order = CL_BGRA;
        cl::Image2D image_0(*context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, fmt,
                            image_size.w, image_size.h, 0, image_data);
        cl::Image2D image_1(*context, CL_MEM_READ_WRITE, fmt,
                            image_size.w, image_size.h);

        premult_kernel_manager->CallPremult(image_0, image_1, image_size.w, image_size.h);

        if (parameter.spool_mode) {
            spool_kernel_manager->CallKernel(
                image_1, image_0, image_size.w, image_size.h, parameter);
        } else {
            if (parameter.amount != 1.0) {
                if (parameter.anti_aliasing) {
                    ms_barrel_kernel_manager->CallKernel(
                        image_1, image_0, image_size.w, image_size.h, parameter);
                } else {
                    barrel_kernel_manager->CallKernel(
                        image_1, image_0, image_size.w, image_size.h, parameter);
                }
            } else {
                std::vector<uchar> empty_image(image_size.w * image_size.h * 4, 0);
                command_queue_manager->WriteImage2D(image_0, true, 0, 0,
                                                    image_size.w, image_size.h, empty_image.data());            }
        }

        unpremult_kernel_manager->CallUnpremult(image_0, image_1, image_size.w, image_size.h);

        command_queue_manager->ReadImage2D(image_1, true, 0, 0,
                                           image_size.w, image_size.h, image_data);

        aut::putpixeldata(L, image_data);
    } else {
        cv::Size mat_size(image_size.w, image_size.h);
        cv::Mat image_inout(mat_size, CV_8UC4, image_data);
        cv::Mat image_0(mat_size, CV_32FC4, cv::Scalar::all(0));
        cv::Mat image_1(mat_size, CV_32FC4, cv::Scalar::all(0));

        PremultKernel(image_inout, image_0);

        if (parameter.spool_mode) {
            SpoolCPUKernel(image_0, image_1, image_size, parameter);
        } else {
            BarrelCPUKernel(image_0, image_1, image_size, parameter);
        }

        UnpremultKernel(image_1, image_inout);

        aut::putpixeldata(L, reinterpret_cast<aut::PixelRGBA*>(image_inout.data));
    }

    aut::DebugPrint("Total Time : ", sw.Stop(), " ms");

    return 0;
}

// Lua側に登録する関数
static luaL_Reg optics_compensation[] = {
{"OpticsCompensation", OpticsCompensation},
{nullptr, nullptr}
};

// Lua側にモジュールを登録
extern "C" {
__declspec(dllexport) int luaopen_OpticsCompensation_s(lua_State *L) {
    luaL_register(L, "OpticsCompensation_s", optics_compensation);
    return 1;
}
}
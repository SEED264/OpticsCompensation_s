#include <string>
#include <aut/AUL_Utils.h>
#include <CL/cl.hpp>
#include <lua.hpp>
#include "cl_manager.h"
#include "cl_kernel.h"
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

bool first_time = true;

int OpticsCompensation(lua_State *L) {
    StopWatch sw(true);
    OpticsCompensationParameter parameter;
    int arg_num = static_cast<int>(lua_gettop(L));
    parameter.amount = static_cast<float>(lua_tonumber(L, 1) / 100);
    if (arg_num >=2)
        parameter.anti_aliasing = static_cast<bool>(lua_tointeger(L, 2));

    if (!parameter.amount)
        return 0;
    if (parameter.amount < 0) {
        parameter.spool_mode = true;
        parameter.amount *= -1;
    }

    aut::PixelRGBA *image_data;
    aut::Size2D image_size;
    aut::getpixeldata(L, &image_data, &image_size);

    if (!opencl_manager)
        opencl_manager = new OpenCLManager(kernel_source);
    cl::Context *con = opencl_manager->GetContext();
    CLCommandQueueManager *cqman = opencl_manager->GetCommandQueueManager();

    if (!spool_kernel_manager)
        spool_kernel_manager = new SpoolKernelManager(opencl_manager->GetProgram(), cqman->GetCommandQueue());
    if (!barrel_kernel_manager)
        barrel_kernel_manager = new BarrelKernelManager(opencl_manager->GetProgram(), cqman->GetCommandQueue());
    if (!ms_barrel_kernel_manager)
        ms_barrel_kernel_manager = new MSBarrelKernelManager(opencl_manager->GetProgram(), cqman->GetCommandQueue());

    if (first_time) {
        CLDeviceManager *dman = opencl_manager->GetDeviceManager();
        for (unsigned int i = 0; i < dman->GetDeviceNum(); i++) {
            aut::DebugPrint("Device ", i, " : ", dman->GetDeviceName(i));
        }
        aut::DebugPrint("Build Log : ",
                        opencl_manager->GetProgramManager()->GetBuildLog());
        first_time = false;
    }

    cl::ImageFormat fmt;
    fmt.image_channel_data_type = CL_UNORM_INT8;
    fmt.image_channel_order = CL_BGRA;
    cl::Image2D image(*con, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, fmt,
                      image_size.w, image_size.h, 0, image_data);

    cl::Image2D out_image(*con, CL_MEM_READ_WRITE, fmt,
                          image_size.w, image_size.h);
    if (parameter.spool_mode) {
        spool_kernel_manager->CallKernel(
            image, out_image, image_size.w, image_size.h, parameter);
    } else {
        if (parameter.anti_aliasing) {
            ms_barrel_kernel_manager->CallKernel(
                image, out_image, image_size.w, image_size.h, parameter);
        } else {
            barrel_kernel_manager->CallKernel(
                image, out_image, image_size.w, image_size.h, parameter);
        }
    }

    cqman->ReadImage2D(out_image, true, 0, 0, image_size.w, image_size.h, image_data);

    aut::putpixeldata(L, image_data);

    aut::DebugPrint("  Total Time : ", sw.Stop(), " ms");

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
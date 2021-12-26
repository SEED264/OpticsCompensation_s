#ifndef _OPTICSCOMPENSATION_S_SRC_CL_MANAGER_H_
#define _OPTICSCOMPENSATION_S_SRC_CL_MANAGER_H_

#include <stdexcept>
#include <string>
#include <vector>
#include <CL/cl.hpp>
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <windows.h>
#include <delayimp.h>

class CLPlatformManager {
public:
    CLPlatformManager();
    std::vector<cl::Platform>* GetPlatforms();

    std::vector<cl::Platform>* GetPlatformList() { return &platforms_; }
    cl::Platform* GetSelectedPlatform() { return &platforms_[selected_index_]; }

    cl::Platform* SelectPlatform(int platform_index);

    std::size_t GetPlatformNum() const { return platforms_.size(); }
    std::string GetPlatformName(int platform_index);
    std::string GetVendorName(int platform_index);

private:
    int selected_index_;
    std::vector<cl::Platform> platforms_;
};

class CLDeviceManager {
public:
    CLDeviceManager(cl_device_type device_type, cl::Platform *platform = nullptr);
    CLDeviceManager(const cl::Context *context);
    std::vector<cl::Device>* GetDevices(const cl::Platform *platform);
    std::vector<cl::Device>* GetDevices(const cl::Context *context);

    std::vector<cl::Device>* GetDeviceList() { return &devices_; }
    cl::Device* GetSelectedDevice() { return &devices_[selected_index_]; }

    void SetDeviceType(cl_device_type device_type);

    cl::Device* SelectDevice(int device_index);

    std::size_t GetDeviceNum() const { return devices_.size(); }
    std::string GetDeviceName(int device_index);
    std::string GetDeviceVendor(int device_index);

private:
    int selected_index_;
    cl_device_type device_type_;
    cl::Platform *platform_;
    std::vector<cl::Device> devices_;
};

class CLContextManager {
public:
    CLContextManager(const cl::Device *device = nullptr);
    CLContextManager(cl_device_type device_type);

    cl::Context* CreateContextFromDevice(const cl::Device *device);
    cl::Context* CreateContextFromDeviceType(cl_device_type device_type);

    cl::Context* GetContext() const { return context_; }

private:
    cl::Context *context_;
};

class CLCommandQueueManager {
public:
    CLCommandQueueManager(const cl::Context *context);

    cl::CommandQueue* CreateCommandQueue(const cl::Context *context);

    cl::CommandQueue* GetCommandQueue() const { return command_queue_; }

    void ReadBuffer(const cl::Buffer &buffer, cl_bool blocking,
                    std::size_t offset, std::size_t size, void *ptr);
    void WriteBuffer(const cl::Buffer &buffer, cl_bool blocking,
                     std::size_t offset, std::size_t size, void *ptr);
    void CopyBuffer(const cl::Buffer &src, const cl::Buffer &dst,
                    std::size_t src_offset, std::size_t dst_offset, std::size_t size);

    void ReadImage2D(const cl::Image2D &image, cl_bool blocking,
                     std::size_t origin_x, std::size_t origin_y,
                     std::size_t region_x, std::size_t region_y,
                     void *ptr);
    void WriteImage2D(const cl::Image2D &image, cl_bool blocking,
                     std::size_t origin_x, std::size_t origin_y,
                     std::size_t region_x, std::size_t region_y,
                     void *ptr);
    void FillImage2D(const cl::Image2D &image, cl_float4 fill_color,
                     std::size_t origin_x, std::size_t origin_y,
                     std::size_t region_x, std::size_t region_y);

private:
    cl::CommandQueue *command_queue_;
};

class CLProgramManager {
public:
    CLProgramManager(const cl::Context *context, const std::string &source, bool build = false);

    cl::Program* GetProgram() const { return program_; }
    std::string GetBuildLog();

    void Build();

private:
    cl::Program *program_;
};

class CLKernelManager {
public:
    CLKernelManager(const cl::Program *program, const std::string kernel_name);

    void SetCommandQueue(cl::CommandQueue *command_queue);

    cl::Kernel* GetKernel() const { return kernel_; }

protected:
    std::string kernel_name_;
    cl::Kernel *kernel_;
    cl::CommandQueue *command_queue_;
};

class OpenCLManager {
public:
    OpenCLManager(const std::string &kernel_source);

    cl::Platform* GetPlatform() { return platform_manager_->GetSelectedPlatform(); }
    cl::Device* GetDevice() { return device_manager_->GetSelectedDevice(); }
    cl::Context* GetContext() { return context_manager_->GetContext(); }
    cl::CommandQueue* GetCommandQueue() {return command_queue_manager_->GetCommandQueue(); }
    cl::Program* GetProgram() { return program_manager_->GetProgram(); }

    CLPlatformManager* GetPlatformManager();
    CLDeviceManager* GetDeviceManager();
    CLContextManager* GetContextManager();
    CLCommandQueueManager* GetCommandQueueManager();
    CLProgramManager* GetProgramManager();

private:
    CLPlatformManager *platform_manager_;
    CLDeviceManager *device_manager_;
    CLContextManager *context_manager_;
    CLCommandQueueManager *command_queue_manager_;
    CLProgramManager *program_manager_;
};

inline void LoadOpenCLDLL() {
    bool success = false;
    try {
    HRESULT result = __HrLoadAllImportsForDll("OpenCL.dll");
        success = SUCCEEDED(result);
    } catch (...) {
        success = false;
    }

    // Throw exception if failed to load OpenCL.dll
    if (!success) {
        throw std::runtime_error("Failed to load OpenCL.dll");
    }
}

#endif // _OPTICSCOMPENSATION_S_SRC_CL_MANAGER_H_
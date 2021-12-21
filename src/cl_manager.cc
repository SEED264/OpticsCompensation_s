#include "cl_manager.h"
#include "debug_helper.h"
#include "exception.h"

/* CLPlatformManager */

CLPlatformManager::CLPlatformManager() :
    selected_index_(0) {
    GetPlatforms();
}

std::vector<cl::Platform>* CLPlatformManager::GetPlatforms() {
    cl::Platform::get(&platforms_);
    return &platforms_;
}

cl::Platform* CLPlatformManager::SelectPlatform(int platform_index) {
    selected_index_ = platform_index;
    return GetSelectedPlatform();
}

std::string CLPlatformManager::GetPlatformName(int platform_index) {
    return platforms_[platform_index].getInfo<CL_PLATFORM_NAME>();
}

std::string CLPlatformManager::GetVendorName(int platform_index) {
    return platforms_[platform_index].getInfo<CL_PLATFORM_VENDOR>();
}

/* CLDeviceManager */

CLDeviceManager::CLDeviceManager(cl_device_type device_type, cl::Platform *platform) :
    selected_index_(0),
    device_type_(device_type),
    platform_(platform) {
    SetDeviceType(device_type_);
}

CLDeviceManager::CLDeviceManager(const cl::Context *context) :
    selected_index_(0),
    platform_(nullptr) {
    GetDevices(context);
    if (devices_.size() != 0)
        device_type_ = devices_[0].getInfo<CL_DEVICE_TYPE>();
}

std::vector<cl::Device>* CLDeviceManager::GetDevices(const cl::Platform *platform) {
    cl_int err;
    if (platform)
        err = platform->getDevices(device_type_, &devices_);
    CheckCLErrorCode("Init device", err);
    return &devices_;
}

std::vector<cl::Device>* CLDeviceManager::GetDevices(const cl::Context *context) {
    cl_int err;
    if (context)
        devices_ = context->getInfo<CL_CONTEXT_DEVICES>(&err);
    CheckCLErrorCode("Init device", err);
    return &devices_;
}
void CLDeviceManager::SetDeviceType(cl_device_type device_type) {
    device_type_ = device_type;
    GetDevices(platform_);
}

cl::Device* CLDeviceManager::SelectDevice(int device_index){
    selected_index_ = device_index;
    return GetSelectedDevice();
}

std::string CLDeviceManager::GetDeviceName(int device_index) {
    return devices_[device_index].getInfo<CL_DEVICE_NAME>();
}

std::string CLDeviceManager::GetDeviceVendor(int device_index) {
    return devices_[device_index].getInfo<CL_DEVICE_VENDOR>();
}

/* CLContextManager */

CLContextManager::CLContextManager(const cl::Device *device) {
    if (device)
        context_ = CreateContextFromDevice(device);
}

CLContextManager::CLContextManager(cl_device_type device_type) {
    context_ = CreateContextFromDeviceType(device_type);
}

cl::Context* CLContextManager::CreateContextFromDevice(const cl::Device *device) {
    std::vector<cl::Device> devices = {*device};
    cl_int err;
    context_ = new cl::Context(devices, nullptr, nullptr, nullptr, &err);
    CheckCLErrorCode("Init context", err);
    return context_;
}

cl::Context* CLContextManager::CreateContextFromDeviceType(cl_device_type device_type) {
    cl_int err;
    context_ = new cl::Context(device_type, nullptr, nullptr, nullptr, &err);
    CheckCLErrorCode("Init context", err);
    return context_;
}

/* CLCommandQueueManager */

CLCommandQueueManager::CLCommandQueueManager(const cl::Context *context) :
    command_queue_(nullptr) {
    CreateCommandQueue(context);
}

cl::CommandQueue* CLCommandQueueManager::CreateCommandQueue(const cl::Context *context) {
    auto device = context->getInfo<CL_CONTEXT_DEVICES>()[0];
    cl_int err;
    command_queue_ = new cl::CommandQueue(*context, device, 0, &err);
    CheckCLErrorCode("Init command queue", err);
    return command_queue_;
}

void CLCommandQueueManager::ReadBuffer(const cl::Buffer &buffer, cl_bool blocking,
                                       std::size_t offset, std::size_t size, void *ptr) {
    command_queue_->enqueueReadBuffer(buffer, blocking, offset, size, ptr);
}
void CLCommandQueueManager::WriteBuffer(const cl::Buffer &buffer, cl_bool blocking,
                                        std::size_t offset, std::size_t size, void *ptr) {
    command_queue_->enqueueWriteBuffer(buffer, blocking, offset, size, ptr);
}

void CLCommandQueueManager::CopyBuffer(const cl::Buffer &src, const cl::Buffer &dst,
                                       std::size_t src_offset, std::size_t dst_offset, std::size_t size) {
    command_queue_->enqueueCopyBuffer(src, dst, src_offset, dst_offset, size);
}

void CLCommandQueueManager::ReadImage2D(const cl::Image2D &image, cl_bool blocking,
                     std::size_t origin_x, std::size_t origin_y,
                     std::size_t region_x, std::size_t region_y,
                     void *ptr) {
    cl::size_t<3> origin;
    origin[0] = origin_x;
    origin[1] = origin_y;
    origin[2] = 0;
    cl::size_t<3> region;
    region[0] = region_x;
    region[1] = region_y;
    region[2] = 1;

    cl_int err = command_queue_->enqueueReadImage(image, blocking, origin, region, 0, 0, ptr);
}
void CLCommandQueueManager::WriteImage2D(const cl::Image2D &image, cl_bool blocking,
                     std::size_t origin_x, std::size_t origin_y,
                     std::size_t region_x, std::size_t region_y,
                     void *ptr) {
    cl::size_t<3> origin;
    origin[0] = origin_x;
    origin[1] = origin_y;
    origin[2] = 0;
    cl::size_t<3> region;
    region[0] = region_x;
    region[1] = region_y;
    region[2] = 1;

    command_queue_->enqueueWriteImage(image, blocking, origin, region, 0, 0, ptr);
}

void CLCommandQueueManager::FillImage2D(const cl::Image2D &image, cl_float4 fill_color,
                                        std::size_t origin_x, std::size_t origin_y,
                                        std::size_t region_x, std::size_t region_y) {
    cl::size_t<3> origin;
    origin[0] = 0;
    origin[1] = 0;
    origin[2] = 0;
    cl::size_t<3> region;
    region[0] = region_x;
    region[1] = region_y;
    region[2] = 1;

    command_queue_->enqueueFillImage(image, fill_color, origin, region);
}

/* CLProgramManager */

CLProgramManager::CLProgramManager(const cl::Context *context, const std::string &source, bool build) {
    cl_int err;
    program_ = new cl::Program(*context, source, build, &err);
    CheckCLErrorCode("Init program", err);
}

std::string CLProgramManager::GetBuildLog() {
    auto device = program_->getInfo<CL_PROGRAM_DEVICES>()[0];
    return program_->getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
}

void CLProgramManager::Build() {
    program_->build();
}

/* CLKernelManager */

CLKernelManager::CLKernelManager(const cl::Program *program, const std::string kernel_name) :
    kernel_name_(kernel_name) {
    kernel_ = new cl::Kernel(*program, kernel_name_.c_str());
}

void CLKernelManager::SetCommandQueue(cl::CommandQueue *command_queue) {
    command_queue_ = command_queue;
}

/* OpenCLManager */

OpenCLManager::OpenCLManager(const std::string &kernel_source) {
    OutDebugInfo("Init context manager");
    context_manager_ = new CLContextManager(CL_DEVICE_TYPE_GPU);
    cl::Context *context = context_manager_->GetContext();

    // Print context info when built in debug config
    DebugPrintContextInfo(*context);

    device_manager_ = new CLDeviceManager(context_manager_->GetContext());
    program_manager_ = new CLProgramManager(context, kernel_source, true);
    command_queue_manager_ = new CLCommandQueueManager(context);
}

CLPlatformManager* OpenCLManager::GetPlatformManager() {
    return platform_manager_;
}

CLDeviceManager* OpenCLManager::GetDeviceManager() {
    return device_manager_;
}

CLContextManager* OpenCLManager::GetContextManager() {
    return context_manager_;
}

CLCommandQueueManager* OpenCLManager::GetCommandQueueManager() {
    return command_queue_manager_;
}

CLProgramManager* OpenCLManager::GetProgramManager() {
    return program_manager_;
}
#ifndef _OPTICSCOMPENSATION_S_SRC_DEBUG_HELPER_H
#define _OPTICSCOMPENSATION_S_SRC_DEBUG_HELPER_H

#include <vector>
#include "out_debug.h"
#include <CL/cl.hpp>

#ifndef _DEBUG
#define RETURN_IN_RELEASE return;
#else
#define RETURN_IN_RELEASE
#endif

void DebugPrintContextInfo(cl::Context context) {
    RETURN_IN_RELEASE
    // Get count of devices in context
    auto device_count = context.getInfo<CL_CONTEXT_NUM_DEVICES>();

    // Return if device not found
    if (device_count == 0) {
        OutDebugInfo("GPU not found.");
        return;
    }

    // Get devices in context
    auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
    // Print devices info
    for (size_t i = 0; i < device_count; i++) {
        auto &device = devices[i];
        // Get device and vendor's name
        auto device_name = device.getInfo<CL_DEVICE_NAME>();
        auto device_vendor_name = device.getInfo<CL_DEVICE_VENDOR>();
        // Print the retrieved information
        OutDebugInfo(std::string("Device ") + std::to_string(i) +
                    "    Name : " + device_name +
                    "    Vendor : " + device_vendor_name);
    }
}

#endif // _OPTICSCOMPENSATION_S_SRC_DEBUG_HELPER_H
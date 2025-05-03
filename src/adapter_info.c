/* Copyright (c) 2025 h5law <dev@h5law.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <glfw/glfw3.h>

#include "utils.h"

int main(int argc, char *argv[])
{
    WGPUDevice device = create_general_device();
    if (device == NULL) {
        fprintf(stderr, "Unable to get general device info\n");
        return EXIT_FAILURE;
    }

    WGPUAdapterInfo adapterInfo = WGPU_ADAPTER_INFO_INIT;
    WGPUStatus      infoStatus = wgpuDeviceGetAdapterInfo(device, &adapterInfo);
    if (infoStatus != WGPUStatus_Success) {
        fprintf(stderr, "Unable to get adapter info\n");
        return EXIT_FAILURE;
    }

    GLFWwindow *window = get_glfw_window();

    // WGPUSurface surface = get_surface();

    // if ((( WGPUDeviceDescriptor * )adapterInfo.nextInChain) != NULL) {
    //     char dbuf[(( WGPUDeviceDescriptor * )adapterInfo.nextInChain)
    //                       ->label.length +
    //               1];
    //     snprintf(dbuf,
    //              (( WGPUDeviceDescriptor * )adapterInfo.nextInChain)
    //                              ->label.length +
    //                      1,
    //              "%s",
    //              (( WGPUDeviceDescriptor * )adapterInfo.nextInChain)
    //                      ->label.data);
    //     printf("Device Descriptor Label: \t\t%s\n", dbuf);
    // }

    size_t      len = 0;
    const char *backendType =
            get_backend_type_from_code(adapterInfo.backendType, &len);
    printf("Backend Type: \t\t%s\n", backendType);
    len = 0;
    const char *adapterType =
            get_backend_type_from_code(adapterInfo.adapterType, &len);
    printf("Adapter Type: \t\t%s\n", adapterType);
    len = 0;
    printf("VendorID: \t\t0x%02x\n", adapterInfo.vendorID);
    char buf[adapterInfo.vendor.length + 1];
    snprintf(buf, adapterInfo.vendor.length + 1, "%s", adapterInfo.vendor.data);
    printf("Vendor: \t\t%s\n", buf);
    char buf2[adapterInfo.architecture.length + 1];
    snprintf(buf2, adapterInfo.architecture.length + 1, "%s",
             adapterInfo.architecture.data);
    printf("Architecture: \t\t%s\n", buf2);
    printf("DeviceID: \t\t0x%02x\n", adapterInfo.deviceID);
    char buf3[adapterInfo.device.length + 1];
    snprintf(buf3, adapterInfo.device.length + 1, "%s",
             adapterInfo.device.data);
    printf("Device: \t\t%s\n", buf3);
    char buf4[adapterInfo.description.length + 1];
    snprintf(buf4, adapterInfo.description.length + 1, "%s",
             adapterInfo.description.data);
    printf("Description: \t\t%s\n", buf4);

    WGPUSupportedFeatures features = WGPU_SUPPORTED_FEATURES_INIT;
    wgpuDeviceGetFeatures(device, &features);
    for (size_t i = 0; i < features.featureCount; ++i) {
        size_t      len = 0;
        const char *featName =
                get_feature_name_from_code(features.features[i], &len);
        printf("Device feature: \t%s(0x%02x)\n", featName,
               features.features[i]);
    }
    WGPUAdapter adapter = wgpuDeviceGetAdapter(device);
    features            = WGPU_SUPPORTED_FEATURES_INIT;
    wgpuAdapterGetFeatures(adapter, &features);
    for (size_t i = 0; i < features.featureCount; ++i) {
        size_t      len = 0;
        const char *featName =
                get_feature_name_from_code(features.features[i], &len);
        printf("Adapter feature: \t%s(0x%02x)\n", featName,
               features.features[i]);
    }

    while (!glfwWindowShouldClose(window)) {
        // Check whether the user clicked on the close button (and any other
        // mouse/key event, which we don't use so far)
        glfwPollEvents();
    }

    // wgpuSurfaceRelease(surface);
    wgpuDeviceRelease(device);
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

// vim: set ft=c ts=4 sts=4 sw=4 et cin ai nospell

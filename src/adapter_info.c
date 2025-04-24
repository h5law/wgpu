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

#include <_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dawn/webgpu.h>

#include "feature_names.h"

void callback(WGPURequestAdapterStatus status, WGPUAdapter adapter,
              struct WGPUStringView message, void *userdata1, void *userdata2)
{
    if (status != WGPURequestAdapterStatus_Success) {
        fprintf(stderr, "Request Adapter callback not successfull: %s\n",
                message.data);
        return;
    }
    memmove(userdata1, &adapter, sizeof(WGPUAdapter));
}

int main(int argc, char *argv[])
{
    WGPUInstanceDescriptor instanceDesc          = {0};
    instanceDesc.capabilities.timedWaitAnyEnable = true;
    WGPUInstance instance = wgpuCreateInstance(&instanceDesc);
    if (instance == NULL) {
        fprintf(stderr, "Unable to create WGPU instance\n");
        return EXIT_FAILURE;
    }

    WGPURequestAdapterOptions options = {0};
    WGPUAdapter               adapter;

    WGPURequestAdapterCallbackInfo callbackInfo = {0};
    callbackInfo.mode                           = WGPUCallbackMode_WaitAnyOnly;
    callbackInfo.userdata1                      = &adapter;
    callbackInfo.callback = ( WGPURequestAdapterCallback )callback;

    WGPUFuture future =
            wgpuInstanceRequestAdapter(instance, &options, callbackInfo);
    WGPUFutureWaitInfo futureInfo = {.future = future, .completed = false};
    WGPUWaitStatus     waitStatus =
            wgpuInstanceWaitAny(instance, 1, &futureInfo, UINT64_MAX);

    struct timespec ts;
    ts.tv_sec  = 0;
    ts.tv_nsec = 200 * 1000;
    while (!futureInfo.completed) {
        printf("Waiting for future to complete...\n");
        nanosleep(&ts, NULL);
    }

    if (waitStatus != WGPUWaitStatus_Success) {
        fprintf(stderr, "Unsuccessful instance wait any call\n");
        return EXIT_FAILURE;
    }
    if (callbackInfo.userdata1 == NULL) {
        fprintf(stderr, "Unable to get adapter from instance\n");
        return EXIT_FAILURE;
    }

    WGPUDawnAdapterPropertiesPowerPreference power_props =
            WGPU_DAWN_ADAPTER_PROPERTIES_POWER_PREFERENCE_INIT;
    WGPUAdapterInfo adapterInfo = {0};
    adapterInfo.nextInChain     = ( WGPUChainedStruct * )&power_props;
    WGPUStatus infoStatus       = wgpuAdapterGetInfo(adapter, &adapterInfo);
    if (infoStatus != WGPUStatus_Success) {
        fprintf(stderr, "Unable to get adapter info\n");
        return EXIT_FAILURE;
    }

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

    WGPUSupportedFeatures features = {0};
    wgpuAdapterGetFeatures(adapter, &features);
    for (size_t i = 0; i < features.featureCount; ++i) {
        printf("Supports feature: \t%s(0x%02x)\n",
               _FEATURE_NAME_FROM_CODE(features.features[i]),
               features.features[i]);
    }

    return EXIT_SUCCESS;
}

// vim: set ft=c ts=4 sts=4 sw=4 et cin ai nospell

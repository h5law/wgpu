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
#include <string.h>
#include <time.h>

#include <webgpu/webgpu.h>

#define WGPU_TARGET_MACOS         1
#define WGPU_TARGET_LINUX_X11     2
#define WGPU_TARGET_WINDOWS       3
#define WGPU_TARGET_LINUX_WAYLAND 4
#define WGPU_TARGET_EMSCRIPTEN    5

#ifdef __EMSCRIPTEN__
#define WGPU_TARGET WGPU_TARGET_EMSCRIPTEN
#endif
#ifdef _WIN32
#define WGPU_TARGET WGPU_TARGET_WINDOWS
#endif
#ifdef __APPLE__
#define WGPU_TARGET WGPU_TARGET_MACOS
#endif
#ifdef _GLFW_WAYLAND
#define WGPU_TARGET WGPU_TARGET_LINUX_WAYLAND
#endif
#ifdef _GLFW_X11
#define WGPU_TARGET WGPU_TARGET_LINUX_X11
#endif

#include <GLFW/glfw3.h>
#if WGPU_TARGET == WGPU_TARGET_MACOS
#define _GLFW_COCOA
#define GLFW_EXPOSE_NATIVE_COCOA
#elif WGPU_TARGET == WGPU_TARGET_LINUX_X11
#define GLFW_EXPOSE_NATIVE_X11
#elif WGPU_TARGET == WGPU_TARGET_LINUX_WAYLAND
#define GLFW_EXPOSE_NATIVE_WAYLAND
#elif WGPU_TARGET == WGPU_TARGET_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#ifndef __EMSCRIPTEN__
#include <GLFW/glfw3native.h>
#endif

#include "glfw3webgpu.h"
#include "utils.h"

#ifndef _BACKEND_TYPE_FROM_CODE
#define _BACKEND_TYPE_FROM_CODE(code)                                          \
    ((code == 0x00000000)   ? "Undefined"                                      \
     : (code == 0x00000001) ? "Null"                                           \
     : (code == 0x00000002) ? "WebGPU"                                         \
     : (code == 0x00000003) ? "D3D11"                                          \
     : (code == 0x00000004) ? "D3D12"                                          \
     : (code == 0x00000005) ? "Metal"                                          \
     : (code == 0x00000006) ? "Vulkan"                                         \
     : (code == 0x00000007) ? "OpenGL"                                         \
     : (code == 0x00000008) ? "OpenGLES"                                       \
     : (code == 0x7FFFFFFF) ? "Force32"                                        \
                            : "Unknown")
#endif /* ifndef _BACKEND_TYPE_FROM_CODE */

#ifndef _ADAPTER_TYPE_FROM_CODE
#define _ADAPTER_TYPE_FROM_CODE(code)                                          \
    ((code == 0x00000001)   ? "DiscreteGPU"                                    \
     : (code == 0x00000002) ? "IntegratedGPU"                                  \
     : (code == 0x00000003) ? "CPU"                                            \
     : (code == 0x00000004) ? "Unknown"                                        \
     : (code == 0x7FFFFFFF) ? "Force32"                                        \
                            : "Undefined")
#endif /* ifndef _ADAPTER_TYPE_FROM_CODE */

#ifndef _FEATURE_NAME_FROM_CODE
#define _FEATURE_NAME_FROM_CODE(code)                                          \
    ((code == 0x00000001)   ? "DepthClipControl"                               \
     : (code == 0x00000002) ? "Depth32FloatStencil8"                           \
     : (code == 0x00000003) ? "TimestampQuery"                                 \
     : (code == 0x00000004) ? "TextureCompressionBC"                           \
     : (code == 0x00000005) ? "TextureCompressionBCSliced3D"                   \
     : (code == 0x00000006) ? "TextureCompressionETC2"                         \
     : (code == 0x00000007) ? "TextureCompressionASTC"                         \
     : (code == 0x00000008) ? "TextureCompressionASTCSliced3D"                 \
     : (code == 0x00000009) ? "IndirectFirstInstance"                          \
     : (code == 0x0000000A) ? "ShaderF16"                                      \
     : (code == 0x0000000B) ? "RG11B10UfloatRenderable"                        \
     : (code == 0x0000000C) ? "BGRA8UnormStorage"                              \
     : (code == 0x0000000D) ? "Float32Filterable"                              \
     : (code == 0x0000000E) ? "Float32Blendable"                               \
     : (code == 0x0000000F) ? "ClipDistances"                                  \
     : (code == 0x00000010) ? "DualSourceBlending"                             \
     : (code == 0x00000011) ? "Subgroups"                                      \
     : (code == 0x00000012) ? "CoreFeaturesAndLimits"                          \
     : (code == 0x00050000) ? "DawnInternalUsages"                             \
     : (code == 0x00050001) ? "DawnMultiPlanarFormats"                         \
     : (code == 0x00050002) ? "DawnNative"                                     \
     : (code == 0x00050003) ? "ChromiumExperimentalTimestampQueryInsidePasses" \
     : (code == 0x00050004) ? "ImplicitDeviceSynchronization"                  \
     : (code == 0x00050005) ? "ChromiumExperimentalImmediateData"              \
     : (code == 0x00050006) ? "TransientAttachments"                           \
     : (code == 0x00050007) ? "MSAARenderToSingleSampled"                      \
     : (code == 0x00050008) ? "D3D11MultithreadProtected"                      \
     : (code == 0x00050009) ? "ANGLETextureSharing"                            \
     : (code == 0x0005000A) ? "PixelLocalStorageCoherent"                      \
     : (code == 0x0005000B) ? "PixelLocalStorageNonCoherent"                   \
     : (code == 0x0005000C) ? "Unorm16TextureFormats"                          \
     : (code == 0x0005000D) ? "Snorm16TextureFormats"                          \
     : (code == 0x0005000E) ? "MultiPlanarFormatExtendedUsages"                \
     : (code == 0x0005000F) ? "MultiPlanarFormatP010"                          \
     : (code == 0x00050010) ? "HostMappedPointer"                              \
     : (code == 0x00050011) ? "MultiPlanarRenderTargets"                       \
     : (code == 0x00050012) ? "MultiPlanarFormatNv12a"                         \
     : (code == 0x00050013) ? "FramebufferFetch"                               \
     : (code == 0x00050014) ? "BufferMapExtendedUsages"                        \
     : (code == 0x00050015) ? "AdapterPropertiesMemoryHeaps"                   \
     : (code == 0x00050016) ? "AdapterPropertiesD3D"                           \
     : (code == 0x00050017) ? "AdapterPropertiesVk"                            \
     : (code == 0x00050018) ? "R8UnormStorage"                                 \
     : (code == 0x00050019) ? "DawnFormatCapabilities"                         \
     : (code == 0x0005001A) ? "DawnDrmFormatCapabilities"                      \
     : (code == 0x0005001B) ? "Norm16TextureFormats"                           \
     : (code == 0x0005001C) ? "MultiPlanarFormatNv16"                          \
     : (code == 0x0005001D) ? "MultiPlanarFormatNv24"                          \
     : (code == 0x0005001E) ? "MultiPlanarFormatP210"                          \
     : (code == 0x0005001F) ? "MultiPlanarFormatP410"                          \
     : (code == 0x00050020) ? "SharedTextureMemoryVkDedicatedAllocation"       \
     : (code == 0x00050021) ? "SharedTextureMemoryAHardwareBuffer"             \
     : (code == 0x00050022) ? "SharedTextureMemoryDmaBuf"                      \
     : (code == 0x00050023) ? "SharedTextureMemoryOpaqueFD"                    \
     : (code == 0x00050024) ? "SharedTextureMemoryZirconHandle"                \
     : (code == 0x00050025) ? "SharedTextureMemoryDXGISharedHandle"            \
     : (code == 0x00050026) ? "SharedTextureMemoryD3D11Texture2D"              \
     : (code == 0x00050027) ? "SharedTextureMemoryIOSurface"                   \
     : (code == 0x00050028) ? "SharedTextureMemoryEGLImage"                    \
     : (code == 0x00050029) ? "SharedFenceVkSemaphoreOpaqueFD"                 \
     : (code == 0x0005002A) ? "SharedFenceSyncFD"                              \
     : (code == 0x0005002B) ? "SharedFenceVkSemaphoreZirconHandle"             \
     : (code == 0x0005002C) ? "SharedFenceDXGISharedHandle"                    \
     : (code == 0x0005002D) ? "SharedFenceMTLSharedEvent"                      \
     : (code == 0x0005002E) ? "SharedBufferMemoryD3D12Resource"                \
     : (code == 0x0005002F) ? "StaticSamplers"                                 \
     : (code == 0x00050030) ? "YCbCrVulkanSamplers"                            \
     : (code == 0x00050031) ? "ShaderModuleCompilationOptions"                 \
     : (code == 0x00050032) ? "DawnLoadResolveTexture"                         \
     : (code == 0x00050033) ? "DawnPartialLoadResolveTexture"                  \
     : (code == 0x00050034) ? "MultiDrawIndirect"                              \
     : (code == 0x00050035) ? "DawnTexelCopyBufferRowAlignment"                \
     : (code == 0x00050036) ? "FlexibleTextureViews"                           \
     : (code == 0x00050037) ? "ChromiumExperimentalSubgroupMatrix"             \
     : (code == 0x00050038) ? "SharedFenceEGLSync"                             \
     : (code == 0x00050039) ? "DawnDeviceAllocatorControl"                     \
     : (code == 0x7FFFFFFF) ? "Force3"                                         \
                            : "UnkownFeature")
#endif /* ifndef _FEATURE_NAME_FROM_CODE */

const char *get_feature_name_from_code(unsigned long code, size_t *len)
{
    const char *name = _FEATURE_NAME_FROM_CODE(code);
    *len             = strlen(name);
    return name;
}

const char *get_backend_type_from_code(unsigned long code, size_t *len)
{
    const char *type = _BACKEND_TYPE_FROM_CODE(code);
    *len             = strlen(type);
    return type;
}

const char *get_adapter_type_from_code(unsigned long code, size_t *len)
{
    const char *type = _ADAPTER_TYPE_FROM_CODE(code);
    *len             = strlen(type);
    return type;
}

static GLFWwindow *window = NULL;
const unsigned int xWin   = 640;
const unsigned int yWin   = 480;
static WGPUSurface surface;

enum command_buffer_t {
    None,
    /* Name taken from:
     * https://dawn.googlesource.com/samples/+/refs/heads/main/ ...
     *  ... src/samples/utils/SampleUtils.cpp
     * https://dawn.googlesource.com/dawn/+/refs/heads/main/ ...
     *  ... src/dawn/utils/TerribleCommandBuffer.h
     */
    Terrible,
    /* double terrible command_buffer_t (as per the same source) */
};

static enum command_buffer_t cmd_buf_t =
        None; /* Terrible; // When implemented */

void error_callback(int code, const char *description)
{
    if (description)
        fprintf(stderr, "[GLFW-ERR] Error (%d):\t\t%s\n", code, description);
}

void adapter_callback(WGPURequestAdapterStatus status, WGPUAdapter adapter,
                      struct WGPUStringView message, void *userdata1,
                      void *userdata2)
{
    if (status != WGPURequestAdapterStatus_Success) {
        char mbuf[message.length + 1];
        snprintf(mbuf, message.length + 1, "%s", message.data);
        fprintf(stderr, "[ERR] Request Adapter callback not successfull: %s\n",
                mbuf);
        return;
    }
    memmove(userdata1, &adapter, sizeof(WGPUAdapter));
}

void logging_callback(WGPULoggingType type, struct WGPUStringView message,
                      WGPU_NULLABLE void *userdata1,
                      WGPU_NULLABLE void *userdata2)
{
    char mbuf[message.length + 1];
    snprintf(mbuf, message.length + 1, "%s", message.data);

    switch (type) {
    case WGPULoggingType_Warning:
    case WGPULoggingType_Error:
    case WGPULoggingType_Force32:
        fprintf(stderr, "[%s] (%d)\t%s\n",
                ((type == WGPULoggingType_Warning)   ? "WARN"
                 : (type == WGPULoggingType_Force32) ? "FRC32"
                                                     : "ERR"),
                *( int * )userdata1, mbuf);
        break;
    case WGPULoggingType_Info:
    case WGPULoggingType_Verbose:
        printf("[%s] (%d)\t%s\n",
               ((type == WGPULoggingType_Verbose) ? "VERB" : "INFO"),
               *( int * )userdata1, mbuf);
        break;
    }
}

void exception_callback(const WGPUDevice *device, WGPUErrorType type,
                        struct WGPUStringView message,
                        WGPU_NULLABLE void   *userdata1,
                        WGPU_NULLABLE void   *userdata2)
{
    char mbuf[message.length + 1];
    snprintf(mbuf, message.length + 1, "%s", message.data);

    fprintf(stderr, "[U-ERR] (%d)\t", *( int * )userdata1);

    switch (type) {
    case WGPUErrorType_NoError:
        fprintf(stderr, "No Error: %s\n", mbuf);
        break;
    case WGPUErrorType_Validation:
        fprintf(stderr, "Validation: %s\n", mbuf);
        break;
    case WGPUErrorType_OutOfMemory:
        fprintf(stderr, "Out Of Memory: %s\n", mbuf);
        break;
    case WGPUErrorType_Internal:
        fprintf(stderr, "Internal: %s\n", mbuf);
        break;
    case WGPUErrorType_Unknown:
        fprintf(stderr, "Unknown: %s\n", mbuf);
        break;
    case WGPUErrorType_Force32:
        fprintf(stderr, "Force32: %s\n", mbuf);
        break;
    }

    return;
}

void lost_callback(const WGPUDevice *device, WGPUDeviceLostReason reason,
                   struct WGPUStringView message, WGPU_NULLABLE void *userdata1,
                   WGPU_NULLABLE void *userdata2)
{
    char mbuf[message.length + 1];
    snprintf(mbuf, message.length + 1, "%s", message.data);

    fprintf(stderr, "[WARN] (%d)\t Device Lost: ", *( int * )userdata1);

    switch (reason) {
    case WGPUDeviceLostReason_Unknown:
        fprintf(stderr, "Unknown: %s\n", mbuf);
        break;
    case WGPUDeviceLostReason_Destroyed:
        fprintf(stderr, "Destroyed: %s\n", mbuf);
        break;
    case WGPUDeviceLostReason_CallbackCancelled:
        fprintf(stderr, "Callback Cancelled: %s\n", mbuf);
        break;
    case WGPUDeviceLostReason_FailedCreation:
        fprintf(stderr, "Failed Creation: %s\n", mbuf);
        break;
    case WGPUDeviceLostReason_Force32:
        fprintf(stderr, "Force32: %s\n", mbuf);
        break;
    }

    return;
}

WGPUDevice create_general_device()
{
    if (glfwInit() != GLFW_TRUE) {
        fprintf(stderr, "Unable to initialise glfw3\n");
        return NULL;
    }
    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(xWin, yWin, "window", NULL, NULL);
    if (window == NULL) {
        const char *description;
        int         code = glfwGetError(&description);
        if (description)
            fprintf(stderr,
                    "[GLFW-ERR] Unable to create glfw3 window (%d): %s\n", code,
                    description);
        glfwTerminate();
        return NULL;
    }

    WGPUInstanceDescriptor instanceDesc = WGPU_INSTANCE_DESCRIPTOR_INIT;
    instanceDesc.capabilities.timedWaitAnyEnable = true;
    WGPUInstance instance = wgpuCreateInstance(&instanceDesc);
    if (instance == NULL) {
        fprintf(stderr, "[ERR] Unable to create WGPU instance\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    WGPUSurface backendSurface = glfwCreateWindowWGPUSurface(instance, window);
    // if (surface == NULL) {
    //     fprintf(stderr, "[ERR] Unable to create surface for window\n");
    //     glfwDestroyWindow(window);
    //     glfwTerminate();
    //     return NULL;
    // }

    WGPURequestAdapterOptions options = {0};
    options.nextInChain               = NULL;
    options.compatibleSurface         = backendSurface;

#ifdef GLFW_EXPOSE_NATIVE_COCOA
    options.backendType = WGPUBackendType_Metal;
#endif /* GLFW_EXPOSE_NATIVE_COCOA */

    WGPUAdapter                    adapter      = {0};
    WGPURequestAdapterCallbackInfo callbackInfo = {0};
    callbackInfo.mode                           = WGPUCallbackMode_WaitAnyOnly;
    callbackInfo.userdata1                      = &adapter;
    callbackInfo.callback                       = adapter_callback;

    WGPUFuture future =
            wgpuInstanceRequestAdapter(instance, &options, callbackInfo);
    WGPUFutureWaitInfo futureInfo = {.future = future, .completed = false};
    WGPUWaitStatus     waitStatus =
            wgpuInstanceWaitAny(instance, 1, &futureInfo, UINT64_MAX);
    struct timespec ts;
    ts.tv_sec  = 0;
    ts.tv_nsec = 200 * 1000;
    while (!futureInfo.completed) {
        printf("[ERR] Waiting for future to complete...\n");
        nanosleep(&ts, NULL);
    }
    if (waitStatus != WGPUWaitStatus_Success) {
        fprintf(stderr, "[ERR] Unsuccessful instance wait any call\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }
    if (callbackInfo.userdata1 == NULL) {
        fprintf(stderr, "[ERR] Unable to get adapter from instance\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    WGPUAdapterInfo adapterInfo = WGPU_ADAPTER_INFO_INIT;
    WGPUStatus      infoStatus  = wgpuAdapterGetInfo(adapter, &adapterInfo);
    if (infoStatus != WGPUStatus_Success) {
        fprintf(stderr, "[ERR] Unable to get adapter info\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    WGPUDeviceDescriptor deviceDesc = WGPU_DEVICE_DESCRIPTOR_INIT;

    WGPUUncapturedErrorCallbackInfo exceptionInfo =
            WGPU_UNCAPTURED_ERROR_CALLBACK_INFO_INIT;
    exceptionInfo.callback                 = exception_callback;
    exceptionInfo.userdata1                = &adapterInfo.deviceID;
    deviceDesc.uncapturedErrorCallbackInfo = exceptionInfo;
    WGPUDeviceLostCallbackInfo lostInfo = WGPU_DEVICE_LOST_CALLBACK_INFO_INIT;
    lostInfo.mode                       = WGPUCallbackMode_AllowSpontaneous;
    lostInfo.callback                   = lost_callback;
    lostInfo.userdata1                  = &adapterInfo.deviceID;
    deviceDesc.deviceLostCallbackInfo   = lostInfo;

    WGPUDevice backendDevice = wgpuAdapterCreateDevice(adapter, &deviceDesc);
    if (backendDevice == NULL) {
        fprintf(stderr, "[ERR] Unable to create device from adapter\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    // WGPUSurfaceConfiguration surfaceConfig = WGPU_SURFACE_CONFIGURATION_INIT;
    // surfaceConfig.device                   = backendDevice;
    // surfaceConfig.width                    = xWin;
    // surfaceConfig.height                   = yWin;
    // wgpuSurfaceConfigure(backendSurface, &surfaceConfig);

    WGPUDevice device = NULL;

    // switch (cmd_buf_t) {
    // case None:
    // case None:
    device  = backendDevice;
    surface = backendSurface;
    //     break;
    // case Terrible:
    //     break;
    // };

    WGPULoggingCallbackInfo loggingInfo = WGPU_LOGGING_CALLBACK_INFO_INIT;
    loggingInfo.callback                = logging_callback;
    loggingInfo.userdata1               = &adapterInfo.deviceID;
    wgpuDeviceSetLoggingCallback(device, loggingInfo);

    wgpuInstanceRelease(instance);
    wgpuAdapterRelease(adapter);

    return device;
}

GLFWwindow *get_glfw_window() { return window; }

WGPUSurface get_surface() { return surface; }

// vim: set ft=c ts=4 sts=4 sw=4 et cin ai nospell

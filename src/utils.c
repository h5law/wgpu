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

#include <wgpu/webgpu.h>
#include <wgpu/wgpu.h>

#ifdef GLFW_EXPOSE_NATIVE_COCOA
#include <Foundation/Foundation.h>
#include <QuartzCore/CAMetalLayer.h>
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "utils.h"

#ifndef LOG_PREFIX
#define LOG_PREFIX "[WGPU::LOG]"
#endif /* ifdef LOG_PREFIX */

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
    ((code == 0x00000000)   ? "Undefined"                                      \
     : (code == 0x00000001) ? "DepthClipControl"                               \
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
     : (code == 0x7FFFFFFF) ? "Force3"                                         \
                            : "UnkownFeature")
#endif /* ifndef _FEATURE_NAME_FROM_CODE */

#ifndef _DEVICE_LIMITS
#define _DEVICE_LIMITS(limit)                                                  \
    printf("Device Limit: \t\tmaxTextureDimension1D(%d)\n",                    \
           limit.maxTextureDimension1D);                                       \
    printf("Device Limit: \t\tmaxTextureDimension2D(%d)\n",                    \
           limit.maxTextureDimension2D);                                       \
    printf("Device Limit: \t\tmaxTextureDimension3D(%d)\n",                    \
           limit.maxTextureDimension3D);                                       \
    printf("Device Limit: \t\tmaxTextureArrayLayers(%d)\n",                    \
           limit.maxTextureArrayLayers);                                       \
    printf("Device Limit: \t\tmaxBindGroups(%d)\n", limit.maxBindGroups);      \
    printf("Device Limit: \t\tmaxBindGroupsPlusVertexBuffers(%d)\n",           \
           limit.maxBindGroupsPlusVertexBuffers);                              \
    printf("Device Limit: \t\tmaxBindingsPerBindGroup(%d)\n",                  \
           limit.maxBindingsPerBindGroup);                                     \
    printf("Device Limit: "                                                    \
           "\t\tmaxDynamicUniformBuffersPerPipelineLayout(%d)\n",              \
           limit.maxDynamicUniformBuffersPerPipelineLayout);                   \
    printf("Device Limit: "                                                    \
           "\t\tmaxDynamicStorageBuffersPerPipelineLayout(%d)\n",              \
           limit.maxDynamicStorageBuffersPerPipelineLayout);                   \
    printf("Device Limit: \t\tmaxSampledTexturesPerShaderStage(%d)\n",         \
           limit.maxSampledTexturesPerShaderStage);                            \
    printf("Device Limit: \t\tmaxSamplersPerShaderStage(%d)\n",                \
           limit.maxSamplersPerShaderStage);                                   \
    printf("Device Limit: \t\tmaxStorageBuffersPerShaderStage(%d)\n",          \
           limit.maxStorageBuffersPerShaderStage);                             \
    printf("Device Limit: \t\tmaxStorageTexturesPerShaderStage(%d)\n",         \
           limit.maxStorageTexturesPerShaderStage);                            \
    printf("Device Limit: \t\tmaxUniformBuffersPerShaderStage(%d)\n",          \
           limit.maxUniformBuffersPerShaderStage);                             \
    printf("Device Limit: \t\tmaxUniformBufferBindingSize(%llu)\n",            \
           limit.maxUniformBufferBindingSize);                                 \
    printf("Device Limit: \t\tmaxStorageBufferBindingSize(%llu)\n",            \
           limit.maxStorageBufferBindingSize);                                 \
    printf("Device Limit: \t\tminUniformBufferOffsetAlignment(%d)\n",          \
           limit.minUniformBufferOffsetAlignment);                             \
    printf("Device Limit: \t\tminStorageBufferOffsetAlignment(%d)\n",          \
           limit.minStorageBufferOffsetAlignment);                             \
    printf("Device Limit: \t\tmaxVertexBuffers(%d)\n",                         \
           limit.maxVertexBuffers);                                            \
    printf("Device Limit: \t\tmaxBufferSize(%llu)\n", limit.maxBufferSize);    \
    printf("Device Limit: \t\tmaxVertexAttributes(%d)\n",                      \
           limit.maxVertexAttributes);                                         \
    printf("Device Limit: \t\tmaxVertexBufferArrayStride(%d)\n",               \
           limit.maxVertexBufferArrayStride);                                  \
    printf("Device Limit: \t\tmaxInterStageShaderVariables(%d)\n",             \
           limit.maxInterStageShaderVariables);                                \
    printf("Device Limit: \t\tmaxColorAttachments(%d)\n",                      \
           limit.maxColorAttachments);                                         \
    printf("Device Limit: \t\tmaxColorAttachmentBytesPerSample(%d)\n",         \
           limit.maxColorAttachmentBytesPerSample);                            \
    printf("Device Limit: \t\tmaxComputeWorkgroupStorageSize(%d)\n",           \
           limit.maxComputeWorkgroupStorageSize);                              \
    printf("Device Limit: \t\tmaxComputeInvocationsPerWorkgroup(%d)\n",        \
           limit.maxComputeInvocationsPerWorkgroup);                           \
    printf("Device Limit: \t\tmaxComputeWorkgroupSizeX(%d)\n",                 \
           limit.maxComputeWorkgroupSizeX);                                    \
    printf("Device Limit: \t\tmaxComputeWorkgroupSizeY(%d)\n",                 \
           limit.maxComputeWorkgroupSizeY);                                    \
    printf("Device Limit: \t\tmaxComputeWorkgroupSizeZ(%d)\n",                 \
           limit.maxComputeWorkgroupSizeZ);                                    \
    printf("Device Limit: \t\tmaxComputeWorkgroupsPerDimension(%d)\n",         \
           limit.maxComputeWorkgroupsPerDimension);
#endif /* ifndef _DEVICE_LIMITS */

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

void get_device_limits(WGPULimits limits) { _DEVICE_LIMITS(limits); }

const unsigned int xWin  = 640;
const unsigned int yWin  = 480;
struct WGPUState   state = {0};

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

static void adapter_callback(WGPURequestAdapterStatus status,
                             WGPUAdapter adapter, struct WGPUStringView message,
                             void *userdata1, void *userdata2)
{
    char mbuf[message.length + 1];
    snprintf(mbuf, message.length + 1, "%s", message.data);
    if (status != WGPURequestAdapterStatus_Success) {
        fprintf(stderr, "[ERR] Request Adapter callback unsuccessful: %s\n",
                mbuf);
        return;
    }
    memmove(userdata1, &adapter, sizeof(WGPUAdapter));
    printf(LOG_PREFIX " request_adapter status=%#.8x message=%s\n", status,
           mbuf);
}

static void device_callback(WGPURequestDeviceStatus status, WGPUDevice device,
                            WGPUStringView message, void *userdata1,
                            void *userdata2)
{
    if (status != WGPURequestDeviceStatus_Success) {
        char mbuf[message.length + 1];
        snprintf(mbuf, message.length + 1, "%s", message.data);
        fprintf(stderr, "[ERR] Request Device callback unsuccessful: %s\n",
                mbuf);
        return;
    }
    memmove(userdata1, &device, sizeof(WGPUDevice));
    printf(LOG_PREFIX " request_device status=%#.8x message=%.*s\n", status,
           ( int )message.length, message.data);
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
    case WGPUDeviceLostReason_InstanceDropped:
        fprintf(stderr, "Instance Dropped: %s\n", mbuf);
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

struct WGPUState create_general_device()
{
    if (glfwInit() != GLFW_TRUE) {
        fprintf(stderr, "Unable to initialise glfw3\n");
        return state;
    }
    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    state.window = glfwCreateWindow(xWin, yWin, "window", NULL, NULL);
    if (state.window == NULL) {
        const char *description;
        int         code = glfwGetError(&description);
        if (description)
            fprintf(stderr,
                    "[GLFW-ERR] Unable to create glfw3 window (%d): %s\n", code,
                    description);
        glfwTerminate();
        return state;
    }

    state.instance = wgpuCreateInstance(NULL);
    if (state.instance == NULL) {
        fprintf(stderr, "[ERR] Unable to create WGPU instance\n");
        glfwDestroyWindow(state.window);
        glfwTerminate();
        return state;
    }

#if defined(GLFW_EXPOSE_NATIVE_COCOA)
    {
        id        metal_layer = NULL;
        NSWindow *ns_window   = glfwGetCocoaWindow(state.window);
        [ns_window.contentView setWantsLayer:YES];
        metal_layer = [CAMetalLayer layer];
        [ns_window.contentView setLayer:metal_layer];
        state.surface = wgpuInstanceCreateSurface(
                state.instance,
                &( const WGPUSurfaceDescriptor ){
                        .nextInChain =
                                ( const WGPUChainedStruct * )&(
                                        const WGPUSurfaceSourceMetalLayer ){
                                        .chain =
                                                ( const WGPUChainedStruct ){
                                                        .sType =
                                                                WGPUSType_SurfaceSourceMetalLayer,
                                                },
                                        .layer = metal_layer,
                                },
                });
    }
#elif defined(GLFW_EXPOSE_NATIVE_WAYLAND) && defined(GLFW_EXPOSE_NATIVE_X11)
    if (glfwGetPlatform() == GLFW_PLATFORM_X11) {
        Display *x11_display = glfwGetX11Display();
        Window   x11_window  = glfwGetX11Window(state.window);
        state.surface        = wgpuInstanceCreateSurface(
                state.instance,
                &( const WGPUSurfaceDescriptor ){
                               .nextInChain =
                                ( const WGPUChainedStruct * )&(
                                        const WGPUSurfaceSourceXlibWindow ){
                                               .chain =
                                                ( const WGPUChainedStruct ){
                                                               .sType =
                                                                WGPUSType_SurfaceSourceXlibWindow,
                                                },
                                               .display = x11_display,
                                               .window  = x11_window,
                                },
                });
    }
    if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND) {
        struct wl_display *wayland_display = glfwGetWaylandDisplay();
        struct wl_surface *wayland_surface = glfwGetWaylandWindow(state.window);
        state.surface                      = wgpuInstanceCreateSurface(
                state.instance,
                &( const WGPUSurfaceDescriptor ){
                                             .nextInChain =
                                ( const WGPUChainedStruct * )&(
                                        const WGPUSurfaceSourceWaylandSurface ){
                                                             .chain =
                                                ( const WGPUChainedStruct ){
                                                                             .sType =
                                                                WGPUSType_SurfaceSourceWaylandSurface,
                                                },
                                                             .display = wayland_display,
                                                             .surface = wayland_surface,
                                },
                });
    }
#elif defined(GLFW_EXPOSE_NATIVE_WIN32)
    {
        HWND      hwnd      = glfwGetWin32Window(state.window);
        HINSTANCE hinstance = GetModuleHandle(NULL);
        state.surface       = wgpuInstanceCreateSurface(
                state.instance,
                &( const WGPUSurfaceDescriptor ){
                              .nextInChain =
                                ( const WGPUChainedStruct * )&(
                                        const WGPUSurfaceSourceWindowsHWND ){
                                              .chain =
                                                ( const WGPUChainedStruct ){
                                                              .sType =
                                                                WGPUSType_SurfaceSourceWindowsHWND,
                                                },
                                              .hinstance = hinstance,
                                              .hwnd      = hwnd,
                                },
                });
    }
#else
#error "Unsupported GLFW native platform"
#endif

    WGPURequestAdapterOptions options = {0};
    options.nextInChain               = NULL;
    options.featureLevel              = WGPUFeatureLevel_Core;
    options.compatibleSurface         = state.surface;

    WGPURequestAdapterCallbackInfo callbackInfo = {0};
    callbackInfo.mode                           = WGPUCallbackMode_WaitAnyOnly;
    callbackInfo.userdata1                      = &state.adapter;
    callbackInfo.callback                       = adapter_callback;

    wgpuInstanceRequestAdapter(state.instance, &options, callbackInfo);
    if (state.adapter == NULL) {
        fprintf(stderr, "[ERR] Unable to get adapter from instance\n");
        glfwDestroyWindow(state.window);
        glfwTerminate();
        return state;
    }

    WGPUAdapterInfo adapterInfo = {0};
    WGPUStatus infoStatus = wgpuAdapterGetInfo(state.adapter, &adapterInfo);
    if (infoStatus != WGPUStatus_Success) {
        fprintf(stderr, "[ERR] Unable to get adapter info\n");
        glfwDestroyWindow(state.window);
        glfwTerminate();
        return state;
    }

    WGPUDeviceDescriptor            deviceDesc    = {0};
    WGPUUncapturedErrorCallbackInfo exceptionInfo = {0};
    exceptionInfo.callback                        = exception_callback;
    exceptionInfo.userdata1                       = &adapterInfo.deviceID;
    deviceDesc.uncapturedErrorCallbackInfo        = exceptionInfo;
    WGPUDeviceLostCallbackInfo lostInfo           = {0};
    lostInfo.mode                     = WGPUCallbackMode_AllowSpontaneous;
    lostInfo.callback                 = lost_callback;
    lostInfo.userdata1                = &adapterInfo.deviceID;
    deviceDesc.deviceLostCallbackInfo = lostInfo;

    wgpuAdapterRequestDevice(
            state.adapter, &deviceDesc,
            ( const WGPURequestDeviceCallbackInfo ){
                    .callback = device_callback, .userdata1 = &state.device});
    if (state.device == NULL) {
        fprintf(stderr, "[ERR] Unable to create device from adapter\n");
        glfwDestroyWindow(state.window);
        glfwTerminate();
        return state;
    }

    WGPUSurfaceCapabilities surface_capabilities = {0};
    wgpuSurfaceGetCapabilities(state.surface, state.adapter,
                               &surface_capabilities);
    state.surfaceConfig.nextInChain = NULL;
    state.surfaceConfig.device      = state.device;
    state.surfaceConfig.width       = xWin;
    state.surfaceConfig.height      = yWin;
    state.surfaceConfig.usage       = WGPUTextureUsage_RenderAttachment,
    state.surfaceConfig.format      = surface_capabilities.formats[0],
    state.surfaceConfig.presentMode = WGPUPresentMode_Fifo,
    state.surfaceConfig.alphaMode   = surface_capabilities.alphaModes[0],
    wgpuSurfaceConfigure(state.surface, &state.surfaceConfig);

    // switch (cmd_buf_t) {
    // case None:
    // case None:
    // device = backendDevice;
    //     break;
    // case Terrible:
    //     break;
    // };

    return state;
}

GLFWwindow *get_glfw_window() { return state.window; }

WGPUSurface get_surface() { return state.surface; }

// vim: set ft=c ts=4 sts=4 sw=4 et cin ai nospell

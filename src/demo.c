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

#include <GLFW/glfw3.h>
#include <wgpu/webgpu.h>
#include <wgpu/wgpu.h>

#include "utils.h"

int main(int argc, char *argv[])
{
    struct WGPUState state = create_general_device();
    if (state.device == NULL) {
        fprintf(stderr, "Unable to get general device\n");
        if (state.window) {
            glfwDestroyWindow(state.window);
            glfwTerminate();
        }
        return EXIT_FAILURE;
    }

    WGPUAdapterInfo adapterInfo = {0};
    wgpuAdapterGetInfo(state.adapter, &adapterInfo);
    printf("Surface:\t\t%p(0x%02lx)\n", ( void * )state.surface,
           ( unsigned long )(( void * )&state.surface));
    size_t      len = 0;
    const char *backendType =
            get_backend_type_from_code(adapterInfo.backendType, &len);
    printf("Backend Type: \t\t%s\n", backendType);
    len = 0;
    const char *adapterType =
            get_adapter_type_from_code(adapterInfo.adapterType, &len);
    printf("Adapter Type: \t\t%s\n", adapterType);
    len = 0;
    printf("VendorID: \t\t0x%02x\n", adapterInfo.vendorID);
    if (adapterInfo.vendor.length > 0) {
        char buf[adapterInfo.vendor.length + 1];
        snprintf(buf, adapterInfo.vendor.length + 1, "%s",
                 adapterInfo.vendor.data);
        printf("Vendor: \t\t%s\n", buf);
    }
    if (adapterInfo.architecture.length > 0) {
        char buf2[adapterInfo.architecture.length + 1];
        snprintf(buf2, adapterInfo.architecture.length + 1, "%s",
                 adapterInfo.architecture.data);
        printf("Architecture: \t\t%s\n", buf2);
    }
    printf("DeviceID: \t\t0x%02x\n", adapterInfo.deviceID);
    if (adapterInfo.device.length > 0) {
        char buf3[adapterInfo.device.length + 1];
        snprintf(buf3, adapterInfo.device.length + 1, "%s",
                 adapterInfo.device.data);
        printf("Device: \t\t%s\n", buf3);
    }
    if (adapterInfo.description.length > 0) {
        char buf4[adapterInfo.description.length + 1];
        snprintf(buf4, adapterInfo.description.length + 1, "%s",
                 adapterInfo.description.data);
        printf("Description: \t\t%s\n", buf4);
    }
    // WGPUSupportedFeatures features = {0};
    // wgpuDeviceGetFeatures(state.device, &features);
    // for (size_t i = 0; i < features.featureCount; ++i) {
    //     size_t      len = 0;
    //     const char *featName =
    //             get_feature_name_from_code(features.features[i], &len);
    //     printf("Device feature: \t%s(0x%02x)\n", featName,
    //            features.features[i]);
    // }
    // WGPULimits limits = {0};
    // wgpuDeviceGetLimits(state.device, &limits);
    // get_device_limits(limits);
    // WGPUSupportedFeatures features2 = {0};
    // wgpuAdapterGetFeatures(state.adapter, &features2);
    // for (size_t i = 0; i < features2.featureCount; ++i) {
    //     size_t      len = 0;
    //     const char *featName =
    //             get_feature_name_from_code(features2.features[i], &len);
    //     printf("Adapter feature: \t%s(0x%02x)\n", featName,
    //            features2.features[i]);
    // }

    WGPUQueue queue = wgpuDeviceGetQueue(state.device);
    if (queue == NULL) {
        fprintf(stderr, "Unable to get device queue\n");
        glfwDestroyWindow(state.window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    static const char fWgsl[] = "@fragment\n"
                                "fn main() -> @location(0) vec4f {\n"
                                "    return vec4f(1.0, 1.0, 1.0, 1.0);\n"
                                "}\n";
    WGPUStringView    fsWgsl  = {.length = strlen(fWgsl), .data = fWgsl};
    WGPUShaderModuleDescriptor fsShaderModDesc = {
            .nextInChain = &(( WGPUShaderSourceWGSL ){
                                     .chain.sType = WGPUSType_ShaderSourceWGSL,
                                     .code        = fsWgsl,
                             })
                                    .chain};
    WGPUShaderModule fsShaderMod =
            wgpuDeviceCreateShaderModule(state.device, &fsShaderModDesc);

    static const char vWgsl[] =
            "struct VertexIn {\n"
            "    @location(0) a_particlePos : vec2f,\n"
            "              @location(1) a_particleVel : vec2f,\n"
            "    @location(2) a_pos : vec2f,\n"
            "};\n"
            "    @vertex\n"
            "    fn main(input : VertexIn) -> @builtin(position) vec4f {\n"
            "    var angle : f32 = -atan2(input.a_particleVel.x, "
            "input.a_particleVel.y);\n"
            "    var pos : vec2f = vec2f(\n"
            "        (input.a_pos.x * cos(angle)) - (input.a_pos.y * "
            "sin(angle)),\n"
            "        (input.a_pos.x * sin(angle)) + (input.a_pos.y * "
            "cos(angle)));\n"
            "    return vec4f(pos + input.a_particlePos, 0.0, 1.0);\n"
            "}\n";
    WGPUStringView vsWgsl = {.length = strlen(vWgsl), .data = vWgsl};
    WGPUShaderModuleDescriptor vsShaderModDesc = {
            .nextInChain = &(( WGPUShaderSourceWGSL ){
                                     .chain.sType = WGPUSType_ShaderSourceWGSL,
                                     .code        = vsWgsl,
                             })
                                    .chain};
    WGPUShaderModule vsShaderMod =
            wgpuDeviceCreateShaderModule(state.device, &vsShaderModDesc);

    static const char wgsl[] =
            "struct Particle {\n"
            "    pos : vec2f,\n"
            "    vel : vec2f,\n"
            "};\n"
            "struct SimParams {\n"
            "    deltaT : f32,\n"
            "    rule1Distance : f32,\n"
            "    rule2Distance : f32,\n"
            "    rule3Distance : f32,\n"
            "    rule1Scale : f32,\n"
            "    rule2Scale : f32,\n"
            "    rule3Scale : f32,\n"
            "    particleCount : u32,\n"
            "};\n"
            "struct Particles {\n"
            "    particles : array<Particle>,\n"
            "};\n"
            "@binding(0) @group(0) var<uniform> params : SimParams;\n"
            "@binding(1) @group(0) var<storage, read_write> particlesA : "
            "Particles;\n"
            "@binding(2) @group(0) var<storage, read_write> particlesB : "
            "Particles;\n"
            "// "
            "https://github.com/austinEng/Project6-Vulkan-Flocking/blob/master/"
            "data/shaders/computeparticles/particle.comp\n"
            "@compute @workgroup_size(64)\n"
            "fn main(@builtin(global_invocation_id) GlobalInvocationID : "
            "vec3u) {\n"
            "    var index : u32 = GlobalInvocationID.x;\n"
            "    if (index >= params.particleCount) {\n"
            "        return;\n"
            "    }\n"
            "    var vPos : vec2f = particlesA.particles[index].pos;\n"
            "    var vVel : vec2f = particlesA.particles[index].vel;\n"
            "    var cMass : vec2f = vec2f(0.0, 0.0);\n"
            "    var cVel : vec2f = vec2f(0.0, 0.0);\n"
            "    var colVel : vec2f = vec2f(0.0, 0.0);\n"
            "    var cMassCount : u32 = 0u;\n"
            "    var cVelCount : u32 = 0u;\n"
            "    var pos : vec2f;\n"
            "    var vel : vec2f;\n"
            "    for (var i : u32 = 0u; i < params.particleCount; i = i + 1u) "
            "{\n"
            "        if (i == index) {\n"
            "            continue;\n"
            "        }\n"
            "        pos = particlesA.particles[i].pos.xy;\n"
            "        vel = particlesA.particles[i].vel.xy;\n"
            "        if (distance(pos, vPos) < params.rule1Distance) {\n"
            "            cMass = cMass + pos;\n"
            "            cMassCount = cMassCount + 1u;\n"
            "        }\n"
            "        if (distance(pos, vPos) < params.rule2Distance) {\n"
            "            colVel = colVel - (pos - vPos);\n"
            "        }\n"
            "        if (distance(pos, vPos) < params.rule3Distance) {\n"
            "            cVel = cVel + vel;\n"
            "            cVelCount = cVelCount + 1u;\n"
            "        }\n"
            "    }\n"
            "    if (cMassCount > 0u) {\n"
            "        cMass = (cMass / vec2f(f32(cMassCount), f32(cMassCount))) "
            "- vPos;\n"
            "    }\n"
            "    if (cVelCount > 0u) {\n"
            "        cVel = cVel / vec2f(f32(cVelCount), f32(cVelCount));\n"
            "    }\n"
            "    vVel = vVel + (cMass * params.rule1Scale) + (colVel * "
            "params.rule2Scale) +\n"
            "        (cVel * params.rule3Scale);\n"
            "    // clamp velocity for a more pleasing simulation\n"
            "    vVel = normalize(vVel) * clamp(length(vVel), 0.0, 0.1);\n"
            "    // kinematic update\n"
            "    vPos = vPos + (vVel * params.deltaT);\n"
            "    // Wrap around boundary\n"
            "    if (vPos.x < -1.0) {\n"
            "        vPos.x = 1.0;\n"
            "    }\n"
            "    if (vPos.x > 1.0) {\n"
            "        vPos.x = -1.0;\n"
            "    }\n"
            "    if (vPos.y < -1.0) {\n"
            "        vPos.y = 1.0;\n"
            "    }\n"
            "    if (vPos.y > 1.0) {\n"
            "        vPos.y = -1.0;\n"
            "    }\n"
            "    // Write back\n"
            "    particlesB.particles[index].pos = vPos;\n"
            "    particlesB.particles[index].vel = vVel;\n"
            "    return;\n"
            "}\n";
    WGPUStringView shaderCode = {.length = strlen(wgsl), .data = wgsl};
    WGPUShaderModuleDescriptor shaderModDesc = {
            .nextInChain = &(( WGPUShaderSourceWGSL ){
                                     .chain.sType = WGPUSType_ShaderSourceWGSL,
                                     .code        = shaderCode,
                             })
                                    .chain,
    };
    WGPUShaderModule shaderMod =
            wgpuDeviceCreateShaderModule(state.device, &shaderModDesc);

    WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(
            state.device, &( const WGPUPipelineLayoutDescriptor ){
                                  .label = {"pipeline_layout", WGPU_STRLEN},
                          });
    if (pipeline_layout == NULL) {
        fprintf(stderr, "Unable to get pipeline layout\n");
        glfwDestroyWindow(state.window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    WGPUSurfaceCapabilities surfaceCaps = {0};
    wgpuSurfaceGetCapabilities(state.surface, state.adapter, &surfaceCaps);

    WGPURenderPipeline renderPipe = wgpuDeviceCreateRenderPipeline(
            state.device,
            &( const WGPURenderPipelineDescriptor ){
                    .label  = {"render_pipeline", WGPU_STRLEN},
                    .layout = pipeline_layout,
                    .vertex =
                            ( const WGPUVertexState ){
                                    .module      = vsShaderMod,
                                    .bufferCount = 2,
                            },
                    .fragment =
                            &( const WGPUFragmentState ){
                                    .module      = fsShaderMod,
                                    .targetCount = 1,
                                    .targets =
                                            ( const WGPUColorTargetState[] ){
                                                    ( const WGPUColorTargetState ){
                                                            .format =
                                                                    surfaceCaps.formats
                                                                            [0],
                                                            .writeMask =
                                                                    WGPUColorWriteMask_All,
                                                    },
                                            },
                            },
                    .primitive =
                            ( const WGPUPrimitiveState ){
                                    .topology =
                                            WGPUPrimitiveTopology_TriangleList,
                            },
                    .multisample =
                            ( const WGPUMultisampleState ){
                                    .count = 1,
                                    .mask  = 0xFFFFFFFF,
                            },
            });

    state.surfaceConfig = ( const WGPUSurfaceConfiguration ){
            .device      = state.device,
            .usage       = WGPUTextureUsage_RenderAttachment,
            .format      = surfaceCaps.formats[0],
            .presentMode = WGPUPresentMode_Fifo,
            .alphaMode   = surfaceCaps.alphaModes[0],
    };

    int width, height;
    glfwGetWindowSize(state.window, &width, &height);
    state.surfaceConfig.width  = width;
    state.surfaceConfig.height = height;
    wgpuSurfaceConfigure(state.surface, &state.surfaceConfig);

    while (!glfwWindowShouldClose(state.window)) {
        // Check whether the user clicked on the close button (and any other
        // mouse/key event, which we don't use so far)
        glfwPollEvents();
    }

    wgpuDeviceRelease(state.device);
    wgpuAdapterRelease(state.adapter);
    wgpuSurfaceRelease(state.surface);
    glfwDestroyWindow(state.window);
    wgpuInstanceRelease(state.instance);
    glfwTerminate();

    return EXIT_SUCCESS;
}

// vim: set ft=c ts=4 sts=4 sw=4 et cin ai nospell

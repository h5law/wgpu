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

#ifndef WGPU_UTILS_H
#define WGPU_UTILS_H

#include <GLFW/glfw3.h>
#include <wgpu/webgpu.h>
#include <wgpu/wgpu.h>

int  init_app(int argc, const char **argv);
int  should_quit();
void do_flush();
void process_events();

GLFWwindow *get_glfw_window();

struct WGPUState {
    WGPUInstance             instance;
    WGPUAdapter              adapter;
    WGPUSurface              surface;
    WGPUSurfaceConfiguration surfaceConfig;
    WGPUDevice               device;
    GLFWwindow              *window;
};
struct WGPUState create_general_device();
WGPUTextureView  create_default_depth_stencil_view(WGPUDevice device);
WGPUSurface      get_surface();

const char *get_feature_name_from_code(unsigned long code, size_t *len);
const char *get_adapter_type_from_code(unsigned long code, size_t *len);
const char *get_backend_type_from_code(unsigned long code, size_t *len);
void        get_device_limits(WGPULimits limits);

#endif /* ifndef WGPU_UTILS_H */

// vim: set ft=c ts=4 sts=4 sw=4 et cin ai nospell

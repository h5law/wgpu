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

#ifndef WGPU_FEATURES_H
#define WGPU_FEATURES_H

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
                            : "")

#endif /* ifndef WGPU_FEATURES_H */

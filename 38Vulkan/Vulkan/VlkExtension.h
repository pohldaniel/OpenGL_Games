#pragma once
#include <vulkan/vulkan.h>

typedef void (APIENTRY* PFN_vkCmdBindShadersEXT)(VkCommandBuffer commandBuffer, uint32_t stageCount, const VkShaderStageFlagBits* pStages, const VkShaderEXT* pShaders);
typedef void (APIENTRY* PFN_vkCmdSetColorBlendEnableEXT)(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkBool32* pColorBlendEnables);
typedef void (APIENTRY* PFN_vkCmdSetSampleMaskEXT)(VkCommandBuffer commandBuffer, VkSampleCountFlagBits samples, const VkSampleMask* pSampleMask);
typedef void (APIENTRY* PFN_vkCmdSetVertexInputEXT)(VkCommandBuffer commandBuffer, uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions, uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions);
typedef void (APIENTRY* PFN_vkCmdSetPolygonModeEXT)(VkCommandBuffer commandBuffer, VkPolygonMode polygonMode);
typedef void (APIENTRY* PFN_vkCmdSetColorBlendEquationEXT)(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t  attachmentCount, const VkColorBlendEquationEXT* pColorBlendEquations);
typedef void (APIENTRY* PFN_vkCmdSetRasterizationSamplesEXT)(VkCommandBuffer commandBuffer, VkSampleCountFlagBits rasterizationSamples);
typedef void (APIENTRY* PFN_vkCmdSetAlphaToCoverageEnableEXT)(VkCommandBuffer commandBuffer, VkBool32  alphaToCoverageEnable);
typedef void (APIENTRY* PFN_vkCmdSetColorWriteMaskEXT)(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorComponentFlags* pColorWriteMasks);
typedef VkResult (APIENTRY* PFN_vkCreateDebugUtilsMessengerEXT)(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger);
typedef VkResult(APIENTRY* PFN_vkCreateShadersEXT)(VkDevice device, uint32_t createInfoCount, const VkShaderCreateInfoEXT* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkShaderEXT* pShaders);

extern "C" {

    extern PFN_vkCmdBindShadersEXT pfn_vkCmdBindShadersEXT;
    extern PFN_vkCmdSetColorBlendEnableEXT pfn_vkCmdSetColorBlendEnableEXT;
    extern PFN_vkCmdSetSampleMaskEXT pfn_vkCmdSetSampleMaskEXT;
    extern PFN_vkCmdSetVertexInputEXT pfn_vkCmdSetVertexInputEXT;
    extern PFN_vkCmdSetPolygonModeEXT pfn_vkCmdSetPolygonModeEXT;
    extern PFN_vkCmdSetColorBlendEquationEXT pfn_vkCmdSetColorBlendEquationEXT;
    extern PFN_vkCmdSetRasterizationSamplesEXT pfn_vkCmdSetRasterizationSamplesEXT;
    extern PFN_vkCmdSetAlphaToCoverageEnableEXT pfn_vkCmdSetAlphaToCoverageEnableEXT;
    extern PFN_vkCmdSetColorWriteMaskEXT pfn_vkCmdSetColorWriteMaskEXT;
    extern PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT;
    extern PFN_vkCreateShadersEXT pfn_vkCreateShadersEXT;

    extern void vkExtensionInit(VkInstance instance);

	extern void vkCmdBindShadersEXT(VkCommandBuffer commandBuffer, uint32_t stageCount, const VkShaderStageFlagBits* pStages, const VkShaderEXT* pShaders);
    extern void vkCmdSetColorBlendEnableEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkBool32* pColorBlendEnables);  
    extern void vkCmdSetSampleMaskEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits samples, const VkSampleMask* pSampleMask);
    extern void vkCmdSetVertexInputEXT(VkCommandBuffer commandBuffer, uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions, uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions);   
    extern void vkCmdSetPolygonModeEXT(VkCommandBuffer commandBuffer, VkPolygonMode polygonMode);
    extern void vkCmdSetColorBlendEquationEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t  attachmentCount, const VkColorBlendEquationEXT* pColorBlendEquations);  
    extern void vkCmdSetRasterizationSamplesEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits rasterizationSamples);
    extern void vkCmdSetAlphaToCoverageEnableEXT(VkCommandBuffer commandBuffer, VkBool32  alphaToCoverageEnable);
    extern void vkCmdSetColorWriteMaskEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorComponentFlags* pColorWriteMasks);
    extern VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,VkDebugUtilsMessengerEXT* pMessenger);
    extern VkResult vkCreateShadersEXT(VkDevice device, uint32_t createInfoCount, const VkShaderCreateInfoEXT* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkShaderEXT* pShaders);
}
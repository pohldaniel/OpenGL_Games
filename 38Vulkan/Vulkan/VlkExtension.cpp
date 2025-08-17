#include <iostream>
#include "VlkExtension.h"

PFN_vkCmdBindShadersEXT pfn_vkCmdBindShadersEXT = 0;
PFN_vkCmdSetColorBlendEnableEXT pfn_vkCmdSetColorBlendEnableEXT = 0;
PFN_vkCmdSetSampleMaskEXT pfn_vkCmdSetSampleMaskEXT = 0;
PFN_vkCmdSetVertexInputEXT pfn_vkCmdSetVertexInputEXT = 0;
PFN_vkCmdSetPolygonModeEXT pfn_vkCmdSetPolygonModeEXT = 0;
PFN_vkCmdSetColorBlendEquationEXT pfn_vkCmdSetColorBlendEquationEXT = 0;
PFN_vkCmdSetRasterizationSamplesEXT pfn_vkCmdSetRasterizationSamplesEXT = 0;
PFN_vkCmdSetAlphaToCoverageEnableEXT pfn_vkCmdSetAlphaToCoverageEnableEXT = 0;
PFN_vkCmdSetColorWriteMaskEXT pfn_vkCmdSetColorWriteMaskEXT = 0;
PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT = 0;
PFN_vkCreateShadersEXT pfn_vkCreateShadersEXT = 0;

void vkExtensionInit(VkInstance instance) {
	pfn_vkCmdBindShadersEXT = reinterpret_cast<PFN_vkCmdBindShadersEXT>(vkGetInstanceProcAddr(instance, "vkCmdBindShadersEXT"));
	pfn_vkCmdSetColorBlendEnableEXT = reinterpret_cast<PFN_vkCmdSetColorBlendEnableEXT>(vkGetInstanceProcAddr(instance, "vkCmdSetColorBlendEnableEXT"));
	pfn_vkCmdSetSampleMaskEXT = reinterpret_cast<PFN_vkCmdSetSampleMaskEXT>(vkGetInstanceProcAddr(instance, "vkCmdSetSampleMaskEXT"));
	pfn_vkCmdSetVertexInputEXT = reinterpret_cast<PFN_vkCmdSetVertexInputEXT>(vkGetInstanceProcAddr(instance, "vkCmdSetVertexInputEXT"));;
	pfn_vkCmdSetPolygonModeEXT = reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>(vkGetInstanceProcAddr(instance, "vkCmdSetPolygonModeEXT"));
	pfn_vkCmdSetColorBlendEquationEXT = reinterpret_cast<PFN_vkCmdSetColorBlendEquationEXT>(vkGetInstanceProcAddr(instance, "vkCmdSetColorBlendEquationEXT"));
	pfn_vkCmdSetRasterizationSamplesEXT = reinterpret_cast<PFN_vkCmdSetRasterizationSamplesEXT>(vkGetInstanceProcAddr(instance, "vkCmdSetRasterizationSamplesEXT"));
	pfn_vkCmdSetAlphaToCoverageEnableEXT = reinterpret_cast<PFN_vkCmdSetAlphaToCoverageEnableEXT>(vkGetInstanceProcAddr(instance, "vkCmdSetAlphaToCoverageEnableEXT"));
	pfn_vkCmdSetColorWriteMaskEXT = reinterpret_cast<PFN_vkCmdSetColorWriteMaskEXT>(vkGetInstanceProcAddr(instance, "vkCmdSetColorWriteMaskEXT"));
	pfn_vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	pfn_vkCreateShadersEXT = reinterpret_cast<PFN_vkCreateShadersEXT>(vkGetInstanceProcAddr(instance, "vkCreateShadersEXT"));
}

void vkCmdBindShadersEXT(VkCommandBuffer commandBuffer, uint32_t stageCount, const VkShaderStageFlagBits* pStages, const VkShaderEXT* pShaders){
	pfn_vkCmdBindShadersEXT(commandBuffer, stageCount, pStages, pShaders);
}

void vkCmdSetColorBlendEnableEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkBool32* pColorBlendEnables) {
	pfn_vkCmdSetColorBlendEnableEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendEnables);
}

void vkCmdSetSampleMaskEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits samples, const VkSampleMask* pSampleMask) {
	pfn_vkCmdSetSampleMaskEXT(commandBuffer, samples, pSampleMask);
}

void vkCmdSetVertexInputEXT(VkCommandBuffer commandBuffer, uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions, uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions) {
	pfn_vkCmdSetVertexInputEXT(commandBuffer, vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
}

void vkCmdSetPolygonModeEXT(VkCommandBuffer commandBuffer, VkPolygonMode polygonMode) {
	pfn_vkCmdSetPolygonModeEXT(commandBuffer, polygonMode);
}

void vkCmdSetColorBlendEquationEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t  attachmentCount, const VkColorBlendEquationEXT* pColorBlendEquations) {
	pfn_vkCmdSetColorBlendEquationEXT(commandBuffer, firstAttachment, attachmentCount, pColorBlendEquations);
}

void vkCmdSetRasterizationSamplesEXT(VkCommandBuffer commandBuffer, VkSampleCountFlagBits rasterizationSamples) {
	pfn_vkCmdSetRasterizationSamplesEXT(commandBuffer, rasterizationSamples);
}

void vkCmdSetAlphaToCoverageEnableEXT(VkCommandBuffer commandBuffer, VkBool32  alphaToCoverageEnable) {
	pfn_vkCmdSetAlphaToCoverageEnableEXT(commandBuffer, alphaToCoverageEnable);
}

void vkCmdSetColorWriteMaskEXT(VkCommandBuffer commandBuffer, uint32_t firstAttachment, uint32_t attachmentCount, const VkColorComponentFlags* pColorWriteMasks) {
	pfn_vkCmdSetColorWriteMaskEXT(commandBuffer, firstAttachment, attachmentCount, pColorWriteMasks);
}

VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) {
	return pfn_vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VkResult vkCreateShadersEXT(VkDevice device, uint32_t createInfoCount, const VkShaderCreateInfoEXT* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkShaderEXT* pShaders) {
	return pfn_vkCreateShadersEXT(device, createInfoCount, pCreateInfos, pAllocator, pShaders);
}
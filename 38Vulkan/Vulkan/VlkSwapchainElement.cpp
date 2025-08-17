#include <iostream>
#include <imgui_impl_vulkan.h>
#include "VlkContext.h"
#include "VlkSwapchain.h"
#include "VlkSwapchainElement.h"
#include "VlkMesh.h"
#include "VlkTexture.h"

VlkSwapchainElement::VlkSwapchainElement(VlkSwapchain* swapchain, VkImage image, VkImage depthImage) : image(image), depthImage(depthImage), swapchain(swapchain) {
    const VkFormat vkDepthFormat = vlkContext.vkDepthFormat;

    vlkCreateCommandBuffer(commandBuffer);
    vlkCreateSemaphore(startSemaphore);
    vlkCreateSemaphore(endSemaphore);
    vlkCreateFence(fence);

    vlkCreateImageView(imageView, image, swapchain->format, VK_IMAGE_ASPECT_COLOR_BIT, { VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY });
    vlkCreateImageView(depthImageView, depthImage, vkDepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

    {
        colorAttachment = {};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = imageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = { { 1.0f, 1.0f, 1.0f, 1.0f } };

        depthStencilAttachment = {};
        depthStencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depthStencilAttachment.imageView = depthImageView;
        depthStencilAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthStencilAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
        depthStencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthStencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthStencilAttachment.clearValue.depthStencil.depth = 1.0f;
        depthStencilAttachment.clearValue.depthStencil.stencil = 0;

        renderingInfo = {};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = { { 0, 0 },{ swapchain->width, swapchain->height } };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthStencilAttachment;
        renderingInfo.pStencilAttachment = &depthStencilAttachment;

        viewport = {};
        viewport.x = 0.0f;
        viewport.y = static_cast<float>(swapchain->height);
        viewport.width = static_cast<float>(swapchain->width);
        viewport.height = -static_cast<float>(swapchain->height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor = { { 0, 0 },{ swapchain->width, swapchain->height } };
    }
}

VlkSwapchainElement::~VlkSwapchainElement(){
    const VkDevice& vkDevice = vlkContext.vkDevice;
    const VkCommandPool& vkCommandPool = vlkContext.vkCommandPool;

    vkFreeCommandBuffers(vkDevice, vkCommandPool, 1, &commandBuffer);
    vkDestroySemaphore(vkDevice, startSemaphore, nullptr);
    vkDestroySemaphore(vkDevice, endSemaphore, nullptr);
    vkDestroyFence(vkDevice, fence, nullptr);
    vkDestroyImageView(vkDevice, imageView, nullptr);
    vkDestroyImageView(vkDevice, depthImageView, nullptr);
}

void VlkSwapchainElement::draw(){
    const VkPolygonMode vkPolygonMode = vlkContext.vkPolygonMode;
    const VkPipelineLayout vkPipelineLayout = vlkContext.vkPipelineLayout;
    const VkDescriptorSet vkDescriptorSet = vlkContext.vkDescriptorSetUbo;

    const bool drawUi = vlkContext.drawUi;
    
    vlkBeginCommandBuffer(commandBuffer);

    vlkTransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT,
                             VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                             VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT);

    vlkTransitionImageLayout(commandBuffer, depthImage, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                             VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                             VK_ACCESS_NONE, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
      
    // Begin rendering   
    vkCmdBeginRendering(commandBuffer, &renderingInfo);
  
    vkCmdSetViewportWithCount(commandBuffer, 1, &viewport);
    vkCmdSetScissorWithCount(commandBuffer, 1, &scissor);
    vkCmdSetPolygonModeEXT(commandBuffer, vkPolygonMode);
    vkCmdSetDepthWriteEnable(commandBuffer, VK_TRUE);
    vkCmdSetDepthTestEnable(commandBuffer, VK_TRUE);
    vkCmdSetStencilTestEnable(commandBuffer, VK_FALSE);
    vkCmdSetDepthCompareOp(commandBuffer, VK_COMPARE_OP_LESS_OR_EQUAL);
    vkCmdSetRasterizerDiscardEnable(commandBuffer, false);
    vkCmdSetCullMode(commandBuffer, VK_CULL_MODE_NONE);
    vkCmdSetDepthBiasEnable(commandBuffer, false);
    vkCmdSetLineWidth(commandBuffer, 2.0f);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, 1, &vkDescriptorSet, 0, NULL);

    OnDraw(commandBuffer);
    
    if(drawUi)
      ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

    // End rendering
    vkCmdEndRendering(commandBuffer);

    vlkTransitionImageLayout(commandBuffer, image, VK_IMAGE_ASPECT_COLOR_BIT,
                             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                             VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_NONE,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    
    vlkTransitionImageLayout(commandBuffer, depthImage, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_NONE,
                             VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

    vlkEndCommandBuffer(commandBuffer);
}

void VlkSwapchainElement::setOnDraw(std::function<void(const VkCommandBuffer& commandBuffer)> fun) {
    OnDraw = fun;
}
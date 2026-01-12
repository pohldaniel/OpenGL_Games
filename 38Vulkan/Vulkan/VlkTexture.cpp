#include <iostream>
#include <vector>
#include <SOIL2/SOIL2.h>
#include "VlkContext.h"
#include "VlkTexture.h"

VlkTexture::VlkTexture(const VkDescriptorSetLayout& vkDescriptorSetLayout) :
    vkDescriptorSetLayout(vkDescriptorSetLayout),
    m_vkImage(VK_NULL_HANDLE), 
    m_vkDeviceMemory(VK_NULL_HANDLE), 
    m_vkImageView(VK_NULL_HANDLE), 
    m_vkDescriptorSet(VK_NULL_HANDLE),
    m_width(0u),
    m_height(0u),
    m_channels(0u){

}

VlkTexture::VlkTexture(VlkTexture const& rhs) : vkDescriptorSetLayout(rhs.vkDescriptorSetLayout), m_vkImage(rhs.m_vkImage), m_vkDeviceMemory(rhs.m_vkDeviceMemory), m_vkImageView(rhs.m_vkImageView), m_vkDescriptorSet(rhs.m_vkDescriptorSet), m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels) {

}

VlkTexture::VlkTexture(VlkTexture&& rhs) noexcept : vkDescriptorSetLayout(rhs.vkDescriptorSetLayout), m_vkImage(std::move(rhs.m_vkImage)), m_vkDeviceMemory(std::move(rhs.m_vkDeviceMemory)), m_vkImageView(std::move(rhs.m_vkImageView)), m_vkDescriptorSet(std::move(rhs.m_vkDescriptorSet)), m_width(rhs.m_width), m_height(rhs.m_height), m_channels(rhs.m_channels) {

}

void VlkTexture::loadFromFile(std::string fileName, const bool flipVertical) {
    int numComponents, width, height;
    unsigned char* imageData = SOIL_load_image(fileName.c_str(), &width, &height, &numComponents, SOIL_LOAD_AUTO);

    if (numComponents == 3) {

        unsigned char* bytesNew = (unsigned char*)malloc(width * height * 4);

        for (unsigned int i = 0, k = 0; i < static_cast<unsigned int>(width * height * 4); i = i + 4, k = k + 3) {
            bytesNew[i] = imageData[k];
            bytesNew[i + 1] = imageData[k + 1];
            bytesNew[i + 2] = imageData[k + 2];
            bytesNew[i + 3] = 255;
        }

        SOIL_free_image_data(imageData);
        imageData = bytesNew;
        numComponents = 4;
    }

    m_width = width;
    m_height = height;
    m_channels = numComponents;

    if (flipVertical)
        FlipVertical(imageData, numComponents * width, height);

    VkDeviceSize imageSize = width * height * numComponents;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    vlkCreateBuffer(stagingBuffer, stagingBufferMemory, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vlkMapBuffer(stagingBufferMemory, reinterpret_cast<const void*>(imageData), imageSize);
    vlkCreateImage(m_vkImage, m_vkDeviceMemory, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    const VkCommandBuffer& vkCommandBuffer = vlkContext.vkCommandBuffer;
    vlkBeginCommandBuffer(vkCommandBuffer);
    vlkTransitionImageLayout(vkCommandBuffer, m_vkImage, VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
    vlkEndCommandBuffer(vkCommandBuffer, true);
    vlkCopyBufferToImage(stagingBuffer, m_vkImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    vlkBeginCommandBuffer(vkCommandBuffer);
    vlkTransitionImageLayout(vkCommandBuffer, m_vkImage, VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    vlkEndCommandBuffer(vkCommandBuffer, true);
    vkDestroyBuffer(vlkContext.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(vlkContext.vkDevice, stagingBufferMemory, nullptr);

    vlkCreateImageView(m_vkImageView, m_vkImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, { VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY , VK_COMPONENT_SWIZZLE_IDENTITY });
    
    // Allocate Sampler for every texture
    vlkAllocateDescriptorSet(m_vkDescriptorSet, vkDescriptorSetLayout);
    vlkBindImageViewToDescriptorSet(m_vkImageView, m_vkDescriptorSet, 0u);

    SOIL_free_image_data(imageData);
}

void VlkTexture::FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height) {
    std::vector<unsigned char> srcPixels(padWidth * height);
    memcpy(&srcPixels[0], data, padWidth * height);
    unsigned char* pSrcRow = 0;
    unsigned char* pDestRow = 0;
    for (unsigned int i = 0; i < height; ++i) {
        pSrcRow = &srcPixels[(height - 1 - i) * padWidth];
        pDestRow = &data[i * padWidth];
        memcpy(pDestRow, pSrcRow, padWidth);
    }
}
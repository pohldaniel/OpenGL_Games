#include <Windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <iostream>

#include "vk_renderer.h"
#include "dds_strcuts.h"
#include "platform.h"
#include "Application.h"
#include "vk_init.h"

//PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT_ = nullptr;

#define ArraySize(arr) sizeof((arr)) / sizeof((arr[0]))

#define VK_CHECK(result)                                      \
    if (result != VK_SUCCESS)                                 \
    {                                                         \
        std::cout << "Vulkan Error: " << result << std::endl; \
        __debugbreak();                                       \
        return false;                                         \
    }

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
	VkDebugUtilsMessageTypeFlagsEXT msgFlags,
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData)
{
	std::cout << pCallbackData->pMessage << std::endl;
	return false;
}

void printStats(VkPhysicalDevice& device) {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);

	std::cout << "Name: " << properties.deviceName << std::endl;
}

bool isTypeOf(VkPhysicalDevice& device, VkPhysicalDeviceType physicalDeviceType) {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	return properties.deviceType == physicalDeviceType;
}

//https://gamedev.stackexchange.com/questions/124738/how-to-select-the-most-powerful-vkdevice
bool vk_init(VkContext *vkcontext, void *window)
{
	vkcontext->screenSize.width = Application::Width;
	vkcontext->screenSize.height = Application::Height;

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Pong";
	appInfo.pEngineName = "Ponggine";

	char *extensions[] = {
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		VK_KHR_SURFACE_EXTENSION_NAME
		//VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
		//VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME 
	};

	char *layers[]{
		"VK_LAYER_KHRONOS_validation" 
	};

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.ppEnabledExtensionNames = extensions;
	instanceInfo.enabledExtensionCount = ArraySize(extensions);
	instanceInfo.ppEnabledLayerNames = layers;
	instanceInfo.enabledLayerCount = ArraySize(layers);
	VK_CHECK(vkCreateInstance(&instanceInfo, 0, &vkcontext->instance));

	auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkcontext->instance, "vkCreateDebugUtilsMessengerEXT");

	if (vkCreateDebugUtilsMessengerEXT)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		debugInfo.pfnUserCallback = vk_debug_callback;

		vkCreateDebugUtilsMessengerEXT(vkcontext->instance, &debugInfo, 0, &vkcontext->debugMessenger);
	}
	else
	{
		return false;
	}

	// Create Surface
	{
		VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.hwnd = (HWND)window;
		surfaceInfo.hinstance = GetModuleHandleA(0);
		VK_CHECK(vkCreateWin32SurfaceKHR(vkcontext->instance, &surfaceInfo, 0, &vkcontext->surface));

	}

	// Choose GPU
	{
		vkcontext->graphicsIdx = -1;

		uint32_t gpuCount = 0;
		//TODO: Suballocation from Main Allocation
		VkPhysicalDevice gpus[10];
		VK_CHECK(vkEnumeratePhysicalDevices(vkcontext->instance, &gpuCount, 0));
		VK_CHECK(vkEnumeratePhysicalDevices(vkcontext->instance, &gpuCount, gpus));

		
		for (uint32_t i = 0; i < gpuCount; i++){
			VkPhysicalDevice gpu = gpus[i];
			//printStats(gpu);
			if (isTypeOf(gpu, VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU))
				continue;

			uint32_t queueFamilyCount = 0;
			//TODO: Suballocation from Main Allocation
			VkQueueFamilyProperties queueProps[10];
			vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, 0);
			vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueProps);

			for (uint32_t j = 0; j < queueFamilyCount; j++){
				if (queueProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT){
					VkBool32 surfaceSupport = VK_FALSE;
					VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, j, vkcontext->surface, &surfaceSupport));

					if (surfaceSupport){
						vkcontext->graphicsIdx = j;
						vkcontext->gpu = gpu;					
						break;
					}
				}
			}
		}

		if (vkcontext->graphicsIdx < 0)
		{
			return false;
		}
	}

	// Logical Device
	{
		float queuePriority = 1.0f;

		VkDeviceQueueCreateInfo queueInfo = {};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = vkcontext->graphicsIdx;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &queuePriority;

		char *extensions[] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.fillModeNonSolid = VK_TRUE;

		VkDeviceCreateInfo deviceInfo = {};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pQueueCreateInfos = &queueInfo;
		deviceInfo.queueCreateInfoCount = 1;
		deviceInfo.ppEnabledExtensionNames = extensions;
		deviceInfo.enabledExtensionCount = ArraySize(extensions);
		deviceInfo.pEnabledFeatures = &deviceFeatures;
		VK_CHECK(vkCreateDevice(vkcontext->gpu, &deviceInfo, 0, &vkcontext->device));

		// Get Graphics Queue
		vkGetDeviceQueue(vkcontext->device, vkcontext->graphicsIdx, 0, &vkcontext->graphicsQueue);
	}

	// Swapchain
	{
		uint32_t formatCount = 0;
		//TODO: Suballocation from Main Memory
		VkSurfaceFormatKHR surfaceFormats[10];
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vkcontext->gpu, vkcontext->surface, &formatCount, 0));
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(vkcontext->gpu, vkcontext->surface, &formatCount, surfaceFormats));

		for (uint32_t i = 0; i < formatCount; i++)
		{
			VkSurfaceFormatKHR format = surfaceFormats[i];

			if (format.format == VK_FORMAT_B8G8R8A8_SRGB)
			{
				vkcontext->surfaceFormat = format;
				break;
			}
		}

		VkSurfaceCapabilitiesKHR surfaceCaps = {};
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkcontext->gpu, vkcontext->surface, &surfaceCaps));
		uint32_t imgCount = surfaceCaps.minImageCount + 1;
		imgCount = imgCount > surfaceCaps.maxImageCount ? imgCount - 1 : imgCount;

		VkSwapchainCreateInfoKHR scInfo = {};
		scInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		scInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		scInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		scInfo.surface = vkcontext->surface;
		scInfo.imageFormat = vkcontext->surfaceFormat.format;
		scInfo.preTransform = surfaceCaps.currentTransform;
		scInfo.imageExtent = surfaceCaps.currentExtent;
		scInfo.minImageCount = imgCount;
		scInfo.imageArrayLayers = 1;

		VK_CHECK(vkCreateSwapchainKHR(vkcontext->device, &scInfo, 0, &vkcontext->swapchain));

		VK_CHECK(vkGetSwapchainImagesKHR(vkcontext->device, vkcontext->swapchain, &vkcontext->scImgCount, 0));
		VK_CHECK(vkGetSwapchainImagesKHR(vkcontext->device, vkcontext->swapchain, &vkcontext->scImgCount, vkcontext->scImages));

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.format = vkcontext->surfaceFormat.format;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		for (uint32_t i = 0; i < vkcontext->scImgCount; i++)
		{
			viewInfo.image = vkcontext->scImages[i];
			VK_CHECK(vkCreateImageView(vkcontext->device, &viewInfo, 0, &vkcontext->scImgViews[i]));
		}
	}

	// Render Pass
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = vkcontext->surfaceFormat.format;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription attachments[] = {
			colorAttachment };

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0; // This is an index into the attachments array
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDesc = {};
		subpassDesc.colorAttachmentCount = 1;
		subpassDesc.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		rpInfo.pAttachments = attachments;
		rpInfo.attachmentCount = ArraySize(attachments);
		rpInfo.subpassCount = 1;
		rpInfo.pSubpasses = &subpassDesc;

		VK_CHECK(vkCreateRenderPass(vkcontext->device, &rpInfo, 0, &vkcontext->renderPass));
	}

	// Frame Buffers
	{
		VkFramebufferCreateInfo fbInfo = {};
		fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbInfo.renderPass = vkcontext->renderPass;
		fbInfo.width = vkcontext->screenSize.width;
		fbInfo.height = vkcontext->screenSize.height;
		fbInfo.layers = 1;
		fbInfo.attachmentCount = 1;

		for (uint32_t i = 0; i < vkcontext->scImgCount; i++)
		{
			fbInfo.pAttachments = &vkcontext->scImgViews[i];
			VK_CHECK(vkCreateFramebuffer(vkcontext->device, &fbInfo, 0, &vkcontext->framebuffers[i]));
		}
	}

	// Command Pool
	{
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = vkcontext->graphicsIdx;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK(vkCreateCommandPool(vkcontext->device, &poolInfo, 0, &vkcontext->commandPool));
	}

	// Command Buffer
	{
		VkCommandBufferAllocateInfo allocInfo = cmd_alloc_info(vkcontext->commandPool);
		VK_CHECK(vkAllocateCommandBuffers(vkcontext->device, &allocInfo, &vkcontext->cmd));
	}

	// Sync Objects
	{
		VkSemaphoreCreateInfo semaInfo = {};
		semaInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_CHECK(vkCreateSemaphore(vkcontext->device, &semaInfo, 0, &vkcontext->aquireSemaphore));
		VK_CHECK(vkCreateSemaphore(vkcontext->device, &semaInfo, 0, &vkcontext->submitSemaphore));

		VkFenceCreateInfo fenceInfo = fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
		VK_CHECK(vkCreateFence(vkcontext->device, &fenceInfo, 0, &vkcontext->imgAvailableFence));
	}

	// Create Descriptor Set Layouts
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &binding;

		VK_CHECK(vkCreateDescriptorSetLayout(vkcontext->device, &layoutInfo, 0, &vkcontext->setLayout));
	}

	// Create Pipeline Layout
	{
		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = 1;
		layoutInfo.pSetLayouts = &vkcontext->setLayout;
		VK_CHECK(vkCreatePipelineLayout(vkcontext->device, &layoutInfo, 0, &vkcontext->pipeLayout));
	}

	// Create a Pipeline
	{

		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendState = {};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.pAttachments = &colorBlendAttachment;
		colorBlendState.attachmentCount = 1;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkViewport viewport = {};
		viewport.maxDepth = 1.0;

		VkRect2D scissor = {};

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pViewports = &viewport;
		viewportState.viewportCount = 1;
		viewportState.pScissors = &scissor;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationState.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkShaderModule vertexShader, fragmentShader;

		// Vertex Shader
		{
			uint32_t lengthInBytes;
			uint32_t *vertexCode = (uint32_t *)platform_read_file("res/shader/shader.vert.spv", &lengthInBytes);

			VkShaderModuleCreateInfo shaderInfo = {};
			shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderInfo.pCode = vertexCode;
			shaderInfo.codeSize = lengthInBytes;
			VK_CHECK(vkCreateShaderModule(vkcontext->device, &shaderInfo, 0, &vertexShader));

			delete vertexCode;
		}

		// Fragment Shader
		{
			uint32_t lengthInBytes;
			uint32_t *fragmentCode = (uint32_t *)platform_read_file("res/shader/shader.frag.spv", &lengthInBytes);

			VkShaderModuleCreateInfo shaderInfo = {};
			shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderInfo.pCode = fragmentCode;
			shaderInfo.codeSize = lengthInBytes;
			VK_CHECK(vkCreateShaderModule(vkcontext->device, &shaderInfo, 0, &fragmentShader));

			delete fragmentCode;
		}

		VkPipelineShaderStageCreateInfo vertStage = {};
		vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertStage.pName = "main";
		vertStage.module = vertexShader;

		VkPipelineShaderStageCreateInfo fragStage = {};
		fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragStage.pName = "main";
		fragStage.module = fragmentShader;

		VkPipelineShaderStageCreateInfo shaderStages[2]{
			vertStage,
			fragStage };

		VkDynamicState dynamicStates[]{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pDynamicStates = dynamicStates;
		dynamicState.dynamicStateCount = ArraySize(dynamicStates);

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.renderPass = vkcontext->renderPass;
		pipelineInfo.pVertexInputState = &vertexInputState;
		pipelineInfo.pColorBlendState = &colorBlendState;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizationState;
		pipelineInfo.pMultisampleState = &multisampleState;
		pipelineInfo.stageCount = ArraySize(shaderStages);
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = vkcontext->pipeLayout;
		pipelineInfo.pStages = shaderStages;

		VK_CHECK(vkCreateGraphicsPipelines(vkcontext->device, 0, 1, &pipelineInfo, nullptr, &vkcontext->solid));

		// Wire frame rendering pipeline
		//if (deviceFeatures.fillModeNonSolid) {
			rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
			rasterizationState.lineWidth = 1.0f;
			VK_CHECK(vkCreateGraphicsPipelines(vkcontext->device, 0, 1, &pipelineInfo, nullptr, &vkcontext->wireframe));
		//}

		vkDestroyShaderModule(vkcontext->device, vertexShader, 0);
		vkDestroyShaderModule(vkcontext->device, fragmentShader, 0);
	}

	// Staging Buffer
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.size = MB(1);
		VK_CHECK(vkCreateBuffer(vkcontext->device, &bufferInfo, 0, &vkcontext->stagingBuffer.buffer));

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vkcontext->device, vkcontext->stagingBuffer.buffer, &memRequirements);

		VkPhysicalDeviceMemoryProperties gpuMemProps;
		vkGetPhysicalDeviceMemoryProperties(vkcontext->gpu, &gpuMemProps);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = MB(1);
		for (uint32_t i = 0; i < gpuMemProps.memoryTypeCount; i++)
		{
			if (memRequirements.memoryTypeBits & (1 << i) &&
				(gpuMemProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
				(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
			{
				allocInfo.memoryTypeIndex = i;
			}
		}

		VK_CHECK(vkAllocateMemory(vkcontext->device, &allocInfo, 0, &vkcontext->stagingBuffer.memory));
		VK_CHECK(vkMapMemory(vkcontext->device, vkcontext->stagingBuffer.memory, 0, MB(1), 0, &vkcontext->stagingBuffer.data));
		VK_CHECK(vkBindBufferMemory(vkcontext->device, vkcontext->stagingBuffer.buffer, vkcontext->stagingBuffer.memory, 0));
	}

	// Create Image
	{
		uint32_t fileSize;
		DDSFile *data = (DDSFile *)platform_read_file("res/textures/cakez.DDS", &fileSize);
		uint32_t textureSize = data->header.Width * data->header.Height * 4;

		memcpy(vkcontext->stagingBuffer.data, &data->dataBegin, textureSize);

		//TODO: Assertions

		VkImageCreateInfo imgInfo = {};
		imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgInfo.mipLevels = 1;
		imgInfo.arrayLayers = 1;
		imgInfo.imageType = VK_IMAGE_TYPE_2D;
		imgInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imgInfo.extent = { data->header.Width, data->header.Height, 1 };
		imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		// imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VK_CHECK(vkCreateImage(vkcontext->device, &imgInfo, 0, &vkcontext->image.image));

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vkcontext->device, vkcontext->image.image, &memRequirements);

		VkPhysicalDeviceMemoryProperties gpuMemProps;
		vkGetPhysicalDeviceMemoryProperties(vkcontext->gpu, &gpuMemProps);

		VkMemoryAllocateInfo allocInfo = {};
		for (uint32_t i = 0; i < gpuMemProps.memoryTypeCount; i++)
		{
			if (memRequirements.memoryTypeBits & (1 << i) &&
				(gpuMemProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			{
				allocInfo.memoryTypeIndex = i;
			}
		}

		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = textureSize;
		VK_CHECK(vkAllocateMemory(vkcontext->device, &allocInfo, 0, &vkcontext->image.memory));
		VK_CHECK(vkBindImageMemory(vkcontext->device, vkcontext->image.image, vkcontext->image.memory, 0));

		VkCommandBuffer cmd;
		VkCommandBufferAllocateInfo cmdAlloc = cmd_alloc_info(vkcontext->commandPool);
		VK_CHECK(vkAllocateCommandBuffers(vkcontext->device, &cmdAlloc, &cmd));

		VkCommandBufferBeginInfo beginInfo = cmd_begin_info();
		VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

		VkImageSubresourceRange range = {};
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.layerCount = 1;
		range.levelCount = 1;

		// Transition Layout to Transfer optimal
		VkImageMemoryBarrier imgMemBarrier = {};
		imgMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imgMemBarrier.image = vkcontext->image.image;
		imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		imgMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imgMemBarrier.subresourceRange = range;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0,
			1, &imgMemBarrier);

		VkBufferImageCopy copyRegion = {};
		copyRegion.imageExtent = { data->header.Width, data->header.Height, 1 };
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vkCmdCopyBufferToImage(cmd, vkcontext->stagingBuffer.buffer, vkcontext->image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		imgMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imgMemBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imgMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imgMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0,
			1, &imgMemBarrier);

		VK_CHECK(vkEndCommandBuffer(cmd));

		VkFence uploadFence;
		VkFenceCreateInfo fenceInfo = fence_info();
		VK_CHECK(vkCreateFence(vkcontext->device, &fenceInfo, 0, &uploadFence));

		VkSubmitInfo submitInfo = submit_info(&cmd);
		VK_CHECK(vkQueueSubmit(vkcontext->graphicsQueue, 1, &submitInfo, uploadFence));

		VK_CHECK(vkWaitForFences(vkcontext->device, 1, &uploadFence, true, UINT64_MAX));
	}

	// Create Image View
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = vkcontext->image.image;
		viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

		VK_CHECK(vkCreateImageView(vkcontext->device, &viewInfo, 0, &vkcontext->image.view));
	}

	// Create Sampler
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.magFilter = VK_FILTER_NEAREST;

		VK_CHECK(vkCreateSampler(vkcontext->device, &samplerInfo, 0, &vkcontext->sampler));
	}

	// Create Descriptor Pool
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.maxSets = 1;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		VK_CHECK(vkCreateDescriptorPool(vkcontext->device, &poolInfo, 0, &vkcontext->descPool));
	}

	// Create Descriptor Set
	{
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pSetLayouts = &vkcontext->setLayout;
		allocInfo.descriptorSetCount = 1;
		allocInfo.descriptorPool = vkcontext->descPool;
		VK_CHECK(vkAllocateDescriptorSets(vkcontext->device, &allocInfo, &vkcontext->descSet));
	}

	// Update Descriptor Set
	{
		VkDescriptorImageInfo imgInfo = {};
		imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imgInfo.imageView = vkcontext->image.view;
		imgInfo.sampler = vkcontext->sampler;

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = vkcontext->descSet;
		write.pImageInfo = &imgInfo;
		write.dstBinding = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		vkUpdateDescriptorSets(vkcontext->device, 1, &write, 0, 0);
	}

	return true;
}

bool vk_render(VkContext *vkcontext)
{
	uint32_t imgIdx;

	// We wait on the GPU to be done with the work
	VK_CHECK(vkWaitForFences(vkcontext->device, 1, &vkcontext->imgAvailableFence, VK_TRUE, UINT64_MAX));
	VK_CHECK(vkResetFences(vkcontext->device, 1, &vkcontext->imgAvailableFence));

	// This waits on the timeout until the image is ready, if timeout reached -> VK_TIMEOUT
	VK_CHECK(vkAcquireNextImageKHR(vkcontext->device, vkcontext->swapchain, UINT64_MAX, vkcontext->aquireSemaphore, 0, &imgIdx));

	VkCommandBuffer cmd = vkcontext->cmd;
	vkResetCommandBuffer(cmd, 0);

	VkCommandBufferBeginInfo beginInfo = cmd_begin_info();
	VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

	// Clear Color to Yellow
	VkClearValue clearValue = {};
	clearValue.color = { 1, 1, 0, 1 };

	VkRenderPassBeginInfo rpBeginInfo = {};
	rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpBeginInfo.renderArea.extent = vkcontext->screenSize;
	rpBeginInfo.clearValueCount = 1;
	rpBeginInfo.pClearValues = &clearValue;
	rpBeginInfo.renderPass = vkcontext->renderPass;
	rpBeginInfo.framebuffer = vkcontext->framebuffers[imgIdx];
	vkCmdBeginRenderPass(cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Rendering Commands
	{
		VkViewport viewport = {};
		viewport.maxDepth = 1.0f;
		viewport.width = vkcontext->screenSize.width;
		viewport.height = vkcontext->screenSize.height;

		VkRect2D scissor = {};
		scissor.extent = vkcontext->screenSize;

		vkCmdSetViewport(cmd, 0, 1, &viewport);
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkcontext->pipeLayout,
			0, 1, &vkcontext->descSet, 0, 0);

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkcontext->enableWireFrame ? vkcontext->wireframe : vkcontext->solid);
		vkCmdDraw(cmd, 6, 1, 0, 0);
		// Actually draw
		// Bind pipe
		// Bind desc
		// Draw
		// Reapeat
	}

	vkCmdEndRenderPass(cmd);

	VK_CHECK(vkEndCommandBuffer(cmd));

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	// This call will signal the Fence when the GPU Work is done
	VkSubmitInfo submitInfo = submit_info(&cmd);
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.pSignalSemaphores = &vkcontext->submitSemaphore;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &vkcontext->aquireSemaphore;
	submitInfo.waitSemaphoreCount = 1;
	VK_CHECK(vkQueueSubmit(vkcontext->graphicsQueue, 1, &submitInfo, vkcontext->imgAvailableFence));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pSwapchains = &vkcontext->swapchain;
	presentInfo.swapchainCount = 1;
	presentInfo.pImageIndices = &imgIdx;
	presentInfo.pWaitSemaphores = &vkcontext->submitSemaphore;
	presentInfo.waitSemaphoreCount = 1;
	VK_CHECK(vkQueuePresentKHR(vkcontext->graphicsQueue, &presentInfo));

	return true;
}

void setWireFrame(VkContext *vkcontext, bool flag) {
	vkcontext->enableWireFrame = flag;
}
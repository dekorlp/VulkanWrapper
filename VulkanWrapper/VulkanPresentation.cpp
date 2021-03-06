#include "vulkanpresentation.h"


void CVulkanPresentation::CreateSurface(CVulkanInstance* instance, HWND hwnd, SDL_Window* window)
{
	m_Instance = instance;
	m_Window = window;

	VkSurfaceKHR surface;
	if (!SDL_Vulkan_CreateSurface(window, instance->GetInstance(), &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	instance->SetSurface(surface);
}

void CVulkanPresentation::DestroySurface()
{
	vkDestroySurfaceKHR(m_Instance->GetInstance(), m_Instance->GetSurface() , nullptr);
}

VkSurfaceKHR CVulkanPresentation::GetSurface()
{
	return m_Instance->GetSurface();
}

VkSurfaceFormatKHR CVulkanPresentation::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR CVulkanPresentation::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D CVulkanPresentation::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, unsigned int width, unsigned int height) {

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {

		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void CVulkanPresentation::CreateSwapChain(unsigned int width, unsigned int height)
{
	
	//m_LogicalDevice = logicalDevice;
	SSwapChainSupportDetails swapChainSupport = CVulkanUtils::QuerySwapChainSupport(m_Instance->GetPhysicalDevice(), m_Instance->GetSurface());

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, width, height);

	m_Instance->SetSwapchainExtend(extent);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Instance->GetSurface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	SQueueFamilyIndices indices = CVulkanUtils::findQueueFamilies(m_Instance->GetPhysicalDevice(), m_Instance->GetSurface());
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;
	VkSwapchainKHR swapChain;
	if (vkCreateSwapchainKHR(m_Instance->GetLogicalDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}
	m_Instance->SetSwapchain(swapChain);

	std::vector<VkImage> swapChainImages;
	vkGetSwapchainImagesKHR(m_Instance->GetLogicalDevice(), m_Instance->GetSwapchain(), &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_Instance->GetLogicalDevice(), m_Instance->GetSwapchain(), &imageCount, swapChainImages.data());
	m_Instance->SetSwapchainImages(swapChainImages);


	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void CVulkanPresentation::DestroySwapChain()
{
	vkDestroySwapchainKHR(m_Instance->GetLogicalDevice(), m_Instance->GetSwapchain(), nullptr);
}

void CVulkanPresentation::CreateImageViews()
{
	std::vector<VkImageView> swapChainImageViews;
	swapChainImageViews.resize(m_Instance->GetSwapchainImages().size());

	for (size_t i = 0; i < m_Instance->GetSwapchainImages().size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_Instance->GetSwapchainImages()[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_Instance->GetLogicalDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}

	m_Instance->SetSwapchainImageViews(swapChainImageViews);

}

void CVulkanPresentation::CreateRenderPass() {
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = GetSwapChainImageFormat();
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


	//VkPipelineLayout pipelineLayout;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	VkRenderPass renderPass;
	if (vkCreateRenderPass(m_Instance->GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
	m_Instance->SetRenderPass(renderPass);
}

void CVulkanPresentation::DestroyRenderPass()
{
	vkDestroyRenderPass(m_Instance->GetLogicalDevice(), m_Instance->GetRenderPass(), nullptr);
}

VkRenderPass CVulkanPresentation::GetRenderPass()
{
	return m_Instance->GetRenderPass();
}


void CVulkanPresentation::DestroyImageViews()
{
	for (auto imageView : m_Instance->GetSwapchainImageViews()) {
		vkDestroyImageView(m_Instance->GetLogicalDevice(), imageView, nullptr);
	}
}

VkExtent2D CVulkanPresentation::GetSwapChainExtend()
{
	return this->swapChainExtent;
}

VkFormat CVulkanPresentation::GetSwapChainImageFormat()
{
	return swapChainImageFormat;
}

std::vector<VkImageView> CVulkanPresentation::GetSwapChainImageViews()
{
	return m_Instance->GetSwapchainImageViews();
}

VkSwapchainKHR CVulkanPresentation::GetSwapChain()
{
	return m_Instance->GetSwapchain();
}

std::vector<VkImage> CVulkanPresentation::GetSwapChainImages()
{
	return m_Instance->GetSwapchainImages();
}

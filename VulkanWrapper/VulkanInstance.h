#pragma once
#include <string>
#include <vulkan/vulkan.h>
//#include <vector>

#include "VulkanValidationLayers.h"

struct SVulkanExtensions
{
	const std::vector<const char*> InstanceExtensions = {

	};
	const std::vector<const char*> logicalDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};
};

class CVulkanInstance
{
public:
	VkInstance CreateVulkanInstance(std::string applicationName, uint32_t applicationVersion, uint32_t vulkanVersion, uint32_t sdlExtensionCount, const char** sdlExtensions);
	void destroyVulkanInstance();
	std::vector<VkExtensionProperties> getExtensions();
	std::vector<const char*> getExtensionsStr();
	VkInstance GetInstance();
	const bool IsvalidationLayerEnabled();
	const std::vector<const char*> GetValidationLayers();

	//////////////////////////////////////////
	void SetPhysicalDevice(VkPhysicalDevice physicalDevice);
	VkPhysicalDevice GetPhysicalDevice();

	void SetLogicalDevice(VkDevice logicalDevice);
	VkDevice GetLogicalDevice();

	void SetSurface(VkSurfaceKHR surface);
	VkSurfaceKHR GetSurface();

	void SetSwapchainExtend(VkExtent2D swapchainExtend);
	VkExtent2D GetSwapchainExtend();

	void SetRenderPass(VkRenderPass renderPass);
	VkRenderPass GetRenderPass();

	void SetGraphicsQueue(VkQueue graphicsQueue);
	VkQueue GetGraphicsQueue();

	void SetPresentQueue(VkQueue presentQueue);
	VkQueue GetPresentQueue();

	void SetSwapChainImageViews(std::vector<VkImageView> swapchainImageViews);
	std::vector<VkImageView> GetSwapChainImageView();

	void SetSwapchain(VkSwapchainKHR swapchain);
	VkSwapchainKHR GetSwapchain();

	void SetSwapchainImages(std::vector<VkImage> images);
	std::vector<VkImage> GetSwapchainImages();

	void SetSwapchainFrameBuffers(std::vector<VkFramebuffer> framebuffers);
	std::vector<VkFramebuffer> GetSwapchainFrameBuffers();

	void SetSelectedImageIndexToDraw(uint32_t selectedImageIndexToDraw);
	uint32_t GetSelectedImageIndexToDraw();

	CVulkanInstance& operator=(CVulkanInstance const* rhs);


private: 
	VkInstance instance;
	std::vector<VkExtensionProperties> extensions;
	std::vector<const char*> extensionStr;
	CVulkanValidationLayers validationLayers;
	bool areValidationLayersEnables;


	///////////////////////////////////////////
	VkPhysicalDevice m_PhysicalDevice;
	VkSurfaceKHR m_Surface;
	VkExtent2D m_SwapChainExtent;
	VkRenderPass m_renderPass;
	VkDevice m_LogicalDevice;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	std::vector<VkImageView> swapChainImageViews;
	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	uint32_t m_SelectedImageIndexToDraw;

	
};
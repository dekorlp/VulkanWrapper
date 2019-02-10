#pragma once
#include <string>
#include <vulkan/vulkan.h>

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

	void SetSwapchainImageViews(std::vector<VkImageView> swapChainImageViews);
	std::vector<VkImageView> GetSwapchainImageViews();

	void SetSwapchain(VkSwapchainKHR swapChain);
	VkSwapchainKHR GetSwapchain();

	void SetSwapchainImages(std::vector<VkImage> swapChainImages);
	std::vector<VkImage> GetSwapchainImages();

	void SetSelectedImageToDraw(uint32_t selectedImageToDraw);
	uint32_t GetSelectedImageToDraw();

	void SetCommandPool(VkCommandPool commandPool);
	VkCommandPool GetCommandPool();

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
	VkCommandPool m_CommandPool;

	std::vector<VkImageView> swapChainImageViews;
	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> swapChainImages;

	uint32_t m_SelectedImageToDraw;
};
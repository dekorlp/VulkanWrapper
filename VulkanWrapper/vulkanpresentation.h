#pragma once
#include "SDL_Vulkan.h"
#include "SDL_syswm.h"
#include<algorithm>

#undef max
#undef min

#include <vulkan/vulkan.h>
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanQueueFamilie.h"



class CVulkanPresentation
{
public:
	void CreateSurface(CVulkanInstance* instance, HWND hwnd, SDL_Window* window);
	void DestroySurface();
	VkSurfaceKHR GetSurface();
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, unsigned int width, unsigned int height);
	void CreateSwapChain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, unsigned int width, unsigned int height);
	void DestroySwapChain();
	void CreateImageViews();
	void DestroyImageViews();
	VkExtent2D GetSwapChainExtend();
	VkFormat GetSwapChainImageFormat();
	std::vector<VkImageView> GetSwapChainImageViews();
	std::vector<VkImage> GetSwapChainImages();
	VkSwapchainKHR GetSwapChain();
	void CreateRenderPass();
	void DestroyRenderPass();
	VkRenderPass GetRenderPass();

private:
	//VkSurfaceKHR m_Surface;
	CVulkanInstance* m_Instance;
	//VkInstance m_Instance;
	VkSwapchainKHR m_SwapChain;
	VkDevice m_LogicalDevice;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	SDL_Window* m_Window;
	//VkRenderPass renderPass;
};
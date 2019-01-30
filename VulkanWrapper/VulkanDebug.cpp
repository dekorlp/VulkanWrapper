#include "VulkanDebug.h"

void CVulkanDebug::SetupVulkanDebug(CVulkanInstance* instance)
{
	m_Instance = instance->GetInstance();
	m_AreValidationLayersEnables = instance->IsvalidationLayerEnabled();
}

void CVulkanDebug::SetupDebugCallback( void* debugCallback)
{
	if (!m_AreValidationLayersEnables) return; // do not debug in release mode
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)debugCallback;
	createInfo.pUserData = nullptr; // Optional

	VkDebugUtilsMessengerEXT callback;
	if (CreateDebugUtilsMessengerEXT(&createInfo, nullptr, &callback) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
	}
	m_DebugHandler.push_back(callback);
}

VkResult CVulkanDebug::CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(m_Instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void CVulkanDebug::DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator) {
	if (!m_AreValidationLayersEnables) return; // do not debug in release mode

	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");

	for (const auto& debugHandler : m_DebugHandler)
	{
		if (func != nullptr) {
			func(m_Instance, debugHandler, pAllocator);
		}
	}
}
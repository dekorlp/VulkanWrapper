#include "VulkanInstance.h"

VkInstance CVulkanInstance::CreateVulkanInstance(std::string applicationName, uint32_t applicationVersion, uint32_t vulkanVersion, uint32_t sdlExtensionCount, const char** sdlExtensions)
{
#ifdef NDEBUG
	areValidationLayersEnables = false;
#else
	areValidationLayersEnables = true;
#endif

	if (areValidationLayersEnables && !validationLayers.CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = applicationName.c_str();
	appInfo.applicationVersion = applicationVersion;
	appInfo.apiVersion = vulkanVersion;
	appInfo.pEngineName = "Pyrogenesis";
	appInfo.engineVersion = applicationVersion;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	const std::vector<const char*> validationLayersVector = validationLayers.GetValidationLayers();
	if (areValidationLayersEnables) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayersVector.size());
		createInfo.ppEnabledLayerNames = validationLayersVector.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	auto SDLExtensions = validationLayers.GetRequiredExtensions(sdlExtensionCount, sdlExtensions, areValidationLayersEnables);
	SVulkanExtensions vulkanExtensions;
	for (auto extension : vulkanExtensions.InstanceExtensions)
	{
		SDLExtensions.push_back(extension);
	}

	extensionStr = SDLExtensions;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(SDLExtensions.size());
	createInfo.ppEnabledExtensionNames = SDLExtensions.data();

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	extensions = std::vector<VkExtensionProperties>(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	
	



	return instance;
}

void CVulkanInstance::destroyVulkanInstance()
{
	vkDestroyInstance(instance, nullptr);
}

std::vector<VkExtensionProperties> CVulkanInstance::getExtensions()
{
	return this->extensions;
}

std::vector<const char*> CVulkanInstance::getExtensionsStr()
{
	return this->extensionStr;
}

VkInstance CVulkanInstance::GetInstance()
{
	return this->instance;
}

const bool CVulkanInstance::IsvalidationLayerEnabled()
{
	return areValidationLayersEnables;
}

const std::vector<const char*> CVulkanInstance::GetValidationLayers()
{
	return validationLayers.GetValidationLayers();
}

void CVulkanInstance::SetPhysicalDevice(VkPhysicalDevice physicalDevice)
{
	this->m_PhysicalDevice = physicalDevice;
}

VkPhysicalDevice CVulkanInstance::GetPhysicalDevice()
{
	return this->m_PhysicalDevice;
}
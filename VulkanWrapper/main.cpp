#include <iostream>
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_Vulkan.h"
#include <fstream>
#include <vulkan/vulkan.h>
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanPresentation.h"
#include "VulkanPipeline.h"
#include "VulkanDrawing.h"
#include "VulkanDebug.h"
#include "VulkanMesh.h"
#include "CustomVertex.h"
#include <vector>



// Debug callback Handler
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

class VulkanSampleApplication {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

private:

	void initWindow() {
		width = 800;
		height = 800;
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
		window = SDL_CreateWindow("Vulkan Sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_VULKAN);
		SDL_SetWindowResizable(window, SDL_TRUE);
		
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(window, &info);
		isRunning = true;
	}

	void initVulkan() {

		uint32_t sdlExtensionCount = 0;
		const char** sdlExtensions = nullptr;

		SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr);

		sdlExtensions = new const char *[sdlExtensionCount];
		SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, sdlExtensions);
		
		instance = vulkanInstance.CreateVulkanInstance("Vulkan Sample", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_1, sdlExtensionCount, sdlExtensions);

		
		std::cout << "available extensions:" << std::endl;

		for (const auto& extension : vulkanInstance.getExtensions()) {
			std::cout << "\t" << extension.extensionName << std::endl;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////
		vulkanDebug.SetupVulkanDebug(&vulkanInstance);
		vulkanDebug.SetupDebugCallback(DebugCallback);
		vulkanPresentation.CreateSurface(vulkanInstance, info.info.win.window, window);
		vulkanPhysicalDevice.SelectPhysicalDevice(&vulkanInstance, vulkanPresentation.GetSurface());
		vulkanLogicalDevice.CreateLogicalDevice(vulkanInstance, vulkanPhysicalDevice, vulkanPresentation);
		vulkanPresentation.CreateSwapChain(vulkanPhysicalDevice.GetPhysicalDevice(), vulkanLogicalDevice.getDevice(), width, height);
		vulkanPresentation.CreateImageViews();
		vulkanPresentation.CreateRenderPass();

		vulkanDrawing.Init(vulkanPhysicalDevice, vulkanLogicalDevice, vulkanPresentation);
		vulkanDrawing.CreateFrameBuffers();
		vulkanDrawing.CreateCommandPool();

		CCustomVertex vertex;

		vulkanPipeline.InitVulkanPipeline(vulkanLogicalDevice, vulkanPresentation);
		vulkanPipeline.CreateDescriptorSetLayout();
		vulkanPipeline.CreateGraphicsPipeline(readFile("shader/Descriptor/vert.spv"), readFile("shader/Descriptor/frag.spv"), vertex);

		plane.Init(vulkanPhysicalDevice, vulkanLogicalDevice, vulkanPresentation, vulkanDrawing.GetCommandPool());

		plane.CreateVertexBuffer(
		{
		{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
		{ { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f } },
		});

		plane.createIndexBuffer({
			0, 1, 2, 2, 3, 0,
		});

		plane.CreateUniformBuffer();
		plane.CreateDescriptorPool();
		plane.CreateDescriptorSet(vulkanPipeline);
		plane.CreateSecondaryCommandBuffers(vulkanPresentation, vulkanPipeline, vulkanDrawing.getFramebuffers());

		
		vulkanDrawing.RegisterMesh(plane);

		vulkanDrawing.CreateCommandBuffers();
		vulkanDrawing.CreateSyncObjects();	
	}


	void recreateSwapChain(int resizeWith, int resizeHeight)
	{
		vkDeviceWaitIdle(vulkanLogicalDevice.getDevice());

		vulkanDrawing.DestroyFrameBuffers();
		vulkanDrawing.DestroyCommandBuffers();
		plane.DestroySecondaryCommandBuffer();
		vulkanPresentation.DestroyRenderPass();
		vulkanPresentation.DestroyImageViews();
		vulkanPresentation.DestroySwapChain();
		
		int width;
		int height;
		SDL_Vulkan_GetDrawableSize(window, &width, &height);

		vulkanPresentation.CreateSwapChain(vulkanPhysicalDevice.GetPhysicalDevice(), vulkanLogicalDevice.getDevice(), width, height);
		vulkanPresentation.CreateImageViews();
		vulkanPresentation.CreateRenderPass();

		vulkanDrawing.Init(vulkanPhysicalDevice, vulkanLogicalDevice, vulkanPresentation);
		vulkanDrawing.CreateFrameBuffers();

		plane.CreateSecondaryCommandBuffers(vulkanPresentation, vulkanPipeline, vulkanDrawing.getFramebuffers());
		
		vulkanDrawing.RegisterMesh(plane);
		vulkanDrawing.CreateCommandBuffers();
		
	

	}

	void mainLoop() {
		SDL_Event evt;
		while (isRunning)
		{
			while (SDL_PollEvent(&evt))
			{
				if (evt.type == SDL_QUIT)
				{
					isRunning = false;

				}
				if (evt.type == SDL_WINDOWEVENT)
				{
					if (evt.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						int w;
						int h;

						SDL_Vulkan_GetDrawableSize(window, &w, &h);
						recreateSwapChain(w, h);
					}
					if (evt.window.event == SDL_WINDOWEVENT_MINIMIZED)
					{
						int w = 0, h = 0;
						while (w == 0 || h == 0)
						{
							SDL_WaitEvent(&evt);

							if (evt.window.event == SDL_WINDOWEVENT_RESTORED)
							{
								SDL_Vulkan_GetDrawableSize(window, &w, &h);
							}
						}
					}
					
				}
			}
			
			vulkanDrawing.Draw();
			plane.UpdateUniformBuffers();
		}

		// wait until operations are finished
		vkDeviceWaitIdle(vulkanLogicalDevice.getDevice());
	}

	void cleanup() {


		vulkanDrawing.DestroySemaphores();
		plane.DestroySecondaryCommandBuffer();
		vulkanDrawing.DestroyCommandBuffers();
		vulkanDrawing.DestroyCommandPool();
		plane.DestroyDescriptorPool();
		plane.DestroyUniformBuffers();
		plane.DestroyIndexBuffer();
		plane.DestroyVertexBuffer();
		vulkanDrawing.DestroyFrameBuffers();
		vulkanPipeline.DestroyPipeline();
		vulkanPipeline.DestroyDescriptorSetLayout();
		vulkanPresentation.DestroyRenderPass();
		vulkanPresentation.DestroyImageViews();
		vulkanPresentation.DestroySwapChain();
		vulkanLogicalDevice.DestroyLogicalDevice();
		vulkanPresentation.DestroySurface();
		vulkanDebug.DestroyDebugUtilsMessengerEXT(nullptr);
		vulkanInstance.destroyVulkanInstance();
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	SDL_Window *window;
	SDL_SysWMinfo info;
	bool isRunning;
	CVulkanInstance vulkanInstance;
	CVulkanDebug vulkanDebug;
	CVulkanPhysicalDevice vulkanPhysicalDevice;
	CVulkanLogicalDevice vulkanLogicalDevice;
	CVulkanPresentation vulkanPresentation;
	CVulkanPipeline vulkanPipeline;
	CVulkanDrawing vulkanDrawing;
	CVulkanMesh plane;

	VkInstance instance;
	int  width;
	int height;

};

int main(int argc, char *args[])
{
	VulkanSampleApplication app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return 0;
}


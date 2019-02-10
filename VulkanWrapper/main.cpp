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
#include "SUniformBufferObject.h"
#include "VulkanUniform.h"
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
		vulkanDebug.SetupVulkanDebug(vulkanInstance);
		vulkanDebug.SetupDebugCallback(DebugCallback);
		vulkanPresentation.CreateSurface(&vulkanInstance, info.info.win.window, window);
		vulkanPhysicalDevice.SelectPhysicalDevice(&vulkanInstance);
		vulkanLogicalDevice.CreateLogicalDevice(&vulkanInstance);
		vulkanPresentation.CreateSwapChain(width, height);
		vulkanPresentation.CreateImageViews();
		vulkanPresentation.CreateRenderPass();

		vulkanDrawing.Init(&vulkanInstance);
		vulkanDrawing.CreateFrameBuffers();
		vulkanDrawing.CreateCommandPool();

		vulkanPipeline.InitVulkanPipeline(&vulkanInstance);
		vulkanPipeline.AddShader(readFile("shader/Descriptor/vert.spv"), VK_SHADER_STAGE_VERTEX_BIT);
		vulkanPipeline.AddShader(readFile("shader/Descriptor/frag.spv"), VK_SHADER_STAGE_FRAGMENT_BIT);

		uniform1.CreateUniform(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		vulkanPipeline.AddUniform(uniform1);
		vulkanPipeline.CreateDescriptorSetLayouts();
		vulkanPipeline.CreateGraphicsPipeline(CCustomVertex());

		plane.Init(&vulkanInstance, &vulkanPipeline);
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

		

		plane.CreateUniformBuffer(&uniform1, sizeof(UniformBufferObject));
		plane.CreateDescriptorPool();
		plane.CreateDescriptorSet();
		plane.CreateDescriptorWrite(&uniform1, sizeof(UniformBufferObject));
		plane.CreateSecondaryCommandBuffers();

		
		vulkanDrawing.RegisterMesh(&plane);

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

		vulkanPresentation.CreateSwapChain(width, height);
		vulkanPresentation.CreateImageViews();
		vulkanPresentation.CreateRenderPass();

		vulkanDrawing.Init(&vulkanInstance);
		vulkanDrawing.CreateFrameBuffers();

		plane.CreateSecondaryCommandBuffers();
		
		vulkanDrawing.RegisterMesh(&plane);
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
			
			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			UniformBufferObject ubo = {};
			ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			ubo.proj = glm::perspective(glm::radians(45.0f), vulkanInstance.GetSwapchainExtend().width / (float)vulkanInstance.GetSwapchainExtend().height, 0.1f, 10.0f);

			ubo.proj[1][1] *= -1;

			vulkanDrawing.DrawFrame();
			plane.UpdateUniformBuffer(uniform1, &ubo, sizeof(ubo));
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
		plane.DestroyUniformBuffers(&uniform1);
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
	CVulkanUniform uniform1;

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


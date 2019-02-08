#include "VulkanDrawing.h"

void CVulkanDrawing::Init(CVulkanInstance* instance)
{
	m_Instance = instance;
	m_VulkanMesh.clear();
}

void CVulkanDrawing::CreateFrameBuffers()
{
	swapChainFramebuffers.resize(m_Instance->GetSwapchainImageViews().size());

	for (size_t i = 0; i < m_Instance->GetSwapchainImageViews().size(); i++) {
		VkImageView attachments[] = {
			m_Instance->GetSwapchainImageViews()[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_Instance->GetRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_Instance->GetSwapchainExtend().width;
		framebufferInfo.height = m_Instance->GetSwapchainExtend().height;
		framebufferInfo.layers = 1;
		if (vkCreateFramebuffer(m_Instance->GetLogicalDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void CVulkanDrawing::DestroyFrameBuffers()
{
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(m_Instance->GetLogicalDevice(), framebuffer, nullptr);
	}
}

void CVulkanDrawing::CreateCommandPool()
{
	SQueueFamilyIndices queueFamilyIndices = CVulkanQueueFamily::findQueueFamilies(m_Instance->GetPhysicalDevice(), m_Instance->GetSurface());

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(m_Instance->GetLogicalDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void CVulkanDrawing::DestroyCommandPool()
{
	vkDestroyCommandPool(m_Instance->GetLogicalDevice(), commandPool, nullptr);
}

void CVulkanDrawing::CreateCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(m_Instance->GetLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_Instance->GetRenderPass();
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_Instance->GetSwapchainExtend();
		
		// background color
		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		
		for (unsigned int j = 0; j < m_VulkanMesh.size(); j++)
		{
			vkCmdExecuteCommands(commandBuffers[i], 1, m_VulkanMesh.at(j).GetCommandBuffer());
		}
			
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

	}
}

void CVulkanDrawing::DestroyCommandBuffers()
{
	vkFreeCommandBuffers(m_Instance->GetLogicalDevice(), commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
}

void CVulkanDrawing::CreateSemaphores()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(m_Instance->GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_Instance->GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create semaphores for a frame!");
		}
	}
}

void CVulkanDrawing::DestroySemaphores()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(m_Instance->GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_Instance->GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_Instance->GetLogicalDevice(), inFlightFences[i], nullptr);
	}
}

void CVulkanDrawing::RegisterMesh(CVulkanMesh* mesh)
{
	unsigned int vectorIndex = m_VulkanMesh.size();
	mesh->SetVectorIndex(vectorIndex);
	m_VulkanMesh.push_back(*mesh);
}

void CVulkanDrawing::UnregisterMesh(CVulkanMesh* mesh)
{
	DestroyCommandBuffers();
	
	std::vector<CVulkanMesh>::iterator position = std::find(m_VulkanMesh.begin(), m_VulkanMesh.end(), mesh);
	if (position != m_VulkanMesh.end())
	{
		m_VulkanMesh.erase(std::find(m_VulkanMesh.begin(), m_VulkanMesh.end(), mesh));
	}

	CreateCommandBuffers();
}



void CVulkanDrawing::DrawFrame()
{
	vkWaitForFences(m_Instance->GetLogicalDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(m_Instance->GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(m_Instance->GetLogicalDevice(), m_Instance->GetSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	for(unsigned int i = 0; i < m_VulkanMesh.size(); i++)
	{
		m_Instance->SetSelectedImageToDraw(imageIndex);
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(m_Instance->GetLogicalDevice(), 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(m_Instance->GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	
	VkSwapchainKHR swapChains[] = { m_Instance->GetSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	vkQueuePresentKHR(m_Instance->GetPresentQueue(), &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	
}

void CVulkanDrawing::CreateSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(m_Instance->GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_Instance->GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_Instance->GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

VkCommandPool CVulkanDrawing::GetCommandPool()
{
	return commandPool;
}

std::vector<VkFramebuffer> CVulkanDrawing::getFramebuffers()
{
	return swapChainFramebuffers;
}
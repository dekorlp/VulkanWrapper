#include "VulkanDrawing.h"

void CVulkanDrawing::Init(CVulkanPhysicalDevice physicalDevice, CVulkanLogicalDevice logicalDevice, CVulkanPresentation presentation)
{
	m_LogicalDevice = logicalDevice;
	m_Presentation = presentation;
	m_PhysicalDevice = physicalDevice;
	m_VulkanMesh.clear();
}

void CVulkanDrawing::CreateFrameBuffers()
{
	swapChainFramebuffers.resize(m_Presentation.GetSwapChainImageViews().size());

	for (size_t i = 0; i < m_Presentation.GetSwapChainImageViews().size(); i++) {
		VkImageView attachments[] = {
			m_Presentation.GetSwapChainImageViews()[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_Presentation.GetRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_Presentation.GetSwapChainExtend().width;
		framebufferInfo.height = m_Presentation.GetSwapChainExtend().height;
		framebufferInfo.layers = 1;
		if (vkCreateFramebuffer(m_LogicalDevice.getDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void CVulkanDrawing::DestroyFrameBuffers()
{
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(m_LogicalDevice.getDevice(), framebuffer, nullptr);
	}
}

void CVulkanDrawing::CreateCommandPool()
{
	SQueueFamilyIndices queueFamilyIndices = CVulkanQueueFamily::findQueueFamilies(m_PhysicalDevice.GetPhysicalDevice(), m_Presentation.GetSurface());

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(m_LogicalDevice.getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void CVulkanDrawing::DestroyCommandPool()
{
	vkDestroyCommandPool(m_LogicalDevice.getDevice(), commandPool, nullptr);
}

void CVulkanDrawing::CreateCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(m_LogicalDevice.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
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
		renderPassInfo.renderPass = m_Presentation.GetRenderPass();
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_Presentation.GetSwapChainExtend();
		
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
	vkFreeCommandBuffers(m_LogicalDevice.getDevice(), commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
}

void CVulkanDrawing::CreateSemaphores()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(m_LogicalDevice.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_LogicalDevice.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create semaphores for a frame!");
		}
	}
}

void CVulkanDrawing::DestroySemaphores()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(m_LogicalDevice.getDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_LogicalDevice.getDevice(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_LogicalDevice.getDevice(), inFlightFences[i], nullptr);
	}
}

void CVulkanDrawing::RegisterMesh(CVulkanMesh mesh)
{
	m_VulkanMesh.push_back(mesh);
}

void CVulkanDrawing::UnregisterMesh(CVulkanMesh mesh)
{
	m_VulkanMesh.erase(std::find(m_VulkanMesh.begin(), m_VulkanMesh.end(), mesh));
	DestroyCommandBuffers();
	CreateCommandBuffers();
}



void CVulkanDrawing::Draw()
{
	vkWaitForFences(m_LogicalDevice.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(m_LogicalDevice.getDevice(), 1, &inFlightFences[currentFrame]);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(m_LogicalDevice.getDevice(), m_Presentation.GetSwapChain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	for(unsigned int i = 0; i < m_VulkanMesh.size(); i++)
	{
		//m_VulkanMesh.at(i).UpdateUniformBuffers(imageIndex);
		m_VulkanMesh.at(i).SetCurrentImage(imageIndex);
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

	vkResetFences(m_LogicalDevice.getDevice(), 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(m_LogicalDevice.GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	
	VkSwapchainKHR swapChains[] = { m_Presentation.GetSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	vkQueuePresentKHR(m_LogicalDevice.GetPresentQueue(), &presentInfo);

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
		if (vkCreateSemaphore(m_LogicalDevice.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_LogicalDevice.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_LogicalDevice.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
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
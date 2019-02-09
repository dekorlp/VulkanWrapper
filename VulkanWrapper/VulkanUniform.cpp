#include "VulkanUniform.h"

void CVulkanUniform::CreateUniform(unsigned int uniformSet, unsigned int uniformBinding, VkDescriptorType descriptorTyp, VkShaderStageFlags shaderStage)
{
	this->mUniformSet = uniformSet;
	this->mUniformBinding = uniformBinding;
	this->mDescriptorType = descriptorTyp;
	this->mShaderStage = shaderStage;
}

void CVulkanUniform::SetDescriptorSetLayout(VkDescriptorSetLayout *descriptorSetLayout)
{
	this->m_DescriptorSetLayout = descriptorSetLayout;
}

VkDescriptorSetLayout* CVulkanUniform::GetDescriptorSetLayout()
{
	return this->m_DescriptorSetLayout;
}

unsigned int CVulkanUniform::GetUniformSet()
{
	return this->mUniformSet;
}

unsigned int CVulkanUniform::GetUniformBinding()
{
	return this->mUniformBinding;
}

VkDescriptorType CVulkanUniform::GetDescriptorType()
{
	return this->mDescriptorType;
}

VkShaderStageFlags CVulkanUniform::GetShaderStageFlag()
{
	return this->mShaderStage;
}
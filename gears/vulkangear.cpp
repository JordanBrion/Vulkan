/*
* Vulkan Example - Animated gears using multiple uniform buffers
*
* See readme.md for details
*
* Copyright (C) 2015 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "vulkangear.h"

int32_t VulkanGear::newVertex(std::vector<Vertex> *vBuffer, float x, float y, float z, const glm::vec3& normal)
{
	Vertex v(
		glm::vec3(x, y, z),
		normal,
		color
		);
	vBuffer->push_back(v);
	return vBuffer->size() - 1;
}

void VulkanGear::newFace(std::vector<uint32_t> *iBuffer, int a, int b, int c)
{
	iBuffer->push_back(a);
	iBuffer->push_back(b);
	iBuffer->push_back(c);
}

VulkanGear::VulkanGear(vk::Device device, VulkanExampleBase *example)
{
	this->device = device;
	this->exampleBase = example;
}

VulkanGear::~VulkanGear()
{
	// Clean up vulkan resources
	device.destroyBuffer(uniformData.buffer, nullptr);
	device.freeMemory(uniformData.memory, nullptr);

	device.destroyBuffer(vertexBuffer.buf, nullptr);
	device.freeMemory(vertexBuffer.mem, nullptr);

	device.destroyBuffer(indexBuffer.buf, nullptr);
	device.freeMemory(indexBuffer.mem, nullptr);
}

void VulkanGear::generate(float inner_radius, float outer_radius, float width, int teeth, float tooth_depth, glm::vec3 color, glm::vec3 pos, float rotSpeed, float rotOffset)
{
	this->color = color;
	this->pos = pos;
	this->rotOffset = rotOffset;
	this->rotSpeed = rotSpeed;

	std::vector<Vertex> vBuffer;
	std::vector<uint32_t> iBuffer;

	int i;
	float r0, r1, r2;
	float ta, da;
	float u1, v1, u2, v2, len;
	float cos_ta, cos_ta_1da, cos_ta_2da, cos_ta_3da, cos_ta_4da;
	float sin_ta, sin_ta_1da, sin_ta_2da, sin_ta_3da, sin_ta_4da;
	int32_t ix0, ix1, ix2, ix3, ix4, ix5;

	r0 = inner_radius;
	r1 = outer_radius - tooth_depth / 2.0;
	r2 = outer_radius + tooth_depth / 2.0;
	da = 2.0 * M_PI / teeth / 4.0;

	glm::vec3 normal;

	for (i = 0; i < teeth; i++)
	{
		ta = i * 2.0 * M_PI / teeth;
		// todo : naming
		cos_ta = cos(ta);
		cos_ta_1da = cos(ta + da);
		cos_ta_2da = cos(ta + 2 * da);
		cos_ta_3da = cos(ta + 3 * da);
		cos_ta_4da = cos(ta + 4 * da);
		sin_ta = sin(ta);
		sin_ta_1da = sin(ta + da);
		sin_ta_2da = sin(ta + 2 * da);
		sin_ta_3da = sin(ta + 3 * da);
		sin_ta_4da = sin(ta + 4 * da);

		u1 = r2 * cos_ta_1da - r1 * cos_ta;
		v1 = r2 * sin_ta_1da - r1 * sin_ta;
		len = sqrt(u1 * u1 + v1 * v1);
		u1 /= len;
		v1 /= len;
		u2 = r1 * cos_ta_3da - r2 * cos_ta_2da;
		v2 = r1 * sin_ta_3da - r2 * sin_ta_2da;

		// front face
		normal = glm::vec3(0.0, 0.0, 1.0);
		ix0 = newVertex(&vBuffer, r0 * cos_ta, r0 * sin_ta, width * 0.5, normal);
		ix1 = newVertex(&vBuffer, r1 * cos_ta, r1 * sin_ta, width * 0.5, normal);
		ix2 = newVertex(&vBuffer, r0 * cos_ta, r0 * sin_ta, width * 0.5, normal);
		ix3 = newVertex(&vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, width * 0.5, normal);
		ix4 = newVertex(&vBuffer, r0 * cos_ta_4da, r0 * sin_ta_4da, width * 0.5, normal);
		ix5 = newVertex(&vBuffer, r1 * cos_ta_4da, r1 * sin_ta_4da, width * 0.5, normal);
		newFace(&iBuffer, ix0, ix1, ix2);
		newFace(&iBuffer, ix1, ix3, ix2);
		newFace(&iBuffer, ix2, ix3, ix4);
		newFace(&iBuffer, ix3, ix5, ix4);

		// front sides of teeth
		normal = glm::vec3(0.0, 0.0, 1.0);
		ix0 = newVertex(&vBuffer, r1 * cos_ta, r1 * sin_ta, width * 0.5, normal);
		ix1 = newVertex(&vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, width * 0.5, normal);
		ix2 = newVertex(&vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, width * 0.5, normal);
		ix3 = newVertex(&vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, width * 0.5, normal);
		newFace(&iBuffer, ix0, ix1, ix2);
		newFace(&iBuffer, ix1, ix3, ix2);

		// back face 
		normal = glm::vec3(0.0, 0.0, -1.0);
		ix0 = newVertex(&vBuffer, r1 * cos_ta, r1 * sin_ta, -width * 0.5, normal);
		ix1 = newVertex(&vBuffer, r0 * cos_ta, r0 * sin_ta, -width * 0.5, normal);
		ix2 = newVertex(&vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, -width * 0.5, normal);
		ix3 = newVertex(&vBuffer, r0 * cos_ta, r0 * sin_ta, -width * 0.5, normal);
		ix4 = newVertex(&vBuffer, r1 * cos_ta_4da, r1 * sin_ta_4da, -width * 0.5, normal);
		ix5 = newVertex(&vBuffer, r0 * cos_ta_4da, r0 * sin_ta_4da, -width * 0.5, normal);
		newFace(&iBuffer, ix0, ix1, ix2);
		newFace(&iBuffer, ix1, ix3, ix2);
		newFace(&iBuffer, ix2, ix3, ix4);
		newFace(&iBuffer, ix3, ix5, ix4);

		// back sides of teeth 
		normal = glm::vec3(0.0, 0.0, -1.0);
		ix0 = newVertex(&vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, -width * 0.5, normal);
		ix1 = newVertex(&vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, -width * 0.5, normal);
		ix2 = newVertex(&vBuffer, r1 * cos_ta, r1 * sin_ta, -width * 0.5, normal);
		ix3 = newVertex(&vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, -width * 0.5, normal);
		newFace(&iBuffer, ix0, ix1, ix2);
		newFace(&iBuffer, ix1, ix3, ix2);

		// draw outward faces of teeth 
		normal = glm::vec3(v1, -u1, 0.0);
		ix0 = newVertex(&vBuffer, r1 * cos_ta, r1 * sin_ta, width * 0.5, normal);
		ix1 = newVertex(&vBuffer, r1 * cos_ta, r1 * sin_ta, -width * 0.5, normal);
		ix2 = newVertex(&vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, width * 0.5, normal);
		ix3 = newVertex(&vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, -width * 0.5, normal);
		newFace(&iBuffer, ix0, ix1, ix2);
		newFace(&iBuffer, ix1, ix3, ix2);

		normal = glm::vec3(cos_ta, sin_ta, 0.0);
		ix0 = newVertex(&vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, width * 0.5, normal);
		ix1 = newVertex(&vBuffer, r2 * cos_ta_1da, r2 * sin_ta_1da, -width * 0.5, normal);
		ix2 = newVertex(&vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, width * 0.5, normal);
		ix3 = newVertex(&vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, -width * 0.5, normal);
		newFace(&iBuffer, ix0, ix1, ix2);
		newFace(&iBuffer, ix1, ix3, ix2);

		normal = glm::vec3(v2, -u2, 0.0);
		ix0 = newVertex(&vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, width * 0.5, normal);
		ix1 = newVertex(&vBuffer, r2 * cos_ta_2da, r2 * sin_ta_2da, -width * 0.5, normal);
		ix2 = newVertex(&vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, width * 0.5, normal);
		ix3 = newVertex(&vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, -width * 0.5, normal);
		newFace(&iBuffer, ix0, ix1, ix2);
		newFace(&iBuffer, ix1, ix3, ix2);

		normal = glm::vec3(cos_ta, sin_ta, 0.0);
		ix0 = newVertex(&vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, width * 0.5, normal);
		ix1 = newVertex(&vBuffer, r1 * cos_ta_3da, r1 * sin_ta_3da, -width * 0.5, normal);
		ix2 = newVertex(&vBuffer, r1 * cos_ta_4da, r1 * sin_ta_4da, width * 0.5, normal);
		ix3 = newVertex(&vBuffer, r1 * cos_ta_4da, r1 * sin_ta_4da, -width * 0.5, normal);
		newFace(&iBuffer, ix0, ix1, ix2);
		newFace(&iBuffer, ix1, ix3, ix2);

		// draw inside radius cylinder 
		ix0 = newVertex(&vBuffer, r0 * cos_ta, r0 * sin_ta, -width * 0.5, glm::vec3(-cos_ta, -sin_ta, 0.0));
		ix1 = newVertex(&vBuffer, r0 * cos_ta, r0 * sin_ta, width * 0.5, glm::vec3(-cos_ta, -sin_ta, 0.0));
		ix2 = newVertex(&vBuffer, r0 * cos_ta_4da, r0 * sin_ta_4da, -width * 0.5, glm::vec3(-cos_ta_4da, -sin_ta_4da, 0.0));
		ix3 = newVertex(&vBuffer, r0 * cos_ta_4da, r0 * sin_ta_4da, width * 0.5, glm::vec3(-cos_ta_4da, -sin_ta_4da, 0.0));
		newFace(&iBuffer, ix0, ix1, ix2);
		newFace(&iBuffer, ix1, ix3, ix2);
	}

	int vertexBufferSize = vBuffer.size() * sizeof(Vertex);
	int indexBufferSize = iBuffer.size() * sizeof(uint32_t);

	vk::MemoryAllocateInfo memAlloc;
	vk::MemoryRequirements memReqs;

	void *data;

	// Generate vertex buffer
	vk::BufferCreateInfo vBufferInfo = vkTools::initializers::bufferCreateInfo(vk::BufferUsageFlagBits::eVertexBuffer, vertexBufferSize);
	vertexBuffer.buf = device.createBuffer(vBufferInfo, nullptr);
	
	memReqs = device.getBufferMemoryRequirements(vertexBuffer.buf);
	memAlloc.allocationSize = memReqs.size;
	exampleBase->getMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible, &memAlloc.memoryTypeIndex);
	vertexBuffer.mem = device.allocateMemory(memAlloc, nullptr);
	
	data = device.mapMemory(vertexBuffer.mem, 0, vertexBufferSize, vk::MemoryMapFlags());
	
	memcpy(data, vBuffer.data(), vertexBufferSize);
	device.unmapMemory(vertexBuffer.mem);
	device.bindBufferMemory(vertexBuffer.buf, vertexBuffer.mem, 0);
	

	// Generate index buffer
	vk::BufferCreateInfo iBufferInfo = vkTools::initializers::bufferCreateInfo(vk::BufferUsageFlagBits::eIndexBuffer, indexBufferSize);
	indexBuffer.buf = device.createBuffer(iBufferInfo, nullptr);
	
	memReqs = device.getBufferMemoryRequirements(indexBuffer.buf);
	memAlloc.allocationSize = memReqs.size;
	exampleBase->getMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible, &memAlloc.memoryTypeIndex);
	indexBuffer.mem = device.allocateMemory(memAlloc, nullptr);
	
	data = device.mapMemory(indexBuffer.mem, 0, indexBufferSize, vk::MemoryMapFlags());
	
	memcpy(data, iBuffer.data(), indexBufferSize);
	device.unmapMemory(indexBuffer.mem);
	device.bindBufferMemory(indexBuffer.buf, indexBuffer.mem, 0);
	
	indexBuffer.count = iBuffer.size();

	prepareUniformBuffer();
}

void VulkanGear::draw(vk::CommandBuffer cmdbuffer, vk::PipelineLayout pipelineLayout)
{
	vk::DeviceSize offsets = 0;
	cmdbuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr);
	cmdbuffer.bindVertexBuffers(0, vertexBuffer.buf, offsets);
	cmdbuffer.bindIndexBuffer(indexBuffer.buf, 0, vk::IndexType::eUint32);
	cmdbuffer.drawIndexed(indexBuffer.count, 1, 0, 0, 1);
}

void VulkanGear::updateUniformBuffer(glm::mat4 perspective, glm::vec3 rotation, float zoom, float timer)
{
	ubo.projection = perspective;

	ubo.view = glm::lookAt(
		glm::vec3(0, 0, -zoom),
		glm::vec3(-1.0, -1.5, 0),
		glm::vec3(0, 1, 0)
		);
	ubo.view = glm::rotate(ubo.view, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	ubo.view = glm::rotate(ubo.view, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

	ubo.model = glm::mat4();
	ubo.model = glm::translate(ubo.model, pos);
	rotation.z = (rotSpeed * timer) + rotOffset;
	ubo.model = glm::rotate(ubo.model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.normal = glm::inverseTranspose(ubo.view * ubo.model);

	//ubo.lightPos = lightPos;
	ubo.lightPos = glm::vec3(0.0f, 0.0f, 2.5f);
	ubo.lightPos.x = sin(glm::radians(timer)) * 8.0f;
	ubo.lightPos.z = cos(glm::radians(timer)) * 8.0f;

	void *pData = device.mapMemory(uniformData.memory, 0, sizeof(ubo), vk::MemoryMapFlags());
	
	memcpy(pData, &ubo, sizeof(ubo));
	device.unmapMemory(uniformData.memory);
}

void VulkanGear::setupDescriptorSet(vk::DescriptorPool pool, vk::DescriptorSetLayout descriptorSetLayout)
{
	vk::DescriptorSetAllocateInfo allocInfo =
		vkTools::initializers::descriptorSetAllocateInfo(pool, &descriptorSetLayout, 1);

	descriptorSet = device.allocateDescriptorSets(allocInfo)[0];

	// Binding 0 : Vertex shader uniform buffer
	vk::WriteDescriptorSet writeDescriptorSet =
		vkTools::initializers::writeDescriptorSet(descriptorSet, vk::DescriptorType::eUniformBuffer, 0, &uniformData.descriptor);

	device.updateDescriptorSets(writeDescriptorSet, nullptr);
}

void VulkanGear::prepareUniformBuffer()
{
	// Vertex shader uniform buffer block
	vk::MemoryAllocateInfo allocInfo;
	vk::MemoryRequirements memReqs;

	vk::BufferCreateInfo bufferInfo = vkTools::initializers::bufferCreateInfo(vk::BufferUsageFlagBits::eUniformBuffer, sizeof(ubo));

	uniformData.buffer = device.createBuffer(bufferInfo, nullptr);
	
	memReqs = device.getBufferMemoryRequirements(uniformData.buffer);
	allocInfo.allocationSize = memReqs.size;
	exampleBase->getMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible, &allocInfo.memoryTypeIndex);
	uniformData.memory = device.allocateMemory(allocInfo, nullptr);
	
	device.bindBufferMemory(uniformData.buffer, uniformData.memory, 0);
	

	uniformData.descriptor.buffer = uniformData.buffer;
	uniformData.descriptor.offset = 0;
	uniformData.descriptor.range = sizeof(ubo);
	uniformData.allocSize = allocInfo.allocationSize;
}

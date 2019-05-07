//------------------------------------------------------------------------------
// vkshaderrwbuffer.cc
// (C) 2016-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "render/stdneb.h"
#include "vkshaderrwbuffer.h"
#include "vkgraphicsdevice.h"
#include "coregraphics/config.h"
#include "vkutilities.h"

namespace Vulkan
{

ShaderRWBufferAllocator shaderRWBufferAllocator(0x00FFFFFF);

SizeT ShaderRWBufferStretchInterface::Grow(const SizeT capacity, const SizeT numInstances, SizeT & newCapacity)
{
	VkShaderRWBufferLoadInfo& setupInfo = shaderRWBufferAllocator.Get<0>(this->obj.id24);
	VkShaderRWBufferRuntimeInfo& runtimeInfo = shaderRWBufferAllocator.Get<1>(this->obj.id24);
	VkShaderRWBufferMapInfo& mapInfo = shaderRWBufferAllocator.Get<2>(this->obj.id24);
#if NEBULA_DEBUG
	n_assert(this->obj.id8 == ConstantBufferIdType);
#endif
	// new capacity is the old one, plus the number of elements we wish to allocate, although never allocate fewer than grow
	SizeT increment = capacity >> 1;
	increment = Math::n_iclamp(increment, setupInfo.grow, 65535);
	n_assert(increment >= numInstances);
	newCapacity = capacity + increment;

	// create new buffer
	const Util::Set<uint32_t>& queues = Vulkan::GetQueueFamilies();
	setupInfo.info.pQueueFamilyIndices = queues.KeysAsArray().Begin();
	setupInfo.info.queueFamilyIndexCount = (uint32_t)queues.Size();
	setupInfo.info.size = newCapacity * setupInfo.stride;

	VkBuffer newBuf;
	VkResult res = vkCreateBuffer(setupInfo.dev, &setupInfo.info, nullptr, &newBuf);
	n_assert(res == VK_SUCCESS);

	// allocate new instance memory, alignedSize is the aligned size of a single buffer
	VkDeviceMemory newMem;
	uint32_t alignedSize;
	VkUtilities::AllocateBufferMemory(setupInfo.dev, newBuf, newMem, VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT), alignedSize);

	// bind to buffer, this is the reason why we must destroy and create the buffer again
	res = vkBindBufferMemory(setupInfo.dev, newBuf, newMem, 0);
	n_assert(res == VK_SUCCESS);

	// copy old to new, old memory is already mapped
	void* dstData;

	// map new memory with new capacity, avoids a second map
	res = vkMapMemory(setupInfo.dev, newMem, 0, alignedSize, 0, &dstData);
	n_assert(res == VK_SUCCESS);
	memcpy(dstData, mapInfo.data, setupInfo.size);
	vkUnmapMemory(setupInfo.dev, setupInfo.mem);

	// clean up old data	
	vkDestroyBuffer(setupInfo.dev, runtimeInfo.buf, nullptr);
	vkFreeMemory(setupInfo.dev, setupInfo.mem, nullptr);

	// replace old device memory and size
	setupInfo.size = alignedSize;
	runtimeInfo.buf = newBuf;
	setupInfo.mem = newMem;
	mapInfo.data = dstData;
	return alignedSize;
}

//------------------------------------------------------------------------------
/**
*/
const VkBuffer
ShaderRWBufferGetVkBuffer(const CoreGraphics::ShaderRWBufferId id)
{
	return shaderRWBufferAllocator.Get<1>(id.id24).buf;
}

} // namespace Vulkan

namespace CoreGraphics
{

using namespace Vulkan;
//------------------------------------------------------------------------------
/**
*/
const ShaderRWBufferId
CreateShaderRWBuffer(const ShaderRWBufferCreateInfo& info)
{
	Ids::Id32 id = shaderRWBufferAllocator.Alloc();

	VkShaderRWBufferLoadInfo& setupInfo = shaderRWBufferAllocator.Get<0>(id);
	VkShaderRWBufferRuntimeInfo& runtimeInfo = shaderRWBufferAllocator.Get<1>(id);
	VkShaderRWBufferMapInfo& mapInfo = shaderRWBufferAllocator.Get<2>(id);
	ShaderRWBufferStretchInterface& stretch = shaderRWBufferAllocator.Get<3>(id);

	VkPhysicalDeviceProperties props = Vulkan::GetCurrentProperties();
	setupInfo.dev = Vulkan::GetCurrentDevice();
	setupInfo.numBuffers = info.numBackingBuffers;
	SizeT size = info.size;

	const Util::Set<uint32_t>& queues = Vulkan::GetQueueFamilies();
	setupInfo.info =
	{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0,
		(VkDeviceSize)(size * setupInfo.numBuffers),
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_SHARING_MODE_CONCURRENT,
		(uint32_t)queues.Size(),
		queues.KeysAsArray().Begin()
	};
	VkResult res = vkCreateBuffer(setupInfo.dev, &setupInfo.info, nullptr, &runtimeInfo.buf);
	n_assert(res == VK_SUCCESS);

	uint32_t alignedSize;
	VkUtilities::AllocateBufferMemory(setupInfo.dev, runtimeInfo.buf, setupInfo.mem, VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT), alignedSize);

	// bind to buffer
	res = vkBindBufferMemory(setupInfo.dev, runtimeInfo.buf, setupInfo.mem, 0);
	n_assert(res == VK_SUCCESS);

	// size and stride for a single buffer are equal
	setupInfo.size = alignedSize;
	setupInfo.stride = alignedSize / setupInfo.numBuffers;

	ShaderRWBufferId ret;
	ret.id24 = id;
	ret.id8 = ShaderRWBufferIdType;
	stretch.obj = ret;
	stretch.resizer.Setup(setupInfo.stride, (SizeT)props.limits.minStorageBufferOffsetAlignment, setupInfo.numBuffers);
	stretch.resizer.SetTarget(&stretch);

	// map memory so we can use it later
	res = vkMapMemory(setupInfo.dev, setupInfo.mem, 0, alignedSize, 0, &mapInfo.data);
	n_assert(res == VK_SUCCESS);

#if NEBULA_GRAPHICS_DEBUG
	ObjectSetName(ret, info.name.Value());
#endif

	CoreGraphics::RegisterShaderRWBuffer(info.name, ret);

	return ret;
}

//------------------------------------------------------------------------------
/**
*/
void
DestroyShaderRWBuffer(const ShaderRWBufferId id)
{
	VkShaderRWBufferLoadInfo& setupInfo = shaderRWBufferAllocator.Get<0>(id.id24);
	VkShaderRWBufferRuntimeInfo& runtimeInfo = shaderRWBufferAllocator.Get<1>(id.id24);

	vkUnmapMemory(setupInfo.dev, setupInfo.mem);
	vkDestroyBuffer(setupInfo.dev, runtimeInfo.buf, nullptr);
	vkFreeMemory(setupInfo.dev, setupInfo.mem, nullptr);

	shaderRWBufferAllocator.Dealloc(id.id24);
}

} // namespace CoreGraphics
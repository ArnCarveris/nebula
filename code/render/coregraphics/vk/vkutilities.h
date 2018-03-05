#pragma once
//------------------------------------------------------------------------------
/**
	Implements some Vulkan related utilities
	
	(C) 2017 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "vkdeferredcommand.h"
#include "coregraphics/gpubuffertypes.h"
#include "coregraphics/pixelformat.h"
#include "coregraphics/texture.h"
#include "coregraphics/cmdbuffer.h"
namespace Vulkan
{
class VkTexture;
class VkUtilities
{
public:
	/// constructor
	VkUtilities();
	/// destructor
	virtual ~VkUtilities();

	/// perform image layout transition immediately
	static void ImageLayoutTransition(CoreGraphicsQueueType queue, VkImageMemoryBarrier barrier);
	/// perform image layout transition immediately
	static void ImageLayoutTransition(VkCommandBuffer buf, VkImageMemoryBarrier barrier);
	/// create image memory barrier
	static VkImageMemoryBarrier ImageMemoryBarrier(const VkImage& img, VkImageSubresourceRange subres, VkImageLayout oldLayout, VkImageLayout newLayout);
	/// create image ownership change
	static VkImageMemoryBarrier ImageMemoryBarrier(const VkImage& img, VkImageSubresourceRange subres, CoreGraphicsQueueType fromQueue, CoreGraphicsQueueType toQueue, VkImageLayout layout);
	/// create buffer memory barrier
	static VkBufferMemoryBarrier BufferMemoryBarrier(const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, VkAccessFlags srcAccess, VkAccessFlags dstAccess);
	/// transition image between layouts
	static void ChangeImageLayout(const VkImageMemoryBarrier& barrier, const CoreGraphicsQueueType type);
	/// transition image ownership
	static void ImageOwnershipChange(CoreGraphicsQueueType queue, VkImageMemoryBarrier barrier);
	/// perform image color clear
	static void ImageColorClear(const VkImage& image, const CoreGraphicsQueueType queue, VkImageLayout layout, VkClearColorValue clearValue, VkImageSubresourceRange subres);
	/// perform image depth stencil clear
	static void ImageDepthStencilClear(const VkImage& image, const CoreGraphicsQueueType queue, VkImageLayout layout, VkClearDepthStencilValue clearValue, VkImageSubresourceRange subres);

	/// allocate a buffer memory storage, num is a multiplier for how many times the size needs to be duplicated
	static void AllocateBufferMemory(const VkDevice dev, const VkBuffer& buf, VkDeviceMemory& bufmem, VkMemoryPropertyFlagBits flags, uint32_t& bufsize);
	/// allocate an image memory storage, num is a multiplier for how many times the size needs to be duplicated
	static void AllocateImageMemory(const VkDevice dev, const VkImage& img, VkDeviceMemory& imgmem, VkMemoryPropertyFlagBits flags, uint32_t& imgsize);
	/// figure out which memory type fits given memory bits and required properties
	static VkResult GetMemoryType(uint32_t bits, VkMemoryPropertyFlags flags, uint32_t& index);

	/// update buffer memory from CPU
	static void BufferUpdate(const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, const void* data);
	/// update buffer memory from CPU
	static void BufferUpdate(VkCommandBuffer cmd, const VkBuffer& buf, VkDeviceSize offset, VkDeviceSize size, const void* data);
	/// update image memory from CPU
	static void ImageUpdate(const VkImage& img, const VkImageCreateInfo& info, uint32_t mip, uint32_t face, VkDeviceSize size, uint32_t* data);

	/// perform image read-back, and saves to buffer (SLOW!)
	static void ReadImage(const VkImage tex, CoreGraphics::PixelFormat::Code format, CoreGraphics::TextureDimensions dims, CoreGraphics::TextureType type, VkImageCopy copy, uint32_t& outMemSize, VkDeviceMemory& outMem, VkBuffer& outBuffer);
	/// perform image write-back, transitions data from buffer to image (SLOW!)
	static void WriteImage(const VkBuffer& srcImg, const VkImage& dstImg, VkImageCopy copy);
	/// helper to begin immediate transfer
	static CoreGraphics::CmdBufferId BeginImmediateTransfer();
	/// helper to end immediate transfer
	static void EndImmediateTransfer(CoreGraphics::CmdBufferId cmdBuf);
};
} // namespace Vulkan
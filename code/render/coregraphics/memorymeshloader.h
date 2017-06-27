#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::MemoryMeshLoader
    
    Setup a Mesh object from a given vertex, index buffer and primitive group.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "resources/resourceloader.h"
#include "coregraphics/base/resourcebase.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/primitivegroup.h"

//------------------------------------------------------------------------------
namespace CoreGraphics
{
class MemoryMeshLoader : public Resources::ResourceLoader
{
    __DeclareClass(MemoryMeshLoader);
public:
    /// constructor
    MemoryMeshLoader();
    /// set the intended resource usage (default is UsageImmutable)
    void SetUsage(Base::GpuResourceBase::Usage usage);
    /// get resource usage
    Base::GpuResourceBase::Usage GetUsage() const;
    /// set the intended resource access (default is AccessNone)
    void SetAccess(Base::GpuResourceBase::Access access);
    /// get the resource access
    Base::GpuResourceBase::Access GetAccess() const;
    /// set vertex buffer
    void SetVertexBuffer(const Ptr<CoreGraphics::VertexBuffer>& vBuffer);
    /// set index buffer
    void SetIndexBuffer(const Ptr<CoreGraphics::IndexBuffer>& iBuffer);
    /// set primitive group
    void SetPrimitiveGroups(const Util::Array<CoreGraphics::PrimitiveGroup>& pGroup);

    /// called by resource when a load is requested
    virtual bool OnLoadRequested();

protected:
    /// setup mesh resource from given memory data
    bool SetupMeshFromMemory();

private:    
    Base::GpuResourceBase::Usage usage;
    Base::GpuResourceBase::Access access;
    Ptr<CoreGraphics::VertexBuffer> vertexBuffer;
    Ptr<CoreGraphics::IndexBuffer> indexBuffer;
    Util::Array<CoreGraphics::PrimitiveGroup> primitiveGroups;
};

//------------------------------------------------------------------------------
/**
*/
inline void
MemoryMeshLoader::SetUsage(Base::GpuResourceBase::Usage usage_)
{
    this->usage = usage_;
}

//------------------------------------------------------------------------------
/**
*/
inline Base::GpuResourceBase::Usage
MemoryMeshLoader::GetUsage() const
{
    return this->usage;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MemoryMeshLoader::SetAccess(Base::GpuResourceBase::Access access_)
{
    this->access = access_;
}

//------------------------------------------------------------------------------
/**
*/
inline Base::GpuResourceBase::Access
MemoryMeshLoader::GetAccess() const
{
    return this->access;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
MemoryMeshLoader::SetVertexBuffer(const Ptr<CoreGraphics::VertexBuffer>& vBuffer)
{
    this->vertexBuffer = vBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
MemoryMeshLoader::SetIndexBuffer(const Ptr<CoreGraphics::IndexBuffer>& iBuffer)
{
    this->indexBuffer = iBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
MemoryMeshLoader::SetPrimitiveGroups(const Util::Array<CoreGraphics::PrimitiveGroup>& pGroup)
{
    this->primitiveGroups = pGroup;
}

} // namespace CoreGraphics
//------------------------------------------------------------------------------

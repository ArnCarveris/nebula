// NIDL #version:57#
#ifdef _WIN32
#define NOMINMAX
#endif
//------------------------------------------------------------------------------
//  graphicsprotocol.cc
//  (C) Individual contributors, see AUTHORS file
//
//  MACHINE GENERATED, DON'T EDIT!
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicsprotocol.h"
#include "scripting/bindings.h"
//------------------------------------------------------------------------------
namespace Msg
{
PYBIND11_EMBEDDED_MODULE(graphicsprotocol, m)
{
    m.doc() = "namespace Msg";
    m.def("SetModel", &SetModel::Send, "Set the model resource.");
}
} // namespace Msg

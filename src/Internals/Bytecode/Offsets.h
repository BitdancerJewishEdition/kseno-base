#pragma once
#include <Windows.h>

namespace Offsets {
    namespace VisualEngine
    {
        inline uintptr_t Pointer = 0x7AF2720;
        inline uintptr_t ToFakeDataModel = 0x700;
        inline uintptr_t FakeToRealDataModel = 0x1C0;
    }

    namespace DataModel
    {
        inline uintptr_t GameLoaded = 0x5F8;
    }

    namespace Instance 
    {
        inline uintptr_t ChildrenEnd = 0x8; 
        inline uintptr_t ChildrenStart = 0x70; 
        inline uintptr_t ClassBase = 0x1400; 
        inline uintptr_t ClassDescriptor = 0x18; 
        inline uintptr_t ClassName = 0x8; 
        inline uintptr_t Name = 0xb0; 
        inline uintptr_t Parent = 0x68; 
    }

    namespace ScriptContext
    {
        inline uintptr_t RequireBypass = 0x8e9;
    }

    namespace ModuleScript
    {
        inline uintptr_t Embedded = 0x150; 
    }

     namespace ProtectedString {
        inline constexpr uintptr_t String = 0x10;
        inline constexpr uintptr_t Size = 0x20;
    }
}
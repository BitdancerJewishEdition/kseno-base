#pragma once
#include <cstdint>

namespace Offsets
{
	namespace VisualEngine
	{
		inline constexpr uintptr_t Pointer = 0x79449e0;
		inline constexpr uintptr_t ToFakeDataModel = 0x700;
		inline constexpr uintptr_t ToRealDataModel = 0x1C0;
	}

	namespace Instance
	{
		inline constexpr uintptr_t Name = 0xB0;
		inline constexpr uintptr_t ClassDescriptor = 0x18;
		inline constexpr uintptr_t ClassName = 0x8;
		inline constexpr uintptr_t ChildrenStart = 0x70;
		inline constexpr uintptr_t ChildrenEnd = 0x8;
	}

	namespace ModuleScript
	{
		inline constexpr uintptr_t Bytecode = 0x150;
	}

	namespace LocalScript
	{
		inline constexpr uintptr_t Bytecode = 0x1A8;
	}
	
	namespace ScriptContext
	{
		inline constexpr uintptr_t RequireBypass = 0x969;
	}

	namespace ByteCode
	{
		inline constexpr uintptr_t Pointer = 0x10;
		inline constexpr uintptr_t Size = 0x20;
	}

}

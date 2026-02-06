#include "Roblox.hpp"
#include <windows.h>
#include <winternl.h>
#define self(ts) reinterpret_cast<unsigned long long>(ts)

typedef LONG NTSTATUS;

typedef NTSTATUS(WINAPI* pNtReadVirtualMemory)(
	HANDLE,
	PVOID,
	PVOID,
	SIZE_T,
	PSIZE_T
	);

typedef NTSTATUS(WINAPI* pNtUnlockVirtualMemory)(
	HANDLE,
	PVOID*,
	PSIZE_T,
	ULONG
	);

static pNtReadVirtualMemory NtReadVirtualMemory =
(pNtReadVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");

static pNtUnlockVirtualMemory NtUnlockVirtualMemory =
(pNtUnlockVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtUnlockVirtualMemory");

using pNtWriteVirtualMemory = NTSTATUS(NTAPI*)(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	SIZE_T NumberOfBytesToWrite,
	PSIZE_T NumberOfBytesWritten
	);

static pNtWriteVirtualMemory NtWriteVirtualMemory =
(pNtWriteVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");

auto Roblox::Instance::GetChildren() -> std::vector<Roblox::Instance*>
{
	static thread_local std::vector<Roblox::Instance*> children;
	children.clear();

	auto childrenstart = memory->read<unsigned long long>(self(this) + Offsets::Instance::ChildrenStart);
	auto childrenend = memory->read<unsigned long long>(childrenstart + Offsets::Instance::ChildrenEnd);

	for (auto instance = memory->read<unsigned long long>(childrenstart); instance < childrenend; instance += 0x10)
	{
		children.emplace_back(memory->read<Roblox::Instance*>(instance));
	}

	return std::vector<Roblox::Instance*>(children);
}

auto Roblox::Instance::FindFirstChild(std::string name) -> Roblox::Instance*
{
	auto children = this->GetChildren();

	for (Roblox::Instance* child : children)
	{
		if (child->Name() == name)
			return child;
	}

	return nullptr;
}

auto Roblox::Instance::FindFirstChildByClass(std::string classname) -> Roblox::Instance*
{
	auto children = this->GetChildren();

	for (Roblox::Instance* child : children)
	{
		if (child->ClassName() == classname)
			return child;
	}

	return nullptr;
}

auto Roblox::Instance::GetBytecode() -> std::string
{
	if (this->ClassName() != "LocalScript" && this->ClassName() != "ModuleScript" && this->ClassName() != "CoreScript")
		return "";

	auto embeddedType = (this->ClassName() == "LocalScript" || this->ClassName() == "CoreScript") ? Offsets::LocalScript::Bytecode : Offsets::ModuleScript::Bytecode;
	auto embedded = memory->read<unsigned long long>(self(this) + embeddedType);
	auto bytecodePointer = memory->read<unsigned long long>(embedded + Offsets::ByteCode::Pointer);
	auto bytecodeSize = memory->read<unsigned long long>(embedded + Offsets::ByteCode::Size);

	std::string bytecodeBuffer;
	bytecodeBuffer.resize(bytecodeSize);

	MEMORY_BASIC_INFORMATION bi;
	VirtualQueryEx(memory->get_process_handle(), reinterpret_cast<LPCVOID>(bytecodePointer), &bi, sizeof(bi));

	NtReadVirtualMemory(memory->get_process_handle(), reinterpret_cast<PVOID>(bytecodePointer), bytecodeBuffer.data(), bytecodeSize, nullptr);

	PVOID baddr = bi.AllocationBase;
	SIZE_T size = bi.RegionSize;
	NtUnlockVirtualMemory(memory->get_process_handle(), &baddr, &size, 1);

	return Bytecode::newDecompress(bytecodeBuffer);
}

auto Roblox::Instance::SetBytecode(const std::string& bytecodeArg) -> std::string
{
	if (this->ClassName() != "LocalScript" && this->ClassName() != "ModuleScript" && this->ClassName() != "CoreScript")
		return "";

	auto embeddedType = (this->ClassName() == "LocalScript" || this->ClassName() == "CoreScript") ? Offsets::LocalScript::Bytecode : Offsets::ModuleScript::Bytecode;
	auto embedded = memory->read<unsigned long long>(self(this) + embeddedType);

	LPVOID newPointer = VirtualAllocEx(memory->get_process_handle(), nullptr, bytecodeArg.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	SIZE_T bytesWritten;
	if (!WriteProcessMemory(memory->get_process_handle(), newPointer, bytecodeArg.data(), bytecodeArg.size(), &bytesWritten) || bytesWritten != bytecodeArg.size()) {
		VirtualFreeEx(memory->get_process_handle(), newPointer, 0, MEM_RELEASE);
		return "jew";
	}

	memory->write<unsigned long long>(embedded + Offsets::ByteCode::Pointer, reinterpret_cast<uintptr_t>(newPointer));
	memory->write<unsigned long long>(embedded + Offsets::ByteCode::Size, bytecodeArg.size());

	return "mhm";
}

auto Roblox::Instance::SpoofWith(uintptr_t instanceblox) -> void
{
	memory->write(self(this) + 0x8, instanceblox);
}

auto Roblox::Instance::EnableRequireBypass() -> void
{
	memory->write<unsigned long long>(self(this) + Offsets::ScriptContext::RequireBypass, 1);
}

auto Roblox::Instance::Self() -> uintptr_t
{
	return self(this);
}

std::string Roblox::Instance::Name()
{
	auto name = memory->read<unsigned long long>(self(this) + Offsets::Instance::Name);

	if (name)
		return memory->read_string(name);

	return "Failed to find Instance Name";
}

std::string Roblox::Instance::ClassName()
{
	auto classdescriptor = memory->read<unsigned long long>(self(this) + Offsets::Instance::ClassDescriptor);
	auto classname = memory->read<unsigned long long>(classdescriptor + Offsets::Instance::ClassName);

	if (classname)
		return memory->read_string(classname);

	return "Failed to find Instance ClassName";
}
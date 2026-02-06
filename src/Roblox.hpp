#include <vector>
#include <string>
#include <Internals/Memory/memory.h>
#include <Internals/Bytecode/Bytecode.hpp>
#include <Internals/Roblox/Offsets.hpp>

namespace Roblox
{
	class Instance
	{
	public:
		auto GetChildren() -> std::vector<Roblox::Instance*>;
		auto FindFirstChild(std::string name) -> Roblox::Instance*;
		auto FindFirstChildByClass(std::string classname) -> Roblox::Instance*;
		auto GetBytecode() -> std::string;
		auto SetBytecode(const std::string& bytecodeArg) -> std::string;
		auto SpoofWith(uintptr_t instanceblox) -> void;
		auto EnableRequireBypass() -> void;
		auto Self() -> uintptr_t;

		std::string Name();
		std::string ClassName();
	};

	class DataModel final : public Roblox::Instance
	{
	public:
		static auto Get() -> Roblox::DataModel*
		{
			if (!memory) return nullptr;

			auto VisualEngine = memory->read<unsigned long long>(memory->get_module_address() + Offsets::VisualEngine::Pointer);
			auto FakeDataModel = memory->read<unsigned long long>(VisualEngine + Offsets::VisualEngine::ToFakeDataModel);
			auto DataModel = memory->read<unsigned long long>(FakeDataModel + Offsets::VisualEngine::ToRealDataModel);

			return reinterpret_cast<Roblox::DataModel*>(DataModel);
		}
	};
}
#include <iostream>
#include "Roblox.hpp"
#include <filesystem>
#include <thread>
#include <chrono>
#include <fstream>

auto GetMainWindowFromPID(DWORD pid) -> HWND {
	HWND hwnd = nullptr;
	struct EnumData {
		DWORD pid;
		HWND hwnd;
	} data = { pid, nullptr };

	auto callback = [](HWND h, LPARAM lParam) -> BOOL {
		EnumData* pData = (EnumData*)lParam;
		DWORD windowPID;
		GetWindowThreadProcessId(h, &windowPID);
		if (windowPID == pData->pid && GetWindow(h, GW_OWNER) == nullptr && IsWindowVisible(h)) {
			pData->hwnd = h;
			return FALSE;
		}
		return TRUE;
		};

	EnumWindows(callback, (LPARAM)&data);
	return data.hwnd;
}

auto main() -> int
{
	SetConsoleTitleA("kseno dev build");
	memory->attach_to_process("RobloxPlayerBeta.exe");
	memory->find_module_address("RobloxPlayerBeta.exe");

	auto dm = Roblox::DataModel::Get();
	std::cout << "base -> 0x" << std::hex << memory->get_module_address() << std::endl;
	std::cout << "workspace -> 0x" << std::hex << dm << std::endl;

	auto [success, bytecode] = Bytecode::Compile("script.Parent=nil;task.spawn(function() print('injected kseno base!') end);while true do task.wait(9e9) end");

	//game.StarterPlayer.StarterPlayerScripts.PlayerModule.ControlModule.VRNavigation
	auto VRNavigation = dm->FindFirstChildByClass("StarterPlayer")->FindFirstChild("StarterPlayerScripts")->FindFirstChild("PlayerModule")->FindFirstChild("ControlModule")->FindFirstChild("VRNavigation");
	auto PlayerListManager = dm->FindFirstChildByClass("CoreGui")->FindFirstChild("RobloxGui")->FindFirstChild("Modules")->FindFirstChild("PlayerList")->FindFirstChild("PlayerListManager");
	
	if (!VRNavigation)
	{
		std::cout << "failed to get VRNavigation" << std::endl;
		_getwch();
		return 1;
	}

	if (!PlayerListManager)
	{
		std::cout << "failed to get PlayerListManager" << std::endl;
		_getwch();
		return 1;
	}

	dm->FindFirstChildByClass("ScriptContext")->EnableRequireBypass();
	VRNavigation->SetBytecode(bytecode);
	PlayerListManager->SpoofWith(VRNavigation->Self());

	HWND window = GetMainWindowFromPID(memory->get_process_id());
	std::cout << "injected!" << std::endl;
	
	while (GetForegroundWindow() != window) {
		if (!IsWindowVisible(window)) ShowWindow(window, SW_SHOW);
		if (IsIconic(window)) ShowWindow(window, SW_RESTORE);

		DWORD currentThreadId = GetCurrentThreadId();
		DWORD fgThreadId = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
		AttachThreadInput(currentThreadId, fgThreadId, TRUE);
		BringWindowToTop(window);
		SetForegroundWindow(window);
		SetFocus(window);
		AttachThreadInput(currentThreadId, fgThreadId, FALSE);
	}

	INPUT inputs[2] = {};
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_ESCAPE;
	inputs[0].ki.wScan = MapVirtualKey(VK_ESCAPE, 0);
	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_ESCAPE;
	inputs[1].ki.wScan = MapVirtualKey(VK_ESCAPE, 0);
	inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(2, inputs, sizeof(INPUT));

	std::this_thread::sleep_for(std::chrono::microseconds(250000));
	PlayerListManager->SpoofWith(PlayerListManager->Self());

	_getwch();

	return 1;
}
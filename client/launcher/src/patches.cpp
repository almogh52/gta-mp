#include "patches.h"

#include <Windows.h>
#include <winternl.h>

#include "../../shared/hook/pattern.h"
#include "../../shared/hook/modify.h"
#include "../../shared/hook/manager.h"

typedef NTSTATUS (*NtQueryInformationProcessType)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL);
NTSTATUS NtQueryInformationProcessHook(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL)
{
	// Call original function
	NTSTATUS status = ((NtQueryInformationProcessType)gtamp::hook::manager::get_trampoline("NtQueryInformationProcess"))(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);

	// Get the explorer process id
	DWORD explorerPid;
	HWND shellWindow = GetShellWindow();
	GetWindowThreadProcessId(shellWindow, &explorerPid);

	if (NT_SUCCESS(status))
	{
		if (ProcessInformationClass == ProcessBasicInformation)
		{
			((PPROCESS_BASIC_INFORMATION)ProcessInformation)->Reserved3 = (PVOID)explorerPid;
		}
		else if (ProcessInformationClass == 30) // ProcessDebugObjectHandle
		{
			*(HANDLE *)ProcessInformation = 0;

			return ((NTSTATUS)0xC0000353L);
		}
		else if (ProcessInformationClass == 7) // ProcessDebugPort
		{
			*(HANDLE *)ProcessInformation = 0;
		}
		else if (ProcessInformationClass == 31)
		{
			*(ULONG *)ProcessInformation = 1;
		}
	}

	return status;
}

void gtamp::launcher::patches::apply_pre_load_patches()
{
	void *nt_query_information_process = GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtQueryInformationProcess");
	void *raise_user_exception = GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "KiRaiseUserExceptionDispatcher");

	// Install the hook for the NtQueryInformationProcess WinAPI call to allow for debugging
	hook::manager::install_hook("NtQueryInformationProcess", nt_query_information_process, NtQueryInformationProcessHook);

	// Get the PEB and patch it to set being debugged to false
	PPEB peb = (PPEB)__readgsqword(0x60);
	peb->BeingDebugged = false;

	// Remove the protection on the raise exception WinAPI function
	DWORD oldProtect;
	VirtualProtect(raise_user_exception, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

	// Disable the raise excpetion function
	hook::modify::ret(raise_user_exception);
}

void gtamp::launcher::patches::apply_post_load_patches()
{
	// Get the address of the launcher check
	uint8_t *launcher_check = hook::pattern("E8 ? ? ? ? 84 C0 75 ? B2 01 B9 2F A9 C2 F4");

	// Remove the check of the launcher
	hook::modify::put<uint8_t>(launcher_check + 7, 0xEB);
	hook::modify::nop(launcher_check, 7);
}
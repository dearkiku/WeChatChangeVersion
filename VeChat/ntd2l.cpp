#include "ntd2l.h"

void GetModuleInfoForProcess(DWORD processId, const wchar_t* targetModuleName) {

	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
	if (processHandle == NULL) {
		// printf("无法打开进程，错误码：%d\n", GetLastError());
		return;
	}
	BOOL bTarget = FALSE;
	BOOL bSource = FALSE;

	IsWow64Process(GetCurrentProcess(), &bSource);
	IsWow64Process(processHandle, &bTarget);

	SYSTEM_INFO si;
	GetSystemInfo(&si);

	if (bTarget == FALSE && bSource == TRUE) {
		HMODULE NtdllModule = GetModuleHandle(L"ntdll.dll");


		pfnNtWow64QueryInformationProcess64 NtWow64QueryInformationProcess64 = (pfnNtWow64QueryInformationProcess64)GetProcAddress(NtdllModule, "NtWow64QueryInformationProcess64");
		pfnNtWow64ReadVirtualMemory64 NtWow64ReadVirtualMemory64 = (pfnNtWow64ReadVirtualMemory64)GetProcAddress(NtdllModule, "NtWow64ReadVirtualMemory64");
		PROCESS_BASIC_INFORMATION64 pbi64 = { 0 };
		if (NT_SUCCESS(NtWow64QueryInformationProcess64(processHandle, ProcessBasicInformation, &pbi64, sizeof(pbi64), NULL))) {
			DWORD64 Ldr64 = 0;
			LIST_ENTRY64 ListEntry64 = { 0 };
			LDR_DATA_TABLE_ENTRY64 LDTE64 = { 0 };
			wchar_t ProPath64[256];
			if (NT_SUCCESS(NtWow64ReadVirtualMemory64(processHandle, (PVOID64)(pbi64.PebBaseAddress + offsetof(PEB64, Ldr)), &Ldr64, sizeof(Ldr64), NULL))) {
				if (NT_SUCCESS(NtWow64ReadVirtualMemory64(processHandle, (PVOID64)(Ldr64 + offsetof(PEB_LDR_DATA64, InLoadOrderModuleList)), &ListEntry64, sizeof(LIST_ENTRY64), NULL))) {
					if (NT_SUCCESS(NtWow64ReadVirtualMemory64(processHandle, (PVOID64)(ListEntry64.Flink), &LDTE64, sizeof(_LDR_DATA_TABLE_ENTRY64), NULL))) {
						while (1) {
							if (LDTE64.InLoadOrderLinks.Flink == ListEntry64.Flink) break;
							if (NT_SUCCESS(NtWow64ReadVirtualMemory64(processHandle, (PVOID64)LDTE64.FullDllName.Buffer, ProPath64, sizeof(ProPath64), NULL))) {
								if (wcscmp(ProPath64, targetModuleName) == 0) {
									// printf("模块基址:0x%llX\t模块大小:0x%X\t模块路径:%ls\n", LDTE64.DllBase, LDTE64.SizeOfImage, ProPath64);
									break;
								}
							}
							if (!NT_SUCCESS(NtWow64ReadVirtualMemory64(processHandle, (PVOID64)LDTE64.InLoadOrderLinks.Flink, &LDTE64, sizeof(_LDR_DATA_TABLE_ENTRY64), NULL))) break;
						}
					}
				}
			}
		}
	}
	else if (bTarget == TRUE && bSource == TRUE || si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_IA64) {

		HMODULE NtdllModule = GetModuleHandle(L"ntdll.dll");
		pfnNtQueryInformationProcess NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(NtdllModule, "NtQueryInformationProcess");
		PROCESS_BASIC_INFORMATION32 pbi32 = { 0 };
		if (NT_SUCCESS(NtQueryInformationProcess(processHandle, ProcessBasicInformation, &pbi32, sizeof(pbi32), NULL))) {
			DWORD Ldr32 = 0;
			LIST_ENTRY32 ListEntry32 = { 0 };
			LDR_DATA_TABLE_ENTRY32 LDTE32 = { 0 };
			wchar_t ProPath32[256];
			if (ReadProcessMemory(processHandle, (PVOID)(pbi32.PebBaseAddress + offsetof(PEB32, Ldr)), &Ldr32, sizeof(Ldr32), NULL)) {
				if (ReadProcessMemory(processHandle, (PVOID)(Ldr32 + offsetof(PEB_LDR_DATA32, InLoadOrderModuleList)), &ListEntry32, sizeof(LIST_ENTRY32), NULL)) {
					if (ReadProcessMemory(processHandle, (PVOID)(ListEntry32.Flink), &LDTE32, sizeof(_LDR_DATA_TABLE_ENTRY32), NULL)) {
						while (1) {
							if (LDTE32.InLoadOrderLinks.Flink == ListEntry32.Flink) break;
							if (ReadProcessMemory(processHandle, (PVOID)LDTE32.FullDllName.Buffer, ProPath32, sizeof(ProPath32), NULL)) {
								if (wcscmp(ProPath32, targetModuleName) == 0) {
									// printf("模块基址:0x%X\t模块大小:0x%X\t模块路径:%ls\n", LDTE32.DllBase, LDTE32.SizeOfImage, ProPath32);
									break;
								}
							}
							if (!ReadProcessMemory(processHandle, (PVOID)LDTE32.InLoadOrderLinks.Flink, &LDTE32, sizeof(_LDR_DATA_TABLE_ENTRY32), NULL)) break;
						}
					}
				}
			}
		}
	}

	CloseHandle(processHandle);
}



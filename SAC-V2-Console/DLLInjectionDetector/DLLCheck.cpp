#include "DLLInjectorDector.h"
#include "Utils.h"
#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include "..//AbortFailureDetects.h"

typedef ULONG(NTAPI* RtlGetFullPathName_U)(PCWSTR FileName, ULONG Size, PWSTR Buffer, PWSTR* ShortName);
static RtlGetFullPathName_U RtlGetFullPathName_U_ = nullptr;

ULONG NTAPI RtlGetFullPathName_U_t(PCWSTR FileName, ULONG Size, PWSTR Buffer, PWSTR* ShortName)
{
	//printf("RtlGetFullPathName_U_t -> %ls - %u\n", FileName, Size);

	auto pModuleBase = CUtils::GetModuleAddressFromName(FileName);
	if (pModuleBase)
	{
		ErrorHandler::ErrorMessage("19", 2);
	}

	return RtlGetFullPathName_U_(FileName, Size, Buffer, ShortName);
}

void InitializeDLLCheck()
{
	auto hNtdll = LoadLibraryA("ntdll.dll");
	if (hNtdll)
	{
		//printf("hNtdll: %p\n", hNtdll);
		assert(hNtdll);

		auto RtlGetFullPathName_U_o = reinterpret_cast<RtlGetFullPathName_U>(GetProcAddress(hNtdll, "RtlGetFullPathName_U"));
		//printf("RtlGetFullPathName_U: %p\n", RtlGetFullPathName_U_o);
		assert(RtlGetFullPathName_U_o);

		RtlGetFullPathName_U_ = reinterpret_cast<RtlGetFullPathName_U>(CUtils::DetourFunc(reinterpret_cast<PBYTE>(RtlGetFullPathName_U_o), reinterpret_cast<PBYTE>(RtlGetFullPathName_U_t), 5));
		//printf("RtlGetFullPathName_U(detour): %p\n", RtlGetFullPathName_U_);

		DWORD dwOld = 0;
		auto bProtectRet = VirtualProtect(RtlGetFullPathName_U_, 5, PAGE_EXECUTE_READWRITE, &dwOld);
		assert(bProtectRet);
	}
	else
	{
		ErrorHandler::ErrorMessage("18", 1);
	}
}
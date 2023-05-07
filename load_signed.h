#pragma once

#include "log.h"
namespace prototypes
{
	typedef NTSTATUS(*NtLoadDriver)(PUNICODE_STRING DriverServiceName);
	typedef NTSTATUS(*NtUnloadDriver)(PUNICODE_STRING DriverServiceName);
	typedef NTSTATUS(*RtlAdjustPrivilege)(_In_ ULONG Privilege, _In_ BOOLEAN Enable, _In_ BOOLEAN Client, _Out_ PBOOLEAN WasEnabled);
}

namespace native
{
	constexpr auto SystemModuleInformation = 11;
	constexpr auto SystemHandleInformation = 16;
	constexpr auto SystemExtendedHandleInformation = 64;

	typedef struct _RTL_PROCESS_MODULE_INFORMATION
	{
		HANDLE Section;
		PVOID MappedBase;
		PVOID ImageBase;
		ULONG ImageSize;
		ULONG Flags;
		USHORT LoadOrderIndex;
		USHORT InitOrderIndex;
		USHORT LoadCount;
		USHORT OffsetToFileName;
		UCHAR FullPathName[256];
	} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

	typedef struct _RTL_PROCESS_MODULES
	{
		ULONG NumberOfModules;
		RTL_PROCESS_MODULE_INFORMATION Modules[1];
	} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;
}

namespace load_signed
{
	BOOL SetupByteFile();
	std::string GetTemporaryLocation();
	std::string GetDriverName();
	std::string GetDriverPath();
	bool ReadFileToMemory(const char* FilePath, void*& OutputBuffer);
	bool WriteFileToMemory(const char* DesiredFilePath, const char* Address, SIZE_T Size);
	bool IsDriverRunning();
	bool RegisterSignedDriver(const char* DriverPath, const char* DriverName);
	bool UnloadSignedDriver(const char* DriverName);
}
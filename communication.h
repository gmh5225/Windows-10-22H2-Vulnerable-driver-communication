#pragma once

#include "load_signed.h"


namespace communication
{
#pragma pack(push, 1)

	typedef struct __READ_PHYSICAL_MEMORY
	{
		void* PhysAddPtr;
		unsigned int NumberOfBytes;
	}READ_PHYSICAL_MEMORY, * PREAD_PHYSICAL_MEMORY;

	typedef struct _READ_MSR
	{
		unsigned long Msr;
		unsigned long Output;

	}READ_MSR, * PREAD_MSR;

	typedef struct _READ_CONTROL_REGISTER
	{
		BYTE CaseNumber;
		unsigned long long Output;
	}READ_CONTROL_REGISTER, * PREAD_CONTROL_REGISTER;




#pragma pack(pop)
	inline void* GlobalDriverHandle;

	uint64_t GetSystemModuleImageBase(const char* SystemModuleName);
	void* OpenDriverHandle();
	void InitGlobalHandle();
	unsigned long long ReadMsr(unsigned long ModelSpecificRegister);
	uintptr_t ReadPhysicalMemory(void* PhysicalAddress, unsigned long NumberOfBytes);

	std::pair<DWORDLONG, DWORD> AllocateContigiousMemory(ULONG NumberOfBytes);
	void FreeContiguousMemory(DWORDLONG BaseAddress, DWORDLONG NumberOfBytes);

	bool WriteControlRegister(uint64_t ControlRegister, DWORDLONG NewValue);
	DWORDLONG ReadControlRegister(uint64_t ControlRegister);
}

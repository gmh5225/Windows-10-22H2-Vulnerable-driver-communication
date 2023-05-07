#pragma once

#include "load_signed.h"


namespace communication
{
#pragma pack(push, 1)
	typedef struct _READ_CONTROL_REGISTER
	{
		BYTE CaseNumber;
		unsigned long long Output;
	}READ_CONTROL_REGISTER, * PREAD_CONTROL_REGISTER;
#pragma pack(pop)
	
	inline void* GlobalDriverHandle;

	void* OpenDriverHandle();
	void InitGlobalHandle();

	std::pair<DWORDLONG, DWORD> AllocateContigiousMemory(ULONG NumberOfBytes);
	void FreeContiguousMemory(DWORDLONG BaseAddress, DWORDLONG NumberOfBytes);

	bool WriteControlRegister(uint64_t ControlRegister, DWORDLONG NewValue);
	DWORDLONG ReadControlRegister(uint64_t ControlRegister);
}

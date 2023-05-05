
std::pair<DWORDLONG, DWORD> communication::AllocateContigiousMemory(ULONG NumberOfBytes)
{
	DWORDLONG InputArg[2];
	*InputArg = NumberOfBytes;

	DWORD BytesReturned = 5;
	DeviceIoControl(GlobalDriverHandle, 0x222880, &InputArg, sizeof(InputArg),
		&InputArg, sizeof(InputArg), &BytesReturned,
		nullptr);

	return { *(InputArg + 1), BytesReturned };
}

void communication::FreeContiguousMemory(DWORDLONG BaseAddress, DWORDLONG NumberOfBytes)
{
	DWORDLONG InputArg[2];
	*InputArg = NumberOfBytes;
	InputArg[1] = BaseAddress;

	DWORD BytesReturned;
	DeviceIoControl(GlobalDriverHandle, 0x222884, &InputArg, sizeof(InputArg), &InputArg, sizeof(InputArg),
		&BytesReturned, nullptr);

}

bool communication::WriteControlRegister(uint64_t ControlRegister, DWORDLONG NewValue)
{
	uint64_t InputArg[2];
	*InputArg = ControlRegister;
	*(InputArg + 1) = NewValue;

	DWORD BytesReturned;
	DeviceIoControl(GlobalDriverHandle, 2238576, &InputArg, sizeof(InputArg), &InputArg, sizeof(InputArg),
		&BytesReturned, nullptr);
}

DWORDLONG communication::ReadControlRegister(uint64_t ControlRegister)
{
	uint64_t InputBuffer[2];
	*InputBuffer = ControlRegister;

	unsigned long BytesReturned;
	DeviceIoControl(GlobalDriverHandle, 2238572, &InputBuffer, sizeof(InputBuffer),
		&InputBuffer, sizeof(InputBuffer), &BytesReturned, nullptr);

	return *(InputBuffer + 1);
}

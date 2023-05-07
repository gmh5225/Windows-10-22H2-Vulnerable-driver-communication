#include "log.h"

void logOutput::LogMessage(const char* Message)
{
	HANDLE pStdColour = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(pStdColour, 9);
	std::cout << "[%] ";
	SetConsoleTextAttribute(pStdColour, 7);
	std::cout << Message << std::endl;
}

void logOutput::LogAddress(uintptr_t Address)
{
	HANDLE pStdColour = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(pStdColour, 4);
	std::cout << "[&] ";
	SetConsoleTextAttribute(pStdColour, 7);
	std::cout << std::hex << Address << std::endl;
}

void logOutput::LogMessageAndAddress(const char* Message, uintptr_t Address)
{
	HANDLE pStdColour = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(pStdColour, 10);
	std::cout << "[$] ";
	SetConsoleTextAttribute(pStdColour, 7);
	std::cout << Message << std::hex << Address << "\n";
}

void logOutput::LogSuccess(const char* Message)
{
	HANDLE pStdColour = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(pStdColour, 11);
	std::cout << "[+] ";
	SetConsoleTextAttribute(pStdColour, 7);
	std::cout << Message << "\n";
}

void logOutput::LogError(const char* Message)
{
	HANDLE pStdColour = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(pStdColour, 7);
	std::cout << "[-] ";
	SetConsoleTextAttribute(pStdColour, 12);
	std::cout << Message << std::endl;
	SetConsoleTextAttribute(pStdColour, 7);
}

void logOutput::DebugLogAddr(const char* Message, uintptr_t Address)
{
	HANDLE pStdColour = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(pStdColour, 3);
	std::cout << "[d] ";
	SetConsoleTextAttribute(pStdColour, 7);
	std::cout << Message << std::hex << Address << std::endl;
}

void logOutput::DebugLog(const char* Message)
{
	HANDLE pStdColour = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(pStdColour, 3);
	std::cout << "[d] ";
	SetConsoleTextAttribute(pStdColour, 7);
	std::cout << Message << std::endl;

}

void logOutput::LogErrorGLA(const char* Message, int Error) {
	HANDLE pStdColour = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(pStdColour, 7);
	std::cout << "[-] ";
	SetConsoleTextAttribute(pStdColour, 12);
	std::cout << Message << std::hex << Error << std::endl;
	SetConsoleTextAttribute(pStdColour, 7);
}

void logOutput::LogSuccessStatus(const char* Message, int StatusCode)
{
	HANDLE pStdColour = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(pStdColour, 7);
	std::cout << "[+] " << Message << std::hex << StatusCode << std::endl;
}
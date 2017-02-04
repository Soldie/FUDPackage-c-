#pragma once
class CStealthInject
{
public:
	CStealthInject();
	~CStealthInject();
public:
	IMAGE_DOS_HEADER _mainDosHeader;
	IMAGE_NT_HEADERS _mainNtHeaders;

	IMAGE_DOS_HEADER _injectingDosHeader;
	IMAGE_NT_HEADERS32 _injectingNtHeaders;
	IMAGE_DOS_HEADER* _injectedDosHeader;
	IMAGE_NT_HEADERS32* _injectedNtHeaders;
	char* _dllData;
	char* _dllStart;
public:
	bool StealthLoadLibraryMemory(HANDLE hProcess, void* address);
	void StealthLoadLibrary(void* hProcess, const char* path);
private:
	HMODULE GetRemoteModuleHandle(HANDLE hProcess, const char* module);
	FARPROC RemoteGetProcAddress(HANDLE hProcess, HMODULE hModule, const char* func);
	DWORD Inject(HANDLE hProcess, const char* dllname);
	bool ReadMemory(HANDLE hProcess, LPVOID baseAddress, LPVOID buffer, int size);
	bool WriteMemory(HANDLE hProcess, LPVOID baseAddress, LPVOID buffer, int size);
	void LoadSections(HANDLE hProcess);
	void Relocate(HANDLE hProcess, int delta);
	void ResolveImports(HANDLE hProcess);
};


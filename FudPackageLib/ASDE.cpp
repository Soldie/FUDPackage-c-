#include "stdafx.h"
#include "Base.h"
#include "PassEmulator.h"
#include <winioctl.h>
#include "asde_tools.h"
#include <string>
//////////////////////////////////////////////////////////////////////////
int gensandbox_less_than_onegb() {
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);
	return (statex.ullTotalPhys / 1024) < 1048576 ? TRUE : FALSE;
}

int gensandbox_one_cpu_GetSystemInfo() {
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	return siSysInfo.dwNumberOfProcessors <= 2 ? TRUE : FALSE;
}

int gensandbox_drive_size() {
	HANDLE drive;
	BOOL result;
	GET_LENGTH_INFORMATION size;
	DWORD lpBytesReturned;

	drive = CreateFile("\\\\.\\PhysicalDrive0", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (drive == INVALID_HANDLE_VALUE) {
		// Someone is playing tricks. Or not enough privileges.
		CloseHandle(drive);
		return FALSE;
	}
	result = DeviceIoControl(drive, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &size,
		sizeof(GET_LENGTH_INFORMATION), &lpBytesReturned, NULL);
	CloseHandle(drive);
	if (result != 0) {
		if (size.Length.QuadPart / 1073741824 <= 60) /* <= 60 GB */
			return TRUE;
	}
	return FALSE;
}

int gensandbox_drive_size2() {
	ULARGE_INTEGER total_bytes;

	if (GetDiskFreeSpaceExA("C:\\", NULL, &total_bytes, NULL))
	{
		if (total_bytes.QuadPart / 1073741824 <= 60) /* <= 60 GB */
			return TRUE;
	}
	return FALSE;
}

int gensandbox_sleep_patched() {
	DWORD time1;

	time1 = GetTickCount();
	Sleep(500);
	if ((GetTickCount() - time1) > 450) return FALSE;
	else return TRUE;
}

int gensandbox_username() {
	char username[200];
	size_t i;
	DWORD usersize = sizeof(username);
	GetUserName(username, &usersize);
	for (i = 0; i < strlen(username); i++) { /* case-insensitive */
		username[i] = toupper(username[i]);
	}
	if (strstr(username, "SANDBOX") != NULL) {
		return TRUE;
	}
	if (strstr(username, "VIRUS") != NULL) {
		return TRUE;
	}
	if (strstr(username, "MALWARE") != NULL) {
		return TRUE;
	}
	return FALSE;
}

int gensandbox_path() {
	char path[500];
	size_t i;
	DWORD pathsize = sizeof(path);
	GetModuleFileName(NULL, path, pathsize);
	for (i = 0; i < strlen(path); i++) { /* case-insensitive */
		path[i] = toupper(path[i]);
	}
	if (strstr(path, "\\SAMPLE") != NULL) {
		return TRUE;
	}
	if (strstr(path, "\\VIRUS") != NULL) {
		return TRUE;
	}
	if (strstr(path, "SANDBOX") != NULL) {
		return TRUE;
	}
	return FALSE;
}

int gensandbox_common_names() {
	DWORD dwSize = MAX_PATH;
	char szLogicalDrives[MAX_PATH] = { 0 };
	DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);
	BOOL exists;

	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		char* szSingleDrive = szLogicalDrives;
		char filename[MAX_PATH] = { 0 };
		while (*szSingleDrive)
		{
			if (GetDriveType(szSingleDrive) != DRIVE_REMOVABLE) {
				snprintf(filename, MAX_PATH, "%ssample.exe", szSingleDrive);
				exists = _exists_file(filename);
				if (exists) return TRUE;
				snprintf(filename, MAX_PATH, "%smalware.exe", szSingleDrive);
				exists = _exists_file(filename);
				if (exists) return TRUE;
			}
			szSingleDrive += strlen(szSingleDrive) + 1;
		}
	}
	return FALSE;
}
int gensandbox_one_cpu() {
	DWORD NumberOfProcessors = 0;
	__asm
	{
		mov eax,fs:[0x18]
		mov eax,[eax+0x30]
		mov eax,[eax+0x64]
		mov NumberOfProcessors,eax
	}
	return NumberOfProcessors <= 2 ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
static inline void cpuid_vendor_00(char * vendor) {
	int Cpuinfo[4] = { 0 };
	__cpuid(Cpuinfo, 0);
	int ebx, ecx, edx;
	ebx = Cpuinfo[1];
	ecx = Cpuinfo[2];
	edx = Cpuinfo[3];
	sprintf(vendor, "%c%c%c%c", ebx, (ebx >> 8), (ebx >> 16), (ebx >> 24));
	sprintf(vendor + 4, "%c%c%c%c", edx, (edx >> 8), (edx >> 16), (edx >> 24));
	sprintf(vendor + 8, "%c%c%c%c", ecx, (ecx >> 8), (ecx >> 16), (ecx >> 24));
	vendor[12] = 0x00;
}
static inline void cpuid_brand(char * brand, uint32_t eax_value) {
	int eax, ebx, ecx, edx;
	int Cpuinfo[4] = { 0 };
	__cpuid(Cpuinfo, eax_value);
	eax = Cpuinfo[0];
	ebx = Cpuinfo[1];
	ecx = Cpuinfo[2];
	edx = Cpuinfo[3];
	//__asm__ volatile("cpuid" \
	//	: "=a"(eax), \
	//	"=b"(ebx), \
	//	"=c"(ecx), \
	//	"=d"(edx) \
	//	: "a"(eax_value));
	sprintf(brand, "%c%c%c%c", eax, (eax >> 8), (eax >> 16), (eax >> 24));
	sprintf(brand + 4, "%c%c%c%c", ebx, (ebx >> 8), (ebx >> 16), (ebx >> 24));
	sprintf(brand + 8, "%c%c%c%c", ecx, (ecx >> 8), (ecx >> 16), (ecx >> 24));
	sprintf(brand + 12, "%c%c%c%c", edx, (edx >> 8), (edx >> 16), (edx >> 24));
}
void cpu_write_brand(char * brand) {
	int xx = 0;
	/* Check if Processor Brand String is supported */
	//__asm__ volatile(".intel_syntax noprefix;"
	//	"mov eax, 0x80000000;"
	//	"cpuid;"
	//	"cmp eax, 0x80000004;"
	//	"xor eax, eax;"
	//	"setge al;"
	//	".att_syntax;"
	//	: "=a"(eax)
	//	);
	__asm
	{
		pushad
		mov eax,0x80000000
		cpuid
		cmp eax,0x80000004
		xor eax,eax
		setge al
		mov xx,eax
		popad
	}
	/* It's supported, so fill char * brand */
	if (xx) {
		cpuid_brand(brand, 0x80000002);
		cpuid_brand(brand + 16, 0x80000003);
		cpuid_brand(brand + 32, 0x80000004);
		brand[48] = 0x00;
	}
}
void cpu_write_vendor(char * vendor) {
	cpuid_vendor_00(vendor);
}
//////////////////////////////////////////////////////////////////////////
int bochs_reg_key1() {
	return _exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\Description\\System", "SystemBiosVersion", "BOCHS");
}

int bochs_cpu_amd1() {
	char cpu_brand[49];
	cpu_write_brand(cpu_brand);
	/* It checks the lowercase P in 'processor', an actual AMD returns Processor */
	return !memcmp(cpu_brand, "AMD Athlon(tm) processor", 24) ? TRUE : FALSE;
}

int bochs_cpu_amd2() {
	int XX;
	//__asm__ volatile(".intel_syntax noprefix;"
	//	"xor eax, eax;"
	//	"cpuid;"
	//	"cmp ecx, 0x444d4163;" /* AMD CPU? */
	//	"jne b2not_detected;"
	//	"mov eax, 0x8fffffff;" /* query easter egg */
	//	"cpuid;"
	//	"jecxz b2detected;" /* ECX value not filled */
	//	"b2not_detected: xor eax, eax; jmp b2exit;"
	//	"b2detected: mov eax, 0x1;"
	//	"b2exit: nop;"
	//	".att_syntax;"
	//	: "=a"(eax));
	__asm
	{
		pushad
		xor eax,eax
		cpuid
		cmp ecx, 0x444d4163
		jne b2not_detected
		mov eax,0x8fffffff
		cpuid
		jecxz b2detected
		b2not_detected: 
		xor eax, eax; 
		jmp b2exit
		b2detected: 
		mov eax, 0x1
		b2exit:
		mov XX,eax
		popad
	}
	return XX ? TRUE : FALSE;
}

int bochs_cpu_intel1() {
	char cpu_brand[49];
	cpu_write_brand(cpu_brand);
	/* This processor name is not known to be valid in an actual CPU */
	return !memcmp(cpu_brand, "              Intel(R) Pentium(R) 4 CPU        ", 47) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
int qemu_reg_key1() {
	return _exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", "Identifier", "QEMU");
}

int qemu_reg_key2() {
	return _exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\Description\\System", "SystemBiosVersion", "QEMU");
}

int qemu_cpu_name() {
	char cpu_brand[49];
	cpu_write_brand(cpu_brand);
	return !memcmp(cpu_brand, "QEMU Virtual CPU", 16) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
int wine_detect_get_unix_file_name() {
	HMODULE k32;
	k32 = GetModuleHandle("kernel32.dll");
	if (k32 != NULL) {
		if (GetProcAddress(k32, "wine_get_unix_file_name") != NULL) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
}

int wine_reg_key1() {
	return _exists_regkey(HKEY_CURRENT_USER, "SOFTWARE\\Wine");
}
//////////////////////////////////////////////////////////////////////////
/**
* Cuckoo Sandbox definitions.
*/
/**
* Extra space allocated with the hooks information structure.
*/
#define TLS_HOOK_INFO_RETADDR_SPACE 0x100

/**
* Hook informnation stored by Cuckoo at FS:[TLS_HOOK_INFO].
*/
struct hook_info {
	unsigned int depth_count;
	unsigned int hook_count;
	unsigned int retaddr_esp;
	unsigned int last_error;
	unsigned int ret_last_error;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
};

/**
* Read the address of the hooks information in the TLS.
*/
struct hook_info *read_hook_info() {
	int result = NULL;

	//__asm__ volatile ("mov %%fs:0x44,%%eax" : "=a" (result));
	__asm
	{
		mov eax,fs:[0x44]
		mov result,eax
	}
	return (hook_info *)result;
}

/**
* Cuckoo stores the return addresses in a extra space allocated in conjunction
* with the hook information function. The only way to check if the structure
* is valid is to calculate what is the minimum and maximum value for the
* return address value location.
*/
int cuckoo_check_tls() {
	struct hook_info *info = read_hook_info();

	if (info == NULL) {
		return FALSE;
	}

	unsigned int minimum = ((unsigned int)info + sizeof(struct hook_info));
	unsigned int maximum = minimum + TLS_HOOK_INFO_RETADDR_SPACE;

	return (info != NULL) && (info->retaddr_esp >= minimum && info->retaddr_esp <= maximum) ?
		TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
/* Thx Inaki for this! (@virtualminds_es) */
#include <shellapi.h>
static int check_hook_m1(DWORD * dwAddress) {
	BYTE *b = (BYTE *)dwAddress;
	return (*b == 0x8b) && (*(b + 1) == 0xff) ? FALSE : TRUE;
}

int check_hook_DeleteFileW_m1() {
	return check_hook_m1((DWORD *)DeleteFileW);
}

int check_hook_ShellExecuteExW_m1() {
	return check_hook_m1((DWORD *)ShellExecuteExW);
}

int check_hook_CreateProcessA_m1() {
	return check_hook_m1((DWORD *)CreateProcessA);
}
//////////////////////////////////////////////////////////////////////////
int sboxie_detect_sbiedll() {
	if (GetModuleHandle("sbiedll.dll") != NULL) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////////
/**
* SCSI registry key check
**/
int vbox_reg_key1() {
	return _exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", "Identifier", "VBOX");
}

/**
* SystemBiosVersion registry key check
**/
int vbox_reg_key2() {
	return _exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\Description\\System", "SystemBiosVersion", "VBOX");
}

/**
* VirtualBox Guest Additions key check
**/
int vbox_reg_key3() {
	return _exists_regkey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Oracle\\VirtualBox Guest Additions");
}

/**
* VideoBiosVersion key check
**/
int vbox_reg_key4() {
	return _exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\Description\\System", "VideoBiosVersion", "VIRTUALBOX");
}

/**
* ACPI Regkey detection
**/
int vbox_reg_key5() {
	return _exists_regkey(HKEY_LOCAL_MACHINE, "HARDWARE\\ACPI\\DSDT\\VBOX__");
}

/**
* FADT ACPI Regkey detection
**/
int vbox_reg_key7() {
	return _exists_regkey(HKEY_LOCAL_MACHINE, "HARDWARE\\ACPI\\FADT\\VBOX__");
}

/**
* RSDT ACPI Regkey detection
**/
int vbox_reg_key8() {
	return _exists_regkey(HKEY_LOCAL_MACHINE, "HARDWARE\\ACPI\\RSDT\\VBOX__");
}

/**
* VirtualBox Services Regkey detection
**/
int vbox_reg_key9(int writelogs) {
	int res = FALSE, i;
	const int count = 5;
	char message[200];

	std::string strs[count];
	strs[0] = "SYSTEM\\ControlSet001\\Services\\VBoxGuest";
	strs[1] = "SYSTEM\\ControlSet001\\Services\\VBoxMouse";
	strs[2] = "SYSTEM\\ControlSet001\\Services\\VBoxService";
	strs[3] = "SYSTEM\\ControlSet001\\Services\\VBoxSF";
	strs[4] = "SYSTEM\\ControlSet001\\Services\\VBoxVideo";
	for (i = 0; i < count; i++) {
		if (_exists_regkey(HKEY_LOCAL_MACHINE, ( char *)strs[i].c_str())) {
			//snprintf(message, sizeof(message) - sizeof(message[0]), "VirtualBox traced using Reg key HKLM\\%s", strs[i]);
			//if (writelogs) write_log(message);
			res = TRUE;
		}
	}
	return res;
}

/**
* HARDWARE\\DESCRIPTION\\System SystemBiosDate == 06/23/99
**/
int vbox_reg_key10() {
	return _exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System", "SystemBiosDate", "06/23/99");
}

/**
* VirtualBox driver files in \\WINDOWS\\system32\\drivers\\
**/
int vbox_sysfile1(int writelogs) {
	const int count = 4;
	std::string strs[count];
	int res = FALSE, i = 0;
	char message[200];

	strs[0] = "C:\\WINDOWS\\system32\\drivers\\VBoxMouse.sys";
	strs[1] = "C:\\WINDOWS\\system32\\drivers\\VBoxGuest.sys";
	strs[2] = "C:\\WINDOWS\\system32\\drivers\\VBoxSF.sys";
	strs[3] = "C:\\WINDOWS\\system32\\drivers\\VBoxVideo.sys";
	for (i = 0; i < count; i++) {
		if (_exists_file(( char *)strs[i].c_str())) {
			//snprintf(message, sizeof(message) - sizeof(message[0]), "VirtualBox traced using driver file %s", strs[i]);
			//if (writelogs) write_log(message);
			res = TRUE;
		}
	}
	return res;
}

/**
* VirtualBox other system files
**/
int vbox_sysfile2(int writelogs) {
	const int count = 14;
	std::string strs[count];
	int res = FALSE, i = 0;
	char message[200];

	strs[0] = "C:\\WINDOWS\\system32\\vboxdisp.dll";
	strs[1] = "C:\\WINDOWS\\system32\\vboxhook.dll";
	strs[2] = "C:\\WINDOWS\\system32\\vboxmrxnp.dll";
	strs[3] = "C:\\WINDOWS\\system32\\vboxogl.dll";
	strs[4] = "C:\\WINDOWS\\system32\\vboxoglarrayspu.dll";
	strs[5] = "C:\\WINDOWS\\system32\\vboxoglcrutil.dll";
	strs[6] = "C:\\WINDOWS\\system32\\vboxoglerrorspu.dll";
	strs[7] = "C:\\WINDOWS\\system32\\vboxoglfeedbackspu.dll";
	strs[8] = "C:\\WINDOWS\\system32\\vboxoglpackspu.dll";
	strs[9] = "C:\\WINDOWS\\system32\\vboxoglpassthroughspu.dll";
	strs[10] = "C:\\WINDOWS\\system32\\vboxservice.exe";
	strs[11] = "C:\\WINDOWS\\system32\\vboxtray.exe";
	strs[12] = "C:\\WINDOWS\\system32\\VBoxControl.exe";
	strs[13] = "C:\\program files\\oracle\\virtualbox guest additions\\";
	for (i = 0; i < count; i++) {
		if (_exists_file(( char *)strs[i].c_str())) {
			//snprintf(message, sizeof(message) - sizeof(message[0]), "VirtualBox traced using system file %s", strs[i]);
			//if (writelogs) write_log(message);
			res = TRUE;
		}
	}
	return res;
}

/**
* NIC MAC check
**/
int vbox_mac() {
	/* VirtualBox mac starts with 08:00:27 */
	return _check_mac_vendor("\x08\x00\x27");
}

/**
* VirtualBox devices
**/
int vbox_devices(int writelogs) {
	HANDLE h;
	const int count = 4;
	std::string strs[count];
	int res = FALSE, i = 0;
	char message[200];

	/* Got this list from https://github.com/cuckoobox/community/blob/master/modules/signatures/antivm_vbox_devices.py */
	strs[0] = "\\\\.\\VBoxMiniRdrDN";
	strs[1] = "\\\\.\\pipe\\VBoxMiniRdDN";
	strs[2] = "\\\\.\\VBoxTrayIPC";
	strs[3] = "\\\\.\\pipe\\VBoxTrayIPC";
	for (i = 0; i < count; i++) {
		h = CreateFile((const char *)strs[i].c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h != INVALID_HANDLE_VALUE) {
			//snprintf(message, sizeof(message) - sizeof(message[0]), "VirtualBox traced using device %s", strs[i]);
			//if (writelogs) write_log(message);
			res = TRUE;
		}
	}
	return res;
}

/**
* Checking for Tray window
* https://twitter.com/waleedassar
**/
int vbox_traywindow() {
	HWND h1, h2;
	h1 = FindWindow("VBoxTrayToolWndClass", NULL);
	h2 = FindWindow(NULL, "VBoxTrayToolWnd");
	if (h1 || h2) return TRUE;
	else return FALSE;
}

/**
* Checking network shared
* https://twitter.com/waleedassar
**/
int vbox_network_share() {
	unsigned long pnsize = 0x1000;
	char provider[0x1000];
	/* a0rtega : any reason for this to be in the heap :?, changed to stack */
	//char * provider = (char *)LocalAlloc(LMEM_ZEROINIT, pnsize);

	int retv = WNetGetProviderName(WNNC_NET_RDR2SAMPLE, provider, &pnsize);
	if (retv == NO_ERROR) {
		if (lstrcmpi(provider, "VirtualBox Shared Folders") == 0) {
			//LocalFree(provider);
			return TRUE;
		}
		else {
			//LocalFree(provider);
			return FALSE;
		}
	}
	return FALSE;
}

/**
* Checking for virtual box processes
**/
int vbox_processes(int writelogs) {
	int res = FALSE;
	HANDLE hpSnap;
	PROCESSENTRY32 pentry;

	hpSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hpSnap != INVALID_HANDLE_VALUE) {
		pentry.dwSize = sizeof(PROCESSENTRY32);
	}
	else {
		return FALSE;
	}

	if (!Process32First(hpSnap, &pentry)) {
		CloseHandle(hpSnap);
		return FALSE;
	}

	do {
		if (lstrcmpi(pentry.szExeFile, "vboxservice.exe") == 0) {
			//if (writelogs) write_log("VirtualBox traced using vboxservice.exe process");
			res = TRUE;
		}
		if (lstrcmpi(pentry.szExeFile, "vboxtray.exe") == 0) {
			//if (writelogs) write_log("VirtualBox traced using vboxtray.exe process");
			res = TRUE;
		}
	} while (Process32Next(hpSnap, &pentry));
	return res;
}

/**
* Check if the device identifier ("PCI\\VEN_80EE&DEV_CAFE") in the returned rows.
*/
int vbox_wmi_check_row(IWbemClassObject *row) {
	CIMTYPE type = CIM_ILLEGAL;
	VARIANT value;

	HRESULT hresult = row->Get(L"DeviceId", 0, &value, &type, 0);

	if (FAILED(hresult) || V_VT(&value) == VT_NULL || type != CIM_STRING) {
		return FALSE;
	}

	return (wcsstr(V_BSTR(&value), L"PCI\\VEN_80EE&DEV_CAFE") != NULL) ? TRUE : FALSE;
}

/**
* Check for devices VirtualBox devices using WMI.
*/
int vbox_wmi_devices() {
	IWbemServices *services = NULL;

	if (wmi_initialize(L"root\\cimv2", &services) != TRUE) {
		return FALSE;
	}

	int result = wmi_check_query(services, L"WQL", L"SELECT DeviceId FROM Win32_PnPEntity",
		&vbox_wmi_check_row);

	wmi_cleanup(services);

	return result;
}
//////////////////////////////////////////////////////////////////////////
int vmware_reg_key1() {
	if (_exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", "Identifier", "VMWARE") ||
		_exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 1\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", "Identifier", "VMWARE") ||
		_exists_regkey_value_str(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 2\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", "Identifier", "VMWARE")
		)
		return TRUE;
	else
		return FALSE;
}

int vmware_reg_key2() {
	return _exists_regkey(HKEY_LOCAL_MACHINE, "SOFTWARE\\VMware, Inc.\\VMware Tools");
}

int vmware_sysfile1() {
	return _exists_file("C:\\WINDOWS\\system32\\drivers\\vmmouse.sys");
}

int vmware_sysfile2() {
	return _exists_file("C:\\WINDOWS\\system32\\drivers\\vmhgfs.sys");
}

int vmware_mac() {
	/*
	VMware is any of
	00:05:69
	00:0C:29
	00:1C:14
	00:50:56
	*/
	if (_check_mac_vendor("\x00\x05\x69")) {
		return TRUE;
	}
	else if (_check_mac_vendor("\x00\x0C\x29")) {
		return TRUE;
	}
	else if (_check_mac_vendor("\x00\x1C\x14")) {
		return TRUE;
	}
	else if (_check_mac_vendor("\x00\x50\x56")) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

int vmware_adapter_name() {
	return _check_adapter_name("VMware");
}

int vmware_devices(int writelogs) {
	HANDLE h;
	const int count = 2;
	std::string strs[count];
	int res = FALSE, i = 0;
	char message[200];

	strs[0] = "\\\\.\\HGFS";
	strs[1] = "\\\\.\\vmci";
	for (i = 0; i < count; i++) {
		h = CreateFile((char *)strs[i].c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h != INVALID_HANDLE_VALUE) {
		//	snprintf(message, sizeof(message) - sizeof(message[0]), "VMWare traced using device %s", strs[i]);
		//	if (writelogs) write_log(message);
			res = TRUE;
		}
	}
	return res;
}

/**
* Check the serial number ("VMware") in the returned rows.
*/
int vmware_wmi_check_row(IWbemClassObject *row) {
	CIMTYPE type = CIM_ILLEGAL;
	VARIANT value;

	HRESULT hresult = row->Get(L"SerialNumber", 0, &value, &type, 0);

	if (FAILED(hresult) || V_VT(&value) == VT_NULL || type != CIM_STRING) {
		return FALSE;
	}

	return (wcsstr(V_BSTR(&value), L"VMware") != NULL) ? TRUE : FALSE;
}

/**
* Check for the computer serial using WMI.
*/
int vmware_wmi_serial() {
	IWbemServices *services = NULL;

	if (wmi_initialize(L"root\\cimv2", &services) != TRUE) {
		return FALSE;
	}

	int result = wmi_check_query(services, L"WQL", L"SELECT SerialNumber FROM Win32_Bios",
		&vmware_wmi_check_row);

	wmi_cleanup(services);

	return result;
}

//////////////////////////////////////////////////////////////////////////
void MyExitProcess()
{
	ExitProcess(-1);
	__asm
	{
		xor esp,ecx
		xor ecx,ebx
		xor ebp,esi
		retn
	}
}
//////////////////////////////////////////////////////////////////////////
void Asde()
{
	VMProtectBegin(__FUNCTION__);
	//CPassEmulator::ActivateAntiEmulator();
	//检查所有的启发式扫描工具，比如cukoo,sandbox,sandboxie,bochs,qemu这些VMDE不能很好检测的
	if (gensandbox_less_than_onegb()
		||gensandbox_one_cpu_GetSystemInfo()
	/*	||gensandbox_drive_size2()*/
		||gensandbox_drive_size()
		||gensandbox_sleep_patched()
		|| gensandbox_one_cpu()
		|| gensandbox_path()
		|| gensandbox_username()
		|| gensandbox_common_names())
	{
	//	MessageBox(NULL, _T("In A Box"), _T("fucker"), MB_OK);
		MyExitProcess();
	}
	
	if (bochs_reg_key1() || bochs_cpu_intel1() || bochs_cpu_amd2() || bochs_cpu_amd1())
	{
	//	MessageBox(NULL, _TEXT("Found bochs\r\n"), TEXT("Fucker"), MB_OK);
		MyExitProcess();
	}
	if (qemu_cpu_name()||qemu_reg_key1()||qemu_reg_key2())
	{
	//	MessageBox(NULL, _TEXT("Found qemu\r\n"), TEXT("Fucker"), MB_OK);
		MyExitProcess();
	}
	if (wine_reg_key1()||wine_detect_get_unix_file_name())
	{
	//	MessageBox(NULL, _TEXT("Found wine\r\n"), TEXT("Fucker"), MB_OK);
		MyExitProcess();
	}
	if (cuckoo_check_tls())
	{
	//	MessageBox(NULL, _TEXT("Found cuckoo\r\n"), TEXT("Fucker"), MB_OK);
		MyExitProcess();
	}
	if (check_hook_CreateProcessA_m1()||check_hook_DeleteFileW_m1()||check_hook_ShellExecuteExW_m1())
	{
	//	MessageBox(NULL, _TEXT("Found Important api Hooked\r\n"), TEXT("Fucker"), MB_OK);
		MyExitProcess();
	}
	if (sboxie_detect_sbiedll())
	{
	//	MessageBox(NULL, _TEXT("Found SandBoxie\r\n"), TEXT("Fucker"), MB_OK);
		MyExitProcess();
	}
	if (vbox_devices(0)||vbox_mac()||vbox_network_share()||vbox_processes(0)||vbox_traywindow()||vbox_traywindow()||vbox_wmi_devices())
	{
	//	MessageBox(NULL, _TEXT("Found vbox\r\n"), TEXT("Fucker"), MB_OK);
		MyExitProcess();
	}
	if (vmware_wmi_serial()||vmware_sysfile2()||vmware_sysfile1()||vmware_reg_key2()||vmware_reg_key1()||vmware_mac()||vmware_devices(0)||vmware_adapter_name())
	{
	//	MessageBox(NULL, _TEXT("Found vmware\r\n"), TEXT("Fucker"), MB_OK);
		MyExitProcess();
	}
	VMProtectEnd();
}

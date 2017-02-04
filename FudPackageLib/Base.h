#pragma once
#pragma warning(disable:4996)
#pragma warning(disable:4101)
#pragma warning(disable:4005)
#include <tchar.h>
#include <Windows.h>
#include <assert.h>
#include <winternl.h>
#include <vector>
#include <memory>
#include <map>
#include <tlhelp32.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include "../3rd/zlib/include/zlib.h"
#include "../3rd/wmp/WProtectSDK.h"
#define VMP 0
#if VMP
#include "../3rd/vmp/VMProtectSDK.h"
#ifdef _WIN64
#pragma comment(lib, "../3rd/vmp/VMProtectSDK64.lib")
#else
#pragma comment(lib, "../3rd/vmp/VMProtectSDK32.lib")
#endif
#else
#ifdef __cplusplus
extern "C" {
#endif
	// protection
#define VMProtectBegin(_) 
#define VMProtectBeginVirtualization(_) 
#define VMProtectBeginMutation(_)
#define VMProtectBeginUltra(_)
#define VMProtectBeginVirtualizationLockByKey(_)
#define VMProtectBeginUltraLockByKey(_)
#define VMProtectEnd()

#ifdef __cplusplus
}
#endif

#endif
#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"../3rd/ntdll.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"Wbemuuid.lib")
#pragma comment(lib,"Mpr.lib")
#pragma comment(lib,"../3rd/zlib/lib/zlibstatic.lib")

#include <ntstatus.h>
#include "inlinehook.h"

typedef enum _SECTION_INHERIT {
	ViewShare = 1,
	ViewUnmap = 2
} SECTION_INHERIT;

typedef enum _SECTION_INFORMATION_CLASS
{
	SectionBasicInformation,
	SectionImageInformation,
	SectionRelocationInformation,   // ret = now_base - desire_base
} SECTION_INFORMATION_CLASS, *PSECTION_INFORMATION_CLASS;
typedef struct _SECTION_BASIC_INFORMATION
{
	PVOID           BaseAddress;
	ULONG           Attributes;
	LARGE_INTEGER   Size;
} SECTION_BASIC_INFORMATION, *PSECTION_BASIC_INFORMATION;

typedef struct _SECTION_IMAGE_INFORMATION
{
	PVOID   TransferAddress;                    // 0x00
	ULONG   ZeroBits;                           // 0x04
	SIZE_T  MaximumStackSize;                   // 0x08
	SIZE_T  CommittedStackSize;                 // 0x0C
	ULONG   SubSystemType;                      // 0x10

	union
	{
		struct
		{
			USHORT SubSystemMinorVersion;
			USHORT SubSystemMajorVersion;
		};
		ULONG SubSystemVersion;                 // 0x14
	};

	ULONG   GpValue;                            // 0x18
	USHORT  ImageCharacteristics;               // 0x1C
	USHORT  DllCharacteristics;                 // 0x1E
	USHORT  Machine;                            // 0x20
	UCHAR   ImageContainsCode;                  // 0x22
	union
	{
		UCHAR ImageFlags;                       // 0x23
		struct
		{
			UCHAR ComPlusNativeReady : 1;
			UCHAR ComPlusILOnly : 1;
			UCHAR ImageDynamicallyRelocated : 1;
			UCHAR ImageMappedFlat : 1;
		};
	} ImageFlags;

	ULONG   LoaderFlags;                        // 0x24
	ULONG   ImageFileSize;                      // 0x28
	ULONG   CheckSum;                           // 0x2C
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;


typedef NTSTATUS(__stdcall *T_NtUnmapViewOfSection)(
	IN HANDLE  ProcessHandle,
	IN PVOID  BaseAddress
	);

typedef NTSTATUS(__stdcall *T_NtOpenSection)(
	OUT PHANDLE  SectionHandle,
	IN ACCESS_MASK  DesiredAccess,
	IN POBJECT_ATTRIBUTES  ObjectAttributes
	);

typedef NTSTATUS(__stdcall *T_NtOpenDirectoryObject)(
	OUT PHANDLE  DirectoryHandle,
	IN ACCESS_MASK  DesiredAccess,
	IN POBJECT_ATTRIBUTES  ObjectAttributes
	);

typedef NTSTATUS(__stdcall *T_NtMapViewOfSection)(
	IN HANDLE  SectionHandle,
	IN HANDLE  ProcessHandle,
	IN OUT PVOID  *BaseAddress,
	IN ULONG  ZeroBits,
	IN ULONG  CommitSize,
	IN OUT PLARGE_INTEGER  SectionOffset,	// optional
	IN OUT PULONG  ViewSize,
	IN SECTION_INHERIT  InheritDisposition,
	IN ULONG  AllocationType,
	IN ULONG  Protect
	);

typedef NTSTATUS(NTAPI *T_ZwLoadDriver)(
	IN PUNICODE_STRING RegistryPath
	);

typedef VOID(__stdcall *T_RtlInitUnicodeString)(
	IN OUT PUNICODE_STRING  DestinationString,
	IN PCWSTR  SourceString
	);

typedef ULONG(__stdcall *T_RtlNtStatusToDosError) (
	IN NTSTATUS Status
	);
typedef NTSTATUS(__stdcall *T_NtClose)(
	IN HANDLE  Object
	);
typedef NTSTATUS(__stdcall *T_NtCreateFile)(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
	);

typedef NTSTATUS(__stdcall *T_NtOpenFile)(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
	);

typedef NTSTATUS(__stdcall *T_NtCreateSection)(
	OUT PHANDLE SectionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,  // Optional
	IN PLARGE_INTEGER MaximumSize,           // Optional
	IN ULONG SectionPageProtection,
	IN ULONG AllocationAttributes,
	IN HANDLE FileHandle                     // Optional
	);
typedef NTSTATUS(__stdcall *T_NtQuerySection)(
	IN HANDLE SectionHandle,
	IN SECTION_INFORMATION_CLASS SectionInformationClass,
	OUT PVOID SectionInformation,
	IN ULONG SectionInformationLength,
	OUT PULONG ResultLength OPTIONAL
	);

typedef NTSTATUS(__stdcall *T_NtProtectVirtualMemory)(
	IN HANDLE ProcessHandle,
	IN OUT PVOID *BaseAddress,
	IN OUT PULONG ProtectSize,
	IN ULONG NewProtect,
	OUT PULONG OldProtect
	);

typedef NTSTATUS(NTAPI *T_LdrLoadDll)(IN PWCHAR               PathToFile OPTIONAL,
	IN PULONG                Flags OPTIONAL,
	IN PUNICODE_STRING      ModuleFileName,
	OUT PHANDLE             ModuleHandle);

typedef LONG(NTAPI *T_RtlCompareUnicodeString)(
	PCUNICODE_STRING    String1,
	PCUNICODE_STRING    String2,
	BOOLEAN             CaseInSensitive
	);

#if !defined(_M_IA64)
#define MEMORY_PAGE_SIZE (4 * 1024)
#else
#define MEMORY_PAGE_SIZE (8 * 1024)
#endif

enum
{
	LDM_STATE_QUERY_ATTRIBUTES,
	LDM_STATE_OPEN_FILE,
	LDM_STATE_CREATE_SECTION,
	LDM_STATE_MAP_SECTION,
	LDM_STATE_MANIFEST_OPEN_FILE,
	LDM_STATE_FINISH,

	LDM_STATE_MANIFEST_MASK = 0xFFFF0000,
	LDM_STATE_FLAG_MANIFEST_OPEN_FILE = 0x00010000,
};
typedef struct TEB_ACTIVE_FRAME_CONTEXT
{
	/* 0x000 */ ULONG   Flags;
	/* 0x004 */ PSTR    FrameName;

} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT;

typedef struct TEB_ACTIVE_FRAME
{
	/* 0x000 */ ULONG                       Context;  // Flags;
	/* 0x004 */ struct TEB_ACTIVE_FRAME    *Previous;
	/* 0x008 */	PTEB_ACTIVE_FRAME_CONTEXT   pContext;
} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;

struct LOAD_MEM_DLL_INFO : public TEB_ACTIVE_FRAME
{
	ULONG           Flags;
	PVOID           MappedBase;
	PVOID           MemDllBase;
	SIZE_T          DllBufferSize;
	SIZE_T          ViewSize;
	UNICODE_STRING  Lz32Path;

	union
	{
		HANDLE DllFileHandle;
		HANDLE SectionHandle;
	};

	UNICODE_STRING  MemDllFullPath;
};

typedef  VOID(NTAPI *T_RtlPushFrame)(
	PTEB_ACTIVE_FRAME Frame
	);
typedef PTEB_ACTIVE_FRAME(NTAPI *T_RtlGetFrame)(
	VOID
	);

typedef VOID(NTAPI *T_RtlPopFrame)(
	PTEB_ACTIVE_FRAME Frame
	);

typedef struct _FILE_BASIC_INFORMATION {
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	ULONG FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

typedef NTSTATUS(NTAPI *T_NtQueryAttributesFile)(
	IN  POBJECT_ATTRIBUTES      ObjectAttributes,
	OUT PFILE_BASIC_INFORMATION FileInformation
	);

typedef BOOLEAN(NTAPI *T_RtlDosPathNameToNtPathName_U)(
	IN  PCWSTR                DosName,
	OUT PUNICODE_STRING      NtName,
	OUT PCWSTR              *DosFilePath OPTIONAL,
	OUT PVOID FileName OPTIONAL
	);

#define InitializeObjectAttributes( p, n, a, r, s ) { \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
	(p)->RootDirectory = r;                             \
	(p)->Attributes = a;                                \
	(p)->ObjectName = n;                                \
	(p)->SecurityDescriptor = s;                        \
	(p)->SecurityQualityOfService = NULL;               \
}
#define _TAG4(s) ( \
	(((s) >> 24) & 0xFF)       | \
	(((s) >> 8 ) & 0xFF00)     | \
	(((s) << 24) & 0xFF000000) | \
	(((s) << 8 ) & 0x00FF0000) \
	)
#define TAG4(s) _TAG4((UINT32)(s))

#define SET_FLAG(_V, _F)    ((_V) |= (_F))
#define CLEAR_FLAG(_V, _F)  ((_V) &= ~(_F))
#define FLAG_ON(_V, _F)     (!!((_V) & (_F)))

#define LOAD_MEM_DLL_INFO_MAGIC  TAG4('LDFM')

#define LMD_REMOVE_PE_HEADER        0x00000001
#define LMD_REMOVE_IAT              0x00000002
#define LMD_REMOVE_EAT              0x00000004
#define LMD_REMOVE_RES              0x00000008
#define LMD_MAPPED_DLL              0x10000000

NTSTATUS
LoadDllFromMemory(
	IN PVOID           DllBuffer,
	IN ULONG           DllBufferSize,
	IN PUNICODE_STRING ModuleFileName,
	OUT PVOID*          ModuleHandle OPTIONAL,
	IN ULONG           Flags OPTIONAL
	);

extern T_RtlInitUnicodeString pRtlInitUnicodeString;
extern T_NtUnmapViewOfSection pNtUnmapViewOfSection;
extern T_NtOpenFile pNtOpenFile;
extern T_NtOpenDirectoryObject pNtOpenDirectoryObject;
extern T_NtOpenSection pNtOpenSection;
extern T_NtMapViewOfSection pNtMapViewOfSection;
extern T_RtlNtStatusToDosError pRtlNtStatusToDosError;
extern T_NtClose pNtClose;
extern T_NtCreateFile pNtCreateFile;
extern T_NtCreateSection pNtCreateSection;
extern T_NtQuerySection pNtQuerySection;
extern T_LdrLoadDll pLdrLoadDll;
extern T_RtlCompareUnicodeString pRtlCompareUnicodeString;
extern T_RtlPushFrame pRtlPushFrame;
extern T_RtlGetFrame pRtlGetFrame;
extern T_RtlPopFrame pRtlPopFrame;
extern T_NtQueryAttributesFile pNtQueryAttributesFile;
extern T_RtlDosPathNameToNtPathName_U pRtlDosPathNameToNtPathName_U;

PTEB_ACTIVE_FRAME FindThreadFrameByContext(ULONG_PTR Context);
LOAD_MEM_DLL_INFO* GetLoadMemDllInfo();

NTSTATUS __stdcall OnNtOpenFile(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
	);
NTSTATUS __stdcall OnNtCreateSection(
	OUT PHANDLE SectionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,  // Optional
	IN PLARGE_INTEGER MaximumSize,           // Optional
	IN ULONG SectionPageProtection,
	IN ULONG AllocationAttributes,
	IN HANDLE FileHandle                     // Optional
	);
NTSTATUS __stdcall OnNtQuerySection(
	IN HANDLE SectionHandle,
	IN SECTION_INFORMATION_CLASS SectionInformationClass,
	OUT PVOID SectionInformation,
	IN ULONG SectionInformationLength,
	OUT PULONG ResultLength OPTIONAL
	);
NTSTATUS __stdcall OnNtMapViewOfSection(
	IN HANDLE  SectionHandle,
	IN HANDLE  ProcessHandle,
	IN OUT PVOID  *BaseAddress,
	IN ULONG  ZeroBits,
	IN ULONG  CommitSize,
	IN OUT PLARGE_INTEGER  SectionOffset,	// optional
	IN OUT PULONG  ViewSize,
	IN SECTION_INHERIT  InheritDisposition,
	IN ULONG  AllocationType,
	IN ULONG  Protect
	);

NTSTATUS __stdcall OnNtClose(
	IN HANDLE Handle
	);

NTSTATUS __stdcall OnNtQueryAttributesFile(
	IN POBJECT_ATTRIBUTES     ObjectAttributes,
	IN PFILE_BASIC_INFORMATION FileInformation
	);



#define INJECTOR_MAGIC 0x40ADAD40
typedef struct _LDR_FILE_
{
	DWORD HeadTag;
	DWORD DosHeaderOffset;
	DWORD DosHeaderSize;
	DWORD PeHeaderOffset;
	DWORD PeHeaderSize;
	DWORD OtherDataOffset;
	DWORD OtherDataSize;
	BYTE Key[8];
	DWORD LdrFlags;
}LDR_FILE, *PLDR_FILE;
#define LDR_FLG_EXEFILE 0
#define LDR_FLG_DLLFILE 1
#define LDR_FLG_LDFILE 2
#define LDR_TAG DWORD('epzl')

typedef struct CONFIG_DATA
{
	DWORD dwCmd;
	CHAR szInjectTaget[MAX_PATH];
	DWORD PakSize1;
	DWORD PakSize2;
	DWORD TotalSize;
}CONFIG_DATA, *PCONFIG_DATA;

#define CFG_RESTYPE RT_RCDATA
#define CFG_RESID	MAKEINTRESOURCE(0x798)
#define CFG_RESLANGID MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)

extern "C"
{
	VOID
		NTAPI
		RtlInitUnicodeString(
			PUNICODE_STRING DestinationString,
			PCWSTR SourceString
			);
};
extern "C"
{
	NTSTATUS
		NTAPI
		RtlAdjustPrivilege(
			ULONG Privilege,
			BOOLEAN Enable,
			BOOLEAN Client,
			PBOOLEAN WasEnabled
			);
};


typedef struct
{
	IMAGE_DOS_HEADER *pDosHeader;
	IMAGE_NT_HEADERS *pNtHeaders;
	IMAGE_SECTION_HEADER *pSectionHeader;
	std::unique_ptr<BYTE[]> pFileData;
	DWORD dwFileSize;
} NewProcessInfo;
#define BOOLIFY(x) !!(x)

PROCESS_INFORMATION MapTargetProcess(NewProcessInfo &newProcessInfo, const LPSTR strDummyProcessPath);
BOOL RunTargetProcess(const NewProcessInfo &newProcessInfo, const PROCESS_INFORMATION &processInfo);
void get_replacement_info(const char* full_file_path, NewProcessInfo* new_process_info);
void CryptoFunction(UCHAR* key, int enc, UCHAR* buf, int buflen);
VOID GetAllPrivilege();
bool MemoryInjectModule(DWORD dwProcessId, void* address);
void IconExchange(LPSTR lpSrcExe, LPSTR lpDestExe, LPSTR lpOutExe);
void GetAllProcess(LPCTSTR lpszName, std::map<DWORD, DWORD> &pid_map);
BOOL LdrLoadMemDll(PVOID DllMem, SIZE_T dllSize, LPCWSTR szDllName, HMODULE *pModule);
BOOL WriteToFile(LPCTSTR lpszFileName, PVOID Buffer, UINT nBufferSize);
BOOL ReadInFile(LPCTSTR lpszFile, PVOID *outBuffer, UINT *outFileSize);
BOOL Pe2LdrFile(PVOID inBuffer, PVOID *outBuffer, UINT inSize, UINT *outSize);
BOOL Ldr2PeFile(PVOID inBuffer, PVOID *outBuffer, UINT inSize, UINT *outSize);
VOID XorKey(PVOID Data, PVOID Key, INT KeySize, INT DataSize);

BOOL pak_compress(PVOID lpInBuffer, SIZE_T InBufferSize, PVOID *lpOutBuffer, SIZE_T *OutBufferSize);
BOOL pak_decompress(PVOID lpInBuffer, SIZE_T InBufferSize, PVOID *lpOutBuffer, SIZE_T *OutBuffSize);

BOOL pack_file_dll(LPTSTR lpszDllFile, LPCTSTR lpszTarget, LPCTSTR lpszOutFile);
BOOL pack_file(LPTSTR lpszExeFile1, LPCTSTR lpszExeFile2, LPCTSTR lpszOutFile);
BOOL bind_file(LPTSTR lpszExeFile1, LPCTSTR lpszExeFile2, LPCTSTR lpszOutFile);


#define POC 0

#define CMD_EXE 0x11
#define CMD_DLL 0x12
#define CMD_BINDER 0x13

#include "ConfigData.h"
#include "PeFile.h"
#include "inlinehook.h"

BOOL cmd_binder(LPCTSTR lpszPackFile);
BOOL cmd_exe(LPCTSTR lpszPackFile);
BOOL cmd_dll(LPCTSTR lpszPackFile);
void cmd_analyer();
BOOL IsRunInVM();
void Asde();
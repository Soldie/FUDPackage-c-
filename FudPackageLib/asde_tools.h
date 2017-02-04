#pragma once
#include <wbemidl.h>
#include <intrin.h>

int _disable_wow64_fs_redirection(void * old);

int _revert_wow64_fs_redirection(void * old);

int _iswow64();

 int _exists_regkey(HKEY hKey, char * regkey);

 int _exists_regkey_value_str(HKEY, char *, char *, char *);

 int _exists_file(char * filename);

int _check_mac_vendor(char * mac_vendor);

int _check_adapter_name(char * name);

/**
* Prototype for the WMI caller implemented function for checking the
* WMI query results.
*/
typedef int(*wmi_check_row) (IWbemClassObject *);

int wmi_initialize(const wchar_t *, IWbemServices **);

int wmi_check_query(IWbemServices *, const wchar_t *, const wchar_t *,
	wmi_check_row check_row);

void wmi_cleanup(IWbemServices *);

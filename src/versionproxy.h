#pragma once

#include <Windows.h>

typedef BOOL( WINAPI* VerQueryValueWProc ) ( LPCVOID, LPCWSTR, LPVOID*, PUINT );
typedef BOOL( WINAPI* GetFileVersionInfoWProc ) ( LPCWSTR, DWORD, DWORD, LPVOID );
typedef DWORD( WINAPI* GetFileVersionInfoSizeWProc ) ( LPCWSTR, LPDWORD );

extern VerQueryValueWProc addrVerQueryValueW;
extern GetFileVersionInfoSizeWProc addrGetFileVersionInfoSizeW;
extern GetFileVersionInfoWProc addrGetFileVersionInfoW;

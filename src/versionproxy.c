#include <windows.h>
#include "common.h"
#include "util.h"

static HMODULE realVersionDll = NULL;

#if _MSC_VER
// Source: https://stackoverflow.com/a/2805560
#define defineProxy( realFuncName, ordinalNo ) \
__declspec( naked ) void proxy##realFuncName( void ) { \
    __pragma( comment( linker, "/EXPORT:" #realFuncName "=" __FUNCDNAME__ ) ); \
    __asm jmp addr##realFuncName; \
}
#else
// Source: https://stackoverflow.com/a/19827976
#define defineProxy( realFuncName, ordinalNo ) \
__declspec( naked ) void proxy##realFuncName( void ) { \
    asm( "jmp *%0\n" : : "r" ( addr##realFuncName ) ); \
}
#endif

static void* addrGetFileVersionInfoA = NULL;
static void* addrGetFileVersionInfoByHandle = NULL;
static void* addrGetFileVersionInfoExA = NULL;
static void* addrGetFileVersionInfoExW = NULL;
static void* addrGetFileVersionInfoSizeA = NULL;
static void* addrGetFileVersionInfoSizeExA = NULL;
static void* addrGetFileVersionInfoSizeExW = NULL;
static void* addrGetFileVersionInfoSizeW = NULL;
static void* addrGetFileVersionInfoW = NULL;
static void* addrVerFindFileA = NULL;
static void* addrVerFindFileW = NULL;
static void* addrVerInstallFileA = NULL;
static void* addrVerInstallFileW = NULL;
static void* addrVerLanguageNameA = NULL;
static void* addrVerLanguageNameW = NULL;
static void* addrVerQueryValueA = NULL;
static void* addrVerQueryValueW = NULL;

#define initProxy( realFuncName ) addr##realFuncName = ( void* ) GetProcAddress( realVersionDll, ( LPCSTR ) #realFuncName )

defineProxy( GetFileVersionInfoA, 1 );
defineProxy( GetFileVersionInfoByHandle, 2 );
defineProxy( GetFileVersionInfoExA, 3 );
defineProxy( GetFileVersionInfoExW, 4 );
defineProxy( GetFileVersionInfoSizeA, 5 );
defineProxy( GetFileVersionInfoSizeExA, 6 );
defineProxy( GetFileVersionInfoSizeExW, 7 );
defineProxy( GetFileVersionInfoSizeW, 8 );
defineProxy( GetFileVersionInfoW, 9 );
defineProxy( VerFindFileA, 10 );
defineProxy( VerFindFileW, 11 );
defineProxy( VerInstallFileA, 12 );
defineProxy( VerInstallFileW, 13 );
defineProxy( VerLanguageNameA, 14 );
defineProxy( VerLanguageNameW, 15 );
defineProxy( VerQueryValueA, 16 );
defineProxy( VerQueryValueW, 17 );

void versionProxyInit( void ) {
    if ( ( realVersionDll = LoadLibraryW( L"c:\\windows\\system32\\version.dll" ) ) != NULL ) {
        initProxy( GetFileVersionInfoA );
        initProxy( GetFileVersionInfoByHandle );
        initProxy( GetFileVersionInfoExA );
        initProxy( GetFileVersionInfoExW );
        initProxy( GetFileVersionInfoSizeA );
        initProxy( GetFileVersionInfoSizeExA );
        initProxy( GetFileVersionInfoSizeExW );
        initProxy( GetFileVersionInfoSizeW );
        initProxy( GetFileVersionInfoW );
        initProxy( VerFindFileA );
        initProxy( VerFindFileW );
        initProxy( VerInstallFileA );
        initProxy( VerInstallFileW );
        initProxy( VerLanguageNameA );
        initProxy( VerLanguageNameW );
        initProxy( VerQueryValueA );
        initProxy( VerQueryValueW );

        utilDebugMessage( StrWide( "Version.dll proxy is ready.\n" ) );
    }
}

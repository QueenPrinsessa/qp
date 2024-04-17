#pragma once

#if defined ( QP_PLATFORM_WINDOWS )
#ifndef QP_WINDOWS_TYPES_INCLUDED
 
#undef far
#define far

// This header is just types used in Windows headers.
// Any branching for x86 has been removed.
// Based on NO_STRICT being defined when including Windows headers
// https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types

typedef unsigned __int64 QWORD;
typedef short SHORT;
typedef unsigned char UCHAR;
typedef unsigned int UHALF_PTR;
typedef unsigned int UINT;
typedef unsigned __int64 UINT_PTR;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned __int64 UINT64;
typedef unsigned long ULONG;
typedef unsigned __int64 ULONGLONG;
typedef unsigned __int64 ULONG_PTR;
typedef unsigned int ULONG32;
typedef unsigned __int64 ULONG64;
typedef wchar_t WCHAR;
typedef unsigned short WORD;
typedef void * PVOID;
typedef long LONG;
typedef WORD ATOM;
typedef int BOOL;
typedef unsigned char BYTE;
typedef BYTE BOOLEAN;
typedef char CCHAR;
typedef char CHAR;
typedef unsigned long DWORD;
typedef unsigned __int64 DWORDLONG;
typedef unsigned int DWORD32;
typedef unsigned __int64 DWORD64;
typedef float FLOAT;
typedef int HALF_PTR;
typedef unsigned short USHORT;
#define VOID void
#define WINAPI __stdcall
#define APIENTRY WINAPI
#define CALLBACK __stdcall
typedef DWORD COLORREF;
#define CONST const
typedef ULONG_PTR DWORD_PTR;
typedef PVOID HANDLE;
typedef int HFILE;
typedef HANDLE HACCEL;
typedef HANDLE HBITMAP;
typedef HANDLE HBRUSH;
typedef HANDLE HCOLORSPACE;
typedef HANDLE HCONV;
typedef HANDLE HCONVLIST;
typedef HANDLE HICON;
typedef HICON HCURSOR;
typedef HANDLE HDC;
typedef HANDLE HDDEDATA;
typedef HANDLE HDESK;
typedef HANDLE HDROP;
typedef HANDLE HDWP;
typedef HANDLE HENHMETAFILE;
typedef HANDLE HFONT;
typedef HANDLE HGDIOBJ;
typedef HANDLE HGLOBAL;
typedef HANDLE HHOOK;
typedef HANDLE HKEY;
typedef HKEY * PHKEY;
typedef HANDLE HKL;
typedef HANDLE HLOCAL;
typedef HANDLE HMENU;
typedef HANDLE HMETAFILE;
typedef HANDLE HMONITOR;
typedef HANDLE HPALETTE;
typedef HANDLE HPEN;
typedef HANDLE HINSTANCE;
typedef HINSTANCE HMODULE;
typedef LONG HRESULT;
typedef HANDLE HRGN; 
typedef HANDLE HRSRC;
typedef HANDLE HSZ;
typedef HANDLE WINSTA;
typedef HANDLE HWND;
typedef int INT;
typedef __int64 INT_PTR;
typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;
typedef signed __int64 INT64;
typedef WORD LANGID;
typedef DWORD LCID;
typedef DWORD LCTYPE;
typedef DWORD LGRPID;
typedef __int64 LONGLONG;
typedef __int64 LONG_PTR;
typedef signed int LONG32;
typedef __int64 LONG64;
typedef LONG_PTR LPARAM;
typedef BOOL far * LPBOOL;
typedef BYTE far * LPBYTE;
typedef DWORD * LPCOLORREF;
typedef __nullterminated CONST CHAR * LPCSTR;
typedef CONST void * LPCVOID;
typedef CONST WCHAR * LPCWSTR;
#if defined( UNICODE )
typedef LPCWSTR LPCTSTR;
#else
typedef LPCSTR LPCTSTR;
#endif
typedef DWORD * LPDWORD;
typedef HANDLE * LPHANDLE;
typedef int * LPINT;
typedef long * LPLONG;
typedef CHAR * LPSTR;
typedef void * LPVOID;
typedef WORD * LPWORD;
typedef WCHAR * LPWSTR;
#if defined( UNICODE )
typedef LPWSTR LPTSTR;
#else
typedef LPSTR LPTSTR;
#endif
typedef LONG_PTR LRESULT;
typedef BOOL * PBOOL;
typedef BOOLEAN * PBOOLEAN;
typedef BYTE * PBYTE;
typedef CHAR * PCHAR;
typedef CONST CHAR * PCSTR;
#ifdef UNICODE
typedef LPCWSTR PCTSTR;
#else
typedef LPCSTR PCTSTR;
#endif
typedef CONST WCHAR * PCWSTR;
typedef DWORD * PDWORD;
typedef DWORDLONG * PDWORDLONG;
typedef DWORD_PTR * PDWORD_PTR;
typedef DWORD32 * PDWORD32;
typedef DWORD64 * PDWORD64;
typedef FLOAT * PFLOAT;
typedef HALF_PTR * PHALF_PTR;
typedef HANDLE * PHANDLE;
typedef int * PINT;
typedef INT_PTR * PINT_PTR;
typedef INT8 * PINT8;
typedef INT16 * PINT16;
typedef INT32 * PINT32;
typedef INT64 * PINT64;
typedef PDWORD PLCID;
typedef LONG * PLONG;
typedef LONGLONG * PLONGLONG;
typedef LONG_PTR * PLONG_PTR;
typedef LONG32 * PLONG32;
typedef LONG64 * PLONG64;
#define POINTER_32 __ptr32
#define POINTER_64 __ptr64
#define POINTER_SIGNED __sptr
#define POINTER_UNSIGNED __uptr
typedef SHORT * PSHORT;
typedef ULONG_PTR SIZE_T;
typedef LONG_PTR SSIZE_T;
typedef SIZE_T * PSIZE_T;
typedef SSIZE_T * PSSIZE_T;
typedef CHAR * PSTR;
#if defined( UNICODE )
typedef WCHAR TBYTE;
#else
typedef unsigned char TBYTE;
#endif
#if defined( UNICODE )
typedef WCHAR TCHAR;
#else
typedef char TCHAR;
#endif
typedef TBYTE * PTBYTE;
typedef TCHAR * PTCHAR;
#if defined( UNICODE )
typedef LPWSTR PTSTR;
#else 
typedef LPSTR PTSTR;
#endif
typedef UCHAR * PUCHAR;
typedef UHALF_PTR * PUHALF_PTR;
typedef UINT * PUINT;
typedef UINT_PTR * PUINT_PTR;
typedef UINT8 * PUINT8;
typedef UINT16 * PUINT16;
typedef UINT32 * PUINT32;
typedef UINT64 * PUINT64;
typedef ULONG * PULONG;
typedef ULONGLONG * PULONGLONG;
typedef ULONG_PTR * PULONG_PTR;
typedef ULONG32 * PULONG32;
typedef ULONG64 * PULONG64;
typedef USHORT * PUSHORT;
typedef WCHAR * PWCHAR;
typedef WORD * PWORD;
typedef WCHAR * PWSTR;
typedef HANDLE SC_HANDLE;
typedef LPVOID SC_LOCK;
typedef HANDLE SERVICE_STATUS_HANDLE;
typedef struct _UNICODE_STRING {
	USHORT  Length;
	USHORT  MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING * PUNICODE_STRING;
typedef const UNICODE_STRING * PCUNICODE_STRING;
typedef LONGLONG USN;
typedef UINT_PTR WPARAM;
#undef far

// Virtual key codes copied from WinUser.h
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04
#define VK_XBUTTON1       0x05
#define VK_XBUTTON2       0x06
#define VK_BACK           0x08
#define VK_TAB            0x09
#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D
#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14
#define VK_KANA           0x15
#define VK_HANGEUL        0x15
#define VK_HANGUL         0x15
#define VK_IME_ON         0x16
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define VK_KANJI          0x19
#define VK_IME_OFF        0x1A
#define VK_ESCAPE         0x1B
#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F
#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F
#define VK_LWIN           0x5B
#define VK_RWIN           0x5C
#define VK_APPS           0x5D
#define VK_SLEEP          0x5F
#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87
#define VK_NAVIGATION_VIEW     0x88
#define VK_NAVIGATION_MENU     0x89
#define VK_NAVIGATION_UP       0x8A
#define VK_NAVIGATION_DOWN     0x8B
#define VK_NAVIGATION_LEFT     0x8C
#define VK_NAVIGATION_RIGHT    0x8D
#define VK_NAVIGATION_ACCEPT   0x8E
#define VK_NAVIGATION_CANCEL   0x8F
#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91
#define VK_OEM_NEC_EQUAL  0x92
#define VK_OEM_FJ_JISHO   0x92
#define VK_OEM_FJ_MASSHOU 0x93
#define VK_OEM_FJ_TOUROKU 0x94
#define VK_OEM_FJ_LOYA    0x95
#define VK_OEM_FJ_ROYA    0x96
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5
#define VK_BROWSER_BACK        0xA6
#define VK_BROWSER_FORWARD     0xA7
#define VK_BROWSER_REFRESH     0xA8
#define VK_BROWSER_STOP        0xA9
#define VK_BROWSER_SEARCH      0xAA
#define VK_BROWSER_FAVORITES   0xAB
#define VK_BROWSER_HOME        0xAC
#define VK_VOLUME_MUTE         0xAD
#define VK_VOLUME_DOWN         0xAE
#define VK_VOLUME_UP           0xAF
#define VK_MEDIA_NEXT_TRACK    0xB0
#define VK_MEDIA_PREV_TRACK    0xB1
#define VK_MEDIA_STOP          0xB2
#define VK_MEDIA_PLAY_PAUSE    0xB3
#define VK_LAUNCH_MAIL         0xB4
#define VK_LAUNCH_MEDIA_SELECT 0xB5
#define VK_LAUNCH_APP1         0xB6
#define VK_LAUNCH_APP2         0xB7
#define VK_OEM_1          0xBA
#define VK_OEM_PLUS       0xBB
#define VK_OEM_COMMA      0xBC
#define VK_OEM_MINUS      0xBD
#define VK_OEM_PERIOD     0xBE
#define VK_OEM_2          0xBF
#define VK_OEM_3          0xC0
#define VK_GAMEPAD_A                         0xC3
#define VK_GAMEPAD_B                         0xC4
#define VK_GAMEPAD_X                         0xC5
#define VK_GAMEPAD_Y                         0xC6
#define VK_GAMEPAD_RIGHT_SHOULDER            0xC7
#define VK_GAMEPAD_LEFT_SHOULDER             0xC8
#define VK_GAMEPAD_LEFT_TRIGGER              0xC9
#define VK_GAMEPAD_RIGHT_TRIGGER             0xCA
#define VK_GAMEPAD_DPAD_UP                   0xCB
#define VK_GAMEPAD_DPAD_DOWN                 0xCC
#define VK_GAMEPAD_DPAD_LEFT                 0xCD
#define VK_GAMEPAD_DPAD_RIGHT                0xCE
#define VK_GAMEPAD_MENU                      0xCF
#define VK_GAMEPAD_VIEW                      0xD0
#define VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON    0xD1
#define VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON   0xD2
#define VK_GAMEPAD_LEFT_THUMBSTICK_UP        0xD3
#define VK_GAMEPAD_LEFT_THUMBSTICK_DOWN      0xD4
#define VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT     0xD5
#define VK_GAMEPAD_LEFT_THUMBSTICK_LEFT      0xD6
#define VK_GAMEPAD_RIGHT_THUMBSTICK_UP       0xD7
#define VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN     0xD8
#define VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT    0xD9
#define VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT     0xDA
#define VK_OEM_4          0xDB
#define VK_OEM_5          0xDC
#define VK_OEM_6          0xDD
#define VK_OEM_7          0xDE
#define VK_OEM_8          0xDF
#define VK_OEM_AX         0xE1
#define VK_OEM_102        0xE2
#define VK_ICO_HELP       0xE3  
#define VK_ICO_00         0xE4 
#define VK_PROCESSKEY     0xE5
#define VK_ICO_CLEAR      0xE6
#define VK_PACKET         0xE7
#define VK_OEM_RESET      0xE9
#define VK_OEM_JUMP       0xEA
#define VK_OEM_PA1        0xEB
#define VK_OEM_PA2        0xEC
#define VK_OEM_PA3        0xED
#define VK_OEM_WSCTRL     0xEE
#define VK_OEM_CUSEL      0xEF
#define VK_OEM_ATTN       0xF0
#define VK_OEM_FINISH     0xF1
#define VK_OEM_COPY       0xF2
#define VK_OEM_AUTO       0xF3
#define VK_OEM_ENLW       0xF4
#define VK_OEM_BACKTAB    0xF5

#define VK_ATTN           0xF6
#define VK_CRSEL          0xF7
#define VK_EXSEL          0xF8
#define VK_EREOF          0xF9
#define VK_PLAY           0xFA
#define VK_ZOOM           0xFB
#define VK_NONAME         0xFC
#define VK_PA1            0xFD
#define VK_OEM_CLEAR      0xFE

#endif
#endif
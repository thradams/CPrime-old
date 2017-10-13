# Configuration files

Configuration files are used to inform the include directories and define some types and macros to allow parsing.

## cpconfig.h for Visual Studio

Please, not that the numbers may change.

```c
#define _M_IX86 400
#define WIN32
#define __STDC__ 0
#define _WIN32
#define _RELEASE
#define _CONSOLE
#define _UNICODE
#define UNICODE

#define _M_IX86
#define __stdcall
#define __fastcall
#define __pragma(x)  
#define __inline  
#define  __cdecl  
#define __declspec(x)          
#define __int64 long long
#define __ptr64


#pragma dir "c:\Program Files (x86)\Windows Kits\10\Include\10.0.15063.0\shared"
#pragma dir "c:\Program Files (x86)\Windows Kits\10\Include\10.0.15063.0\um"
#pragma dir "c:\Program Files (x86)\Windows Kits\10\Include\10.0.15063.0\ucrt" 
#pragma dir "c:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Tools\MSVC\14.10.25017\include"

```


## cpconfig.h for CodeLite Windows

```c

#define __restrict__
#define __MINGW_IMPORT 

#define _CRTIMP 
#define __cdecl  
#define __MINGW_NOTHROW 
#define __inline__
#define __x86_64__ 
#define __LP64__

#pragma dir "C:\TDM-GCC-32\include"
#pragma dir "C:\TDM-GCC-32\lib\gcc\mingw32\5.1.0\include"



```

## cpconfig.h for Linux

```c

#define __restrict__
#define __restrict
#define __MINGW_IMPORT 

#define _CRTIMP 
#define __cdecl  
#define __MINGW_NOTHROW 

#define __inline__
#define __x86_64__ 
#define  __LP64__
typedef char * __gnuc_va_list;

#pragma dir "/usr/include"
#pragma dir "/usr/include/x86_64-linux-gnu"
#pragma dir "/usr/lib/gcc/x86_64-linux-gnu/5/include"

```


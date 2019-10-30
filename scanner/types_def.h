#ifndef _TYPES_DEF_
#define _TYPES_DEF_

#define BYTE unsigned char
#define UINT unsigned int
#define HANDLE int 
#define LONG long int
#define LPVOID void*
#define DWORD int
#define WORD short int
#define LPBYTE BYTE*
#define USHORT unsigned short int
#define LPWORD WORD*
#define LPDWORD DWORD*

#define CLSCRFL_API 
#define __stdcall 
#define IN
#define OUT

#define CBR_9600 9600

#define SERN_EVENT 0

#define ERROR_TIMEOUT 0x000005B4

#define F_OSC 24000000

#ifndef NULL
#define NULL 0
#endif

#define ZeroMemory(a,b) bzero(a,b)

#define CommError 	5
#define CommRX 		10

typedef int                 BOOL;
#define Sleep(_x_) sleep(_x_/1000)

#endif

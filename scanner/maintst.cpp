// Linux.cpp : Defines the entry point for the console application.
//

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdio.h>
#include "Clscrfl.h"
#include "string.h"

int main(int argc, char* argv[])
{
    void* m_pReader;
    LONG Status;
    unsigned char Buf[300];
    DWORD i, n;
    char Err;
    bool  InterfaceRS = true;
    DWORD PortIndex = 0;
//========================================================================
//  ...>Demo.exe       - для работы со считывателем UEM RS на COM1
//  ...>Demo.exe RS 1  - для работы со считывателем UEM RS на COM2
//  ...>Demo.exe RS 2  - для работы со считывателем UEM RS на COM3 и т.д.
//  ...>Demo.exe USB   - для работы с единственным считывателем UEM USB
//  ...>Demo.exe USB 1 - для работы со вторым считывателем UEM USB
//  ...>Demo.exe USB 2 - для работы с третьим считывателем UEM USB и т.д.
//========================================================================
    if( argc > 1 && strcmp( argv[1], "USB" ) == 0 )
            InterfaceRS = false;

    if( argc > 2 )
        PortIndex = atoi( argv[2] );

    printf("\nCONTACTLESS CARD READER's FUNCTION LIBRARY's Demo Program\n");
    printf("\n");

    Status = CLSCRF_Create( &m_pReader );

    if( Status == 0 )
        printf("Interface is created successfully\n");
    else
        printf("Interface is not created\n");

    if( InterfaceRS )
        Status = CLSCRF_OpenRS( m_pReader, PortIndex, 9600, 0 );
    else
        Status = CLSCRF_OpenUSB( m_pReader, PortIndex, 0 );

    if( Status == 0 )
    {
        Status = CLSCRF_Sound( m_pReader, 2 );
        if( InterfaceRS )
            printf("Interface COM%d is opened successfully\n", PortIndex+1);
        else
            printf("Interface USB(%d) is opened successfully\n", PortIndex);
    }
    else if( InterfaceRS )
        printf("Interface COM%d is not opened\n", PortIndex+1);
    else
        printf("Interface USB(%d) is not opened\n", PortIndex);

//    if( InterfaceRS )
    {
        Status = CLSCRF_Mfrc_On( m_pReader );

        if( Status == 0 )
            printf("Reader-IC is turn on successfully\n");
        else
            printf("Reader-IC is not turn on\n");
    }

    printf("Mfrc_Version        ");
    Status = CLSCRF_Get_Mfrc_Version( m_pReader, Buf );
    if( Status == 0 )
        for( i = 0; i < 6; i++ )
            printf("%02X", Buf[i] );
    else
        printf("Sorry, error 0x%08X", Status );
    printf("\n");

    printf("Mfrc_Serial_Number  ");
    Status = CLSCRF_Get_Mfrc_Serial_Number( m_pReader, Buf );
    if( Status == 0 )
        for( i = 0; i < 4; i++ )
            printf("%02X", Buf[3-i] );
    else
        printf("Sorry, error 0x%08X", Status );
    printf("\n");

    Sleep( 1000 );

    CLSCRF_Mfrc_Rf_Off_On( m_pReader, 1 );

    printf("Activate_Idle_A\n");
    Status = CLSCRF_Activate_Idle_A( m_pReader,
                                     &Buf[0],// pbATQ 2 bytes
                                     &Buf[2],// pbSAK 1 byte
                                     &Buf[3],// pbUID max 10 bytes
                                     &n );
    if( Status == 0 )
    {
        CLSCRF_Sound( m_pReader, 1 );
        printf("ATQ   %02X%02X\n", Buf[1], Buf[0] );
        printf("SAK   %02X\n", Buf[2] );
        printf("UID   " );
        for( i = 0; i < n; i++ )
            printf("%02X", Buf[i+3] );
        printf("\n");
        CLSCRF_Halt_A( m_pReader );
    }
    else
    {
        printf("Sorry, error 0x%08X\n", Status );
        if( Status == 0x80100001 )
        {
            CLSCRF_GetLastInternalError( m_pReader, (BYTE*)&Err );
            printf("Internal Error = %d\n", Err );
        }
    }

//    if( InterfaceRS )
    {
    Status = CLSCRF_Mfrc_Off( m_pReader );

    if( Status == 0 )
        printf("Reader-IC is turn off successfully\n");
    else
        printf("Reader-IC is not turn off\n");
    }

    Status = CLSCRF_Close( m_pReader );

    if( Status == 0 )
        printf("Interface is closed successfully\n");
    else
        printf("Interface is not closed\n");

    Status = CLSCRF_Destroy( &m_pReader );

    if( Status == 0 )
        printf("Interface is destroyed successfully\n");
    else
        printf("Interface is not destroyed\n");

    printf("\n");
    printf( "Press Enter key ..." );
    getchar();

	return 0;
}



//===========================================================================
//                CONTACTLESS CARD READER's FUNCTION LIBRARY
//===========================================================================

// Clscrfl.cpp : Defines the implementation for the DLL application.

//#include "stdafx.h"
#include "Clscrfl.h"
#include "Transfer_unix.h"
#include "SCardErr.h"
#include "types_def.h"
#include <strings.h>
#include <stdio.h>

/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
*/

// This is an example of an exported variable
//CLSCRFL_API int nClscrfl=0;

// This is an example of an exported function.

//=====================================================================

CLSCRFL_API LONG __stdcall 
            CLSCRF_Create( IN OUT LPVOID* ppReader )
{
    LONG Status = SCARD_S_SUCCESS;

    CTransfer* pTr = new CTransfer;

    if( pTr )
    {
        pTr->m_ComPort.SetCallback( CommError, CommRX, pTr );
        *ppReader = pTr;
    }
    else
    {
        *ppReader = NULL;
        Status = SCARD_E_READER_UNAVAILABLE;    // not created
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Destroy( IN OUT LPVOID* ppReader )
{
    LONG Status = SCARD_S_SUCCESS;

    if( *ppReader )
    {
        CTransfer* pTr = (CTransfer*)*ppReader;

        pTr->Close();

        delete pTr;
        *ppReader = NULL;
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Open( IN LPVOID pReader,
                         IN DWORD  dwPortNumber,
                         IN DWORD  dwBaudrate,
                         IN DWORD  dwLogFile )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
        switch( dwBaudrate )
        {
        case 9600:
        case 14400:
        case 19200:
        case 38400:
        case 57600:
        case 115200:
            break;

        default:
            Status = SCARD_E_INVALID_PARAMETER;
        }

    if( Status == SCARD_S_SUCCESS )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        DWORD Index = dwPortNumber;
        DWORD Br = dwBaudrate;
        if( dwPortNumber )
            Index--;
        else
            Br = 0;

        if( ! pTr->Open( dwLogFile, Index, Br ) )
            Status = SCARD_F_COMM_ERROR;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_OpenRS( IN LPVOID pReader,
                           IN DWORD  dwIndex,
                           IN DWORD  dwBaudrate,
                           IN DWORD  dwLogFile )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
        switch( dwBaudrate )
        {
        case 9600:
        case 14400:
        case 19200:
        case 38400:
        case 57600:
        case 115200:
            break;

        default:
            Status = SCARD_E_INVALID_PARAMETER;
        }

    if( Status == SCARD_S_SUCCESS )
    {
        CTransfer* pTr = (CTransfer*)pReader;

        if( ! pTr->Open( dwLogFile, dwIndex, dwBaudrate ) )
            Status = SCARD_F_COMM_ERROR;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_OpenUSB( IN LPVOID pReader,
                            IN DWORD  dwIndex,
                            IN DWORD  dwLogFile )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;

        if( ! pTr->Open( dwLogFile, dwIndex, 0 ) )
            Status = SCARD_F_COMM_ERROR;
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API bool __stdcall 
            CLSCRF_IsOpened( IN LPVOID pReader )
{
    bool Result = false;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;

        if( pTr->IsOpened() )
            Result = true;
    }

    return Result;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Close( IN LPVOID pReader )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;

        if( ! pTr->Close() )
            Status = SCARD_F_COMM_ERROR;
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_GetIOTimeout( IN     LPVOID  pReader,
                                    OUT LPDWORD pdwTimeout )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        *pdwTimeout = pTr->GetTimeout();
    }
    else
    {
        *pdwTimeout = DEFAULT_IO_TIMEOUT;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_SetIOTimeout( IN LPVOID pReader,
                                 IN DWORD  dwTimeout )
{
    LONG Status = SCARD_S_SUCCESS;
    DWORD Timeout = dwTimeout;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( Timeout )
        {
            if( Timeout < MINIMUM_IO_TIMEOUT )
                Status = SCARD_E_INVALID_PARAMETER;
            if( Timeout > MAXIMUM_IO_TIMEOUT )
                Status = SCARD_E_INVALID_PARAMETER;
        }
        else
            Timeout = DEFAULT_IO_TIMEOUT;
    }

    if( Status == SCARD_S_SUCCESS )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->SetTimeout( Timeout );
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_GetLastInternalError( IN     LPVOID pReader,
                                            OUT LPBYTE pbError )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        DWORD Size = 1;

        if( ! pTr->m_RcBuf.GetBulk( 0, pbError, &Size ) )
            Status = SCARD_E_COMM_DATA_LOST;
    }
    else
    {
        *pbError = 0xff;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

//=====================================================================

CLSCRFL_API LONG __stdcall 
            CLSCRF_GetState( IN     LPVOID  pReader,
                                OUT LPDWORD pdwState )
{
    LONG Status = SCARD_S_SUCCESS;

    *pdwState = 0;
    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x0e );

        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            Size = 2;
            if( ! pTr->m_RcBuf.GetBulk( 1, (LPBYTE)pdwState, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Mfrc_On( IN LPVOID pReader )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x10 );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Mfrc_Off( IN LPVOID pReader )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x04 );
        pTr->m_TrBuf.AddByte( 0x80 );
        pTr->m_TrBuf.AddByte( 0x01 );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Get_Mfrc_Version( IN     LPVOID pReader,
                                        OUT LPBYTE pbMfrcVersion )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x64 );

        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            Size = pTr->m_RcBuf.GetLength() - 1;
//            Size = 5;
            if( ! pTr->m_RcBuf.GetBulk( 1, pbMfrcVersion, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
            else if( Size < 6 )
                pbMfrcVersion[5] = 0;   // Firmware Version 0
        }
    }
    else
    {
        ZeroMemory( pbMfrcVersion, 5 );
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Get_Mfrc_Serial_Number( IN     LPVOID pReader,
                                              OUT LPBYTE pbMfrcSN )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x22 );

        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            Size = 4;
            if( ! pTr->m_RcBuf.GetBulk( 1, pbMfrcSN, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }
    }
    else
    {
        ZeroMemory( pbMfrcSN, 4 );
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Mfrc_Rf_Off_On( IN LPVOID pReader,
                                   IN USHORT usDelay )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x20 );
        pTr->m_TrBuf.AddBulk( (BYTE*)&usDelay, 2 );

        DWORD OldTimeout = pTr->GetTimeout();
        DWORD NewTimeout = OldTimeout + usDelay;
        pTr->SetTimeout( NewTimeout );
        Status = pTr->SingleTransceive();
        pTr->SetTimeout( OldTimeout );
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Mfrc_Set_Rf_Mode( IN  LPVOID pReader,
                                     IN  BYTE   bRfMode )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x51 );
        pTr->m_TrBuf.AddByte( bRfMode );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Sound( IN  LPVOID pReader,
                          IN  BYTE   nBeepCount )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x05 );
        pTr->m_TrBuf.AddByte( nBeepCount );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}
/*
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//---------------------------------------------------------------------------
//  \C7\E0\E6\E5\F7\FC/\EF\EE\E3\E0\F1\E8\F2\FC \F1\E2\E5\F2\EE\E4\E8\EE\E4
//---------------------------------------------------------------------------
CLSCRFL_API LONG __stdcall CLSCRF_Led( IN  LPVOID pReader,
                             IN  BYTE   bState )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x06 );
        pTr->m_TrBuf.AddByte( bState );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}
*/
CLSCRFL_API LONG __stdcall 
            CLSCRF_Led( IN LPVOID pReader,
                        IN BYTE   bBlinkColor,
                        IN BYTE   bBlinkCount,
                        IN BYTE   bPostColor )
{
	printf("CLSCRF_Led call..\n");
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
		printf("CLSCRF_Led, pReader - ok!\n");
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x07 );
        pTr->m_TrBuf.AddByte( bBlinkColor );
        pTr->m_TrBuf.AddByte( bBlinkCount );
        pTr->m_TrBuf.AddByte( bPostColor );

		printf("Ready to SingleTranceive()\n");
        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_UART_Baudrate( IN  LPVOID pReader,
                                  IN  DWORD  dwBaudrate )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
        switch( dwBaudrate )
        {
        case 9600:
        case 14400:
        case 19200:
        case 38400:
        case 57600:
        case 115200:
            break;

        default:
            Status = SCARD_E_INVALID_PARAMETER;
        }

    if( Status == SCARD_S_SUCCESS )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x0f );
        BYTE br = (BYTE)( F_OSC / ( 16 * dwBaudrate ) );
        pTr->m_TrBuf.AddByte( br );

        Status = pTr->SingleTransceive();
    }

    return Status;
}

//=====================================================================

CLSCRFL_API LONG __stdcall 
            CLSCRF_Activate_Idle_A( IN     LPVOID  pReader,
                                       OUT LPBYTE  pbATQ,
                                       OUT LPBYTE  pbSAK,
                                       OUT LPBYTE  pbUID,
                                    IN OUT LPDWORD pdwUIDLength )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x43 );

        DWORD Size;
        Status = pTr->SingleTransceive();


        if( Status == SCARD_S_SUCCESS )
        {
            Size = 2;
            if( ! pTr->m_RcBuf.GetBulk( 1, pbATQ, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }

        if( Status == SCARD_S_SUCCESS )
        {
            Size = 1;
            if( ! pTr->m_RcBuf.GetBulk( 3, pbSAK, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }

        if( Status == SCARD_S_SUCCESS )
        {
            *pdwUIDLength = 0; //kbhSCARD_E_INSUFFICIENT_BUFFER
            Size = 1;
            if( ! pTr->m_RcBuf.GetBulk( 4, (LPBYTE)pdwUIDLength, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }

        if( Status == SCARD_S_SUCCESS )
        {
            Size = *pdwUIDLength;
            if( ! pTr->m_RcBuf.GetBulk( 5, pbUID, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }
    }
    else
    {
        ZeroMemory( pbATQ, 2 );
        *pbSAK = 0;
        ZeroMemory( pbUID, 10 );
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Halt_A( IN LPVOID pReader )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x1d );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Activate_Wakeup_A( IN     LPVOID pReader,
                                      IN     LPBYTE pbUID,
                                      IN     DWORD  UIDLength,
                                         OUT LPBYTE pbATQ,
                                         OUT LPBYTE pbSAK )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x44 );
        pTr->m_TrBuf.AddByte( (BYTE) UIDLength );
        pTr->m_TrBuf.AddBulk( pbUID, UIDLength );

        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            Size = 2;
            if( ! pTr->m_RcBuf.GetBulk( 1, pbATQ, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }

        if( Status == SCARD_S_SUCCESS )
        {
            Size = 1;
            if( ! pTr->m_RcBuf.GetBulk( 3, pbSAK, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }
    }
    else
    {
        ZeroMemory( pbATQ, 2 );
        *pbSAK = 0;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

//=====================================================================

CLSCRFL_API LONG __stdcall 
            CLSCRF_Activate_Idle_B( IN     LPVOID  pReader,
                                    IN     BYTE    bAfi,
                                    IN     BYTE    bSni,
                                       OUT LPDWORD pdwPUPI,
                                       OUT LPDWORD pdwAppData,
                                       OUT LPDWORD pdwProtInfo,
                                    IN OUT LPDWORD pdwCardCount )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x56 );
        pTr->m_TrBuf.AddByte( bAfi );
        pTr->m_TrBuf.AddByte( bSni );

        DWORD Ptr = 1;
        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            *pdwCardCount = pTr->m_RcBuf.GetLength() / 11;
            for( DWORD i = 0; i < *pdwCardCount; i++ )
            {
                Size = 4;
                if( pTr->m_RcBuf.GetBulk( Ptr, (unsigned char*)&pdwPUPI[i], &Size ) )
                    Ptr += Size;
                else
                {
                    Status = SCARD_E_COMM_DATA_LOST;
                    break;
                }

                Size = 4;
                if( pTr->m_RcBuf.GetBulk( Ptr, (unsigned char*)&pdwAppData[i], &Size ) )
                    Ptr += Size;
                else
                {
                    Status = SCARD_E_COMM_DATA_LOST;
                    break;
                }

                Size = 3;
                pdwProtInfo[i] = 0;
                if( pTr->m_RcBuf.GetBulk( Ptr, (unsigned char*)&pdwProtInfo[i], &Size ) )
                    Ptr += Size;
                else
                {
                    Status = SCARD_E_COMM_DATA_LOST;
                    break;
                }
            }
        }
    }
    else
    {
        *pdwCardCount = 0;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Attrib_B( IN     LPVOID  pReader,
                             IN     LPBYTE  pbPUPI,
                             IN     BYTE    bParam1,
                             IN     BYTE    bParam2,
                             IN     BYTE    bParam3,
                             IN     BYTE    bParam4,
                             IN OUT LPBYTE  pbHigherLayerBuf,
                             IN     DWORD   dwHLBufSize,
                             IN OUT LPDWORD pdwHLInfLength,
                                OUT LPBYTE  pbMbliCid )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x54 );
        pTr->m_TrBuf.AddByte( 8 + (BYTE)*pdwHLInfLength );
        pTr->m_TrBuf.AddBulk( pbPUPI, 4 );
        pTr->m_TrBuf.AddByte( bParam1 );
        pTr->m_TrBuf.AddByte( bParam2 );
        pTr->m_TrBuf.AddByte( bParam3 );
        pTr->m_TrBuf.AddByte( bParam4 );
        if( *pdwHLInfLength )
            pTr->m_TrBuf.AddBulk( pbHigherLayerBuf, *pdwHLInfLength );

        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            DWORD Size;
            *pdwHLInfLength = pTr->m_RcBuf.GetLength();
            if( *pdwHLInfLength > 1 )
            {
                Size = 1;
                if( ! pTr->m_RcBuf.GetBulk( 1, pbMbliCid, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
                else if( (*pbMbliCid & 0x0f) != bParam4 )
                    Status = SCARD_F_INTERNAL_ERROR;

                *pdwHLInfLength -= 2;
            }
            else
            {
                *pdwHLInfLength = 0;
                Status = SCARD_E_COMM_DATA_LOST;
            }

            if( Status == SCARD_S_SUCCESS && *pdwHLInfLength )
            {
                Size = *pdwHLInfLength;
                if( ! pTr->m_RcBuf.GetBulk( 3, pbHigherLayerBuf, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
            }
        }
    }
    else
    {
        *pdwHLInfLength = 0;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Halt_B( IN LPVOID pReader,
                           IN LPBYTE pbPUPI )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x55 );
        pTr->m_TrBuf.AddBulk( pbPUPI, 4 );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Activate_Wakeup_B( IN     LPVOID  pReader,
                                      IN     BYTE    bAfi,
                                      IN     BYTE    bSni,
                                         OUT LPDWORD pdwPUPI,
                                         OUT LPDWORD pdwAppData,
                                         OUT LPDWORD pdwProtInfo,
                                      IN OUT LPDWORD pdwCardCount )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x57 );
        pTr->m_TrBuf.AddByte( bAfi );
        pTr->m_TrBuf.AddByte( bSni );

        DWORD Ptr = 1;
        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            *pdwCardCount = pTr->m_RcBuf.GetLength() / 11;
            for( DWORD i = 0; i < *pdwCardCount; i++ )
            {
                Size = 4;
                if( pTr->m_RcBuf.GetBulk( Ptr, (unsigned char*)&pdwPUPI[i], &Size ) )
                    Ptr += Size;
                else
                {
                    Status = SCARD_E_COMM_DATA_LOST;
                    break;
                }

                Size = 4;
                if( pTr->m_RcBuf.GetBulk( Ptr, (unsigned char*)&pdwAppData[i], &Size ) )
                    Ptr += Size;
                else
                {
                    Status = SCARD_E_COMM_DATA_LOST;
                    break;
                }

                Size = 3;
                pdwProtInfo[i] = 0;
                if( pTr->m_RcBuf.GetBulk( Ptr, (unsigned char*)&pdwProtInfo[i], &Size ) )
                    Ptr += Size;
                else
                {
                    Status = SCARD_E_COMM_DATA_LOST;
                    break;
                }
            }
        }
    }
    else
    {
        *pdwCardCount = 0;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

//=====================================================================

CLSCRFL_API LONG __stdcall 
            CLSCRF_Inventory_15693( IN     LPVOID  pReader,
                                    IN     BYTE    bFlags,
                                    IN     BYTE    bInventory,
                                    IN     BYTE    bAfi,
                                    IN     BYTE    bMaskLen,
                                    IN     LPCBYTE pbMaskVal, 
                                       OUT LPBYTE  pbRecvBuffer, 
                                    IN OUT LPDWORD pdwRecvLength )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x31 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bInventory );
        pTr->m_TrBuf.AddByte( bAfi );
        pTr->m_TrBuf.AddByte( bMaskLen );
        int SendLength = ( bMaskLen + 7 ) / 8;
        pTr->m_TrBuf.AddBulk( pbMaskVal, SendLength );

        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            DWORD RecvLength = 0;
            DWORD Size = pTr->m_RcBuf.GetLength();
            if( Size > *pdwRecvLength )
            {
                *pdwRecvLength = 0;
                return SCARD_E_INSUFFICIENT_BUFFER;
            }

            Size = 2;
            *pdwRecvLength = 0;
            if( ! pTr->m_RcBuf.GetBulk( 1, (LPBYTE)pdwRecvLength, &Size ) )
            {
                *pdwRecvLength = 0;
                if( Status == SCARD_S_SUCCESS )
                    return SCARD_E_COMM_DATA_LOST;
            }

            if( *pdwRecvLength )
            {
                if( ! pTr->m_RcBuf.GetBulk( 3, pbRecvBuffer, pdwRecvLength ) )
                    Status = SCARD_E_COMM_DATA_LOST;
            }
        }
        else
            *pdwRecvLength = 0;
    }
    else
    {
        *pdwRecvLength = 0;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Stay_Quiet_15693( IN LPVOID pReader,
                                     IN BYTE   bFlags,
                                     IN BYTE   bStayQuiet,
                                     IN LPBYTE pbUID )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x32 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bStayQuiet );
        pTr->m_TrBuf.AddBulk( pbUID, 8 );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_Select_15693( IN     LPVOID pReader,
                                 IN     BYTE   bFlags,
                                 IN     BYTE   bSelect,
                                 IN     LPBYTE pbUID,
                                    OUT LPBYTE pbFlags,
                                    OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 1200;         // 11,3 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bSelect != 0x25 )
         || ( ( bFlags & 0x20 ) == 0 )
         || ( pbUID == NULL ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = 10;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bSelect );
        pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_ResetToReady_15693( IN     LPVOID pReader,
                                       IN     BYTE   bFlags,
                                       IN     BYTE   bResetToReady,
                                       IN     LPBYTE pbUID,
                                          OUT LPBYTE pbFlags,
                                          OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 1200;         // 11,3 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bResetToReady != 0x26 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 10 : 2;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bResetToReady );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

//=====================================================================

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareStandard_HostCodeKey( IN     LPVOID pReader,
                                               IN     LPBYTE pbUncoded,
                                                  OUT LPBYTE pbCoded )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x16 );
        pTr->m_TrBuf.AddBulk( pbUncoded, 6 );

        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            Size = 12;
            if( ! pTr->m_RcBuf.GetBulk( 1, pbCoded, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }
    }
    else
    {
        ZeroMemory( pbCoded, 12 );
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

BYTE GetActualBlockAddress( DWORD Sector, DWORD Block )
{
    DWORD Addr = 4 * Sector + Block;
    if( Sector >= 32 )
        Addr += 12 * ( Sector - 32 );
    return (BYTE)Addr;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareStandard_AuthKey( IN LPVOID pReader,
                                           IN BYTE   bAuthMode,
                                           IN LPBYTE pbUID,
                                           IN DWORD  dwSector,
                                           IN LPBYTE pbCodedKey )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x18 );
        pTr->m_TrBuf.AddByte( bAuthMode );
        pTr->m_TrBuf.AddBulk( pbUID, 4 );
        pTr->m_TrBuf.AddBulk( pbCodedKey, 12 );
        pTr->m_TrBuf.AddByte( GetActualBlockAddress( dwSector, 0 ) );

        Status = pTr->SingleTransceive();

    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareStandard_WriteKeyToE2( IN LPVOID pReader,
                                                IN BYTE   bKeyType,
                                                IN DWORD  dwSector,
                                                IN LPBYTE pbUncoded )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x17 );
        pTr->m_TrBuf.AddByte( bKeyType );
        pTr->m_TrBuf.AddByte( (BYTE)dwSector & 0xff );
        pTr->m_TrBuf.AddBulk( pbUncoded, 6 );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareStandard_AuthE2( IN LPVOID pReader,
                                          IN BYTE   bAuthMode,
                                          IN LPBYTE pbUID,
                                          IN DWORD  dwSector )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x15 );
        pTr->m_TrBuf.AddByte( bAuthMode );
        pTr->m_TrBuf.AddBulk( pbUID, 4 );
        pTr->m_TrBuf.AddByte( (BYTE)dwSector & 0xff );
        pTr->m_TrBuf.AddByte( GetActualBlockAddress( dwSector, 0 ) );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareStandard_Read( IN     LPVOID pReader,
                                        IN     DWORD  dwSector,
                                        IN     DWORD  dwBlock,
                                           OUT LPBYTE pbRecvBuffer )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x19 );
        pTr->m_TrBuf.AddByte( GetActualBlockAddress( dwSector, dwBlock ) );

        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
            if( pTr->m_RcBuf.GetLength() != 17 )
                Status = SCARD_E_COMM_DATA_LOST;
            else
            {
                Size = 16;
                if( ! pTr->m_RcBuf.GetBulk( 1, pbRecvBuffer, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
            }
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }
    if( Status )
        ZeroMemory( pbRecvBuffer, 16 );

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareStandard_Write( IN LPVOID pReader,
                                         IN DWORD  dwSector,
                                         IN DWORD  dwBlock,
                                         IN LPBYTE pbSendBuffer )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x1A );
        pTr->m_TrBuf.AddByte( GetActualBlockAddress( dwSector, dwBlock ) );
        pTr->m_TrBuf.AddBulk( pbSendBuffer, 16 );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

LONG MifareStandardValue( IN LPVOID pReader,
                          IN BYTE   bOper,
                          IN DWORD  dwSector,
                          IN DWORD  dwSourceBlock,
                          IN DWORD  dwValue,
                          IN DWORD  dwTargetBlock )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x1B );
        pTr->m_TrBuf.AddByte( bOper );
        pTr->m_TrBuf.AddByte( GetActualBlockAddress( dwSector, dwSourceBlock ) );
        pTr->m_TrBuf.AddBulk( (unsigned char*)&dwValue, 4 );
        pTr->m_TrBuf.AddByte( GetActualBlockAddress( dwSector, dwTargetBlock ) );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

#define PICC_DECREMENT     0xC0

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareStandard_Decrement( IN LPVOID pReader,
                                             IN DWORD  dwSector,
                                             IN DWORD  dwSourceBlock,
                                             IN DWORD  dwValue,
                                             IN DWORD  dwTargetBlock )
{
    return MifareStandardValue( pReader, PICC_DECREMENT,
        dwSector, dwSourceBlock, dwValue, dwTargetBlock );
}

#define PICC_INCREMENT     0xC1

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareStandard_Increment( IN LPVOID pReader,
                                             IN DWORD  dwSector,
                                             IN DWORD  dwSourceBlock,
                                             IN DWORD  dwValue,
                                             IN DWORD  dwTargetBlock )
{
    return MifareStandardValue( pReader, PICC_INCREMENT,
        dwSector, dwSourceBlock, dwValue, dwTargetBlock );
}

#define PICC_RESTORE       0xC2

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareStandard_Restore( IN LPVOID pReader,
                                           IN DWORD  dwSector,
                                           IN DWORD  dwSourceBlock,
                                           IN DWORD  dwTargetBlock )
{
    return MifareStandardValue( pReader, PICC_RESTORE,
        dwSector, dwSourceBlock, 0, dwTargetBlock );
}

//=====================================================================

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareUltralight_Read( IN     LPVOID pReader,
                                          IN     BYTE   bPage,
                                             OUT LPBYTE pbRecvBuffer )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x19 );
        pTr->m_TrBuf.AddByte( bPage );

        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            Size = 4;
            if( ! pTr->m_RcBuf.GetBulk( 1, pbRecvBuffer, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }
    }
    else
    {
        ZeroMemory( pbRecvBuffer, 4 );
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_MifareUltralight_Write( IN LPVOID pReader,
                                           IN BYTE   bPage,
                                           IN LPBYTE pbSendBuffer )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x1e );
        pTr->m_TrBuf.AddByte( bPage );
        pTr->m_TrBuf.AddBulk( pbSendBuffer, 4 );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

//=====================================================================

CLSCRFL_API LONG __stdcall 
            CLSCRF_ReadSingleBlock_15693( IN     LPVOID pReader,
                                          IN     BYTE   bFlags,
                                          IN     BYTE   bReadSingleBlock,
                                          IN     LPBYTE pbUID,
                                          IN     BYTE   bBlockNumber,
                                             OUT LPBYTE pbFlags,
                                             OUT LPBYTE pbBlockSecurityStatus,
                                             OUT LPBYTE pbData,
                                             OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 1200;         // 11,3 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;
    BOOL fOption = bFlags & 0x40;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bReadSingleBlock != 0x20 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 11 : 3;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bReadSingleBlock );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddByte( bBlockNumber );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 5 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        *pbBlockSecurityStatus = 0;
        if( *pbFlags & 0x01 )
        {
            *pbErrorCode = pTr->m_RcBuf[4];
            ZeroMemory( pbData, ICODE_BLOCK_SIZE );
        }
        else
        {
            DWORD Expected = ( fOption ) ? 9 : 8;
            if( Size < Expected )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            if( Status == SCARD_S_SUCCESS )
            {
                DWORD Index = 4;
                if( fOption )
                    *pbBlockSecurityStatus = pTr->m_RcBuf[Index++];
                Size = ICODE_BLOCK_SIZE;
                if( ! pTr->m_RcBuf.GetBulk( Index, pbData, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
            }
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_WriteSingleBlock_15693( IN     LPVOID pReader,
                                           IN     BYTE   bFlags,
                                           IN     BYTE   bWriteSingleBlock,
                                           IN     LPBYTE pbUID,
                                           IN     BYTE   bBlockNumber,
                                           IN     LPBYTE pbData,
                                              OUT LPBYTE pbFlags,
                                              OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 2500;         // 23,6 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bWriteSingleBlock != 0x21 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 15 : 7;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bWriteSingleBlock );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddByte( bBlockNumber );
        pTr->m_TrBuf.AddBulk( pbData, ICODE_BLOCK_SIZE );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_LockBlock_15693( IN     LPVOID pReader,
                                    IN     BYTE   bFlags,
                                    IN     BYTE   bLockBlock,
                                    IN     LPBYTE pbUID,
                                    IN     BYTE   bBlockNumber,
                                       OUT LPBYTE pbFlags,
                                       OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 2500;         // 23,6 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bLockBlock != 0x22 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 11 : 3;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bLockBlock );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddByte( bBlockNumber );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_ReadMultipleBlocks_15693( IN     LPVOID pReader,
                                             IN     BYTE   bFlags,
                                             IN     BYTE   bReadMultipleBlock,
                                             IN     LPBYTE pbUID,
                                             IN     BYTE   bFirstBlockNumber,
                                             IN OUT LPBYTE pbNumberOfBlocks,
                                                OUT LPBYTE pbFlags,
                                                OUT LPBYTE pbBlockSecurityStatus,
                                                OUT LPBYTE pbData,
                                                OUT LPBYTE pbErrorCode )

{
    const DWORD Timeout = 1200;         // 11,3 ms
    LONG  Status = SCARD_S_SUCCESS;
    int   BlockCount = *pbNumberOfBlocks + 1;
    ZeroMemory( pbBlockSecurityStatus, BlockCount );
    ZeroMemory( pbData, BlockCount * ICODE_BLOCK_SIZE );
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;
    BOOL fOption = bFlags & 0x40;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bReadMultipleBlock != 0x23 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD PieceCount;
    DWORD SendLength = ( fAddress ) ? 12 : 4;
    DWORD BlockStatusSize = ( fOption ) ? 1 + ICODE_BLOCK_SIZE : ICODE_BLOCK_SIZE;
    int   Max_Blocks = ( ICODE_PACKET_SIZE - 4 ) / BlockStatusSize;
    int   PieceFirst = bFirstBlockNumber;
    DWORD IndexIn = 0;
    DWORD IndexBSS = 0;
    DWORD IndexDat = 0;
    *pbNumberOfBlocks = 0;

    do
    {
        if( Status == SCARD_S_SUCCESS )
        {
            PieceCount = ( BlockCount > Max_Blocks ) ? Max_Blocks : BlockCount;
            pTr->m_TrBuf.Clear();
            pTr->m_TrBuf.AddByte( 0x48 );
            pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
            pTr->m_TrBuf.AddByte( bFlags );
            pTr->m_TrBuf.AddByte( bReadMultipleBlock );
            if( fAddress )
                pTr->m_TrBuf.AddBulk( pbUID, 8 );
            pTr->m_TrBuf.AddByte( PieceFirst );
            pTr->m_TrBuf.AddByte( (BYTE)(PieceCount - 1) );
            pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

            Status = pTr->SingleTransceive();
        }

        DWORD Size;

        if( Status == SCARD_S_SUCCESS )
        {
            Size = pTr->m_RcBuf.GetLength();
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
        }

//        DWORD Expected;

        if( Status == SCARD_S_SUCCESS )
        {
            *pbFlags = pTr->m_RcBuf[3];
            if( *pbFlags & 0x01 )
            {
//                if( *pbNumberOfBlocks )
//                    *pbFlags = 0;
//                else
                    *pbErrorCode = pTr->m_RcBuf[4];
                break;
            }
            else
            {
//                Expected = 4 + PieceCount * BlockStatusSize;
//                if( Size < Expected )
//                    Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
                if( Status == SCARD_S_SUCCESS )
                {
                    IndexIn = 4;
                    do
                    {
                        if( fOption )
                            pbBlockSecurityStatus[IndexBSS++] = pTr->m_RcBuf[IndexIn++];
                        Size = ICODE_BLOCK_SIZE;
                        if( ! pTr->m_RcBuf.GetBulk( IndexIn, &pbData[IndexDat], &Size ) )
                        {
                            Status = SCARD_E_COMM_DATA_LOST;
                            break;
                        }
                        else
                        {
                            *pbNumberOfBlocks += 1;
                            IndexDat += ICODE_BLOCK_SIZE;
                            IndexIn  += ICODE_BLOCK_SIZE;
                        }
                    } while ( IndexIn < pTr->m_RcBuf.GetLength() );
                }
            }
        }

        if( Status == SCARD_S_SUCCESS )
        {
            PieceFirst += PieceCount;
            BlockCount -= PieceCount;
        }
        else
            break;

    } while ( BlockCount > 0 );

    if( Status == SCARD_S_SUCCESS )
        *pbNumberOfBlocks -= 1;

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_WriteAFI_15693( IN     LPVOID pReader,
                                   IN     BYTE   bFlags,
                                   IN     BYTE   bWriteAFI,
                                   IN     LPBYTE pbUID,
                                   IN     BYTE   bAFI,
                                      OUT LPBYTE pbFlags,
                                      OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 2500;         // 23,6 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bWriteAFI != 0x27 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 11 : 3;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bWriteAFI );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddByte( bAFI );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_LockAFI_15693( IN     LPVOID pReader,
                                  IN     BYTE   bFlags,
                                  IN     BYTE   bLockAFI,
                                  IN     LPBYTE pbUID,
                                     OUT LPBYTE pbFlags,
                                     OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 2500;         // 23,6 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bLockAFI != 0x28 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 10 : 2;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bLockAFI );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_WriteDSFID_15693( IN     LPVOID pReader,
                                     IN     BYTE   bFlags,
                                     IN     BYTE   bWriteDSFID,
                                     IN     LPBYTE pbUID,
                                     IN     BYTE   bDSFID,
                                        OUT LPBYTE pbFlags,
                                        OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 2500;         // 23,6 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bWriteDSFID != 0x29 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 11 : 3;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bWriteDSFID );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddByte( bDSFID );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_LockDSFID_15693( IN     LPVOID pReader,
                                    IN     BYTE   bFlags,
                                    IN     BYTE   bLockDSFID,
                                    IN     LPBYTE pbUID,
                                       OUT LPBYTE pbFlags,
                                       OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 2500;         // 23,6 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bLockDSFID != 0x2a )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 10 : 2;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bLockDSFID );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_GetSystemInfo_15693( IN     LPVOID pReader,
                                        IN     BYTE   bFlags,
                                        IN     BYTE   bGetSystemInfo,
                                        IN OUT LPBYTE pbUID,
                                           OUT LPBYTE pbFlags,
                                           OUT LPBYTE pbInfoFlags,
                                           OUT LPBYTE pbDSFID,
                                           OUT LPBYTE pbAFI,
                                           OUT LPWORD pbMemorySize,
                                           OUT LPBYTE pbICReference,
                                           OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 1200;         // 11,3 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbDSFID = 0;
    *pbAFI = 0;
    *pbMemorySize = 0;
    *pbICReference = 0;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bGetSystemInfo != 0x2b )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 10 : 2;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bGetSystemInfo );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size = 0;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 5 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    DWORD Index = 0;

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
            *pbErrorCode = pTr->m_RcBuf[4];
        else
        {
            Index = 4;
            if( Status == SCARD_S_SUCCESS )
            {
                Size = 1;
                if( ! pTr->m_RcBuf.GetBulk( Index++, pbInfoFlags, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
            }
            if( Status == SCARD_S_SUCCESS )
            {
                Size = 8;
                if( ! pTr->m_RcBuf.GetBulk( Index, pbUID, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
                Index += 8;
            }
            if( Status == SCARD_S_SUCCESS && ( *pbInfoFlags & 0x01 ) )
            {
                Size = 1;
                if( ! pTr->m_RcBuf.GetBulk( Index++, pbDSFID, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
            }
            if( Status == SCARD_S_SUCCESS && ( *pbInfoFlags & 0x02 ) )
            {
                Size = 1;
                if( ! pTr->m_RcBuf.GetBulk( Index++, pbAFI, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
            }
            if( Status == SCARD_S_SUCCESS && ( *pbInfoFlags & 0x04 ) )
            {
                Size = 2;
                if( ! pTr->m_RcBuf.GetBulk( Index, (LPBYTE)pbMemorySize, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
                Index += 2;
            }
            if( Status == SCARD_S_SUCCESS && ( *pbInfoFlags & 0x08 ) )
            {
                Size = 1;
                if( ! pTr->m_RcBuf.GetBulk( Index, pbICReference, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
            }
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_GetMultipleBSS_15693( IN     LPVOID pReader,
                                         IN     BYTE   bFlags,
                                         IN     BYTE   bGetMultipleBSS,
                                         IN     LPBYTE pbUID,
                                         IN     BYTE   bFirstBlockNumber,
                                         IN OUT LPBYTE pbNumberOfBlocks,
                                            OUT LPBYTE pbFlags,
                                            OUT LPBYTE pbBlockSecurityStatus,
                                            OUT LPBYTE pbErrorCode )

{
    const DWORD Timeout = 1200;         // 11,3 ms
    LONG  Status = SCARD_S_SUCCESS;
    int   BlockCount = *pbNumberOfBlocks + 1;
    ZeroMemory( pbBlockSecurityStatus, BlockCount );
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bGetMultipleBSS != 0x2c )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD PieceCount;
    DWORD SendLength = ( fAddress ) ? 12 : 4;
    int   Max_Blocks = ICODE_PACKET_SIZE;
    int   PieceFirst = bFirstBlockNumber;
    DWORD IndexIn = 0;
    DWORD IndexBSS = 0;
    *pbNumberOfBlocks = 0;

    do
    {
        if( Status == SCARD_S_SUCCESS )
        {
            PieceCount = ( BlockCount > Max_Blocks ) ? Max_Blocks : BlockCount;
            pTr->m_TrBuf.Clear();
            pTr->m_TrBuf.AddByte( 0x48 );
            pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
            pTr->m_TrBuf.AddByte( bFlags );
            pTr->m_TrBuf.AddByte( bGetMultipleBSS );
            if( fAddress )
                pTr->m_TrBuf.AddBulk( pbUID, 8 );
            pTr->m_TrBuf.AddByte( PieceFirst );
            pTr->m_TrBuf.AddByte( (BYTE)(PieceCount - 1) );
            pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

            Status = pTr->SingleTransceive();
        }

        DWORD Size;

        if( Status == SCARD_S_SUCCESS )
        {
            Size = pTr->m_RcBuf.GetLength();
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
        }

//        DWORD Expected;

        if( Status == SCARD_S_SUCCESS )
        {
            *pbFlags = pTr->m_RcBuf[3];
            if( *pbFlags & 0x01 )
            {
                    *pbErrorCode = pTr->m_RcBuf[4];
                break;
            }
            else
            {
//                Expected = 4 + PieceCount * BlockStatusSize;
//                if( Size < Expected )
//                    Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
                if( Status == SCARD_S_SUCCESS )
                {
                    IndexIn = 4;
                    do
                    {
                        pbBlockSecurityStatus[IndexBSS++] = pTr->m_RcBuf[IndexIn++];
                        *pbNumberOfBlocks += 1;
                    } while ( IndexIn < pTr->m_RcBuf.GetLength() );
                }
            }
        }

        if( Status == SCARD_S_SUCCESS )
        {
            PieceFirst += PieceCount;
            BlockCount -= PieceCount;
        }
        else
            break;

    } while ( BlockCount > 0 );

    if( Status == SCARD_S_SUCCESS )
        *pbNumberOfBlocks -= 1;

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_SetEAS_15693( IN     LPVOID pReader,
                                 IN     BYTE   bFlags,
                                 IN     BYTE   bSetEAS,
                                 IN     BYTE   bICMfgCode,
                                 IN     LPBYTE pbUID,
                                    OUT LPBYTE pbFlags,
                                    OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 2500;         // 23,6 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bSetEAS != 0xa2 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 11 : 3;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bSetEAS );
        pTr->m_TrBuf.AddByte( bICMfgCode );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_ResetEAS_15693( IN     LPVOID pReader,
                                   IN     BYTE   bFlags,
                                   IN     BYTE   bResetEAS,
                                   IN     BYTE   bICMfgCode,
                                   IN     LPBYTE pbUID,
                                      OUT LPBYTE pbFlags,
                                      OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 2500;         // 23,6 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bResetEAS != 0xa3 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 11 : 3;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bResetEAS );
        pTr->m_TrBuf.AddByte( bICMfgCode );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_LockEAS_15693( IN     LPVOID pReader,
                                  IN     BYTE   bFlags,
                                  IN     BYTE   bLockEAS,
                                  IN     BYTE   bICMfgCode,
                                  IN     LPBYTE pbUID,
                                     OUT LPBYTE pbFlags,
                                     OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 2500;         // 23,6 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bLockEAS != 0xa4 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 11 : 3;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bLockEAS );
        pTr->m_TrBuf.AddByte( bICMfgCode );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 4 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            if( Size < 5 )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            else
                *pbErrorCode = pTr->m_RcBuf[4];
        }
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_EASAlarm_15693( IN     LPVOID pReader,
                                   IN     BYTE   bFlags,
                                   IN     BYTE   bEASAlarm,
                                   IN     BYTE   bICMfgCode,
                                   IN     LPBYTE pbUID,
                                      OUT LPBYTE pbFlags,
                                      OUT LPBYTE pbEASData,
                                      OUT LPBYTE pbErrorCode )
{
    const DWORD Timeout = 1200;         // 11,3 ms
    LONG Status = SCARD_S_SUCCESS;
    *pbErrorCode = 0;
    *pbFlags = 0x01;
    BOOL fAddress = bFlags & 0x20;

    if( pReader == NULL )
        Status = SCARD_E_READER_UNAVAILABLE;

    if( Status == SCARD_S_SUCCESS )
    {
        if( ( bEASAlarm != 0xa5 )
         || ( fAddress && ( pbUID == NULL ) ) )
            Status = SCARD_E_INVALID_PARAMETER;
    }

    CTransfer* pTr = (CTransfer*)pReader;
    DWORD SendLength;

    if( Status == SCARD_S_SUCCESS )
    {
        SendLength = ( fAddress ) ? 11 : 3;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&SendLength, 2 );
        pTr->m_TrBuf.AddByte( bFlags );
        pTr->m_TrBuf.AddByte( bEASAlarm );
        pTr->m_TrBuf.AddByte( bICMfgCode );
        if( fAddress )
            pTr->m_TrBuf.AddBulk( pbUID, 8 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&Timeout, 4 );

        Status = pTr->SingleTransceive();
    }

    DWORD Size;

    if( Status == SCARD_S_SUCCESS )
    {
        Size = pTr->m_RcBuf.GetLength();
        if( Size < 5 )
            Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
    }

    if( Status == SCARD_S_SUCCESS )
    {
        *pbFlags = pTr->m_RcBuf[3];
        if( *pbFlags & 0x01 )
        {
            *pbErrorCode = pTr->m_RcBuf[4];
            ZeroMemory( pbEASData, ICODE_EAS_DATA_SIZE );
        }
        else
        {
            DWORD Expected = 4 + ICODE_EAS_DATA_SIZE;
            if( Size < Expected )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
            if( Status == SCARD_S_SUCCESS )
            {
                DWORD Index = 4;
                Size = ICODE_EAS_DATA_SIZE;
                if( ! pTr->m_RcBuf.GetBulk( Index, pbEASData, &Size ) )
                    Status = SCARD_E_COMM_DATA_LOST;
            }
        }
    }

    return Status;
}
//=====================================================================

CLSCRFL_API LONG __stdcall 
            CLSCRF_DirectIO_Reader( IN     LPVOID  pReader,
                                    IN     LPCBYTE pbSendBuffer, 
                                    IN     DWORD   dwSendLength, 
                                       OUT LPBYTE  pbRecvBuffer, 
                                    IN OUT LPDWORD pdwRecvLength )
{
    LONG Status = SCARD_S_SUCCESS;

    if( dwSendLength == 0 )
    {
        *pdwRecvLength = 0;
        Status = SCARD_E_INVALID_PARAMETER;
    }
    else if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddBulk( pbSendBuffer, dwSendLength );

        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            DWORD Size = pTr->m_RcBuf.GetLength();
            if( Size < *pdwRecvLength )
                *pdwRecvLength = Size;

            if( ! pTr->m_RcBuf.GetBulk( 0, pbRecvBuffer, pdwRecvLength ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }
        else
            *pdwRecvLength = 0;
    }
    else
    {
        *pdwRecvLength = 0;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_DirectIO_Card( IN     LPVOID  pReader,
                                  IN     LPCBYTE pbSendBuffer, 
                                  IN     DWORD   dwSendLength, 
                                  IN     DWORD   dwTimeout,
                                     OUT LPBYTE  pbRecvBuffer, 
                                  IN OUT LPDWORD pdwRecvLength )
{
    LONG Status = SCARD_S_SUCCESS;
    DWORD BufSize = *pdwRecvLength;

    if( dwSendLength == 0 )
    {
        *pdwRecvLength = 0;
        Status = SCARD_E_INVALID_PARAMETER;
    }
    else if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();

        pTr->m_TrBuf.AddByte( 0x48 );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&dwSendLength, 2 );
        pTr->m_TrBuf.AddBulk( pbSendBuffer, dwSendLength );
        pTr->m_TrBuf.AddBulk( (LPBYTE)&dwTimeout, 4 );

        Status = pTr->SingleTransceive();

        DWORD Size = pTr->m_RcBuf.GetLength();

        if( Size < 2 )  // Length is absent
        {
            *pdwRecvLength = 0;
            if( Status == SCARD_S_SUCCESS )
                Status = SCARD_E_COMM_DATA_LOST;    // 0x8010002F
        }
        else
        {
            Size = 2;
            *pdwRecvLength = 0;
            if( ! pTr->m_RcBuf.GetBulk( 1, (LPBYTE)pdwRecvLength, &Size ) )
            {
                *pdwRecvLength = 0;
                if( Status == SCARD_S_SUCCESS )
                    Status = SCARD_E_COMM_DATA_LOST;
            }

            if( *pdwRecvLength > BufSize )
            {
                *pdwRecvLength = BufSize;
                if( Status == SCARD_S_SUCCESS )
                    Status = SCARD_E_INSUFFICIENT_BUFFER;
            }

            if( Size = *pdwRecvLength )
                if( ! pTr->m_RcBuf.GetBulk( 3, pbRecvBuffer, pdwRecvLength ) )
                {
                    if( Status == SCARD_S_SUCCESS )
                        Status = SCARD_E_COMM_DATA_LOST;
                }
        }
    }
    else
    {
        *pdwRecvLength = 0;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

//=====================================================================
//#if ADDRESS_RS_485
CLSCRFL_API LONG __stdcall 
            CLSCRF_GetIOAddress( IN     LPVOID pReader,
                                    OUT LPBYTE pbIOAddr )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        *pbIOAddr = pTr->GetIOAddress();
    }
    else
    {
        *pbIOAddr = 0;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_SetIOAddress( IN LPVOID pReader,
                                 IN BYTE   bIOAddr )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->SetIOAddress( bIOAddr );
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_ReadDeviceAddress( IN     LPVOID pReader,
                                         OUT LPBYTE pbDevAddr )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x7a );

        DWORD Size;
        Status = pTr->SingleTransceive();

        if( Status == SCARD_S_SUCCESS )
        {
            Size = 1;
            if( ! pTr->m_RcBuf.GetBulk( 1, pbDevAddr, &Size ) )
                Status = SCARD_E_COMM_DATA_LOST;
        }
    }
    else
    {
        *pbDevAddr = 0;
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}

CLSCRFL_API LONG __stdcall 
            CLSCRF_WriteDeviceAddress( IN LPVOID pReader,
                                       IN BYTE   bDevAddr )
{
    LONG Status = SCARD_S_SUCCESS;

    if( pReader )
    {
        CTransfer* pTr = (CTransfer*)pReader;
        pTr->m_TrBuf.Clear();
        pTr->m_TrBuf.AddByte( 0x77 );
        pTr->m_TrBuf.AddByte( bDevAddr );

        Status = pTr->SingleTransceive();
    }
    else
    {
        Status = SCARD_E_READER_UNAVAILABLE;
    }

    return Status;
}
//#endif
//=====================================================================

// Transfer.h: interface for the CTransfer class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _TRANSFER_H_
#define _TRANSFER_H_

#define TX_BUF_SIZE     300
#define RX_BUF_SIZE     300

#define GLOBAL_IO_TIMEOUT       2000

#include "types_def.h"
#include "SCardErr.h"
#include <termios.h>
#include <unistd.h>

/* Количество секунд, после которых read прерывается */
#define TIMEOUT_SEC 3

/*
#ifndef _CDECODEBUFSIZE
#define _CDECODEBUFSIZE
//                                  0    1    2    3    4    5    6    7
static DWORD cDecodeBufSize [] = {  16,  24,  32,  40,  48,  64,  96, 128, 
//                                  8    9   10   11   12   13   14   15
                                   256, 256, 256, 256, 256, 256, 256, 256 };

const unsigned char cPCD_FSDI = 5;
const DWORD cPCD_BufSize = cDecodeBufSize[cPCD_FSDI];     // PCD FIFO Size = 64
#endif 
*/
#define DEFAULT_IO_TIMEOUT       1000
#define MINIMUM_IO_TIMEOUT         50
#define MAXIMUM_IO_TIMEOUT   86400000

#define F_OSC   24000000

#define B_SPECIAL               0xFD // 
#define B_START                 0xFD // 
#define B_STOP                  0xFE // 
#define B_STUFF                 0xFF // 

#define ICODE_PACKET_SIZE     32
#define ICODE_BLOCK_SIZE       4
#define ICODE_EAS_DATA_SIZE   32

typedef union _STATE_TRANS
{
    unsigned char n;
    struct flags{
        unsigned RX_start : 1;  // Получен стартовый байт
        unsigned RX_stuff : 1;  // Получен байт-прокладка
        unsigned RX_addr  : 1;  // Получен полный кадр
        unsigned _X_busy  : 1;  // //Новую команду принимать нельзя
        unsigned _X_OK    : 1;  // //Ответ получен
        unsigned _X_Err   : 1;  // //Обнаружена ошибка
        unsigned _TX_last : 1;
        unsigned _reset   : 1;
    } b;
} STATE_TRANS;


class CTransceiveBuffer 
{
public:
	CTransceiveBuffer ();
	virtual ~CTransceiveBuffer ();

    DWORD GetLength() { return m_Count; }
    void Clear() { m_Count = 0; }
    bool AddByte( unsigned char Value );
    bool AddBulk( const unsigned char *Data, DWORD Size );
    bool GetBulk( int nIndex, unsigned char *Data, DWORD *pSize );
    BYTE operator []( int nIndex ) const;

protected:
    DWORD           m_Count;
    unsigned char   m_Buf[TX_BUF_SIZE];
};

typedef void  (*LPFN_TRANS_ERROR)( int Port, UINT Oper, DWORD Err, void *Context  );
typedef void  (*LPFN_TRANS_RX)( int Port, BYTE *Data, int Size, void *Context );

class FakeClass
{
public:
	void SetCallback(int a, int b, void *v)
	{
	}
};

class CTransfer
{
public:
	CTransfer();
	virtual ~CTransfer();

//    void Transmit( unsigned char *Data, int Size );
    bool Open( DWORD LogLevel, int Port, int Baud );
	bool IsOpened() { return (fd > 0); }
    bool Close( void );

    DWORD GetTimeout() { return m_Timeout; }
    void SetTimeout( DWORD NewTimeout );
//#if ADDRESS_RS_485
    BYTE GetIOAddress() { return m_IO_Addr; }
    void SetIOAddress( BYTE Addr ) { m_IO_Addr = Addr; }
//#endif
    HANDLE  m_EvIOComplete;
    DWORD m_Error;
    LONG    m_LongError;
    void OnError( int Port, UINT Oper, DWORD Err );
    void OnRX( int Port, BYTE *RXBuff, int Size );

    int m_PortNumber;
    int m_Baudrate;
    unsigned char m_SendBuffer[ RX_BUF_SIZE ];

    CTransceiveBuffer m_TrBuf;
    CTransceiveBuffer m_RcBuf;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    LONG SingleTransceive();
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	FakeClass m_ComPort;
protected:

    unsigned char m_RX_buf[ RX_BUF_SIZE ];
    unsigned char m_TX_buf[ TX_BUF_SIZE ];
    DWORD m_RX_cnt;
    DWORD m_TX_cnt;
    DWORD m_Timeout;

//#if ADDRESS_RS_485
    unsigned char m_IO_Addr;
//#endif
    unsigned char m_Cmd_ID;
    STATE_TRANS   m_state;

    unsigned short m_RX_FCS;
    unsigned short m_TX_FCS;

    void SetEvent(int ev);
    int event;
    
    void RX_Reset( void );
    void CalculateFCS( unsigned short &FCS, unsigned char b );
    void PostByte( unsigned char b );
    void ByteStuff( unsigned char b );
    char OnReceive( unsigned char inp );
	/* Дескриптор устройства */
	int fd;
	struct termios old_port_attr;
	void _setup_timeout();
};


//static void CommError( int Port, UINT Oper, DWORD Err, LPVOID Context ) \
//    { ((CTransfer*)Context)->OnError( Port, Oper, Err ); }

//static void CommRX( int Port, BYTE *RXBuff, int Size, LPVOID Context ) \
//    { ((CTransfer*)Context)->OnRX( Port, RXBuff, Size ); }

//set SetEvent(int ev);

#endif // !defined(AFX_TRANSFER_H__14085D87_B9A2_4C05_B2F3_B4456CC197A9__INCLUDED_)

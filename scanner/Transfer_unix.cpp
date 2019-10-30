// Transfer.cpp: implementation of the CTransfer class.
//
//////////////////////////////////////////////////////////////////////

#include "Transfer_unix.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>

//#include "MfErrNo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EV_COMPLETE 5

/* Для реализации тайм аутов */
void sig_alarm(int sig)
{
	/* Nop */
//	printf("SIGALARM\n");
}

//////////////////////////////////////////////////////////////////////
//                      class CTransceiveBuffer 
//////////////////////////////////////////////////////////////////////

CTransceiveBuffer::CTransceiveBuffer()
{
    m_Count = 0;
}

CTransceiveBuffer::~CTransceiveBuffer()
{
}

bool CTransceiveBuffer::AddByte( unsigned char Value )
{
    if( m_Count + 1 > TX_BUF_SIZE )
        return false;

    m_Buf[ m_Count ] = Value;
    m_Count++;

    return true;
}

bool CTransceiveBuffer::AddBulk( const unsigned char *Data, DWORD Size )
{
    bool bRet = false;

    if( m_Count + Size > TX_BUF_SIZE )
        Size = TX_BUF_SIZE - m_Count;
    else
        bRet = true;

    memcpy( &m_Buf[ m_Count ], Data, Size );
    m_Count += Size;

    return bRet;
}

bool CTransceiveBuffer::GetBulk( int nIndex, unsigned char *Data, DWORD *pSize )
{
    bool bRet = false;
    DWORD CpySize = *pSize;

    if( nIndex + *pSize > m_Count )
        CpySize = TX_BUF_SIZE - nIndex;

    if( CpySize && CpySize == *pSize )
        bRet = true;

    if( CpySize )
        memcpy( Data, &m_Buf[ nIndex ], CpySize );

    *pSize = CpySize;
    return bRet;
}

BYTE CTransceiveBuffer::operator []( int nIndex ) const
{
    if( nIndex > TX_BUF_SIZE )
        return 0;
    else
        return m_Buf[ nIndex ];
}

//static CTransfer* lpTr;

// 10:10:50.646 R     00 B6 AB F4 B9 49 4E 63 2A 10 B4 8E
//                 FD 00 B6 AB F4 B9 49 4E 63 2A 10 B4 8E 00 FE 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction     class CTransfer
//////////////////////////////////////////////////////////////////////
CTransfer::CTransfer()
{
//#if ADDRESS_RS_485
    m_IO_Addr = 0;
//#endif
    m_Cmd_ID = 0;
    m_Timeout = DEFAULT_IO_TIMEOUT;
    m_PortNumber = -1;
    m_Baudrate = 0;
	fd = -1;
}

CTransfer::~CTransfer()
{
    Close();
}

void CTransfer::_setup_timeout()
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sig_alarm;
	act.sa_flags = SA_ONESHOT;
	if (sigaction(SIGALRM, &act, 0) != 0)
	{
		printf("Could not set up timeout\n");
	}
	else
	{
		/* seting up timeout */
		//alarm(TIMEOUT_SEC);
		struct itimerval old, new_;
		new_.it_interval.tv_usec = 0;
		new_.it_interval.tv_sec = 0;
		new_.it_value.tv_usec = 0;
		new_.it_value.tv_sec = (long int) TIMEOUT_SEC;
		if (setitimer (ITIMER_REAL, &new_, &old) < 0)
		{
			printf("Could not set up timeout\n");
		}
	}
}

/* Baud задавать в целочисленном значении скорости */
bool CTransfer::Open( DWORD LogLevel, int Port, int Baud )
{
    RX_Reset();
    //UART_RX_ENABLE();
    bool bResult = false;
    m_PortNumber = Port;    // from 0
    m_Baudrate = Baud;      // 0 -> USB, else -> COM-port

    if( m_Baudrate )
    {
		/* rs232 */
		char str[20];
		int c = 0;

#ifndef DEBUG
		sprintf(str, "/dev/ttyS%d", Port );
#else
		sprintf(str, "/dev/skel%d", Port );
#endif
		printf("%s\n", str);
		fd = open(str, O_RDWR | O_NOCTTY/* | O_NONBLOCK */);
		if (fd > 0)
		{
			struct termios new_attr;
		
			tcgetattr(fd, &old_port_attr);
			
			bzero(&new_attr, sizeof(new_attr));
			cfmakeraw(&new_attr);
			
			speed_t speed = B9600;
			switch (Baud)
			{
				case 115200:
					speed = B115200;
					break;
					
				case 57600:
					speed = B57600;
					break;

				case 38400:
					speed = B38400;
					break;

				case 19200:
					speed = B19200;
					break;

				case 4800:
					speed = B4800;
					break;

				case 9600:
				default:
					speed = B9600;
					
			}
			cfsetspeed(&new_attr, speed);
			
			bResult = true;	
			
			new_attr.c_cflag |= (CREAD | CLOCAL | CS8);
		
			int res = tcsetattr(fd, TCSANOW, &new_attr);
			if (res < 0)
			{
				close(fd);
				bResult = false;
			}
			
			tcflush(fd, TCIFLUSH);

			ioctl(fd, TIOCMGET, &c);
			if (c & TIOCM_DTR)
				c &= ~TIOCM_DTR;
			if (c & TIOCM_RTS)
				c &= ~TIOCM_RTS;
			ioctl(fd, TIOCMSET, &c);

			usleep(100); /* wait for finish setup process */
		}
		else
		{
			fd = -1;
		}
    }
    else
    {
		char str[20];

		sprintf(str, "/dev/microm%d", Port );

		/* USB */
		printf("Try connect to USB device %s'\n", str);
		fd = open(str, O_RDWR);
		if (fd > 0)
		{
			printf("Device opened.\n");
			bResult = true;
		}
		else
		{
			printf("Device NOT opened.\n");
			fd = -1;
		}
    }

    return bResult;
}

bool CTransfer::Close( void )
{
//	tcsetattr(fd, TCSANOW, &old_port_attr);
	close(fd);
	fd = -1;	
    return true;
}

void CTransfer::RX_Reset( void )
{
    m_state.n = 0;  // в т.ч. m_state.b.RX_stuff = 0;
    m_RX_cnt = 0;
    m_RX_FCS = 0xFFFF;
}

void CTransfer::CalculateFCS( unsigned short &FCS, unsigned char b )
{
    unsigned char i;
    unsigned short w;

    w = ( b ^ FCS ) & 0xFF;
    i = 8;
    do{
        if( w & 1 ) 
        {
            w >>= 1;
            w ^= 0x8408;
        } 
        else 
        {
            w >>= 1;
        }
    } while( --i );
    FCS = w ^ ( FCS >> 8 );
}

void CTransfer::PostByte( unsigned char b )
{
    CalculateFCS( m_TX_FCS, b );
    ByteStuff( b );
}

void CTransfer::ByteStuff( unsigned char b )
{
    if( b < B_SPECIAL )
        m_TX_buf[m_TX_cnt++] = b;
    else                            // ByteStuffing
    {      
        m_TX_buf[m_TX_cnt++] = B_STUFF;
        m_TX_buf[m_TX_cnt++] = B_STUFF - b;
    }
}

char CTransfer::OnReceive( unsigned char inp )
{
    unsigned char b;
/*
	if( m_Timer )
	{
		KillTimer( NULL, m_Timer );
		m_Timer = 0;
	}
*/
    switch( inp )
    {
    case B_START:  // Frame begin
        RX_Reset();
        m_state.b.RX_start = 1;
        break;

    case B_STOP:  // Frame end
/*
Для формирования ответа считывателем должны быть приняты как минимум 
стартовый и стоповый байты, идентификатор кадра, код команды и FCS. 
Также должны отсутствовать ошибки байтстаффинга в пределах кадра. 
Иначе никакие ответы не формируются и мастер должен повторить запрос 
по окончании таймаута, выставив соответствующий признак повторного запроса.
*/

        if( m_state.b.RX_start 
            && ( m_RX_cnt >= 4 ))//идентификатор кадра(1), код команды(1) и FCS(2) 
        {
            m_RX_FCS ^= 0xFFFF;
            if( m_RX_FCS == 0x0F47 )
            {
                return 0;
            }
            else
            {
/*
Ответ NACK 1 посылается в случае ошибки FCS. Единственная цель 
такого ответа - ускорить повтор запроса до окончания таймаута.
*/
//                Acknowledgment( NACK1_FCS_ERROR );
                return 2;
            }
        }
        else
        {
            RX_Reset();
            return 3;
        }
        break;

    case B_STUFF:  // Byte stuffing
        if( m_state.b.RX_start )
            m_state.b.RX_stuff = 1;
        break;

    default:    // Usual bytes
        if( ! m_state.b.RX_start )
		{
            return 4;
		}

        if( m_state.b.RX_stuff )
        {
            if( inp > 2 )       // Ошибка байтстаффинга
            {
                RX_Reset();
                return 5;
            }
            m_state.b.RX_stuff = 0;
            b = B_STUFF - inp;
        }
        else
        {
            b = inp;
        }

//#if ADDRESS_RS_485
        if( m_state.b.RX_addr )
        {
        m_RX_buf[m_RX_cnt++] = b;
        }
        else
        {
            if( m_IO_Addr && m_IO_Addr != b )
            {
                RX_Reset();
                return 7;           // оНКСВЕМ НРБЕР НР ОНЯРНПНММЕЦН СЯРПНИЯРБЮ
            }
            m_state.b.RX_addr = 1;
        }
//#else
//            m_RX_buf[m_RX_cnt++] = b;
//#endif
/*
Считыватель вправе игнорировать запрос если количество байт между стартовым 
и стоповым условиями больше чем оговорено самой большой командой.
*/
        if( m_RX_cnt < RX_BUF_SIZE )
        {
            CalculateFCS( m_RX_FCS, b );
        }
        else
        {
            RX_Reset();
            return 6;
        }

    }
    return 1;
}

void CTransfer::OnRX( int Port, BYTE *RXBuff, int Size )
{
    char Error;
    for( int i = 0; i < Size; i++ )
    {
        Error = OnReceive( RXBuff[i] );
        if( Error > 1 )
        {
//            if( m_lpfnError ) 
//                (*m_lpfnError)( Port, SERN_EVENT, Error, m_NotifContext );
            OnError( Port, SERN_EVENT, Error );
            return;
        }
    }
    if( ! Error )
    {
//        if( m_lpfnRX ) 
//            (*m_lpfnRX)( Port, m_RX_buf, m_RX_cnt - 2, m_NotifContext );
        //SetEvent( m_EvIOComplete );
        SetEvent( EV_COMPLETE  );
//        m_state.b.RX_OK = 1;
    }
}

void CTransfer::OnError( int Port, UINT Oper, DWORD Err )
{
//    if( m_lpfnError )
//        (*m_lpfnError)( Port, Oper, Err, m_NotifContext );
//        str.Format( "Порт COM%d   OK", Port+1 );
//	CString str, so, se;
    if( !( (Oper == SERN_EVENT) && (Err == 1) ) )
    {
/*
        switch( Oper )
        {
        case SERN_EVENT:
            so = "EVENT";
            break;

        case SERN_READ:
            so = "READ";
            break;

        case SERN_WRITE:
            so = "WRITE";
            break;

        default:
            so = "NO";
        }

        switch( Err )
        {
        case CE_RXOVER:
            se = "CE_RXOVER";
            break;

        case CE_OVERRUN:
            se = "CE_OVERRUN";
            break;

        case CE_RXPARITY:
            se = "CE_RXPARITY";
            break;

        case CE_FRAME:
            se = "CE_FRAME";
            break;

        case CE_TXFULL:
            se = "CE_TXFULL";
            break;

        case CE_MODE:
            se = "CE_MODE";
            break;

        case ERROR_TIMEOUT:
            se = "ERROR_TIMEOUT";
            break;

        default:
            se.Format( "0x%08X", Err );
        }
//        str.Format( "Порт COM%d   %s   Ошибка    %s", Port+1, so, se );
*/
        m_Error = Err;
//        m_state.b.RX_Err = 1;
    }
//    CCardPage* pPage = (CCardPage*)GetActivePage();
//    pPage->m_StatusBar.SetWindowText( str );
//    m_pCurPage->m_StatusBar.SetWindowText( str );
}
/*
//void CTransfer::SendCommand( unsigned char Oper, unsigned char Size, unsigned char *Param )
void CTransfer::Transmit( unsigned char *Data, int Size )
{
//    m_Cmd_op = Data[0];
    m_TX_cnt = 0;
    m_TX_FCS = 0xffff;
    m_TX_buf[m_TX_cnt++] = B_START;  // Frame begin
    PostByte( m_Cmd_ID++ );
    for( int i = 0; i < Size; i++ )
        PostByte( Data[i] );
    m_TX_FCS ^= 0xffff;
    ByteStuff( m_TX_FCS & 0xFF );
    ByteStuff( ( m_TX_FCS >> 8 ) & 0xFF );
    m_TX_buf[m_TX_cnt++] = B_STOP;  // Frame end
//    m_ComPort.Write( m_TX_buf, m_TX_cnt );
	write(fd, m_TX_buf, m_TX_cnt);

}
*/
//////////////////////////////////////////////////////////////////////
LONG CTransfer::SingleTransceive()
{
    m_Error = 0;
    LONG Status = 0;

    if( ! IsOpened() )
    {
        Status = SCARD_E_NO_READERS_AVAILABLE;
        return Status;
    }

    if( m_TrBuf.GetLength() == 0 )
    {
        Status = SCARD_E_NOT_TRANSACTED;
//        memcpy( pbRecvBuffer, &Status, *pcbRecvLength );
        return Status;
    }

    m_RcBuf.Clear();

/*    if( m_Baudrate )
    {
        m_TX_cnt = 0;
        m_TX_FCS = 0xffff;
        m_TX_buf[m_TX_cnt++] = B_START;  // Frame begin
//#if ADDRESS_RS_485
        PostByte( m_IO_Addr );
//#endif
        PostByte( m_Cmd_ID++ );
        DWORD Count = m_TrBuf.GetLength();
        for( DWORD i = 0; i < m_TrBuf.GetLength(); i++ )
            PostByte( m_TrBuf[i] );
        m_TX_FCS ^= 0xffff;
        ByteStuff( m_TX_FCS & 0xFF );
        ByteStuff( ( m_TX_FCS >> 8 ) & 0xFF );
        m_TX_buf[m_TX_cnt++] = B_STOP;  // Frame end

        //m_ComPort.Write( m_TX_buf, m_TX_cnt );
		write(fd, m_TX_buf, m_TX_cnt);

        DWORD Wait_Result = WaitForSingleObject( m_EvIOComplete, m_Timeout );
        switch( Wait_Result )
        {
        case WAIT_ABANDONED:
            m_Error = ERROR_INVALID_HANDLE_STATE;   // == 0x00000649
            break;

        case WAIT_OBJECT_0:
            ResetEvent( m_EvIOComplete );
            break;

        case WAIT_TIMEOUT:
            m_Error = ERROR_TIMEOUT;                // == 0x000005B4
            break;

        }

        if( m_Error )               // ( COM-port & Protocol ) Errors (4 bytes)
        {
            Status = m_Error;
        } 
        else
        {
            if( m_RX_buf[2] )       // ( Reader & Card ) Errors (1 byte)
            {
                Status = SCARD_F_INTERNAL_ERROR;
                m_RcBuf.AddByte( m_RX_buf[2] );
            }
            else
            {
                m_RcBuf.AddBulk( &m_RX_buf[2], m_RX_cnt - 4 );
            }
        }
    }
    else */
    {
        m_TX_cnt = (BYTE)m_TrBuf.GetLength();
        DWORD Count = m_TX_cnt;

		/* Записываем  данные в устройство */
	
		m_TX_cnt = 0;
        m_TX_FCS = 0xffff;
        m_TX_buf[m_TX_cnt++] = B_START;  // Frame begin
        PostByte( m_IO_Addr );
        PostByte( m_Cmd_ID++ );
        Count = m_TrBuf.GetLength();
        for( DWORD i = 0; i < m_TrBuf.GetLength(); i++ )
            PostByte( m_TrBuf[i] );
        m_TX_FCS ^= 0xffff;
        ByteStuff( m_TX_FCS & 0xFF );
        ByteStuff( ( m_TX_FCS >> 8 ) & 0xFF );
        m_TX_buf[m_TX_cnt++] = B_STOP;  // Frame end

		int ret = write(fd, m_TX_buf, m_TX_cnt);
		
		event = 0;
		m_Error = 0;
		m_RX_cnt = 0;
		BYTE buf[RX_BUF_SIZE];
		/* считываем */
		while (true)
		{
			_setup_timeout();
			int ret = read(fd, buf, RX_BUF_SIZE);
			if (ret > 0)
			{
				OnRX(m_PortNumber, buf, ret);
			} else if (ret < 0)
			{
				/* Сработал таймер  или произошла ошибка */
				/*if (errno == EINTR) */
				m_Error = ERROR_TIMEOUT;
				break;
			}
			
			if (m_Error)
			{
				/* Таймаут или ошибка из OnRX */
				break;
			}

			if (event == EV_COMPLETE)
			{
				/* Прочли до конца */
				break;
			}
		}
		
        if( m_Error )               // ( COM-port & Protocol ) Errors (4 bytes)
        {
            Status = m_Error;
        } 
        else
        {
            if( m_RX_buf[2] )       // ( Reader & Card ) Errors (1 byte)
            {
                Status = SCARD_F_INTERNAL_ERROR;
                m_RcBuf.AddByte( m_RX_buf[2] );
            }
            else
            {
                m_RcBuf.AddBulk( &m_RX_buf[2], m_RX_cnt - 4 );
            }
        }
    }
   

    return Status;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void CTransfer::SetTimeout( DWORD NewTimeout )
{
    if( NewTimeout )
        m_Timeout = NewTimeout; 
    else
        m_Timeout = DEFAULT_IO_TIMEOUT; 
}

void CTransfer::SetEvent(int ev)
{
	event = ev;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

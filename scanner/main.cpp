#include <stdio.h>
#include "Clscrfl.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <fstream>

#define USE_UINPUT

#if defined( USE_XDO2 ) | defined( USE_XDO3 )
extern "C" {
#include <xdo.h>
}
#endif

#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/time.h>
#include <sys/types.h>

#if defined( USE_UINPUT )

static int uinp_fd = -1;
struct uinput_user_dev uinp;
struct input_event event;

int setup_uinput_device() {

    int i=0;

    uinp_fd = open( "/dev/uinput", O_WRONLY | O_NDELAY );

    if ( !uinp_fd ) {
        printf( "Unable to open /dev/uinput\n" );
        return -1;
    }

    memset( &uinp,0,sizeof( uinp ) );
    strncpy( uinp.name, "PolyVision Touch Screen", UINPUT_MAX_NAME_SIZE );
    uinp.id.version = 4;
    uinp.id.bustype = BUS_USB;

    ioctl( uinp_fd, UI_SET_EVBIT, EV_KEY );
    ioctl( uinp_fd, UI_SET_EVBIT, EV_REL );
    ioctl( uinp_fd, UI_SET_RELBIT, REL_X );
    ioctl( uinp_fd, UI_SET_RELBIT, REL_Y );
    for ( i=0; i < 256; i++ ) {
        ioctl( uinp_fd, UI_SET_KEYBIT, i );
    }
    ioctl( uinp_fd, UI_SET_KEYBIT, BTN_MOUSE );
    ioctl( uinp_fd, UI_SET_KEYBIT, BTN_TOUCH );
    ioctl( uinp_fd, UI_SET_KEYBIT, BTN_MOUSE );
    ioctl( uinp_fd, UI_SET_KEYBIT, BTN_LEFT );
    ioctl( uinp_fd, UI_SET_KEYBIT, BTN_MIDDLE );
    ioctl( uinp_fd, UI_SET_KEYBIT, BTN_RIGHT );
    ioctl( uinp_fd, UI_SET_KEYBIT, BTN_FORWARD );
    ioctl( uinp_fd, UI_SET_KEYBIT, BTN_BACK );

    write( uinp_fd, &uinp, sizeof( uinp ) );
    if ( ioctl( uinp_fd, UI_DEV_CREATE ) ) {
        printf( "Unable to create UINPUT device." );
        return -1;
    }
    return 1;
}

void press_button( char c ) {

    int key_code = 0;
    switch( c )
    {
    case '\n':
        key_code = KEY_ENTER;
        break;
    case '0':
        key_code = KEY_0;
        break;
    case '1':
        key_code = KEY_1;
        break;
    case '2':
        key_code = KEY_2;
        break;
    case '3':
        key_code = KEY_3;
        break;
    case '4':
        key_code = KEY_4;
        break;
    case '5':
        key_code = KEY_5;
        break;
    case '6':
        key_code = KEY_6;
        break;
    case '7':
        key_code = KEY_7;
        break;
    case '8':
        key_code = KEY_8;
        break;
    case '9':
        key_code = KEY_9;
        break;
    case 'A':
        key_code = KEY_A;
        break;
    case 'B':
        key_code = KEY_B;
        break;
    case 'C':
        key_code = KEY_C;
        break;
    case 'D':
        key_code = KEY_D;
        break;
    case 'E':
        key_code = KEY_E;
        break;
    case 'F':
        key_code = KEY_F;
        break;
    case 'Z':
        key_code = KEY_CAPSLOCK;
        break;
    }

    // Report BUTTON CLICK - PRESS event
    memset( &event, 0, sizeof( event ) );
    gettimeofday( &event.time, NULL );

    event.type = EV_KEY;
    event.code = key_code;
    event.value = 1;
    write( uinp_fd, &event, sizeof( event ) );

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write( uinp_fd, &event, sizeof( event ) );

    // Report BUTTON CLICK - RELEASE event
    memset( &event, 0, sizeof( event ) );
    gettimeofday( &event.time, NULL );

    event.type = EV_KEY;
    event.code = key_code;
    event.value = 0;
    write( uinp_fd, &event, sizeof( event ) );

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write( uinp_fd, &event, sizeof( event ) );
}
#endif

std::string exec( const char* cmd ) {

    char buffer[128];
    std::string result = "";
    FILE* pipe = popen( cmd, "r" );

    if ( !pipe ) throw std::runtime_error( "popen() failed!" );
    try {
        while ( !feof( pipe ) ) {
            if ( fgets( buffer, 128, pipe ) != NULL )
                result += buffer;
        }
    } catch ( ... ) {
        pclose( pipe );
        throw;
    }
    pclose( pipe );
    return result;
}

bool isTitleFound( std::string title ) {

    std::string line;
    std::ifstream infile( "/usr/share/microem/dont_send_eol.txt" );
    while ( infile >> line ) {
        if ( title.find( line ) != std::string::npos ) {
            return true;
        }
    }
    return false;
}

void signal_callback_handler( int signum ) {
  exit( signum );
}

int main( int argc, char* argv[] ) {

	signal( SIGINT, signal_callback_handler );

	//system( "sudo insmod /lib/modules/`uname -r`/microem.ko 2>/dev/null 1>/dev/null" );

#ifdef USE_UINPUT
    if ( setup_uinput_device() < 0 ) {
        printf( "Unable to find uinput device\n" );
        return -1;
    }
#elif defined( USE_XDO2 )
    xdo_t *xd = xdo_new( ":0.0" );
#elif defined( USE_XDO3 )
    xdo_t *xd = xdo_new( ":0.0" );
#endif

    void* m_pReader;
    long int ret;
    int PortIndex = 1;
    unsigned char Buf[300];
    int i, n;
    unsigned char Size;
    char Err;

    ret = CLSCRF_Create( &m_pReader );

    if ( ret == 0 ) {

        for ( int i = 0; i < 10; i++ ) {
            PortIndex = i;
            ret = CLSCRF_OpenUSB( m_pReader, PortIndex, 0 );
            if ( ret == 0 ) {
                break;
            }
        }

        if ( ret == 0 ) {

            printf( "Connected to device.\n" );
            //CLSCRF_Sound( &m_pReader, 1 );

            ret = CLSCRF_Mfrc_On( m_pReader );
            if ( ret == 0 ) {

                printf( "Transponder - on.\n" );
                ret = CLSCRF_Get_Mfrc_Version( m_pReader, Buf );
                ret = CLSCRF_Get_Mfrc_Serial_Number( m_pReader, Buf );
                //printf( "Resetting magnet field..\n" );
                //ret = CLSCRF_Mfrc_Rf_Off_On( &m_pReader, 10 );
                usleep( 1000000 );
                printf( "Setting RF Mode to ISO-15693..\n" );
                ret = CLSCRF_Mfrc_Set_Rf_Mode( m_pReader, 0x40 );

                if ( ret == 0 ) {

                    printf( "RF Mode set successfully.\n" );
                    //CLSCRF_Sound( &m_pReader, 3 );
                    //usleep( 50000 );
                    Sleep( 100 );
                    // Здесь можно получать идентификатор карты!

                    printf( "Waiting for Cards..\n" );
                    while ( true ) {

                        n = 16;
                        ret = CLSCRF_Inventory_15693( m_pReader,
                                                      0x26,//Flags,
                                                      0x01,//Inventory,
                                                      0,//AFI,
                                                      0,//MaskLen,
                                                      NULL,//*MaskVal,
                                                      Buf, // LPBYTE  pbRecvBuffer,
                                                      &n ); // LPDWORD pdwRecvLength );
                        if ( ret == 0 && n >= 12 ) {

                            CLSCRF_Sound( m_pReader, 1 );
                            Size = Buf[1];

                            if ( Size >= 10 ) {

                                char uid[17];

                                sprintf( uid, "%02X%02X%02X%02X%02X%02X%02X%02X\n",
                                  Buf[11], Buf[10], Buf[9], Buf[8], Buf[7], Buf[6], Buf[5], Buf[4] );

                                std::cout << uid << std::endl;

                                // xdotool под root разве сможет подключиться к окнам не от root?
                                std::string title = exec( "xdotool getactivewindow getwindowname" );

                                if ( isTitleFound( title ) ) {
                                  uid[16] = 0;
                                }

#ifdef USE_UINPUT
                                for ( int cn = 0; cn < 17; cn++ ) {
                                    press_button( uid[cn] );
                                }
#elif defined( USE_XDO2 )
                                xdo_type( xd, CURRENTWINDOW, uid, 20000 );

#elif defined( USE_XDO3 )
                                xdo_enter_text_window( xd, CURRENTWINDOW, uid, 20000 );
#endif

                                CLSCRF_Stay_Quiet_15693( m_pReader, 0x22, 0x02, &Buf[4] );
                            }
                        }
                        /* else
                        {
                        	if ( ret == 0x8010002E )
                        	{
                        		printf( "Cards not found.\n" );
                        	}
                        	else
                        	{
                        		printf( "Sorry, error 0x%08X\n", ret );
                        		if ( ret == 0x80100001 )
                        		{
                        			CLSCRF_GetLastInternalError( m_pReader, ( BYTE* )&Err );
                        			printf( "Internal Error = %d\n", Err );
                        		}
                        	}
                        }*/

                        usleep( 250000 );
                    }
                }
                else {
                    printf( "RF Mode set error ( %lu ).\n", ret );
                }
            } else {
                printf( "Transponder error ( %lu ).\n", ret );
            }

            ret = CLSCRF_Mfrc_Off( &m_pReader );
            CLSCRF_Close( m_pReader );
            ret = CLSCRF_Destroy( &m_pReader );
#ifdef USE_UINPUT
            ioctl( uinp_fd, UI_DEV_DESTROY );
            close( uinp_fd );
#endif
            if ( ret == 0 ) {
                printf( "Device connection destroyed.\n" );
            } else {
                printf( "Disconnect error ( %lu ).\n", ret );
            }
        } else {
            if ( ret == SCARD_E_READER_UNAVAILABLE ) {

                printf( "Device unavailable.\n" );
            }
            printf( "Device not found.\n" );
        }
    } else {
        printf( "Can not create instance. %lu \n", ret );
    }
}

#include <windows.h>
#include <stdio.h>

volatile bool   cont   = true;
HWND            parent = NULL;
HWND            target = NULL;

#define AUTOCLOSE_NONE  0
#define AUTOCLOSE_DONE  1

BOOL CALLBACK
_autoClose( HWND h, LPARAM param )
{
    char    buf[BUFSIZ + 2];

    buf[0] = '\0';
    GetWindowText( h, buf, BUFSIZ );

    if ( !strncmp( buf, "いいえ", 7 ) )
        target = h;
    else if ( !strncmp( buf, "ランタイム エラーが発生しました。", 34 ) ) {
        if ( parent && target ) {
            SendMessage( target, BM_CLICK, 0, 0 );
            *(long *)param = AUTOCLOSE_DONE;
        }
    }

    return ( TRUE );
}

BOOL
autoClose()
{
#define MAX_TITLE_LEN   1024
    char    title[MAX_TITLE_LEN + 2];
    HWND    h = GetTopWindow( NULL );
    BOOL    ret = FALSE;

    while ( h ) {
        parent   = NULL;
        title[0] = '\0';
        GetWindowText( h, title, MAX_TITLE_LEN );

        if ( title[0] && !strncmp( title, "エラー", 7 ) ) {
            HWND hh = FindWindowEx( h, NULL, "Button", NULL );
            if ( hh ) {
                long    result = AUTOCLOSE_NONE;

                parent = h;
                target = NULL;
                EnumChildWindows( h, _autoClose, (LPARAM)&result );

                if ( result == AUTOCLOSE_DONE )
                    ret = TRUE;
            }
        }

        h = GetNextWindow( h, GW_HWNDNEXT );
    }

    return ( ret );
}


BOOL
CtrlHandler( DWORD ctrlType )
{
    switch ( ctrlType ) {
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        cont = false;
        return ( TRUE );

    default:
        break;
    }

    return ( FALSE );
}

int APIENTRY
WinMain( HINSTANCE hInstance,
         HINSTANCE hPrevInstance,
         LPSTR     lpCmdLine,
         int       nCmdShow )
{
#define AUTOCLOSE_EVENT_PROCEED \
            "__autoClose_fd3a5a74-dc5d-4f2a-84a4-9670b9019a21__"
    HANDLE  event = CreateEvent(NULL, FALSE, FALSE, AUTOCLOSE_EVENT_PROCEED);
    if ( !event || (GetLastError() == ERROR_ALREADY_EXISTS) )
        return ( 255 );

    cont = true;
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE)CtrlHandler, TRUE );
    while ( cont )
        Sleep( autoClose() ? 300 : 5000 );

    return ( 0 );
}

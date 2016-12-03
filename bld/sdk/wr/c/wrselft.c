/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <wwindows.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrmsg.h"
#include "wrmaini.h"
#include "wrdmsgi.h"
#include "selft.h"
#include "jdlg.h"
#include "winexprt.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    const char          *file_name;
    bool                is32bit;
    bool                use_wres;
    WRFileType          file_type;
    FARPROC             hcb;
} WRSFT;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT extern BOOL CALLBACK WRSelectFileTypeProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/*****************************************************************************/
static void WRSetWinInfo( HWND, WRSFT * );
static bool WRGetWinInfo( HWND, WRSFT * );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WRFileType WRFTARRAY[3][2][2] = {
    { { WR_WIN16W_RES, WR_WIN16M_RES }, { WR_WINNTW_RES, WR_WINNTM_RES } },
    { { WR_WIN16_EXE,  WR_WIN16_EXE  }, { WR_WINNT_EXE,  WR_WINNT_EXE  } },
    { { WR_WIN16_DLL,  WR_WIN16_DLL  }, { WR_WINNT_DLL,  WR_WINNT_DLL  } }
};

static WRFileType educatedGuess( const char *name, bool is32bit, bool use_wres )
{
    char        ext[_MAX_EXT];
    WRFileType  guess;

    if( name == NULL ) {
        return( WR_DONT_KNOW );
    }

    guess = WR_DONT_KNOW;

    _splitpath( name, NULL, NULL, NULL, ext );

    if( stricmp( ext, ".exe" ) == 0 ) {
        if( is32bit ) {
            guess = WR_WINNT_EXE;
        } else {
            guess = WR_WIN16_EXE;
        }
    } else if( stricmp( ext, ".dll" ) == 0 ) {
        if( is32bit ) {
            guess = WR_WINNT_DLL;
        } else {
            guess = WR_WIN16_DLL;
        }
    } else if( stricmp( ext, ".res" ) == 0 ) {
        if( is32bit ) {
            guess = WR_WINNTW_RES;
        } else {
            if( use_wres ) {
                guess = WR_WIN16W_RES;
            } else {
                guess = WR_WIN16M_RES;
            }
        }
    }

    return( guess );
}

WRFileType WRAPI WRSelectFileType( HWND parent, const char *name, bool is32bit,
                                       bool use_wres, FARPROC hcb )
{
    DLGPROC     proc;
    HINSTANCE   inst;
    INT_PTR     modified;
    WRSFT       sft;
    WRFileType  guess;

    guess = WRGuessFileType( name );
    if( guess != WR_DONT_KNOW ) {
        return( guess );
    }

    guess = educatedGuess( name, is32bit, use_wres );
    if( guess != WR_DONT_KNOW ) {
        return( guess );
    }

    sft.hcb = hcb;
    sft.file_name = name;
    sft.file_type = WR_DONT_KNOW;
    sft.is32bit = is32bit;
    sft.use_wres  = use_wres;
    inst = WRGetInstance();

    proc = (DLGPROC)MakeProcInstance( (FARPROC)WRSelectFileTypeProc, inst );

    modified = JDialogBoxParam( inst, "WRSelectFileType", parent, proc, (LPARAM)&sft );

    FreeProcInstance( (FARPROC)proc );

    if( modified == -1 ) {
        return( WR_DONT_KNOW );
    }

    return( sft.file_type );
}

WRFileType WRAPI WRGuessFileType( const char *name )
{
    char        ext[_MAX_EXT];
    WRFileType  guess;

    if( name == NULL ) {
        return( WR_DONT_KNOW );
    }

    guess = WR_DONT_KNOW;

    _splitpath( name, NULL, NULL, NULL, ext );

    if( stricmp( ext, ".bmp" ) == 0 ) {
        guess = WR_WIN_BITMAP;
    } else if( stricmp( ext, ".cur" ) == 0 ) {
        guess = WR_WIN_CURSOR;
    } else if( stricmp( ext, ".ico" ) == 0 ) {
        guess = WR_WIN_ICON;
    } else if( stricmp( ext, ".rc" ) == 0 ) {
        guess = WR_WIN_RC;
    } else if( stricmp( ext, ".dlg" ) == 0 ) {
        guess = WR_WIN_RC_DLG;
    } else if( stricmp( ext, ".str" ) == 0 ) {
        guess = WR_WIN_RC_STR;
    } else if( stricmp( ext, ".mnu" ) == 0 ) {
        guess = WR_WIN_RC_MENU;
    } else if( stricmp( ext, ".acc" ) == 0 ) {
        guess = WR_WIN_RC_ACCEL;
    }

    return( guess );
}

void WRSetWinInfo( HWND hDlg, WRSFT *sft )
{
    char        ext[_MAX_EXT];
    bool        no_exe;

    if( sft == NULL ) {
        return;
    }

    no_exe = FALSE;

    if( sft->file_name != NULL ) {
        SendDlgItemMessage( hDlg, IDM_FILENAME, WM_SETTEXT, 0,
                            (LPARAM)(LPSTR)sft->file_name );
        _splitpath( sft->file_name, NULL, NULL, NULL, ext );
        if( stricmp( ext, ".res" ) == 0 ) {
            CheckDlgButton( hDlg, IDM_FTRES, BST_CHECKED );
            no_exe = TRUE;
        } else if( stricmp( ext, ".exe" ) == 0 ) {
            CheckDlgButton( hDlg, IDM_FTEXE, BST_CHECKED );
        } else if( stricmp( ext, ".dll" ) == 0 ) {
            CheckDlgButton( hDlg, IDM_FTDLL, BST_CHECKED );
        }
    }

#ifdef __NT__
    if( sft->is32bit ) {
        CheckDlgButton( hDlg, IDM_TSWINNT, BST_CHECKED );
    } else {
        CheckDlgButton( hDlg, IDM_TSWIN, BST_CHECKED );
    }
#else
    EnableWindow( GetDlgItem( hDlg, IDM_TSWINNT ), FALSE );
    CheckDlgButton( hDlg, IDM_TSWIN, BST_CHECKED );
#endif

    if( no_exe ) {
        EnableWindow( GetDlgItem( hDlg, IDM_FTEXE ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDM_FTDLL ), FALSE );
        if( sft->is32bit ) {
            EnableWindow( GetDlgItem( hDlg, IDM_RFMS ), FALSE );
        }
        if( sft->use_wres || sft->is32bit ) {
            CheckDlgButton( hDlg, IDM_RFWAT, BST_CHECKED );
        } else {
            CheckDlgButton( hDlg, IDM_RFMS, BST_CHECKED );
        }
    } else {
        EnableWindow( GetDlgItem( hDlg, IDM_RFWAT ), FALSE );
        EnableWindow( GetDlgItem( hDlg, IDM_RFMS ), FALSE );
    }
}

bool WRGetWinInfo( HWND hDlg, WRSFT *sft )
{
    int ft, ts, rf;

    if( sft == NULL ) {
        return( true );
    }

    if( IsDlgButtonChecked( hDlg, IDM_FTRES ) ) {
        ft = 0;
    } else if( IsDlgButtonChecked( hDlg, IDM_FTEXE ) ) {
        ft = 1;
    } else if( IsDlgButtonChecked( hDlg, IDM_FTDLL ) ) {
        ft = 2;
    } else {
        ft = -1;
    }

    if( IsDlgButtonChecked( hDlg, IDM_TSWIN ) ) {
        ts = 0;
    } else if( IsDlgButtonChecked( hDlg, IDM_TSWINNT ) ) {
        ts = 1;
    } else {
        ts = -1;
    }

    if( ft != 0 ) {
        rf = 0;
    } else {
        if( IsDlgButtonChecked( hDlg, IDM_RFWAT ) ) {
            rf = 0;
        } else if( IsDlgButtonChecked( hDlg, IDM_RFMS ) ) {
            rf = 1;
        } else {
            rf = -1;
        }
    }

    if( ft >= 0 && ts >= 0 && rf >= 0 ) {
        sft->file_type = WRFTARRAY[ft][ts][rf];
    } else {
        sft->file_type = WR_DONT_KNOW;
        return( false );
    }
    return( true );
}

WINEXPORT BOOL CALLBACK WRSelectFileTypeProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WRSFT       *sft;
    BOOL        ret;

    ret = FALSE;

    switch( message ) {
    case WM_DESTROY:
        WRUnregisterDialog( hDlg );
        break;

    case WM_INITDIALOG:
        sft = (WRSFT *)lParam;
        SET_DLGDATA( hDlg, sft );
        WRRegisterDialog( hDlg );
        WRSetWinInfo( hDlg, sft );
        ret = TRUE;
        break;

    case WM_SYSCOLORCHANGE:
        WRCtl3dColorChange();
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDM_SFTHELP:
            sft = (WRSFT *)GET_DLGDATA( hDlg );
            if( sft != NULL && sft->hcb != NULL ) {
                (*(void (*)(void))sft->hcb)();
            }
            break;

        case IDOK:
            sft = (WRSFT *)GET_DLGDATA( hDlg );
            if( sft == NULL ) {
                EndDialog( hDlg, FALSE );
                ret = TRUE;
            } else if( WRGetWinInfo( hDlg, sft ) ) {
                EndDialog( hDlg, TRUE );
                ret = TRUE;
            } else {
                WRDisplayErrorMsg( WR_INVALIDSELECTION );
            }
            break;

        case IDCANCEL:
            EndDialog( hDlg, FALSE );
            ret = TRUE;
            break;

        case IDM_TSWINNT:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( !IsDlgButtonChecked( hDlg, IDM_FTRES ) ) {
                break;
            }
            if( IsDlgButtonChecked( hDlg, LOWORD( wParam ) ) ) {
                EnableWindow( GetDlgItem( hDlg, IDM_RFMS ), FALSE );
                CheckDlgButton( hDlg, IDM_RFMS, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDM_RFWAT, BST_CHECKED );
            } else {
                EnableWindow( GetDlgItem( hDlg, IDM_RFMS ), TRUE );
            }
            break;

        case IDM_TSWIN:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( !IsDlgButtonChecked( hDlg, IDM_FTRES ) ) {
                break;
            }
            if( IsDlgButtonChecked( hDlg, LOWORD( wParam ) ) ) {
                EnableWindow( GetDlgItem( hDlg, IDM_RFMS ), TRUE );
            }
            break;

        case IDM_FTEXE:
        case IDM_FTDLL:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( IsDlgButtonChecked( hDlg, LOWORD( wParam ) ) ) {
                EnableWindow( GetDlgItem( hDlg, IDM_RFWAT ), FALSE );
                EnableWindow( GetDlgItem( hDlg, IDM_RFMS ), FALSE );
                CheckDlgButton( hDlg, IDM_RFWAT, BST_UNCHECKED );
                CheckDlgButton( hDlg, IDM_RFMS, BST_UNCHECKED );
            }
            break;

        case IDM_FTRES:
            if( GET_WM_COMMAND_CMD( wParam, lParam ) != BN_CLICKED ) {
                break;
            }
            if( IsDlgButtonChecked( hDlg, LOWORD( wParam ) ) ) {
                EnableWindow( GetDlgItem( hDlg, IDM_RFWAT ), TRUE );
                if( IsDlgButtonChecked( hDlg, IDM_TSWINNT ) ) {
                    EnableWindow( GetDlgItem( hDlg, IDM_RFMS ), FALSE );
                    CheckDlgButton( hDlg, IDM_RFMS, BST_UNCHECKED );
                    CheckDlgButton( hDlg, IDM_RFWAT, BST_CHECKED );
                } else {
                    EnableWindow( GetDlgItem( hDlg, IDM_RFMS ), TRUE );
                    CheckDlgButton( hDlg, IDM_RFMS, BST_UNCHECKED );
                    CheckDlgButton( hDlg, IDM_RFWAT, BST_CHECKED );
                }
            }
            break;
        }
        break;
    }

    return( ret );
}

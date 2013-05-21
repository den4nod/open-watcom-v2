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


#include "cgstd.h"
#include "coderep.h"
#include "score.h"
#include "model.h"
#include "pattern.h"
#include "procdef.h"
#include "vergen.h"
#include "opcodes.h"

extern  name            *AllocRegName(hw_reg_set);

extern  proc_def        *CurrProc;

extern  bool    MultiIns( instruction *ins ) {
/********************************************/
    ins = ins;
    return( FALSE );
}


extern  void    ScInitRegs( score *sc ) {
/***************************************/
    sc = sc;
}


extern  void    AddRegs() {
/*************************/

}


extern  void    ScoreSegments( score *sc ) {
/******************************************/
    sc = sc;
}


extern  bool    ScAddOk( hw_reg_set reg1, hw_reg_set reg2 ) {
/***********************************************************/
    reg1 = reg1;
    reg2 = reg2;
    return( TRUE );
}


extern  bool    ScConvert( instruction *ins ) {
/*********************************************/
    ins = ins;
    return( FALSE );
}


extern  bool    CanReplace( instruction *ins ) {
/**********************************************/
    ins = ins;
    return( TRUE );
}

extern  bool    ScRealRegister( name *reg ) {
/********************************************
    Return "TRUE" if "reg" is a real machine register and not some
    monstrosity like R1:R0:R2 used for calls.
*/

    return( reg->n.name_class != XX );
}

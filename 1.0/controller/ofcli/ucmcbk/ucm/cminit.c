
/*
 * $Source: /cvsroot/fslrepo/sdn-of/controller/ofcli/ucmcbk/ucm/cminit.c,v $
 * $Revision: 1.1.2.2 $
 * $Date: 2013/09/26 11:44:45 $
 */ 

/**************************************************************************/
/* Copyright (c) 2010, 2011 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/
/****************************************************************************
 *  File                : 
 *  Description         : This file contains source code implmenting 
 *                        configuration middleware data model tree.
 *  Revision History    :
 *    version    Date          Author           Change Description
****************************************************************************/
#ifdef OF_CM_SUPPORT
#ifdef OF_CM_CNTRL_SUPPORT

#include "cbcmninc.h" 
#include "cmcomm.h"
#include "dpcbk.h"

int32_t UCMCBKInit (void);
extern void UCMDPInit(void);
extern int32_t CM_RegisterDMNodes (void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t UCMCBKInit ()
{
  CM_CBK_DEBUG_PRINT ("Entered");

  CM_RegisterDMNodes ();
  
	UCMDPInit();

	UCMCreateCommChannelwithController();
  
	return OF_SUCCESS;
}



#endif /*OF_CM_SUPPORT */
#endif /*OF_CM_CNTRL_SUPPORT */

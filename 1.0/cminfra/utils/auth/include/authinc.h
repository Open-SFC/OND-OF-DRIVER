/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name: authinc.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: This file includes all the other header file 
 *              those are needed to build config middleware data model.
 * 
 */

#ifndef CM_AUTH_INCLD_H
#define CM_AUTH_INCLD_H

#include "cmincld.h"
#include "cmdll.h"
#include "cmdefs.h"
#include "cmutil.h"
#include "cmfile.h"
#include "dmgdef.h"
#include "cmtransgdf.h"
#include "jegdef.h"

#include "authgdef.h"
#include "authgif.h"

#ifdef CM_PAM_SUPPORT

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <pam_private.h>
#include <security/pam_modules.h>
#include <unistd.h>
#include <pwd.h>
#include <crypt.h>
#include <shadow.h>

#endif /* CM_PAM_SUPPORT */


#endif /* CM_AUTH_INCLD_H */

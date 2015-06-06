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
 * File name: jemem.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifndef UCMJE_MEM_H
#define UCMJE_MEM_H

/* JE MEM Pool */
void cm_je_create_cfgsession_mempool (void);
void *cm_je_cfgsession_alloc (void);
void cm_je_cfgsession_free (struct je_config_session * pFree);

void cm_je_create_cfgcache_mempool (void);
void *cm_je_cfgcache_alloc (void);
void cm_je_cfgcache_free (struct je_config_session_cache * pFree);

void cm_je_create_cfgrequest_mempool (void);
void *cm_je_cfgrequest_alloc (void);
void cm_je_cfgrequest_free (struct je_config_request * pFree);

/* JE Transport Channel MEM Pool */
void cm_je_create_tnsprtchannel_mempool (void);
void *cm_je_tnsprtchannel_alloc (void);
void cm_je_tnsprtchannel_free (struct cmje_tnsprtchannel_info * pFree);

#endif

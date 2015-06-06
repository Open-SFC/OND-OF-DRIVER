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
 * File name: cbkldef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/25/2013
 * Description: 
 * 
 */

#define FILL_CM_IV_PAIR(IvPair,Id,ValType,Value)\
{\
  uint32_t value_length=0;\
  value_length = of_strlen(Value);\
  IvPair.id_ui = Id ;\
  IvPair.value_type = ValType ;\
  IvPair.value_length = value_length;\
  IvPair.value_p =   (char *) of_calloc (1, value_length + 1);\
  if (IvPair.value_p == NULL)\
  {\
    return OF_FAILURE;\
  }\
  of_strncpy (IvPair.value_p,Value, value_length);\
 }

#define FILL_CM_IV_PAIR_RET(IvPair,Id,ValType,Value,Ret)\
{\
  uint32_t value_length=0;\
  value_length = of_strlen(Value);\
  IvPair.id_ui = Id ;\
  IvPair.value_type = ValType ;\
  IvPair.value_length = value_length;\
  IvPair.value_p =   (char *) of_calloc (1, value_length + 1);\
  if (IvPair.value_p == NULL)\
  {\
    Ret;\
  }\
  of_strncpy (IvPair.value_p,Value, value_length);\
 }

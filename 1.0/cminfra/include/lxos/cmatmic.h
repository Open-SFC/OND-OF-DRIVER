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
 * File name: ucmatmic.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains smp wrappers.
 * 
 */

#ifndef _UCMATMIC_H_
#define _UCMATMIC_H_

// #define 

#ifndef __KERNEL__
typedef unsigned int cm_atomic_t;

/* subtract iVal from Var and returns true if the var is 0 else false*/
static __inline__ unsigned char atomic_sub_and_test(int32_t iVal, cm_atomic_t *Var)
{
  *Var -= iVal;
  return (*Var ? FALSE:TRUE);
}

/* Decrement Var by one and returns true if the var is 0 else false*/
static __inline__ char atomic_dec_and_test(cm_atomic_t *Var)
{
  (*Var)--;
  return (*Var ? FALSE:TRUE);
}

/* increment Var by one and returns true if the var is 0 else false*/
static __inline__ char atomic_inc_and_test(cm_atomic_t *Var)
{
  (*Var)++;
  return (*Var ? FALSE:TRUE);
}


#define CM_ATOMIC_SET(Var,Value)        (Var = Value)
#define CM_ATOMIC_INC(Var)              (Var++)
#define CM_ATOMIC_DEC(Var)              (Var--)
#define CM_ATOMIC_READ(Var)             (Var)
#define CM_ATOMIC_SUB_AND_TEST(Val,Var) (atomic_sub_and_test(Val,&Var))
#define CM_ATOMIC_ADD(Val,Var)          (Var += Val)
#define CM_ATOMIC_SUB(Val,Var)          (Var -= Val)
#define CM_ATOMIC_DEC_AND_TEST(Var)     (atomic_dec_and_test(&Var))
#define CM_ATOMIC_INC_AND_TEST(Var)     (atomic_inc_and_test(&Var))


typedef struct
{
}cm_smp_atomic_t;

#define CM_SMP_ATOMIC_SET(Var,Value)        
#define CM_SMP_ATOMIC_INC(Var)              
#define CM_SMP_ATOMIC_DEC(Var)              
#define CM_SMP_ATOMIC_READ(Var)             (Var)
#define CM_SMP_ATOMIC_SUB_AND_TEST(Val,Var) FALSE
#define CM_SMP_ATOMIC_ADD(Val,Var)          
#define CM_SMP_ATOMIC_SUB(Val,Var)          
#define CM_SMP_ATOMIC_DEC_AND_TEST(Var)     FALSE
#define CM_SMP_ATOMIC_INC_AND_TEST(Var)     FALSE

#define CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(Var) 

#else

#ifndef CM_LINUX_KERNEL_2_6_21
#define cm_atomic_t atomic_t
#else
#ifdef CM_LINUX_2_6_27
#define cm_atomic_t volatile atomic_t
#else
#define cm_atomic_t volatile atomic_t
#endif
#endif
#define CM_ATOMIC_SET(Var,Value)        atomic_set(&Var,Value)
#define CM_ATOMIC_INC(Var)              atomic_inc((atomic_t*)&Var)
#define CM_ATOMIC_READ(Var)             atomic_read(&Var)
#define CM_ATOMIC_DEC(Var)              atomic_dec((atomic_t*)&Var)
#define CM_ATOMIC_SUB_AND_TEST(Val,Var) atomic_sub_and_test(Val, (atomic_t*)&Var)
#define CM_ATOMIC_ADD(Val,Var)          atomic_add(Val, (atomic_t*)&Var)
#define CM_ATOMIC_SUB(Val,Var)          atomic_sub(Val,(atomic_t*)&Var)
#define CM_ATOMIC_DEC_AND_TEST(Var)     atomic_dec_and_test((atomic_t*)&Var)
#define CM_ATOMIC_INC_AND_TEST(Var)     atomic_inc_and_test((atomic_t*)&Var)

#ifdef CM_CONFIG_SMP
#define cm_smp_atomic_t cm_atomic_t

#define CM_SMP_ATOMIC_SET(Var,Value)        CM_ATOMIC_SET(Var,Value)
#define CM_SMP_ATOMIC_INC(Var)              CM_ATOMIC_INC(Var)
#define CM_SMP_ATOMIC_DEC(Var)              CM_ATOMIC_DEC(Var)
#define CM_SMP_ATOMIC_READ(Var)             CM_ATOMIC_READ(Var)
#define CM_SMP_ATOMIC_SUB_AND_TEST(Val,Var) CM_ATOMIC_SUB_AND_TEST(Val, Var)
#define CM_SMP_ATOMIC_ADD(Val,Var)          CM_ATOMIC_ADD(Val, Var)
#define CM_SMP_ATOMIC_SUB(Val,Var)          CM_ATOMIC_SUB(Val,Var)
#define CM_SMP_ATOMIC_DEC_AND_TEST(Var)     CM_ATOMIC_DEC_AND_TEST(Var)
#define CM_SMP_ATOMIC_INC_AND_TEST(Var)     CM_ATOMIC_INC_AND_TEST(Var)
#define CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(Var)
#define CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO_TMP(Var) while(CM_ATOMIC_READ(Var) != 0)

#else /*CM_CONFIG_SMP*/

typedef struct
{
}cm_smp_atomic_t;

#define CM_SMP_ATOMIC_SET(Var,Value)        
#define CM_SMP_ATOMIC_INC(Var)              
#define CM_SMP_ATOMIC_DEC(Var)              
#define CM_SMP_ATOMIC_READ(Var)             (Var)
#define CM_SMP_ATOMIC_SUB_AND_TEST(Val,Var) FALSE
#define CM_SMP_ATOMIC_ADD(Val,Var)          
#define CM_SMP_ATOMIC_SUB(Val,Var)          
#define CM_SMP_ATOMIC_DEC_AND_TEST(Var)     FALSE
#define CM_SMP_ATOMIC_INC_AND_TEST(Var)     FALSE
#define CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO(Var) 
#define CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO_TMP(Var) 
#endif /*CM_CONFIG_SMP*/

#endif /* __KERNEL__ */

#endif /*_UCMATMIC_H_*/


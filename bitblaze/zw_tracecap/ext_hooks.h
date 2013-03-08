/********************************************************************
** hook_helpers.h
** Author: Cody Hartwig <chartwig@cs.cmu.edu>
**         Heng Yin <hyin@ece.cmu.edu>
**
** Owned and copyright by the BitBlaze group, 2007. All rights reserved.
** Do not copy, disclose, or distribute without explicit written
** permission.
**
** This file should be included where these functions are defined.
** XXX: currently this is hook_helpers.c
**
*/

#ifndef _HOOK_HELPERS_H_
#define _HOOK_HELPERS_H_

#include <inttypes.h>

/* apparently taintcheck.h assumes this has already been included */
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
extern int check_ti();
extern int inc_ti();
extern int dec_ti();
extern uint64_t get_reg_taint(int reg_id);
extern int clean_taint_reg(int reg_id);
extern uint32_t get_retaddr();

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // _HOOK_HELPERS_H_

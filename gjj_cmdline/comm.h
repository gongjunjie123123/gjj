/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation.
 * Copyright (c) 2009, Olivier MATZ <zer0@droids-corp.org>
 * All rights reserved.
 */

#ifndef _COMM_H_
#define _COMM_H_


/**
 * @file
 *
 * Command line API
 */

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdint.h>

#define __rte_unused
#define __rte_format_printf(a, b) 
#define __rte_experimental 

#define    RTE_LIBRTE_CMDLINE_DEBUG
#define    RTE_MIN(a, b) ((a) < (b) ? (a) : (b))


#ifdef __cplusplus
}
#endif

#endif /*  */



/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation.
 * Copyright (c) 2009, Olivier MATZ <zer0@droids-corp.org>
 * All rights reserved.
 */

#ifndef _RDLINE_H_
#define _RDLINE_H_

/**
 * This file is a small equivalent to the GNU readline library, but it
 * was originally designed for small systems, like Atmel AVR
 * microcontrollers (8 bits). It only uses malloc() on object creation.
 *
 * Obviously, it does not support as many things as the GNU readline,
 * but at least it supports some interesting features like a kill
 * buffer and a command history.
 *
 * It also have a feature that does not have the GNU readline (as far
 * as I know): we can have several instances of it running at the same
 * time, even on a monothread program, since it works with callbacks.
 *
 * The lib is designed for a client-side or a server-side use:
 * - server-side: the server receives all data from a socket, including
 *   control chars, like arrows, tabulations, ... The client is
 *   very simple, it can be a telnet or a minicom through a serial line.
 * - client-side: the client receives its data through its stdin for
 *   instance.
 */

#include <stdio.h>
//#include <rte_compat.h>
#include "cmdline_cirbuf.h"
#include "cmdline_vt100.h"
#include "comm.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rdline;

typedef int (rdline_write_char_t)(struct rdline *rdl, char);
typedef void (rdline_validate_t)(struct rdline *rdl,
				 const char *buf, unsigned int size);
typedef int (rdline_complete_t)(struct rdline *rdl, const char *buf,
				char *dstbuf, unsigned int dstsize,
				int *state);

/**
 * Allocate and initialize a new rdline instance.
 *
 * \param write_char The function used by the function to write a character
 * \param validate A pointer to the function to execute when the
 *                 user validates the buffer.
 * \param complete A pointer to the function to execute when the
 *                 user completes the buffer.
 * \param opaque User data for use in the callbacks.
 *
 * \return New rdline object on success, NULL on failure.
 */
__rte_experimental
struct rdline *rdline_new(rdline_write_char_t *write_char,
			  rdline_validate_t *validate,
			  rdline_complete_t *complete,
			  void *opaque);

/**
 * Free an rdline instance.
 *
 * \param rdl A pointer to an initialized struct rdline.
 *            If NULL, this function is a no-op.
 */
__rte_experimental
void rdline_free(struct rdline *rdl);

/**
 * Init the current buffer, and display a prompt.
 * \param rdl A pointer to a struct rdline
 * \param prompt A string containing the prompt
 */
void rdline_newline(struct rdline *rdl, const char *prompt);

/**
 * Call it and all received chars will be ignored.
 * \param rdl A pointer to a struct rdline
 */
void rdline_stop(struct rdline *rdl);

/**
 * Same than rdline_stop() except that next calls to rdline_char_in()
 * will return RDLINE_RES_EXITED.
 * \param rdl A pointer to a struct rdline
 */
void rdline_quit(struct rdline *rdl);

/**
 * Restart after a call to rdline_stop() or rdline_quit()
 * \param rdl A pointer to a struct rdline
 */
void rdline_restart(struct rdline *rdl);

/**
 * Redisplay the current buffer
 * \param rdl A pointer to a struct rdline
 */
void rdline_redisplay(struct rdline *rdl);

/**
 * Reset the current buffer and setup for a new line.
 *  \param rdl A pointer to a struct rdline
 */
void rdline_reset(struct rdline *rdl);


/* return status for rdline_char_in() */
#define RDLINE_RES_SUCCESS       0
#define RDLINE_RES_VALIDATED     1
#define RDLINE_RES_COMPLETE      2
#define RDLINE_RES_NOT_RUNNING  -1
#define RDLINE_RES_EOF          -2
#define RDLINE_RES_EXITED       -3

/**
 * append a char to the readline buffer.
 * Return RDLINE_RES_VALIDATE when the line has been validated.
 * Return RDLINE_RES_COMPLETE when the user asked to complete the buffer.
 * Return RDLINE_RES_NOT_RUNNING if it is not running.
 * Return RDLINE_RES_EOF if EOF (ctrl-d on an empty line).
 * Else return RDLINE_RES_SUCCESS.
 * XXX error case when the buffer is full ?
 *
 * \param rdl A pointer to a struct rdline
 * \param c The character to append
 */
int rdline_char_in(struct rdline *rdl, char c);

/**
 * Return the current buffer, terminated by '\0'.
 * \param rdl A pointer to a struct rdline
 */
const char *rdline_get_buffer(struct rdline *rdl);


/**
 * Add the buffer to history.
 * return < 0 on error.
 * \param rdl A pointer to a struct rdline
 * \param buf A buffer that is terminated by '\0'
 */
int rdline_add_history(struct rdline *rdl, const char *buf);

/**
 * Clear current history
 * \param rdl A pointer to a struct rdline
 */
void rdline_clear_history(struct rdline *rdl);

/**
 * Get the i-th history item
 */
char *rdline_get_history_item(struct rdline *rdl, unsigned int i);

/**
 * Get maximum history buffer size.
 */
__rte_experimental
size_t rdline_get_history_buffer_size(struct rdline *rdl);

/**
 * Get the opaque pointer supplied on struct rdline creation.
 */
__rte_experimental
void *rdline_get_opaque(struct rdline *rdl);

#ifdef __cplusplus
}
#endif

#endif /* _RDLINE_H_ */

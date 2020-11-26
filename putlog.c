/*-
 * Copyright 2003-2020 Hiroki Sato <hrs@allbsd.org>.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "anongitssh.h"
#include "putlog.h"

#ifdef	NO_SYSLOG
bool putlog_syslog = false;
#else
bool putlog_syslog = true;
#endif

void putlog(int p, const char *msg, ...)
{
	va_list ap;

	FILE *fp;
	char msgbuf[PUTLOG_MAXLEN];

	switch (p) {
	case LOG_ERR:
		fp = stderr;
		break;
	case LOG_DEBUG:
	case LOG_INFO:
		fp = stdout;
		break;
	default:
		return;
	}
	msgbuf[0] = '\0';

	va_start(ap, msg);
	vsnprintf(msgbuf, sizeof(msgbuf), msg, ap);
	va_end(ap);

	if (putlog_syslog)
		(void)syslog(p, "%s", msgbuf);
	else
		(void)fprintf(fp, "%s\n", msgbuf);
	return;
}

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
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef	README_URL
#define README_URL	"https://www.allbsd.org/git.html"
#endif
#ifndef	CONTACT
#define CONTACT		"hrs@allbsd.org"
#endif
#ifndef	REPODIR_PREFIX
#define	REPODIR_PREFIX	"/var/home/anongit/freebsd"
#endif
#ifndef	ANONGIT_USER
#define	ANONGIT_USER	"anongit"
#endif
#ifndef	GITUPLOADPACK_PATH
#define	GITUPLOADPACK_PATH	"/usr/local/libexec/git-upload-pack"
#endif

#ifndef DEFAULT_PATH
#define	DEFAULT_PATH	"/bin:/usr/bin"
#endif
#ifndef DEFAULT_SHELL
#define DEFAULT_SHELL	"/bin/sh"
#endif

#endif /* _CONFIG_H_ */

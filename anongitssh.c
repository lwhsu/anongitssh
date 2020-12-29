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

#if defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__)
#include <sys/param.h>
#endif

#include <dirent.h>
#include <errno.h>
#if defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__)
#include <paths.h>
#endif
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "anongitssh.h"
#include "config.h"
#include "putlog.h"

static const struct chrootenv anongit_ce[] = {
	{
		.ce_repodir_prefix = REPODIR_PREFIX,
		.ce_exec = GITUPLOADPACK_PATH,
		.ce_chrootdir = "/",
		.ce_nam = ANONGIT_USER,
		.ce_contact = CONTACT
	},
	{ NULL, NULL, NULL, NULL, NULL }
};

void usage(void);

int
main(int argc, char *argv[])
{
	struct passwd *pw;
	struct chrootenv const *ce = NULL;
	DIR *dirp;
	char *env[6], **e;	/* Max environment variables for git. */
	char *p, *p0;
	char *g_args_str;
	char *g_argv[32];	/* Max arguments for git. */
	char repodir[PATH_MAX];
	char repolabel[PATH_MAX];
	int g_argc;
	int i;

	memset(&g_argv[0], 0, sizeof(g_argv));
	memset(&repodir[0], 0, sizeof(repodir));
	memset(&repolabel[0], 0, sizeof(repolabel));
	memset(&env[0], 0, sizeof(env));
	e = &env[0];
	if((*e++ = strdup("PATH="  DEFAULT_PATH)) == NULL)
		return(EXIT_FAILURE);
	if((*e++ = strdup("SHELL=" DEFAULT_SHELL)) == NULL)
		return(EXIT_FAILURE);
	if((*e++ = strdup("HOME=/")) == NULL)
		return(EXIT_FAILURE);
	if((*e++ = strdup("GIT_PROTOCOL=version=2")) == NULL)
		return(EXIT_FAILURE);
	*e = NULL;
  
	if (putlog_syslog)
		openlog("anongitssh", LOG_PID | LOG_NDELAY, LOG_USER);

	if ((pw = getpwuid(getuid())) == NULL) {
		putlog(LOG_ERR, "Internal error: uid %d: %s", getuid(),
		    strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (pw->pw_dir == NULL) {
		putlog(LOG_ERR, "Internal error: no home directory specified.");
		exit(EXIT_FAILURE);
	}
	for (i = 0; anongit_ce[i].ce_repodir_prefix != NULL; i++) {
		putlog(LOG_DEBUG,
        	    "DEBUG: Looking up uid from the authoried users list: "
		    "pw->pw_name %s: anongit_ce[i].ce_nam: %s",
        	    pw->pw_name, anongit_ce[i].ce_nam);
		if ((strlen(pw->pw_name) == strlen(anongit_ce[i].ce_nam)) &&
		    strcmp(pw->pw_name, anongit_ce[i].ce_nam) == 0) {
			ce = &anongit_ce[i];
			break;
		}
	}
	if (ce == NULL) {
		putlog(LOG_ERR, "Internal error: "
		    "No match in the authorized users list");
		return(EXIT_FAILURE);
	}
	if (setuid(0) == -1) {
		putlog(LOG_ERR, "Internal error: "
		    "Root privilege is required: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* now a superuser */

	if (chroot(ce->ce_chrootdir) == -1) {
		putlog(LOG_ERR, "Internal error: "
		    "chroot to %s: %s", ce->ce_chrootdir, strerror(errno));
		exit(EXIT_FAILURE);
	} else {
		chdir("/");
	}
	if ((pw = getpwnam(ce->ce_nam)) == NULL) {
		putlog(LOG_ERR, "Internal error: "
		    "username %s: not found", ce->ce_nam);
		exit(EXIT_FAILURE);
	}
	if (setuid(pw->pw_uid)) {
		putlog(LOG_ERR, "Internal error: "
		    "uid %d: %s", pw->pw_uid, strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* now a restricted user */

	if ((dirp = opendir(ce->ce_repodir_prefix)) == NULL) {
		putlog(LOG_ERR, "Internal error: "
		    "repodir_prefix = %s: %s", ce->ce_repodir_prefix,
		    strerror(errno));
		exit(EXIT_FAILURE);
	}
	closedir(dirp);
	if (argc != 3) {
		putlog(LOG_ERR, "Internal error: argc != 3: %d", argc);
		exit(EXIT_FAILURE);
	}
	if (strncmp(argv[0], "anongitssh", sizeof("anongitssh") - 1) != 0) {
		putlog(LOG_ERR, "Internal error: "
		    "argv[0] != anongitssh: %s", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (strncmp(argv[1], "-c", sizeof("-c") - 1) != 0) {
		putlog(LOG_DEBUG, "argv[1] != -c:", argv[1]);
		usage();
		exit(EXIT_FAILURE);
	}
	if (strncmp("git-upload-pack", argv[2], sizeof("git-upload-pack") - 1)
	    != 0) {
		putlog(LOG_ERR, "argv[2] != git-upload-pack: %s", argv[2]);
		usage();
		exit(EXIT_FAILURE);
	}

	g_args_str = strdup(argv[2]);
	if (g_args_str == NULL) {
		putlog(LOG_ERR, "strdup(argv[2]) failed.");
		usage();
		exit(EXIT_FAILURE);
	}
	g_argc = 0;
	p0 = g_args_str;
	while ((p = strchr(p0, ' ')) != NULL &&
	    g_argc < (int)nitems(g_argv)) {
		*p = '\0';
		g_argv[g_argc] = strdup(p0);
		putlog(LOG_DEBUG, "g_argv[%d] = %s", g_argc, g_argv[g_argc]);
		if (g_argv[g_argc] == NULL) {
			putlog(LOG_ERR, "strdup(g_argv[g_argc]) failed.");
			exit(EXIT_FAILURE);
		}
		p0 = p + 1;
		g_argc++;
	}
	/* The last argument. */
	if (*p0 != '\0' && g_argc < (int)nitems(g_argv)) {
		g_argv[g_argc] = strdup(p0);
		putlog(LOG_DEBUG, "DEBUG: g_argv[%d] = %s", g_argc,
		    g_argv[g_argc]);
		if (g_argv[g_argc] == NULL) {
			putlog(LOG_ERR, "Internal error: "
			    "strdup(g_argv[g_argc]) failed.");
			exit(EXIT_FAILURE);
		}
		g_argc++;
	}
	if (g_argc < 2) {
		putlog(LOG_ERR, "Access denied: g_argc < 2");
		usage();
		exit(EXIT_FAILURE);
	}
	free(g_args_str);
	/* Remove dangerous characters in the repodir. */
	p = &repolabel[0];
	p0 = g_argv[g_argc - 1];
	while (*p0 != '\0') {
		while (*p0 == '\'' || *p0 == '\"' || *p0 == '&' ||
		    *p0 == ';' || *p0 == '/')
			p0++;
		*p++ = *p0++;
	}
	/* Dot or dotdot in the repolabel should be rejected. */
	if (strncmp(repolabel, "..", sizeof("..")) == 0 ||
	    strncmp(repolabel, ".", sizeof(".")) == 0) {
		putlog(LOG_ERR, "Access denied: "
		    "repodir (%s) is dot or dotdot", repolabel);
		usage();
		exit(EXIT_FAILURE);
	}
	snprintf(repodir, sizeof(repodir), "%s/%s",
	    ce->ce_repodir_prefix, repolabel);
	if ((dirp = opendir(repodir)) == NULL) {
		putlog(LOG_ERR, "Access denied: %s: %s", repodir,
		    strerror(errno));
		usage();
		exit(EXIT_FAILURE);
	}
	/* Replace the repo directory. */
	free(g_argv[g_argc - 1]);
	g_argv[g_argc - 1] = repodir;

	putlog(LOG_INFO, "Connected successful: %s", repodir);
	for (i = 0; i < g_argc; i++)
		putlog(LOG_DEBUG, "DEBUG: g_argv[%d] = %s", i, g_argv[i]);

	if (putlog_syslog)
		closelog();

	execle(ce->ce_exec, g_argv[0], g_argv[g_argc - 1], NULL, env);
	perror("execle");
	putlog(LOG_ERR, "%s: unable to exec", ce->ce_exec);

	return(EXIT_FAILURE);
}

void usage(void)
{
	(void)fprintf(stderr,
	    "==> ERROR: invalid request.  "
	    "For instructions, visit this URL:\r\n\t"
	    README_URL
	    "\r\n");
	sleep(10);
	exit(EXIT_SUCCESS);
}

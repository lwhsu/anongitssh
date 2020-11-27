#

PROG=   anongitssh
SRCS=	anongitssh.c putlog.c
MK_DEBUG_FILES=	no

README_URL?=		https://www.allbsd.org/anongit.html
REPODIR_PREFIX?=	/var/home/anongit/freebsd
ANONGIT_USER?=		anongit
CONTACT?=		hrs@allbsd.org
GITUPLOADPACK_PATH?=	/usr/local/bin/git-upload-pack

BINOWN?=	root
BINMODE?=	4111
BINDIR?=	/usr/local/libexec
SHAREDIR?=	/usr/local/share
# Clang/LLVM
CFLAGS+=	-std=gnu99 \
		-fstack-protector-strong -Wsystem-headers \
		-Werror -Wall -Wno-format-y2k -W -Wno-unused-parameter \
		-Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith \
		-Wreturn-type -Wcast-qual -Wwrite-strings -Wswitch -Wshadow \
		-Wunused-parameter -Wcast-align -Wchar-subscripts -Winline \
		-Wnested-externs -Wredundant-decls -Wold-style-definition \
		-Wno-pointer-sign -Wmissing-variable-declarations \
		-Wthread-safety -Wno-empty-body -Wno-string-plus-int \
		-Wno-unused-const-variable  -Qunused-arguments
.if defined(NO_SYSLOG)
CFLAGS+=	-DNO_SYSLOG
.endif
.if defined(README_URL)
CFLAGS+=	-DREADME_URL=\"${README_URL}\"
.endif
.if defined(ANONGIT_USER)
CFLAGS+=	-DANONGIT_USER=\"${ANONGIT_USER}\"
.endif
.if defined(REPODIR_PREFIX)
CFLAGS+=	-DREPODIR_PREFIX=\"${REPODIR_PREFIX}\"
.endif
.if defined(REPODIR_PREFIX)
CFLAGS+=	-DCONTACT=\"${CONTACT}\"
.endif
.if defined(GITUPLOADPACK_PATH)
CFLAGS+=	-DGITUPLOADPACK_PATH=\"${GITUPLOADPACK_PATH}\"
.endif

.include <bsd.prog.mk>

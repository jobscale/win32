#############################################################################
# Version number (for RPM)
#############################################################################

VERSION_MAJOR = 3
VERSION_MINOR = 0
VERSION_POINT = 17
CODE_STATUS   = stable
VERSION       = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_POINT)
SNAPVERSION   = $(VERSION)-$(shell date "+%Y%m%d")


#############################################################################
# Build tools
#############################################################################

PROGRAM    = pRunUAC.exe
CC         = gcc-3
ECHO       = echo
GZIP_PROG  = gzip

# id -u is not universal. FIXME: need to set from configure. Breaks on
# Solaris.
#ID         = id -u
ID         = id
LD         = gcc-3
RM         = rm -f
CP         = cp -f
RMDIR      = rmdir
MKDIR      = ./mkinstalldirs
STRIP_PROG = strip
SED        = sed
OD         = od
GREP       = grep
CAT        = cat
MV         = mv
TAR        = tar
LN         = ln
TOUCH      = touch
KILL       = kill
CHMOD      = chmod
CHOWN      = chown
CHGRP      = chgrp
GROUPS     = groups
WDUMP      =  -dump
JADECAT    = 
JADEBIN    = false
DB         = $(JADEBIN) $(JADECAT) -ihtml -t sgml  -D.. -d ldp.dsl\#html
DB2HTML    = false
MAN2HTML   = man2html
G2H_CMD    = groff -mandoc -Thtml
TARGET_OS  = i686-pc-cygwin
PERL       = perl
DOC_DIR    = doc/source
DOC_TMP    = $(DOC_DIR)/tmp
DOC_STATUS = p-stable

# Program to do LF->CRLF
#
# The sed version should be the most portable, but it doesn't for for me,
# the other two do.  FIXME.
#   - Jon
#DOSFILTER  = $(SED) -e $$'s,$$,\r,'
#DOSFILTER  = gawk -v ORS='\r\n' '{print $0;}'
DOSFILTER  = $(PERL) -p -e 's/\n/\r\n/'
CVSROOT    = :pserver:anonymous@ijbswa.cvs.sourceforge.net:/cvsroot/ijbswa
#TMPDIR     := $(shell mktemp -d /tmp/$(PROGRAM).XXXXXX)
# If your SF user name differs from your local one,
# change this to "ssh -l sf-username"
SSH	= ssh


#############################################################################
# Filenames and libraries
#############################################################################

C_SRC  = cre.cpp

C_OBJS = $(C_SRC:.cpp=.o)
C_HDRS = $(C_SRC:.cpp=.h)

W32_OBJS  = app.res
W32_LIB   = -lwsock32 -lcomctl32

# PThreads library, if needed.
PTHREAD_LIB  = #-lpthreadGC

SRCS         = $(C_SRC)
OBJS         = $(C_OBJS) $(W32_OBJS)
HDRS         = $(C_HDRS)
LIBS         = -lws2_32  -lz $(W32_LIB) $(PTHREAD_LIB)


#############################################################################
# Compiler switches
#############################################################################

# The flag "-mno-win32" can be used by Cygwin to emulate a un?x type build.
# The flag "-mwindows -mno-cygwin" will cause Cygwin to use MingW32 for a
# Win32 GUI build.
# The flag "-pthread" is required if using Pthreads under Linux (and
# possibly other OSs).
SPECIAL_CFLAGS = -mwindows -mno-cygwin

# Add your flags here 
OTHER_CFLAGS =   

CFLAGS = -pipe -O2 -DWINVER=0x501  $(OTHER_CFLAGS) $(SPECIAL_CFLAGS) -Wall \
          -Ipcre 

LDFLAGS =  $(DEBUG_CFLAGS) $(SPECIAL_CFLAGS)


#############################################################################
# Define this explicitly because Solaris is broken!
#############################################################################
all: $(PROGRAM)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

app.res: app.rc app.ico app.manifest
	windres -D__MINGW32__=0.2 -O coff -i $< -o $@


#############################################################################
# Build section.
#
# There should NOT be any targets above this line.
#############################################################################
clean:
	$(RM) $(OBJS) $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(LD) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(LIBS)


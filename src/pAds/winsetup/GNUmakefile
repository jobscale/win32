##########################################################################
# $Id: GNUmakefile,v 1.20 2009/03/08 20:41:13 ler762 Exp $
##########################################################################
#
# Written by and Copyright (C) 2002-2009 the Privoxy team.
# http://www.privoxy.org/
#
# This script originally written by and Copyright (C) 2002
# Jonathan Foster
#
# This program is free software; you can redistribute it 
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will
# be useful, but WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# The GNU General Public License should be included with
# this file.  If not, you can view it at
# http://www.gnu.org/copyleft/gpl.html
# or write to the Free Software Foundation, Inc., 59
# Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
##########################################################################
#
# $Log: GNUmakefile,v $
# Revision 1.20  2009/03/08 20:41:13  ler762
# work-around for cygwin/vista permissions problem after copying
# /bin/mgwz.dll to the build directory where the build process dies with
#   File: failed opening file "build\mgwz.dll"
#   Error in script "pAds_winthreads.nsi" on line 75 -- aborting creation process
#
# Revision 1.19  2009/02/22 15:54:58  ler762
# The user manual links to the developer manual so we need
# to include doc/developer-manual in the distribution.
#
# Revision 1.18  2009/02/21 15:14:36  ler762
# Update the copyright notice
# Set the "user-manual" location in config.txt to be the locally installed docs
#
# Revision 1.17  2009/02/12 15:41:06  ler762
# include user.filter in the distribution package
#
# Revision 1.16  2009/02/12 14:16:42  ler762
# remove --disable-dynamic-pcrs option (fixes make warning message)
# remove references to jarfile & standard.action
# add unix2dos conversion of match-all.action
#
# Revision 1.15  2008/07/31 05:09:59  ler762
# doc/user-manual.txt and doc/faq.txt were removed from the document set
#
# Revision 1.14  2007/12/16 21:57:36  gjmurphy
# fix for bug #1440012: Default trust file not found due to missing ".txt" extension
#
# Revision 1.13  2007/12/16 20:07:46  gjmurphy
# [temporary] fix for bug #1850972: local user manual directory issue, CONFIG.TXT
#
# Revision 1.12  2007/12/16 16:26:54  gjmurphy
# updated for building v3.0.7;fixed nsis breakages;enabled building with zlib support;can now be build against a checked out current/;switched compressor settings to reduce installer size
#
# Revision 1.11  2006/08/15 20:53:02  david__schmidt
# Windows (Cygwin) wasn't building the text config files properly.
#
# Revision 1.10  2006/08/14 11:14:11  david__schmidt
# Windows service integration
#
# Revision 1.9  2002/09/12 14:57:11  oes
# Sync with branch: Include p_doc.css
#
# Revision 1.8  2002/09/12 13:34:56  oes
# - Added standard.action, user.action
# - Removed dosfilter for (gone) re_filterfile
# - Added doc/images (fixes bug #606800)
#
# Revision 1.7  2002/04/11 22:12:45  jongfoster
# Using docs from CVS
#
# Revision 1.6  2002/04/02 22:44:11  jongfoster
# Updating to use checked-in version of NSIS.
# Defining almost all external programs at the top of the file.
# Adding copyright and log comments.
#
#
##########################################################################


##########################################################################
# External programs
##########################################################################
SED        = sed
PERL       = perl
AWK        = awk
WGET       = wget
STRIP      = strip
AUTOCONF   = autoconf
AUTOHEADER = autoheader

# Program to copy a directory from a web site to the local disk.
WEBMIRROR = $(WGET) -nv -nH -nd -N -r -np

# Program to do LF->CRLF
#
# The sed version should be the most portable, but it doesn't work for me,
# the other two do.  FIXME.
#   - Jon
#
DOSFILTER = $(SED) -e $$'s,$$,\r,'
#DOSFILTER = $(AWK) -v ORS='\r\n' '{print $0;}'
#DOSFILTER = $(PERL) -p -e 's/\n/\r\n/'

# Path to NSIS
MAKENSIS = ./nsis/nsis-2.30.exe


##########################################################################
# More configuration
##########################################################################

# Directory containing the source code
SOURCE   = ../current

# Directory to build an image of the installed file system in.
DEST     = ./build

# The name of the EXE installer (must match the name specified in
# the .NSI script)
TARGET=privoxy_setup.exe

# The NSIS script that generates the installer.
NSIFILE=pAds_winthreads.nsi


##########################################################################
# Rules
##########################################################################

winrelease: $(TARGET)

winbuild: $(DEST)/pAds.exe templates inifiles doc webdoc

clean:
	rm -rf $(DEST) $(TARGET)

clobber: clean
	rm -f $(TARGET) $(SOURCE)/pAds.exe

.PHONY: winrelease winbuild clean


$(TARGET): $(NSIFILE) winbuild
	$(MAKENSIS) $<

$(DEST):
	mkdir $@

$(DEST)/templates:
	mkdir $@

$(DEST)/doc:
	mkdir $@

$(DEST)/%.txt: $(SOURCE)/% $(DEST)
	$(DOSFILTER) < $< > $@ 


inifiles: $(DEST)/config.txt $(DEST)/match-all.action $(DEST)/default.action $(DEST)/user.action \
          $(DEST)/default.filter $(DEST)/trust.txt $(DEST)/user.filter


$(DEST)/config.txt: $(SOURCE)/config $(DEST)
	$(SED) -e 's!trustfile trust!trustfile trust.txt!' \
	       -e 's!logfile logfile!logfile privoxy.log!' \
               -e 's!#user-manual http://www.privoxy.org/user-manual/!user-manual ./doc/user-manual/!' \
	       -e 's!#Win32-only: !!' \
	       < $< | \
	       $(DOSFILTER) > $@

$(DEST)/match-all.action: $(SOURCE)/match-all.action $(DEST)
	$(DOSFILTER) < $< > $@

$(DEST)/default.action: $(SOURCE)/default.action $(DEST)
	$(DOSFILTER) < $< > $@ 

$(DEST)/user.action: $(SOURCE)/user.action $(DEST)
	$(DOSFILTER) < $< > $@ 

$(DEST)/default.filter: $(SOURCE)/default.filter $(DEST)
	$(DOSFILTER) < $< > $@ 

$(DEST)/user.filter: $(SOURCE)/user.filter $(DEST)
	$(DOSFILTER) < $< > $@ 

#gjm hack: grab mgwz.dll from the mingw-zlib cygwin package.

$(DEST)/pAds.exe: $(SOURCE)/pAds.exe $(DEST)
	cp /bin/mgwz.dll $(DEST)
#cygwin/Vista permissions hack
	chmod u+rwx $(DEST)/mgwz.dll
	$(STRIP) -o $@ $<

$(SOURCE)/configure: $(SOURCE)/configure.in
	cd $(SOURCE) && $(AUTOCONF)

$(SOURCE)/config.h.in: $(SOURCE)/configure $(SOURCE)/acconfig.h
	cd $(SOURCE) && $(AUTOHEADER)

$(SOURCE)/config.h: $(SOURCE)/configure $(SOURCE)/config.h.in
	cd $(SOURCE) && ./configure --enable-mingw32 --disable-pthread --disable-dynamic-pcre --enable-zlib

$(SOURCE)/pAds.exe: $(SOURCE)/configure $(SOURCE)/config.h
	cd $(SOURCE) && $(MAKE) clean
	cd $(SOURCE) && $(MAKE)


templates: $(patsubst $(SOURCE)/%,$(DEST)/%,$(wildcard $(SOURCE)/templates/*))

$(DEST)/templates/%: $(SOURCE)/templates/% $(DEST)/templates
	if test -f $< ; then $(DOSFILTER) < $< > $@ ; fi


doc: $(DEST)/AUTHORS.txt $(DEST)/README.txt $(DEST)/LICENSE.txt

webdoc: $(DEST)/doc/developer-manual $(DEST)/doc/faq $(DEST)/doc/images $(DEST)/doc/user-manual
	cp $(SOURCE)/doc/webserver/p_doc.css $(DEST)/doc
	cp $(SOURCE)/doc/webserver/p_doc.css $(DEST)/doc/user-manual/


$(DEST)/doc/gpl.html: $(SOURCE)/doc/gpl.html $(DEST)/doc
	$(DOSFILTER) < $< > $@ 

$(DEST)/doc/%.txt : $(SOURCE)/doc/text/%.txt $(DEST)/doc
	$(DOSFILTER) < $< > $@ 

$(DEST)/doc/developer-manual: $(DEST)/doc $(SOURCE)/doc/webserver/developer-manual/index.html
	cp -r $(SOURCE)/doc/webserver/developer-manual $(DEST)/doc

$(DEST)/doc/user-manual: $(DEST)/doc $(SOURCE)/doc/webserver/user-manual/index.html
	cp -r $(SOURCE)/doc/webserver/user-manual $(DEST)/doc

$(DEST)/doc/faq: $(DEST)/doc $(SOURCE)/doc/webserver/faq/index.html
	cp -r $(SOURCE)/doc/webserver/faq $(DEST)/doc

$(DEST)/doc/images: $(DEST)/doc
	cp -r $(SOURCE)/doc/webserver/images $(DEST)/doc

##########################################################################
# That's all, folks!
##########################################################################


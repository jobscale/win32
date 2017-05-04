# //////////////////////////////////////////////////////////////////////////////
# // Copyright 2005-2008 Plustar Corporation. All rights reserved. プラスター

# Makefile

TARG0=../pConsolePage.inl

SRC0=401.html 404.html

#

all:
	make $(TARG0)
	@echo "OK!";

$(TARG0): $(SRC0)
	echo | awk '{printf("\nstatic const TCHAR js_err_401_html[] =\n");}' > $@; \
	sed -e 's/"/\\"/g' -e 's/^/\t"/' -e 's/$$/\\n"/' 401.html >> $@; \
	echo | awk '{printf(";\n");}' >> $@; \
	echo | awk '{printf("\nstatic const TCHAR js_err_404_html[] =\n");}' >> $@; \
	sed -e 's/"/\\"/g' -e 's/^/\t"/' -e 's/$$/\\n"/' 404.html >> $@; \
	echo | awk '{printf(";\n");}' >> $@

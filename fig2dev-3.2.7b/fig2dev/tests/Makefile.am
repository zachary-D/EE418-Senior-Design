## Run automake on this file to produce Makefile.in

# Fig2dev: Translate Fig code to various Devices
# Copyright (c) 1991 by Micah Beck
# Parts Copyright (c) 1985-1988 by Supoj Sutanthavibul
# Parts Copyright (c) 1989-2015 by Brian V. Smith
# Parts Copyright (c) 2015-2018 by Thomas Loimer
#
# Any party obtaining a copy of these files is granted, free of charge, a
# full and unrestricted irrevocable, world-wide, paid up, royalty-free,
# nonexclusive right and license to deal in this software and documentation
# files (the "Software"), including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense and/or sell copies
# of the Software, and to permit persons who receive copies from any such
# party to do so, with the only requirement being that the above copyright
# and this permission notice remain intact.

# fig2dev/tests/Makefile.am
# Author: Thomas Loimer, 2016-2018.


# testsuite and package.m4 must be distributed, hence they are
# written into the source tree
TESTSUITE = $(srcdir)/testsuite
EXTRA_DIST = testsuite package.m4 $(TESTSUITE_AT) atlocal.in data
TESTSUITE_AT = testsuite.at bitmaps.at input.at output.at read.at

# ??
DISTCLEANFILES = atconfig
MAINTAINERCLEANFILES = Makefile.in $(TESTSUITE) $(srcdir)/package.m4

$(srcdir)/package.m4: $(top_srcdir)/configure.ac
	:;{ \
	  echo 'm4_define([AT_PACKAGE_NAME], [@PACKAGE_NAME@])'; \
	  echo 'm4_define([AT_PACKAGE_TARNAME], [@PACKAGE_TARNAME@])'; \
	  echo 'm4_define([AT_PACKAGE_VERSION], [@PACKAGE_VERSION@])'; \
	  echo 'm4_define([AT_PACKAGE_STRING], [@PACKAGE_STRING@])'; \
	  echo 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	  echo 'm4_define([AT_PACKAGE_URL], [@PACKAGE_URL@])'; \
	} >'$(srcdir)/package.m4'

check_PROGRAMS = test1
# test1 calls malloc(). AC_FUNC_MALLOC may replace malloc() by rpl_malloc()
# which is defined in malloc.o. The latter is contained in $(LIBOBJS) in
# ${LIBOBJDIR}. Specifically, ./configure CFLAGS="-fsanitize=address" causes
# this replacment. Unfortunately, using test1_LDADD, causes make distcheck to
# fail, because both this Makefile and the one in .. try to clean
# ../libs/.deps/*.Po on make distcheck, when ./configure is used with
# --enable-dependency-tracking, which is the default. Using
# EXTRA_test1_DEPENDENCIES = $(LIBOBJS) also failed.
#
# Uncomment the line below, when using CFLAGS=-fsanitize=address
#test1_LDADD = $(LIBOBJS)

# keep the definitions below in sync with those in ../dev/Makefile.am
if ENABLE_I18N
AM_CPPFLAGS = -DI18N_DATADIR="\"$(i18ndir)\""
endif

check-local: atconfig $(TESTSUITE) atlocal
	$(SHELL) '$(TESTSUITE)' INSTALLCHECK=no $(TESTSUITEFLAGS)

installcheck-local: atconfig $(TESTSUITE) atlocal
	$(SHELL) '$(TESTSUITE)' AUTOTEST_PATH='$(bindir)' $(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' || \
		$(SHELL) '$(TESTSUITE)' --clean

AUTOTEST = $(AUTOM4TE) --language=autotest

$(TESTSUITE): $(srcdir)/package.m4 $(TESTSUITE_AT)
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

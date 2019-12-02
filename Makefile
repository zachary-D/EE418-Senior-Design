.PHONY: clean

#P_DIRS := fig2dev xfig

all: fig2dev xfig #$(P_DIRS)

xfig: fig2dev xfig*/Makefile
	$(MAKE) -C "xfig"*

fig2dev: fig2dev*/Makefile
	$(MAKE) -C "fig2dev"*

#.SECONDEXPANSION:
#$(P_DIRS): $$($$@*/Makefile)
#	$(MAKE) -C "$@"*

#$(P_DIRS)*/Makefile:

%/Makefile %/config.h: %/configure %/Makefile.in
	cd $(@D) && ./configure

%/Makefile.in: %/Makefile.am %/config.h.in
	cd $(@D) && automake

%/configure: %/configure.ac %/aclocal.m4
	cd $(@D) && autoconf

%/aclocal.m4: %/configure.ac
	cd $(@D) && aclocal

%/config.h.in: %/configure.ac
	cd $(@D) && autoheader

findDel := find ./* ! -wholename './Makefile' -name

#There's gotta be a better way to do this
clean:
	$(findDel) *.o -delete
	$(findDel) Makefile -delete
	$(findDel) Makefile.in -delete
	$(findDel) config.h -delete
	$(findDel) config.h.in -delete
	$(findDel) config.status -delete
	$(findDel) config.log -delete
	$(findDel) configure -delete
	$(findDel) aclocal.m4 -delete
	$(findDel) aclocal -delete

rebuild:
	$(MAKE) clean
	$(MAKE) all

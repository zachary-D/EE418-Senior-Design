all: fig2dev fig2dev_path xfig

fig2dev: fig2dev*/Makefile
	$(MAKE) -C fig2dev*

fig2dev*/Makefile:
	cd fig2dev* && ./configure

xfig: xfig*/Makefile
	$(MAKE) -C xfig*

xfig*/Makefile:
	cd xfig* && ./configure

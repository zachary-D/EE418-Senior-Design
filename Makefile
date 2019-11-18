all: fig2dev xfig

fig2dev: fig2dev*/Makefile
	$(MAKE) -C fig2dev*

fig2dev*/Makefile: fig2dev*/configure
	cd fig2dev* && ./configure

fig2dev*/configure:
	cd fig2dev* && cp build/* ./

xfig: xfig*/Makefile
	$(MAKE) -C xfig*

xfig*/Makefile: xfig*/configure
	cd xfig* && ./configure

xfig*/configure:
	cd xfig* && cp build/* ./

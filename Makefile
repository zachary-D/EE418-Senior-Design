.PHONY: clean

all: fig2dev xfig

fig2dev: fig2dev*/Makefile
	$(MAKE) -C fig2dev*

fig2dev*/Makefile: fig2dev*/configure
	cd fig2dev* && ./configure && aclocal

fig2dev*/configure:
	cd fig2dev* && cp -R build/* ./

xfig: xfig*/Makefile
	$(MAKE) -C xfig*

xfig*/Makefile: xfig*/configure
	cd xfig* && ./configure && aclocal

xfig*/configure:
	cd xfig* && cp -R build/* ./

clean:
	$(MAKE) clean -C fig2dev*
	$(MAKE) clean -C xfig*

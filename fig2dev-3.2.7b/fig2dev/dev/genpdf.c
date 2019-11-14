/*
 * Fig2dev: Translate Fig code to various Devices
 * Parts Copyright (c) 1989-2015 by Brian V. Smith
 * Parts Copyright (c) 2015-2018 by Thomas Loimer
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and documentation
 * files (the "Software"), including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense and/or sell copies
 * of the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that the above copyright
 * and this permission notice remain intact.
 *
 */

/*
 * genpdf.c: convert fig to pdf
 *
 * Author: Brian V. Smith
 *		Uses genps functions to generate PostScript output then
 *		calls ghostscript (device pdfwrite) to convert it to pdf.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "fig2dev.h"
#include "object.h"	/* does #include <X11/xpm.h> */
#include "genps.h"

static char	*com;

void
genpdf_option(char opt, char *optarg)
{
	static bool	init = false;
	if (!init) {
		epsflag = true; /* by default, generate eps, then pdf */
		pdfflag = true;
		init = true;
	}

	/* -P...pagemode, or a pagesize (-z) is given, implies -P */
	if (opt == 'z' || opt == 'P')
		epsflag = false;
	gen_ps_eps_option(opt, optarg);
}

static void
pdf_broken_pipe(int sig)
{
	(void)	sig;

	fputs("fig2dev: broken pipe when translating to pdf\n", stderr);
	fprintf(stderr, "command was: %s\n", com);
	exit(EXIT_FAILURE);
}

void
genpdf_start(F_compound *objects)
{
	size_t	n;
	char	*ofile;

	/* divert output from ps driver to the pipe into ghostscript */
	/* but first close the output file that main() opened */
	if (tfp != stdout) {	/* equivalent to to != NULL */
		fclose(tfp);
		n = strlen(to);
		ofile = to;
	} else {
		n = 0;
		ofile = "-";
	}

	if ((com = malloc(n + 200)) == NULL) {
	       fputs("Cannot allocate memory for ghostscript command string.\n",
			stderr);
		exit(EXIT_FAILURE);
	}
	/* Notes about -ColorImageFilter:
	 * /FlateEncode produces a lossless but large image
	 * /DCTEncode produces a lossy but much smaller image
	 */
	/* -o ... is equivalent to -dBATCH -dNOPAUSE, see ghostscript Use.htm
	   and -dBATCH is equivalent to -c quit */
	sprintf(com, "%s -q -sAutoRotatePages=None "
		"-dAutoFilterColorImages=false -dColorImageFilter=/DCTEncode "
		"-sDEVICE=pdfwrite -dPDFSETTINGS=/prepress -o '%s' -",
		GSEXE, ofile);
	(void) signal(SIGPIPE, pdf_broken_pipe);
	if ((tfp = popen(com, "w")) == 0) {
		fprintf(stderr, "fig2dev: Cannot open pipe to ghostscript\n");
		fprintf(stderr, "command was: %s\n", com);
		exit(EXIT_FAILURE);
	}
	genps_start(objects);
}

int
genpdf_end(void)
{
	int	 status;

	/* wrap up the postscript output */
	if (genps_end() != 0) {
		pclose(tfp);
		free(com);
		return -1;		/* error, return now */
	}

	status = pclose(tfp);
	/* we've already closed the original output file */
	tfp = 0;	/* so main() does not close tfp again */
	if (status != 0) {
		fprintf(stderr, "Error in ghostcript command\n");
		fprintf(stderr, "command was: %s\n", com);
		free(com);
		return -1;
	}
	(void) signal(SIGPIPE, SIG_DFL);
	free(com);

	return 0;
}

struct driver dev_pdf = {
	genpdf_option,
	genpdf_start,
	genps_grid,
	genps_arc,
	genps_ellipse,
	genps_line,
	genps_spline,
	genps_text,
	genpdf_end,
	INCLUDE_TEXT
};

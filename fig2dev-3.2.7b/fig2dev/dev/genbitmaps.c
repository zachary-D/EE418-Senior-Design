/*
 * Fig2dev: Translate Fig code to various Devices
 * Copyright (c) 1991 by Micah Beck
 * Parts Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2015 by Brian V. Smith
 * Parts Copyright (c) 2015-2019 by Thomas Loimer
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
 * genbitmaps.c: convert fig to various bitmaps
 *
 * Author: Brian V. Smith
 *	Handles AutoCad Slide, GIF, JPEG, TIFF, PCX, PNG, XBM and XPM.
 *	Uses genps functions to generate PostScript output then calls
 *	ghostscript to convert it to the output language if ghostscript
 *	has a driver for that language, or to ppm if otherwise. If the
 *	latter, ppmtoxxx is then called to make the final xxx file.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>

#include "fig2dev.h"	/* includes "bool.h" */
#include "object.h"	/* does #include <X11/xpm.h> */
#include "colors.h"	/* lookup_X_color(), rgb2luminance() */
#include "genps.h"
#include "xtmpfile.h"

		/* See below, "The largest command string..." */
static char	com_buf[200 + L_xtmpnam + 256];
static char	*com;
static FILE	*errfile;
static char	errfname[L_xtmpnam] = "f2derrorXXXXXX";
static int	jpeg_quality = 75;
static int	border_margin = 0;
static int	smooth = 0;

void
genbitmaps_option(char opt, char *optarg)
{
	switch (opt) {

	case 'b':			/* border margin around bitmap */
		sscanf(optarg, "%d", &border_margin);
		break;

	case 'g':		/* background color (handled in ps driver) */
		if (lookup_X_color(optarg, &background) >= 0) {
			bgspec = true;
		} else {
			fprintf(stderr, "Can't parse color '%s', ignoring "
					"background option\n", optarg);
		}
		break;

	case 'N':			/* convert colors to grayscale */
		grayonly = 1;
		break;

	case 'q':			/* jpeg image quality */
		if (strcmp(lang, "jpeg") != 0)
			fputs("-q option only allowed for jpeg quality; "
				"ignored\n", stderr);
		sscanf(optarg, "%d", &jpeg_quality);
		if (jpeg_quality < 1)
			jpeg_quality = 1;
		else if (jpeg_quality > 100)
			jpeg_quality = 100;
		break;

	case 't':			/* GIF transparent color */
		if (strcmp(lang, "gif") != 0)
			fputs("-t option only allowed for GIF transparent "
				"color; ignored\n", stderr);
		(void) strcpy(gif_transparent, optarg);
		transspec = true;
		break;

	case 'S':			/* smoothing factor */
		sscanf(optarg, "%d", &smooth);
		if (smooth != 0 && smooth != 1 && smooth != 2 && smooth != 4) {
			fprintf(stderr, "fig2dev: bad value for -S option: %s, "
					"should be 0, 2 or 4\n", optarg);
			exit(1);
		}
		break;

	case 'G':
	case 'L':
		break;

	default:
		put_msg(Err_badarg, opt, lang);
		break;
	}
}

static void
bitmaps_broken_pipe(int sig)
{
	(void)	sig;

	fputs("fig2dev: broken pipe when trying to create image\n", stderr);
	fprintf(stderr, "command was: %s\n", com);
	exit(EXIT_FAILURE);
}

void
genbitmaps_start(F_compound *objects)
{
#ifdef GSEXE /* bracket the entire function */
	char	*gsdev;
	int	n;

	n = ceil(border_margin * THICK_SCALE);
	llx -= n;	lly -= n;
	urx += n;	ury += n;

	/* start allocating ghostscript command string */
	if (to)	{		/* equivalent to tfp != stdout */
		fclose(tfp);	/* close the output file that main() opened */
		n = strlen(to);
	} else {
		n = 0;
	}

	/*
	 * The largest command string is, allowing for long long int, 19 digits
	 * 12345678 10        20        30        40        50        602345
	 * gswin32c -q -dSAFER -r80 -gINT_MAX890123456789xINT_MAX89012345678
	 * 9 -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -sDEVICE=ppmraw -o - -
	 * { ppmquant 256 | ppmtogif -transparent \#123456; } >f2derrorXXXXXX
	 * + L_xtmpnam + strlen(to) = 197 chars + L_xtmnnam + strlen(to).
	 */
	if (n > 256 && (com = malloc(n + 200 + L_xtmpnam)) == NULL) {
		fputs("Cannot allocate memory for ghostscript command string.\n",
			stderr);
		exit(EXIT_FAILURE);
	} else {
		com = com_buf;
	}

	/* start building up ghostscript command string */
	n = sprintf(com, "%s -q -dSAFER -r80 -g%dx%d", GSEXE,
			(int) ceil(mag * (urx-llx) / THICK_SCALE),
			(int) ceil(mag * (ury-lly) / THICK_SCALE));

	/*
	 * Use ghostscript built-in drivers. In addition to these, ghostscript
	 * has drivers for bmp (bmp256, bmp16m), pcx (pcx256), tiff (tiff24nc),
	 * and png (png16m). But use the netpbm programs for the latter,
	 * because the netpbm programs produce smaller files.
	 */
	gsdev = NULL;
	if (strcmp(lang, "ppm") == 0) {
		gsdev = "ppmraw";
	} else if (strcmp(lang, "jpeg") == 0) {
		gsdev = "jpeg";
		n += sprintf(com + n, " -dJPEGQ=%d", jpeg_quality);
	}

	if (smooth > 1)
		n += sprintf(com + n,
			" -dTextAlphaBits=%d -dGraphicsAlphaBits=%d",
			smooth, smooth);

	if (gsdev) {
		n += sprintf(com + n, " -sDEVICE=%s -o ", gsdev);
		if (to)
			sprintf(com + n, "'%s' -", to);
		else
			strcat(com + n, "- -");
	} else {
		/* let ghostscript convert to ppm */
		n += sprintf(com + n, " -sDEVICE=ppmraw -o - - | ");

		/* and add the pipe to the netpbm commands */
		if (strcmp(lang, "gif") == 0) {
			if (gif_transparent[0])
				/* escape the first char of the
				   transparent color (#) for the shell */
				n += sprintf(com + n,
			      "{ ppmquant 256 | ppmtogif -transparent \\%s; }",
					gif_transparent);
			else
				n += sprintf(com + n,
					"{ ppmquant 256 | ppmtogif; }");
		} else if (strcmp(lang, "xbm") == 0) {
			n += sprintf(com + n,
				"{ ppmtopgm | pgmtopbm | pbmtoxbm; }");
		} else if (strcmp(lang, "xpm") == 0) {
			n += sprintf(com + n, "{ ppmquant 256 | ppmtoxpm; }");
		} else if (strcmp(lang, "sld") == 0) {
			n += sprintf(com + n, "ppmtoacad");
		} else if (strcmp(lang, "pcx") == 0) {
			n += sprintf(com + n, "ppmtopcx");
		} else if (strcmp(lang, "png") == 0) {
			n += sprintf(com + n, "pnmtopng");
		} else if (strcmp(lang, "tiff") == 0) {
			n += sprintf(com + n, "pnmtotiff");
		} else {
			fprintf(stderr,
				"fig2dev: unsupported image format: %s\n",
				lang);
			exit(EXIT_FAILURE);
		}

		/* netpbm programs are chatty; catch their output */
		if ((errfile = xtmpfile(errfname)) == 0)
			fprintf(stderr, "Can't create error log file %s\n",
				errfname);
		else
			n += sprintf(com + n, " 2>%s", errfname);

		if (to)
			sprintf(com + n, " >'%s'", to);
	}

	(void) signal(SIGPIPE, bitmaps_broken_pipe);
	if ((tfp = popen(com, "w")) == 0) {
		fprintf(stderr, "fig2dev: Can't open pipe to ghostscript\n");
		fprintf(stderr, "command was: %s\n", com);
		if (com != com_buf)
			free(com);
		if (errfile) {
			fclose(errfile);
			remove(errfname);
		}
		exit(EXIT_FAILURE);
	}

	/* generate eps and not ps */
	epsflag = true;
	genps_start(objects);
#else
	fputs("Ghostscript command not available. Cannot create bitmaps.\n",
		stderr);
	exit(EXIT_FAILURE);
#endif	/* #ifdef GSEXE */
}

int
genbitmaps_end(void)
{
	int	status;

	/* wrap up the postscript output */
	if (genps_end() != 0) {
		pclose(tfp);
		if (com != com_buf)
			free(com);
		if (errfile) {
			fclose(errfile);
			remove(errfname);
		}
		return -1;
	}

	status = pclose(tfp);
	tfp = 0;	/* Otherwise main() tries to close tfp again */
	(void) signal(SIGPIPE, SIG_DFL);

	if (status != 0) {
		fputs("Error in ghostcript or netpbm command\n", stderr);
		fprintf(stderr, "command was: %s\n", com);
		if (errfile == NULL) {
			fprintf(stderr,
				"Error log file %s not available\n", errfname);
		} else {
			fputs("Messages resulting:\n", stderr);
			while (!feof(errfile)) {
				if (fgets(com_buf, sizeof(com_buf), errfile) ==
						NULL)
					break;
				fprintf(stderr, "  %s", com);
			}
			fclose(errfile);
			remove(errfname);
		}
		if (com != com_buf)
			free(com);
		return -1;
	}

	/* finally, remove the temporary file and the error file */
	if (errfile) {
		fclose(errfile);
		remove(errfname);
	}
	if (com != com_buf)
		free(com);

	return status;
}

struct driver dev_bitmaps = {
	genbitmaps_option,
	genbitmaps_start,
	genps_grid,
	genps_arc,
	genps_ellipse,
	genps_line,
	genps_spline,
	genps_text,
	genbitmaps_end,
	INCLUDE_TEXT
};

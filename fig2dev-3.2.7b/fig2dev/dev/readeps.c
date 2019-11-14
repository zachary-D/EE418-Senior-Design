/*
 * Fig2dev: Translate Fig code to various Devices
 * Parts Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 2015-2017 by Thomas Loimer
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
 * readeps.c: import EPS into PostScript
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fig2dev.h"	/* includes "bool.h" */
#include "object.h"	/* does #include <X11/xpm.h> */

/* for both procedures:
     return codes:  1 : success
		    0 : failure
*/

static int	read_eps_pdf(FILE *file, int filetype, F_pic *pic,
				int *llx, int* lly, bool pdf_flag);

/* read a PDF file */

int
read_pdf(FILE *file, int filetype, F_pic *pic, int *llx, int *lly)
{
    return read_eps_pdf(file,filetype,pic,llx,lly,true);
}

/* read an EPS file */

/* return codes:  PicSuccess (1) : success
		  FileInvalid (-2) : invalid file
*/

int
read_eps(FILE *file, int filetype, F_pic *pic, int *llx, int *lly)
{
    return read_eps_pdf(file,filetype,pic,llx,lly,false);
}


static int
read_eps_pdf(FILE *file, int filetype, F_pic *pic, int *llx, int* lly,
		bool pdf_flag)
{
	(void)	filetype;
	char	buf[BUFSIZ];
	double	fllx, flly, furx, fury;
	int	nested;
	char	*c;

	pic->bit_size.x = pic->bit_size.y = 0;
	pic->subtype = P_EPS;

	/* give some initial values for bounding in case none is found */
	*llx = 0;
	*lly = 0;
	pic->bit_size.x = 10;
	pic->bit_size.y = 10;
	nested = 0;

	while (fgets(buf, BUFSIZ, file) != NULL) {
	    /* look for /MediaBox for pdf file */
	    if (pdf_flag) {
		if (!strncmp(buf, "/MediaBox", 9)) {	/* look for the MediaBox spec */
		    c = strchr(buf, '[') + 1;
		    if (c && sscanf(c, "%d %d %d %d", llx, lly, &urx, &ury) < 4)
		    {
			*llx = *lly = 0;
			urx = paperdef[0].width*72;
			ury = paperdef[0].height*72;
			put_msg("Bad MediaBox in imported PDF file %s, assuming %s size",
				pic->file, metric? "A4" : "Letter" );
		    }
		}
	    /* look for bounding box for EPS file */
	    } else if (!nested && !strncmp(buf, "%%BoundingBox:", 14)) {
		c = buf + 14;
		/* skip past white space */
		while (*c == ' ' || *c == '\t')
		    ++c;
		if (strncmp(c, "(atend)", 7)) {	/* make sure not an (atend) */
		    if (sscanf(c, "%lf %lf %lf %lf",
				&fllx, &flly, &furx, &fury) < 4) {
			fprintf(stderr,"Bad EPS bitmap file: %s\n", pic->file);
			return 0;
		    }
		    *llx = (int) floor(fllx);
		    *lly = (int) floor(flly);
		    pic->bit_size.x = (int) (furx-fllx);
		    pic->bit_size.y = (int) (fury-flly);
		    break;
		}
	    } else if (!strncmp(buf, "%%Begin", 7)) {
		++nested;
	    } else if (nested && !strncmp(buf, "%%End", 5)) {
		--nested;
	    }
	}
	fprintf(tfp, "%% Begin Imported %s File: %s\n",
				pdf_flag? "PDF" : "EPS", pic->file);
	fprintf(tfp, "%%%%BeginDocument: %s\n", pic->file);
	fputs("%\n", tfp);
	return 1;
}


/*
 * Write the eps-part of the epsi-file pointed to by in to out.
 * return codes:	 0	success,
 *			-1	nothing written,
 *		   termination	on partial write
 */
int
append_epsi(FILE *in, const char *filename, FILE *out)
{
	size_t		l = 12;
	unsigned char	buf[BUFSIZ];
	int		i;
	unsigned int	start = 0;
	unsigned int	length = 0;

	if (fread(buf, 1, l, in) != l) {
		fprintf(stderr, "Cannot read EPSI file %s.\n", filename);
		return -1;
	}
	for (i = 0; i < 4; ++i) {
		/* buf must be unsigned, otherwise the left shift fails */
		start += buf[i+4] << i*8;
		length += buf[i+8] << i*8;
	}
	/* read forward to start of eps section.
	   do not use fseek, in might be a pipe */
	if (fread(buf, 1, start - l, in) != start - l)
		return -1;

#define	EPSI_ERROR(mode)	fprintf(stderr, "Error when " mode \
		" embedded EPSI file %s.\nAborting.\n", filename), \
		exit(EXIT_FAILURE)

	l = BUFSIZ;
	while (length > l) {
		if (fread(buf, 1, l, in) != l)
			EPSI_ERROR("reading");
		length -= l;
		if (fwrite(buf, 1, l, out) != l)
			EPSI_ERROR("writing");
	}
	/* length < BUFSIZ  ( = l)*/
	if (length > 0) {
		if (fread(buf, 1, length, in) != length)
			EPSI_ERROR("reading");
		if (fwrite(buf, 1, length, out) != length)
			EPSI_ERROR("writing");
	}
	return 0;
}

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef	HAVE_STRINGS_H
#include <strings.h>
#endif
#include <math.h>
#include "pi.h"

#include "fig2dev.h"	/* includes "bool.h" */
#include "object.h"	/* does #include <X11/xpm.h> */
#include "gensvg.c"




void
genvdx_option(char opt, char *optarg)
{
    switch (opt) {
	case 'G':		/* ignore language and grid */
	case 'L':
	    break;
	case 'z':
	    (void) strcpy (papersize, optarg);
	    paperspec = true;
	    break;
	default:
	    put_msg (Err_badarg, opt, "vdx");
	    exit (1);
    }
}

void
genvdx_start(F_compound *objects)
{
    const struct paperdef	*pd;
    int     pagewidth = -1, pageheight = -1;
    int     vw, vh;
    char    date_buf[CREATION_TIME_LEN];

    fprintf(tfp, "%s\n", PREAMBLE);
    fprintf(tfp, "<!-- Creator: %s Version %s -->\n",
		  prog, PACKAGE_VERSION);

    if (creation_date(date_buf))
	fprintf(tfp, "<!-- CreationDate: %s -->\n", date_buf);
    fprintf(tfp, "<!-- Magnification: %.3g -->\n", mag);


    if (paperspec) {
	/* convert paper size from ppi to inches */
	for (pd = paperdef; pd->name != NULL; ++pd)
	    if (strcasecmp(papersize, pd->name) == 0) {
		pagewidth = pd->width;
		pageheight = pd->height;
		strcpy(papersize, pd->name);	/* use the "nice" form */
		break;
	    }
	if (pagewidth < 0 || pageheight < 0) {
	    (void) fprintf(stderr, "Unknown paper size `%s'\n", papersize);
	    exit(1);
	}
	if (landscape) {
	    vh = pagewidth;
	    vw = pageheight;
	} else {
	    vw = pagewidth;
	    vh = pageheight;
	}
    } else {
	vw = ceil((urx - llx) * 72. * mag / ppi);
	vh = ceil((ury - lly) * 72. * mag / ppi);
    }
    
    /* fputs("<svg\txmlns=\"http://www.w3.org/2000/svg\"\n", tfp);
    fputs("\txmlns:xlink=\"http://www.w3.org/1999/xlink\"\n", tfp);
    fprintf(tfp,
	"\twidth=\"%dpt\" height=\"%dpt\"\n\tviewBox=\"%d %d %d %d\">\n",
	vw, vh, llx, lly, urx - llx , ury - lly); */

    if (objects->comments)
	print_svgcomments("<desc>", objects->comments, "</desc>\n");
    fputs("<g fill=\"none\">\n", tfp);

}

int
genvdx_end(void)
{
    fprintf(tfp, "</g>\n</vdx>\n");
    return 0;
}



void
print_svgcomments(char *s1, F_comment *comments, char *s2)
{
	unsigned char	*c;
	while (comments) {
		fputs(s1, tfp);
		for (c = (unsigned char *)comments->comment; *c; ++c)
			put_sanitized_char((int)*c);
		fputs(s2, tfp);
		comments = comments->next;
	}
}



/* driver defs */

struct driver dev_vdx = {
	genvdx_option,
	genvdx_start,
	gendev_nogrid,
	gensvg_arc,
	gensvg_ellipse,
	gensvg_line,
	gensvg_spline,
	gensvg_text,
	genvdx_end,
	INCLUDE_TEXT
};

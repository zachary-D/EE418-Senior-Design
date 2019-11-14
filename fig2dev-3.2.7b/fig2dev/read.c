/*
 * Fig2dev: Translate Fig code to various Devices
 * Copyright (c) 1991 by Micah Beck
 * Parts Copyright (c) 1985-1988 by Supoj Sutanthavibul
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef	HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef	HAVE_STRERROR
#include <errno.h>
#endif
#include <ctype.h>
#include <limits.h>

#include "fig2dev.h"	/* includes "bool.h" */
#include "alloc.h"
#include "object.h"	/* does #include <X11/xpm.h> */
#include "free.h"
#include "read.h"
#include "trans_spline.h"

extern F_arrow	*make_arrow(int type, int style,	/* arrow.c */
				double thickness, double wid, double ht);

User_color	 user_colors[MAX_USR_COLS];		/* fig2dev.h */
int		 user_col_indx[MAX_USR_COLS];		/* fig2dev.h */
int		 num_usr_cols;				/* fig2dev.h */
int		 num_object;		/* read1_3.c */
					/* flags, psfonts.h, genps.c */
int		 v2_flag;		/* Protocol V2.0 or higher */
int		 v21_flag;		/* Protocol V2.1 or higher */
int		 v30_flag;		/* Protocol V3.0 or higher */
int		 v32_flag;		/* Protocol V3.2 or higher */

static void		 read_colordef(void);
static F_ellipse	*read_ellipseobject(void);
static F_line		*read_lineobject(FILE *fp);
static F_text		*read_textobject(FILE *fp);
static F_spline		*read_splineobject(FILE *fp);
static F_arc		*read_arcobject(FILE *fp);
static F_compound	*read_compoundobject(FILE *fp);
static F_comment	*attach_comments(void);
static void		 count_lines_correctly(FILE *fp);
static void		 init_pats_used(void);
static int		 read_objects(FILE *fp, F_compound *obj);
static int		 get_line(FILE *fp);
static void		 skip_line(FILE *fp);
static int		 backslash_count(char cp[], int start);
static int		 save_comment(void);
static char	Err_incomp[] = "Incomplete %s object at line %d.";
static char	Err_invalid[] = "Invalid %s object at line %d.";
static char	Err_arrow[] = "Invalid %s arrow at line %d.";

#define		FILL_CONVERT(f)	((v2_flag || (f) < WHITE_FILL) \
					? (v30_flag? f: (f-1)) : 20 - ((f)-1)*5)

/* max number of comments that can be stored with each object */
#define		MAXCOMMENTS	100

static int	 gif_colnum = 0;
static char	 buf[BUFSIZ];
static int	 line_no = 0;
static char	*comments[MAXCOMMENTS];	/* comments saved for current object */
static int	 numcom;		/* current comment index */
static bool	 com_alloc = false;	/* whether or not the comment array
					 * has been initialized */

void
read_fail_message(char *file, int err)
{
	if (err == 0)		/* Successful read */
	    return;
#if (_POSIX_C_SOURCE - 0) >= 200112L
	else if (err == ENAMETOOLONG)
	    put_msg("File name \"%s\" is too long", file);
	else if (err == ENOENT)
	    put_msg("File \"%s\" does not exist", file);
	else if (err == ENOTDIR)
	    put_msg("A name in the path \"%s\" is not a directory", file);
	else if (err == EACCES)
	    put_msg("Read access to file \"%s\" is blocked", file);
	else if (err == EISDIR)
	    put_msg("File \"%s\" is a directory", file);
#endif
	else if (err == -2) {
	    put_msg("File \"%s\" is empty", file);
	    }
	else if (err == -1) {
	    /* Format error; relevant error message is already delivered */
	    }
	else
#ifdef	HAVE_STRERROR
	    put_msg("File \"%s\" is not accessible; %s", file, strerror(err));
#else
	    put_msg("File \"%s\" is not accessible.", file);
#endif
}

/**********************************************************
Read_fig returns :

     0 : successful read.
    -1 : File is in incorrect format
    -2 : File is empty
err_no : if file can not be read for various reasons

The resolution (ppi) is stored in global "ppi"
**********************************************************/

int
read_fig(char *file_name, F_compound *obj)
{
	FILE		*fp;

	if ((fp = fopen(file_name, "r")) == NULL)
#ifdef HAVE_STRERROR
	    return errno;
#else
	    return -1;
#endif
	else
	    return readfp_fig(fp, obj);
}

int
readfp_fig(FILE *fp, F_compound *obj)
{
	char	c;
	int	i, status;

	num_object = 0;
	num_usr_cols = 0;
	init_pats_used();

	/* reset comment number */
	numcom = 0;
	/* initialize the comment array */
	if (!com_alloc)
		for (i = 0; i < MAXCOMMENTS; ++i)
			comments[i] = (char *) NULL;
	com_alloc = true;
	memset((char*)obj, '\0', COMOBJ_SIZE);

	/* read first character to see if it is "#" (#FIG 1.4 and newer) */
	c = fgetc(fp);
	if (feof(fp))
		return -2;
	memset((char*)obj, '\0', COMOBJ_SIZE);
	/* put the character back */
	ungetc(c, fp);
	if (c == '#')
		status = read_objects(fp, obj);
	else
		status = read_1_3_objects(fp, obj);
	(void)fclose(fp);
	return status;
}

int
read_objects(FILE *fp, F_compound *obj)
{
	F_ellipse	*e, *le = NULL;
	F_line		*l, *ll = NULL;
	F_text		*t, *lt = NULL;
	F_spline	*s, *ls = NULL;
	F_arc		*a, *la = NULL;
	F_compound	*c, *lc = NULL;
	int		object, coord_sys, len;

	memset((char*)obj, '\0', COMOBJ_SIZE);

	(void) fgets(buf, BUFSIZ, fp);	/* get the version line */
	if (strncmp(buf, "#FIG ", 5)) {
		put_msg("Incorrect format string in first line of input file.");
		return -1;
	}

	/* remove newline and any carriage return (from a PC, perhaps) */
	len = strlen(buf);
	if (buf[len-1] == '\n') {
		if (buf[len-2] == '\r')
			buf[len-2] = '\0';
		else
			buf[len-1] = '\0';
	} else {	/* fgets() only stops at newline and end-of-file */
		put_msg("File is truncated at first line.");
		return -1;
	}

	/* v2_flag is for version 2 or higher */
	v2_flag = (!strncmp(buf, "#FIG 2", 6) || !strncmp(buf, "#FIG 3", 6));
	/* v21_flag is for version 2.1 or higher */
	v21_flag = (!strncmp(buf, "#FIG 2.1", 8) || !strncmp(buf, "#FIG 3", 6));
	/* version 2.2 was only beta - 3.0 is the official release (they are identical) */
	v30_flag = (!strncmp(buf, "#FIG 3", 6) || !strncmp(buf, "#FIG 2.2", 8));
	/* version 3.2 contains paper size, magnif, multiple page and transparent color
	   in Fig file */
	v32_flag = (!strncmp(buf, "#FIG 3.2", 8));
	if (strncmp(&buf[5], PACKAGE_VERSION, 3) > 0) {
	    put_msg("Fig file format (%s) newer than this version of fig2dev (%s), exiting",
			&buf[5], PACKAGE_VERSION);
	    exit(1);
	}

	if (v30_flag) {
	    /* read the orientation spec (landscape/portrait) */
	    line_no=1;
	    if (get_line(fp) < 0) {
		put_msg("File is truncated at landscape/portrait specification.");
		return -1;
	    }
	    /* but set only if the user didn't specify the orientation
	       on the command line */
	    if (!orientspec)
		landscape = !strncasecmp(buf,"land",4);

	    /* now read the metric/inches spec OR centering spec */
	    if (get_line(fp) < 0) {
		put_msg("File is truncated at metric/inches or centering specification.");
		return -1;
	    }
	    /* read justification spec */
	    if ((strncasecmp(buf,"center",6) == 0) ||
		(strncasecmp(buf,"flush",5) == 0)) {
		/* but set only if user didn't specify it */
		if (!centerspec)
		    center = strncasecmp(buf,"flush",5);
		/* now read metric/inches spec */
		if (get_line(fp) < 0) {
		    put_msg("File is truncated at metric/inches specification.");
		    return -1;
		}
	    }
	    /* read metric/inches spec */
	    /* if metric, scale magnification to correct for xfig display error */
	    if (strncasecmp(buf,"metric", 6) == 0) {
		metric = 1;
	    } else {
		metric = 0;
	    }

	    /* new stuff in 3.2 */
	    if (v32_flag) {
		char *p;
		/* read the paper size */
		if (get_line(fp) < 0) {
		    put_msg("File is truncated at paper size specification.");
		    return -1;
		}
		if (!paperspec) {
		    strcpy(papersize,buf);
		    /* and truncate at first blank, if any */
		    if ((p=strchr(papersize,' ')))
			*p = '\0';
		}

		/* read the magnification */
		if (get_line(fp) < 0) {
		    put_msg("File is truncated at magnification specification.");
		    return -1;
		}
		/* if the users hasn't specified a magnification on the command line,
		   use the one in the file */
		if (!magspec) {
		    mag = atof(buf)/100.0;
		    if (mag <= 0.)
			mag = 1.;
		    fontmag = mag;
		}

		/* read the multiple page flag */
		if (get_line(fp) < 0) {
		    put_msg("File is truncated at multiple page specification.");
		    return -1;
		}
		if (!multispec)
		    multi_page = (strncasecmp(buf,"multiple",8) == 0);

		/* Read the GIF transparent color. */
		if (get_line(fp) < 0) {
		    put_msg("File is truncated at transparent color specification.");
		    return -1;
		}
		if (!transspec) {
		    gif_colnum = atoi(buf);
		    if (gif_colnum < -3) {
			put_msg("Invalid color number for transparent color.");
			return -1;
		    }
		    /* if standard color, get the name from the array */
		    /* for user colors, wait till we've read in the file to get the value */
		    if (gif_colnum < NUM_STD_COLS && gif_colnum >= 0)
			strcpy(gif_transparent,Fig_color_names[gif_colnum]);
		}
	    }
	} else {
	/* older than 3.1, no metric/inch flag */
	    metric = 0;
	}

	/* v 3.1 or older, set paper size unless user asked with -z */
	if (!v32_flag) {
	    if (!paperspec) {
		if (metric)
		    strcpy(papersize,"A4");
		else
		    strcpy(papersize,"Letter");
	    }
	}

	/* now read for resolution and coord_sys (coord_sys is not used) */
	if (get_line(fp) < 0) {
	    put_msg("File is truncated at resolution specification.");
	    return -1;
	}
	if (sscanf(buf,"%lf%d\n", &ppi, &coord_sys) != 2) {
	    put_msg("Incomplete resolution information at line %d.", line_no);
	    return -1;
	}
	if (ppi <= 0.) {
	    put_msg("Invalid resolution information (%g) at line %d.",
		    ppi, line_no);
	    return -1;
	}

	/* convert line thickness from screen resolution */
	THICK_SCALE = ppi/80;

	/* attach any comments found thus far to the whole figure */
	obj->comments = attach_comments();

	while (get_line(fp) > 0) {
	    if (sscanf(buf, "%d", &object) != 1) {
		put_msg("Incorrect format at line %d.", line_no);
		return -1;
	    }
	    switch (object) {
		case OBJ_COLOR_DEF:
		    read_colordef();
		    if (num_object) {
			put_msg("Color definitions must come before other objects (line %d).",
				line_no);
			return (-1);
		    }
		    ++num_usr_cols;
		    break;
		case OBJ_POLYLINE :
		    if ((l = read_lineobject(fp)) == NULL)
			return -1;
#ifdef V4_0
		    if ((l->pic != NULL) && (l->pic->figure != NULL)) {
			if (lc)
			    lc = (lc->next = l->pic->figure);
			else
			    lc = obj->compounds = l->pic->figure;
			free_linestorage(l);
			num_object++;
		    } else {
			if (ll)
			    ll = (ll->next = l);
			else
			    ll = obj->lines = l;
			num_object++;
		    }
		    break;
#else /* V4_0 */
		    if (ll)
			ll = (ll->next = l);
		    else
			ll = obj->lines = l;
		    num_object++;
		    break;
#endif /* V4_0 */
		case OBJ_SPLINE :
		    if ((s = read_splineobject(fp)) == NULL) {
			return -1;
			}
		    if (v32_flag){ /* s is a line */
		      if (ll)
			ll = (ll->next = (F_line *) s);
		      else
			ll = obj->lines = (F_line *) s;
		      num_object++;
		      break;
		    }
		    if (ls)
			ls = (ls->next = s);
		    else
			ls = obj->splines = s;
		    num_object++;
		    break;
		case OBJ_ELLIPSE :
		    if ((e = read_ellipseobject()) == NULL)
			return -1;
		    if (le)
			le = (le->next = e);
		    else
			le = obj->ellipses = e;
		    num_object++;
		    break;
		case OBJ_ARC :
		    if ((a = read_arcobject(fp)) == NULL)
			return -1;
		    if (la)
			la = (la->next = a);
		    else
			la = obj->arcs = a;
		    num_object++;
		    break;
		case OBJ_TEXT :
		    if ((t = read_textobject(fp)) == NULL)
			return -1;
		    if (lt)
			lt = (lt->next = t);
		    else
			lt = obj->texts = t;
		    num_object++;
		    break;
		case OBJ_COMPOUND :
		    if ((c = read_compoundobject(fp)) == NULL)
			return -1;
		    if (lc)
			lc = (lc->next = c);
		    else
			lc = obj->compounds = c;
		    num_object++;
		    break;
		default :
		    put_msg("Incorrect object code at line %d.", line_no);
		    return -1;
		} /*  switch */
	} /*  while (get_line(fp)) */

	/* if user color was requested for GIF transparent color, get the
	   rgb values from the user color array now that we've read them in */
	if (gif_colnum >= NUM_STD_COLS) {
	    int i;
	    for (i=0; i<num_usr_cols; ++i)
		if (user_col_indx[i] == gif_colnum)
		    break;
	    if (i < num_usr_cols)
		sprintf(gif_transparent,"#%2x%2x%2x",
				user_colors[i].r,user_colors[i].g,user_colors[i].b);
	}

	if (feof(fp))
	    return 0;
	else
#ifdef HAVE_STRERROR
	    return errno;
#else
	    return -1;
#endif
} /*  read_objects */

static void
read_colordef(void)
{
    int			c;
    unsigned int	r,g,b;

    if ((sscanf(buf, "%*d %d #%2x%2x%2x", &c, &r, &g, &b) != 4) ||
		(c < NUM_STD_COLS)) {
	buf[strlen(buf)-1]='\0';	/* remove the newline */
	put_msg("Invalid color definition: %s, setting to black (#00000).",buf);
	r=g=b=0;
    }
    user_col_indx[num_usr_cols] = c;
    user_colors[num_usr_cols].r = r;
    user_colors[num_usr_cols].g = g;
    user_colors[num_usr_cols].b = b;
}

static void
fix_and_note_color(int *color)
{
    int		    i;
    if (*color < DEFAULT) {
	put_msg("Invalid color number %d at line %d, using default color.",
			*color, line_no);
	*color = DEFAULT;
	return;
    }
    if (*color < NUM_STD_COLS) {
	if (*color >= BLACK_COLOR) {
	    std_color_used[*color] = true;
	}
	return;
    }
    for (i=0; i<num_usr_cols; ++i)
	if (*color == user_col_indx[i]) {
		*color = i + NUM_STD_COLS;
		return;
	}
    put_msg("Cannot locate user color %d, using default color at line %d.",
		*color, line_no);
    *color = DEFAULT;
    return;
}

static void
note_fill(int fill, int *color)
{
	if (fill != UNFILLED) {
		fix_and_note_color(color);
		if (fill >= NUMSHADES + NUMTINTS) {
			pattern_used[fill - NUMSHADES - NUMTINTS] = true;
			pats_used = true;
		}
	}
}

static F_arc *
read_arcobject(FILE *fp)
{
	F_arc	*a;
	int	n, fa, ba;
	int	type, style;
	double	thickness, wid, ht;

	if (NULL == (Arc_malloc(a))) {
	    put_msg(Err_mem);
	    return NULL;
	    }
	a->pen = 0;
	a->fill_style = 0;
	a->for_arrow = NULL;
	a->back_arrow = NULL;
	a->next = NULL;
	if (v30_flag) {
	    n = sscanf(buf, "%*d%d%d%d%d%d%d%d%d%lf%d%d%d%d%lf%lf%d%d%d%d%d%d\n",
		&a->type, &a->style, &a->thickness,
		&a->pen_color, &a->fill_color, &a->depth, &a->pen, &a->fill_style,
		&a->style_val, &a->cap_style,
		&a->direction, &fa, &ba,
		&a->center.x, &a->center.y,
		&a->point[0].x, &a->point[0].y,
		&a->point[1].x, &a->point[1].y,
		&a->point[2].x, &a->point[2].y);
	} else {
	    n = sscanf(buf, "%*d%d%d%d%d%d%d%d%lf%d%d%d%lf%lf%d%d%d%d%d%d\n",
		&a->type, &a->style, &a->thickness,
		&a->pen_color, &a->depth, &a->pen, &a->fill_style,
		&a->style_val, &a->direction, &fa, &ba,
		&a->center.x, &a->center.y,
		&a->point[0].x, &a->point[0].y,
		&a->point[1].x, &a->point[1].y,
		&a->point[2].x, &a->point[2].y);
	    a->fill_color = a->pen_color;
	    a->cap_style = 0;        /* butt line cap */
	}
	if ((v30_flag && n != 21) || (!v30_flag && n != 19)) {
	    put_msg(Err_incomp, "arc", line_no);
	    free(a);
	    return NULL;
	}
	a->thickness *= round(THICK_SCALE);
	a->fill_style = FILL_CONVERT(a->fill_style);
	if (INVALID_ARC(a)) {
		put_msg(Err_invalid, "arc", line_no);
		free(a);
		return NULL;
	}
	fix_and_note_color(&a->pen_color);
	note_fill(a->fill_style, &a->fill_color);
	if (fa) {
	    if (get_line(fp) < 0 ||
	        sscanf(buf, "%d%d%lf%lf%lf",
				&type, &style, &thickness, &wid, &ht) != 5) {
		    put_msg(Err_incomp, "arc", line_no);
		    free(a);
		    return NULL;
	    }
	    if ((a->for_arrow = make_arrow(type, style, thickness, wid, ht))
			    == NULL) {
		    put_msg(Err_arrow, "forward", line_no);
		    free(a);
		    return NULL;
	    }
	}
	if (ba) {
	    if (get_line(fp) < 0 ||
	        sscanf(buf, "%d%d%lf%lf%lf",
				&type, &style, &thickness, &wid, &ht) != 5) {
		    put_msg(Err_incomp, "arc", line_no);
		    free(a);
		    return NULL;
	    }
	    if ((a->back_arrow = make_arrow(type, style, thickness, wid, ht))
			    == NULL) {
		    put_msg(Err_arrow, "backward", line_no);
		    free(a);
		    return NULL;
	    }
	}
	a->comments = attach_comments();		/* attach any comments */
	return a;
}

static F_compound *
read_compoundobject(FILE *fp)
{
	F_arc		*a, *la = NULL;
	F_ellipse	*e, *le = NULL;
	F_line		*l, *ll = NULL;
	F_spline	*s, *ls = NULL;
	F_text		*t, *lt = NULL;
	F_compound	*com, *c, *lc = NULL;
	int		n, object;

	Compound_malloc(com);
	com->arcs = NULL;
	com->ellipses = NULL;
	com->lines = NULL;
	com->splines = NULL;
	com->texts = NULL;
	com->compounds = NULL;
	com->next = NULL;
	com->comments = attach_comments();	/* attach any comments */

	n = sscanf(buf, "%*d%d%d%d%d\n", &com->nwcorner.x, &com->nwcorner.y,
		&com->secorner.x, &com->secorner.y);
	if (n != 4) {
	    put_msg(Err_incomp, "compound", line_no);
	    free(com);
	    return NULL;
	    }
	while (get_line(fp) > 0) {
	    if (sscanf(buf, "%d", &object) != 1) {
		put_msg(Err_incomp, "compound", line_no);
		free_compound(&com);
		return NULL;
		}
	    switch (object) {
		case OBJ_POLYLINE :
		    if ((l = read_lineobject(fp)) == NULL) {
			return NULL;
			}
#ifdef V4_0
		    if (l->pic->figure != NULL) {
			if (lc)
			    lc = (lc->next = l->pic->figure);
			else
			    lc = com->compounds = l->pic->figure;
			free_linestorage(l);
		    } else {
#endif /* V4_0 */
			if (ll)
			    ll = (ll->next = l);
			else
			    ll = com->lines = l;
#ifdef V4_0
		    }
#endif /* V4_0 */
		    break;
		case OBJ_SPLINE :
		    if ((s = read_splineobject(fp)) == NULL) {
			return NULL;
			}
		    if (v32_flag){ /* s is a line */
		      if (ll)
			ll = (ll->next = (F_line *) s);
		      else
			ll = com->lines = (F_line *) s;
		      break;
		    }
		    if (ls)
			ls = (ls->next = s);
		    else
			ls = com->splines = s;
		    break;
		case OBJ_ELLIPSE :
		    if ((e = read_ellipseobject()) == NULL) {
			return NULL;
			}
		    if (le)
			le = (le->next = e);
		    else
			le = com->ellipses = e;
		    break;
		case OBJ_ARC :
		    if ((a = read_arcobject(fp)) == NULL) {
			return NULL;
			}
		    if (la)
			la = (la->next = a);
		    else
			la = com->arcs = a;
		    break;
		case OBJ_TEXT :
		    if ((t = read_textobject(fp)) == NULL) {
			return NULL;
			}
		    if (lt)
			lt = (lt->next = t);
		    else
			lt = com->texts = t;
		    break;
		case OBJ_COMPOUND :
		    if ((c = read_compoundobject(fp)) == NULL) {
			return NULL;
			}
		    if (lc)
			lc = (lc->next = c);
		    else
			lc = com->compounds = c;
		    break;
		case OBJ_END_COMPOUND :
		    return com;
		default :
		    put_msg("Wrong object code at line %d", line_no);
		    return NULL;
		} /*  switch */
	    }
	if (feof(fp))
	    return com;
	else
	    return NULL;
	}

static F_ellipse *
read_ellipseobject(void)
{
	F_ellipse	*e;
	int		n;

	Ellipse_malloc(e);
	e->fill_style = 0;
	e->pen = 0;
	e->next = NULL;
	if (v30_flag) {
	    n = sscanf(buf, "%*d%d%d%d%d%d%d%d%d%lf%d%lf%d%d%d%d%d%d%d%d\n",
		&e->type, &e->style, &e->thickness,
		&e->pen_color, &e->fill_color, &e->depth, &e->pen, &e->fill_style,
		&e->style_val, &e->direction, &e->angle,
		&e->center.x, &e->center.y,
		&e->radiuses.x, &e->radiuses.y,
		&e->start.x, &e->start.y,
		&e->end.x, &e->end.y);
	} else {
	    n = sscanf(buf, "%*d%d%d%d%d%d%d%d%lf%d%lf%d%d%d%d%d%d%d%d\n",
		&e->type, &e->style, &e->thickness,
		&e->pen_color, &e->depth, &e->pen, &e->fill_style,
		&e->style_val, &e->direction, &e->angle,
		&e->center.x, &e->center.y,
		&e->radiuses.x, &e->radiuses.y,
		&e->start.x, &e->start.y,
		&e->end.x, &e->end.y);
	    e->fill_color = e->pen_color;
	}
	if ((v30_flag && n != 19) || (!v30_flag && n != 18)) {
	    put_msg(Err_incomp, "ellipse", line_no);
	    free(e);
	    return NULL;
	    }
	fix_and_note_color(&e->pen_color);
	e->thickness *= round(THICK_SCALE);
	e->fill_style = FILL_CONVERT(e->fill_style);
	if (e->radiuses.x < 0)
		e->radiuses.x = -e->radiuses.x;
	if (e->radiuses.y < 0)
		e->radiuses.y = -e->radiuses.y;
	if (INVALID_ELLIPSE(e)) {
		put_msg(Err_invalid, "ellipse", line_no);
		free(e);
		return NULL;
	}
	note_fill(e->fill_style, &e->fill_color);
	e->comments = attach_comments();	/* attach any comments */
	return e;
}

/*
 * Sanitize line objects. Return 0 on success, -1 otherwise.
 * On error, call free_linestorage(l) after sanitize_lineobject().
 * polylines: remove fill, if less than 3 points
 *		remove arrows, if only one point
 * rectangles, polygons: last point must coincide with first point
 * rectangle: convert to polygon, if not 5 points
 * rectangle with rounded corners: error, if not 5 points
 * boxes: allow only vertical and horizontal edges, convert to polygon otherwise
 */
static int
sanitize_lineobject(
	F_line *l,	/* the line */
	F_point *p	/* the last point of the line */
	)
{
	F_point	*q;
		/* CHANGE if object definitions change! Also, see below. */
	const char    *obj_name[] = {
				"rectangle",
				"polygon",
				"rectangle with rounded corners",
				"picture box"
	};

	if (l->type == T_POLYLINE &&
		(l->points->next == NULL || l->points->next->next == NULL)) {
	    l->fill_style = UNFILLED;
	    if (l->points->next == NULL) {
		if (l->for_arrow) {
		    /* tests/testsuite -k polyline,read.c */
		    put_msg(
			"A single point with a forward arrow - remove the arrow.");
		    free(l->for_arrow);
		    l->for_arrow = NULL;
		}
		if (l->back_arrow) {
		    put_msg(
			"A single point with a backward arrow - remove the arrow.");
		    free(l->back_arrow);
		    l->back_arrow = NULL;
		}
	    }
	    return 0;
	}

	if ((l->type == T_BOX || l->type == T_POLYGON ||
			l->type == T_ARC_BOX || l->type == T_PIC_BOX) &&
		(l->points->x != p->x || l->points->y != p->y)) {
	    /* tests/testsuite -k open,read.c */
	    put_msg("An open %s at line %d - close it.",
		    /* CHANGE THIS if object definitions change! */
		    obj_name[l->type-2],
		    line_no);
	    if (NULL == (Point_malloc(q))) {
		put_msg(Err_mem);
		return -1;
	    }
	    p->next = q;
	    q->next = NULL;
	    q->x = l->points->x;
	    q->y = l->points->y;
	}

	if (l->type == T_BOX || l->type == T_ARC_BOX || l->type == T_PIC_BOX) {
	    int	npts = 1;
	    for (q = l->points; q->next; q = q->next)
		++npts;
	    if (l->type == T_BOX && npts != 5) {
		/* tests/testsuite -k polygon,read.c */
		put_msg("A rectangle with %d corners at line %d - convert to a polygon.",
			npts - 1, line_no);
		l->type = T_POLYGON;
		return 0;
	    }
	    if (npts != 5) {	/* && (T_ARC_BOX || PIC_BOX) */
		/* tests/testsuite -k malformed,read.c */
		put_msg("A malformed %s at line %d - it has %d corners.",
			obj_name[l->type-2], line_no, npts - 1);
		return -1;
	    } else {	/* npts == 5 && (T_BOX || T_ARC_BOX || PIC_BOX) */
		q = l->points->next->next;
		if ((l->points->x != l->points->next->x ||
			q->x != q->next->x || l->points->y != q->next->y ||
			l->points->next->y != q->y) &&
			(l->points->y != l->points->next->y ||
			q->y != q->next->y || l->points->x != q->next->x ||
			l->points->next->x != q->x)) {
		    /* tests/testsuite -k distorted,read.c */
		    if (l->type == T_BOX) {
			put_msg("A distorted %s at line %d - convert to a polygon.",
			    obj_name[l->type-2], line_no);
			l->type = T_POLYGON;
			return 0;
		    }
		    put_msg("A distorted or inclined %s at line %d.",
			    obj_name[l->type-2], line_no);
		    return -1;
		}
	    }
	}

	return 0;
}

static F_line *
read_lineobject(FILE *fp)
{
	F_line	*l;
	F_point	*o = NULL, *p, *q;
	int	n, x, y, fa, ba;
	int	type, style, radius_flag;
	double	thickness, wid, ht;
	int	npts;

	Line_malloc(l);
	l->points = NULL;
	l->pen = 0;
	l->fill_style = 0;
	l->for_arrow = NULL;
	l->back_arrow = NULL;
	l->next = NULL;
	l->join_style = 0;
	l->cap_style = 0;        /* butt line cap */
	l->pic = NULL;
	l->comments = NULL;

	sscanf(buf,"%*d%d",&l->type);	/* get the line type */

	radius_flag = v30_flag || v21_flag || (v2_flag && l->type == T_ARC_BOX);
	if (radius_flag) {
	    if (v30_flag) {
		n = sscanf(buf, "%*d%d%d%d%d%d%d%d%d%lf%d%d%d%d%d%d",
		&l->type,&l->style,&l->thickness,&l->pen_color,&l->fill_color,
		&l->depth,&l->pen,&l->fill_style,&l->style_val,
		&l->join_style,&l->cap_style,
		&l->radius,&fa,&ba,&npts);
	    } else {
		n = sscanf(buf, "%*d%d%d%d%d%d%d%d%lf%d%d%d",
		&l->type,&l->style,&l->thickness,&l->pen_color,
		&l->depth,&l->pen,&l->fill_style,&l->style_val,&l->radius,&fa, &ba);
		l->fill_color = l->pen_color;
	    }
	}
	/* old format uses pen for radius of arc-box corners */
	else {
	    n = sscanf(buf, "%*d%d%d%d%d%d%d%d%lf%d%d",
			&l->type,&l->style,&l->thickness,&l->pen_color,
			&l->depth,&l->pen,&l->fill_style,&l->style_val,&fa,&ba);
	    l->fill_color = l->pen_color;
	    if (l->type == T_ARC_BOX)
		{
		l->radius = (int) l->pen;
		l->pen = 0;
		}
	    else
		l->radius = 0;
	}
	if ((!radius_flag && n!=10) ||
	     (radius_flag && ((!v30_flag && n!=11)||(v30_flag && n!=15)))) {
	    put_msg(Err_incomp, "line", line_no);
	    free(l);
	    return NULL;
	}
	l->radius *= round(THICK_SCALE);
	l->thickness *= round(THICK_SCALE);
	l->fill_style = FILL_CONVERT(l->fill_style);
	if (INVALID_LINE(l)) {
		put_msg(Err_invalid, "line", line_no);
		free(l);
		return NULL;
	}
	note_fill(l->fill_style, &l->fill_color);
	fix_and_note_color(&l->pen_color);
	if (fa) {
	    if (get_line(fp) < 0 ||
			sscanf(buf, "%d%d%lf%lf%lf",
				&type, &style, &thickness, &wid, &ht) != 5) {
		put_msg(Err_incomp, "line", line_no);
		free(l);
		return NULL;
	    }
	    if ((l->for_arrow = make_arrow(type, style, thickness, wid, ht))
			    == NULL) {
		    put_msg(Err_arrow, "forward", line_no);
		    free(l);
		    return NULL;
	    }
	}
	if (ba) {
	    if (get_line(fp) < 0 ||
			sscanf(buf, "%d%d%lf%lf%lf",
				&type, &style, &thickness, &wid, &ht) != 5) {
		put_msg(Err_incomp, "line", line_no);
		free_linestorage(l);
		return NULL;
	    }
	    if ((l->back_arrow = make_arrow(type, style, thickness, wid, ht))
			    == NULL) {
		    put_msg(Err_arrow, "backward", line_no);
		    free_linestorage(l);
		    return NULL;
	    }
	}
	if (l->type == T_PIC_BOX) {
	    char	file[BUFSIZ], *c;
	    size_t	len;

	    if ((Pic_malloc(l->pic)) == NULL) {
		free(l);
		return NULL;
	    }
	    l->pic->transp = -1;
	    l->pic->bitmap = NULL;
#ifdef HAVE_X11_XPM_H
	    /* initialize l->pic->xpmimage by (ab)using a
	       public libxpm-function */
	    XpmCreateXpmImageFromBuffer("", &l->pic->xpmimage, NULL);
#endif

			/* %[^\n]: really, read until first '\0' in buf */
	    if (get_line(fp) < 0 || sscanf(buf, "%d %[^\n]",
					&l->pic->flipped, file) != 2) {
	        put_msg(Err_incomp, "picture", line_no);
		free(l);
	        return NULL;
	    }
	    /* if there is a path in the .fig filename, and the path of the
	     * imported picture filename is NOT absolute, prepend the
	     * .fig file path to it
	     */
	    if (from && (c = strrchr(from, '/')) && file[0] != '/') {
		if ((l->pic->file = malloc((size_t)(c - from + 2) +
				    (len = strlen(file)))) ==
			NULL) {
		    put_msg(Err_mem);
		    free(l);	/* Points not read yet. */
		    return NULL;
		}
		strncpy(l->pic->file, from, (size_t)(c - from + 1));
		memcpy(l->pic->file + (c - from + 1), file, len + 1);
	    } else {
		/* either absolute picture path or no path in .fig filename */
		l->pic->file = malloc(len = strlen(file) + 1);
		memcpy(l->pic->file, file, len);
	    }
	}

	if (NULL == (l->points = Point_malloc(p))) {
	    put_msg(Err_mem);
	    free_linestorage(l);
	    return NULL;
	}
	p->next = NULL;

	/* read first point of line */
	++line_no;
	if (fscanf(fp, "%d%d", &p->x, &p->y) != 2) {
	    put_msg(Err_incomp, "line", line_no);
	    free_linestorage(l);
	    return NULL;
	}

	if (!v30_flag)
	    npts = 1000000;
	for (--npts; npts > 0; --npts) {
	    count_lines_correctly(fp);
	    if (fscanf(fp, "%d%d", &x, &y) != 2) {
		put_msg(Err_incomp, "line", line_no);
		free_linestorage(l);
		return NULL;
	    }
	    if (!v30_flag && x == 9999)
	       break;
	    /* tests/testsuite -k coincident,read.c */
	    if (x == p->x && y == p->y) /* skip co-incident points */
		continue;
	    if (NULL == (Point_malloc(q))) {
		put_msg(Err_mem);
		free_linestorage(l);
		return NULL;
	    }
	    q->x = x;
	    q->y = y;
	    q->next = NULL;
	    p->next = q;
	    o = p;
	    p = q;
	}

	l->last[0].x = p->x;
	l->last[0].y = p->y;
	if (o) {
	    l->last[1].x = o->x;
	    l->last[1].y = o->y;
	}

	if (sanitize_lineobject(l, p)) {
	    free_linestorage(l);
	    return NULL;
	}

	l->comments = attach_comments();	/* attach any comments */
	/* skip to the end of the line */
	skip_line(fp);
	return l;
}

static F_spline *
read_splineobject(FILE *fp)
{
	F_spline	*s;
	F_line          *l;
	F_point		*p, *q;
	F_control	*cp, *cq;
	int		c, n, x, y, fa, ba;
	int		type, style, npts;
	double		thickness, wid, ht;
	double		lx, ly, rx, ry;

	Spline_malloc(s);
	s->points = NULL;
	s->controls = NULL;
	s->pen = 0;
	s->fill_style = 0;
	s->for_arrow = NULL;
	s->back_arrow = NULL;
	s->comments = NULL;
	s->next = NULL;

	if (v30_flag) {
	    n = sscanf(buf, "%*d%d%d%d%d%d%d%d%d%lf%d%d%d%d",
		&s->type, &s->style, &s->thickness,
		&s->pen_color, &s->fill_color,
		&s->depth, &s->pen, &s->fill_style, &s->style_val,
		&s->cap_style, &fa, &ba, &npts);
	} else {
	    n = sscanf(buf, "%*d%d%d%d%d%d%d%d%lf%d%d",
		&s->type, &s->style, &s->thickness, &s->pen_color,
		&s->depth, &s->pen, &s->fill_style, &s->style_val, &fa, &ba);
	    s->fill_color = s->pen_color;
	    s->cap_style = 0;        /* butt line cap */
	}
	if ((v30_flag && n != 13) || (!v30_flag && n != 10)) {
	    put_msg(Err_incomp, "spline", line_no);
	    free(s);
	    return NULL;
	    }
	s->thickness *= round(THICK_SCALE);
	s->fill_style = FILL_CONVERT(s->fill_style);
	if (INVALID_SPLINE(s)) {
		put_msg(Err_invalid, "spline", line_no);
		free(s);
		return NULL;
	}
	note_fill(s->fill_style, &s->fill_color);
	fix_and_note_color(&s->pen_color);
	if (fa) {
	    if (get_line(fp) < 0 ||
			    sscanf(buf, "%d%d%lf%lf%lf",
				    &type, &style, &thickness, &wid, &ht) != 5) {
		    put_msg(Err_incomp, "spline", line_no);
		    free(s);
		    return NULL;
	    }
	    if ((s->for_arrow = make_arrow(type, style, thickness, wid, ht))
			    == NULL) {
		    put_msg(Err_arrow, "forward", line_no);
		    free(s);
		    return NULL;
	    }
	}
	if (ba) {
	    if (get_line(fp) < 0 ||
			    sscanf(buf, "%d%d%lf%lf%lf",
				    &type, &style, &thickness, &wid, &ht) != 5) {
		    put_msg(Err_incomp, "spline", line_no);
		    free_splinestorage(s);
		    return NULL;
	    }
	    if ((s->back_arrow = make_arrow(type, style, thickness, wid, ht))
			    == NULL) {
		    put_msg(Err_arrow, "backward", line_no);
		    free_splinestorage(s);
		    return NULL;
	    }
	}

	/* Read points */
	/* read first point of line */
	++line_no;
	if ((n = fscanf(fp, "%d%d", &x, &y)) != 2) {
	    put_msg(Err_incomp, "spline", line_no);
	    free_splinestorage(s);
	    return NULL;
	    };
	if (NULL == (s->points = Point_malloc(p))) {
	    put_msg(Err_mem);
	    free_splinestorage(s);
	    return NULL;
	    }
	p->x = x; p->y = y; p->next = NULL;
	c = 1;
	if (!v30_flag)
		npts = 1000000;
	if (npts < 2) {
		put_msg(Err_incomp, "spline", line_no);
		free_splinestorage(s);
		return NULL;
	}
	for (--npts; npts > 0; --npts) {
	    /* keep track of newlines for line counter */
	    count_lines_correctly(fp);
	    if (fscanf(fp, "%d%d", &x, &y) != 2) {
		put_msg(Err_incomp, "spline", line_no);
		free_splinestorage(s);
		return NULL;
		};
	    if (!v30_flag && x == 9999)
		break;
	    if (NULL == (Point_malloc(q))) {
		put_msg(Err_mem);
		free_splinestorage(s);
		return NULL;
		}
	    q->x = x;
	    q->y = y;
	    p->next = q;
	    p = q;
	    ++c;
	    }
	p->next = NULL;
	s->comments = attach_comments();	/* attach any comments */

	if (v32_flag) {
	    /* transform x-splines into lines */

	    F_control * ptr;
	    double control_s;

	    make_control_factors(s);
	    ptr = s->controls;
	    while (ptr) {    /* read controls */
		/* keep track of newlines for line counter */
		count_lines_correctly(fp);
		if ((n = fscanf(fp, "%lf", &control_s)) != 1) {
		  put_msg(Err_incomp, "spline", line_no);
		  free_splinestorage(s);
		  return NULL;
		}
		ptr->s = control_s;
		ptr = ptr->next;
	      }

	    l = create_line_with_spline(s);
	    free_splinestorage(s);
	    /* skip to end of line */
	    skip_line(fp);
	    if (l == NULL)
		return NULL;
	    return (F_spline *)l;   /* return the new line */
	  }

	if (approx_spline(s)) {
	    skip_line(fp);
	    return s;
	}
	/* Read controls from older versions */
	/* keep track of newlines for line counter */
	count_lines_correctly(fp);
	if ((n = fscanf(fp, "%lf%lf%lf%lf", &lx, &ly, &rx, &ry)) != 4) {
	    put_msg(Err_incomp, "spline", line_no);
	    free_splinestorage(s);
	    return NULL;
	}
	if (NULL == (s->controls = Control_malloc(cp))) {
	    put_msg(Err_mem);
	    free_splinestorage(s);
	    return NULL;
	}
	cp->lx = lx; cp->ly = ly;
	cp->rx = rx; cp->ry = ry;
	while (--c) {
	    /* keep track of newlines for line counter */
	    count_lines_correctly(fp);
	    if (fscanf(fp, "%lf%lf%lf%lf", &lx, &ly, &rx, &ry) != 4) {
		put_msg(Err_incomp, "spline", line_no);
		cp->next = NULL;
		free_splinestorage(s);
		return NULL;
	    }
	    if (NULL == (Control_malloc(cq))) {
		put_msg(Err_mem);
		cp->next = NULL;
		free_splinestorage(s);
		return NULL;
	    }
	    cq->lx = lx; cq->ly = ly;
	    cq->rx = rx; cq->ry = ry;
	    cp->next = cq;
	    cp = cq;
	}
	cp->next = NULL;

	/* skip to the end of the line */
	skip_line(fp);
	return s;
}

static F_text *
read_textobject(FILE *fp)
{
	F_text	*t;
	int	n, ignore = 0;
	char	s[BUFSIZ], s_temp[BUFSIZ], junk[2];
	int	more, len, l;

	Text_malloc(t);
	t->font = 0;
	t->size = 0.0;
	t->comments = NULL;
	t->next = NULL;

	if (v30_flag) {	/* order of parms is more like other objects now,
			   string is now terminated with the literal '\001',
			   and 8-bit characters are represented as \xxx */

	    n = sscanf(buf, "%*d%d%d%d%d%d%lf%lf%d%lf%lf%d%d%[^\n]",
		&t->type, &t->color, &t->depth, &t->pen,
		&t->font, &t->size, &t->angle,
		&t->flags, &t->height, &t->length,
		&t->base_x, &t->base_y, s);
	} else {
	    /* The text object is terminated by a CONTROL-A, so we read
		everything up to the CONTROL-A and then read that character.
		If we do not find the CONTROL-A on this line then this must
		be a multi-line text object and we will have to read more. */

	    n = sscanf(buf,"%*d%d%d%lf%d%d%d%lf%d%lf%lf%d%d%[^\1]%[\1]",
		&t->type, &t->font, &t->size, &t->pen,
		&t->color, &t->depth, &t->angle,
		&t->flags, &t->height, &t->length,
		&t->base_x, &t->base_y, s, junk);
	}
	if ((n != 14) && (n != 13)) {
	  put_msg(Err_incomp, "text", line_no);
	  free(t);
	  return NULL;
	}

	if (font_size != 0.0) {
	    /* scale length/height of text by ratio of requested font size to actual size */
	    t->length = t->length * font_size/t->size;
	    t->height = t->height * font_size/t->size;
	    t->size = font_size;	/* and set to requested size */
	}
	if (t->size <= 0.0)
	    t->size = (float) DEFAULT_FONT_SIZE;
	more = 0;
	if (!v30_flag && n == 13)
	    more = 1;  /* in older xfig there is more if ^A wasn't found yet */
	else if (v30_flag) {	/* in 3.0 there is more if \001 wasn't found */
	    len = strlen(s);
	    if ((strcmp(&s[len-4],"\\001") == 0) &&	/* if we find '\000' */
	        !(backslash_count(s, len-5) % 2)) {	/* and not '\\000' */
		    more = 0;				/* then there are no more lines */
		    s[len-4]='\0';			/* and get rid of the '\001' */
	    } else {
		more = 1;
		s[len++]='\n';				/* put back the end of line char */
		s[len] = '\0';				/* and terminate it */
	    }
	}
	if (more) {
	  /* Read in the subsequent lines of the text if there are more */
	  do {
	    ++line_no;				/* As is done in get_line */
	    if (fgets(s_temp, BUFSIZ, fp) == NULL)
		break;
	    len = strlen(s_temp)-1;		/* ignore newline */
	    if (len > 0 && s_temp[len-1] == '\r') { /* strip any trailing CR */
		s_temp[len-1] = '\0';
		len--;
	    }
	    if (v30_flag) {
		if ((strncmp(&s_temp[len-4],"\\001",4) == 0) &&
		    !(backslash_count(s_temp, len-5) % 2)) {
			n=0;			/* found the '\001', set n to stop */
			s_temp[len-4]='\0';	/* and get rid of the '\001' */
		} else {
			n=1;			/* keep going (more lines) */
		}
	    } else {
		n = sscanf(buf, "%[^\1]%[\1]", s_temp, junk);
	    }
	    /* Safety check */
	    if (strlen(s)+1 + strlen(s_temp)+1 > BUFSIZ) {
	      /* Too many characters.  Ignore the rest. */
	      ignore = 1;
	    }
	    if (!ignore)
	      strcat(s, s_temp);
	  } while (n == 1);
	}

	if (v30_flag) {		/* now convert any \xxx to ascii characters */
	    if (strchr(s,'\\')) {
		unsigned int num;
		len = strlen(s);
		for (l=0,n=0; l < len; ++l) {
		    if (s[l]=='\\') {
			/* a backslash, see if a digit follows */
			if (l < len && isdigit(s[l+1])) {
			    /* yes, scan for 3 digit octal value */
			    if (sscanf(&s[l+1],"%3o",&num)!=1) {
				put_msg("Error in parsing text string on line %d",
						line_no);
				return NULL;
			    }
			    buf[n++]= (unsigned char) num;	/* put char in */
			    l += 3;			/* skip over digits */
			} else {
			    buf[n++] = s[++l];		/* some other escaped character */
			}
		    } else {
			buf[n++] = s[l];		/* ordinary character */
		    }
		}
		buf[n]='\0';		/* terminate */
		strcpy(s,buf);		/* copy back to s */
	    }
	}
	if (strlen(s) == 0)
		(void)strcpy(s, " ");
	t->cstring = calloc((unsigned)(strlen(s)), sizeof(char));
	if (NULL == t->cstring) {
	    put_msg(Err_mem);
	    free(t);
	    return NULL;
	}
	(void)strcpy(t->cstring, s+1);

	if (!v21_flag && (t->font == 0 || t->font == DEFAULT))
		t->flags = ((t->flags != DEFAULT) ? t->flags : 0)
				| SPECIAL_TEXT;

	if (v2_flag && !v21_flag && !special_text(t))
		t->flags = ((t->flags != DEFAULT) ? t->flags : 0)
				| PSFONT_TEXT;

	if (INVALID_TEXT(t)) {
		put_msg(Err_invalid, "text", line_no);
		free_text(&t);
		return NULL;
	}
	fix_and_note_color(&t->color);
	t->comments = attach_comments();	/* attach any comments */
	return t;
}


/* count consecutive backslashes backwards */

static int
backslash_count(char cp[], int start)
{
  int i, count = 0;

  for(i=start; i>=0; i--) {
    if (cp[i] == '\\')
	count++;
    else
	break;
  }
  return count;
}

/* attach comments in linked list */

static F_comment *
attach_comments(void)
{
    int		    i;
    F_comment	   *comp, *icomp;

    if (numcom == 0)
	return NULL;

    icomp = comp = (F_comment *) malloc(COMMENT_SIZE);
    for (i=0; i<numcom; i++) {
	comp->comment = malloc(strlen(comments[i])+1);
	strcpy(comp->comment, comments[i]);
	if (i<numcom-1) {
	    comp->next = (F_comment *) malloc(COMMENT_SIZE);
	    comp = comp->next;
	} else
	    comp->next = NULL;
    }
    /* reset comment number */
    numcom = 0;
    return icomp;
}

static int
get_line(FILE *fp)
{
	int	len;
	while (1) {
		if (NULL == fgets(buf, BUFSIZ, fp)) {
			return -1;
		}
		++line_no;
		if (*buf == '#') {			/* save any comments */
			if (save_comment() < 0)
				return -1;
			/* skip empty lines */
		} else if (*buf != '\n' || !(*buf == '\r' && buf[1] == '\n')) {
			len = strlen(buf);
			/* remove newline and possibly a carriage return */
			if (buf[len-1] == '\n')
				buf[len - (buf[len-2] == '\r' ? 2 : 1)] = '\0';
			return 1;
		}
	}
}

/* save a comment line to be stored with the *subsequent* object */

static int
save_comment(void)
{
    int		    i;

    /* skip too many comment lines */
    if (numcom == MAXCOMMENTS)
	return 2;
    i=strlen(buf);
    /* see if we've allocated space for this comment */
    if (comments[numcom])
	free(comments[numcom]);
    if ((comments[numcom] = malloc(i+1)) == NULL)
	    return -1;
    /* remove any newline */
    if (buf[i-1] == '\n')
	buf[i-1] = '\0';
    i=1;
    if (buf[1] == ' ')	/* remove one leading blank from the comment, if there is one */
	i=2;
    strcpy(comments[numcom++], &buf[i]);
    return 1;
}

/* skip to the end of the current line and any subsequent blank lines */

static void
skip_line(FILE *fp)
{
    while (fgetc(fp) != '\n') {
	if (feof(fp))
	    return;
    }
}

/* reset pattern_used[] array */

static void
init_pats_used(void)
{
	int i;
	pats_used = false;
	for (i = 0; i < NUMPATTERNS; ++i)
		pattern_used[i] = false;
}

#ifdef V4_0

/* stackobj, push(), and pop() procs used for imported Fig pictures */

int suppress_error=0;

static struct stackobj {
	User_color	user_colors[MAX_USR_COLS];
	int		user_col_indx[MAX_USR_COLS];
	int		num_usr_cols;
	char		buf[BUFSIZ];
	int		line_no;
	int		num_object;
	int		v2_flag;
	int		v21_flag;
	int		v30_flag;
	float		THICK_SCALE;
	int		landscape;
	int		center;
	double		mag;
	bool		pats_used;
	bool		pattern_used[NUMPATTERNS];
	bool		std_color_used[NUM_STD_COLS];
	bool		arrows_used;
	bool		arrow_used[NUMARROWS];

	struct		stackobj *previous;
	int		stack_height;
};

static struct stackobj *top = NULL;

static int push() {
  struct stackobj* newobj;
  int i;

  if (NULL==(newobj= (struct stackobj*) malloc(sizeof(struct stackobj))))
    return -1;

  newobj->previous=top;
  top=newobj;

  for (i=0; i<MAX_USR_COLS; ++i) {
    top->user_colors[i] = user_colors[i];
    top->user_col_indx[i] = user_col_indx[i];
  }
  top->num_usr_cols = num_usr_cols;
  for (i=0; i < BUFSIZ; ++i)
    top->buf[i]=buf[i];
  top->line_no=line_no;
  top->num_object=num_object;
  top->v2_flag=v2_flag;
  top->v21_flag=v21_flag;
  top->v30_flag=v30_flag;
  top->THICK_SCALE=THICK_SCALE;
  top->landscape=landscape;
  top->center=center;
  top->mag=mag;
  top->pats_used=pats_used;
  for (i=0; i<NUMPATTERNS; ++i)
    top->pattern_used[i]=pattern_used[i];
  for (i=0; i<NUM_STD_COLS; ++i)
    top->std_color_used[i]=std_color_used[i];
  top->arrows_used = arrows_used;
  for (i=0; i<NUMARROWS; ++i)
    top->arrow_used[i]=arrow_used[i];

  if (top->previous!=NULL) {
    top->stack_height=top->previous->stack_height+1;
  }
  else
    top->stack_height=0;

  line_no=0;
  return 0;
}

static int pop() {
  struct stackobj *oldtop;
  int i;
  if (top==NULL)
    return -1;

  for (i=0; i < MAX_USR_COLS;i++) {
    user_colors[i]=top->user_colors[i];
    user_col_indx[i]=top->user_col_indx[i];
  }
  num_usr_cols=top->num_usr_cols;
  for (i=0;i < BUFSIZ;++i)
    buf[i]=top->buf[i];
  line_no=top->line_no;
  num_object=top->num_object;
  v2_flag=top->v2_flag;
  v21_flag=top->v21_flag;
  v30_flag=top->v30_flag;
  THICK_SCALE=top->THICK_SCALE;
  landscape=top->landscape;
  center=top->center;
  mag=top->mag;
  pats_used=top->pats_used;
  for (i=0; i < NUMPATTERNS;i++)
    pattern_used[i]=top->pattern_used[i];
  for (i=0;i < NUM_STD_COLS;++i)
    std_color_used[i]=top->std_color_used[i];
  arrows_used = top->arrows_used;
  for (i=0; i < NUMARROWS; ++i)
    arrow_used[i]=top->arrow_used[i];

  oldtop=top;
  top=top->previous;
  free (oldtop);

  if (top==NULL)
    return 0;
  else
    return top->stack_height;

}

#endif /* V4_0 */

/* this function is to count line numbers correctly while reading
 * input files.
 * It skips all tabs and spaces and increments the global
 * variable line_no if a newline was found.
 * If any other character is read, it is put back to the input
 * stream and the function returns.
 * It should be called from within the point reading loops
 * in the read_{line,spline}object functions, where the point
 * coordinates may be given in an arbitrary number of lines.
 * Added by Andreas_Bagge@maush2.han.de (A.Bagge), 14.12.94
 */

static void
count_lines_correctly(FILE *fp)
{
	int cc;
	do {
	    cc = getc(fp);
	    if (cc == '\n') {
		++line_no;
		cc=getc(fp);
	    }
	} while (cc == ' ' || cc == '\t');
	ungetc(cc,fp);
}

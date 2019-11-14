/*
 * Fig2dev: Translate Fig code to various Devices
 * Copyright (c) 1985 by Supoj Sutantavibul
 * Copyright (c) 1991 by Micah Beck
 * Parts Copyright (c) 1989-2002 by Brian V. Smith
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

#include "fig2dev.h"
#include "alloc.h"
#include "object.h"	/* does #include <X11/xpm.h> */

static double		forward_arrow_wid = 4;
static double		forward_arrow_ht = 8;
static int		forward_arrow_type = 0;
static int		forward_arrow_style = 0;
static double		forward_arrow_thickness = 1;

static double		backward_arrow_wid = 4;
static double		backward_arrow_ht = 8;
static int		backward_arrow_type = 0;
static int		backward_arrow_style = 0;
static double		backward_arrow_thickness = 1;

F_arrow *
forward_arrow(void)
{
	F_arrow		*a;

	if (NULL == (Arrow_malloc(a))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	a->type = forward_arrow_type;
	a->style = forward_arrow_style;
	a->thickness = forward_arrow_thickness*THICK_SCALE;
	a->wid = forward_arrow_wid;
	a->ht = forward_arrow_ht;
	return(a);
}

F_arrow *
backward_arrow(void)
{
	F_arrow		*a;

	if (NULL == (Arrow_malloc(a))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	a->type = backward_arrow_type;
	a->style = backward_arrow_style;
	a->thickness = backward_arrow_thickness*THICK_SCALE;
	a->wid = backward_arrow_wid;
	a->ht = backward_arrow_ht;
	return(a);
}

F_arrow *
make_arrow(int type, int style, double thickness, double wid, double ht)
{
	F_arrow		*a;

	if (style < 0 || style > 1 || type < 0 || (type + 1) * 2 > NUMARROWS)
		return NULL;
	if (NULL == (Arrow_malloc(a))) {
		put_msg(Err_mem);
		return NULL;
	}

	arrows_used = true;
	arrow_used[2*type + style] = true;

	a->type = type;
	a->style = style;
	a->thickness = thickness*THICK_SCALE;
	a->wid = wid;
	a->ht = ht;
	return a;
}

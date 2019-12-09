#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "u_search.h"
#include "u_list.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_modepanel.h"
#include "w_mousefun.h"
#include "w_msgpanel.h"
#include "f_util.h"
#include "u_geom.h"
#include "u_redraw.h"
#include "w_cursor.h"

static void send_back(F_line *obj, int type, int x, int y, F_point *p, F_point *q, int pnum);

void send_to_back_selected(void)
{
    set_mousefun("send back", "", "", LOC_OBJ, LOC_OBJ, LOC_OBJ); //mouse button choice text
    canvas_leftbut_proc = object_search_left; //sets action of left click
    init_searchproc_left(send_back); //sends to instance
    set_cursor(pick9_cursor); //"finger" pointer
}

static void send_back(F_line *obj, int type, int x, int y, F_point *p, F_point *q, int pnum)
{
    //uses &objects as a compound to use pre-existing function
    if (find_largest_depth(&objects) == MAX_DEPTH) {
        //set the depth to max, update depth manager
        remove_depth(type, obj->depth);
        obj->depth = MAX_DEPTH;
        add_depth(type, obj->depth);
        //warning message
        put_msg("There was already an object at 999 depth. Change one manually if they overlap.");
    }
    else {
        //set the depth to largest+1, update depth manager
        remove_depth(type, obj->depth);
        obj->depth = find_largest_depth(&objects)+1;
        add_depth(type, obj->depth); //must be after setting depth
        //success message
        put_msg("Successfully sent back.");
    }

    redisplay_canvas(); //redraw to see changes
}


#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "e_scale.h"
#include "u_draw.h"
#include "u_search.h"
#include "u_create.h"
#include "u_elastic.h"
#include "u_list.h"
#include "u_markers.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_modepanel.h"
#include "w_mousefun.h"
#include "w_msgpanel.h"

#include "f_util.h"
#include "u_geom.h"
#include "u_redraw.h"
#include "w_cursor.h"
#include "w_util.h"

static void send_back(F_line *obj, int type, int x, int y, F_point *p, F_point *q, int pnum);


void send_to_back_selected(void)
{
    //FROM e_movept.c (same pointer)
     set_mousefun("send back", "", "", LOC_OBJ, LOC_OBJ, LOC_OBJ);
     canvas_kbd_proc = null_proc;
     canvas_locmove_proc = null_proc;
     canvas_ref_proc = null_proc;


    canvas_leftbut_proc = object_search_left;
    //canvas_middlebut_proc = point_search_middle;
    init_searchproc_left(send_back);
    //init_searchproc_middle(delete_arrow_head);
    // //init_searchproc_left(init_arb_move_point);
    // //init_searchproc_middle(init_stretch_move_point);
    // canvas_leftbut_proc 
    // = point_search_left;
     //canvas_middlebut_proc = null_proc;
     canvas_rightbut_proc = null_proc;
     set_cursor(pick9_cursor);
     force_anglegeom();
     reset_action_on();
     
}

static void send_back(F_line *obj, int type, int x, int y, F_point *p, F_point *q, int pnum)
{
    if (obj->depth == 999) {
        put_msg("999 depth");
    }

    remove_depth(type, obj->depth); //manages the depth manager
    obj->depth = 998;
	add_depth(type, obj->depth);
    redisplay_canvas();

}


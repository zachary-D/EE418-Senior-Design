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
//#include "w_modepanel.h"
#include "w_mousefun.h"
#include "w_msgpanel.h"

#include "f_util.h"
#include "u_geom.h"
#include "u_redraw.h"
#include "w_cursor.h"
#include "w_util.h"


//desired_depth = max_depth()->depth+1;

void send_to_back_selected(void)
{
    //FROM e_movept.c (same pointer)
	//this particular line is for that top right icon with the mouse buttons
    set_mousefun("Send to Back Select", " ", " ", LOC_OBJ, LOC_OBJ, LOC_OBJ);
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    //init_searchproc_left(init_arb_move_point);
    //init_searchproc_middle(init_stretch_move_point);
    canvas_leftbut_proc = set_depth;//what the left mouse button will call
    canvas_middlebut_proc = null_proc;//want the middle and right 
    canvas_rightbut_proc = null_proc;
    set_cursor(pick9_cursor);//@TODO need to figure out wha this means
    force_anglegeom();//@TODO don't think i need this
    reset_action_on();
    return;
}


void set_depth(selected_object, desired_depth)

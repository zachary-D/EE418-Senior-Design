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

<<<<<<< HEAD
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
    set_cursor(pick9_cursor);//@TO-DO need to figure out what this means, think its just what cursor is used while the tool is active
			//leaving it as is
    //force_anglegeom();//@TO-DO don't think i need this
    reset_action_on();
    return;
}


void set_depth(selected_object, desired_depth)
{
//@TODO need to actually create
//needs to send the object value to this function so that the depth can be changed, also need to find the current max depth and
//set the depth to that +1

//major first step-find out how to reference the selected object
//then i need to find that objects depth
//then that depth needs to be changed

//found this in e_edit.c
//(void) int_panel(generic_vals.depth, form, "Depth", lbeside, &depth_panel, MIN_DEPTH, MAX_DEPTH, 1);

//Left over from a merge conflict, likely trash but uncertain
/*     set_mousefun("send back", "", "", LOC_OBJ, LOC_OBJ, LOC_OBJ);
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
*/
=======
void send_to_back_selected(void)
{
    set_mousefun("send back", "", "", LOC_OBJ, LOC_OBJ, LOC_OBJ); //mouse button choice text
    canvas_leftbut_proc = object_search_left; //sets action of left click
    init_searchproc_left(send_back); //sends to instance
    set_cursor(pick9_cursor); //"finger" pointer
>>>>>>> a355a325f6845fc38b6f9912b342c2b0c06afaba
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

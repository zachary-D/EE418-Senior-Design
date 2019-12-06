#ifndef GENSVG_H
#define GENSVG_H


extern void put_sanitized_char(int c);
extern void continue_paint(int fill_style, int pen_color, int fill_color);
extern void continue_paint_w_clip(int fill_style, int pen_color, int fill_color);
extern void	gensvg_arc(F_arc *a);
extern void	gensvg_ellipse(F_ellipse *e);
extern void	gensvg_line(F_line *l);
extern void	gensvg_spline(F_spline *s);
extern void	gensvg_text(F_text *t);

#endif /* GENSVG_H */
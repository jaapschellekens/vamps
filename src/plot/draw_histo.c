#include "graph.h"

double	ctm[6];			/* coor --> plot matrix */
int	llx, lly, urx, ury;	/* the graph viewport limits */

/*
 * draw_graph draws a histogram from the data
 */
int
draw_histo(p, np, linemode, width)
Coord2	*p;
int	np, linemode, width;
{
	int	i, j;		 	/* indices in data array */
	double	yval;			/* box height y value */
	extern char	*linemodes[];	/* in graph.c */

	getmatrix(ctm);
	getviewport(&llx, &lly, &urx, &ury);
	linemod(linemodes[linemode]);

	if(width < 0) {		/* backward boxes */
		for(i = -width; i < np; i -= width) {
			yval = 0.0;
			for(j = i; j > i + width; j--) {
				if(p[j].x < p[j-1].x)	/* exceeds dataset */
					break;
				yval += p[j].y;
			}
			if(j - i == width)
				draw_box(p[j].x, 0.0, p[i].x, yval);
			else
				i = j;	/* align new dataset */
		}
	}
	else if(width > 0) {	/* forward boxes */
		for(i = 0; i < np - width; i += width) {
			yval = 0.0;
			for(j = i; j < i + width; j++) {
				if(p[j].x > p[j+1].x)	/* exceeds dataset */
					break;
				yval += p[j].y;
			}
			if(j - i == width)
				draw_box(p[j].x, 0.0, p[i].x, yval);
			else
				i = j + 1 - width;	/* align new dataset */
		}
	}
	else for(i = 0; i < np; i++)	/* draw lines graph */
		draw_box(p[i].x, 0.0, p[i].x, p[i].y);

	return(0);
}

draw_box(vx0, vy0, vx1, vy1)
double	vx0, vy0, vx1, vy1;
{
	int	x0, y0, x1, y1;

	x0 = xcvt(ctm, vx0, vy0);
	y0 = ycvt(ctm, vx0, vy0);
	x1 = xcvt(ctm, vx1, vy1);
	y1 = ycvt(ctm, vx1, vy1);

/* clip box */
	if((x0 < llx && x1 < llx) || (x0 > urx && x1 > urx)
		|| (y0 < lly && y1 < lly) || (y0 > ury && y1 > ury))
		return;	/* fully outside viewport */

	if(x0 < llx)
		x0 = llx;
	else if(x0 > urx)
		x0 = urx;
	if(x1 < llx)
		x1 = llx;
	else if(x1 > urx)
		x1 = urx;
	if(y0 < lly)
		y0 = lly;
	else if(y0 > ury)
		y0 = ury;
	if(y1 < lly)
		y1 = lly;
	else if(y1 > ury)
		y1 = ury;

	(void)box(x0, y0, x1, y1);

	return;
}

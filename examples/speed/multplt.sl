variable _multon = 0;
variable _subindex = 0;
variable _mm;

%I:pprint
%@Void pprint()
% 
% Print the current plot on a postscript printer

define pprint ()
{
	echo("not implemented yet");
	echo("use native gnuplot commands to accomplish this");
}

%I:subplot
%@Void subplot(x, y, index)
%
% Set the plotting system for multiple plots on one page. @x@ and @y@ are the
% number of plots in the x and y direction respectively. @index@ denotes the
% number of the subplot in which the next plot is drawn. Numbering starts
% at one.  The first plot is drawn in the upper left corner, the last in the
% lower right. Currently subplot only works with gnuplot. See also
% @endsubplot@.
define subplot(x, y, index)
{
	variable xo, yo, xs, ys;

	if ((index > (x * y)) or (index <= 0)) {
		echo("subplot: index out of range");
		return;
	}

	xs = 1.0/x;
	ys = 1.0/y;
	plt("set size 1,1");
	if (_multon == 0) {
		plt("set multiplot");
		_multon = 1;
	}

	plt(sprintf("set size %f,%f",xs,ys,2));

	yo =  1.0 - (ys * (1 + ((index - 1) / x)));
	xo =  xs * ((index - 1) mod x);
	plt(sprintf("set origin %f,%f",xo,yo,2));
	_subindex = index;
}

%I:endsubplot
%@ Void endsubplot()
%
% Resets the plotting system to single plot mode. Also needed to actually
% draw the plot on some terminals. See also @subplot@.
define endsubplot()
{
	_multon = 0;
	plt("set nomultiplot");
	plt("set size 1,1");
	plt("set origin 0,0");
}


%define plot(m)
%{
%	_mm = m;
%	plt("plot @_mm@ using 1:2");
%}

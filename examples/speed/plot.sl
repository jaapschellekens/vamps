%I:plot.sl
%@plot.sl - high level plotting functions
%
% Plot functions to allow quick visualization of Vamps output files
% and matrix variables. The syntax of the high level functions (@f_plot@
% @p_plot@ and @m_plot@) is the same, no matter which underlying graphics method
% is used. Low level functions depend on the graphics method used. The
% following graphics methods can be used:
%@	plot		- gnu plotutils lib 1.1
%@	agl		- AGL graphics lib 3.?
%@	ext_plot	- using external gnu plotutils programs (UNIX only)	
%@	gnu_plot	- using gnuplot interface (UNIX only)	
%@			  see also @#@ and @plt@.	
% Which of these methods can be used in this version of vamps is
% printed to the screen when using the @plot@ command to initialize the
% plot system. The @show_plotsys()@ function can be used for this purpose
% as well.
%
% The @outputdev@ variable can be used to specify screen preview (1) or
% hardcopy output (0).
%
% If more than one graphics method is available you can switch between 
% methods be setting the @m_plot@ function to the method specific function.
% Use the @set_plotsys@ or the @set_plotsys_byname@ function to do so.

%I:plot
%@ Void plot
% Initialize plot functions. None of functions in @plot.sl@ can be
% used before this command is issued. 
% Functions are:
%@ f_plot, m_plot, p_plot
% See @plot.sl@ for a description of the methods used
variable plverb = 0;
define plot {};

autoload("subplot","multplt.sl");
autoload("endsubplot","multplt.sl");

%I:plotstyle
%@Int plotstyle;
%
% Set the plotting style:
%@ 0 = lines, 1 is markers, 2 = markers & lines
variable plotstyle = 0; % 0 = lines, 1 is markers, 2 = markers & lines

% the stats functions should be loaded as well for this to work.
stats; % Load the stats functions

variable nrplotsys = 0; % Number of plotting systems 
variable curplotsys = 0; % Current plotting system
variable plotsys = create_array('s', 10, 2, 2); 
variable plotsl_version = 0.11;
variable tmpplotname = "tt.plt";
variable outputdev = 1; %  0 = hardcopy 1 = screen
variable nrhcplot = 0; % number of hard copies generated
variable nrscplot = 0; % number of screen plots generated
variable mm; % matrix to plot

if (plverb)
	printf("plot: initializing plot.sl version %4.2f\n",plotsl_version,1);
if (plverb > 1)
	printf("plot: outputdev = %d (0 = hardcopy, 1 = screen)\n",outputdev,1);

%------------------------------------------------------------------------------
% Use default plotting function. Does nothing
%------------------------------------------------------------------------------
plotsys[nrplotsys,0] = "Default (does nothing)";
plotsys[nrplotsys,1] = "def_m_plot";
nrplotsys++;
define def_m_plot(m, xcol, ycol, title, xlab, ylab)
{
	fprintf(stderr,"plot: (dev) No plotting device available\n",0);
	return;
}

#ifdef AGL
%------------------------------------------------------------------------------
% Use AGL compiled in graphics if present
%------------------------------------------------------------------------------
if (strcmp(getenv("DISPLAY"),"") == 0){
	variable agl_screen = "tt:";
}else{
	variable agl_screen = "x:";
}

AG_SSET("msgw;debug=0;ERRF=aglerr.log");

if (strcmp(agl_screen,"ws:") == 0 ){
if (plverb)
	fprintf(stderr,"plot: (agl) Make sure the x11serv program is started\n",0);
}

variable agl_hard = "ps:";
plotsys[nrplotsys,0] = "AGL internal graphics";
plotsys[nrplotsys,1] = "agl_m_plot";
nrplotsys++;
if (plverb)
fprintf(stderr,"plot: (agl) initializing %s internal graphics (agl_screen = %s, agl_hard = %s)\n",AG_IDN,agl_screen,agl_hard,3);
define agl_m_plot(m, xcol, ycol, title, xlab, ylab)
{
	variable max = mmax(m);
	variable min = mmin(m);
	variable options;
	
	mm = m;

	if (strcmp(getenv("AGL3CONFIG"),"") == 0){
		echo("AGL3CONFIG environment variable not set.");
		echo("example: setenv AGL3CONFIG /home/schj/agl/");
		echo("You can also use the S-Lang putenv function:");
		echo("putenv(\"AGL3CONFIG=/location/of AGL/runtime/files/\")");
		return;
	}
	options = sprintf("title=%s;laby=%s;labx=%s;",title,ylab,xlab,3);
	if (nrscplot < 1 and outputdev == 1){
		%() = AG_VDEF(agl_screen,min[0,xcol],max[0,xcol],min[0,ycol],max[0,ycol],0.0,0.0);
		() = AG_VDEF(agl_screen,0,1.0,0,1.0,0.0,0.0);
		nrscplot++;
	}
	if (outputdev == 0){
		%() = AG_VDEF(agl_hard,min[0,xcol],max[0,xcol],min[0,ycol],max[0,ycol],0.0,0.0);
		() = AG_VDEF(agl_hard,0,1.0,0,1.0,0.0,0.0);
		nrhcplot++;
		fprintf(stderr,"plot: (agl) created plot %d\n",nrhcplot,1);
	}
	AG_AXES(min[0,xcol],max[0,xcol],min[0,ycol],max[0,ycol],options);

	if (plotstyle == 0)
		AG_GPLL(m,xcol,ycol);

	if (plotstyle == 2){
		AG_GPLL(m,xcol,ycol);
		AG_GPLM(m,xcol,ycol,4);
	}

	if (plotstyle == 1)
		AG_GPLM(m,xcol,ycol,4);

	AG_VUPD;
	if (outputdev == 0) % close the plotting file
		AG_CLS;
}
#endif
#ifdef PLOT
%------------------------------------------------------------------------------
% Use the (not finished) linked  plot library
%------------------------------------------------------------------------------
variable pl_dev = 1;
plotsys[nrplotsys,0] = "libPLOT internal graphics";
plotsys[nrplotsys,1] = "plot_m_plot";
nrplotsys++;
if (plverb)
echo("plot: (plot) initializing libPLOT internal graphics");
() = pl_init_f();
define plot_m_plot(m, xcol, ycol, title, xlab, ylab)
{
	variable max = mmax(m);
	variable min = mmin(m);
	variable rows,i;
	variable fn;

	mm = m;
	% only hard copy (plot) at the moment
	nrhcplot++;
	(rows, ) = asize(m);
	echo(rows);
	pl_output_is_ascii = 1;
	fn = sprintf("plt.%d",nrhcplot,1);
	fprintf(stderr,"plot: (plot) Created %s\n",fn,1);
	pl_outfile("|plot2X -I");
	() = pl_openpl;
	() = pl_fspace(min[0,xcol],min[0,ycol],max[0,xcol],max[0,ycol]);

	for (i = 1; i < rows; i++){
		() = pl_fline(m[i - 1 ,0],m[i - 1, 1],m[i ,0],m[i, 1]);
	}
	() = pl_closepl;
}

#endif

#ifndef MSDOS
variable pl_dev = 1;
plotsys[nrplotsys,0] = "gnuplot external graphics";
plotsys[nrplotsys,1] = "gnu_m_plot";
nrplotsys++;
if (plverb)
	echo("plot: (gnuplot) initializing gnuplot external graphics");
define gnu_m_plot(m, xcol, ycol, title, xlab, ylab)
{
	variable plstr;
	
	mm = m;
	% plt(sprintf("set auto",0));
	mwrite(m,tmpplotname);
	if (outputdev == 0){
		plt("set term postscript\n");
		plt(sprintf("set output 'psplot.%d'\n",nrhcplot,1));
		fprintf(stderr,"plot: (gnu_plot) created output in psplot.%d\n",nrhcplot,1);
		nrhcplot++;
	} else {
		if (strcmp(getenv("DISPLAY"),"") == 0){
			echo("plot: (gnuplot) no display set, using tektronic graphics.");
			plt("set term vttek");
		}
		nrscplot++;
	}
	plt(sprintf("set xlabel \"%s\"\n",xlab,1));
	plt(sprintf("set ylabel \"%s\"\n",ylab,1));
	%plstr = sprintf("plot \"%s\" using %d:%d\n",tmpplotname,xcol + 1,ycol + 1,3); 
	plstr = sprintf("plot @mm@ using %d:%d\n",xcol + 1,ycol + 1,2); 
	plt(plstr);

	if (outputdev == 0)
		plt("set output\n");
	() = system(sprintf("rm %s",tmpplotname,1));
}
#endif

#ifndef MSDOS
%-------------------------------------------------------------------------
% use external programs (GNU plotutils1.1)
% ------------------------------------------------------------------------
if (plverb)
	echo("plot: (plot_ext) initializing gnu external programs");
if (plverb)
	echo("plot: (plot_ext) use the graph_opt variable to add extra graph options");

variable graph_opt = "";
plotsys[nrplotsys,0] = "gnu plotutils external programs";
plotsys[nrplotsys,1] = "ext_plot_m_plot";
nrplotsys++;

define ext_plot_m_plot (m, xcol, ycol, title, xlab, ylab)
{
	variable gr_opt;

	mm = m;
	if (plotstyle == 0)
		gr_opt = graph_opt;

	if (plotstyle == 1)
		gr_opt = strcat("-m-1 -S 2 ",graph_opt);

	if (plotstyle == 2)
		gr_opt = strcat("-m1 -S 2 ",graph_opt);

	mwrite(m,tmpplotname);
	if (outputdev == 1){
		if (strcmp(getenv("DISPLAY"),"") == 0){
			echo("plot: (plot) no display set, using tektronic graphics.");
			() = system(sprintf("awk '{print $%d,$%d}' %s | graph-tek -X%s -Y%s -L%s %s ",xcol + 1,ycol + 1,tmpplotname,xlab,ylab,title,gr_opt,7));
		}else{
			echo("plot: (plot) Using X11 graphics");
			() = system(sprintf("awk '{print $%d,$%d}' %s | graph-X -X%s -Y%s -L%s %s ",xcol + 1,ycol + 1,tmpplotname,xlab,ylab,title,gr_opt,7));
		}
		nrscplot++;
	}else{
		() = system(sprintf("awk '{print $%d,$%d}' %s | graph-ps -X%s -Y%s -L%s %s %s > %s.ps",xcol + 1,ycol + 1,tmpplotname,xlab,ylab,title,gr_opt,title,8));
		fprintf(stderr,"plot: (ext_plot) Created %s.ps.\n",title,1);
		nrhcplot++;
	}
	() = system(sprintf("rm %s",tmpplotname,1));
}
#endif

%I:m_plot
%@ Void m_plot(Matrix m, Int xcol, Int ycol, String title, String xlab,
%@		String ylab)
%
% Plot an XY diagram of matrix @m@ using the current graphics method.
% Column numbering for @xcol@ and @ycol@ starts at zero. Depending on the
% graphics method used the top, x and y-axis labels strings cannot be empty.
% example:
%@ outputdev = 1; % screen graphics
%@ m_plot(m,0,1,"Matrix m","Time","Q");
%@ outputdev = 0; % Plot is OK, make hardcopy
%@ m_plot(m,0,1,"Matrix m","Time","Q");
% See also @plot@ and @plot.sl@.
variable m_plot = &def_m_plot;


%I:p_plot
%@Void p_plot(String var,String  thef, Float time)
%
% Plot an XY diagram of depth profile of @var@ for @time@ in Vamps
% outputfile @thef@. Can only  be used with variable like theta
% and h that are profile data.
%
% See also @m_plot@ and @f_plot@
define p_plot(var,thef,time)
{
	variable ar;
	variable xlab;

	if (vprof(var,time,thef) != -1){
		ar = ();
		% feed to m_plot...
		xlab = var;
		m_plot(ar,0,1,sprintf("\"%s:%s\"",var,thef,2),xlab,"depth");
		%m_plot(ar,xcol,ycol,"","","");
	} else {
		echo("Plotting failed.");
		return;
	}
}

%I:f_plot
%@Void f_plot(String var,String  thef,Int xcol, Int ycol)
%
% Plots var @var@ from vamps output file @thef@.
% It does so by calling @vsel@ to construct a matrix. This
% matrix is than passed to @m_plot@ to make the actual plot.
% Column 0 is time.
% See also @plot@ and @plot.sl@.
define f_plot(var,thef,xcol,ycol)
{
	variable ar;
	variable xlab;

	% read variable from output file
	if (vsel(var,-1,thef) != -1){
		ar = ();
		% feed to m_plot...
		if (xcol != 0)
			xlab = var;
		else
			xlab = "Time";

		m_plot(ar,xcol,ycol,sprintf("\"%s:%s\"",var,thef,2),xlab,var);
		%m_plot(ar,xcol,ycol,"","","");
	} else {
		echo("Plotting failed.");
		return;
	}
}

%I:show_plotsys
%@ Void show_plotsys
%
% Prints short descriptions of all the installed plotting systems
% to the screen. The system indicated with a '*' is the active system.
define show_plotsys()
{
	variable i;

	for (i = 0; i < nrplotsys; i++){
		if (i == curplotsys)
			fprintf(stderr,"*",0);
		else	
			fprintf(stderr," ",0);
		fprintf(stderr,"%d:\t%s (%s)\n",i,plotsys[i,0],plotsys[i,1],3);
	}
}

%I:set_plotsys
%@ Void set_plotsys(Int nr)
%
% Sets the uses plotting system to the system @nr@. It uses @show_plotsys@
% to display the updated list of available plot-systems. If the @plverb@
% variable is > 1
define set_plotsys(nr)
{
	curplotsys = nr;
	eval(sprintf("m_plot = &%s;\n",plotsys[nr,1],1));
	if (plverb > 1)
		show_plotsys;
}

%I:set_plotsys_byname
%@ Void set_plotsys_byname(String name)
%
% Sets the uses plotting system to the system @name@. It uses @show_plotsys@
% to display the updated list of available plot-systems.
define set_plotsys_byname(name)
{
	variable i;

	for (i = 0; i < nrplotsys; i++){
		if (strcmp(name,plotsys[i, 1]) == 0){
			set_plotsys(i);
			return;
		}
	}
	fprintf(stderr,"plot: could not find plot system %s\n",name,1);
}

set_plotsys_byname("gnu_m_plot");

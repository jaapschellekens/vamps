% Simple test setup of graphical progress using the PLOT library
% This setup only works on UNIX systems with X11 support

% initialize plotting system
plot;

define vamps_progress {}

define at_start()
{
#ifdef AGL
	set_plotsys_byname("agl_m_plot");
	() = AG_VDEF("tt:",0,1.0,0,1.0,0,0);
	AG_AXES(0,float(v_steps),0.35 *
	float(v_volsat),float(v_volsat),"xlab=Time;title=Water content");
	AG_VUPD;
#else
#ifdef PLOT
#ifdef UNIX
	set_plotsys_byname("plot_m_plot");
	pl_output_is_ascii = 1;
	pl_outfile("|plot2X -I");
	() = pl_openpl;
	() = pl_fspace(0,0,v_steps,v_volsat);
	() = pl_fmove(0,0);
#endif
#endif
#endif
	pause(1);
} 

define at_end()
{
#ifdef AGL
	pause(-1);
	AG_CLS;
#else
#ifdef PLOT
#ifdef UNIX
	() = pl_closepl();
#endif
#endif
#endif
}

variable v_volact_old = 0.0;
define each_step()
{
#ifdef AGL
	variable m = matrix(2,2);
	
	m[0,0] = v_thisstep - 1;
	m[1,0] = v_thisstep;
	m[1,1] = v_volact;
	m[0,1] = v_volact_old;
	v_volact_old = v_volact;
	AG_GPLL(m,0,1);
	AG_VUPD;
#else
#ifdef PLOT
#ifdef UNIX
	() = pl_fcont(v_thisstep,v_volact);
	() = pl_flush;
	() = pl_endpoly();
#endif
#endif
#endif
}

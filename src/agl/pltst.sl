variable m =  matrix(100,2);
variable i;

for (i = 0; i < 100; i++){
	m[i,0] = i;
	m[i,1] = (i + 1) * 100.0;
}

variable drv = "x:";
AG_SSET("msgw;debug=1;ERRF=/dev/null");
echo(AG_VDEF(drv,0.0,1.0,0.0,1.0,0.0,0.0));
AG_WDEF(0.0,100.0,1.0,10000.0);
AG_SSET("logy");
AG_GPLL(m,0,1);AG_VUPD;
pause(-1);
echo(AG_VDEF(drv,0.0, 0.5, 0.0, 0.5,0.0,0.0)); % Redraw everything in different scales
AG_AXES(0.0, 100.0, 1.0, 10000.0,"logy;labx=X axis;laby=Y axis;title=Redrawing 1");
AG_GPLL(m,0,1);AG_VUPD;
pause(-1);
variable vp = AG_VDEF(drv, 0.5, 1.0, 0.0, 0.5,0.0,0.0); 
AG_AXES(0.0, 100.0, 1.0, 10000.0,"gridy;logy;labx=X axis;laby=Y axis;title=Redrawing 2");
AG_GPLL(m,0,1);AG_VUPD;
pause(-1);
echo(AG_VDEF(drv,0.0, 0.5, 0.5, 1.0,0.0,0.0)); 
AG_AXES(0.0, 100.0, 1.0, 10000.0,"gridx;logy;labx=X axis;laby=Y axis;title=Redrawing 3");
AG_GPLL(m,0,1);AG_VUPD;
pause(-1);
echo(AG_VDEF(drv, 0.5,1.0,0.5,1.0,0.0,0.0)); 
AG_AXES(0.0, 100.0, 1.0, 10000.0,"grid;xspace=9;labx=X axis;laby=Y axis;title=Redrawing 4");
AG_GPLL(m,0,1);AG_VUPD;
AG_SSET("cursor=-1");
pause(-1);
AG_CLS;

% exit;


define at_start ()
{
	echo(steps);
	echo(volsat);
	AG_VDEF(drv,0.0,float(steps),0.2 * volsat,volsat,0.0,0.0);
	AG_AXES(0.0,float(steps),0.2 * volsat,volsat,"title=Actual_water_content;");
	AG_VUPD;
	pause(0);
}
define at_end ()
{
	pause(0);
	AG_CLS();
}

variable volactold = 0.0;
define each_step ()
{
	variable m = matrix(2,2);

	if (thisstep > 0) {
		m[0, 1] = volactold;
		m[0, 0] = thisstep - 1;
		m[1, 1] = volact;
		m[1, 0] = thisstep;
		AG_GPLL(m,0,1);
		AG_VUPD;
	}

	volactold = volact;
}

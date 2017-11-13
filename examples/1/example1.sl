% This S-Lang file sets some stuff for example1.inp
define stop{}; % Get rid of mass balance check
plot;

define at_start()
{
	variable ans;

	plot;
	verbose = 0;
	soilverb = 1;
	echo("-------------------------------------------------------------------");
	echo("Simple Vamps example. In this case all top fluxes are precalculated");
	echo("(you can find them in the 'all.inp' file) and only one soil layer");
	echo("is defined.");
	echo("This example run takes about 4 seconds on my AMD486 160Mhz");
	echo("-------------------------------------------------------------------");
	if (strcmp("y",usergetstr("start the run (y/n)")) != 0)
		exit;
} 

define at_end()
{
	echo("");
	if (strcmp("y",usergetstr("create a report of this run (y/n)")) != 0)
		exit;
	rinmem("example1.out");
	rep("example1.out","example1.rep");
	echo("Report saved in 'example1.rep'.");
	printf("Vamps used %f cpu seconds for this run\n",cpu,1);
	if (strcmp("y",usergetstr("make graphs of results (y/n)")) != 0)
		exit;
	show_plotsys;
	set_plotsys(integer(usergetstr("give number of plotting system to use: ")));
	if( strcmp("y",usergetstr("Do you have graphics capable terminal (y/n)")) != 0){
		outputdev = 0;
		echo("Saving output to disk instead.");
	}

	f_plot("volact","example1.out",0,1);
	echo("Plot of the water content in time");
	pause(0);
	f_plot("theta","example1.out",0,1);
	echo("Plot theta at layer 0");
	pause(0);
	f_plot("theta","example1.out",0,10);
	echo("Plot theta at layer 10");
	pause(0);
	f_plot("qbot","example1.out",0,1);
	echo("Drainage through bottom of profile");
	pause(0);
	f_plot("SMD","example1.out",0,1);
	echo("Soil moisture deficit");
	pause(0);
}

define each_step()
{
}

() = evalfile("soilf.sl");

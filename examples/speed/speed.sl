% This S-Lang file sets some stuff for example1.inp

define stop{}; % Get rid of mass balance check

define at_start()
{
	variable ans;

	verbose = 0;
	soilverb = 1;
	echo("-------------------------------------------------------------------");
	echo("Vamps example to determine calculation speed of your setup");
	echo("It uses a fixed dt of 1.0E-4.");
	echo("This example run takes about 90 seconds on my AMD486 160Mhz");
	echo("Results are stored in speed.txt");
	echo("-------------------------------------------------------------------");
} 

define at_end()
{
	variable cpu_used = cpu;
	variable fp;

	printf("Vamps used %f cpu seconds for this run\n",cpu_used,1);
	printf("(%f iterations per sec)\n",(v_steps / 1.0E-4) / cpu_used,1);
	variable mhz = usergetstr("MHZ your cpu runs on: ");
	variable cpu_man = usergetstr("Manufactorer of your cpu: ");
	variable cpu_name = usergetstr("Name of your cpu: ");
	variable system_name = usergetstr("Name of your system/motherboard: ");
	variable os_name = _OS_;
	variable tab = getdefint("soil","mktable",0,"speed.inp",0);



   	fp = fopen("speed.txt","a");
	if (fp == -1)
		fp = stdout;
	fprintf(fp,"#Vamps version %s [%s] (%s)\n",_VERSTRING_,_BUILDDATE_,_OS_,3);
	fprintf(fp,"%d,\t%s,\t%s,\t%s,\t%s,\t\t%s,\t%f,\t%5.2f\n",tab,system_name,string(cpu_name),mhz,cpu_man,os_name,cpu_used,(v_steps / 1.0E-4) / cpu_used,8);
	() = fclose(fp);
}

define each_step()
{
}

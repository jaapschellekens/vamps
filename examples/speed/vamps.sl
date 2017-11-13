#ifdef VAMPS
% File to be loaded by vamps at startup. 
% If vamps is compiled with S-Lang support this file _MUST_
% be loaded. Usually this file should reside in your vampslibdir
%
%
if (verbose)
	print("Loading vamp.sl...\n");

%I:debug
%@ Void debug()
%
% Alias for @print_stack@, prints the present contents of the S-Lang
% stack to stdout
%
define debug() { print_stack(); }

%I:echo
%@ Void echo(obj)
%
% Prints an ascii representation of obj, followed by a newline
% on standard output. Supported are: Character, Float, Integer,
% String, and 1D and 2D Arrays for these types.
%
define echo(obj)
{
	variable i, j, m, n, s;

	if(_obj_type(obj) != 20) {
		(,) = fputs(string(obj), stdout);
		() = fputs("\n", stdout);
		return;
	}
	switch(array_info())
	{ case 1: (, m) = ();
		for(i = 0; i < m; i++) {
			() = fputs(string(obj[i]), stdout);
			() = fputs("\n", stdout);
		}
	}
	{ case 2: (, m, n) = ();
		for(i = 0; i < m; i++) {
			s = string(obj[i,0]);
			for(j = 1; j < n; j++) {
				s = strcat(s, " ");
				s = strcat(s, string(obj[i,j]));
			}
			s = strcat(s, "\n");
			() = fputs(s, stdout);
		}
	}
	{ (,,,) = (); error("echo: Three-dimensional arrays not supported"); }
}


%I:ts_echo
%@ Void ts_echo(obj)
%
% Prints an ascii representation of obj, followed by a newline
% on standard output. Supported are: Character, Float, Integer,
% String, and 1D and 2D Arrays for these types. In this case
% all floats and first columns of matrixes are regarded in @ts(5)@
% format
%
define ts_echo(obj)
{
	variable i, j, m, n, s;

	if(_obj_type(obj) != 20) {
		(,) = fputs(string(obj), stdout);
		() = fputs("\n", stdout);
		return;
	}
	switch(array_info())
	{ case 1: (, m) = ();
		for(i = 0; i < m; i++) {
			() = fputs(ts_fmt(obj[i]), stdout);
			() = fputs("\n", stdout);
		}
	}
	{ case 2: (, m, n) = ();
		for(i = 0; i < m; i++) {
			s = ts_fmt(obj[i,0]);
			for(j = 1; j < n; j++) {
				s = strcat(s, " ");
				s = strcat(s, string(obj[i,j]));
			}
			s = strcat(s, "\n");
			() = fputs(s, stdout);
		}
	}
	{ (,,,) = (); error("ts_echo: Three-dimensional arrays not supported"); }
}

%I:printf
%I:fprintf
%I:sprintf
%@ Void printf(String fmt, ..., Integer N)
%@ Void fprintf(Integer fp, String fmt, ..., Integer N)
%@ String sprintf(String fmt, ..., Integer N)
%
% Formatted printing cf. the C printf family, except that
% the number of parameters N must be given. printf and
% fprintf return nothing, sprintf returns the formatted
% string.
%
define printf()
{
       () = fputs(Sprintf(), stdout);
       () = fflush(stdout);
}

define fprintf()
{
	variable s = Sprintf();
	variable fp = ();
	() = fputs(s, fp);
	() = fflush(fp);
}

define sprintf()
{
	Sprintf();
}


% There are functions defined in other files as well, the following
% statements make these autoload whenn called.
#ifdef UNIX
variable tmpdir = "/tmp";
variable pager = "more";
variable cat = "cat";
variable pathsap = "/";
#else
variable tmpdir = ".";
variable pager = "more";
variable cat = "type";
variable pathsap = "\\";
#endif

% another print function
define p(obj)
{
   print(string(obj));
   print("\n");
}

() = evalfile("site.sl");
() = evalfile("util.sl");

% Autoload of utillities
autoload("rep","rep.sl");
autoload("v_show_data_sets","runut.sl");
autoload("showprof","runut.sl");
autoload("saveprof","runut.sl");
autoload("savets","runut.sl");
autoload("v_save_all_sets","runut.sl");
autoload("stop","stop.sl");   

% Autoload of regression equations
autoload("Rs_to_Rn","regress.sl");
autoload("estrs","regress.sl");

% To initialize stat, marquardt-fit and plot utilities
autoload("stats", "stats.sl");
autoload("plot", "plot.sl");
autoload("fit", "fit.sl");

%I:apropos
%@apropos(String substring)
%
% apropos prints all functions and variables containing @substring@ to
% stdout. It assumes the @print()@ intrinsic
define ret_type(str);
define apropos (what)
{
   variable n = slapropos(what);
   variable i, f;

   if (n) 
      print (Sprintf("found %d matches:\n", n, 1));
   else 
      { print ("No matches.\n"); return; }
   
   loop (n) 
     { =f; print (Sprintf("%-26s %-26s\n", f, ret_type(f), 2)); }	

   return;
}

define ret_type(str)
{
variable t = is_defined(str);

  switch (t)
    {case 1: return "Intrinsic function";}
    {case 2: return "User defined function";}
    {case -1: return "Intrinsic variable";}
    {case -2: return "User defined variable";}

  return "Unknown";
}

define startup_func ()
{
	prompt("{%$}Vamps>","{%$}?>");
}


%--------------------------------------------------------------------------
% By changing the parameters printed in the Sxtraout function
% you can customize what is printed in the xtraout file.
variable x_fid = -1;

% Better leave this one alone
define Sopenxtraout(file)
{
   x_fid = fopen(file,"w");
   if (x_fid == -1) 
     error ("fopen failed");
}

% Better leave this one alone
define Sclosextraout()
{
   () = fclose(x_fid);
}

% This stuff determines what is printed in the xtraout file 
define Sxtraout ()
{
   variable s;

   !if (x_fid == -1){
      s = Sprintf("%f\t%f\t%f\n",v_t,v_qtop,v_qbot,3);
      () = fputs(s,x_fid);
   }
}

%I:at_start
%@ Void at_start()
%
% This is the vamps s-lang startup function. It can be completely empty
% if you want. The default shows all the data sets in memory.
% Place extra stuff you want to
% have done at startup in this function.
define at_start ()
{
   variable i;
   
   printf("Run contains %s steps.\n",getdefstr("time","steps","NOT FOUND",v_infilename,0),1);
   for (i = 0; i < _sets_; i++)
     printf("Set %d = %s read from %s\n",i,getset_name(i),getset_fname(i),3);
}

%I:at_end
%@ Void at_end()
%
% The vamps s-lang end function. Place extra stuff you want to
% have done at the end of simulation in this function.
define at_end ()
{
variable grfile,s,ss,sss;

   v_printsum ();	% print summary to stderr 
   printf("\n\nVamps used %f CPU seconds\n",cpu(),1);
}

%I:each_step
%@ Void each_step()
%
% The vamps S-Lang function that is executed after each timestep. 
% Place extra stuff you want to have done after each timestep in 
% this function. Normally this function is defined in @vamps.sl@ but
% it can be redefined in any S-Lang file that is loaded by @Vamps@.
% It's default definition is as follows:
%@	define each_step ()
%@	{
%@	   % Check for conditional switches to interactive mode, see stop.sl
%@	   stop();
%@	   v_printstr("CPU",string(cpu)); % Store CPU time used in output file
%@	}
define each_step ()
{
   % Check for conditional switches to interactive mode, see stop.sl
   stop();
   v_printstr("CPU",string(cpu)); % Store CPU time used in output file
}

%I:adim
%I:asize
%I:atype
%@ Integer adim(Array arr)
%@ Integer, ... asize(Array arr)
%@ Integer atype(Array arr)
%
% Give information about array arr. adim returns the array
% dimension (1, 2 or 3), asize returns the size for each
% dimension and atype returns the data type of the array.
%
define adim()
{
	variable d;

	d = array_info();
	loop(d + 1)
		pop();
	return(d);
}

define asize()
{
	_stk_roll(-array_info() - 1);
	pop();
}

define atype()
{
	loop(array_info())
		pop();
}

%I:matrix
%@ Array matrix(Integer m, Integer n)
%
% Returns a matrix (two-dimensional floating point array)
% with @m@ rows and @n@ columns. All elements are set to zero.
% A row vector is created with @m=1@, a column vector is created
% with @n=1@. Elements are addressed by [@i,j@], where @0<=i<m@
% and @0<=j<n@.
%
define matrix(m, n)
{
	return(create_array('f', m, n, 2));
}

%I:linspace
%@ Array linspace(Float x0, Float x1, Integer n)
%
% Returns a vector of @n@ elements with linearly spaced intervals
% between @x0@ and @x1@.
%
define linspace(x0, x1, n)
{
	variable i;
	variable res = create_array('f', n, 1);
	variable inc = float(x1 - x0) / (n - 1);

	_for(0, n - 1, 1) {
		=i;
		res[i] = x0 + i * inc;
	}
	return(res);
}

%I:logspace
%@ Array logspace(Float x0, Float x1, Integer n)
%
% Returns a vector of @n@ elements with logarithmically spaced intervals
% between 1E@x0@ and 1E@x1@.
%
define logspace(x0, x1, n)
{
	variable i;
	variable res = create_array('f', n, 1);
	variable inc = float(x1 - x0) / (n - 1);

	_for(0, n - 1, 1) {
		=i;
		res[i] = pow(10, x0 + i * inc);
	}
	return(res);
}

%I:transpose
%@ Array transpose(Array mtx)
%
% Returns a transposed matrix of @mtx@. The returned matrix is
% a new one, the elements of @mtx@ are left unchanged.
%
define transpose(mtx)
{
	variable i, j, m, n, res;

	( , m, n, ) = array_info(mtx);
	res = create_array('f', n, m, 2);
	for(i = 0; i < n; i++) {
		for(j = 0; j < m; j++)
			res[i,j] = mtx[j,i];
	}
	return(res);
}

% floor and ceil should be in libslang
define floor(x)
{
	variable rx = int(x);
	if(rx > x)
		rx--;

	return(float(rx));
}

define ceil(x)
{
	variable rx = int(x);
	if(rx < x)
		rx++;

	return(float(rx));
}

%I:help
%@ Void help()
%
% Prints short info on the Vamps interactive mode. See also '?'
define help()
{
	printf("This is Vamps %4.2f, S-Lang version %4.2f \n",version/100.0,_slang_version/100.0,2);
	echo("Graphics options:");
#ifdef AGL
	printf("\tAGL Graphics (%s)\n",AG_IDN,1);
#endif
#ifdef PLOT
	printf("\tGNU libPLOT Graphics (%2.2f) (does _NOT_ work)\n",pl_version,1);
#endif	
#ifndef MSDOS
	printf("\tExternal GNU plotutils Graphics\n",0);
	printf("\tExternal gnuplot interface see ?plt\n",0);
#endif
	echo("\ntype '?' for a list of (documented) commands");
	echo("type '? name' for info on name");
	echo("To start a S-Lang script use '@scriptname'");
	echo("To start a shell command use ':command'");
	echo("To start a run with a prepared input file use:");
	echo("@run or v_run(\"an input file name\")");
	echo("To switch to gnuplot mode use the # char");
}

define vamps_progress ()
{
	fprintf(stderr,".",0);
}

%I:v_run
%@ Void v_run(String inf)
%
% Starts a Vamps run with input file @inf@.
define v_run(inf)
{
	variable sCPU = cpu;

	sreadline = 0;
	v_setinfilename(inf);
	v_prelim();
	v_dorun();
	v_dotail();
}

% Do some work
startup_func();

if (verbose)
	printf("done.\n",0);
#ifdef VAMPS

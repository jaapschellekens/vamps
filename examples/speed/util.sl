#ifdef VAMPS
% General utilities _needed_ by  V A M P S

%I:usergetstr
%@ char *usergetstr(char *prompt);
% 
%  Gets a string from user, resets tty for use in interactive 
%  mode, the global variable userstr_l gives the length of the 
%  user string. see also @Slang_reset_tty@ and @SLang_init_tty@
%
%  If the first character of the string is a @':'@ the remaining
%  part of the string is passed to @system()@;
%
%  If the first character of the string is a @'?'@ the remaining
%  part of the string is passed to @vamps_help()@;
%
%@  If the first character of the string is a '@' the remaining
%  part of the string is passed to @eval()@;
%
%  Returns: string
%
variable userstr_l = 0;

define usergetstr(prompt)
{
	variable str = "|||||";

	SLang_reset_tty;
	while(strcmp(str,"|||||") == 0){
		() = fputs(prompt,stderr);
		userstr_l = fgets(stdin);
		str = strtrim(());

		if(str[0] == ':'){
			() = str_replace(str, ":", "");
			() = system();
			str = "|||||";
		}
		if(str[0] == '@'){
			() = str_replace(str, "@", "");
			eval();
			str = "|||||";
		}
		if(str[0] == '?'){
			() = SLang_init_tty;
			() = str_replace(str, "?", "");
			vamps_help();
			str = "|||||";
			SLang_reset_tty;
		}
	}

	% A hack, try this twice!
	if (SLang_init_tty () != 0)
		() = SLang_init_tty ();

	return str;
}

%I:useropenfile
%@ fp *useropenfile(char *prompt, char mode);
%
% Like @usergetstr@ but checks is the input is a file that can
% be opend in @mode@. A file pointer is returned
define useropenfile(prompt,mode)
{
   variable fp = -1;
   variable infile,n;
   
   if (sreadline)
     SLang_reset_tty;
   while (fp == -1) {
      () = fputs(prompt,stderr);
      n = fgets(stdin);
      infile = strtrim(());
      fp = fopen(infile,mode);
   }
   if (sreadline)
     () = SLang_init_tty;
   return fp;
}

%I:vprof
%@[Array], Int  vprof(String var, Float time, String filename)
%
% Returns -1 on failure or the number of rows
% in the array and the array on success.
%
% vprof reads a profile variable from the Vamps output file
% @filename@ for a single timstep (time) in the simulation.
% Depth is placed in column 1 and the value in column 0.
% See also @vsel@
define vprof (var,time,filename)
{
	variable tt,ttt,prf,z,ts,ret,i;

	tt = getdefar("initial","z",filename,0);
	if (tt <= 1){
		fprintf(stderr,"Variable %s not found in %s\n","z",filename,2);
		fprintf(stderr,"Or %s not an array.\n","z",1);
		return -1;
	}else{
		z = ();
	}
	ts = sprintf("t_%d",time,1);
	ttt = getdefar(ts,var,filename,0);
	if (ttt <= 1){
		fprintf(stderr,"Variable %s (t = %d) not found in %s\n",var,time,filename,3);
		fprintf(stderr,"Or %s not an array.\n",var,1);
		return -1;
	}else{
		prf = ();
	}
	ret = create_array('f', tt, 2, 2);

	for(i = 0;i < tt; i++) {
		ret[i, 1] = z[i];
		ret[i, 0] = prf[i];
	}

	return (ret, tt);
}

%I:vsel
%@ [array], Int vsel (parname, layer, filename)
%
% Returns -1 on failure or the number of rows
% in the array and the array on success.
% If @layer == -1@ then all layers are put into
% the array.
% Example:
%@ variable ar;
%@ if (vsel("volact",-1,"sat.out") != -1){
%@ 	ar = ();
%@ }
% See also the stand-alone @vsel@ program and @vprof@.
define vsel(parname,layer,filename)
{
	variable i,j,k,tt,ar,sect;
	variable steps,ret,retret,cols;

	if (opendef(filename) == 0){
		fprintf(stderr,"Could not open %s\n",filename,1);
		return -1;
	}

	steps = getdefint("initial","steps",0,filename,0);
	if (steps == 0){
		fprintf(stderr,"Something wrong with %s\n",filename,1);
		() = closedef();
		return -1;
	}

	tt = getdefar("t_0",parname,filename,0);
	if (tt == 0){
		() = closedef ();
		fprintf(stderr,"Variable %s not found in %s\n",parname,filename,2);
		return -1;
	}else{
		ar = ();
		if (layer == -1){
			ret = create_array('f', steps, tt + 1, 2);
			cols = tt + 1;
		}else{
			ret = create_array('f', steps, 2, 2);
			cols = 2;
		}
		for (i = 0; i < steps; i++){
			sect = Sprintf("t_%d",i,1);
			ret[i, 0] = getdefdoub(sect,"t",0,filename,0);
			tt = getdefar(sect,parname,filename,0);
			if (tt == 0){
				() = closedef ();
				retret = create_array('f',i, cols,2);
				break;
			}
			ar = ();
			for (j = 1; j <= tt; j++)
				if (layer == -1)
					ret[i, j] = ar[j - 1];
				else if (layer == (j - 1))
					ret [i, 1] = ar[j - 1]
		}
	}
	() = closedef();

	if (i != steps){
		for (j = 0; j < i; j++){
			for (k = 0; k < cols; k++)
				retret[j, k] = ret[j, k];
		}
	return (retret, i + 1);
	}

	return (ret, i + 1);
}

%I:f_save
%@ Void f_save(String var, String filename, String outfilename)
%
% Writes the variable @var@ from the vamps outputfile @filename@ to
% an ascii file @outfilename@. Depending on the nature of the variable
% two ore more columns are created. The time data resides in the
% first column.
define f_save(var, thef, outf)
{
	variable ar;

	if (vsel(var,-1,thef) != -1){
		ar = ();
		% feed to m_write...
		mwrite(ar,outf);
	} else {
		echo("Reading of var failed.");
		return;
	}
}

#else
fputs("This script needs to be run with Vamps\n",stderr);
#endif

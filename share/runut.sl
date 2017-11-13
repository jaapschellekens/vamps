#ifdef VAMPS
% These are mainly for use in interactive mode
variable depth,k,h,theta;

printf("Loading runut.sl...",0);

variable pltcmd = "xplot";
variable grcmd = "graph";

%forward declarations
define savets(tsname,fname,points);


%I:v_show_data_sets
%@ Void v_show_data_sets ();
%
% shows all data set currently loaded
%
% Returns: nothing
%
define v_show_data_sets ()
{
   variable i;
   
   for (i = 0; i < _sets_; i++)
     printf("Set %d = %s read from %s\n",i,getset_name(i),getset_fname(i),3);
}


%I:v_save_all_sets
%@ Void v_save_all_sets ()
%
% save all currently loaded datasets to a file. The
% filenames are created by prepending 'set_' to the name of
% the dataset
%
define v_save_all_sets()
{
   variable i;
   variable nname;
  
   for (i = 0; i < _sets_; i++){
      nname = Sprintf("%s.set",getset_name(i),1);
      () = fputs("Saving: ",stderr);
      () = fputs (nname,stderr);
      () = fputs("\n",stderr);
      v_dumpset(i,nname);
   }
}

define showprof(par)
{
   variable i;

   for (i = 0 ; i < v_layers ; i++){
      printf("%f\t%f\n",depth[i],par[i],2);
   }
}

define saveprof(par,fname)
{
   variable i,fname,sfile,s;

   sfile = fopen(fname,"w");
   for (i = 0 ; i < v_layers ; i++){
      s = Sprintf("%f\t%f\n",depth[i],par[i],2);
      () = fputs(s,sfile);
   }
   return  fclose(sfile);
}


%I:savets
%@ Int savets ("tsname", "fname", points)
%
% saves the time series tsname to the file fname from
% point 0 to point points
%
% Returns: -1 on failure, 0 on sucess
define savets(tsname,fname,points)
{
   variable setnum = -1;
   variable sfile;
   variable i,s;

   setnum = getsetbyname(tsname);
   if (setnum == -1){
      printf ("No such time serie: %s\n",tsname,1);
      return -1;
   }
   
   sfile = fopen(fname,"w");
   s = Sprintf("# Name of this set: %s\n",tsname,1);
   () = fputs(s,sfile);
   for (i = 0 ; i < points ; i++){
      s = Sprintf("%f\t%f\n",getset_x(setnum,i),getset_y(setnum,i),2);
      () = fputs(s,sfile);
   }
   () = fclose(sfile);
   
   return 0;
}

#ifdef UNIX

%I:plotpar
%@ Void plotpar("parname")
%
% calls @vsel@ and @graph@ to plot parameter @parname@ from @outfilename@.
%
define plotpar(parname)
{
	() = system(Sprintf("%s -o- -p%s %s | %s -T%s -Xtime | %s\n","vsel", parname,v_outfilename,"graph",parname,"xplot",6));
}


%I:plotts
%@ Int plotts("tsname");
%
% plots a time serie from the start of the run to
% current time 't' using the current plot settings
%
% Returns: nothing
%
define plotts(tsname)
{
   variable setnum = -1;
   variable s,ss;

   ss = Sprintf("%s%stsplot.tt",tmpdir,pathsap,2);
   if (savets(tsname,ss,v_thisstep) == -1)
     return -1;
   
   s = Sprintf("%s -T%s -Xtime %s | %s",grcmd,tsname,ss,pltcmd,4);
   () = system (s);
   
   return 0;
}
#endif

printf("done.\n",0);
#else
fputs("This script needs to be run with Vamps\n",stderr);
#endif

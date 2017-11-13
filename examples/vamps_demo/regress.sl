#ifdef VAMPS
% Canopy storage (S) can be related to LAI. 
printf("Loading Bisley regress.sl...",0);

define Slai_to_s (lai)
{
   return lai*0.3;
}

% The free throughfall coefficient can also be related to lai
define Slai_to_pfree (lai)
{
   return lai*0.001;
}

% This one relates the water diverted to the trunc to lai
define Slai_to_ptr (lai)
{
   return lai*0.001;
}


% R_incoming to R_net
% Shuttleworth, 1984
define Rs_to_Rn (Rs)
{
variable netrad;

    netrad = (Rs * 0.858) - 35.0;

    if (netrad >= 0.0)
    	return netrad;
    else
    	return 0.0;
}


% This one estimates rs from several parameters
define estrs()
{
   variable A,P,U,T;
   variable daypart = 1;
   variable result = -1.0;
   variable daytime = 1;
   variable min = 8.0;
  
   A = getset_y(id.nra,v_thisstep);
   P = getset_y(id.pre, v_thisstep);
   U = getset_y(id.win,v_thisstep);
   T = getset_y(id.tem,v_thisstep);   
      
   if (A < 0.0)
     A = 0.0;
   
   daypart = v_t - float(int(v_t));
   
   % 1700
   if (daypart > 0.708)
     daytime = 0;
   
   % 0800
   if (daypart < 0.333)
     daytime = 0;
   
   if (P == 0.0){
   if (daytime == 1)
	result = exp(-0.014 - 0.003 * A + 0.394 * v_vpd + 0.151 * T - 0.418 * U);	
    else
    	result = 0.0;
   }else {
       result = 0.0;
   }

   v_printstr("rs",sprintf("%f",result,1));
   return result;
}
printf("done.\n",0);
#else
fputs("This script needs to be run with Vamps\n",stderr);
#endif


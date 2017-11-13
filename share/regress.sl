#ifdef VAMPS
% Canopy storage (S) can be related to LAI. 
printf("Loading regress.sl...",0);

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
% It is a rather complex example from a Fiji pine forest
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
      
   % Rain !
   if (P > 0.0)
     return 0.0;
   
   if (A < 0.0)
     A = 0.0;
   
   daypart = v_t - float(int(v_t));
   
   % 1700
   if (daypart > 0.708)
     daytime = 0;
   
   % 0800
   if (daypart < 0.333)
     daytime = 0;
   
   if ((daytime == 1)  and (A > 150.0))
     result = 28.89 - (0.0684 * A) + (4.27 * v_vpd) + (1.06 * v_smd);
   else
     result = 219.2 - (3.33 * A) - (2.125 * v_vpd) - (32.706 * U) + (18.09 * T) + (9.87 * v_smd);
   
   % High radation at night
   if ((A >= 150.0) and (daytime == 0))
     result = 28.89 - (0.0684 * A) + (4.27 * v_vpd) + (1.06 * v_smd);
   
   if ((result < 40.0)  and (A < 150.0))
     result = 28.89 - (0.0684 * A) + (4.27 * v_vpd) + (1.06 * v_smd);


   return 1.39 * result;
}
printf("done.\n",0);
#else
fputs("This script needs to be run with Vamps\n",stderr);
#endif


% This file contains all the S-Lang functions needed to ad
% a new method for theta/head/k relations to Vamps. In this
% case the clapp/hornberger (Campbell 1974) method is added.
% This method is also present in Vamps in a C version which
% allows for testing.

% The following functions should be defined:
% getspars	- get the parameters from the input file
% h2dmc		- convert sunction head to diff moisture cont.
% t2k		- convert theta to k_unsat
% t2h		- convert theta to suction head
% h2t		- convert suction head to theta

variable MAXSUCKHEAD = -1.0E20;
variable b,n, psisat;

% Lets be verbose on loading ...
echo("Loading S-Lang Clapp/Hornberger functions...");

%Igetspars
%@ Void getspars(section, nr)
%
% Gets the parameters from the input file
define getspars(section,nr)
{
	% Get the parameters b en psisat (clapp/hornberger) from
	% the soiltype section in the input file
	% In this case we don't use the nr variable which tells us
	% which soiltype structure this soil belongs to

	b = float(getdefstr(section,"b",0.0,v_infilename,1));
	n = 2.0 + 3.0/b;
	psisat = float(getdefstr(section,"psisat",0.0,v_infilename,1));
}

%Ih2dmc
%@ Float h2dmc (nr, head)
%
define  h2dmc (nr, head)
{
	variable t,tt;

	if (head >= -1.0E-1){
		return 0.0;
	}
	else{
		tt = v_getspar(nr,"thetas")/(-b * psisat);
		t = pow(head/psisat, (-1.0 * (1.0/b)) - 1.0);
		return (t * tt);
	}
}

%It2k
%@ Float t2k (nr, wcon)
%
define  t2k (nr, wcon)
{
	variable relsat;

	relsat = (wcon - v_getspar(nr,"residual_water")) / (v_getspar(nr,"thetas") - v_getspar(nr,"residual_water"));
	if (relsat > 1.0)
		relsat = 1.0;

	if (relsat < 0.001)
		return 1.0E-10;
	else{
		return  v_getspar(nr,"ksat") * pow(wcon/v_getspar(nr,"thetas"),b * n);
	}
}

%It2h
%@ Float t2h(nr, wcon, depth)
%
define  t2h (nr, wcon, depth)
{
  variable ans;

  if ((v_getspar(nr,"thetas") - wcon) < 1.0E-5)
	  return abs(depth);
  else{
	  if ((wcon - v_getspar(nr,"residual_water")) < 1.0E-5){
		  return MAXSUCKHEAD * (1.0 - (wcon - v_getspar(nr,"residual_water")));
	  }
	  ans =  psisat * pow(wcon/v_getspar(nr,"thetas"),-1.0 * b);
	  if ( ans < MAXSUCKHEAD) 
		  return MAXSUCKHEAD * (1.0 - (wcon - v_getspar(nr,"residual_water")));
	  else
		  return ans;
  }
}

%Iht2
%@ Float h2t(nr, head)
%
define  h2t (nr, head)
{

	if (head >= (-1.0 * v_getspar(nr,"psisat")))
		return v_getspar(nr,"thetas");
	else
		return v_getspar(nr,"thetas") * pow(head/psisat,-1.0 / b);
}

define pre_top ()
{
	echo("topsys.sl --------------------------------------------");
	echo("Experimental topsystem  written in S-Lang in which all");
	echo("fluxes are pre-calculated.");
	echo("This should give the same results as system 4");
	echo("-------------------------------------------------------");
	id.spe = readset(getdefstr("ts","spe","ll",v_infilename,1),"spe");
	id.inr = readset(getdefstr("ts","inr","ll",v_infilename,1),"inr");
	id.ptr = readset(getdefstr("ts","ptr","ll",v_infilename,1),"ptr");
}

define post_top()
{
% Add clean up stuff here...
}

%I:tstep_top
%@tstep_top(tstepje)
% Calculates soilevaporation, potential transpiration, interception
% and precipitation for each step at @tstepje@.
%
% Return order should be:
%	return(soilevap, pot-trans, intercep, precipitation);
%
define tstep_top(tstepje)
{
	return (getset_y(id.spe,tstepje), getset_y(id.ptr,tstepje),
	getset_y(id.inr,tstepje), getset_y(id.pre,tstepje));
}

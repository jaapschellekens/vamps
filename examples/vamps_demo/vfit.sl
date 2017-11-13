% Example S-Lang code for using the marquardt fitting routine
variable mes,i,tt = 0.0;
variable calls = 0;
variable orig_var = getdefdoub("canopy","rs",0,"fiji.inp",1);

define each_step (){};
define at_end (){plt("plot @avgtheta:fiji.out@, @mes@ with points"); };
define at_start (){calls++;};
define vamps_progress () 
{
	variable strs = "-\\|/";
	variable len = strlen(strs);
	variable i,tp;

	tp =  v_thisstep - (int(v_thisstep/len) * len);
	fprintf(stderr,"%c\r",strs[tp],1);
};

define fitf(par, m, n)
{
	variable i,j,tt;
	variable pt = 0;
	variable modd;


	if (par[1] < 0.0) {
		return 1;	% return false
	}

	sreadline = 0;
	v_setinfilename("fiji.inp");

	tt = string( par[1]);
   	addvar("_override_","canopy","rs",tt,1);
	v_prelim(); % after this the input file should be read

	v_dorun();
	v_dotail();

	(modd, ) = vsel("avgtheta",-1,"fiji.out"); % get calculated values from outfile

	% BIG klugde to allow for less measurd points than generated	
	for (i = 0; i < m; i++) {
		for (j = 0; j < v_steps; j++)
			if (mes[i, 0] ==  modd[j, 0]){
				rv[i + 1] =  (mes[i, 1] - modd[j, 1]);
				pt++;
			} 
	}

	return 0; % Success
}


variable mes = mread("meant.prn",-1); % measured data mean theta
variable mm, nn = 1;

verbose = 3;
verbose = 0;
rinmem("fiji.inp");

(mm, ) = asize(mes);
maq_verb = 2;
echo(mm);
if  (maqinit(mm,nn) >= 0){
	par[1] = orig_var;
	%inp[4] = inp[4]  * 0.1;
	%inp[3] = inp[3]  * 0.1;

	if (maq_verb)
		maqhead;
	() = maqrun();
	if (maq_verb)
		maqtail;
	maqend(mm);
}else{
	echo("maqinit failed");
}

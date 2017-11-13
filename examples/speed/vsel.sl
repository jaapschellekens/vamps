variable arg;
variable narg = 0;
variable var,inf,outf,res;

var = getarg();
inf = getarg();
outf = getarg();


if (strcmp(outf, "--") == 0) {
	echo("usage: vsel var infile outfile");
	exit;
}


variable pt = vsel(var,-1,inf);

if (pt > 0) {
	res = ();
	mwrite(res,outf);
}else{
	printf("Var %s not found in %s\n",var,inf);
}

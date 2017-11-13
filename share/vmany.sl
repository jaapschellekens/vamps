% Gets _MANY_ vars from a vamps output file
variable arg;
variable narg = 0;
variable var,inf,outf,res;

inf = getarg();


if (strcmp(inf, "--") == 0) {
	echo("usage: infile");
	exit;
}

echo("reading ile in mem...\r");
rinmem(inf);

echo("reading theta...\n");
variable pt = vsel("theta",-1,inf);
res = ();
mwrite(res,"_theta.mat");
echo("writing theta...\n");
variable pt = vsel("h",-1,inf);
res = ();
mwrite(res,"_h.mat");
variable pt = vsel("k",-1,inf);
res = ();
mwrite(res,"_k.mat");

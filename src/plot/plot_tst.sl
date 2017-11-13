%pl_type = 2;
pl_output_is_ascii = 1;
() = pl_init_f();

pl_outfile("ii");
() = pl_openpl;

() = pl_fspace(0,0,10.0,10.0);


variable i;

for (i = 0 ; i < 100; i++){
	() = pl_fline(0,0,i/10.0,10.0);
}

() = pl_closepl;

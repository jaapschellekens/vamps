% Example S-Lang code for using the marquardt fitting routine
variable mydat,i;

%define fitf(double par[],int mmm, int nnn)
define fitf(par, m, n)
{
 variable i;
  
  for (i = 0; i < maq_m; i++)
   rv[i + 1] = mydat[i, 1] - ((mydat[i, 0] * par[1])+(par[2]));

  return 0; % Success
}


variable mm = 2000, nn = 2;

if  (maqinit(mm,nn) >= 0){
	mydat = create_array('f', maq_m, maq_n, 2);
	for (i = 0; i < maq_m; i++){
		mydat[i, 0] = float(i);
		mydat[i, 1] = float(i);
	}
	par[1] = 4.0;
	par[2] = 4000.5;
	inp[3] = 0.00001 ;
	inp[4] =  0.00001;

	maqhead;
	() = maqrun();
	maqtail;
	maqend(mm);
}else{
	fputs("maqinit failed",stderr);
}

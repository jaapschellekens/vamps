% Load high level plotting functions if not already done
plot;

%I:getsoilmat
%@ Array getsoilmat(String fn, Int layer)
%
% construcs a matrix of the soil moisture retension/k_unsat curves
% in a Vamps output file @fn@ for layer @layer@. Head is converted 
% to pf. A layersX4 matrix is returned containing theta, k, theta, 
% pf.
%
define getsoilmat(fn, layer)
{
	variable t2kx,t2ky,t2hx,t2hy;
	variable nn,i,ret;
	
	nn = Sprintf("t2k_%d_x",layer,1);
	() = getdefar("initial",nn,fn,0);
	t2kx = ();
	nn = Sprintf("t2k_%d_y",layer,1);
	() = getdefar("initial",nn,fn,0);
	t2ky = ();
	nn = Sprintf("t2h_%d_x",layer,1);
	() = getdefar("initial",nn,fn,0);
	t2hx = ();
	nn = Sprintf("t2h_%d_y",layer,1);
	() = getdefar("initial",nn,fn,0);
	t2hy = ();

	ret = create_array('f', asize(t2kx), 4, 2);

	for (i = 0; i < asize(t2kx); i++){
		ret[i, 0] = t2kx[i];
		ret[i, 1] = t2ky[i];
		ret[i, 2] = t2hx[i];
		ret[i, 3] = log10(-t2hy[i]);
	}

	return ret;
}

%I:detnrsoilcurves
%@ Void detnrsoilcurves(String fn)
%
% Return the number of soil moisture retension curves in file @fn@
define detnrsoilcurves(fn)
{
	variable nn,i,ret;
	
	i = 0;
	do {
		nn = Sprintf("t2k_%d_x",i,1);
		ret = getdefar("initial",nn,fn,0);
		if (ret > 0)
			pop;
		i++;	
	}while(ret > 0)
	i--;

	return i;
}

variable _smat;

%I:reten_plot
%@ Void reten_plot(String fn, Int layer)
%
% Plots the soil moisture retension plot and the k_unsat curve
% for layer @layer@ in Vamps outputfile @fn@.
define reten_plot(fn,layer)
{
	_smat = getsoilmat(fn,layer);

	subplot(2,1,1);
	plt("set nologscale");
	plt("set xlabel \'theta\'");
	plt("set ylabel \'pf\'");
	plt("plot @_smat@ using 3:4");
	subplot(2,1,2);
	plt("set ylabel \'k unsat\'");
	plt("set logscale y");
	plt("plot @_smat@ using 1:2");
	endsubplot;
}

define allsoilplots(fn)
{
	variable i;
	variable _nrplot = detnrsoilcurves(fn);

	for (i = 0; i < _nrplot; i++){
		plt("set nokey");
		reten_plot(fn, i);
		fprintf(stderr,"plot of layer: %d\n",i,1);
		pause(-1);
	}
}

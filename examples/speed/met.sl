%F:met.sl
% Assorted meteorological functions to estimate evapo(trans)piration.
% This functionality and more can also be found in libmet (in the
% met.lib directory.
% Most come from the Vamps model and have been translated from Pascal
% (provided by Arnoud F) to C to S-Lang
% A matrix wrapper should be added as well as better documentation
%
% Jaap Schellekens


%I:eaes
%@ Float,Float eaes(td,rh)
%
% Determines saturation vapour pressure (ea) and actual vapour presssure 
% (ea) from relative humidity and dry bulb temperature. es is calculated
% according to Bringfeld 1986. Relative humidity should be in % and
% dry bulb temp in degrees Celcius. 
% Example:
%
%@ variable ea,es;
%@
%@ (ea, es) = eaes(20.0,76);
%@ printf("ea = %f es = %f\n",ea,es,2);
%
% Returns: ea, es
define eaes(td,rh)
{
	variable es, ea, tkel;

	tkel = td + 273.15;     % T in degrees Kelvin

% es according to Bringfelt 1986
        es = exp(log(10.0) * (10.79574 * (1 - 273.15 / (tkel)) -
			      5.028 * log((tkel) / 273.15) / log(10.0) +
			      1.50475e-4 * (1 - exp(log(10.0) * -8.2969 *
                  ((tkel) / 273.15 - 1))) + 0.42873e-3 * exp(
              log(10.0) * 4.76955 * (1 - 273.15 / (tkel))) + 0.78614));
% now calculate ea
	ea = rh * es / 100.0;

	return (ea, es);
}


%I:vslope
%@ Float vslope(Td,es);
%
% Calculates the slope of the saturation vapour pressure 
% curve from es and dry bulb temp for use in Penman Eo and
% Penman-Montheith.  
% Required input: Td (dry bulb temp [oC]), es
%
% Returns: slope of the vapour pressure curve
define vslope(Td,es)
{
	variable TEMP = Td + 273.15;     % T in  Kelvin
	variable slope;

  % Slope in mb
  	slope = es * (10.79574 / 0.4343 * (273.15 / (TEMP * TEMP)) -
	5.028 / (TEMP) + 1.50475e-4 * 8.2969 / (0.4343 * 0.4343) /
	    273.15 * exp(log(10.0) * 8.2969 * (1 - (TEMP) / 273.15)) +
	0.42873e-3 * 4.76955 / (0.4343 * 0.4343) * 273.15 / (TEMP * TEMP) *
	  exp(log(10.0) * 4.76955 * (1 - 273.15 / (TEMP))));
	
 	return slope;
}

%I:gamma
%@ Float gamma(td)
%
% Calculates the psychometric constant ([$mbar/{}^o K$])
% It assumes an air pressure of 998mb. Cp is estimated
% at 1005.0 J/kgK
%
% Returns: psychrometric constant
define gamma(td)
{
	variable p, cp, TEMP,l,gamma;
	
  	p = 998.0;  		% pressure in mb
  	cp = 1005.0;   		% J/(kgK)
  	TEMP = td + 273.15;	% T in degrees Kelvin
  	l = 2.501 - 0.00237 * td;   		% L=J/kg
	
	% mbC
	% From waterloo spreadsheet */
	% *Cp = 0.24 *4.1855*(1+0.8*(0.622* *Ea)/(p-(*Ea)));
  	gamma = cp * p / (0.622 * l);   % mbC*/
	return gamma;
}

%I:e0a
%@ Float e0a(Td,Rs,sunratio,u,Ea,Es,Slope,Gamma,L)
%
% Returns: e0 in mm/day (i think)
%The determination of $E_a$
%is described in \cite{calder1990284}:
%\begin{equation}
%E_a = 2.6  (e_s - e_a)  (1 + 0.537 u)
%\end{equation}
%\begin{where}
%\Index{$e_a$}	&	actual vapour pressure [mbar]\\
%\Index{$e_s$}	&	vapour pressure at saturation [mbar]\\
%\Index{$u$}	&	mean daily wind-speed at 2m\\
%\end{where}
define e0a(Td, Rs,sunratio,u,Ea,Es,Slope,Gamma,L)
{
  variable sigma, r, Eaero, T4,Rl,Rnet,RadTerm,AerTerm;

  Ea = Ea * 0.1;
  Es = Es * 0.1;   % From mbar to kPa*/
  Slope = Slope * 0.1;   
  Gamma = Gamma * 0.1; 
  r = 0.06;  % R=0.06 is open water albedo 
  sigma = 5.67e-8;   % Sigma is Stefan-Boltzman constant (W/m2K4)
  T4 = exp(log(Td + 273.15) * 4);
  Rl = 86400 * sigma * T4 * (0.56 - 0.248 * sqrt(Ea)) * (0.1 + 0.9 * sunratio) / L;
  % Rl is de langgolvige stralings component*/
  % 86400 is sec. a day*/
  Rnet = Rs * 86400 * (1 - r) / L - Rl;   % Rs as mean daily W/m2*/
  Eaero = 2.6 * (Es - Ea) * (1 + 0.537 * u);
  % Calder, 1990; Evaporation in the Uplands*/
  % u=MeanDailyWindrun (m/s); Es=MeanDaily sat.vap.pres. (kPa);*/
  % Ea=MeanDaily actual vap.pres. (kPa)*/
  RadTerm = Slope * Rnet / (Slope + Gamma);
  AerTerm = Gamma * Eaero / (Slope + Gamma);
  Rl = Rl * L / 86400;
  % Slope=*/
  return (Slope * Rnet + Gamma * Eaero) / (Slope + Gamma);
}

%I:makkink
%@ Float makkink(rad,Slope,Gamma)
%
% calculates reference evaporation according to Makkink
% The C1 constant is taken as 0.65 and C2 0.0
%
% Returns: makkink reference evaporation in mm/day
define makkink(rad,Slope,Gamma)
{
	variable C1 = 0.65;
	variable C2 = 0.0;
	% 0.1 vanwege mbar naar kpa */
	% 1000 vanwege m naam mm */
 	return 1000 * C1 * (Slope)/(Slope + (Gamma * 0.1)) * rad + C2;
}

%I:e0b
%@ Float e0b(Td,Rs,Rsout,Rnet,u,Ea,Es,Slope,Gamma,L)
%
% Calculates penman open water evaporation using net radiation.
% Use the @delgam@ function to get Gamma, Slope, and L
%
% Returns: e0 in mm/day (i think)
define e0b(Td, Rs, Rsout, Rnet, u, Ea, Es, Slope, Gamma, L)
{
  variable Rnetopen, r, Eaero,Rl;

  	Ea = Ea * 0.10;
  	Es = Ea * 0.10;   	% Van mbar naar kPa*/
  	Slope = Slope * 0.10; 	% Van mbar naar kPa*/
  	Gamma = Gamma * 0.10;  
  	r = 0.06;   	% R=0.06 is albedo for open water
  	Rl = Rs - Rnet - Rsout;
  	Rnetopen = (Rs * (1 - r) - Rl) * 86400 / L;

% Calder, 1990; Evaporation in the Uplands
  	Eaero = 2.6 * (Es - Ea) * (1 + 0.537 * u);
  	return (Slope * Rnetopen + Gamma * Eaero) / (Slope + Gamma);
}


%I:ra
%@ Float ra (z, z0, d, u)
%
%	Calculates ra (aerodynamic resistance) according to:
%@	ra = [ln((z-d)/z0)]^2 /(k^2u)
%@	Where:
%@		d  = displacement height
%@		z  = height of the wind speed measurements
%@		u  = windspeed 
%@		z0 = roughness length
%@		k  = von Karman constant
%
% Returns: ra
define ra (z, z0, d, u)
{
	variable k2 = 0.1681;		% (von Karman constant)^2
	variable tt;

	tt = log ((z - d) / z0);

	return ((tt * tt) / k2)/u;
}


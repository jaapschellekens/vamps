% Initialize the fitting system
% Due to a `BUG' in S-Lang's array handling C- arrays that
% are added cannot be resized. A workaround is to first
% run maqinit() the large values for n and m as to ensure that you
% have enough working space for several fit runs

define fitinit {};
define fit {};
% mm = maximal number of points
% nn = max number of parameters
variable mm = 3000, nn = 4;
() = maqinit(mm,nn);

% Free some memory, use maqinit again with proper values of mm and nn
% to start fitting. See fit.sl for an example
maqend(mm);

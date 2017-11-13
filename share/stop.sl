#ifdef VAMPS
% The stop function should check for conditions that require user
% interaction, a full stop, or severe warnings. It can be empty
% as well, but some check is recommend. This default stop function
% checks for a large error in mass balance.

% forward declarations
define onstop();

define stop()
{
   variable maxerr = 3.0;
   % Switch to interactive mode is the masbal error get to largeL
   if (abs(v_masbal) > maxerr ){
      printf("\a\a\a\a\a\a\a\a\a\a",0);
      printf("Warning, mass balance > %f, check input\n",maxerr,1);
      printf("Switching to interactive mode, check input and vars..\n",0);
      printf("If you want to disable this check type 'define stop () {};'\n",0);
      printf("at the interactive prompt followed by 'cont;' and a blank line.\n",0);
      switchtoint = 1;
      % Do some housekeeping at crash
      onstop();
   }
}

define onstop()
{
}
#ifdef VAMPS

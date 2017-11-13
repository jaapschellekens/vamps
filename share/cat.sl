%I:cat
%@Void cat(inf, outf)
%
% Copies the file @inf@ to @outf@ appending it if
% @outf@ already exists. @cat@ only works with ascii
% files.
define cat(inf, outf)
{
	variable fpi, fpo, ol;

	fpi = fopen(inf,"r");
	fpo = fopen(outf,"a");

	if (fpi != -1)
		if (fpo != -1){
			while(fgets(fpi) > 0) {
				ol = ();
				() = fputs(ol,fpo);
			}
		}
	() = fclose(fpo);
	() = fclose(fpi);
}

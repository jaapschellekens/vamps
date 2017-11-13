define sec(file, section)
{
variable fd, s, len,v, insec = 0;

fd = fopen(file,"r");
while(fgets(fd) > 0) {
	s = strtrim();
	if (is_substr(s,"[") == 1){
		insec = 0;
		%printf("new section %s\n",s,1);
	}
	if(strcmp(section,s) == 0){
		%echo("found:");
		insec = 1;
	}
	if (insec == 1){
	 	v = strv(s, "=");
		if (asize(v) > 1)
			printf("%s\n", v[0], 1);
	}
}
() = fclose(fd);
}

%sec("run5.day.out","[t_0]");

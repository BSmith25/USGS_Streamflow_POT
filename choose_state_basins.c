#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** NAME
**            chose_state_basins.c
**
** SYNOPSIS
**            change_state_basins.exe
**
**            Compiles with:
**            cc change_state_basins.c  -o change_state_basins.exe
** DESCRIPTION
	this program takes arranged state gage ID data, filters to streamflow data with at least 3 years of observations,  and organizes it into pre- and post-2007 record files
	note that USGS uses "water years" which span October-September, not January to December
	input files:	state.txt data file for all gages in state, columns are: stnid typecode #records startyear startmonth startday endyear endmonth endday
			state_names.txt a file of all station names
	output files:	statepre2007_out.txt output file of all streamflow gages with records starting before 10/1/2007, columns are: stnid typecode #records startyear startmonth startday endyear endmonth endday
			statepre2007_names_out.txt names of stations in statepre2007_out.txt 
			statepost2007_out.txt output file of all streamflow gages with records ending after 10/1/2007, columns are: stnid typecode #records startyear startmonth startday endyear endmonth endday
			statepost2007_names_out.txt names of stations in statepost2007_out.txt 
**************************************************/


void main(int argc, char **argv)

{
	FILE  *datafile,*namefile,*outfilea, *outfileb, *outfile2a, *outfile2b;
	int  recno, syear, smonth, sday, eyear, emonth, eday;
	char type[5], sname[75], stn[10];

        /**** open state data file ****/
        if((datafile=fopen("state.txt","r")) == NULL)  {
            printf("cannot open state data file \n");
              exit(1);    }

        /**** open state name file ****/
        if((namefile=fopen("state_names.txt","r")) == NULL)  {
            printf("cannot state names file \n");
              exit(1);    }

	/**** open output file a ****/
 	if((outfilea=fopen("statepre2007_out.txt","w")) == NULL)  {
   		printf("cannot open output file a \n");
   		exit(1);    }

	/**** open output file b ****/
 	if((outfileb=fopen("statepost2007_out.txt","w")) == NULL)  {
   		printf("cannot open output file b \n");
   		exit(1);    }

	/**** open output file 2a ****/
 	if((outfile2a=fopen("statepre2007_names_out.txt","w")) == NULL)  {
   		printf("cannot open names output file a \n");
   		exit(1);    }

	/**** open output file 2b ****/
 	if((outfile2b=fopen("statepost2007_names_out.txt","w")) == NULL)  {
   		printf("cannot open names output file b \n");
   		exit(1);    }
        
        /**** Find stations with discharge data (code 00060) and at least 3 years of data, cut down list and separate into pre and post 2007 water year files ****/
        while((fscanf(datafile,"%s %s %d %d %d %d %d %d %d \n", stn, type, &recno, &syear, &smonth, &sday, &eyear, &emonth, &eday) != EOF)) {
	fscanf(namefile,"%s \n",sname);
        	if (strncmp(type,"00060",5)==0 && eyear-syear >= 3) {
			/**** pre-2007 water year ****/
			if( syear < 2007 | (syear==2007 && smonth <= 9)) {
				fprintf(outfilea,"%s \t %s \t %d \t %d \t %d \t %d \t %d \t %d \t %d \n", stn, type, recno, syear, smonth, sday, eyear, emonth, eday);
				fprintf(outfile2a,"%s \n",sname);
			}
			/**** post-2007 water year ****/
			if ( eyear > 2007 | (eyear==2007 && emonth >= 10)) {
				fprintf(outfileb,"%s \t %s \t %d \t %d \t %d \t %d \t %d \t %d \t %d \n", stn, type, recno, syear, smonth, sday, eyear, emonth, eday);
				fprintf(outfile2b,"%s \n",sname);
			}
		}/*end of if*/
	} /*end of while*/
    fclose(outfilea);
    fclose(outfileb);
    fclose(datafile);
    fclose(outfile2a);
    fclose(outfile2b);
    fclose(namefile);
} /*end of main*/
	

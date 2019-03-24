#include <stdio.h>
#include <stdlib.h>

/*
** NAME
**            anpeaktest.c
**
** SYNOPSIS
**            anpeaktest.exe
**
**            Compiles with:
**            cc anpeaktest.c  -o anpeaktest.exe
** DESCRIPTION
	this program reads annual peak data and decides if the basin is
	worth downloading dischargve values for (if a minimum of 1/2 of the annual peaks exceed the threshold)
	input files: 	jj1 contains the area of the watershed
			a11 contains the annual peaks for the streamflow gage
	output files:	b11 contains a "yes" or "no" for if the full record should be downloaded
**************************************************/

void main(int argc, char **argv)
{

    FILE *areafile,*outfile,*infile;
    float thr,thr_km;
    float peakval;
    float barea;
    float peak_num, total_num;

	thr_km = 1.; /* the threshold for flooding in cm/km^2 */


/* set and open area file */
 	if((areafile=fopen("jj1","r")) == NULL)  {
    		printf("cannot open areafile \n");
      		exit(1);    }

/* convert threshold to cfs using the area */
	fscanf(areafile,"%f ",&barea);
	printf("area: %f",barea);
	thr = thr_km * barea * 2.58999 * 35.3147;
	printf("threshold: %f\n",thr);

/* set and open infile */
 	if((infile=fopen("a11","r")) == NULL)  {
    		printf("cannot open  streamflow file \n");
      		exit(1);    }

/* open output file */
  	if((outfile=fopen("b11","w")) == NULL)  {
    		printf("cannot open output file \n");
      		exit(1);    }

/* Check the number of annual peaks over the threshold */
	peak_num = 0;
	total_num=0;

	while (!feof(infile)) {
		fscanf(infile,"%f",&peakval);
		if (peakval > thr)
			{ peak_num++; }
		total_num++;
	}

/* Print yes or no to the input file (yes if over half of annual peaks exceed the threshold) */	
	if (peak_num>0) {
		if (total_num/peak_num<=2 && total_num>2)
			fprintf(outfile,"%s \n","yes");
		else 
			fprintf(outfile,"%s \n","no"); 
	}
	else	
	fprintf(outfile,"%s \n","no"); 
	

    fclose(infile);
    fclose(outfile);
    fclose(areafile);
}

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
** NAME
**            peaks.c
**
** SYNOPSIS
**            peaks.exe
**
**            Compiles with:
**            cc peaks.c  -o peaks.exe
** DESCRIPTION
	this program reads 1 streamflow minute data and pulls out peaks with
  	a 'thr' minute window above a 'time_thr' value threshold then saves basins with
 	a number of peaks per year over the specifid minimum thresholds
	input files: 	hhh the 1 minute streamflow data file, columns are: stnid year month day daysince1960 hour minute streamflowcfsvalue
			jj1 area of the watershed
			yyy length of the streamflow record in years
	output files: 	kkk information for peaks over threshold values with date/time of the max value, columns are: year month day daysince1960 hour minute streamflowcfsvalue
			zzz "yes" or "no" for if the stream data is worth saving, as determined by the min_ppy (minimum peaks over threshold per year) 
**************************************************/


void main(int argc, char **argv)


{
	FILE  *infile,*outfile,*areafile,*lengthfile,*flashyfile;
    	float thr,pval,time_thr,thr_km;
    	float area, min_ppy, len_yr;
    	double ppy;
    	int mdpt;
    	char stnid[20];
    	int i,j,k,vind,ptime,pind,pk_count;
    	int month[1500];
    	float year[1500],day[1500],dayofr[1500],hour[1500], minute[1500];
    	float value[1500];

/**********************************************************************/
/***  set threshold values ***/
	time_thr = 721.;  /* time threshold in minutes */
	thr_km = 1.; /* streamflow threshold in cm/km^2 */
	pk_count = 0; /* initialize # of peaks */
	min_ppy = 2; /* minimum number of peaks per year to save basin data */
/**********************************************************************/

/* set and open 1 minute list file */
 	if((infile=fopen("hhh","r")) == NULL)  {
    		printf("cannot open  file \n");
      		exit(1);    }

/* set and open area file */
 	if((areafile=fopen("jj1","r")) == NULL)  {
    		printf("cannot open  file \n");
      		exit(1);    }

/* set and open length of record in years file */
 	if((lengthfile=fopen("yyy","r")) == NULL)  {
    		printf("cannot open  file \n");
      		exit(1);    }

/* open output file */
  	if((outfile=fopen("kkk","w")) == NULL)  {
    		printf("cannot open output file \n");
      		exit(1);    }

/* open flashy output file */
  	if((flashyfile=fopen("zzz","w")) == NULL)  {
    		printf("cannot open output file \n");
      		exit(1);    }


/*** convert threshold to cfs ***/
	fscanf(areafile,"%f ",&area);
	printf("area: %f",area);
	thr = thr_km * area * 2.58999 * 35.3147;

/*** assign starting values ***/
	mdpt = floor(time_thr/2); /* midpoint of time threshold */ 
	printf("threshold values %f \n",thr);
	pval = thr;
	pind = 0;

/*** set up array of first time_thr window in the file and find max streamflow if it is over thr ***/
	for(i=0;i<time_thr;i++) /* create time_thr array */
  		fscanf(infile,"%s %f %d %f %f %f %f %f ",stnid,&year[i],&month[i],&day[i],&dayofr[i],&hour[i],&minute[i],&value[i]);
	for(i=0;i<time_thr;i++){ /* increment through */
  		if(value[i]> pval) {
     		pval = value[i]; /* max value */
     		pind = i; /* index of max value */
	}	}

	if((pind == mdpt))  { /* only save the peak if it is the midpoint of the window */
    		fprintf(outfile,"%4.0f %d %f %g %f %f %f\n", year[pind],month[pind],day[pind],dayofr[pind],hour[pind],minute[pind],value[pind]);
    		pk_count++;
	}

/*** increment through streamflow file ***/
	while (!feof(infile)) {

/*** move the window to the next time slot ***/
		for(i=0;i<time_thr-1;i++) { /* update array by moving up a timestep */
   			year[i]=year[i+1];
   			month[i]=month[i+1];
   			day[i] =day[i+1];
   			dayofr[i] =dayofr[i+1];
   			hour[i] = hour[i+1];
   			minute[i] = minute[i+1];
   			value[i] = value[i+1];
		} /* close time_thr array for loop */
		
		j=time_thr-1;

		fscanf(infile,"%s %f %d %f %f %f %f %f",stnid,&year[j],&month[j],&day[j],&dayofr[j],&hour[j],&minute[j],&value[j]); /*get new timetep value*/
		pval = thr;
		pind = 0;
		for(i=0;i<time_thr;i++){ /* find max streamflow of new time_thr window if it is over thr*/
  			if(value[i]> pval) { 
     				pval = value[i];
     				pind = i;
		} 	} /* close if and for loop for new time_thr timestep */
   

  		if((pind == mdpt)) { /* only save the peak if it is the midpoint of the window */
   			fprintf(outfile,"%4.0f %d %2.0f %g %g %g %g\n", year[pind],month[pind],day[pind],dayofr[pind],hour[pind],minute[pind],value[pind]);
   			pk_count++;
		}
	} /* closes while loop */


/* check if more than min_ppy of peaks per year */
	fscanf(lengthfile,"%f ",&len_yr);
	if (pk_count>0 && pk_count/len_yr>=min_ppy && len_yr>3)
		fprintf(flashyfile,"%s\n","yes");
	else
		fprintf(flashyfile,"%s\n","no");

	fprintf(outfile,"# of peaks per year = %f", pk_count/len_yr);

    	fclose(infile);
    	fclose(outfile);
    	fclose(areafile);
    	fclose(flashyfile);

}/* end of main */



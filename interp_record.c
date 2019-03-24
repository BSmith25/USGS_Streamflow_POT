#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
** NAME
**            interp_record.c
**
** SYNOPSIS
**            interp_record.exe
**
**            Compiles with:
**            cc interp_record.c  -o interp_record.exe
** DESCRIPTION
	this program reads USGS Instantaneous Data Archive unit values file for a station
       	for a long term (multi-year) record nd interpolates onto 1 a minute time scale
	input files: 	ggg streamflow file for one gage, columns are: stnid year month day hour minute cfsstreamflowvalue
	output files: 	hhh 1 minute interpolated streamflow, columns are: stnid year month day dayfrom1960 hour minute cfsstreamflowvalue
			yyy length of missing data (over 90 days) periods in years
**************************************************/


int main()


{
	FILE  *infile,*outfile,*lengthfile;
	char finput[20],stnid[20],junk1[300];
	int tint,i,j,k;
	float day0,hour0,minute0,value0,jday0;
	float day,hour,minute,value,jday,day1;
	int jtime0,jtime, year,year0,month,month0,jtimea;
	double miss_yr,len_yr;
	float  vint;
	int ind1,ind2,ind3;
	float pday,phour,pminute;
	int firstday[13],dayyear[54][13],nyears;
	int firstdayofyear[11],pyear,pdoy,pmonth,mdoy;
	int daynoleap[]= {1,32,60,91,121,152,182,213,244,274,305,335,365};
	int dayleap[] = {1,32,61,92,122,153,183,214,245,275,306,336,366};
	int leapyear[] = {1960,1964,1968,1972,1976,1980,1984,1988,1992,1996,2000,2004,2008,2012,2016,2020};
	miss_yr=0.;
	len_yr=1.;

/*** open output file ***/
	if((outfile=fopen("hhh","w")) == NULL)  {
   		printf("cannot open output file \n");
   		exit(1);    }

/*** open length of record in years file ***/
 	if((lengthfile=fopen("yyy","w")) == NULL)  {
   		printf("cannot open output file \n");
   		exit(1);    }

/***** open input file ******/

  	if((infile=fopen("ggg","r")) == NULL)  {
    		printf("cannot open input file \n");
       		exit(1);    }

/*** create array for days of the year, accounting for leap years***/
/*** year 1 = 1960 in the leapyear array ***/
  	nyears = 59;
  	for(i=1960;i<2020;i++){
   		for(j=0;j<13;j++)
       			dayyear[i-1960][j] = daynoleap[j];
    		for(k=0;k<16;k++)
     			if(i==leapyear[k])
      				for(j=0;j<13;j++)
       					dayyear[i-1960][j] = dayleap[j];
    	}

/*** first day of year for each of the years ***/
  /* index 0 = 1960 */
  	firstdayofyear[0] = 1;  /* 1960*/
  	firstdayofyear[1] = firstdayofyear[0] + dayleap[12]; /* 1961 */
  	firstdayofyear[2] = firstdayofyear[1] + daynoleap[12]; /* 1962 */
  	firstdayofyear[3] = firstdayofyear[2] + daynoleap[12]; /* 1963 */
  	firstdayofyear[4] = firstdayofyear[3] + daynoleap[12]; /* 1964 */
	firstdayofyear[5] = firstdayofyear[4] + dayleap[12]; /* 1965 */
  	firstdayofyear[6] = firstdayofyear[5] + daynoleap[12]; /* 1966 */
  	firstdayofyear[7] = firstdayofyear[6] + daynoleap[12]; /* 1967 */
  	firstdayofyear[8] = firstdayofyear[7] + daynoleap[12]; /* 1968 */
  	firstdayofyear[9] = firstdayofyear[8] + dayleap[12]; /* 1969 */
  	firstdayofyear[10] = firstdayofyear[9] + daynoleap[12]; /* 1970 */
  	firstdayofyear[11] = firstdayofyear[10] + daynoleap[12]; /* 1971 */
  	firstdayofyear[12] = firstdayofyear[11] + daynoleap[12]; /* 1972 */
  	firstdayofyear[13] = firstdayofyear[12] + dayleap[12]; /* 1973 */
  	firstdayofyear[14] = firstdayofyear[13] + daynoleap[12]; /* 1974 */
  	firstdayofyear[15] = firstdayofyear[14] + daynoleap[12]; /* 1975 */
  	firstdayofyear[16] = firstdayofyear[15] + daynoleap[12]; /* 1976 */
  	firstdayofyear[17] = firstdayofyear[16] + dayleap[12]; /* 1977 */
  	firstdayofyear[18] = firstdayofyear[17] + daynoleap[12]; /* 1978 */
  	firstdayofyear[19] = firstdayofyear[18] + daynoleap[12]; /* 1979 */
  	firstdayofyear[20] = firstdayofyear[19] + daynoleap[12]; /* 1980 */
  	firstdayofyear[21] = firstdayofyear[20] + dayleap[12]; /* 1981 */
  	firstdayofyear[22] = firstdayofyear[21] + daynoleap[12]; /* 1982 */
  	firstdayofyear[23] = firstdayofyear[22] + daynoleap[12]; /* 1983 */
  	firstdayofyear[24] = firstdayofyear[23] + daynoleap[12]; /* 1984 */
  	firstdayofyear[25] = firstdayofyear[24] + dayleap[12]; /* 1985 */
  	firstdayofyear[26] = firstdayofyear[25] + daynoleap[12]; /* 1986 */
  	firstdayofyear[27] = firstdayofyear[26] + daynoleap[12]; /* 1987 */
  	firstdayofyear[28] = firstdayofyear[27] + daynoleap[12]; /* 1988 */
  	firstdayofyear[29] = firstdayofyear[28] + dayleap[12]; /* 1989 */
  	firstdayofyear[30] = firstdayofyear[29] + daynoleap[12]; /* 1990 */  
  	firstdayofyear[31] = firstdayofyear[30] + daynoleap[12]; /* 1991 */
  	firstdayofyear[32] = firstdayofyear[31] + daynoleap[12]; /* 1992 */
  	firstdayofyear[33] = firstdayofyear[32] + dayleap[12]; /* 1993 */
  	firstdayofyear[34] = firstdayofyear[33] + daynoleap[12]; /* 1994 */
  	firstdayofyear[35] = firstdayofyear[34] + daynoleap[12]; /* 1995 */
  	firstdayofyear[36] = firstdayofyear[35] + daynoleap[12]; /* 1996 */
  	firstdayofyear[37] = firstdayofyear[36] + dayleap[12]; /* 1997 */
  	firstdayofyear[38] = firstdayofyear[37] + daynoleap[12]; /* 1998 */
  	firstdayofyear[39] = firstdayofyear[38] + daynoleap[12]; /* 1999 */
  	firstdayofyear[40] = firstdayofyear[39] + daynoleap[12]; /* 2000 */  
  	firstdayofyear[41] = firstdayofyear[40] + dayleap[12]; /* 2001 */
  	firstdayofyear[42] = firstdayofyear[41] + daynoleap[12]; /* 2002 */
  	firstdayofyear[43] = firstdayofyear[42] + daynoleap[12]; /* 2003 */
  	firstdayofyear[44] = firstdayofyear[43] + daynoleap[12]; /* 2004 */
  	firstdayofyear[45] = firstdayofyear[44] + dayleap[12]; /* 2005 */
  	firstdayofyear[46] = firstdayofyear[45] + daynoleap[12]; /* 2006 */
  	firstdayofyear[47] = firstdayofyear[46] + daynoleap[12]; /* 2007 */
  	firstdayofyear[48] = firstdayofyear[47] + daynoleap[12]; /* 2008 */
  	firstdayofyear[49] = firstdayofyear[48] + dayleap[12]; /* 2009 */
  	firstdayofyear[50] = firstdayofyear[49] + daynoleap[12]; /*2010*/
  	firstdayofyear[51] = firstdayofyear[50] + daynoleap[12]; /* 2011 */
  	firstdayofyear[52] = firstdayofyear[51] + daynoleap[12]; /* 2012 */
  	firstdayofyear[53] = firstdayofyear[52] + dayleap[12]; /* 2013 */
  	firstdayofyear[54] = firstdayofyear[53] + daynoleap[12]; /* 2014 */
  	firstdayofyear[55] = firstdayofyear[54] + daynoleap[12]; /* 2015 */
  	firstdayofyear[56] = firstdayofyear[55] + daynoleap[12]; /* 2016 */
  	firstdayofyear[57] = firstdayofyear[56] + dayleap[12]; /* 2017 */
  	firstdayofyear[58] = firstdayofyear[57] + daynoleap[12]; /* 2018 */
  	firstdayofyear[59] = firstdayofyear[58] + daynoleap[12]; /* 2019 */
	

/*** read first line of input file and create values for the first line***/
	fscanf(infile,"%s %d %d %f %f %f %f", stnid,&year0,&month0,&day0,&hour0,&minute0,&value0);

	day1 = firstdayofyear[0]; 	/* first day of 1960 */
	jday0 = firstdayofyear[year0-1960] + daynoleap[month0-1]-1 + day0; 	/* first day of dataset as days from 1960 */
	for(k=0;k<16;k++)  	/* check for leap year */
  		if(year0==leapyear[k]) jday0 = firstdayofyear[year0-1960] + dayleap[month0-1]-1 + day0;
	jtime0 = (jday0 - day1)*1440. + hour0*60. + minute0; 	/* first day of dataset in minutes from 1960 */
	jtimea=jtime0;

	fprintf(outfile,"%s %d %4d %4.0f %4.0f %3.0f %3.0f %.2f\n",stnid,year0,month0,day0,jday0,hour0,minute0,value0);

/*** increment through the file and interpolate for the intervals ***/
	while (!feof(infile)) {
    		fscanf(infile,"%s %d %d %f %f %f %f", stnid, &year,&month,&day,&hour,&minute,&value);
    		jday = firstdayofyear[year-1960] + daynoleap[month-1]-1 + day; /* day of this reading from 1960 */
    		for(k=0;k<16;k++) /* check for leap year */
    			if(year==leapyear[k]) jday = firstdayofyear[year-1960] + dayleap[month-1]-1 + day;
    		jtime = (jday - day1)*1440. + hour*60. + minute;
    		tint = jtime-jtime0; /* minute of reading from start of this window */
    		
		if(tint > 129600.) { /* if interval (space from last reading) is over 90 days, add it to "missing years of data" */
          		printf("%s %d %d %f %f %f %f\n", stnid,year0,month0,day0,hour0,minute0,value0);
          		printf("%s %d %d %f %f %f %f\n \n \n", stnid,year,month,day,hour,minute,value);
	  		miss_yr = miss_yr + tint/1440./365.;
                     } /* close if loop */

    		vint = value-value0; /* difference in streamflow values over the interval */

      		for (i=0;i<tint;i++){ /* interpolate interval down to 1 minute */
       			jtime = jtime0 + (i+1);
       			value = value0 + (i+1)*(vint/tint);
       			pday = floor(jtime/1440.);

       			for(k=0;k<nyears;k++) { /* check if year has increased */
        			if(pday >= firstdayofyear[k] && pday < firstdayofyear[k+1]) {
             				pyear = k + 1960;
             				pdoy = pday - firstdayofyear[k]+1;
   			}	} /* close if and for loop for 1 minute incrementing */

        		for(j=0;j<12;j++){ /* check if month has increased */
        			if(pdoy >= dayyear[pyear-1960][j] && pdoy < dayyear[pyear-1960][j+1]) { 
        				pmonth = j+1;
        				mdoy = pdoy - dayyear[pyear-1960][j] +1;
   			}	} /* close if and for loop for month check */

       			phour = floor((jtime/1440.-pday)*24.);
       			pminute = (((jtime/1440.-pday)*24.)- phour)*60.; 
          		if(pminute<0.) pminute = 0.;

       			fprintf(outfile,"%s %d %4d %4d %d %3.0f %3.0f %.2f \n",stnid,pyear,pmonth,mdoy,jtime/1440+1,phour,pminute,value);
		}  /*  closes minute for loop */

   		year0 = year; /* Increment through data by moving end values to start values */
   		month0 = month;
   		day0 = day;
   		hour0 = hour;
   		minute0 = minute;
   		value0 = value;
   		jday0 = firstdayofyear[year0-1960] + daynoleap[month0-1]-1 + day0;
   		for(k=0;k<16;k++)
   			if(year0==leapyear[k]) jday0 = firstdayofyear[year0-1960] + dayleap[month0-1]-1 + day0;
   				jtime0 = (jday0 - day1)*1440. + hour0*60. + minute0;
   		
  		len_yr = (jtime - jtimea)/1440./365. - miss_yr; /* calculate length of time for which there is data (excluding when there are over 90 day gaps) */
	}  /*  closes infile while loop */


  	fprintf(lengthfile,"%f \n",len_yr);
  	fclose(outfile);
  	fclose(infile);
  	fclose(lengthfile);
}/* end of main */



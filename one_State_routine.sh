# This script retrieves the basin ID numbers of all USGS gages in a state with discharge data
# then organizes them into a list of pre- and post- 2007 basin info then downloads this data 
# from a list of gage number and dates, finds the Peaks Over Threshold within that data
# and decides if a station exceeds a number of peaks per year of record (2 x 1 cms/sq. km) it saves 
# the station peak data and 1 minute interpolated discharge data, the program deafult is to run for  
# Alabama, "al" and the state can be updates in line 23 as the 2 letter state abbreviation "stateCd=__"
# the Threshold can be changed in peaks.c

# This program was written at a time when pre-2007 USGS data was available through a different site (IDA) than
# post-2007 data, however all data are now available through waterservices.usgs.gov, the sites have been updated, 
# but the pre and post 2007 separation remains.

# Requirements:
# the tool wget is used here, so you should be using a machine with
# RedHat Linux or cygwin with wget

# Create folders for the saved peak and discharge files
mkdir Flashy_ones
mkdir Flashy_ones/peaks
mkdir Flashy_ones/discharge

# Download site numbers for a state
wget  -q "http://waterservices.usgs.gov/nwis/site/?format=rdb,1.0&stateCd=al&outputDataTypeCd=iv,id&parameterCd=00060&hasDataTypeCd=iv,id"  --referer="http://waterservices.usgs.gov/" -O ttt 

# Organize the data into a names file and data file
grep "USGS" ttt > tt1
cut -f 2 tt1 > stn
cut -f 3 tt1 > tt3
cut -f 14 tt1 > type
cut -f 22 tt1 > startd
cut -c 1-4 startd > syear
cut -c 6-7 startd > smonth
cut -c 9-10 startd > sday
cut -f 23 tt1 > endd
cut -c 1-4 endd > eyear
cut -c 6-7 endd > emonth
cut -c 9-10 endd > eday
cut -f 24 tt1 > recno

paste stn type recno syear smonth sday eyear emonth eday > state.txt
sed -e 's/ /_/g' tt3 > state_names.txt

# Pick out the right data types and Organize into pre- and post- 2007 files
cc choose_state_basins.c -o choose_state_basins.exe
./choose_state_basins.exe

# make sure c files are compiled
cc interp_record.c -o interp_record.exe
cc peaks.c -o peaks.exe
cc anpeaktest.c -o anpeaktest.exe

# make copies of the files to mess up
cp statepre2007_out.txt aaa
cp statepost2007_out.txt bbb

# Find the station number and dates from the pre-2007 file and download the data
cat aaa | while read -r f1 f2 f3 f4 f5 f6 f7 f8 f9; do

# download area of basin, only cotinue if the area exists
	wget -q "http://waterservices.usgs.gov/nwis/site/?format=rdb&sites=$f1&siteOutput=expanded"  --referer="http://waterservices.usgs.gov/" -O jjj
	grep $f1 jjj | cut -f 30 > jj1
	if [ -s jj1 ]
		then 	

# Read annual peak data to see if values should be downloaded
		wget -q "http://nwis.waterdata.usgs.gov/nwis/peak?site_no=$f1&agency_cd=USGS&format=rdb"  --referer="nwis.waterdata.usgs.gov/nwis" -O anpeak

		grep $f1 anpeak | sed '1d' > abc
		cut -f 5 abc > a11
		./anpeaktest.exe
	
# if there are large enough annual peaks then download discharge values
		if grep -q "yes" b11
		then

# Download all discharge values data
	          	wget -q "http://waterservices.usgs.gov/nwis/iv/?format=rdb&sites=$f1&startDT=1960-01-01&endDT=2007-09-30&parameterCd=00060"  --referer="http://waterservices.usgs.gov/" -O ccc
	          	grep $f1 ccc | sed '1,2d'  > cc1
	          
          # Check for a doubled 10/01/2007 time period at the end of the data
	       	   		if grep -q "20071001000000" cc1
	          		then
	          			sed -i '$ d' cc1
	          		fi
          
	          	cut -f 3 cc1 > date
	          	cut -c1-4 date > year
	          	cut -c6-7 date > month
	          	cut -c9-10 date > day
	          	cut -c12-13 date > hour
	          	cut -c15-16 date > minute
	          	cut -f2 cc1 > stn
	          	cut -f5 cc1 > value
			if [ ! -s value ]
   			then 	
 				echo $f1
      				echo " ida is empty."
       			fi
          # Save this file as cc3 temporarily
	         	paste -d" " stn year month day hour minute value > cc3
          
          # Check the post-2007 data for the station number, if it is there download it
	          	if grep -q $f1 bbb
	          	then
	          		wget -q "http://waterservices.usgs.gov/nwis/iv/?format=rdb&sites=$f1&startDT=2007-10-01&parameterCd=00060"  --referer="http://waterservices.usgs.gov/" -O ddd
	               			grep "USGS" ddd | sed '1,2d' > dd1
	          			cut -f 3 dd1 > date2
	          			cut -c 1-4 date2 > year2
	          			cut -c 6-7 date2 > month2
	          			cut -c 9-10 date2 > day2
	          			cut -c 12-13 date2 > hour2
	          			cut -c 15-16 date2 > minute2
	          			cut -f 2 dd1 > stn2
	          			cut -f5  dd1 > value2
	          			if [ ! -s value2 ]
	          			then 	
	          				echo $f1
	          				echo " data is empty."
	          			fi
          
          # Save this file as dd3 temporarily
	          		paste -d" " stn2 year2 month2 day2 hour2 minute2 value2 > dd3
          
          
          # Concatenate the two files into the station file
	          		cat cc3 dd3 > ggg
          	
          # If there is no post-2007 data create the station file
	          	else
	          		cp cc3 ggg
	          	fi
          
          # interpret data to one minute
	          	./interp_record.exe
          
          # check number of peaks over 1 cms/km^2
	          	./peaks.exe
          
          # if there are enough peaks per year, save the file
	          	if grep -q "yes" zzz
	          	then 
	          		cp kkk Flashy_ones/peaks/$f1.txt
	          		cp hhh Flashy_ones/discharge/$f1.txt
	          	fi
		fi
	else
		echo $f1
		echo " area is empty"
	fi
done

# Check for stations that have only post-2007 data
cat bbb | while read -r f1 f2 f3 f4 f5 f6 f7 f8 f9; do
	if grep -q -ci $f1 aaa 
	then
		echo "done"
	else
		
# download area of basin and continue if area exists
		wget -q "http://waterservices.usgs.gov/nwis/site/?format=rdb&sites=$f1&siteOutput=expanded"  --referer="http://waterservices.usgs.gov/" -O jjj
		grep $f1 jjj | cut -f 30 > jj1

	if [ -s jj1 ]
		then 

# Read annual peak data to see if values should be downloaded
			wget -q "http://nwis.waterdata.usgs.gov/nwis/peak?site_no=$f1&agency_cd=USGS&format=rdb"  --referer="nwis.waterdata.usgs.gov/nwis" -O anpeak

			grep $f1 anpeak | sed '1d' > abc
			cut -f 5 abc > a11
			./anpeaktest.exe

# if there are large enough annual peaks then download discharge values
			if grep -q "yes" b11
			then
				wget -q "http://waterservices.usgs.gov/nwis/iv/?format=rdb&sites=$f1&startDT=2007-10-01&parameterCd=00060"  --referer="http://waterservices.usgs.gov/" -O ddd
           				grep "USGS" ddd | sed '1,2d' > dd1
      					cut -f 3 dd1 > date2
      					cut -c 1-4 date2 > year2
      					cut -c 6-7 date2 > month2
      					cut -c 9-10 date2 > day2
      					cut -c 12-13 date2 > hour2
      					cut -c 15-16 date2 > minute2
      					cut -f 2 dd1 > stn2
      					cut -f 5  dd1 > value2
      					if [ ! -s value2 ]
      					then 	
      						echo $f1
      						echo " second half data is empty."
      					fi
      	
        # Save this file as ggg 
     	 			paste -d" " stn2 year2 month2 day2 hour2 minute2 value2 > ggg
      	# interpret data to one minute
      				./interp_record.exe
      
      # check number of peaks over 1 cms/km^2
      				./peaks.exe
      	
      # if there are enough peaks per year, save the file
      				if grep -q "yes" zzz
      				then
      					cp kkk Flashy_ones/peaks/$f1.txt
      					cp hhh Flashy_ones/discharge/$f1.txt
      				fi
      			fi
		
	else
		echo $f1 
		echo " area is empty"
	fi
fi
done

# remove all un-needed files
rm a11 aaa abc anpeak b11 bbb cc1 cc3 ccc date date2 day day2 dd1 dd3 ddd eday emonth endd eyear ggg hhh hour hour2 jj1 jjj kkk minute minute2 month month2 recno sday smonth startd stn stn2 syear tt1 tt3 ttt type value value2 year year2 yyy zzz














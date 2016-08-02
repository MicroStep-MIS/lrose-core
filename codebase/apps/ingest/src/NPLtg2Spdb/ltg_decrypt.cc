// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ltg_decrypt:  Read the file, deconvolute, and put to Spdb
//
//  Original from David H(arper?). RAP/NCAR 
//        
//  Hacked to clip out "Bad Data" which was noticed in the data stream Spring 2004.
//  Rejects based on nonsense time stamps and amplitudes. - F. Hage.  NCAR/RAP
//
#include <stdio.h>
#include <time.h>
#include <rapformats/ltg.h>
#include <toolsa/umisc.h>
#include <Spdb/DsSpdb.hh>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Params.hh"

#define LINE_LENGTH 1 
#define UNIX_TIME_DIFF 320630400
#define LON_CONST 0.001068115234
#define LAT_CONST 0.001281738

int ltg_decrypt(char *in_file, Params *P)
{
char           data_buffer[2], 
               buffer[LINE_LENGTH];

int            x = 0, 
               i = 0,
               flash_count = 0,
               start_of_data = 0, 
               multiplicity, 
               duration, 
               kiloamps, 
               quality_flag;
float          latitude, 
               longitude;

unsigned short date[4], 
               prevByte = 0,
               byte0 = 0, 
               byte1 = 0,
               days = 0, 
               hours, 
               mins, 
               secs,
               flash_type;

unsigned long  position = 0;

time_t         calc_time, minTime, maxTime;
long           numProcessed = 0L;


int data_valid = 1;

FILE           *source_fd = NULL;

  if ((source_fd = fopen(in_file, "r")) == NULL)
  {
    fprintf (stderr,"Could not open input file %s\n",in_file);
    return (1);
  }

  minTime =0L; maxTime =0L;

  // Init the SPDB data
  DsSpdb OutSpdb;
  OutSpdb.clearPutChunks();
  OutSpdb.clearUrls();
  OutSpdb.addUrl(P->OutputUrl);

  /* Get the current time */
  time_t now = time(0);

  /* Get rid of the file Header */
  for(x=1;x<=30;x++) { fgets(buffer, 1, source_fd); }

  /* Read lightning data. */
  while (fgets(buffer, 2, source_fd) != NULL)
  {
     byte0 = 0;
     memcpy((void *)&byte0, buffer, 1);

     if ((0x96 == byte0) || (0x97 == byte0))
     {

        // skip 0x96/7 if the first byte of the file after the header
        // as legitimate 0x96/7 bytes are preceeded by a by a flash/count
        // byte
        //
        if (start_of_data == 0) { continue; }

        else 
        { 
           flash_count = 0; flash_type = 0;
           flash_type = byte0; flash_count = prevByte;
        }

        // Clear out prev timestamp  & decrypt timestamp 
        memset((void*)date, 0, sizeof(date[0])*4);

        // Extract date
        for (x=0;x<4;x++)
        {
           fgets(buffer, 2, source_fd);
           byte0 = 0;
           memcpy((void *)&byte0, buffer, 1);
           date[3 - x] = byte0;
        }

        // Deconvolute Date
        memset((void*)data_buffer, 0, sizeof(data_buffer[0])*2);
        memcpy((void *)&data_buffer[0], &date[1], 1);
        memcpy((void *)&data_buffer[1], &date[0], 1);
        memcpy((void *)&byte0, data_buffer, 2);
        days = byte0 >> 1;
        hours = byte0 << 15;
        hours = hours >> 11;
        byte0 = 0;
        memcpy((void *)&byte0, &date[2], 1);
        hours = (byte0 >> 4) ^ hours;
        mins = (byte0 << 4) & 255;
        mins = mins >> 2;
        byte0 = 0;
        memcpy((void *)&byte0, &date[3], 1);
        mins = mins | (byte0 >> 6);
        secs = byte0 & 63;

        // if (P->Debug) fprintf(stderr,"%d days, %d hours, %d minutes, %d secs\n",days,hours,mins,secs);

        // Reject data > current time. 
        // Reject data more than one hour old when in real-time mode.
        //
        calc_time = (((((days * 24) + hours) * 60) + mins) * 60) + secs;
        calc_time += UNIX_TIME_DIFF;
        if ((calc_time > (now + 120)) || 
           ((P->Mode == Params::REALTIME) && (calc_time < (now - P->MaxRealtimeFileTimeDiff))))
        {
	  //
	  // This generally means corrupt input. It's best to sleep here
	  // for a second to avoid hogging the system.
	  //
	  // Niles Oien March 2006.
	  //
	  sleep(1);
	  //
           position = ftell(source_fd);
           position -= 5;
           if ((fseek(source_fd, position, SEEK_SET)) ||
	       ((P->Mode == Params::REALTIME) && (calc_time < (now - P->MaxRealtimeFileTimeDiff))))
           {
              /* I don't think this can happen, but just in case. */
              printf ("Error in time stamp resolution.\n");
              if (source_fd) fclose(source_fd);
              return -1;
           }
           continue;
        }
        else
        {
           // we have a good timestamp, so now we process all of the
           // flashes for that timestamp
           //
           for(x=1; x<=flash_count; x++)
           {
              // this is the old/original message type
              // 
              if (0x96 == flash_type)
              {
                 byte0 = 0;
                 fgets(buffer, 2, source_fd);
                 memcpy((void *)&byte0, buffer, 1);
                 byte1 = 0;
                 fgets(buffer, 2, source_fd);
                 memcpy((void *)&byte1, buffer, 1);
                 byte0 = (byte1 << 8) | byte0;
                 longitude = (byte0 * LON_CONST) - 130;

                 byte0 = 0;
                 fgets(buffer, 2, source_fd);
                 memcpy((void *)&byte0, buffer, 1);
                 byte1 = 0;
                 fgets(buffer, 2, source_fd);
                 memcpy((void *)&byte1, buffer, 1);
                 byte0 = (byte1 << 8) | byte0;
                 quality_flag = byte0 >> 15;
                 byte0 = byte0 & 32767;
                 latitude = (byte0 * LAT_CONST) + 18;

                 byte0 = 0;
                 fgets(buffer, 2, source_fd);
                 memcpy((void *)&byte0, buffer, 1);
                 kiloamps = (byte0 & 127)*2;
                 if (byte0 >> 7)
                 kiloamps = -kiloamps;

                 byte0 = 0;
                 fgets(buffer, 2, source_fd);
                 memcpy((void *)&byte0, buffer, 1);
                 duration = (byte0 >> 4);
                 multiplicity = byte0 & 15;

              } // end of 0x96 message processing

              else
              {  // a type 0x97 message - skip these for now 
                 // Note: the decryption algorithm for this message
                 // can be found in ltgDecrypt.c
                 // 
                 latitude = 0;
                 longitude = 0;
                 kiloamps = 0;
                 quality_flag = 0;
                 multiplicity = 0;
                 duration = 0;
                 for (i=1; i<=8; i++)  // these messages are 8 bytes long
                 {  
                    fgets(buffer, 2, source_fd);
                 }
              } // end of 0x97 message processing

              // If Strike is within our bounding box - Add it to the database
              //
              if( (longitude > P->MinLon) && (longitude < P->MaxLon) &&
                  (latitude > P->MinLat) && (latitude < P->MaxLat) )
              { 

                 // Turn off output after the first  0 amplitude
                 // Seems to indicate  start of bad data.
                 //
                 if(kiloamps == 0) data_valid = 0;

                 if(data_valid) 
                 {
                    LTG_strike_t strk;
                    strk.type =  LTG_TYPE_UNKNOWN;
                    strk.time = calc_time;

		    //
		    // Keep some records for the info file, if we're creating that.
		    //
		    numProcessed++;
		    if ((minTime ==0L) && (maxTime ==0L)){
		      minTime = calc_time; maxTime = calc_time;
		    } else {
		      if (calc_time < minTime) minTime = calc_time;
		      if (calc_time > maxTime) maxTime = calc_time;
		    }

                    strk.longitude = longitude;
                    strk.latitude = latitude;
                    strk.amplitude = kiloamps;

                    if (P->Debug)
                    {
                       fprintf(stderr, "Strike: Latlon: %g, %g , %d - %s",
                       strk.latitude, strk.longitude, strk.amplitude,
                       ctime((time_t *) &calc_time));
                    }

                   // Assign ID's based on Location 
                   int dataType, dataType2;
                   dataType = (int) rint( (strk.latitude + 90.0) * 100.0);
                   dataType2 = (int) rint( (strk.longitude + 180.0) * 100.0);

                   LTG_to_BE( &strk );

                   OutSpdb.addPutChunk( dataType,calc_time, 
                       calc_time + P->ExpireSecs, sizeof(strk),  
                       (void *) &strk, dataType2);
                
                 } // end of data_valid

              } // Is within the bounding box 

           } // for each flash message
   
        } /* if this was a valid timestamp */ 

     } /* if this was a flash message */ 

     // if this wasn't a flash message, store the byte and go on
     //
     prevByte = 0;
     prevByte = byte0;
     start_of_data = 1;

  }/* While there is still data */

  // Commit the data to the database, close the file, and exit
  //
  OutSpdb.put( SPDB_LTG_ID, SPDB_LTG_LABEL);

  if (source_fd) fclose(source_fd);
   
  if ((P->createInfoFile) && (numProcessed > 0)){

    FILE *fp = fopen(P->infoFilename,"a");
    if (fp == NULL){
      fprintf(stderr,"Failed to open %s\n", P->infoFilename);
    } else {
      struct stat buf;
      if (0==stat(in_file, &buf)){
	fprintf(fp, "File %s processed at %s:\n", in_file, utimstr(time(NULL)));
	fprintf(fp, "     ctime : %s\n", utimstr(buf.st_ctime));
	fprintf(fp, "     mtime : %s\n", utimstr(buf.st_mtime));
	fprintf(fp, "     atime : %s\n", utimstr(buf.st_atime));
	fprintf(fp, "     size (bytes) : %ld\n", long(buf.st_size));
	fprintf(fp, "     Data times ran from %s to %s\n",
		utimstr(minTime), utimstr(maxTime));
	fprintf(fp, "     %ld points processed.\n\n",
		numProcessed);
      }
      fclose(fp);
    }

  }


  return 0;
}

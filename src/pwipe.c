/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 **************************************************************************/

/***************************************************************************
 *   ROM 2.4 is copyright 1993-1998 Russ Taylor                            *
 *   ROM has been brought to you by the ROM consortium                     *
 *       Russ Taylor (rtaylor@hypercube.org)                               *
 *       Gabrielle Taylor (gtaylor@hypercube.org)                          *
 *       Brian Moore (zump@rom.org)                                        *
 *   By using this code, you have agreed to follow the terms of the        *
 *   ROM license, in the file Rom24/doc/rom.license                        *
 **************************************************************************/

/***************************************************************************
*    Written by trent for Forgotten Valor (valor.kyndig.com port 3666)	   *
***************************************************************************/

#define NOCRYPT
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
/*#include <unistd.h>*/
#include "merc.h"
#include "recycle.h"

void archive( char*, char*, int );

void wipe_pfiles()
{
    DIR *dp;
    struct dirent *ep;
    char letter ;
    char dir[MAX_STRING_LENGTH];
    
    for( letter = 'a'; letter <= 'z' ; letter++ )
    {     
    	sprintf( dir , "%s%c/", PLAYER_DIR, letter );
    	dp = opendir (dir);
     
    	if (dp != NULL)
    	{
            while ( (ep = readdir (dp)) )
            {
            	if( ep->d_name[0] != '.' )
		    archive( dir, ep->d_name, PFILE_EXPIRE );
            }	
            (void) closedir (dp);
    	} 
    	else
	    bugf( "Could not open %s.", dir );
    }
}        

void archive( char* dir, char* name , int days)
{
    char old_file[MAX_STRING_LENGTH];
    char new_file[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    struct stat fst;
    
    sprintf( old_file, "%s%s" , dir, name );
    
    if ( stat( old_file, &fst ) != -1 )
    {
    	if( difftime( current_time , fst.st_mtime ) > days*60*60*24 )
	{
	    /* logf( "Archiving %s" , name );*/
	    sprintf( new_file, "%s%s" , ARCHIVE_DIR, capitalize(name) );
  	    rename( old_file, new_file );
	    sprintf( buf, "gzip -fq %s", new_file );
	    system( buf );
	}
    }
}    

void remove_inactive( char* dir , int days)
{
    DIR *dp;
    struct dirent *ep;
    struct stat fst;
    char file[MAX_STRING_LENGTH];
    
    dp = opendir (dir);
                     
    if (dp != NULL)
    {
    	while ( (ep = readdir (dp)) )
    	{
    	   if( ep->d_name[0] != '.' )
    	   {
		sprintf( file, "%s%s", dir , ep->d_name );
    		if( (stat( file , &fst ) != -1)
    		  && difftime( current_time , fst.st_mtime ) > days*60*60*24 ) 
		{
	     		/* logf( "Removing %s", ep->d_name ); */
    	      		unlink(file);
   	     	}
    	   }
    	}
        (void) closedir (dp);
    }
    else
    	bugf( "Could not open %s.", dir );
}

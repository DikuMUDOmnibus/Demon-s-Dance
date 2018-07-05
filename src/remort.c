/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
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
*       ROM 2.4 is copyright 1993-1996 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/
/****************************************************************************
*	Demon's Dance MUD, and source code are property of Eric Goetschalckx	*
*	By compiling this code, you agree to include the following in your		*
*	login screen:															*
*	    Derivative of Demon's Dance, by Enzo/Stan							*
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#elif defined(WIN32)
  #include <sys/types.h>
  #include <winsock.h>
  #include <io.h>
  #include <direct.h>
  #define NOCRYPT
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "interp.h"
#include "magic.h"
#include "arena.h"

void do_remor( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REMORT, you must spell it out.\n\r", ch );
    return;
}

void do_remort( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char strsave[MAX_INPUT_LENGTH], player_name[MAX_INPUT_LENGTH];
    char player_pwd[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
    int player_incarnations;

    if ( IS_NPC(ch) || ( d = ch->desc ) == NULL )
	return;

    if ( ch->level < LEVEL_HERO && !IS_SET( ch->act, PLR_REMORT ) )
    {
	sprintf( buf,
	    "You must be level %d to remort\n\r",
	    LEVEL_HERO );
	send_to_char( buf, ch );
	return;
    }

	if( ch->pcdata->incarnations >= 1)
	{
		send_to_char( "You have already reached the second tear of existance, no more can be achieved\n\r", ch );
		return;
	}


    if ( ch->pcdata->confirm_remort )
    {
	if ( argument[0] != '\0' )
	{
	    send_to_char( "Remort status removed.\n\r", ch );
	    ch->pcdata->confirm_remort = FALSE;
	    return;
	}
	else
	{
	    /*
	     * Get ready to delete the pfile, send a nice informational message.
	     */
	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    stop_fighting( ch, TRUE );
	    send_to_char( "You have chosen to remort.  You will now be dropped in at the race\n\r", ch );
	    send_to_char( "selection section of character creation, and will be allowed to choose from\n\r", ch );
	    send_to_char( "a wider selection of races and classes.\n\r\n\r", ch );
	    send_to_char( "In the unlikely event that you are disconnected or the MUD\n\r", ch );
	    send_to_char( "crashes while you are creating your character, create a new character\n\r", ch );
	    send_to_char( "as normal and write a note to 'immortal'.\n\r", ch );
	    send_to_char( "\n\r[Hit Enter to Continue]\n\r", ch );
	    wiznet( "$N has remorted.", ch, NULL, 0, 0, 0 );

	    /*
	     * I quote:
	     * "After extract_char the ch is no longer valid!"
		*/
	    sprintf( player_name, "%s", capitalize( ch->name ) );
	    sprintf( player_pwd, "%s", ch->pcdata->pwd );
		do_function(ch, &do_remove, "all" );
	    player_incarnations = ++ch->pcdata->incarnations;
		save_remort_char(ch);
	    extract_char( ch, TRUE );

	    /*
	     * Delete the pfile, but don't boot the character.
	     * Instead, do a load_char_obj to get a new ch,
	     * saving the password, and the incarnations.  Then,
	     * set the PLR_REMORT bit and drop the player in at
	     * CON_BEGIN_REMORT.
	     */
	    unlink( strsave );
		sprintf(buf, "%sremort", player_name);
	    load_char_obj( d, buf );
		save_char_obj( ch );
		unlink(  buf );
	    if( !IS_SET( ch->act, PLR_REMORT ) )
		SET_BIT( ch->act, PLR_REMORT );
	    d->connected = CON_BEGIN_REMORT;
	    return;
	}
    }

    if ( argument[0] != '\0' )
    {
	send_to_char( "Just type remort.  No argument.\n\r", ch );
	return;
    }

    send_to_char("Type remort again to confirm this command.\n\r", ch );
    send_to_char("WARNING: This command is irreversible.\n\r", ch );
    send_to_char("Typing remort with an argument will undo remort status.\n\r",
	ch );
    ch->pcdata->confirm_remort = TRUE;
    wiznet( "$N is contemplating remorting.",ch,NULL,0,0,get_trust(ch));
}


void save_remort_char( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
	char strsave[MAX_INPUT_LENGTH];
    FILE *fp;

    if ( IS_NPC(ch) )
	return;

    if ( ch->desc != NULL && ch->desc->original != NULL )
	ch = ch->desc->original;

    /* create god log */
    if (IS_IMMORTAL(ch) || ch->level >= LEVEL_IMMORTAL)
    {
	send_to_char("Immortals cant remort moron", ch);
	return;
    }

    fclose( fpReserve );
	sprintf( buf, "%sremort",capitalize( ch->name ));
    sprintf( strsave, "%s%s", PLAYER_DIR, buf );
    if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
    {
		bug( "Save_remort_char: fopen", 0 );
		perror( strsave );
	}
	else
	{
		fwrite_remort( ch, fp );
		if ( ch->carrying != NULL )
			fwrite_obj( ch, ch->carrying, fp, 0 );
		fprintf( fp, "#END\n" );
	}
    fclose( fp );
    if (remove (strsave) != 0)   /* NJG - remove old file so we can rename onto it */
    	perror( strsave );
    if (rename(TEMP_FILE,strsave) != 0)
  	  perror( strsave );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/*
 * Write the remort char.
 */
void fwrite_remort( CHAR_DATA *ch, FILE *fp )
{
    int sn, gn;

    fprintf( fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER"	);

    fprintf( fp, "Name %s~\n",	ch->name		);
    fprintf( fp, "Id   %ld\n", ch->id			);
    fprintf( fp, "LogO %ld\n",	current_time		);
    fprintf( fp, "Vers %d\n",   5			);
    if (ch->short_descr[0] != '\0')
      	fprintf( fp, "ShD  %s~\n",	ch->short_descr	);
    if( ch->long_descr[0] != '\0')
	fprintf( fp, "LnD  %s~\n",	ch->long_descr	);
    if (ch->description[0] != '\0')
    	fprintf( fp, "Desc %s~\n",	ch->description	);
    if (ch->prompt != NULL || !str_cmp(ch->prompt,"<%hhp %mm %vmv> "))
        fprintf( fp, "Prom %s~\n",      ch->prompt  	);
    fprintf( fp, "Sex  %d\n",	ch->sex			);
    if (ch->trust != 0)
    fprintf( fp, "Scro %d\n", 	ch->lines		);
    if (ch->act != 0)
	fprintf( fp, "Act  %s\n",   print_flags(ch->act));
	fprintf( fp, "Pass %s~\n",	ch->pcdata->pwd		);
	fprintf( fp, "Incr %d\n", ch->pcdata->incarnations );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0 )
	    {
		fprintf( fp, "Sk %d '%s'\n",
		    ch->pcdata->learned[sn], skill_table[sn].name );
	    }
	}

	for ( gn = 0; gn < MAX_GROUP; gn++ )
    {
            if ( group_table[gn].name != NULL && ch->pcdata->group_known[gn])
            {
                fprintf( fp, "Gr '%s'\n",group_table[gn].name);
            }
    }
    fprintf( fp, "End\n\n" );
    return;
}

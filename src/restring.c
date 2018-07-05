/****************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,			*
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.	*
 *																			*
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael			*
 *  Chastain, Michael Quan, and Mitchell Tse.								*
 *																			*
 *  In order to use any part of this Merc Diku Mud, you must comply with	*
 *  both the original Diku license in 'license.doc' as well the Merc		*
 *  license in 'license.txt'.  In particular, you may not remove either of	*
 *  these copyright notices.												*
 *																			*
 *  Much time and thought has gone into this software and you are			*
 *  benefitting.  We hope that you share your changes too.  What goes		*
 *  around, comes around.													*
 ***************************************************************************/
 
/****************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor								*
*	ROM has been brought to you by the ROM consortium						*
*	    Russ Taylor (rtaylor@hypercube.org)									*
*	    Gabrielle Taylor (gtaylor@hypercube.org)							*
*	    Brian Moore (zump@rom.org)											*
*	By using this code, you have agreed to follow the terms of the			*
*	ROM license, in the file Rom24/doc/rom.license							*
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
#include <time.h>
#define NOCRYPT
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "arena.h"
#include "olc.h"
#include "slay.h"

void do_restring( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
	char buf [MSL];
    OBJ_DATA *obj;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' )
    {
		if ( ch->pcdata->restring_obj != NULL )
		{
			sprintf(buf,"{WName:{x %s\n\r",ch->pcdata->restring_name);
			stc(buf,ch);
			sprintf(buf,"{WShort:{x %s\n\r",ch->pcdata->restring_short);
			stc(buf,ch);
			sprintf(buf,"{WLong:{x %s\n\r",ch->pcdata->restring_long);
			stc(buf,ch);
			return;
		}
		send_to_char(	"Syntax:\n\r",ch);
		send_to_char(	"  restring <object>\n\r",ch);
		stc(			"  restring <field> <string>\n\r",ch);
		send_to_char(	"    fields: name short long\n\r",ch);
		stc(			"  restring accept\n\r",ch);
		return;
    }
   	/* string an obj */
    if ( ch->pcdata->restring_obj == NULL )
	{
		if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
		{
			send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
			return;
		}
		ch->pcdata->restring_obj = obj;
		sprintf(buf,"{WNow restringing %s\n\r",obj->short_descr);
		stc(buf,ch);
		return;
	}

	if (arg2[0] == '\0')
	{
		do_function(ch, &do_restring, "");
	}

	if ( ch->pcdata->restring_obj != NULL )
	{
		if ( !str_prefix( arg1, "name" ) )
		{
			ch->pcdata->restring_name = str_dup( arg2 );
			sprintf(buf,"{WName queued as{x %s\n\r",arg2);
			stc(buf,ch);
			return;
		}

		if ( !str_prefix( arg1, "short" ) )
		{
			ch->pcdata->restring_short = str_dup( arg2 );
			sprintf(buf,"{WShort description queued as{x %s\n\r",arg2);
			stc(buf,ch);
			return;
		}

		if ( !str_prefix( arg1, "long" ) )
		{
			ch->pcdata->restring_long = str_dup( arg2 );
			sprintf(buf,"{WLong description queued as{x %s\n\r",arg2);
			stc(buf,ch);
			return;
		}

		if ( !str_prefix( arg1, "confirm" ) )
		{
			if (ch->questpoints < 100)
			{
				stc("{RYou do not have the required QP's{x\n\r",ch);
				return;
			}
			if (ch->pcdata->restring_name != NULL 
				&& ch->pcdata->restring_short != NULL
				&& ch->pcdata->restring_long != NULL)
			{
				obj = ch->pcdata->restring_obj;
				free_string( obj->name );
				free_string( obj->short_descr);
				free_string( obj->description);
				obj->name = str_dup( ch->pcdata->restring_name );
				obj->short_descr = str_dup( ch->pcdata->restring_short );
				obj->description = str_dup( ch->pcdata->restring_long );
				stc("{WObject restrung!!\n\r",ch);
				stc("{G100{B QP's{W have been subtracted from your person.{x\n\r",ch);
				ch->questpoints -= 10;
				return;
			}
		}
		if ( !str_prefix( arg1, "cancel" ) )
		{
			if (ch->pcdata->restring_name		!= NULL 
				&& ch->pcdata->restring_short	!= NULL
				&& ch->pcdata->restring_long	!= NULL)
			{
				ch->pcdata->restring_obj = NULL;
				free_string( ch->pcdata->restring_name );
				free_string( ch->pcdata->restring_short );
				free_string( ch->pcdata->restring_long );
				stc("{WRestring Data Cleared.\n\r",ch);
				return;
			}
		}
	}
	/* echo bad use message */
    do_function(ch, &do_restring, "");
}

/****************************************************************************
*       ROM 2.4 is copyright 1993-1995 Russ Taylor							*
*       ROM has been brought to you by the ROM consortium					*
*           Russ Taylor (rtaylor@pacinfo.com)								*
*           Gabrielle Taylor (gtaylor@pacinfo.com)							*
*           Brian Moore (rom@rom.efn.org)									*
*       By using this code, you have agreed to follow the terms of the		*
*       ROM license, in the file Rom24/doc/rom.license						*
****************************************************************************/

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
*	Demon's Dance MUD, and source code are property of Eric Goetschalckx	*
*	By compiling this code, you agree to include the following in your		*
*	login screen:															*
*	    Derivative of Demon's Dance, by Enzo/Stan							*
***************************************************************************/

/*Trophy writing, and displaying file*/

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

void add_sever(CHAR_DATA *ch, CHAR_DATA *victim)
{
	CHAR_DATA *rch;
	char buf[MSL];
	int pos;
    
	
	if (ch->desc == NULL)
	rch = ch;
    else
	rch = ch->desc->original ? ch->desc->original : ch;

    if (IS_NPC(rch))
	return;

    for (pos = 0; pos < MAX_SEVER; pos++)
    {
		if (rch->pcdata->severings[pos] == NULL)
			break;
		if (!str_cmp(victim->short_descr,rch->pcdata->severings[pos]))
		{
			sprintf(buf,"You already have a trophy from %s{x.\n\r",victim->name);
			send_to_char(buf,ch);
			return;
		}
	}

	if (pos >= MAX_SEVER)
	{
		send_to_char("Sorry, you have reached the sever limit.\n\r",ch);
		return;
	}

	rch->pcdata->severings[pos]	= str_dup(victim->short_descr);
    sprintf(buf,"%s has been added to your trophies.\n\r",victim->short_descr);
    send_to_char(buf,ch);
	return;
}

void add_fatality(CHAR_DATA *ch, CHAR_DATA *victim)
{
	CHAR_DATA *rch;
	char buf[MSL];
	int pos;

	if (ch->desc == NULL)
	rch = ch;
    else
	rch = ch->desc->original ? ch->desc->original : ch;

    if (IS_NPC(rch))
	return;

    for (pos = 0; pos < MAX_FATALITY; pos++)
    {
		if (rch->pcdata->fatalities[pos] == NULL)
			break;
		if (!str_cmp(victim->short_descr,rch->pcdata->fatalities[pos]))
		{
			sprintf(buf,"You already have a trophy from %s{x.\n\r",victim->name);
			send_to_char(buf,ch);
			return;
		}
	}

	if (pos >= MAX_FATALITY)
	{
		send_to_char("Sorry, you have reached the fatality limit.\n\r",ch);
		return;
	}
	
	ch->pcdata->fatalities[pos]	= str_dup(victim->short_descr);
	sprintf(buf,"%s has been added to your trophies.\n\r",victim->short_descr);
	send_to_char(buf,ch);
	return;
}


void do_trophy(CHAR_DATA *ch, char *argument)
{
	int pos;
	char buf[MSL];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

    if (IS_NPC(ch))
	return;

    argument = one_argument(argument,arg);
    

    if (arg[0] == '\0')
    {
		/*begin severings*/
		send_to_char("{RSevered legs:\n\r",ch);

		for (pos = 0; pos < MAX_SEVER; pos++)
		{
			if (ch->pcdata->severings[pos] == NULL)
			break;

			sprintf(buf,"{W    %s{x\n\r",ch->pcdata->severings[pos]);
			send_to_char(buf,ch);
		}
		/*end severings*/

		/*begin fatalities*/
		send_to_char("{RFatality victims{W:\n\r",ch);

		for (pos = 0; pos < MAX_FATALITY; pos++)
		{
			if (ch->pcdata->fatalities[pos] == NULL)
			break;
			
			sprintf(buf,"{W    %s{x\n\r",ch->pcdata->fatalities[pos]);
			send_to_char(buf,ch);
		}
		/*end fatalities*/
		return;
	}

	if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

	if	(ch->position == POS_FIGHTING)
	{
		send_to_char("Not while you are in combat",ch);
		return;
	}

	if (victim->position == POS_FIGHTING)
	{
		sprintf(buf,"Not while %s is in combat",victim->name);
		send_to_char(buf,ch);
		return;
	}

	if (victim->position == POS_SLEEPING)
	{
		sprintf(buf,"Not while %s is sleeping",victim->name);
		send_to_char(buf,ch);
		return;
	}

	else
	{
		sprintf(buf,"{WYou show your collection of gruesome trophies to %s",victim->name);
		send_to_char(buf,ch);
			/*begin severings*/
		sprintf(buf,"{R%s's Severed legs{W:\n\r",ch->name);
		send_to_char(buf,victim);

		for (pos = 0; pos < MAX_SEVER; pos++)
		{
			if (ch->pcdata->severings[pos] == NULL)
			break;

			sprintf(buf,"{W    %s{x\n\r",ch->pcdata->severings[pos]);
			send_to_char(buf,victim);
		}
		/*end severings*/

		/*begin fatalities*/
		sprintf(buf,"{R%s's Fatality victims{W:\n\r",ch->name);
		send_to_char(buf,victim);

		for (pos = 0; pos < MAX_FATALITY; pos++)
		{
			if (ch->pcdata->fatalities[pos] == NULL)
			break;

			sprintf(buf,"{W    %s{x\n\r",ch->pcdata->fatalities[pos]);
			send_to_char(buf,victim);
		}
		/*end fatalities*/
		return;
	}
}


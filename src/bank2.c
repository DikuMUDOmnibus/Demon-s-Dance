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

/* Banking system by Vir
 * mailto:leckey@rogers.wave.ca
 * (C)opyright 1997, Eternal Struggle MUD
 * telnet://es.mudservices.com:4321
 * http://es.mudservices.com/index.html
 * Made for a SMAUG MUD Code Base.
 * May be used by anybody providing this header stays.
 */

/* Remember to add a "balance" pointer in the char_data struct in "mud.h"
 * Also, add ch->balance to fwrite_char and fread_char.
 * If you have on-line player creation or remorting, add ch->balance = 0 in
 * "load_char_obj".
 * That's all you need to do.
 * Enjoy!
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "arena.h"
#include "interp.h"
#include "tables.h"
#include "olc.h"
#include "recycle.h"
#include "lookup.h"


void do_balance( CHAR_DATA *ch, char *argument )
{
	 char arg[MAX_INPUT_LENGTH];
	 char buf[MAX_INPUT_LENGTH];
	 argument = one_argument( argument, arg );

	 if ( IS_NPC(ch) )
	 {
	  send_to_char( "Mobs don't have bank accounts.\n\r", ch );
	  return;
	 }

	 if ( IS_IMMORTAL(ch) )
	 {
	  send_to_char( "You don't need a bank account!\n\r", ch );
	  return;
	 }

	 sprintf( buf, "{WYour Current Balance: {Y%ld gold{x", ch->balance );
	 send_to_char( buf, ch );
	 return;
}

void do_withdraw( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;
	int amount;

	location = get_room_index( ROOM_VNUM_BANK );

	argument = one_argument( argument, arg );
	amount = atoi(arg);

	if (IS_NPC(ch))
	{
		send_to_char( "Mobs don't have bank accounts!\n\r", ch );
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char( "Withdraw how much?\n\r", ch );
		return;
	}

	if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))
	{
		send_to_char( "You are not in a bank.\n\r", ch );
		return;
	}

	if (!is_number( arg ))
	{
		send_to_char( "Withdraw how much?\n\r", ch );
		return;
	}

	if ( ch->balance < amount )
	{
		send_to_char( "Your account does not have that much gold in it!\n\r", ch );
		return;
	}
	ch->gold += amount;
	ch->balance -= amount;
	sprintf(buf, "{WYou withdraw {Y%d gold{W, bringing your ammount to {Y%ld{x",amount, ch->balance);
	send_to_char(buf,ch);
	act( "$n withdraws some gold.", ch, NULL, NULL, TO_ROOM );
	return;
}


void do_deposit( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;
	
	int amount;
	
	argument = one_argument( argument, arg );
	amount = atoi(arg);
	location = get_room_index( ROOM_VNUM_BANK );

	if ( IS_NPC(ch) )
	{
		send_to_char( "Mobs don't have bank accounts!\n\r", ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Deposit how much?\n\r", ch );
		return;
	}

	if ( IS_IMMORTAL(ch) )
	{
		send_to_char( "You do not need a bank account!\n\r", ch );
		return;
	}

	if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))
	{
		send_to_char( "You are not in a bank.\n\r", ch );
		return;
	}

	if ( !is_number( arg ) )
	{
		send_to_char( "Deposit how much?\n\r", ch );
		return;
	}

	if ( ch->gold < amount )
	{
		send_to_char( "You don't have that much gold!\n\r", ch );
		return;
	}
	ch->gold -= amount;
	ch->balance += amount;
	sprintf(buf, "{WYou deposit {Y%d gold{W, bringing your ammount to {Y%ld{x",amount, ch->balance);
	send_to_char(buf,ch);
	act( "$n deposits some gold.", ch, NULL, NULL, TO_ROOM );
	return;
}


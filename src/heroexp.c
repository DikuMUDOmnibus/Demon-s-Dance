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


void do_exchange(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *changer;
	char buf[MSL];
	char arg1[MIL];
	char arg2[MIL];
	int value;
	int qp_amt,gold_amt,prac_amt;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    for ( changer = ch->in_room->people; changer != NULL; changer = changer->next_in_room )
    {
		if (!IS_NPC(changer)) continue;
		if (changer->spec_fun == spec_lookup( "spec_exp_changer" )) break;
    }

    if (changer == NULL || changer->spec_fun != spec_lookup( "spec_exp_changer" ))
    {
        send_to_char("You can't do that here.\n\r",ch);
        return;
    }

    if ( changer->fighting != NULL)
    {
		send_to_char("Wait until the fighting stops.\n\r",ch);
        return;
    }

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char("Syntax: exchange <exp ammount> <type>",ch);
		send_to_char("        Suitable types are:",ch);
		send_to_char("           qp gold pracs",ch);
		return;
	}

	if (!is_number(arg1))
	{
		send_to_char("Please enter a numerical ammount of exp to exchange",ch);
		return;
	}

	value = atoi( arg1 );
	if (value <= 0)
	{
		stc("You can only exchange positive values my friend\n\r",ch);
		return;
	}
	if ( !str_cmp( arg2, "qp" ) )
	{
		if (ch->pcdata->hero_exp < value)
		{
			send_to_char("You do not have that much exp in your Hero's Exp Storage",ch);
			return;
		}
		else if( ch->pcdata->hero_exp >= value)
		{
			ch->pcdata->hero_exp -= value;
			qp_amt = (value / 125) ;
			add_qp( ch, qp_amt);
			sprintf(buf,"You traded %d exp for %d QPs",value,qp_amt);
			send_to_char(buf,ch);
			return;
		}
		else
		{
			send_to_char("Syntax: exchange <exp ammount> <type>",ch);
			send_to_char("        Suitable types are:",ch);
			send_to_char("           qp gold pracs",ch);
			return;
		}
	}

	if ( !str_cmp( arg2, "gold" ) )
	{
		if (ch->pcdata->hero_exp <= value)
		{
			send_to_char("You do not have that much exp in your Hero's Exp Storage",ch);
			return;
		}
		else if( ch->pcdata->hero_exp >= value)
		{
			ch->pcdata->hero_exp -= value;
			gold_amt = (value * 8);
			ch->gold += gold_amt;
			sprintf(buf,"You traded %d exp for %d gold",value,gold_amt);
			send_to_char(buf,ch);
			return;
		}
		else
		{
			send_to_char("Syntax: exchange <exp ammount> <type>",ch);
			send_to_char("        Suitable types are:",ch);
			send_to_char("           qp gold pracs",ch);
			return;
		}
	}

	if ( !str_cmp( arg2, "trains" ) )
	{
		if (ch->pcdata->hero_exp <= value)
		{
			send_to_char("You do not have that much exp in your Hero's Exp Storage",ch);
			return;
		}
		else if( ch->pcdata->hero_exp >= value)
		{
			ch->pcdata->hero_exp -= value;
			prac_amt = (value * 3 / 2500);
			ch->practice += prac_amt;
			sprintf(buf,"You traded %d exp for %d trains",value,prac_amt);
			send_to_char(buf,ch);
			return;
		}
		else
		{
			send_to_char("Syntax: exchange <exp ammount> <type>",ch);
			send_to_char("        Suitable types are:",ch);
			send_to_char("           qp gold pracs",ch);
			return;
		}
	}
	else
	{
		send_to_char("Syntax: exchange <exp ammount> <type>",ch);
		send_to_char("        Suitable types are:",ch);
		send_to_char("           qp gold pracs",ch);
		return;
	}
}

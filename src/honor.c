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
/***************************************************************************
 *  Hello and thanks for trying out my arena code. :) All I really have to *
 *  is that I would like a little credit if you dont mind like keeping     *
 *  the statements I make in the file and keeping this header in the file  *
 *  I dont REQUIRE you to give me credit although its always nice to       *
 *  credit the coders for their work(hint hint :p).  Anyways all I really  *
 *  want is for you people to have fun anyways adios.  Any problems,       *
 *  changes, bugs, comments or anything you might wanna see on the next    *
 *  release(if there even is one) mail me at shuvthisupyourass@hotmail.com *
 ******************************[Happy MUDding]******************************/
/****************************************************************************
*	Demon's Dance MUD, and source code are property of Eric Goetschalckx	*
*	By compiling this code, you agree to include the following in your		*
*	login screen:															*
*	    Derivative of Demon's Dance, by Enzo/Stan							*
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "arena.h"

/* Alright you can delete this after you read it I only have here here for a few 
 * ideas and such.  Alright on my mud I have it so you can yourself and fight a 
 * that is cloned after you.  Also I am working on having it so you can challenge
 * groups and guilds and do battle that way which comes in handy for NPK guilds and
 * such.  Anyways if anyone has any ideas or finds some bugs as I didn't test this 
 * all that much just to see if it worked and I could challenge after I challenge 
 * someone.  Alright thats all anyways mail me if you find bugs or have ideas or
 * wanna make a comment about it :) anyways adios have fun 
 */

bool is_challenge;
bool challenge_tme = 0;
bool arena_is_busy = FALSE;
int hstart_time = 0;
int pre_hstart;
/*
 * do_challenge sends the challenge
 */
void do_challenge(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
	int i = 0;

    if (argument[0] == '\0')
    {
        send_to_char("You must specify who you want to challenge.\n\r", ch);
        return;
    }

	if (is_challenge)
	{
		send_to_char("Someone has been challenged wait a few moments the try again.",ch);
		return;
    }

	if (arena_is_busy)
    {
		send_to_char("The arena is being used at the moment..Please wait a few minutes.\n\r",ch);
		return;
    }

	if ((victim = get_char_world(ch,argument)) == NULL)
    {

		act_new( "Sorry but $t seems to be gone from the realms at this time.'",
		    ch,argument, NULL, TO_CHAR,POS_STANDING );
        return; /* simply change the acts to send_to_char if you dont want this. */
    }

	if (victim == ch)
	{
		send_to_char("You can't challenge yourself\n\r",ch);
		return;
	}

    if(IS_SET(ch->plr2,PLR2_CHALLENGED))
    {
		send_to_char("You have been challenged already.  Accept or decline.\n\r",ch);
		return;
    }

    if(IS_SET(victim->plr2,PLR2_CHALLENGER))
    {
		act("$N has already challenged someone else.",ch,NULL,victim,TO_CHAR);
		return;
    }

    if(IS_SET(ch->plr2,PLR2_CHALLENGER))
    {
		act("You have already challenged someone.",ch,NULL,victim,TO_CHAR);
		return;
    }

	if (IS_SET(victim->comm,COMM_AFK))
	{
		act("Sorry but $N is AFK at the moment.",ch,NULL,victim,TO_CHAR);
		return; /* simply change the acts to send_to_char if you dont want this. */
    }
      /* this check was changed you need to change what it says though :p */
	if (IS_NPC(victim))
    {
        send_to_char("You can only challenge players.\n\r",ch);
        return; /* simply change the acts to send_to_char if you dont want this. */
    }

    if (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
	{
		act("Sorry but $E is a higher being.",ch,NULL,victim,TO_CHAR);
		send_to_char("Besides they all would laugh at you!\n\r",ch);
		return; /* simply change the acts to send_to_char if you dont want this. */
 	}

	if (victim->level <= 5)
	{
		act("Sorry but $E is not experianced enough.",ch,NULL,victim,TO_CHAR);
		return; /* simply change the acts to send_to_char if you dont want this. */
	}

	if (victim->fighting != NULL )
    {
        act("Sorry but $N is in combat right now.",ch,NULL,victim,TO_CHAR);
        return;
    }

	if (!str_cmp(argument,"self") || !str_cmp(argument,"mirror match"))
	{              
          return; /* Sorry I dont wanna give away all my stuff right now :p */
      }

	/* this is where the challenge gets sent */
	sprintf(buf, "{W%s has challenged %s to a duel!{x", ch->name, victim->name);
	do_hctalk(ch,buf);
	SET_BIT(ch->plr2,PLR2_CHALLENGER);
	SET_BIT(victim->plr2,PLR2_PRE_CHALLENGED);
	pre_hstart = 3;
	sprintf(buf,"{RYour challenge has been sent if %s does not accept in %d ticks it will be declined{x\n\r",victim->name, pre_hstart);
	send_to_char(buf,ch);
	sprintf(buf,"If they do not accept the challenge will be automatically withdrawn from %s.{x\n\r",victim->name);
	send_to_char(buf,ch);
	sprintf(buf,"{RYou have been challenged by %s!{x\n\r",ch->name);
	send_to_char(buf,victim);
	sprintf(buf,"Type 'accept %s' to accept or 'decline %s' to decline.\n\r",ch->name, ch->name);
	send_to_char(buf,victim);
	challenge_tme = 5;
	is_challenge = TRUE;
	honor_has_started = FALSE;
	return;
}

/*
 * Accepts a challenge
 */
void do_accept(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *room1;
    ROOM_INDEX_DATA *room2;
	ROOM_INDEX_DATA *honor;
    
    if (IS_NPC(ch))
    {
         send_to_char("Hrmm....You must be a new kinda mobile to accept challenges eh?\n\r",ch);
         return;
    }

    if (argument[0] == '\0')
    {
        send_to_char("You must be specify who challenged you.\n\r", ch);
        return;
    }

	if ((victim = get_char_world(ch,argument)) == NULL)
    {
		act_new( "Sorry but $t seems to be gone from the realms at this time.'",
		    ch,argument, NULL, TO_CHAR,POS_STANDING );
        return; /* simply change the acts to send_to_char if you dont want this. */
    }

    if (IS_NPC(victim))
    {
        send_to_char("Not on Mobiles\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char("I bet you think your funny eh?\n\r",ch);
        return;
    }

    if (!IS_SET(victim->plr2, PLR2_CHALLENGER))
    {
        send_to_char("They're not the ones challenging you\n\r",ch);
        return;
    }

    if(!IS_SET(ch->plr2,PLR2_PRE_CHALLENGED))
    {
		act("$N hasn't challenged you.",ch,NULL,victim,TO_CHAR);
		return;
    }
    
    /* these will need to get changed for your mud :p */
    room1 = get_room_index(30001);
    room2 = get_room_index(30002);
	honor = get_room_index(30000);

    sprintf(buf, "{W%s has accepted %s's challenge!{x", ch->name, victim->name);
	do_hctalk(ch,buf);
    send_to_char("{WYou plop onto the ground.{x\n\r",ch);
    char_from_room(ch);
    char_to_room( ch, room2 );
    do_look(ch,"auto");
	ch->hit = ch->max_hit;
	ch->mana = ch->max_mana;
	ch->move = ch->max_move;
	do_stop_aff(ch,ch->name);

    send_to_char("{WYou plop onto the ground.{x\n\r",victim);
    char_from_room(victim);
    char_to_room( victim, room1 );
    do_look(victim, "auto");
	victim->hit = victim->max_hit;
	victim->mana = victim->max_mana;
	victim->move = victim->max_move;
	do_stop_aff(victim,victim->name);

    SET_BIT(ch->plr2,PLR2_CHALLENGED);
	REMOVE_BIT(ch->plr2,PLR2_PRE_CHALLENGED);
    arena_is_busy = TRUE;
    is_challenge = FALSE;
    challenge_tme = 0;
	hstart_time = 2;
	honor_has_started = FALSE;
    return;
}

void do_decline(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    if (argument[0] == '\0')
    {
        send_to_char("You must specify who you want to challenge.\n\r", ch);
        return;
    }

	if (!is_challenge)
	{
		send_to_char("Nobody hase even challenged someone wanna try again?\n\r",ch);
		return;
      }

    if (arena_is_busy)
    {
		send_to_char("The arena is busy...\n\r",ch);
		return;
    }

    if ((victim = get_char_world(ch,argument)) == NULL)
    {

		act_new( "Sorry but $t seems to be gone from the realms at this time.",
		    ch,argument, NULL, TO_CHAR,POS_STANDING );
        return; /* simply change the acts to send_to_char if you dont want this. */
    }

    if(!IS_SET(victim->plr2,PLR2_CHALLENGER))
    {
		act("$N hasn't challenged you.",ch,NULL,victim,TO_CHAR);
		return;
    }

    if (IS_NPC(victim) || victim == ch)
    {
        send_to_char("How did that happen?\n\r",ch);
        return; /* simply change the acts to send_to_char if you dont want this. */
    }

    if(!IS_SET(ch->plr2,PLR2_PRE_CHALLENGED))
    {
		act("$N hasn't challenged you.",ch,NULL,victim,TO_CHAR);
		return;
    }

    is_challenge = FALSE;
    REMOVE_BIT(victim->plr2, PLR2_CHALLENGER);
	REMOVE_BIT(ch->plr2, PLR2_PRE_CHALLENGED);
    sprintf(buf, "{W%s declined %s's invitation to duel!{x", ch->name, victim->name);
	do_hctalk(ch,buf);
    return;
}

void do_unchallenge(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    return;

    if (!IS_SET(ch->plr2,PLR2_CHALLENGER))
    {
        send_to_char("You haven't challenged anyone!\n\r", ch);
        return;
    }

    if ((victim = get_char_world(ch, argument)) == NULL)
    {
        send_to_char("That player is not logged in\n\r", ch);
        return;
    }

    if (arena_is_busy)
    {
        send_to_char("The arena is busy...\n\r", ch);
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char("Not on Mobiles\n\r", ch);
        return;
    }

    is_challenge = FALSE;
    REMOVE_BIT(ch->plr2,PLR2_CHALLENGER);
	REMOVE_BIT(victim->plr2,PLR2_PRE_CHALLENGED);
    sprintf(buf, "{W%s has withdrawn their challenge!{x", ch->name);
	do_hctalk(ch,buf);
    return;
}

 /*
  * Sent to honor viewing room
  */
void send_to_honor( const char *format, CHAR_DATA *ch, const void *arg1, 
 	      const void *arg2, int type, int min_pos )
  {
	static char * const he_she  [] = { "it",  "he",  "she" };
	static char * const him_her [] = { "it",  "him", "her" };
	static char * const his_her [] = { "its", "his", "her" };
	CHAR_DATA 		*to;
	CHAR_DATA 		*vch = ( CHAR_DATA * ) arg2;
	OBJ_DATA 		*obj1 = ( OBJ_DATA  * ) arg1;
	OBJ_DATA 		*obj2 = ( OBJ_DATA  * ) arg2;
	ROOM_INDEX_DATA		*room;
	const 	char 	*str;
	char 		*i = NULL;
	char 		*point;
	char 		*pbuff;
	char 		buffer[ MSL*2 ];
	char 		buf[ MSL   ];
	char 		fname[ MIL  ];
	bool		fColour = FALSE;

	/*
	 * Discard null and zero-length messages.
	 */
	if( !format || !*format )
		return;
  
	/* discard null rooms and chars */
    if( !ch || !ch->in_room )
  	return;
	room = get_room_index(3062);
	to = room->people;
	if( type == TO_VICT )
	{
		if( !vch )
		{
			bug( "Act: null vch with TO_VICT.", 0 );
			return;
		}
	}
	for( ; to ; to = to->next_in_room )
	{
		if( type == TO_CHAR )
			continue;
		if( type == TO_VICT)
			continue;
		point   = buf;
		str     = format;
		while( *str != '\0' )
		{
			if( *str != '$' )
			{
				*point++ = *str++;
				continue;
			}
   
			fColour = TRUE;
			++str;
			i = " <@@@> ";
			if( !arg2 && *str >= 'A' && *str <= 'Z' )
			{
				bug( "Act: missing arg2 for code %d.", *str );
				i = " <@@@> ";
			}
			else
			{
				switch ( *str )
				{
				default:  bug( "Act: bad code %d.", *str );
                          i = " <@@@> ";                                break;
				case 't': i = (char *) arg1;                            break;
				case 'T': i = (char *) arg2;                            break;
				case 'n': i = PERS( ch,  to  );                         break;
				case 'N': i = PERS( vch, to  );                         break;
				case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];        break;
				case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];        break;
				case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];        break;
				case 'M': i = him_her [URANGE(0, vch ->sex, 2)];        break;
				case 's': i = his_her [URANGE(0, ch  ->sex, 2)];        break;
				case 'S': i = his_her [URANGE(0, vch ->sex, 2)];        break;
 
				case 'p':
					i = can_see_obj( to, obj1 )
						? obj1->short_descr: "something";
					break;
 
				case 'P':
					i = can_see_obj( to, obj2 )
						? obj2->short_descr: "something";
					break;
 
				case 'd':
					if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
					{
						i = "door";
					}
					else
					{
						one_argument( (char *) arg2, fname );
						i = fname;
					}
				break;
			}
		}
		++str;
		while ( ( *point = *i ) != '\0' )
			++point, ++i;
	}
	*point++ = '\n';
	*point++ = '\r';
	*point   = '\0';
	buf[0]   = UPPER(buf[0]);
	pbuff	 = buffer;
	colourconv( pbuff, buf, to );
	if ( to->desc != NULL )
	{
	  write_to_buffer( to->desc, buffer, 0 );
	}
	else if ( MOBtrigger )
		p_act_trigger( buf, to, NULL, NULL, ch, arg1, arg2, TRIG_ACT );
	}
}

void honor_update( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	ROOM_INDEX_DATA *honor;
	CHAR_DATA *fch;
	
	honor = get_room_index(30000);

	if (er_found(ch) && !ed_found(ch) && pre_ed_found(ch))
	{
		if (pre_hstart >= 1)
		{
			sprintf(buf,"Challenged has %d ticks to accept", pre_hstart);
			do_hctalk(ch,buf);
			pre_hstart --;
			return;
		}
		else if (pre_hstart == 0)
		{
			sprintf(buf,"Challenged has run out of time to accept");
			do_hctalk(ch,buf);
			for (d = descriptor_list; d != NULL; d = d->next)
			{
				fch = d->original ? d->original : d->character;
				if ( !fch || !fch->plr2) 
					continue;
				if (IS_SET(fch->plr2, PLR2_CHALLENGER))
				{
					REMOVE_BIT(fch->plr2,PLR2_CHALLENGER);
					break;
				}
			}
			for (d = descriptor_list; d != NULL; d = d->next)
			{
				fch = d->original ? d->original : d->character;
				if ( !fch || !fch->plr2) 
					continue;
				if (IS_SET(fch->plr2, PLR2_PRE_CHALLENGED))
				{
					REMOVE_BIT(fch->plr2,PLR2_PRE_CHALLENGED);
					break;
				}
			}
			is_challenge = FALSE;
			return;
		}
	}
			

	if ((arena_is_busy == TRUE)
		&& (is_challenge == FALSE)
		&& (challenge_tme == 0)
		&& honor_has_started == FALSE
		&& er_found 
		&& ed_found)
	{
		if (hstart_time >= 1)
		{
			sprintf(buf,"Combatants have %d ticks for preparation{x", hstart_time);
			do_hctalk(ch,buf);
			hstart_time --;
			return;
		}
		if ((hstart_time == 0)
			&& (honor_has_started == FALSE))
		{
			for (d = descriptor_list; d != NULL; d = d->next)
			{
				fch = d->original ? d->original : d->character;
				if ( !fch || !fch->plr2) 
					continue; 
				if (IS_SET(fch->plr2, PLR2_CHALLENGER))
				{
					char_from_room(fch);
					char_to_room(fch, honor);
				}
				else if (IS_SET(fch->plr2, PLR2_CHALLENGED))
				{
					char_from_room(fch);
					char_to_room(fch, honor);
				}
				else
					continue;
			}
			sprintf(buf,"{RFIGHT!!{W\n\r");
			do_hctalk(ch,buf);
			honor_has_started = TRUE;
			return;
		}
		else
			return;
	}
}

bool er_found ( CHAR_DATA *ch )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *fch;


	for (d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
			continue;
		if (IS_SET(fch->plr2, PLR2_CHALLENGER))
		{
			return TRUE;
		}
	}
	return FALSE;
}

bool ed_found ( CHAR_DATA *ch )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *fch;


	for (d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
			continue;
		if (IS_SET(fch->plr2, PLR2_CHALLENGED))
		{
			return TRUE;
		}
	}
	return FALSE;
}

bool pre_ed_found ( CHAR_DATA *ch )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *fch;


	for (d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
			continue;
		if (IS_SET(fch->plr2, PLR2_PRE_CHALLENGED))
		{
			return TRUE;
		}
	}
	return FALSE;
}


/* This is has been censored even though I hate censorship           *
*********************************************************************
*  Hello I would first like to say hello and thanks for using this  *
*  well that is if you use it. :p  All I have to say is that this   *
*  is a base foundation for a good arena system why i say good is   *
*  because as far as I know with the testing I have done is that    *
*  works without any bugs in the functions I am releasing to you in *
*  this version.  Anyways this is the installation.  So what I say  *
*  in here is what you MUST follow not really I am sure there is a  *
*  better way to do what I did heh anyways.  DONT SEND ME SHIT      *
*  TELLING ME THAT I SUCK AT CODING AND HOW MUCH BETTER THIS COULDA *
*  BEEN WRITTEN.  The reason I say that is because for some reason  *
*  all the arena snippets I have downloaded always had mass ass     *
*  bugs and shit and its easier to write your own after fixing all  *
*  of them.  After all if your looking at this your prolly conside- *
*  using this in your mud so dont say SHIT! heh now that I have     *
*  outta my system lets get on with it!  oh yea have fun :)         *
*********************************************************************/


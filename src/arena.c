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

int		red_leader;
int		blue_leader;
void	remove_teams	args( ( CHAR_DATA *ch ) );

void do_startwar(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	DESCRIPTOR_DATA *d;
	int nMatch;
	char warshow[MSL];
  
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);
	/*make sure enough characters are on*/
    nMatch = 0;
    
	for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
        if ( d->connected != CON_PLAYING)
            continue;
		if ( !d->character ) 
			continue; 
        wch   = ( d->original != NULL ) ? d->original : d->character;
		if (wch->level <= 101)
			nMatch++;
		else continue;
	}

	if (nMatch < 2)
	{
		send_to_char("Sorry, but there arent enough people on to have a war.", ch);
		return;
	}

	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char("Syntax: startwar <type> <min_level> <max_level>\n\r", ch);
		return;
	}

	if ( atoi(arg1) < 1 || atoi(arg1) > 3)
	{
		send_to_char("The type either has to be 1, or 2.\n\r", ch);
		return;
	}

	if ( atoi(arg2) <= 0 || atoi(arg2) > 110)
	{
		send_to_char("Level must be between 1 and 110.\n\r", ch);
		return;
	}

	if (atoi(arg3) <= 0 || atoi(arg3) > 110)
	{
		send_to_char("Level must be between 1 and 110.\n\r", ch);
		return;
	}

	if (atoi(arg3) < atoi(arg2))
	{
		send_to_char("Max level must be greater than the min level.\n\r", ch);
		return;
	}

	if (iswar == TRUE)
	{
		send_to_char("There is already a war going!\n\r", ch);
		return;
	}

	iswar = TRUE;
	wartype = atoi(arg1);
	switch (wartype)
	{
	default:
		sprintf(warshow,"{GSingle{x");
		break;
	case 1:
		sprintf(warshow,"{GSingle{x");
		break;
	case 2:
		sprintf(warshow,"{BT{be{Ra{rm{x");
		break;
	break;
	}
	min_level = atoi(arg2);
	max_level = atoi(arg3);
	sprintf(buf, "{W{c%s {Wwar started for levels {c%d {Wto {C%d{W.  Type {R'{YWAR{R'{W to kill or be killed{x", warshow, min_level, max_level);
	do_wartalk(ch,buf);
	time_left = 3;
	wartimer = 0;
	red_leader = FALSE;
	blue_leader = FALSE;
	red_num = 0;
	blue_num = 0;
	check_red = 0;
	check_blue = 0;
}

void do_petition(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	DESCRIPTOR_DATA *d;
	int nMatch;
	char warshow[MSL];
  
	argument = one_argument(argument, arg1);
	/*make sure enough characters are on*/
    nMatch = 0;
    
	for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
        if ( d->connected != CON_PLAYING)
            continue;
		if ( !d->character ) 
			continue; 
        wch   = ( d->original != NULL ) ? d->original : d->character;
		if (wch->level <= 101)
			nMatch++;
		else continue;
	}

	if (nMatch < 2)
	{
		send_to_char("Sorry, but there arent enough people on to have a war.", ch);
		return;
	}

	if (arg1[0] == '\0')
	{
		send_to_char("Syntax: petition <type>\n\r", ch);
		return;
	}

	if (iswar == TRUE)
	{
		send_to_char("There is already a war going!\n\r", ch);
		return;
	}

	iswar = TRUE;
	wartype = atoi(arg1);
	switch (wartype)
	{
	default:
		sprintf(warshow,"{GSingle{x");
		break;
	case 1:
		sprintf(warshow,"{GSingle{x");
		break;
	case 2:
		sprintf(warshow,"{BT{be{Ra{rm{x");
		break;
	break;
	}
	min_level = 1;
	max_level = 101;
	sprintf(buf, "{W{c%s {Wwar started for levels {c%d {Wto {C%d{W.  Type {R'{YWAR{R'{W to kill or be killed{x", warshow, min_level, max_level);
	do_wartalk(ch,buf);
	time_left = 3;
	wartimer = 0;
	red_leader = FALSE;
	blue_leader = FALSE;
	red_num = 0;
	blue_num = 0;
	check_red = 0;
	check_blue = 0;
}

void do_war(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *location;
	

	if (iswar != TRUE) 
	{
		send_to_char("There is no war going!\n\r", ch);
		return;
	}

	if (ch->level < min_level || ch->level > max_level)
	{
		send_to_char("Sorry, you can't join this war.\n\r", ch);
		return;
	}

	if (IS_SET(ch->plr2, PLR2_ARENA))
	{
		send_to_char("I don't think so.\n\r", ch);
		return;
	}

	if (time_left <= 0)
	{
		send_to_char("{RThat war has already begun{x\n\r",ch);
		return;
	}

	if (wartype == 1)
	{
		if ((location = get_room_index(SINGLE_WAR_WAITING_ROOM)) == NULL)
		{
		send_to_char("Arena is not yet completed, sorry.\n\r", ch);
		return;
		}
		else
		{
		  act("{g$n{W joins the war!{x",ch, NULL, NULL, TO_ROOM); 
		  char_from_room(ch);
		  char_to_room(ch, location);
		  SET_BIT(ch->plr2, PLR2_ARENA);
		  SET_BIT(ch->plr2, PLR2_SINGLE_WAR);
		  sprintf(buf, "{G%s {D({R%d{D) {Wjoins the war!", ch->name, ch->level);
		  do_wartalk(ch,buf);
		  warprize += ch->level;
		  sprintf(buf, "{WWarprize is up to {B%d{x",warprize);
		  do_wartalk(ch,buf);
		  sprintf(buf, "{WTime left to join: {Y%d{x",time_left);
		  do_wartalk(ch,buf);
		  act("$n arrives to do battle", ch, NULL, NULL, TO_ROOM);
		  inwar++;
		  do_look(ch, "auto");
		  return;
		}
	}

	if (wartype == 2)
	{
		location = get_room_index(SINGLE_WAR_WAITING_ROOM);
		if (red_leader == FALSE)
		{
			act("{g$n{W joins the war!{x", ch, NULL, NULL, TO_ROOM); 
			char_from_room(ch);
			char_to_room(ch, location);
			sprintf(buf, "{G%s {D({R%d{D) {Wjoins the war!", ch->name, ch->level);
			do_wartalk(ch,buf);
			sprintf(buf, "{G%s{W is now leader of the {RR{re{Rd{W team.",ch->name);
			do_wartalk(ch,buf);
			SET_BIT(ch->plr2, PLR2_ARENA);
			SET_BIT(ch->plr2, PLR2_RED_LEADER);
			SET_BIT(ch->plr2, PLR2_RED_TEAM);
			red_leader = TRUE;
			warprize += ch->level;
			sprintf(buf, "{WWarprize is up to {B%d{x",warprize);
			do_wartalk(ch,buf);
			sprintf(buf, "{WTime left to join: {Y%d{x",time_left);
			do_wartalk(ch,buf);
			act("$n arrives to do battle", ch, NULL, NULL, TO_ROOM);
			inwar++;
			do_look(ch, "auto");
			next_team = 1;
			red_num ++;
			return;
		}
		if (blue_leader == FALSE && red_leader == TRUE)
		{
			act("{g$n{W joins the war!{x", ch, NULL, NULL, TO_ROOM); 
			char_from_room(ch);
			char_to_room(ch, location);
			sprintf(buf, "{G%s {D({R%d{D) {Wjoins the war!", ch->name, ch->level);
			do_wartalk(ch,buf);
			sprintf(buf, "{G%s{W is now leader of the {BB{bl{Bu{be{W team.",ch->name);
			do_wartalk(ch,buf);
			SET_BIT(ch->plr2, PLR2_ARENA);
			SET_BIT(ch->plr2, PLR2_BLUE_LEADER);
			SET_BIT(ch->plr2, PLR2_BLUE_TEAM);
			blue_leader = TRUE;
			warprize += ch->level;
			sprintf(buf, "{WWarprize is up to {B%d{x",warprize);
			do_wartalk(ch,buf);
			sprintf(buf, "{WTime left to join: {Y%d{x",time_left);
			do_wartalk(ch,buf);
			act("$n arrives to do battle", ch, NULL, NULL, TO_ROOM);
			inwar++;
			do_look(ch, "auto");
			next_team = 0;
			blue_num ++;
			return;
		}
		if ((red_leader == TRUE) && (blue_leader == TRUE))
		{
			assign_red_team(ch);
			return;
		}
	}
	if (wartype == 3)
	{
		if ((location = get_room_index(SINGLE_WAR_WAITING_ROOM)) == NULL)
		{
		send_to_char("Arena is not yet completed, sorry.\n\r", ch);
		return;
		}
		else
		{
		  act("{g$n{W joins the war!{x", ch, NULL, NULL, TO_ROOM); 
		  char_from_room(ch);
		  char_to_room(ch, location);
		  SET_BIT(ch->plr2, PLR2_ARENA);
		  SET_BIT(ch->plr2, PLR2_SINGLE_WAR);
		  sprintf(buf, "{G%s {D({R%d{D) {Wjoins the war!", ch->name, ch->level);
		  do_wartalk(ch,buf);
		  warprize += ch->level;
		  sprintf(buf, "{WWarprize is up to {B%d{x",warprize);
		  do_wartalk(ch,buf);
		  sprintf(buf, "{WTime left to join: {Y%d{x",time_left);
		  do_wartalk(ch,buf);
		  act("$n arrives to do battle", ch, NULL, NULL, TO_ROOM);
		  inwar++;
		  do_look(ch, "auto");
		  return;
		}
	}
}

void war_update( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *fch;
	
	bool found = FALSE;
   
	if (time_left >= 2)
	{
	time_left--;
	sprintf(buf, "{W{G%d {Wtick%s left to join the war.{x", time_left, time_left == 1 ? "" : "s");
	do_wartalk(ch,buf);
    sprintf(buf, "{R%d {W%s %s fighting in the war, so far.{x", inwar, inwar == 1 ? "person" : "people", inwar == 1 ? "is" : "are");
	do_wartalk(ch,buf);
    sprintf(buf, "{WType of war: {c%d {R- {C%d{R, {C%s {Wwar.{x", min_level, max_level, wartype == 1 ? "{GSingle" : "{BT{be{Ra{rm");
	do_wartalk(ch,buf);
    return;
	}
	
	if ((time_left <= 1) && (iswar == TRUE) && (wartimer == 0))
	{
		time_left = 0;
		if (inwar == 0 || inwar == 1)
		{
			sprintf(buf, "{W{WNot enough people for war.  {RWar {YR{ye{Ws{ye{Yt{W.{x");
			do_wartalk(ch,buf);
			inwar = 0;
			warprize = 0;
			iswar = FALSE;
			time_left = 0;
			wartimer = 0;
			min_level = 0;
			max_level = 0;
			wartype = 0;
			for(d = descriptor_list; d != NULL; d = d->next)
			{
				fch = d->original ? d->original : d->character;
				if ( !fch || !fch->plr2) 
					continue; 
				remove_teams(fch);
				if (IS_SET(fch->plr2, PLR2_ARENA))
				{
					char_from_room(fch);
					char_to_room(fch, (get_room_index(ROOM_VNUM_TEMPLE)));
					do_look(fch, "auto");
					REMOVE_BIT(fch->plr2, PLR2_ARENA);
				}
			}
			return;
		}
		else if ((wartype == 1) || (wartype == 3))
		{
			sprintf(buf, "{W{WThe battle begins! {R%d {Wplayers are fighting!{x", inwar);
			do_wartalk(ch,buf);
			wartimer = 20;
			for ( d = descriptor_list; d != NULL; d = d->next )
			{
				fch = d->original ? d->original : d->character;
				if ( !fch || !fch->plr2) 
					continue; 
				if (IS_SET(fch->plr2, PLR2_ARENA))        
				{
					move_char_war(fch);
				}
			}
			return;
		}
		else
		{
			sprintf(buf, "{W{WThe battle begins! {R%d {Wplayers are fighting!{x", inwar);
			do_wartalk(ch,buf);
			wartimer = 20;
			for ( d = descriptor_list; d != NULL; d = d->next )
			{
				fch = d->original ? d->original : d->character;
				if ( !fch || !fch->plr2) 
					continue; 
				if (IS_SET(fch->plr2, PLR2_NO_TEAM) 
					&& IS_SET(fch->plr2, PLR2_ARENA))
				{
					if (next_team == 0)
					{
						sprintf(buf,"{G%s {Wjoins the {RR{re{Rd{W team{x",d->character->name);
						do_wartalk(ch,buf);
						SET_BIT(fch->plr2, PLR2_RED_TEAM);
						red_num ++;
						next_team = 1;
						move_char_war(fch);
					}
					else if (next_team == 1)
					{
						sprintf(buf,"{G%s {Wjoins the {BB{bl{Bu{be{W team{x",d->character->name);
						do_wartalk(ch,buf);
						SET_BIT(fch->plr2, PLR2_BLUE_TEAM);
						blue_num ++;
						next_team = 0;
						move_char_war(fch);
					}
				}
				else if (IS_SET(fch->plr2, PLR2_ARENA) 
					&& (IS_SET(fch->plr2, PLR2_RED_TEAM) 
					|| IS_SET(fch->plr2, PLR2_BLUE_TEAM)))
				{
				move_char_war(fch);
				}
			}
			return;
		}
		return;
	}
	return;
}

void do_stopwar (CHAR_DATA *ch, char *argument)
{	  
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *fch;
	
	DESCRIPTOR_DATA *d;

	if (iswar = FALSE)
	{
		sprintf(buf, "Sorry bud, there's no war at the moment");
		send_to_char(buf, ch);
	}
	else
	sprintf(buf, "{RWar {WR{Re{Ys{Re{Wt {xby {C%s.{x", ch->name);
	do_wartalk(ch,buf);
	sprintf(buf, "You have stopped the war dead in its tracks\n\r");
	send_to_char(buf, ch);
	sprintf(buf, "");
	send_to_char(buf, ch);
    iswar = FALSE;
	inwar = 0;
    time_left = 0;
    wartimer = 0;
    min_level = 0;
    max_level = 0;
    wartype = 0;
	warprize = 0;
	red_leader = 0;
	blue_leader = 0;
	red_num = 0;
	blue_num = 0;
	check_red = 0;
	check_blue = 0;
	for(d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
			continue; 
		remove_teams(fch);
		if (IS_SET(fch->plr2, PLR2_ARENA))
		{
			char_from_room(fch);
			char_to_room(fch, (get_room_index(ROOM_VNUM_TEMPLE)));
			REMOVE_BIT(fch->plr2, PLR2_ARENA);
		}
	}
}

void do_stop_aff(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
	while ( victim->affected )
		affect_remove( victim, victim->affected );
	victim->affected_by	= race_table[victim->race].aff;
}

void war_prize( CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	
	int random;

	random = number_range(90,100);
	train_prize = (warprize * random)/20000;
	qp_prize	= (warprize * random)/500;
	gold_prize	= (warprize * random);
	qp_final	= qp_prize - (train_prize * 20);
	gold_final	= gold_prize - (qp_prize * 1000);
	sprintf(buf, "{G%s{W wins {Y%d{W gold",ch->name, gold_final);
	do_wartalk(ch,buf);
	if (qp_final > 0)
	{
		sprintf(buf, "{G%s{W wins {B%d{W QP's", ch->name, qp_final);
		do_wartalk(ch,buf);
	}
	if (train_prize > 0)
	{
		sprintf(buf, "{G%s{W wins {C%d{W trains", ch->name, train_prize);
		do_wartalk(ch,buf);
	}
	ch->gold += gold_final;
	add_qp(ch, qp_final);
	ch->train += train_prize;
	affect_strip(ch,gsn_fortify);
	REMOVE_BIT(ch->affected2_by,AFF_FORTIFY);
	ch->pcdata->arena_wins ++;
	return;
}

void assign_red_team(CHAR_DATA *ch)
{
	DESCRIPTOR_DATA *d;
	ROOM_INDEX_DATA *location;
	CHAR_DATA *fch;
	char buf[MSL];

	location = get_room_index(SINGLE_WAR_WAITING_ROOM);
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
			continue; 
		if (IS_SET(fch->plr2,PLR2_RED_LEADER))
		{
			if ((ch->max_hit > fch->max_hit) && (check_red != 1))
			{
				act("{g$n{W joins the war!{x", ch, NULL, NULL, TO_ROOM); 
				char_from_room(ch);
				char_to_room(ch, location);
				SET_BIT(ch->plr2, PLR2_ARENA);
				sprintf(buf, "{G%s {D({R%d{D) {Wjoins the war!", ch->name, ch->level);
				do_wartalk(ch,buf);
				sprintf(buf, "{G%s{W is longer the leader of the {RR{re{Rd{W team.",fch->name);
				do_wartalk(ch,buf);
				sprintf(buf, "{G%s{W is now leader of the {RR{re{Rd{W team.",ch->name);
				do_wartalk(ch,buf);
				SET_BIT(ch->plr2,PLR2_RED_LEADER);
				SET_BIT(ch->plr2, PLR2_RED_TEAM);
				REMOVE_BIT(fch->plr2,PLR2_RED_LEADER);
				REMOVE_BIT(fch->plr2,PLR2_RED_TEAM);
				SET_BIT(fch->plr2, PLR2_NO_TEAM);
				warprize += ch->level;
				sprintf(buf, "{WWarprize is up to {B%d{x",warprize);
				do_wartalk(ch,buf);
				sprintf(buf, "{WTime left to join: {Y%d{x",time_left);
				do_wartalk(ch,buf);
				act("$n arrives to do battle", ch, NULL, NULL, TO_ROOM);
				inwar++;
				do_look(ch, "auto");
				next_team = 1;
				return;
			}
			else if (check_blue != 1)
			{
				check_red = 1;
				assign_blue_team(ch);
				return;
			}
			else
			{
				act("{g$n{W joins the war!{x", ch, NULL, NULL, TO_ROOM); 
				char_from_room(ch);
				char_to_room(ch, location);
				SET_BIT(ch->plr2, PLR2_ARENA);
				sprintf(buf, "{G%s {D({R%d{D) {Wjoins the war!", ch->name, ch->level);
				do_wartalk(ch,buf);
				SET_BIT(ch->plr2, PLR2_NO_TEAM);
				warprize += ch->level;
				sprintf(buf, "{WWarprize is up to {B%d{x",warprize);
				do_wartalk(ch,buf);
				sprintf(buf, "{WTime left to join: {Y%d{x",time_left);
				do_wartalk(ch,buf);
				act("$n arrives to do battle", ch, NULL, NULL, TO_ROOM);
				inwar++;
				do_look(ch, "auto");
				next_team = 0;
				check_red = 0;
				check_blue = 0;
				return;
			}
		}
		else
		{
			continue;
		}
		return;
	}
}

void assign_blue_team(CHAR_DATA *ch)
{
	DESCRIPTOR_DATA *d;
	ROOM_INDEX_DATA *location;
	CHAR_DATA *fch;
	char buf[MSL];
	
	location = get_room_index(SINGLE_WAR_WAITING_ROOM);
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
			continue; 
		if (IS_SET(fch->plr2,PLR2_BLUE_LEADER) )
		{
			if ((ch->max_hit > fch->max_hit) && (check_blue != 1))
			{
				act("{g$n{W joins the war!{x", ch, NULL, NULL, TO_ROOM); 
				char_from_room(ch);
				char_to_room(ch, location);
				SET_BIT(ch->plr2, PLR2_ARENA);
				sprintf(buf, "{G%s {D({R%d{D) {Wjoins the war!", ch->name, ch->level);
				do_wartalk(ch,buf);
				sprintf(buf, "{G%s{W is no longer the leader of the {BB{bl{Bu{be{W team.",fch->name);
				do_wartalk(ch,buf);
				sprintf(buf, "{G%s{W is now leader of the {BB{bl{Bu{be{W team.",ch->name);
				do_wartalk(ch,buf);
				SET_BIT(ch->plr2,PLR2_BLUE_LEADER);
				SET_BIT(ch->plr2, PLR2_BLUE_TEAM);
				REMOVE_BIT(fch->plr2,PLR2_BLUE_LEADER);
				REMOVE_BIT(fch->plr2,PLR2_BLUE_TEAM);
				SET_BIT(fch->plr2, PLR2_NO_TEAM);
				warprize += ch->level;
				sprintf(buf, "{WWarprize is up to {B%d{x",warprize);
				do_wartalk(ch,buf);
				sprintf(buf, "{WTime left to join: {Y%d{x",time_left);
				do_wartalk(ch,buf);
				act("$n arrives to do battle", ch, NULL, NULL, TO_ROOM);
				inwar++;
				do_look(ch, "auto");
				next_team = 0;
				return;
			}
			else if (check_red != 1)
			{
				check_blue = 1;
				assign_red_team(ch);
				return;
			}
			else
			{
				act("{g$n{W joins the war!{x", ch, NULL, NULL, TO_ROOM); 
				char_from_room(ch);
				char_to_room(ch, location);
				SET_BIT(ch->plr2, PLR2_ARENA);
				sprintf(buf, "{G%s {D({R%d{D) {Wjoins the war!", ch->name, ch->level);
				do_wartalk(ch,buf);
				SET_BIT(ch->plr2, PLR2_NO_TEAM);
				warprize += ch->level;
				sprintf(buf, "{WWarprize is up to {B%d{x",warprize);
				do_wartalk(ch,buf);
				sprintf(buf, "{WTime left to join: {Y%d{x",time_left);
				do_wartalk(ch,buf);
				act("$n arrives to do battle", ch, NULL, NULL, TO_ROOM);
				inwar++;
				do_look(ch, "auto");
				next_team = 0;
				check_red = 0;
				check_blue = 0;
				return;
			}
		}
		else
		{
			continue;
		}
		return;
	}
}

void remove_teams(CHAR_DATA *ch)
{
	if (IS_SET(ch->plr2, PLR2_RED_LEADER))
		REMOVE_BIT(ch->plr2,PLR2_RED_LEADER);
	if (IS_SET(ch->plr2, PLR2_BLUE_LEADER))
		REMOVE_BIT(ch->plr2,PLR2_BLUE_LEADER);
	if (IS_SET(ch->plr2, PLR2_RED_TEAM))
		REMOVE_BIT(ch->plr2,PLR2_RED_TEAM);
	if (IS_SET(ch->plr2, PLR2_BLUE_TEAM))
		REMOVE_BIT(ch->plr2,PLR2_BLUE_TEAM);
	if (IS_SET(ch->plr2, PLR2_NO_TEAM))
		REMOVE_BIT(ch->plr2,PLR2_NO_TEAM);
	if (IS_SET(ch->plr2,PLR2_SINGLE_WAR))
		REMOVE_BIT(ch->plr2,PLR2_SINGLE_WAR);
	if (IS_SET(ch->plr2,PLR2_CHALLENGED));
		REMOVE_BIT(ch->plr2,PLR2_CHALLENGED);
	if (IS_SET(ch->plr2,PLR2_CHALLENGER));
		REMOVE_BIT(ch->plr2,PLR2_CHALLENGER);
	if (IS_SET(ch->plr2,PLR2_PRE_CHALLENGED));
		REMOVE_BIT(ch->plr2,PLR2_PRE_CHALLENGED);
}

void move_char_war(CHAR_DATA *ch)
{
	ROOM_INDEX_DATA *random;

	/*Function that removes affects and heals arena players*/
	ch->hit = ch->max_hit;
	ch->mana = ch->max_mana;
	ch->move = ch->max_move;
	do_stop_aff(ch,ch->name);
	random = get_room_index( number_range(12001, 12016));
	char_from_room(ch);
	char_to_room(ch, random);
	do_look(ch, "auto");
}

 /*
  * The colour version of the act_new( ) function, -Lope
  *	This is now what sends the act_new( )'s
  */
void send_to_war( const char *format, CHAR_DATA *ch, const void *arg1, 
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
	room = get_room_index(3061);
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

bool is_same_team( CHAR_DATA *ach, CHAR_DATA *bch )
{
	if ( ach == NULL || bch == NULL)
	{
		return FALSE;
	}

	if (IS_SET(ach->plr2,PLR2_RED_TEAM) 
		&& IS_SET(bch->plr2,PLR2_RED_TEAM) 
		&& IS_SET(ach->plr2,PLR2_ARENA))
	{
		return TRUE;
	}
	if (IS_SET(ach->plr2,PLR2_BLUE_TEAM)
		&& IS_SET(bch->plr2,PLR2_BLUE_TEAM) 
		&& IS_SET(ach->plr2,PLR2_ARENA))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool is_arena( CHAR_DATA *ch )
{
	if ( ch == NULL)
	{
		return FALSE;
	}

	if (IS_SET(ch->plr2,PLR2_ARENA))
	{
		return TRUE;
	}

	else
	{
		return FALSE;
	}
}

bool is_honor( CHAR_DATA *ch )
{
	if ( ch == NULL)
		return FALSE;
	
	if (IS_SET(ch->plr2,PLR2_CHALLENGED)
		&& (honor_has_started))
		return TRUE;

	if(IS_SET(ch->plr2,PLR2_CHALLENGER)
		&& (honor_has_started))
		return TRUE;

	else
		return FALSE;
}

bool in_single_war( CHAR_DATA *ch )
{
	if ( ch == NULL)
	{
		return FALSE;
	}

	if (IS_SET(ch->plr2,PLR2_ARENA) && IS_SET(ch->plr2,PLR2_SINGLE_WAR))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


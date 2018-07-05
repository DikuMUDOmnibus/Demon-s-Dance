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
#include <math.h>
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

void do_raid(CHAR_DATA *ch, char *argument)
{
	char buf[MSL];
	char arg1[MIL];
	char arg2[MIL];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		sprintf(buf,"Sytnax:   raid <option>\n\r\
start <clan>\n\r\
join\n\r\
leave\n\r\
protect\n\r\
giveup\n\r\
parley\n\r\
progress\n\r\
stop\n\r");
		stc(buf,ch);
		return;
	}
	if (!str_prefix(arg1,"start"))
    {
		start_raid(ch,arg2);
		return;
	}
	if (!str_prefix(arg1,"join"))
    {
		join_raid(ch);
		return;
	}
		if (!str_prefix(arg1,"leave"))
    {
		leave_raid(ch);
		return;
	}
	if (!str_prefix(arg1,"defend"))
    {
		defend_raid(ch);
		return;
	}
	if (!str_prefix(arg1,"giveup"))
    {
		giveup_raid(ch);
		return;
	}
	if (!str_prefix(arg1,"parley"))
    {
		parley_raid(ch);
		return;
	}	
	if (!str_prefix(arg1,"progress"))
    {
		raid_progress(ch);
		return;
	}
	if (!str_prefix(arg1,"stop"))
    {
		if (ch->level <= LEVEL_HERO)
		{
			stc("You cant do that, an IMM must stop the raid",ch);
			return;
		}
		raid_stop(ch);
	}
	else
	{
		sprintf(buf,"Sytnax:   raid <option>\n\r\
start <clan>\n\r\
join\n\r\
leave\n\r\
protect\n\r\
giveup\n\r\
parley\n\r\
progress\n\r\
stop\n\r");
		stc(buf,ch);
		return;
	}
}
void start_raid(CHAR_DATA *ch, char *target)
{
	int to_raid;
	char buf[MSL];

    if ((to_raid = clan_lookup(target)) == 0)
    {
		stc("No such guild exists.\n\r", ch);
		return;
    }
	
	if (!is_clan(ch))
	{
		stc("You arent part of a clan\n\r",ch);
		return;
	}
	
	if (!is_leader(ch))
	{
		stc("You aren't the leader of your clan\n\r",ch);
		return;
	}
	
	if (raid_info.raiding == 1)
	{
		stc("There is already a raid going on\n\r",ch);
		return;
	}
	if (clan_table[ch->clan].clan_points < 5)
	{
		stc("Your clan does not have enough CPs to begin the raid\n\r",ch);
		return;
	}
	raid_info.raiding = 1;
	raid_info.clan_raider = ch->clan;
	raid_info.clan_defender = to_raid;
	sprintf(buf,"%s is preparing to raid %s!!\n\r",clan_table[raid_info.clan_raider].who_name,
													clan_table[raid_info.clan_defender].who_name);
	raid_info.status = RAID_START;
	raid_info.time_before_raid = 2;
	raid_channel(buf);
	clan_table[raid_info.clan_raider].clan_points -= 5;
	return;
}
void join_raid(CHAR_DATA *ch)
{
	char buf[MSL];
	
	if (!is_clan(ch))
	{
		stc("You arent part of a clan\n\r",ch);
		return;
	}

	if (ch->clan != raid_info.clan_raider)
	{
		stc("Your Clan isnt raiding right now\n\r",ch);
		return;
	}

	if (raid_info.raiding == 0)
	{
		stc("There is no raid at the moment\n\r",ch);
		return;
	}

	if (IS_SET(ch->plr2,PLR2_RAIDER))
	{
		stc("You are already raiding\n\r",ch);
		return;
	}
	if (raid_info.status != RAID_START)
	{
		stc("You have missed your chance to join the raid, sorry\n\r",ch);
		return;
	}
	if (!IS_SET(ch->plr2,PLR2_RAIDER))
	{
		SET_BIT(ch->plr2,PLR2_RAIDER);
		stc("You have joined the raid, FIGHT MAIM KILL!\n\r",ch);
		sprintf(buf,"{W%s {Whas joined the raiding party!!\n\r",ch->name);
		raid_channel(buf);
		raid_info.raiders ++;
		return;
	}
	return;
}
void leave_raid(CHAR_DATA *ch)
{
	char buf[MSL];

	if (!is_clan(ch))
	{
		stc("You arent part of a clan\n\r",ch);
		return;
	}

	if (ch->clan != raid_info.clan_raider)
	{
		stc("Your Clan isnt raiding right now\n\r",ch);
		return;
	}

	if (raid_info.raiding == 0)
	{
		stc("There is no raid at the moment\n\r",ch);
		return;
	}

	if (!IS_SET(ch->plr2,PLR2_RAIDER))
	{
		stc("You aren't raiding\n\r",ch);
		return;
	}
	if (IS_SET(ch->plr2,PLR2_RAIDER))
	{
		REMOVE_BIT(ch->plr2,PLR2_RAIDER);
		stc("You have withdrawn from the raiding party, weakling\n\r",ch);
		sprintf(buf,"{W%s {Whas left the raiding party!!\n\r",ch->name);
		raid_channel(buf);
		raid_info.raiders --;
		return;
	}
	return;
}
void defend_raid(CHAR_DATA *ch)
{
	char buf[MSL];

	if (!is_clan(ch))
	{
		stc("You arent part of a clan\n\r",ch);
		return;
	}

	if (ch->clan != raid_info.clan_defender)
	{
		stc("Your Clan isnt raiding right now\n\r",ch);
		return;
	}

	if (raid_info.raiding == 0)
	{
		stc("There is no raid at the moment\n\r",ch);
		return;
	}

	if (IS_SET(ch->plr2,PLR2_RAIDED))
	{
		stc("You are already raiding\n\r",ch);
		return;
	}
	if (raid_info.status != RAID_START)
	{
		stc("You have missed your chance to join the raid, sorry\n\r",ch);
		return;
	}
	if (!IS_SET(ch->plr2,PLR2_RAIDED))
	{
		SET_BIT(ch->plr2,PLR2_RAIDED);
		stc("You have joined the defending party!\n\r",ch);
		sprintf(buf,"{W%s {Whas joined the defenders!!\n\r",ch->name);
		raid_channel(buf);
		raid_info.defenders ++;
		return;
	}
	return;
}
void giveup_raid(CHAR_DATA *ch)
{
	char buf[MSL];

	if (!is_clan(ch))
	{
		stc("You arent part of a clan\n\r",ch);
		return;
	}

	if (ch->clan != raid_info.clan_defender)
	{
		stc("Your Clan isnt raiding right now\n\r",ch);
		return;
	}

	if (raid_info.raiding == 0)
	{
		stc("There is no raid at the moment\n\r",ch);
		return;
	}

	if (!IS_SET(ch->plr2,PLR2_RAIDED))
	{
		stc("You aren't raiding\n\r",ch);
		return;
	}
	if (IS_SET(ch->plr2,PLR2_RAIDED))
	{
		REMOVE_BIT(ch->plr2,PLR2_RAIDED);
		stc("You removed yourself from the defending party, weakling\n\r",ch);
		sprintf(buf,"{W%s {Whas given up on the defenders!!\n\r",ch->name);
		raid_channel(buf);
		raid_info.defenders --;
		return;
	}
	return;
}
void parley_raid(CHAR_DATA *ch)
{
	stc("parley_raid",ch);
	return;
}
void raid_progress(CHAR_DATA *ch)
{
	char buf[MSL];
	
	if (raid_info.raiding == 0)
	{
		stc("There is no raid going on\n\r",ch);
		return;
	}
	sprintf(buf,"The %s{x are raiding the %s{x\n\r",clan_table[raid_info.clan_raider].who_name,
												clan_table[raid_info.clan_defender].who_name);
	stc(buf,ch);
	sprintf(buf,"Raiders left standing:   %d\n\r",raid_info.raiders);
	stc(buf,ch);
	sprintf(buf,"Defenders left Standing: %d\n\r",raid_info.defenders);
	stc(buf,ch);
	return;
}

void raid_channel( const char *txt)
{
	CHAR_DATA *ch;
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];


  for (d = descriptor_list; d != NULL; d = d->next)
  {
    ch = d->original ? d->original : d->character;

    if (d->connected == CON_PLAYING &&
        !IS_SET(ch->comm,COMM_NOWAR) &&
        !IS_SET(ch->comm,COMM_QUIET))
    {
      sprintf(buf, "{D[{rRAID{D] {W%s{x\n\r", txt);
      send_to_char(buf, ch);
    }
  }
}

void raid_update( )
{
    char buf[MSL];

    if ( raid_info.raiding == 0 )
	{
		return;
	}
	if ( raid_info.status <= 3 )
	{
		sprintf(buf,"Raid Status: {G%d\n\r",raid_info.status);
		raid_channel(buf);
	}
	if(raid_info.status == RAID_START)
	{
		if(raid_info.time_before_raid == 1)
		{
			raid_info.status++;
			raid_info.time_before_raid--;
			sprintf(buf,"The {BPreparation Phase{W has started.{x\n\r");
			move_raiders(clan_table[raid_info.clan_raider].prep_room);
			move_defenders(clan_table[raid_info.clan_defender].prep_room);
			raid_channel(buf);
			raid_info.prep_time = 2;
			return;
		}
		if(raid_info.time_before_raid > 1)
		{
			raid_info.time_before_raid--;
			sprintf(buf,"There are %d ticks until the {BPreparation Phase{W begins.{x\n\r",raid_info.time_before_raid);
			raid_channel(buf);
			return;
		}
	}
	if(raid_info.status == RAID_PREP)
	{
		if(raid_info.prep_time == 1)
		{
			raid_info.status++;
			raid_info.prep_time--;
			sprintf(buf,"The raid begins, as the raiders penetrate their way into the defender's maze\n\r",raid_info.status);
			raid_channel(buf);
			sprintf(buf,"{RIT HAS BEGUN!!{W(like Tsang Sung says it, in the movie){x\n\r",raid_info.status);
			raid_channel(buf);
			sprinkle_raiders( );
			sprinkle_defenders( );
			return;
		}
		if(raid_info.prep_time > 1)
		{
			raid_info.prep_time--;
			sprintf(buf,"{WThere are {G%d{W ticks left in the {BPreparation Phase{x\n\r",raid_info.prep_time);
			raid_channel(buf);
			return;
		}
	}
	if(raid_info.status > RAID_PREP)
	{
		return;
	}
	return;
}

void raid_stop( CHAR_DATA *ch )
{
	char buf[MSL];

	if (ch->level <= LEVEL_HERO)
	{
		stc("You cant do that, an IMM must stop the raid",ch);
		return;
	}
	else
	{
		raiders_win( );
		sprintf(buf,"{W%s {WHAS STOPPED THE RAID{x",ch->name);
		raid_channel(buf);
		return;
	}
}

void move_raiders( int room )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *fch;

	for(d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
			continue; 
		if (IS_SET(fch->plr2, PLR2_RAIDER))
		{
			char_from_room(fch);
			char_to_room(fch, (get_room_index(room)));
			stc("You have been trasported to another place!\n\r",fch);
			do_look(fch,"auto");
		}
	}
	return;
}

void move_defenders( int room )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *fch;
	int prep;

	prep = clan_table[raid_info.clan_defender].prep_room;

	for(d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
			continue; 
		if (IS_SET(fch->plr2, PLR2_RAIDED))
		{
			char_from_room(fch);
			char_to_room(fch, (get_room_index(room)));
			stc("You have been trasported to another place!\n\r",fch);
			do_look(fch,"auto");
		}
	}
	return;
}

void sprinkle_raiders( )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *fch;
	char buf[MSL];
	int room;
	int mid;
	int begin;

	begin = clan_table[raid_info.clan_defender].maze_begin;
	mid = clan_table[raid_info.clan_defender].maze_mid;
	room = number_range(begin,mid);


	for(d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
		{
			continue; 
		}
		if (IS_SET(fch->plr2, PLR2_RAIDER))
		{
			char_from_room(fch);
			char_to_room(fch, (get_room_index(room)));
			sprintf(buf,"The raiding party has been sprinkled over the defender's maze!\n\r");
			raid_channel(buf);
			stc("You have been trasported into the maze!\n\r",fch);
			do_look(fch,"auto");
		}
	}
	return;
}

void sprinkle_defenders( )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *fch;
	char buf[MSL];
	int room;
	int end;
	int mid;

	end = clan_table[raid_info.clan_defender].maze_end;
	mid = clan_table[raid_info.clan_defender].maze_mid;
	

	for(d = descriptor_list; d != NULL; d = d->next)
	{
		room = number_range(mid,end);
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
		{
			continue; 
		}
		if (IS_SET(fch->plr2, PLR2_RAIDED))
		{
			char_from_room(fch);
			char_to_room(fch, (get_room_index(room)));
			sprintf(buf,"The defenders have been sprinkled over their maze!\n\r");
			raid_channel(buf);
			stc("{WYou have been trasported into the maze!{x\n\r",fch);
			do_look(fch,"auto");
		}
	}
	return;
}

bool is_raider(CHAR_DATA *ch)
{
	if ( !ch || !ch->plr2) 
	{
		return FALSE;
	}
	if (IS_SET(ch->plr2,PLR2_RAIDER)
		&& ((ch->in_room->vnum >= clan_table[raid_info.clan_defender].maze_begin)
		&& (ch->in_room->vnum <= clan_table[raid_info.clan_defender].maze_end))
		|| (ch->in_room->vnum == clan_table[raid_info.clan_defender].threshold))
	{
		return TRUE;
	}
	return FALSE;
}
bool is_raided(CHAR_DATA *ch)
{
	if ( !ch || !ch->plr2) 
	{
		return FALSE;
	}
	if (IS_SET(ch->plr2,PLR2_RAIDED)
		&& ((ch->in_room->vnum >= clan_table[raid_info.clan_defender].maze_begin)
		&& (ch->in_room->vnum <= clan_table[raid_info.clan_defender].maze_end))
		|| (ch->in_room->vnum == clan_table[raid_info.clan_defender].threshold))
	{
		return TRUE;
	}
	return TRUE;
}

void raiders_win( void )
{
	char buf[MSL];
	CHAR_DATA *fch;
	DESCRIPTOR_DATA *d;
	ROOM_INDEX_DATA *room;

	sprintf(buf,"{WThe savage %s{W raiding party has smashed past the %s{W's defenses, and plundered the clan hall!\n\r",
		clan_table[raid_info.clan_raider].who_name,
		clan_table[raid_info.clan_defender].who_name);
	raid_channel(buf);

	for(d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
		{
			continue; 
		}
		if (IS_SET(fch->plr2, PLR2_RAIDED))
		{
			room = get_room_index(clan_table[raid_info.clan_defender].room[1]);
			calc_win_raiders();
			char_from_room(fch);
			char_to_room(fch,room);
			REMOVE_BIT(fch->plr2, PLR2_RAIDED);
			stc("{WYour clan has lost the raid, and you have been transported to your morgue{x\n\r",fch);
		}
		if (IS_SET(fch->plr2, PLR2_RAIDER))
		{
			room = get_room_index(clan_table[raid_info.clan_raider].room[0]);
			char_from_room(fch);
			char_to_room(fch,room);
			REMOVE_BIT(fch->plr2, PLR2_RAIDER);
			stc("{WYour clan recalls victoriously from your latest assault, you appear in your Clan Hall weighted down with all sorts of loot!{x\n\r",fch);
		}
	}
	clear_raid_info( );
}

void defenders_win( void )
{
	char buf[MSL];
	CHAR_DATA *fch;
	DESCRIPTOR_DATA *d;
	ROOM_INDEX_DATA *room;

	sprintf(buf,"{WThe savage %s{W raiding party was stopped in its tracks by the stalwart %s{W defenders, and systematically destroyed{x\n\r",
		clan_table[raid_info.clan_raider].who_name,
		clan_table[raid_info.clan_defender].who_name);
	raid_channel(buf);

	for(d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
		{
			continue; 
		}
		if (IS_SET(fch->plr2, PLR2_RAIDER))
		{
			room = get_room_index(clan_table[raid_info.clan_raider].room[1]);
			char_from_room(fch);
			char_to_room(fch,room);
			REMOVE_BIT(fch->plr2, PLR2_RAIDER);
			stc("{WYour clan has lost the raid, and you have been transported to your morgue{x\n\r",fch);
		}
		if (IS_SET(fch->plr2, PLR2_RAIDED))
		{
			room = get_room_index(clan_table[raid_info.clan_defender].room[0]);
			char_from_room(fch);
			char_to_room(fch,room);
			REMOVE_BIT(fch->plr2, PLR2_RAIDED);
			stc("{WYour clan recalls victoriously, having succsessfully defendend your Clan Hall against the raiders.{x\n\r",fch);
			sprintf(buf,"{WYou also have quite a bit in the way of spoils of war, courtesy of the %s{W raiders{x\n\r",clan_table[raid_info.clan_raider].who_name);
			stc(buf,fch);
		}
	}
	clear_raid_info( );
}

void clear_raid_info( void )
{
	raid_info.raiding = 0;
	raid_info.clan_raider = 0;
	raid_info.clan_defender = 0;
	raid_info.raiders = 0;
	raid_info.defenders = 0;
	raid_info.time_before_raid = 0;
	raid_info.prep_time = 0;
	raid_info.status = RAID_NONE;
}

void calc_win_raiders( void )
{
	int sum_levels = 0;
	long qp_reward = 0;
	int num_raiders	= 0;
	int cps			= 0;
	int i;
	char buf[MSL];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *fch;

	i = raid_info.clan_defender;
	for(d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
		{
			continue; 
		}
		if (IS_SET(fch->plr2, PLR2_RAIDER))
		{
			sum_levels += fch->level;
			num_raiders ++;
		}
	}
	if (clan_table[i].clan_points >= 50 )
	{
		cps = (clan_table[i].clan_points/ (sum_levels/100));
		qp_reward = (cps/num_raiders) * 100;
		clan_table[i].clan_points -= cps;
	}
	else
	{
		cps = (clan_table[raid_info.clan_defender].clan_points 
			/ (sum_levels 
			/ (7500/sqrt(clan_table[raid_info.clan_defender].clan_points)))
			/ num_raiders);
		qp_reward = (cps/num_raiders) * 100;
		clan_table[i].clan_points -= cps + (1 * num_raiders);
	}
	sprintf(buf,"{WThe %s{W stole %d CPs in their surprise raid!{x\n\r",
		clan_table[raid_info.clan_raider].who_name,
		cps);
	raid_channel(buf);
	for(d = descriptor_list; d != NULL; d = d->next)
	{
		fch = d->original ? d->original : d->character;
		if ( !fch || !fch->plr2) 
		{
			continue; 
		}
		if (IS_SET(fch->plr2, PLR2_RAIDER))
		{
			sprintf(buf,"%d QPs are your reward\n\r",qp_reward);
			stc(buf,fch);
			fch->questpoints += qp_reward;
		}
	}
	return;
}

			

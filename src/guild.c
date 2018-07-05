/* -----------------------------------------------------------------------
The following snippet was written by Gary McNickle (dharvest) for

from: gary@dharvest.com
website: http://www.dharvest.com
or http://www.dharvest.com/resource.html (rom related)
----------------------------------------------------------------------- */

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


/**  file   : guild.c
*  author : Gary McNickle (gary@dharvest.com)
*  purpose: Meant to provide a guild system configurable without 
*           recompiling. This is done via an ASCII file called "guild.dat"  
*           that you can edit or create with any text editor 
*           (vi/pico/jed/jaret/etc).  All gulid/clan related commands from 
*           ROM 2.4 have been moved to this file in an effort to keep 
*           related code located all in one spot.
*/

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
#include <malloc.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "arena.h"


/** Function: guild_bit_name
* Descr   : Returns the ascii name of a GUILD_ bit vector.
* Returns : (char *)
* Syntax  : (n/a- called by system)
* Written : v1.0 3/98
* Author  : Gary McNickle <gary@dharvest.com>
*/
char *guild_bit_name( int guild_flags )
{
	static char buf[512];

	buf[0] = '\0';
	if ( guild_flags & GUILD_INDEPENDENT	) strcat( buf, " independent"	);
	if ( guild_flags & GUILD_CHANGED	) strcat( buf, " changed"	);
	if ( guild_flags & GUILD_DELETED	) strcat( buf, " deleted"	);
	if ( guild_flags & GUILD_IMMORTAL	) strcat( buf, " immortal"	);

	return ( buf[0] != '\0' ) ? buf+1 : "none";
}



/** Function: load_guilds
* Descr   : Loads the information specific to each guild/clan from
*           the ../data/guild.dat file.
* Returns : (n/a)
* Syntax  : (n/a)
* Written : v1.0 1/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

void load_guilds(void)
{
	FILE *fp;
	char buf[MSL];
	char *string;
	int count = 0;
	int i;
	bool fMatch = FALSE;

	for (i=0; i < MAX_CLAN; i++)
	{
		clan_table[i].name = "";
		clan_table[i].who_name = "";
		clan_table[i].room[0]= 0;
		clan_table[i].room[1]= 0;
		clan_table[i].room[2]= 0;
		clan_table[i].threshold = 0;
		clan_table[i].prep_room = 0;
		clan_table[i].maze_begin = 0;
		clan_table[i].maze_mid = 0;
		clan_table[i].maze_end = 0;
		clan_table[i].rank[0].rankname = "";
		clan_table[i].rank[0].skillname = "";
		clan_table[i].ml[0] = 0;
		clan_table[i].ml[1] = 0;
		clan_table[i].ml[2] = 0;
		clan_table[i].ml[3] = 0;
		clan_table[i].flags = 0;
		clan_table[i].gold  = 0;
		clan_table[i].qp	  = 0;
		clan_table[i].clan_points = 0;
		clan_table[i].member = NULL;
	}

	sprintf(buf, "%sguild.dat", DATA_DIR);

	if ((fp = fopen(buf, "r")) == NULL)
	{
		log_string("Error: guild.dat file not found!");
		exit(1);
	}
	for (;;)
	{
		string = feof(fp) ? "End" : fread_word(fp);

		if (!str_cmp(string, "End"))
			break;

		switch (UPPER(string[0]))
		{
		case 'B':
			clan_table[count].gold = fread_number(fp);
			break;
		case 'C':
			if (!str_cmp(string, "Clan_points"))
			{
				clan_table[count].clan_points = fread_flag( fp );
				fMatch = TRUE;
			}
			break;

		case 'F':
			if (!str_cmp(string, "Flags"))
			{
				clan_table[count].flags  = fread_flag( fp );
				fMatch = TRUE;
			}
			if (!str_cmp(string, "File"))
			{
				clan_table[count].file_name  = fread_string( fp );
				fMatch = TRUE;
			}
			break;

		case 'G':
			count++;
			clan_table[count].name = fread_string(fp);
			fMatch = TRUE;
			break;

		case 'M':			
			if (!str_cmp(string, "Maze"))
			{
				clan_table[count].maze_begin = fread_number(fp);
				clan_table[count].maze_mid = fread_number(fp);
				clan_table[count].maze_end = fread_number(fp);	
				fMatch = TRUE;
			}
			if (!str_cmp(string, "ML"))
			{
				clan_table[count].ml[0] = fread_number(fp);
				clan_table[count].ml[1] = fread_number(fp);
				clan_table[count].ml[2] = fread_number(fp);
				clan_table[count].ml[3] = fread_number(fp);
				fMatch = TRUE;
			}
			if (!str_cmp(string, "Memb"))
			{
				MEMBER_DATA *pmem;
				int leader = FALSE;
				pmem = new_member();
				pmem->level	= fread_number( fp );
				pmem->rank   = fread_number( fp );
				leader = fread_number(fp);
				if (leader)
					pmem->is_ldr = TRUE;
				else
					pmem->is_ldr = FALSE;
				pmem->name	= fread_string(fp);
				member_to_guild(get_clan_data(count),pmem);
				fMatch = TRUE;
			}
			break;

		case 'P':
			if (!str_cmp(string, "Prep"))
			{
				clan_table[count].prep_room = fread_number(fp);	/*prep_room*/
				fMatch = TRUE;
			}
			break;

		case 'Q':
			clan_table[count].qp = fread_number(fp);
			break;

		case 'R':
			if (!str_cmp(string, "Rooms"))
			{
				clan_table[count].room[0] = fread_number(fp);	/* hall   */
				clan_table[count].room[1] = fread_number(fp);	/* morgue */
				clan_table[count].room[2] = fread_number(fp);	/* temple */
				/* clan_table[count].room[3] = fread_number(fp);  threshold */
				fMatch = TRUE;
			} 
			else if (!str_cmp(string, "Rank"))
			{
				i = fread_number(fp);
				clan_table[count].rank[i - 1].rankname = fread_string(fp);
				fMatch = TRUE;
			}
			break;

		case 'S':
			i = fread_number(fp);
			clan_table[count].rank[i - 1].skillname = fread_string(fp);
			fMatch = TRUE;
			break;

		case 'T':
			if (!str_cmp(string, "Threshold"))
			{
				clan_table[count].threshold = fread_number(fp); /* threshold */
				fMatch = TRUE;
			}
			break;

		case 'W':
			clan_table[count].who_name = fread_string(fp);
			fMatch = TRUE;
			break;

		}/* end of switch */

	}				/* end of while (!feof) */

	if (!fMatch)
	{
		bug("Fread_guilds: no match.", 0);
		fread_to_eol(fp);
	}
	fclose(fp);
	return;
} /* end: load_guilds */



/** Function: is_leader
* Descr   : Validates guild leadership of (ch).
* Returns : True (yes, ch is a mortal leader) or False
* Syntax  : (n/a)
* Written : v1.1 3/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

bool is_leader(CHAR_DATA * ch)
{
	/* update by: Zak Johnson (jakj@usa.net) Thanks Zak! */
	if (IS_SET(ch->act, PLR_MORTAL_LEADER))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} /* end: is_leader */



/** Function: can_guild
* Descr   : Validates ability of (ch) to guild other players into
*         : a specific (or generic, if implementor) guild.
* Returns : TRUE/FALSE
* Syntax  : (n/a)
* Written : v1.1 2/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

bool can_guild(CHAR_DATA * ch)
{
	/* ok if ch is a SUPREME or higher */
	if (ch->level >= SUPREME || ch->trust >= SUPREME)
	{
		return TRUE;
	}

	/* not ok if ch is not guilded or is not a mortal leader */
	if (ch->clan == 0 || !is_leader(ch))
	{
		return FALSE;
	}

	return clan_table[ch->clan].ml[0];
} /* end: can_guild */



/** Function: can_deguild
* Descr   : Validates ability of (ch) to de-guild players from a 
*         : particular guild, or generic (if ch=implementor)
* Returns : TRUE/FALSE
* Syntax  : (n/a)
* Written : v1.1 2/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

bool can_deguild(CHAR_DATA * ch)
{
	/* ok if ch is a SUPREME or higher */
	if (ch->level >= SUPREME || ch->trust >= SUPREME)
		return TRUE;

	/* not ok if ch is not guilded or is not a mortal leader */
	if (ch->clan == 0 || !is_leader(ch))
		return FALSE;

	return clan_table[ch->clan].ml[1];
} /* end: can_deguild */



/** Function: can_promote
* Descr   : Validates ability of (ch) to promote players in their guilds
* Returns : True/False
* Syntax  : (n/a)
* Written : v1.1 2/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

bool can_promote(CHAR_DATA * ch)
{
	/* ok if ch is a SUPREME or higher */
	if (ch->level >= SUPREME || ch->trust >= SUPREME)
		return TRUE;

	/* not ok if ch is not guilded or is not a mortal leader */
	if (ch->clan == 0 || !is_leader(ch))
		return FALSE;

	/* is a mortal leader, but do they have the right? */
	return clan_table[ch->clan].ml[2];
} /* end: can_promote */



/** Function: can_demote
* Descr   : Validates ability of (ch) to demote a player within their guild.
* Returns : True/False
* Syntax  : (n/a)
* Written : v1.1 2/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

bool can_demote(CHAR_DATA * ch)
{
	/* ok if ch is a SUPREME or higher */
	if (ch->level >= SUPREME || ch->trust >= SUPREME)
		return TRUE;

	/* not ok if ch is not guilded or is not a mortal leader */
	if (ch->clan == 0 || !is_leader(ch))
		return FALSE;

	return clan_table[ch->clan].ml[3];
} /* end: can_demote */




/** Function: is_clan
* Descr   : Returns wether or not (ch) is a member of any guild/clan
* Returns : True/False
* Syntax  : (n/a)
* Written : v1.0 ?
* Author  : Rom Consortium. 
*/

bool is_clan(CHAR_DATA * ch)
{
	return ch->clan;
} /* end: is_clan */




/** Function: is_same_clan
* Descr   : Returns wether or not (ch) and (victim) are members of
*         : the same guild/clan
* Returns : True/False
* Syntax  : (n/a)
* Written : v1.2 1/98
* Author  : Rom Consortium. Updated by Gary McNickle <gary@dharvest.com>
*/

bool is_same_clan(CHAR_DATA * ch, CHAR_DATA * victim)
{
	if (IS_SET(clan_table[ch->clan].flags,GUILD_INDEPENDENT))
		return FALSE;
	else
		return (ch->clan == victim->clan);
} /* end: is_same_clan */



/** Function: clan_lookup
* Descr   : Returns the numeric position of a clan name within the
*         : clan_table 
* Returns : (int)
* Syntax  : (n/a)
* Written : v1.0 
* Author  : Rom Consortium
*/

int clan_lookup(const char *name)
{
	int clan;

	for (clan = 0; clan < MAX_CLAN; clan++)
	{
		if (!str_prefix(name, clan_table[clan].name))
			return clan;
	}

	return 0;
} /* end: clan_lookup */

/** Function: player_rank
* Descr   : Returns the players (ch) rank as a string.
* Returns : char*
* Syntax  : (n/a)
* Written : v1.0 1/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

char *player_rank(CHAR_DATA * ch)
{
	if (ch->clan == 0)
		return '\0';
	return clan_table[ch->clan].rank[ch->rank].rankname;
} /* end: player_rank */


/** Function: player_clan
* Descr   : Returns the name of the players (ch) clan as a string
* Returns : char*
* Syntax  : (n/a)
* Written : v1.0 1/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

char *player_clan(CHAR_DATA * ch)
{
	if (ch->clan == 0)
		return '\0';
	return clan_table[ch->clan].name;
} /* end: player_clan */


/* Function: do_promote
* Descr   : Promotes (or demotes) a character within their own guild
* Returns : List of ranks if given no argument
* Syntax  : promote <who> <rank #>
* Written : v1.5 3/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

void do_promote(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MIL];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	MEMBER_DATA *pmem;
	int cnt;
	int sn = 0;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (!can_promote(ch))
	{
		send_to_char("huh?\n\r", ch);
		return;
	}

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		/*
		* --------------------------------------------------------------
		* Keep in mind that we are displaying the ranks as 1 - MAX_RANK,
		* so, since the structure is actually 0 - MAX_RANK-1, we need to
		* set "cnt" to cnt-1.
		* -------------------------------------------------------------- 
		*/

		send_to_char("Syntax: promote <who> <rank #>\n\r", ch);
		send_to_char("where rank is one of the following:\n\r", ch);

		for (cnt = 0; cnt < MAX_RANK; cnt++)
		{
			sprintf(buf, "%2d] %s\n\r", cnt + 1,
				is_clan(ch) ? clan_table[ch->clan].rank[cnt].rankname : "(None)");
			send_to_char(buf, ch);
		}
		send_to_char("\n\r", ch);
		return;
	}				/* end syntax */

	if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("They aren't playing.\n\r", ch);
		return;
	}

	if (!is_clan(victim))
	{
		send_to_char("They are not a member of any guilds!\n\r", ch);
		return;
	}

	if (!is_same_clan(ch, victim) &&
		(ch->level < SUPREME))
	{
		send_to_char("They are a member of a guild different than yours!\n\r", ch);
		return;
	}

	if (!str_prefix(arg2, "leader") && ch != victim)
	{
		SET_BIT(victim->act, PLR_MORTAL_LEADER);
		send_to_char("They are now a mortal leader.\n\r", ch);
		send_to_char("You have just been promoted to a leader of your guild!\n\r", victim);
		return;
	}

	cnt = atoi(arg2) - 1;
	if (cnt < 0 ||
		cnt > MAX_RANK -1 ||
		clan_table[victim->clan].rank[cnt].rankname == NULL)
	{
		send_to_char("That rank does not exist!", ch);
		return;
	}
	if (cnt > victim->rank && ((ch == victim) & (!IS_IMMORTAL(ch))))
	{
		send_to_char("Heh. I dont think so...", ch);
		return;
	}

	/** bug report by: Zak Jonhnson (zakj@usa.net)
	* we were checking ch->rank here..*sigh* Thanks Zak!
	*/
	if (cnt > victim->rank)
	{
		sprintf(buf, "You have been promoted to %s!\n\r",
			clan_table[victim->clan].rank[cnt].rankname);
		send_to_char(buf, victim);

		sprintf(buf, "%s has been promoted to %s!\n\r",
			victim->name, clan_table[victim->clan].rank[cnt].rankname);
		send_to_char(buf, ch);
		pmem = member_find(get_clan_data(victim->clan),victim->name);
		sprintf(buf,"%s is the name.\n\r",pmem->name);
		pmem->rank = cnt;
		pmem->level = victim->level;
	}
	else if (cnt < victim->rank)
	{
		if (IS_SET(victim->act, PLR_MORTAL_LEADER))
			REMOVE_BIT(victim->act, PLR_MORTAL_LEADER);

		sprintf(buf, "You have been demoted to %s!\n\r",
			clan_table[victim->clan].rank[cnt].rankname);

		send_to_char(buf, victim);
		sprintf(buf, "%s has been demoted to %s!\n\r",
			victim->name, clan_table[victim->clan].rank[cnt].rankname);

		send_to_char(buf, ch);
		/*
		* ---------------------------------------------------------------
		* Note: I dont think it would be fair here to take away any skills
		* the victim may have earned at a higher rank. It makes no RP sense
		* to do so and only hurts the player (loss of practices etc). Imms
		* may want to keep an eye on this, as we dont want players jumping
		* guilds just to gain new skills.
		* -------------------------------------------------------------- 
		*/
	}				/* else no change */
	victim->rank = cnt;
	return;
} /* end: do_promote */


/** Function: do_guild
* Descr   : Adds a character to the membership of a guild, or optionally,
*         : removes them from a guild.
* Returns : (n/a)
* Syntax  : guild <who> <clan name>
* Written : v1.3 3/98
* Author  : Gary McNickle <gary@dharvest.com>
*/

void do_guild(CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	MEMBER_DATA *pmem;
	int clan;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (!can_guild(ch))
	{
		send_to_char("You cant guild anyone\n\r", ch);
		return;
	}

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: guild <char> <cln name>\n\r", ch);
		return;
	}

	if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("They aren't playing.\n\r", ch);
		return;
	}

	/** thanks to Zanthras for the bug fix here...*/
	if (is_clan(victim) && !is_same_clan(ch, victim) &&
		((ch->level < SUPREME) & (ch->trust < SUPREME)))
	{
		send_to_char("They are a member of a guild other than your own.\n\r", ch);
		return;
	}

	if (!str_prefix(arg2, "none") 
		&& ((ch->level <= 101 && is_same_clan(ch,victim)) 
		|| (ch->level >= 108)))
	{
		send_to_char("They are no longer a member of any guild.\n\r", ch);
		send_to_char("You are no longer a member of any guild!\n\r", victim);
		pmem = member_find(get_clan_data(victim->clan),victim->name);
		member_remove(get_clan_data(victim->clan),pmem);
		victim->clan = 0;
		victim->rank = 0;

		/* add by: Zak Jonhson (zakj@usa.net) */
		if (IS_SET(victim->act, PLR_MORTAL_LEADER))
		{
			REMOVE_BIT(victim->act, PLR_MORTAL_LEADER);
		}
		return;
	}

	clan = clan_lookup(arg2);
	if (clan == 0)
	{
		send_to_char("No such guild exists.\n\r", ch);
		return;
	}
	if(clan != ch->clan && ch->level <= 109)
	{
		stc("Attempting to guild someone into a clan other than your own is prohibited\n\r",ch);
		return;
	}

	if (is_clan(victim))
	{
		pmem = member_find(get_clan_data(victim->clan),victim->name);
		member_remove(get_clan_data(victim->clan),pmem);
	}
	sprintf(buf, "They are now a %s of the %s.\n\r",
		clan_table[clan].rank[0].rankname, clan_table[clan].name);
	send_to_char(buf, ch);

	sprintf(buf, "You are now a %s of the %s.\n\r",
		clan_table[clan].rank[0].rankname, clan_table[clan].name);
	send_to_char(buf, victim);

	victim->clan = clan;
	victim->rank = 0;		/* lowest, default */

	pmem = new_member();
	pmem->name = str_dup(victim->name);
	pmem->level = victim->level;
	pmem->rank = 0;
	pmem->is_ldr = FALSE;
	member_to_guild(get_clan_data(clan),pmem);
}



void do_cache(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int value;
	double qp_to_cp = 0.01;
	double gold_to_cp = 0.00001;
	long cps;

	argument = one_argument(argument,arg);
	argument = one_argument( argument, arg2 );


	if (arg[0] == '\0')
	{
		send_to_char("Syntax:\n\r",ch);
		send_to_char("  cache gold <value>\n\r",ch);
		send_to_char("  cache qp   <value>\n\r",ch);
		return;
	}

	if (!str_prefix(arg,"gold"))
	{
		value = is_number( arg2 ) ? atoi( arg2 ) : -1;
		if (!is_clan(ch))
		{
			send_to_char("You arent part of a clan",ch);
			return;
		}
		if ( arg2[0] == '\0')
		{
			send_to_char("You have to cache a certain ammount of gold",ch);
			return;
		}
		if ( value > 0)
		{
			if (ch->gold >= value)
			{
				cps = (value * gold_to_cp);
				sprintf(buf,"You have donated %d gold to the %s's gold cache, ",value,clan_table[ch->clan].who_name);
				send_to_char(buf,ch);
				sprintf(buf,"for a total of %d CPs\n\r",cps);
				stc(buf,ch);
				ch->gold -= value;
				clan_table[ch->clan].clan_points += cps;
				save_guilds();
				save_guilds_new();
				return;
			}
			sprintf(buf,"You dont have %d gold to donate",value);
			send_to_char(buf,ch);
			return;
		}
		if (value <= 0 )
		{
			sprintf(buf,"You cannot with draw money from %s's cache\n\r",clan_table[ch->clan].who_name);
			send_to_char(buf,ch);
			return;
		}
	}
	else if (!str_prefix(arg,"qp"))
	{
		value = is_number( arg2 ) ? atoi( arg2 ) : -1;
		if (!is_clan(ch))
		{
			send_to_char("You arent part of a clan",ch);
			return;
		}
		if ( arg2[0] == '\0')
		{
			send_to_char("You have to cache a certain ammount of qps\n\r",ch);
			return;
		}
		if ( value > 0)
		{
			if (ch->questpoints >= value)
			{
				cps = (value * qp_to_cp);
				sprintf(buf,"You have donated %d qps to the %s's QP cache, ",value,clan_table[ch->clan].who_name);
				send_to_char(buf,ch);
				sprintf(buf,"for a total of %d CPs\n\r",cps);
				stc(buf,ch);
				ch->questpoints -= value;
				clan_table[ch->clan].clan_points += cps;
				save_guilds();
				save_guilds_new();
				return;
			}
			sprintf(buf,"You dont have %d qps to donate\n\r",value);
			send_to_char(buf,ch);
			return;
		}
		if (value <= 0 )
		{
			sprintf(buf,"You cannot with draw money from %s's cache\n\r",clan_table[ch->clan].who_name);
			send_to_char(buf,ch);
			return;
		}
	}
	else
	{
		send_to_char("Syntax:\n\r",ch);
		send_to_char("  cache gold <value>\n\r",ch);
		send_to_char("  cache qp   <value>\n\r",ch);
		return;
	}
}


void do_guild_show(CHAR_DATA * ch, char *argument)
{
	char buf[MSL];
	int i;
	char arg1[MAX_INPUT_LENGTH];
	int clan;
	int num = 0;

	argument = one_argument(argument, arg1);

	if (arg1[0] == '\0')
	{
		send_to_char("Syntax: guildshow <cln name>\n\r", ch);
		stc(		 "    or: guildshow all\n\r",ch);
		return;
	}

	if (!str_prefix(arg1,"all"))
	{
		for( num = 0; num <= MAX_CLAN; num++)
		{
			if(clan_table[num].name != '\0')
			{
				sprintf(buf, "%-50.50s %s{x\n\r",
					clan_table[num].name,
					clan_table[num].who_name);
				stc(buf,ch);
			}
		}
		return;
	}

	if ((clan = clan_lookup(arg1)) == 0)
	{
		send_to_char("No such guild exists.\n\r", ch);
		return;
	}
	clan = clan_lookup(arg1);
	sprintf(buf, "{YName: %s{x\n\r{YWho Name: %-10s{x\n\r",
		clan_table[clan].name,
		clan_table[clan].who_name);
	send_to_char(buf,ch);

	sprintf(buf, "{YCPs: {D[{W%d{D]{x\n\r",clan_table[clan].clan_points);
	stc(buf,ch);

	sprintf(buf, " {C#    {YRank\n\r{G--------------------------------------------------------------{x\n\r");
	send_to_char(buf,ch);
	for (i=0; i < MAX_RANK; i++)
	{
		sprintf(buf,"{D[{C%d{D] {Y%-25s{x\n\r",i+1, 
			(clan_table[clan].rank[i].rankname  != '\0') ? clan_table[clan].rank[i].rankname : "None");  
		send_to_char(buf,ch);
	}

	sprintf(buf,"\n\r");
	send_to_char(buf,ch);
}

void do_extract(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int value;
	int clan;



	argument = one_argument(argument,arg);
	argument = one_argument(argument, arg2 );

	if (arg[0] == '\0' || arg2 == '\0')
	{
		send_to_char("Syntax:\n\r",ch);
		send_to_char("  extract <clan> <value>\n\r",ch);
		return;
	}

	if ((clan = clan_lookup(arg)) == 0)
	{
		send_to_char("No such guild exists.\n\r", ch);
		return;
	}
	clan = clan_lookup(arg);

	value = is_number( arg2 ) ? atoi( arg2 ) : -1;
	if ( arg2[0] == '\0')
	{
		send_to_char("You have to extract a certain ammount of cps\n\r",ch);
		return;
	}
	if ( value > 0)
	{
		sprintf(buf,"You extract %d from %s's gold cache\n\r",value,clan_table[clan].name);
		stc(buf,ch);
		clan_table[clan].clan_points -= value;
		save_guilds();
		save_guilds_new();
		return;
	}
	if (value <= 0 )
	{
		send_to_char("Immortals cant add to a cache with the extract command\n\r",ch);
		return;
	}
	else
	{
		send_to_char("Syntax:\n\r",ch);
		send_to_char("  cache gold <value>\n\r",ch);
		send_to_char("  cache qp   <value>\n\r",ch);
		return;
	}
}

void save_guild( int i )
{
	char buf[MSL];
	FILE *fp;
	int value;
	CLAN_DATA *pClan;
	MEMBER_DATA *pmem;

	fclose( fpReserve );
	if (clan_table[i].file_name == NULL)
	{
		sprintf(buf,"%s is BROKEN!!",clan_table[i].file_name);
		wiznet(buf,NULL,NULL,0,0,0);
		return;
	}
	else
	{
		sprintf(buf, "%s%s", CLAN_DIR,clan_table[i].file_name);
		wiznet(buf,NULL,NULL,0,0,0);
		if ( ( fp = fopen( buf, "w" ) ) == NULL )
		{
			bug( "Open_clan: fopen", 0 );
			perror( buf );
			return;
		}
		if ( !IS_SET(clan_table[i].flags, GUILD_DELETED) && clan_lookup(clan_table[i].name) != 0)
		{
			fprintf(fp, "\nGuild\t%s~\n", clan_table[i].name);
			fprintf(fp, "Who\t%s~\n", clan_table[i].who_name);
			fprintf(fp, "Rooms\t%d\t%d\t%d\n", 
				clan_table[i].room[0],  
				clan_table[i].room[1],
				clan_table[i].room[2]);
			fprintf( fp,	"Balance %ld\n",	clan_table[i].gold);
			fprintf( fp,	"QP %ld\n",		clan_table[i].qp);
			fprintf( fp,	"Clan_points %ld\n",	clan_table[i].clan_points);
			/*		fprintf( fp,	"File %s~\n",	clan_table[i].file_name);*/

			for (value=0; value < MAX_RANK; value++)
			{  
				if (clan_table[i].rank[value].rankname == NULL ||
					clan_table[i].rank[value].rankname[0] == '\0')
					clan_table[i].rank[value].rankname = str_dup("Unassigned");

				fprintf(fp, "Rank\t%d\t%s~\n", 
					value+1, 
					clan_table[i].rank[value].rankname);
			}

			for (value=0; value < MAX_RANK; value++)
				if (clan_table[i].rank[value].skillname != NULL &&
					clan_table[i].rank[value].skillname[0] != '\0')
					fprintf(fp, "Skill\t%d\t%s~\n", 
					value+1, 
					clan_table[i].rank[value].skillname);

			fprintf(fp, "ML\t%d  %d  %d  %d\n", 
				clan_table[i].ml[0],
				clan_table[i].ml[1],
				clan_table[i].ml[2],
				clan_table[i].ml[3]);

			pClan = get_clan_data(i);
			for ( pmem = pClan->member; pmem != NULL; pmem = pmem->next )
			{
				int leader = 0;
				if (pmem->is_ldr)
					leader = 1;
				else 
					leader = 0;
				fprintf( fp, "Memb %d %d %d %s~\n",
					pmem->level,
					pmem->rank,
					leader,
					pmem->name);
			}
			/* Remove "changed" bit before writing flags to file */
			if (IS_SET(clan_table[i].flags, GUILD_CHANGED))
				REMOVE_BIT(clan_table[i].flags, GUILD_CHANGED);

			fprintf( fp, "Flags %s \n",  fwrite_flag( clan_table[i].flags,  buf ) );

			fprintf(fp, "\n");
			fprintf( fp, "#$\n" );

			fclose( fp );
			fpReserve = fopen( NULL_FILE, "r" );
			return;
		}
	}
}

void do_members(CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	CLAN_DATA *pClan;
	MEMBER_DATA *pmem;
	char online[MSL],ldr[MSL],buf[MSL],arg1[MIL];

	if (IS_IMMORTAL(ch) && argument[0] != '\0')
	{
		if ( argument[0] == '\0')
		{
			stc("You must enter a clan name to check the members of",ch);
			return;
		}
		argument = one_argument(argument,arg1);
		if ((pClan = get_clan_data(clan_lookup(arg1))) == 0)
		{
			send_to_char("No such guild exists.\n\r", ch);
			return;
		}
		else
		{
			pClan = get_clan_data(clan_lookup(arg1));
			if(pClan->name[0] == '\0')
			{
				ptc(ch,"The clan '%s' was not found.\n\r",arg1);
				return;
			}
			ptc(ch,"%s{x Members:\n\r",clan_table[clan_lookup(arg1)].who_name);
			for ( pmem = pClan->member; pmem != NULL; pmem = pmem->next )
			{
				if ( ( victim = get_char_world( ch, pmem->name ) ) != NULL )
					sprintf(online,"{D({GONLINE{D){x");
				else
					sprintf(online,"{D({ROFFLINE{D){x");
				if (pmem->is_ldr)
					sprintf(ldr,"{D({WLdr{D){x");
				else
					sprintf(ldr," ");
				sprintf( buf, "{G%15s {D[%3d{D]{x is a: %15s	%5s	%s\n\r", 
					pmem->name,
					pmem->level,
					clan_table[clan_lookup(arg1)].rank[pmem->rank].rankname,
					ldr,
					online);
				send_to_char( buf, ch );
			}
			return;
		}
		return;
	}
	else if (is_clan(ch))
	{
		pClan = get_clan_data(ch->clan);
		for ( pmem = pClan->member; pmem != NULL; pmem = pmem->next )
		{
			if ( ( victim = get_char_world( ch, pmem->name ) ) != NULL )
				sprintf(online,"{D({GONLINE{D){x");
			else
				sprintf(online,"{D({ROFFLINE{D){x");
			if (pmem->is_ldr)
				sprintf(ldr,"{D({WLdr{D){x");
			else
				sprintf(ldr," ");
			sprintf( buf, "{G%15s {D[%3d{D]{x is a:\t%15s %5s\t%s\n\r", 
				pmem->name,
				pmem->level,
				clan_table[ch->clan].rank[pmem->rank].rankname,
				ldr,
				online);
			send_to_char( buf, ch );
		}
	}
}

void member_to_guild( CLAN_DATA *pClan, MEMBER_DATA *pmem )
{
	MEMBER_DATA *pmem_new;

	pmem_new = new_member();

	*pmem_new		= *pmem;

	VALIDATE(pmem);	/* in case we missed it when we set up pmem */
	pmem_new->next	= pClan->member;
	pClan->member	= pmem_new;
	return;
}
/* stuff for recycling affects */
MEMBER_DATA *member_free;

MEMBER_DATA *new_member(void)
{
	MEMBER_DATA *member;

	if (member_free == NULL)
		member = alloc_perm(sizeof(*member));
	else
	{
		member = member_free;
		member_free = member_free->next;
	}
	VALIDATE(member);
	return member;
}
void free_member(MEMBER_DATA *member)
{
	if (!IS_VALID(member))
		return;

	INVALIDATE(member);
	member->next = member_free;
	member_free = member;
}
MEMBER_DATA  *member_find(CLAN_DATA *pClan, char *name)
{
	MEMBER_DATA *pmem_find;

	for ( pmem_find = pClan->member; pmem_find != NULL; pmem_find = pmem_find->next )
	{
		if ( !str_cmp( pmem_find->name, name ) )
			return pmem_find;
	}
	return NULL;
}

void updated_member(CHAR_DATA *ch)
{
	MEMBER_DATA *pmem;

	if (!is_clan(ch))
		return;
	else
	{
		pmem = member_find(get_clan_data(ch->clan),ch->name);
		pmem->name = str_dup(ch->name);
		pmem->level = ch->level;
		pmem->rank	= ch->rank;
		if (IS_SET(ch->act,PLR_MORTAL_LEADER))
			pmem->is_ldr = TRUE;
		else
			pmem->is_ldr = FALSE;
	}
}

void do_delmem(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	MEMBER_DATA *pmem;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Delete who's membership?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}
	if (!is_clan(victim))
	{
		stc("They're not in a clan\n\r",ch);
		return;
	}
	sprintf(buf,"Searching for %s's member entry\n\r",ch->name);
	stc(buf,ch);
	pmem = member_find(get_clan_data(victim->clan),victim->name);
	if (pmem != NULL)
	{
		member_remove(get_clan_data(victim->clan),pmem);
		stc("Member deleted\n\r",ch);
		return;
	}
	else
	{
		stc("Member not found\n\r",ch);
		return;
	}
}
void member_remove( CLAN_DATA *pclan, MEMBER_DATA *pmem )
{
	if ( pmem == pclan->member )
		pclan->member	= pmem->next;
	else
	{
		MEMBER_DATA *prev;

		for ( prev = pclan->member; prev != NULL; prev = prev->next )
		{
			if ( prev->next == pmem )
			{
				prev->next = pmem->next;
				break;
			}
		}

		if ( prev == NULL )
		{
			bug( "member_remove: cannot find pmem.", 0 );
			return;
		}
	}
	free_member(pmem);
	return;
}


/********************************************************************************
 * Stat List code copyright 1999-2001                                           *
 * Markanth : markanth@spaceservices.net                                        *
 * Devil's Lament : spaceservices.net port 3778                                 *
 * Web Page : http://spaceservices.net/~markanth/                               *
 *                                                                              *
 * All I ask in return is that you give me credit on your mud somewhere         *
 * or email me if you use it.                                                   *
 ********************************************************************************/
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

STAT_DATA *stat_list;
long		TopGameStat[MAX_GAMESTAT];  

void save_statlist( void )
{
	STAT_DATA *pstat;
	FILE *fp;
	bool found = FALSE;
	int i;

	if ((fp = fopen(STAT_FILE, "w")) == NULL)
	{
		perror(STAT_FILE);
	}

	fprintf(fp, "TOPSTATS %d ", MAX_GAMESTAT);
	for (i = 0; i < MAX_GAMESTAT; i++)
		fprintf(fp, "%ld ", TopGameStat[i]);
	fprintf(fp, "\n");
	for (pstat = stat_list; pstat != NULL; pstat = pstat->next)
	{
		found = TRUE;
		fprintf(fp, "%s ", pstat->name);
		for (i = 0; i < MAX_GAMESTAT; i++)
			fprintf(fp, "%ld ", pstat->gamestat[i]);
		fprintf(fp, "\n");
	}
	fclose(fp);
	if (!found)
		unlink(STAT_FILE);
}

void load_statlist(void)
{
	FILE *fp;
	STAT_DATA *stat_last;
	int i,
		maxStat = 0;

	if ((fp = fopen(STAT_FILE, "r")) == NULL)
		return;

	if (!str_cmp(fread_word(fp), "TOPSTATS"));
	{
		maxStat = fread_number(fp);
		for (i = 0; i < maxStat; i++)
			TopGameStat[i] = fread_number(fp);
	}

	stat_last = NULL;
	for (;;)
	{
		STAT_DATA *pstat;

		if (feof(fp))
		{
			fclose(fp);
			return;
		}

		pstat = new_stat_data();
		pstat->name = str_dup(fread_word(fp));
		for (i = 0; i < maxStat; i++)
			pstat->gamestat[i] = fread_number(fp);
		fread_to_eol(fp);

		if (stat_list == NULL)
			stat_list = pstat;
		else
			stat_last->next = pstat;
		stat_last = pstat;
	}
}

void update_statlist(CHAR_DATA * ch, bool delete)
{
	STAT_DATA *prev;
	STAT_DATA *curr;
	int i;

	if (IS_NPC(ch) || IS_IMMORTAL(ch))
		return;

	prev = NULL;

	for (curr = stat_list; curr != NULL; prev = curr, curr = curr->next)
	{
		if (!str_cmp(ch->name, curr->name))
		{
			if (prev == NULL)
				stat_list = stat_list->next;
			else
				prev->next = curr->next;

			//free_stat_data(curr);
			save_statlist();
		}
	}
	if (delete)
	{
		return;
	}

	curr = new_stat_data();
	curr->name = str_dup(ch->name);
	/*update stats according to char_data*/
	ch->pcdata->gamestat[QUEST_POINTS]	= ch->questpoints;
	ch->pcdata->gamestat[PK_KILLS]		= ch->pcdata->pk_kills;
	ch->pcdata->gamestat[PK_DEATHS]		= ch->pcdata->pk_deaths;
	ch->pcdata->gamestat[ARENA_WINS]	= ch->pcdata->arena_wins;
	ch->pcdata->gamestat[ARENA_LOSSES]	= ch->pcdata->arena_losses;
	ch->pcdata->gamestat[HC_WINS]		= ch->pcdata->hc_wins;
	ch->pcdata->gamestat[HC_LOSSES]		= ch->pcdata->hc_losses;
	ch->pcdata->gamestat[GOLD]			= ch->gold + ch->balance;
	for (i = 0; i < MAX_GAMESTAT; i++)
	{
		curr->gamestat[i] = ch->pcdata->gamestat[i];
		if (curr->gamestat[i] > TopGameStat[i])
			TopGameStat[i] = curr->gamestat[i];
	}

	curr->next = stat_list;
	stat_list = curr;
	save_statlist();
	return;
}

void do_showstats(CHAR_DATA * ch, char *argument)
{
	int option;
	char arg[MIL];
	char buf[MSL];

	argument = one_argument(argument, arg);

	if (IS_NULLSTR(arg))
	{
		stc("      {ROPTIONS AVAILABLE:{x\n\r", ch);
		stc("      {G0{x - Game Stats on Demon's Dance (gstats)\n\r", ch);
		stc("      {G1{x - Ranking of Player Killers (pkills)\n\r", ch);
		stc("      {G2{x - Ranking of Player Deaths (pdeaths)\n\r", ch);
		stc("      {G3{x - Ranking of Mob Kills (mkills)\n\r", ch);
		stc("      {G4{x - Ranking of Mob Deaths (mdeaths)\n\r", ch);
		stc("      {G5{x - Ranking of Quests Completed (qcomplete)\n\r", ch);
		stc("      {G6{x - Ranking of Quests Failed (qfail)\n\r", ch);
		stc("      {G7{x - Ranking of Quest Points Earned (qpoints)\n\r", ch);
		stc("      {G8{x - Ranking of Arena Wins (awins)\n\r", ch);
		stc("      {G9{x - Ranking of Arena Losses (alosses)\n\r", ch);
		stc("      {G10{x - Ranking of Honor Circle Wins (hcwins)\n\r", ch);
		stc("      {G11{x - Ranking of Honor Circle Losses (hclosses)\n\r", ch);
		stc("      {G12{x - Ranking of Richness (gold)\n\r", ch);
		if(IS_IMMORTAL(ch))
			stc("      {Gdelete <name>{x - deletes from statlist\n\r", ch);
		return;
	}
	option = atoi(arg);

	if (!str_cmp(arg, "delete") && IS_IMMORTAL(ch))
	{
		STAT_DATA *prev = NULL;
		STAT_DATA *curr = NULL;
		bool found = FALSE;

		for (curr = stat_list; curr != NULL; prev = curr, curr = curr->next)
		{
			if (!str_cmp(argument, curr->name))
			{
				if (prev == NULL)
					stat_list = stat_list->next;
				else
					prev->next = curr->next;

				free_stat_data(curr);
				save_statlist();
				found = TRUE;
			}
		}
		if (!found)
		{
			sprintf(buf, "Error deleting %s.\n\r", argument);
			stc(buf,ch);
		}
	} 
	else if (option == 1 || !str_prefix(arg, "pkills"))
		show_game_stats(ch, PK_KILLS);
	else if (option == 3 || !str_prefix(arg, "mkills"))
		show_game_stats(ch, MOB_KILLS);
	else if (option == 2 || !str_prefix(arg, "pdeaths"))
		show_game_stats(ch, PK_DEATHS);
	else if (option == 4 || !str_prefix(arg, "mdeaths"))
		show_game_stats(ch, MOB_DEATHS);
	else if (option == 5 || !str_prefix(arg, "qcomplete"))
		show_game_stats(ch, QUESTS_COMPLETE);
	else if (option == 6 || !str_prefix(arg, "qfail"))
		show_game_stats(ch, QUESTS_FAIL);
	else if (option == 7 || !str_prefix(arg, "qpoints"))
		show_game_stats(ch, QUEST_POINTS);
	else if (option == 8 || !str_prefix(arg, "awins"))
		show_game_stats(ch, ARENA_WINS);
	else if (option == 9 || !str_prefix(arg, "alosses"))
		show_game_stats(ch, ARENA_LOSSES);
	else if (option == 10 || !str_prefix(arg, "hcwins"))
		show_game_stats(ch, HC_WINS);
	else if (option == 11 || !str_prefix(arg, "hc_losses"))
		show_game_stats(ch, HC_LOSSES);
	else if (option == 12 || !str_prefix(arg, "gold"))
		show_game_stats(ch, GOLD);
	else
		do_showstats(ch, "");

	return;
}

void show_game_stats(CHAR_DATA * ch, int type)
{
	STAT_DATA *curr;
	char buf[MSL];
	long top;
	int count,
		pos;
	bool found = FALSE;
	const char *stat_name[MAX_GAMESTAT] = {
		"PLAYER KILLERS",
			"MOB KILLERS",
			"PK DEATHS",
			"MOB DEATHS",
			"QUESTS COMPLETED",
			"QUESTS FAILED",
			"QUEST POINTS",
			"ARENA WINS",
			"ARENA LOSSES",
			"HONOR CIRCLE WINS",
			"HONOR CIRCLE LOSSES",
			"RICHNESS"
	};
	count = 0;
	pos = 0;
	sprintf(buf, "{CRANKING OF %s{x\n\r", stat_name[type]);
	stc(buf,ch);
	for (top = TopGameStat[type]; top > 0; top--)
	{
		for (curr = stat_list; curr != NULL; curr = curr->next)
		{

			if (curr->gamestat[type] == top)
			{
				if (count++ > 24)
					break;
				sprintf(buf, "{G%2d{w){W %-20s {w[{R%8ld{W]{x\n\r", count, curr->name, curr->gamestat[type]);
				stc(buf,ch);
				found = TRUE;
			}
		}
	}

	if (!found)
	{
		sprintf(buf, "\n\rNo one found yet.\n\r");
		stc(buf,ch);
	}
	return;
}


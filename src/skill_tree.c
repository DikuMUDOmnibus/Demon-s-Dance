/****************************************************************************
*  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,			*
*  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.	*
*																			*
*  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael			*
*  Chastain, Michael Quan, and Mitchell Tse.								*
*																			*r
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
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include "db.h"
#include "arena.h"
#include "interp.h"
#include "tables.h"
#include "olc.h"
#include "recycle.h"
#include "lookup.h"
#include "skill_tree.h"
#include "math.h"

struct	node*	talloc()
{
	return (struct node *) malloc(sizeof(struct node));
}

struct node* empty()
{
	struct node* temp;
	temp = talloc();
	temp->x1 = NULL;
	temp->x2 = NULL;
	temp->x3 = NULL;
	temp->x4 = NULL;
	temp->x5 = NULL;
	temp->prefix = "+";
	temp->tn	= "combat";
	temp->level = 1;
	return temp;
}
void set_prefix(struct node* skill)
{
	switch (skill->level)
	{
	case 1:		skill->prefix = str_dup("");							break;
	case 2:		skill->prefix = str_dup(" |->");						break;
	case 3:		skill->prefix = str_dup(" |   |---->");				break;
	case 4:		skill->prefix = str_dup(" |   |      |--->");		break;
	default:	skill->prefix = str_dup(" |   |      |--->");		break;
		break;
	}

	return;
}

void do_talents( CHAR_DATA *ch, char *argument )
{
	char arg[MSL];
	char first[MIL];
	/*int i = 0;*/
	struct node* check;

	if (IS_NPC(ch))
		return;

	one_argument(argument,arg);
	pull_skill(arg,first);
	check = check_skill_tree(first,arg,ch,uber());

	if (arg[0] == '\0')
	{
		stc("Viable arguments are combat covert sorcery arcane worship or all.\n\r",ch);
		return;
	}
	//	sn = match_skill("combat.offense.piercing","newskills");
	//	sprintf(buf,"{Bmatch_skill returned:{W%s{x, and it was supposed to return com.off.pie\n\r",tree_skill_table[sn].name);
	//	stc(buf,ch);
	/*	if (!str_prefix(arg,"all"))
	{
	display_node(combat(),ch,ch);
	stc("\n\r",ch);
	display_node(covert(),ch,ch);
	stc("\n\r",ch);
	display_node(sorcery(),ch,ch);
	stc("\n\r",ch);
	display_node(arcane(),ch,ch);
	stc("\n\r",ch);stc("Update Bonus done.(uber)\n\r",ch);
	display_node(worship(),ch,ch);
	return;
	}*/
	if (check != NULL)
	{
		update_bonus(ch,combat());
		update_bonus(ch,covert());
		update_bonus(ch,sorcery());
		update_bonus(ch,arcane());
		update_bonus(ch,worship());
		stc("{WSkill                           {RLevel {GBonus   {YCost{x\n\r",ch);
		display_node(check,ch,ch);
		return;
	}
	else
		return;
}
void display_node (struct node* skill, CHAR_DATA *ch,CHAR_DATA *victim)
{
	char buf[MSL],lvl[MSL],bonus[MSL],cost[MSL];

	sprintf(lvl,"{W[{R%5d{W|{x",return_skill_str(victim,skill->tn));
	sprintf(bonus,"{G%5d{W|{x",return_bonus_str(victim,skill->tn));
	sprintf(cost,"{Y%6d{W]{x",calc_skill_cost(ch,skill->tn,1));
	sprintf(buf,"{D%s{W%s%*s {x%s%s%s\n\r",skill->prefix,skill->name,(30 - (strlen(skill->name) + strlen(skill->prefix))),"",lvl,bonus,cost);
	stc(buf,ch);
	if (skill->x1 != NULL)
		display_node(skill->x1,ch,victim);
	if (skill->x2 != NULL)
		display_node(skill->x2,ch,victim);
	if (skill->x3 != NULL)
		display_node(skill->x3,ch,victim);
	if (skill->x4 != NULL)
		display_node(skill->x4,ch,victim);
	if (skill->x5 != NULL)
		display_node(skill->x5,ch,victim);
	return;
}
struct node* check_skill_tree(char *first,char *arg,CHAR_DATA *ch,struct node *node)
{
	/*force function to jump from uber() to first()*/
	if (!strcmp(node->name,"uber"))
	{	
		if(node->x1 != NULL
			&& !strcmp(first,node->x1->tn))
			return check_skill_tree(first,arg,ch,node->x1);
		if(node->x2 != NULL
			&& !strcmp(first,node->x2->tn))
			return check_skill_tree(first,arg,ch,node->x2);
		if(node->x3 != NULL
			&& !strcmp(first,node->x3->tn))
			return check_skill_tree(first,arg,ch,node->x3);
		if(node->x4 != NULL
			&& !strcmp(first,node->x4->tn))
			return check_skill_tree(first,arg,ch,node->x4);
		if(node->x5 != NULL
			&& !strcmp(first,node->x5->tn))
			return check_skill_tree(first,arg,ch,node->x5);
		else
			return NULL;
	}
	if(!str_cmp(node->tn,arg))
		return node;
	if(node->x1 != NULL
		&& !str_prefix(node->x1->tn,arg))
		return check_skill_tree(first,arg,ch,node->x1);
	if(node->x2 != NULL
		&& !str_prefix(node->x2->tn,arg))
		return check_skill_tree(first,arg,ch,node->x2);
	if(node->x3 != NULL
		&& !str_prefix(node->x3->tn,arg))
		return check_skill_tree(first,arg,ch,node->x3);
	if(node->x4 != NULL
		&& !str_prefix(node->x4->tn,arg))
		return check_skill_tree(first,arg,ch,node->x4);
	if(node->x5 != NULL
		&& !str_prefix(node->x5->tn,arg))
		return check_skill_tree(first,arg,ch,node->x5);
	else 
		return NULL;
}
struct	node*	uber()
{
	struct node* temp;

	temp = empty();
	temp->name =	"uber";
	temp->tn	=	"uber";
	temp->x1	=	combat();
	temp->x2	=	covert();
	temp->x3	=	sorcery();
	temp->x4	=	arcane();
	temp->x5	=	worship();
	temp->level	=	1;
	return temp;
}
char *pull_skill( char *argument, char *arg_first )
{
	char cEnd;

	while ( isspace(*argument) )
		argument++;

	cEnd = '.';
	if ( *argument == '\'' || *argument == '"' )
		cEnd = *argument++;

	while ( *argument != '\0' )
	{
		if ( *argument == cEnd )
		{
			argument++;
			break;
		}
		*arg_first = LOWER(*argument);
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while ( isspace(*argument) )
		argument++;

	return argument;
}

void set_skill_levels( CHAR_DATA *ch,int level)
{
	int i;
	if(IS_NPC(ch))
		return;
	else
	{
		for(i = 0; i < MAX_TREE_SKILLS; i++)
		{
			ch->pcdata->skills_array[i] = level;
		}
	}
	return;
}

int	return_skill_sn( CHAR_DATA *ch, int sn)
{
	if (IS_NPC(ch))
		return 0;
	return ch->pcdata->skills_array[sn_talent(sn)];
}
int	return_skill_str( CHAR_DATA *ch, char *skill_name)
{
	int sn;
	if (IS_NPC(ch))
		return 0;
	sn = match_skill(skill_name,"return_skill_str");
	if (sn< 0)
	{
		debug(ch,"return_skill_str: match_skill() failed\n\r");
		return 0;
	}
	if (sn == 5000)
	{
		debug(ch,"return_skill_str: match_skill() matched 'uber'(5000)\n\r",ch);
		return 0;
	}
	return ch->pcdata->skills_array[sn];
}
int	return_bonus_sn( CHAR_DATA *ch, int sn)
{
	if (IS_NPC(ch))
		return 0;
	return ch->pcdata->bonus_array[sn_talent(sn)];
}
int	return_bonus_str( CHAR_DATA *ch, char *skill_name)
{
	int sn;
	sn = match_skill(skill_name,"return_bonus_str");
	if (IS_NPC(ch))
		return 0;
	if (sn < 0)
	{
		debug(ch,"return_bonus: match_skill() failed\n\r",ch);
		debug(ch,"skill_name %s, sn %d\n\r",skill_name,sn);
		return 0;
	}
	if (sn == 5000)
	{
		debug(ch,"return_bonus: match_skill() found UBER(5000)\n\r",ch);
		return 0;
	}
	return ch->pcdata->bonus_array[sn];
}

void do_upgrade( CHAR_DATA *ch, char *argument )
{
	char arg1[MSL],arg2[MSL],buf[MSL],first[MSL];
	/*char *token_str,*copy;
	char **tokens;*/
	CHAR_DATA *mob;
	int	level,i=0;
	struct node* to_raise;

	/*copy = strdup(argument);*/
	argument = one_argument( argument, arg1 );
	strcpy(arg2,argument);
	level = atoi(arg2);

	if ( IS_NPC(ch) )
		return;

	if ( !IS_AWAKE(ch) )
	{
		send_to_char( "In your dreams, or what?\n\r", ch );
		return;
	}

	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
	{
		if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
			break;
	}

	if ( mob == NULL )
	{
		send_to_char( "You can't do that here.\n\r", ch );
		return;
	}

	if ( ch->pcdata->sp <= 0 )
	{
		send_to_char( "You have no skill points left.\n\r", ch );
		return;
	}

	if (!is_number(arg2))
	{
		stc("Please specify a numeric number of levels to raise\n\r",ch);
		return;
	}
	pull_skill(arg1,first);
	to_raise = check_skill_tree(first,arg1,ch,uber());
	if ( to_raise == NULL)
	{
		stc("No such skill found, please check your spelling and try again.\n\r",ch);
		return;
	}
	if ( ch->pcdata->sp < calc_skill_cost(ch,arg1,level) || ch->pcdata->sp <= 0)
	{
		sprintf(buf,"You would need %d Skill Points to raise %s by that many levels\n\r",calc_skill_cost(ch,arg1,level),arg1);
		stc(buf,ch);
		return;
	}
	ch->pcdata->sp -= calc_skill_cost(ch,arg1,level);
	boost_skill(ch,arg1,level);
	update_bonus(ch,combat());
	update_bonus(ch,covert());
	update_bonus(ch,sorcery());
	update_bonus(ch,arcane());
	update_bonus(ch,worship());

	sprintf(buf,"You raise {W%s{x to {R%d{x\n\r",to_raise->tn,return_skill_str(ch,to_raise->tn));
	stc(buf,ch);

	/*
	* The following is some code for tokenizing
	* the argument string into tokens, and then
	* putting them in an array of strings.  Idea
	* turned out to be useless, cause of the way
	* check_skill_tree works (recursively),but 
	* I'm leaving the code anyways.
	*		-Enzo
	*/

	/*	if((tokens = malloc(4 * sizeof(char*) * 24)) == NULL)
	{
	stc("do_upgrade: tokens malloc failed.\n\r",ch);
	return;
	}
	for(i=0;i<=3;i++)
	{
	if((tokens[i] = malloc(24 * sizeof(char))) == NULL)
	{
	stc("do_upgrade: tokens[i] malloc failed.\n\r",ch);
	return;
	}
	tokens[i] = strdup("");
	}
	for(i=0;i<=3;i++)
	{		
	if(i==0)
	{
	token_str = strtok(copy,".");
	if(token_str == NULL || STRLEN(token_str) < 1)
	{
	stc("Your argument was invalid.\n\r",ch);
	return;
	}
	}
	if(token_str != NULL 
	&& STRLEN(token_str) >= 1 )
	{
	tokens[i] = str_dup(token_str);
	token_str = strtok(NULL," .");
	}
	if(isdigit(tokens[i][0]))
	{
	tokens[i] = str_dup("");
	}
	}
	ptc(ch,"%s[.]%s[.]%s[.]%s",tokens[0],tokens[1],tokens[2],tokens[3]);
	free(tokens);
	free(copy);
	*/
	return;
}

void	boost_skill( CHAR_DATA *ch, char *skill_name,int ammount)
{
	struct node* to_raise;
	char first[MSL];
	int sn;

	sn = match_skill(skill_name,"boost_skill");
	if (IS_NPC(ch))
		return;
	else
	{
		if (sn < 0)
		{
			stc("match_skill failed in boost_skill\n\r",ch);
			return;
		}
		if (sn == 5000)
		{
			stc("match_skill found UBER in boost_skill\n\r",ch);
			return;
		}
		else
		{
			ch->pcdata->skills_array[sn] += ammount;
		}
		pull_skill(skill_name,first);
		to_raise = check_skill_tree(first,skill_name,ch,uber());
		if (to_raise != NULL)
		{
			if (to_raise->x1 != NULL)
				boost_skill(ch,to_raise->x1->tn,ammount);
			if (to_raise->x2 != NULL)
				boost_skill(ch,to_raise->x2->tn,ammount);
			if (to_raise->x3 != NULL)
				boost_skill(ch,to_raise->x3->tn,ammount);
			if (to_raise->x4 != NULL)
				boost_skill(ch,to_raise->x4->tn,ammount);
			if (to_raise->x5 != NULL)
				boost_skill(ch,to_raise->x5->tn,ammount);
			return;
		}
		else
			return;
	}
}

//(base_cost)^2/10 - (((class_bonus * 2) + (race_bonus)) * (skill_level)/ 2.5

int calc_one_skill_cost(CHAR_DATA *ch,char *skill_name,int level)
{
	int current = return_skill_str(ch,skill_name);
	int raise = current + level;
	int total = 0;
	int skill,stat;
	int a,b,c,d,i;
	char first[MSL];

	pull_skill(skill_name,first);
	for (i = 0; i < SKILL_GROUPS;i++)
	{
		if (!strcmp(first,check_skill_group[i].name))
		{
			skill	=	check_skill_group[i].skill;
			stat	=	check_skill_group[i].stat;
			break;
		}
	}
	while (current <= raise)
	{
		a = pow(20 + current,2)/10;
		b = class_table[ch->class].skill_bonus[stat] * 2;
		c = pc_race_table[ch->race].skill_bonus[skill];
		d = current;
		total += a + ((b + c) * d/2.5);
		current ++;
	}
	return total;
}

int	calc_skill_cost(CHAR_DATA *ch,char *skill_name,int level)
{
	struct node* to_raise;
	long total = 0;
	char first[MSL];

	pull_skill(skill_name,first);
	to_raise = check_skill_tree(first,skill_name,ch,uber());
	if (to_raise != NULL)
	{
		if (to_raise->x1 == NULL 
			&& to_raise->x2 == NULL
			&& to_raise->x3 == NULL
			&& to_raise->x4 == NULL
			&& to_raise->x5 == NULL)
		{
			total = calc_one_skill_cost(ch,skill_name,level);
			return total;
		}
		else
		{
			if (to_raise->x1 != NULL)
				total += calc_skill_cost(ch,to_raise->x1->tn,level);
			if (to_raise->x2 != NULL)
				total += calc_skill_cost(ch,to_raise->x2->tn,level);
			if (to_raise->x3 != NULL)
				total += calc_skill_cost(ch,to_raise->x3->tn,level);
			if (to_raise->x4 != NULL)
				total += calc_skill_cost(ch,to_raise->x4->tn,level);
			if (to_raise->x5 != NULL)
				total += calc_skill_cost(ch,to_raise->x5->tn,level);
			return total;
		}
	}
	return total;
}
void do_remote_talents( CHAR_DATA *ch, char *argument )
{
	char arg1[MSL];
	char arg2[MSL];
	char first[MIL];
	int i = 0;
	struct node* check;
	CHAR_DATA *victim;

	if (IS_NPC(ch))
		return;

	argument = one_argument(argument,arg1);
	one_argument(argument,arg2);

	if ( arg1[0] == '\0' )
	{
		send_to_char( "Remote View whose score?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Just use score you mook\n\r", ch );
		return;
	}

	if (arg2[0] == '\0')
	{
		stc("Viable arguments are combat covert sorcery arcane worship or all.\n\r",ch);
		return;
	}
	pull_skill(arg2,first);
	check = check_skill_tree(first,arg2,victim,uber());

	if (!str_prefix(arg2,"all"))
	{
		display_node(combat(),ch,victim);
		stc("\n\r",ch);
		display_node(covert(),ch,victim);
		stc("\n\r",ch);
		display_node(sorcery(),ch,victim);
		stc("\n\r",ch);
		display_node(arcane(),ch,victim);
		stc("\n\r",ch);
		display_node(worship(),ch,victim);
		return;
	}
	else if (check != NULL)
	{
		display_node(check,ch,victim);
		return;
	}
	else
		return;
}

int	calc_bonus(CHAR_DATA *ch,struct node* node)
{
	int	skill	= 0;
	int stat	= 0;
	double current = 0;
	int i;
	int bonus = 10;
	char first[MSL];
	if (IS_NPC(ch))
		return 42;
	else
	{
		pull_skill(node->tn,first);
		for (i = 0; i < SKILL_GROUPS;i++)
		{
			if (!strcmp(first,check_skill_group[i].name))
			{
				skill	=	check_skill_group[i].skill;
				stat	=	check_skill_group[i].stat;
				break;
			}
		}
		current = sqrt(return_skill_str(ch,node->tn));
		bonus = (((get_curr_stat(ch,stat)/10) * pc_race_table[ch->race].skill_bonus[skill] * class_table[ch->class].skill_bonus[stat]) / 4) + (current * 8);
		return bonus;
	}
}
void set_bonus(CHAR_DATA *ch,char *skill_name)
{
	struct node* node;
	char first[MSL];
	int sn;
	char buf[MSL];
	long bonus = 0;

	pull_skill(skill_name,first);
	node = check_skill_tree(first,skill_name,ch,uber());
	sn = match_skill(skill_name,"set_bonus");
	if (IS_NPC(ch))
		return;
	else if (node != NULL)
	{
		if (sn < 0)
		{
			stc("match_skill failed in set_bonus\n\r",ch);
			sprintf(buf,"skill_name %s, sn %d\n\r",skill_name,sn);
			stc(buf,ch);
			return;
		}
		if (sn == 5000)
		{
			stc("match_skill found UBER in set_bonus\n\r",ch);
			return;
		}
		bonus = calc_bonus(ch,node);
		ch->pcdata->bonus_array[sn] = bonus;
		//		sprintf(buf,"%s bonus[%d] = %d\n\r",node->tn,sn,bonus);
		//		stc(buf,ch);
		return;
	}
	return;
}

void update_bonus(CHAR_DATA *ch,struct node *node)
{
	/*Check to see if its the uber node*/
	if (!strcmp(node->name,"uber"))
	{	
		if(node->x1 != NULL)
			update_bonus(ch,node->x1);
		if(node->x2 != NULL)
			update_bonus(ch,node->x2);
		if(node->x3 != NULL)
			update_bonus(ch,node->x3);
		if(node->x4 != NULL)
			update_bonus(ch,node->x4);
		if(node->x5 != NULL)
			update_bonus(ch,node->x5);
		return;
	}
	/* Here we set the skills bonus*/
	set_bonus(ch,node->tn);
	/* And here we set the skill's level to the average levels of its children skills*/
	set_parent_avg(ch,node);
	/*Recursion!!*/
	if(node->x1 != NULL)
		update_bonus(ch,node->x1);
	if(node->x2 != NULL)
		update_bonus(ch,node->x2);
	if(node->x3 != NULL)
		update_bonus(ch,node->x3);
	if(node->x4 != NULL)
		update_bonus(ch,node->x4);
	if(node->x5 != NULL)
		update_bonus(ch,node->x5);
	return;
}
void set_parent_avg(CHAR_DATA *ch,struct node *node)
{
	int counter = 0;
	int sum = 0;
	int sn = 0;
	if (node->x1 == NULL 
		&& node->x2 == NULL
		&& node->x3 == NULL
		&& node->x4 == NULL
		&& node->x5 == NULL)
	{
		return;
	}

	if(node->x1 != NULL)
	{
		counter++;
		sum += return_skill_str(ch,node->x1->tn);
	}
	if(node->x2 != NULL)
	{
		counter++;
		sum += return_skill_str(ch,node->x2->tn);
	}
	if(node->x3 != NULL)
	{
		counter++;
		sum += return_skill_str(ch,node->x3->tn);
	}
	if(node->x4 != NULL)
	{
		counter++;
		sum += return_skill_str(ch,node->x4->tn);
	}
	if(node->x5 != NULL)
	{
		counter++;
		sum += return_skill_str(ch,node->x5->tn);
	}
	sn = match_skill(node->tn,"set_parent_avg");
	ch->pcdata->skills_array[sn] = sum/counter;
	return;
}
/*
* Lookup a skill by name.
*/
int match_skill( const char *name,char * called_by )
{
	int sn;
	char buf[MSL];

	if(!strcmp(name,"uber"))
	{
		return 5000;
	}
	for ( sn = 0; sn < MAX_TREE_SKILLS; sn++ )
	{
		if ( tree_skill_table[sn].name == NULL )
			continue;
		if (!str_prefix( name, tree_skill_table[sn].name))
		{
			if(name != NULL && !str_prefix(called_by,"return_skill_str"))
				sprintf(buf,"{W%s:{xparameter name = %s\ttree_skill_table[%d].name = %s",
				called_by,name,sn,
				tree_skill_table[sn].name);
			return sn;
		}
	}
	return -1;
}
void convert_char(CHAR_DATA *ch)
{
	int sn;
	long total = 0;
	char buf[MSL];

	if(IS_NPC(ch))
	{
		stc("Talent function on a mob, incorrect usage.\n\r",ch);
		return;
	}
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( skill_table[sn].name == NULL )
			break;
		total += ch->pcdata->learned[sn];
	}
	ch->pcdata->sp = total;
	ch->version = 6;
	sprintf(buf,"{WDue to character conversion, you have been given %d SP's{x\n\r",total);
	stc(buf,ch);
	return;
}

int weapon_talent(int v0)
{
	switch (v0)
	{
	case WEAPON_SWORD	: return com_o_s_s;
	case WEAPON_DAGGER	: return com_o_p_d;
	case WEAPON_SPEAR	: return com_o_p_s;
	case WEAPON_MACE	: return com_o_b_m;
	case WEAPON_AXE		: return com_o_s_a;
	case WEAPON_FLAIL	: return com_o_b_f;
	case WEAPON_WHIP	: return com_o;
	case WEAPON_POLEARM	: return com_o_s_p;
	case WEAPON_BOW		: return com_o_p_m;
	default				: return -1;
	}
}
int sn_talent(int sn)
{
	if(sn == gsn_backstab)			return cov_s_st;
	if(sn == gsn_circle)			return cov_s_a;
	if(sn == gsn_dodge)				return com_d_d;
	if(sn == gsn_envenom)			return cov_m_p;
	if(sn == gsn_hide)				return cov_s_h;
	if(sn == gsn_peek)				return cov_s_sn;
	if(sn == gsn_pick_lock)			return cov_m_l;
	if(sn == gsn_sneak)				return cov_s_sn;
	if(sn == gsn_steal)				return cov_m_s;

	if(sn == gsn_disarm)			return com_o;
	if(sn == gsn_counter)			return com_d_c;
	if(sn == gsn_enhanced_damage)	return com_s_ma;
	if(sn == gsn_kick)				return com_o;
	if(sn == gsn_parry)				return com_d_p;
	if(sn == gsn_rescue)			return com_d;

	if((sn == gsn_second_attack)
		||(sn == gsn_third_attack)
		||(sn == gsn_fourth_attack)
		||(sn == gsn_fifth_attack))
		return com_s_mu;

	if((sn == gsn_second_cast)
		||(sn == gsn_third_cast))
		return arc;

	if((sn == gsn_blindness)
		||(sn == gsn_charm_person)
		||(sn == gsn_curse))
		return arc_a_p;

	if((sn == gsn_invis)
		||(sn == gsn_mass_invis)
		||(sn == gsn_sleep)
		||(sn == gsn_fly))
		return sor_h_e_p;

	if(sn == gsn_poison)			return sor_h_e;
	if(sn == gsn_plague)			return arc_a_p;
	if(sn == gsn_sanctuary)			return wor_b_p_p;

	/*legionare */
	if((sn == gsn_strike)
		||(sn == gsn_shield_wall)	//com_d_s com_d
		||(sn == gsn_phalanx)		//com_o
		||(sn == gsn_fortify)
		||(sn == gsn_precision))		//com_o
		return com_s_ma;

	/* draconian */
	if(sn == gsn_find_dragon)		return arc_n_s;
	if(sn == gsn_sever)				return com_s_ma;

	/* sensei */
	if((sn == gsn_haduken)			//wor_m_o
		||(sn == gsn_triple_kick))	//com_o_u
		return com_s_ma;

	if(sn == gsn_shadow_slip)		return cov_s_sn;
	if(sn == gsn_fatality)			return com_s_ma; //com_o_u

	/* zealot */
	if(sn == gsn_smite)				return com_s_ma; //wor_m_o

	/* dagashi */
	if(sn == gsn_burst_of_speed)	return com_s_ma;
	if(sn == gsn_coat)				return cov_m_p;
	if(sn == gsn_dagashi_poison)	return cov_m_p;
	if(sn == gsn_gouge)				return com_s_ma; //cov_s_a + com_o_u
	/* new gsns */
	if(sn == gsn_axe)				return com_o_s_a;
	if(sn == gsn_dagger)			return com_o_p_d;
	if(sn == gsn_flail)				return com_o_b_f;
	if(sn == gsn_mace)				return com_o_b_m;
	if(sn == gsn_polearm)			return com_o_s_p;
	if(sn == gsn_shield_block)		return com_d_s;
	if(sn == gsn_spear)				return com_o_p_s;
	if(sn == gsn_sword)				return com_o_s_s;
	if(sn == gsn_whip)				return com_o_u;
	if(sn == gsn_bow)				return com_o_p_m;	

	if(sn == gsn_bash)				return com_o_b;
	if(sn == gsn_berserk)			return com_s_ma;
	if(sn == gsn_dirt)				return com_o_u;
	if(sn == gsn_hand_to_hand)		return com_o_u;
	if(sn == gsn_trip)				return com_o_u;
	if(sn == gsn_whirlwind)			return com_s_ma;

	if((sn == gsn_fast_healing)
		||(sn == gsn_haggle))
		return com_s;

	if(sn == gsn_lore)				return sor;
	if(sn == gsn_meditation)		return wor;

	if((sn == gsn_scrolls)
		||(sn == gsn_staves)
		||(sn == gsn_wands)
		||(sn == gsn_recall))			
		return sor;

	if(sn == gsn_phase)				return arc_a_p;
	if((sn == gsn_search)
		||(sn == gsn_forging))
		return com;

	if(sn == gsn_create_golem)		return arc_n_s;

	else
	{
		switch (sn)
		{
		case (1):		return sor_w_e;			/* acid blast */
		case (2):		return wor_b_p_p;		/* armor */
		case (3):		return wor_b_e_b;		/* bless */
		case (5):		return sor_w_e;			/* burning hands */
		case (6):		return sor_w_e;			/* call lightning */
		case (7):		return sor_h_e_p;		/* calm */
		case (8):		return arc_a_p;			/* cancellation */
		case (9):		return wor_m_o;			/* cause critical */
		case (10):	return wor_m_o;				/* cause light */
		case (11):	return wor_m_o;				/* cause serious */
		case (12):	return sor_w_w;				/* chain lightning */
		case (13):	return sor_h_e_p;			/* change sex */
		case (15):	return sor_w_e;				/* chill touch */
		case (16):	return sor_w_e;				/* colour spray */
		case (17):	return sor_h_e_o;			/* continual light */
		case (18):	return sor_w_w;				/* control weather */

		case (19):								/* create food */
		case (20):								/* create rose */
		case (21):								/* create spring */
		case (22):	return sor_h_e_o;			/* create water */

		case (23):	return wor_b_h_c;		/* cure blindness */
		case (24):	return wor_b_h_r;		/* cure critical */
		case (25):	return wor_b_h_c;		/* cure disease */
		case (26):	return wor_b_h_r;		/* cure light */
		case (27):	return wor_b_h_r;		/* cure poison  */
		case (28):	return wor_b_h_c;		/* cure serious  */
		case (30):	return wor_m_o;			/* demonfire */

		case (31):							/* detect evil */
		case (32):							/* detect good */
		case (33):							/* detect hidden */
		case (34):							/* detect invis */
		case (35):							/* detect magic */
		case (36):							/* detect poison */
		case (37):	return sor_h_d;			/* discern weakness */

		case (38):	return wor_m_o;			/* dispel evil */
		case (39):	return wor_m_o;			/* dispel good */
		case (40):	return arc_a;			/* dispel magic */
		case (41):	return wor_m_o;			/* divine right com_s_ma */
		case (42):	return arc_a_p;			/* dragonfury */
		case (43):	return arc_p_m;			/* elemental shield */
		case (44):	return sor_w_e;			/* earthquake */

		case (45):							/* enchant armor */
		case (46):	return sor_h_e_o;		/* enchant weapon */

		case (47):	return sor_w_e;			/* energy drain */
		case (48):	return -1;				/* erecover */
		case (49):	return wor_m_o;			/* exodus */
		case (50):	return sor_w_e;			/* faerie fire */
		case (51):	return sor_w_w;			/* faerie fog */
		case (52):	return sor_w_p;			/* farsight */
		case (53):	return wor_b_e;			/* fanatics flame */
		case (54):	return arc_g_f;			/* fireball */
		case (55):	return arc_g_f;			/* fireshield */
		case (56):	return sor_w_e;			/* fireproof */
		case (57):	return wor_m_o;			/* flamestrike */
		case (58):	return wor_m_o;			/* judgement */
		case (59):	return sor_h_e_p;		/* fly */
		case (60):	return sor_h_e_o;		/* floating disc */
		case (61):	return wor_b_e_b;		/* frenzy */
		case (62):	return sor_h_t;			/* gate */
		case (63):	return sor_h_e_p;		/* giant strength */
		case (64):	return arc_g_et;		/* giga blast */
		case (66):	return wor_b_h_r;		/* greater healing */
		case (67):	return wor_m_o;			/* harm */
		case (68):	return sor_h_e_p;		/* haste */
		case (69):	return wor_b_h_r;		/* heal */
		case (70):	return sor_h_e_o;		/* heat metal */
		case (71):	return wor_m_o;			/* heavenly justice */
		case (72):	return arc_a_p;			/* hold person */
		case (73):	return wor_b_p_p;		/* holy armor */
		case (74):	return wor_b_h;			/* holy word */
		case (75):	return sor_h_d;			/* identify */
		case (76):	return sor_h_d;			/* infravision */
		case (78):	return sor_w_p;			/* know alignment */
		case (79):	return arc_g_wi;		/* lightning bolt */
		case (80):	return sor_w_p;			/* locate object */
		case (81):	return arc_g_et;		/* magic missile */
		case (82):	return wor_b_h_r;		/* mass healing */
		case (84):	return -1;				/* minsc power */

		case (85):							/* mind thrust */
		case (86):							/* mind blast */
		case (87):	return sor_w_p;			/* mind crush */

		case (88):	return -1;				/* mrecover */
		case (89):	return sor_h_d;			/* mystic sight */

		case (90):							/* nexus */
		case (91):							/* pass door */
		case (94):	return sor_h_t;			/* portal */

		case (95):	return -1;				/* precover */
		case (96):							/* protection evil */
		case (97):	return wor_b_p_s;		/* protection good */
		case (98):	return wor_m_o;			/* ray of truth */
		case (99):	return sor_h_e_o;		/* recharge */
		case (100):	return wor_b_h_c;		/* refresh */
		case (101):	return arc_a;			/* remove curse */
		case (102):	return arc_p_p;			/* repulsar */
		case (103):	return arc_a_o;			/* runic scribing */
		case (104):	return arc_a_o;			/* rune of warding */
		case (106):	return wor_b_p;			/* shield */
		case (107):	return sor_w_e;			/* shocking grasp */
		case (108):	return arc_n_r;			/* skeleton dance */
		case (110):	return sor_h_e_p;		/* slow */
		case (111):	return arc_p_p;			/* stone skin */
		case (112):	return sor_h_t;			/* summon */
		case (113):	return arc_g_et;		/* supernova */
		case (114):	return sor_h_t;			/* teleport */
		case (115):	return -1;				/* thermonuclear blast */
		case (116):	return arc_g_ea;		/* venom lance */
		case (117):	return sor_h_e_p;		/* ventriloquate */
		case (118):	return arc_a_o;			/* vise grip */
		case (119):	return sor_h_e_p;		/* weaken */
		case (120):	return arc_n_r;			/* spectral hand */
		case (121):	return arc_g_ea;		/* acid breath */
		case (122):	return arc_g_f;			/* fire breath */
		case (123):	return arc_g_wa;		/* frost breath */
		case (124):	return arc_g_wi;		/* gas breath */
		case (125):	return arc_g_et;		/* lightning breath */
		case (126):	return arc_g;			/* elemental wrath */
		case (137):	return -1;				/* general purpose */
		case (138):	return -1;				/* high explosive */
		case (203):	return -1;				/* smiling death */
		case (204):	return -1;				/* mantis dance */
		case (205):	return -1;				/* deadly fingertips */
		default: return -1;
		}
	}
}

void do_record_sn( CHAR_DATA *ch, char *argument )
{
	int sn;
	FILE *fp = NULL;

	if ((fp = fopen(SN_FILE, "w")) == NULL)
	{
		perror(SN_FILE);
		return;
	}

	stc("Beggining sn loop..\n\r",ch);
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( skill_table[sn].name == NULL )
		{
			continue;
		}
		if ( skill_table[sn].pgsn != NULL)
		{
			continue;
		}
		else
		{
			ptc(ch,"%s found! (%d)\n\r",skill_table[sn].name,sn);
			fprintf(fp,"%d: %s\n",sn,skill_table[sn].name);
		}
	}
	stc("All SN's have been outputed to sn.txt\n\r",ch);
	fclose(fp);
	unlink(STAT_FILE);
}
void gain_sp( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int gain = 0;

	gain += victim->level * 100;
	if(victim->hitroll > 1000
		|| victim->hitroll * 2 > 1000)
		gain += victim->hitroll;
	else
		gain += victim->hitroll * 2;
	gain += sqrt(victim->level);
	debug(ch,"gain_sp: sqrt()=%ld\n\r",sqrt(victim->level));
	ptc(ch,"{xYou gain %d skill points.\n\r",gain);
	ch->pcdata->sp += gain;
}



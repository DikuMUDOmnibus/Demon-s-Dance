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

/*Golem Source File*/
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


void do_create_golem ( CHAR_DATA *ch, char *argument )
{
	MOB_INDEX_DATA *pMobIndex;
	OBJ_DATA *bag;
	OBJ_DATA *bag_next;
	char arg1[MIL];
	int chance;
	bool bag_found		= FALSE;

	argument = one_argument(argument, arg1);

	if (arg1[0] == '\0')
	{
		send_to_char("Syntax: create <part_set(harpy, goblin...)>\n\r", ch);
		return;
	}

	if (( chance = get_skill(ch,gsn_create_golem)) == 0
	|| (!IS_NPC(ch)
	&& ch->level < skill_table[gsn_create_golem].skill_level[ch->class]))
	{
		send_to_char("You don't know where to start.\n\r",ch);
		return;
	}

	if ( ch->pet != NULL )
	{
		send_to_char("You already a companion.\n\r",ch);
		return;
	}
	if(ch->position == POS_FIGHTING)
	{	
	   send_to_char("You can't study the ritual while in combat!\n\r",ch);
	   return;
	}

/*check for golem bag*/
	for ( bag = ch->carrying; bag != NULL; bag = bag_next )
	{
		bag_next = bag->next_content;
		if (bag->item_type == ITEM_GOLEM_BAG)
		{
			bag_found = TRUE;
			break;
		}
    }

	/*bag not found*/
	if ( !bag_found )
	{
		act( "$n tries to summon a golem, but lacks the required golem bag", ch, NULL, NULL, TO_ROOM );
		act( "You try to summon a golem, but you lack the required golem bag", ch, NULL, NULL, TO_CHAR );
		return;
	}

	if ( ( pMobIndex = get_mob_index(MOB_VNUM_GOLEM) ) == NULL )
	{      
		send_to_char( "The golem mob doesn't exist.\n\r", ch );
		return;
	}

	golem_flesh(ch,"",bag,arg1);
}

bool get_component ( CHAR_DATA *ch, char *argument, OBJ_DATA *bag )
{
	char buf[MSL];
	char arg1[MIL];
	OBJ_DATA *component;
	argument = one_argument(argument, arg1);
	
    component = get_obj_list( ch, arg1, bag->contains );
    if (component == NULL)
    {
		sprintf(buf,"You lack the %s nessecary for this spell",argument );
		return FALSE;
    }
    get_obj( ch, component, bag );
	obj_from_char(component);
	return TRUE;
}

void golem_flesh ( CHAR_DATA *ch, char *argument, OBJ_DATA *bag, char *flesh_type )
{
	char buf[MSL];
	char arg1[MIL];
	OBJ_DATA *component;
	int x;
	bool head_found		= FALSE;
	bool leg_found		= FALSE;
	bool arm_found		= FALSE;
	bool heart_found	= FALSE;
	CHAR_DATA			*pet;
	MOB_INDEX_DATA		*pMobIndex;
	int i;
	char name_buf[MSL];
	char long_buf[MSL];
	char short_buf[MSL];
	int golem_number;

	argument = one_argument(flesh_type, arg1);
	
    component = get_obj_list( ch, arg1, bag->contains );
    if ( component == NULL )
    {
		sprintf(buf,"You lack the %s nessecary for this spell",arg1);
		stc(buf,ch);
		return;
    }

	if ( ( pMobIndex = get_mob_index(MOB_VNUM_GOLEM) ) == NULL )
	{      
		send_to_char( "The golem mob doesn't exist.\n\r", ch );
		return;
	}
	for ( x = 0; x < MAX_FLESH_TYPES; x++ )
	{
		if (golem_flesh_table[x].name != NULL)
		{
			if ( !str_prefix( golem_flesh_table[x].name, component->name ) )
			{
				if (check_heart(ch,golem_flesh_table[x].name,component,bag))
					heart_found = TRUE;
				if (check_head(ch,golem_flesh_table[x].name,component,bag))
					head_found	= TRUE;
				if (check_arm(ch,golem_flesh_table[x].name,component,bag))
					arm_found	= TRUE;
				if (check_leg(ch,golem_flesh_table[x].name,component,bag))
					leg_found	= TRUE;
				golem_number = x;
				break;
			}
		}
	}
	if (heart_found 
		&& head_found 
		&& arm_found 
		&& leg_found)
	{
		pet = create_mobile( pMobIndex );
   		pet->level = ch->level;
		pet->mana = pet->max_mana = 0;
		pet->hit = pet->max_hit = (ch->max_hit + golem_flesh_table[golem_number].hp_mod);
		for(i = 0; i < 4; i++)
		pet->armor[i] = (number_fuzzy(ch->armor[i] - 10) + golem_flesh_table[golem_number].armor_mod);
		pet->hitroll = (number_fuzzy(ch->level*1.5) + golem_flesh_table[golem_number].hit_mod);
		pet->damroll = (number_fuzzy(ch->level*1.5) + golem_flesh_table[golem_number].hit_mod);
    
		/* free up the old mob names */
		free_string(pet->description);
		free_string(pet->name);
		free_string(pet->short_descr);
		free_string(pet->long_descr);
		pet->description = str_dup("You have summoned a Golem to do your bidding\n\r");
		
		sprintf(short_buf,"%s Golem",golem_flesh_table[golem_number].flesh_type);
		pet->short_descr = str_dup(short_buf);
		
		sprintf(long_buf,"A %s golem protects its master.\n\r",golem_flesh_table[golem_number].flesh_type);
		pet->long_descr = str_dup(long_buf);
		
		sprintf(name_buf,"%s golem\n\r",golem_flesh_table[golem_number].flesh_type);
		pet->name = str_dup(name_buf);
		
		pet->dam_type = 10; /* bite */
		char_to_room( pet, ch->in_room );
		act( "You begin assemble the pieces for a $N!.",ch,NULL,pet,TO_CHAR);
		act( "$n begins to assemble the pieces for a $N!", ch, NULL, pet, TO_ROOM );
		WAIT_STATE(ch, 2 * PULSE_MOBILE);
		add_follower( pet, ch );
		pet->leader = ch;
		ch->pet = pet;

		SET_BIT(pet->act, ACT_PET);
		SET_BIT(pet->affected_by, AFF_CHARM);
		return;
	}
	else
	{
		stc("You did not have all the required parts (heart head arm leg) and your components have been wasted\n\r",ch);
		return;
	}

}

bool check_heart ( CHAR_DATA *ch, char *flesh_name,OBJ_DATA *component, OBJ_DATA *bag )
{
	char searching[MIL];
	char getting[MIL];
	OBJ_DATA *to_get;

	sprintf(searching,"%s heart",flesh_name);
	if (get_obj_list( ch, searching, bag->contains ) != NULL)
	{
		sprintf(getting,"'%s'",searching); 
		to_get = get_obj_list( ch, searching, bag->contains );
		get_obj(ch,to_get,bag);
		obj_from_char(to_get);
		return TRUE;
	}
	else
		return FALSE;

}

bool check_head ( CHAR_DATA *ch, char *flesh_name,OBJ_DATA *component, OBJ_DATA *bag )
{
	char searching[MIL];
	char getting[MIL];
	OBJ_DATA *to_get;

	sprintf(searching,"%s head",flesh_name);
	if (get_obj_list( ch, searching, bag->contains ) != NULL)
	{
		sprintf(getting,"'%s'",searching);
		to_get = get_obj_list( ch, searching, bag->contains );
		get_obj(ch,to_get,bag);
		obj_from_char(to_get);
		return TRUE;
	}
	else
		return FALSE;

}

bool check_arm ( CHAR_DATA *ch, char *flesh_name,OBJ_DATA *component, OBJ_DATA *bag )
{
	char searching[MIL];
	char getting[MIL];
	OBJ_DATA *to_get;

	sprintf(searching,"%s arm",flesh_name);
	if (get_obj_list( ch, searching, bag->contains ) != NULL)
	{
		sprintf(getting,"'%s'",searching); 
		to_get = get_obj_list( ch,searching, bag->contains );
		get_obj(ch,to_get,bag);
		obj_from_char(to_get);
		return TRUE;
	}
	else
		return FALSE;

}

bool check_leg ( CHAR_DATA *ch, char *flesh_name,OBJ_DATA *component, OBJ_DATA *bag )
{
	char searching[MIL];
	char getting[MIL];
	OBJ_DATA *to_get;

	sprintf(searching,"%s leg",flesh_name);
	if (get_obj_list( ch, searching, bag->contains ) != NULL)
	{
		sprintf(getting,"'%s'",searching); 
		to_get = get_obj_list( ch, searching, bag->contains );
		get_obj(ch,to_get,bag);
		obj_from_char(to_get);
		return TRUE;
	}
	else
		return FALSE;

}
bool drop_golem( CHAR_DATA *ch, ROOM_INDEX_DATA *room )
{
	int golem_chance;
	int race_chance;
	int part_to_drop = 0;
	int race_to_drop = 0;
	int vnum	= 0;
	OBJ_DATA *part;

	if (number_range(0,100) >= 75 )
	{
		golem_chance = number_range(0,25);
		switch (golem_chance)
		{
			default: case 0: case 1: case 2: case 3: case 4: case 5:
			case 6:	 case 7: case 8: case 9: case 10:
			{
				break;
			}
			case 11: case 12: case 13: case 14: case 15:
			{
				part_to_drop = 1; /*leg*/
				break;
			}
			case 16: case 17: case 18: case 19: case 20:
			{
				part_to_drop = 2; /*arm*/
				break;
			}
			case 21: case 22: case 23:
			{
				part_to_drop = 3; /*heart*/
				break;
			}
			case 24: case 25:
			{
				part_to_drop = 4; /*head*/
				break;
			}
			break;
		}

		race_chance = (number_range(0,20) + number_range(0,20) + number_range(0,20));
		if ((race_chance >= 0) && (race_chance <= 20))
		{
			race_to_drop = 1;/*human*/
		}
		if ((race_chance >= 21) && (race_chance <= 35))
		{
			race_to_drop = 2;/*goblin*/
		}
		if ((race_chance >= 36) && (race_chance <= 46))
		{
			race_to_drop = 3;/*knight*/
		}
		if ((race_chance >= 46) && (race_chance <= 54))
		{
			race_to_drop = 4;/*harpy*/
		}
		if ((race_chance >= 55) && (race_chance <= 58))
		{
			race_to_drop = 5;/*demon*/
		}
		if ((race_chance >= 59) && (race_chance <= 60))
		{
			race_to_drop = 6;/*dragon*/
		}


		if (part_to_drop == 0)
		{
			return FALSE;
		}
		if (race_to_drop == 0)
		{
			return FALSE;
		}
		vnum = part_lookup(race_to_drop,part_to_drop);
		if (vnum != 0)
		{
			part = create_object(get_obj_index(vnum),0);
			obj_to_room( part, room );
			act("A golem component falls to the ground",ch,NULL,NULL,TO_ROOM);
			return TRUE;
		}
		return TRUE;
	}
	else
		return FALSE;
}

int part_lookup( int race_to_find, int part_to_find)
{
	int i;

	for ( i = 0; i < 25; i++ )
	{
		if ((race_to_find == make_golem_table[i].race_to_get)
			&& (part_to_find == make_golem_table[i].part_to_get))
		{
			return make_golem_table[i].vnum;
		}
	}
	return 0;
}

const	struct	golem_flesh_type	golem_flesh_table	[]	=
{
	/*{
		name,flesh_type,
		armor_mod,hit_mod,hp_mod
	}*/

	{ 
		"goblin","steel",
		10,10,-2500
	},
	{ 
		"demon","onyx",
		75,75,6000
	},
	{ 
		"knight","platinum",
		50,50,2000
	},
	{ 
		"human","flesh",
		5,5,-4000
	},
	{ 
		"harpy","garbage",
		100,25,0
	},
	{ 
		"dragon","crystal",
		500,100,10000
	},
	{ 
		NULL,NULL,
		0,0,0
	}
};

const	struct	make_golem_type	make_golem_table	[]	=
{
	/*	{race_to_get, part_to_get, vnum}*/
	{1,1,HUMAN_LEG},
	{1,2,HUMAN_ARM},
	{1,3,HUMAN_HEART},
	{1,4,HUMAN_HEAD},
	{2,1,GOBLIN_LEG},
	{2,2,GOBLIN_ARM},
	{2,3,GOBLIN_HEART},
	{2,4,GOBLIN_HEAD},
	{3,1,KNIGHT_LEG},
	{3,2,KNIGHT_ARM},
	{3,3,KNIGHT_HEART},
	{3,4,KNIGHT_HEAD},
	{4,1,HARPY_LEG},
	{4,2,HARPY_ARM},
	{4,3,HARPY_HEART},
	{4,4,HARPY_HEAD},
	{5,1,DEMON_LEG},
	{5,2,DEMON_ARM},
	{5,3,DEMON_HEART},
	{5,4,DEMON_HEAD},
	{6,1,DRAGON_LEG},
	{6,2,DRAGON_ARM},
	{6,3,DRAGON_HEART},
	{6,4,DRAGON_HEAD},
	{0,0,0}
};


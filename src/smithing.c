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
/*************************************************************
*	Based on As the Wheel Weaves's Publshed smithing snippet *
*	Modded by Stan of Demons Dance                           *
*************************************************************/
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
#include "olc.h"
#include "skill_tree.h"

int clan_lookup args(( const char *name ));

int smith_item_lookup( const char *name )
{
	int item;

	for ( item = 0; item < MAX_SMITH_ITEMS; item++ )
	{
		if (LOWER(name[0]) == LOWER(smith_items_table[item].name[0])
			&&  !str_prefix(name,smith_items_table[item].name))
			return item;
	}
	return 0;
}
void do_smith( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *ore,*item,*furnace,*anvil,*hammer,*barrel,*grinder;
	AFFECT_DATA *pAf;
	char arg1[MIL],arg2[MIL],arg3[MIL],arg5[MIL],name_buf[128];
	char short_buf[128],long_buf[128],buf[MSL],adj[16],help_buf[MSL];
	bool infusing = FALSE, itm = FALSE, hasore = FALSE;
	bool has_furnace = FALSE, has_anvil = FALSE, has_hammer = FALSE;
	bool has_barrel = FALSE, has_grinder = FALSE;
	int i = 0, x = 0, skill = 0, difficulty = 0, y = 0,z = 1;
	int ya = 0, yb = 0, yc = 0,talent_bonus = 0;
	int v2_mod = 0;
	SMITH_ITEM sItem;
	SMITH_ORE sOre;

	name_buf[0] = '\0';
	short_buf[0] = '\0';
	long_buf[0] = '\0';

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	argument = one_argument( argument, arg5 );

	if ( IS_NPC(ch) || ch->desc == NULL )
		return;

	sprintf(help_buf,"None of the arguments can be null, and the syntax must be exact.\n\r\n\r");
	strcat(help_buf,"Syntax:  smith <material> into <item>\n\r");
	strcat(help_buf,"Example: smith steel into dagger\n\r");

	if ( arg1[0] == '\0' )
	{
		stc(help_buf,ch);
		return;
	}
	if ( !str_cmp( arg1, "list" ) )
	{
		stc("Item\t\t\tDifficulty Item\t\t\tDifficulty Item\t\t\tDifficulty\n\r",ch);
		while ( y < (MAX_SMITH_ITEMS - 1))
		{
			if (smith_items_table[y].name != NULL)
			{
				skill = get_skill( ch, gsn_forging );
				talent_bonus = return_bonus_str(ch,"combat.offense");
				debug(ch,"do_smith: skill=%d talent_bonus=%d sum=%d\n\r",skill,talent_bonus,skill+talent_bonus);
				skill += talent_bonus;
				if(smith_items_table[y].difficulty <= skill)
					ptc(ch, "%-21.21s {W%7s{x%s",
						smith_items_table[y].name,diff(smith_items_table[y].difficulty),
						z >= 3 ? "\n\r" : " ");
			}
			y++;
			if(z >= 3)
				z = 1;
			else
				z++;
		}
		stc("\n\rDifficulty on a scale of 1 through 7 stars, 1 being easiest, 7 being hardest.\n\r",ch);
		return;
	}
	if ( arg2[0] == '\0' || arg3[0] == '\0' )
	{
		stc(help_buf,ch);
		return;
	}
	if ( str_cmp( arg2, "into" ) )
	{
		stc(help_buf,ch);
		return;
	}
	if ( ( ore = get_obj_carry( ch, arg1, ch ) ) != NULL )
	{
		if ( ore->item_type != ITEM_SMITH_ORE )
		{
			sprintf(buf,"%s is not a workable piece of ore.\n\r", ore->name );
			stc(buf,ch);
			return;
		}
		for ( x = 0; x < MAX_ORE_ITEMS-1; x++ )
		{
			if ( !str_prefix( smith_ores_table[x].name, ore->name ) )
			{
				hasore = TRUE;
				break;
			}
		}
		if ( !hasore )
		{
			stc( "That ore isnt on the list!\n\r", ch );
			return;
		}
		for ( i = 0; i < MAX_SMITH_ITEMS-1; i++ )
		{
			if ( !str_cmp( arg3, smith_items_table[i].name ) )
			{
				//ptc(ch,"You prepare to make %s from %s.\n\r",smith_items_table[i].name, smith_ores_table[x].name );
				itm = TRUE;
				break;
			}
		}
		sItem	= smith_items_table[i];
		sOre	= smith_ores_table[x];
		if ( itm == FALSE )
		{
			ptc(ch,"%s is not on the forgeable item list",arg3);
			return;
		}
		if ( skill < difficulty )
		{
			ptc(ch,"Your altered skill is only %d, whereas you would need about %d to forge %s %s.\n\r",
				get_skill( ch, gsn_forging ),
				sItem.difficulty,
				sItem.prefix,
				sItem.name);
			return;
		}

		/*change it so this works on difference between altered skill and difficulty*/
		if ( skill >= 200)
			sprintf(adj,"flawlessly");
		else if ( skill >=166 )
			sprintf(adj,"perfectly");
		else if ( skill >= 133 )
			sprintf(adj,"skillfully");
		else if ( skill >= 100 )
			sprintf(adj,"masterfully");
		else if ( skill >= 90 )
			sprintf(adj,"finely");
		else if ( skill >= 75 )
			sprintf(adj,"well");
		else if ( skill >= 50 )
			sprintf(adj,"properly");
		else if ( skill >= 25 )
			sprintf(adj,"simply");
        else if ( skill >= 10 )
			sprintf(adj,"plainly");
		else
			sprintf(adj,"poorly");

		if ( ore->weight < ( sItem.quantity + sItem.weight ) / 2 )
		{
			stc( "You do not have adaquete material.\n\r", ch );
			return;
		}
		act("$n settles at the smithy.", ch, NULL,NULL,TO_ROOM );

		WAIT_STATE( ch, sItem.beats );

		difficulty = sItem.difficulty;
		if ( difficulty > skill )
		{
			act("$n was unable to properly perform the work. Fortunately, the material was spared.", ch, NULL,NULL,TO_ROOM );
			act("You were unable to properly perform the work. Fortunately, the material was spared.", ch, NULL,NULL,TO_CHAR );
			return;
		}

		ptc(ch,"Your are making %s %s from %s.\n\r",
			sItem.prefix,sItem.name,sOre.name);
		ptc(ch,"Smithing Summary: {D({W%d{D,{W%d{D,{%c%d{D){x",
			difficulty,skill,skill - difficulty < 3 ? 'R':'W',skill-difficulty);
		stc(" (difficulty,skill,excess)\n\r",ch);

		for ( furnace = ch->in_room->contents; furnace; furnace = furnace->next_content )
		{
			if ( furnace == NULL )
			{
				stc("There is no anvil here for you to work upon.\n\r",ch);
				return;
			}
			if ( furnace->item_type == ITEM_SMITH_FURNACE )
			{
				break;
			}
		}
		/*
		* Little check to make sure its really ore
		*		-Enzo
		*/
		if ( ore->value[0] == 0 )
		{
			act("$n prepares the ore by placing it in the furnace.", ch,NULL,NULL,TO_ROOM);
			act("You prepare the ore by placing it in the furnace.", ch,NULL,NULL,TO_CHAR);
		}
		else
		{
			stc("{R->{xThis ore has been tainted somehow. Please give to Enzo for a full refund of non-tainted ore.\n\r",ch);
			return;
		}
		for ( anvil = ch->in_room->contents; anvil; anvil = anvil->next_content )
		{
			if ( anvil == NULL )
			{
				stc("There is no anvil here for you to work upon.\n\r",ch);
				return;
			}
			if ( anvil->item_type == ITEM_SMITH_ANVIL )
			{
				break;
			}
		}
		for ( hammer = ch->in_room->contents; hammer; anvil = hammer->next_content )
		{
			if ( hammer == NULL )
			{
				stc("There is no hammer here for you to work with\n\r",ch);
				return;
			}
			if ( anvil->item_type == ITEM_SMITH_ANVIL )
			{
				ore->value[0] += 1;
				break;
			}
		}
		act("$n places the heated ore on the anvil and begins to shape it with $t.",ch,hammer->short_descr,NULL,TO_ROOM);
		act("You place the heated ore on the anvil and begin to shape it with $t.",ch,hammer->short_descr,NULL,TO_CHAR);
		for ( barrel = ch->in_room->contents; barrel; barrel = barrel->next_content )
		{
			if ( barrel == NULL )
			{
				stc("There is no barrel of water here for you to quench your work in.\n\r",ch);
				return;
			}
			if ( barrel->item_type == ITEM_SMITH_BARREL )
			{
				ore->value[0] += 1;
				break;
			}
		}
		act("$n places the cooling material in a barrel of water to quench it.",ch,NULL,NULL,TO_ROOM);
		act("You place the cooling material in a barrel of water to quench it.",ch,NULL,NULL,TO_CHAR);
		for ( grinder = ch->in_room->contents; grinder; grinder = grinder->next_content )
		{
			if ( grinder == NULL )
			{
				stc("There is no grinder here for you to finish your work on.\n\r",ch);
				return;
			}
			if ( grinder->item_type == ITEM_SMITH_GRINDER )
			{
				ore->value[0] += 1;
				break;
			}
		}
		act("$n spins the grinding wheel to smooth the rough edges of $s work.",ch,NULL,NULL,TO_ROOM);
		act("You spin the grinding wheel to smooth the rough edges of your work.",ch,NULL,NULL,TO_CHAR);
		check_improve( ch, gsn_forging, TRUE, 1 );
		debug(ch,"do_smith: (i,x)=(%d,%d)\n\r",i,x);
		switch (i)
		{
		case 26:
			item = create_object( get_obj_index( 10011 ), 1 );
			break;
		case 27:
			item = create_object( get_obj_index( 10012 ), 1 );
			break;
		default:
			item = create_object( get_obj_index( SMITH_PRODUCT ), 1 );
			break;
		}
		sprintf( name_buf, "%s %s", sOre.name, sItem.name );
		sprintf( short_buf, "%s %s{x, %s forged from %s %s{x",
			sItem.prefix,
			sItem.display,
			adj,
			sOre.color,
			sOre.display );
		sprintf( long_buf, "Here lies %s %s{x, %s forged from %s %s{x.",
			sItem.prefix,
			sItem.display,
			adj,
			sOre.color,
			sOre.display );
		item->level = ch->level;
		item->name = str_dup( name_buf );
		item->short_descr = str_dup( short_buf );
		item->orig_short = str_dup( short_buf	);
		item->description = str_dup( long_buf );
		item->item_type = sItem.item_type;
		item->wear_flags += sItem.wear_flags;
		item->weight = 1;
		item->condition = 100;
		item->cost = sItem.weight;
		item->material = str_dup( sOre.name );
		
		item->level	= ch->level - (skill/10);
		if(item->level < sItem.min_level)
			item->level = sItem.min_level;

		if ( item->item_type == ITEM_WEAPON )		
		{
			item->value[0] = sItem.base_v0;
			item->value[1] = sItem.base_v1;
			item->value[2] = sItem.base_v2;
			item->value[3] = attack_lookup( sItem.dam_noun );
		}
		if ( item->item_type == ITEM_ARMOR )
		{
			item->value[0] = sItem.base_pierce;
			item->value[0] += sOre.armor_mod;
			item->value[0] += (skill / 4);
			item->value[1] = sItem.base_bash;
			item->value[1] += sOre.armor_mod;
			item->value[1] += (skill / 4);
			item->value[2] = sItem.base_slash;
			item->value[2] += sOre.armor_mod;
			item->value[2] += (skill / 4);
			item->value[3] = sItem.base_exotic;
			item->value[3] += sOre.armor_mod;
			item->value[3] += (skill / 4);
		}	
		if ( item->item_type == ITEM_QUIVER )
		{
			item->value[0] = sOre.arrow_vnum;
			item->value[1] = sOre.arrow_mod * 100 + (skill * 100);
		}
		/*add the affects*/
		/*first hitroll*/
		if (sOre.hit_mod > 0)
		{
			pAf             =   new_affect();
			pAf->location   =   APPLY_HITROLL;
			pAf->modifier   =   sOre.hit_mod;
			pAf->where	    =   TO_AFFECTS;
			pAf->type	    =	0;
			pAf->duration   =   -1;
			pAf->bitvector  =   0;
			pAf->level      =	item->level;
			pAf->next       =   item->affected;
			affect_to_obj(item,pAf);
		}
		/*then damroll*/
		if (sOre.dam_mod > 0)
		{
			pAf             =   new_affect();
			pAf->location   =   APPLY_DAMROLL;
			pAf->modifier   =   sOre.dam_mod;
			pAf->where	    =   TO_AFFECTS;
			pAf->type	    =	0;
			pAf->duration   =   -1;
			pAf->bitvector  =   0;
			pAf->level      =	item->level;
			pAf->next       =   item->affected;
			affect_to_obj(item,pAf);
		}
		/*done adding affects*/

		extract_obj( ore );
		obj_to_char( item, ch );
		ptc(ch,"Finally done with your labour, you hold %s brand new %s in your hands.\n\r",
			sItem.prefix,sItem.name);
		sprintf(buf,"%s finishes %s %s, a %s forged piece of work, and slips it easily into his trusty ol' inv.\n\r",
			ch->short_descr,
			ch->sex < 2 ? "his":"her",
			item->short_descr,
			adj);
		check_improve(ch,gsn_forging,TRUE,1);
	}
	else
	{
		stc(help_buf,ch);
		return;
	}
	tail_chain();
}


void do_smelt( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *ore;
	OBJ_DATA *ore_next;
	OBJ_DATA *item;
	char arg1[MIL];
	char name_buf[MSL];
	char short_buf[MSL];
	char long_buf[MSL];
	int ore_weight = 0;
	bool hasore = FALSE;
	int x;
	int ore_num;
	char take[MIL];
	int value;

	name_buf[0] = '\0';
	short_buf[0] = '\0';
	long_buf[0] = '\0';

	argument = one_argument( argument, arg1 );


	if ( arg1[0] == '\0' )
	{
		stc("Syntax:  smelt <ore>\n\r", ch );
		stc("Example: smith steel\n\r", ch );
		return;
	}

	for ( x = 0; x < MAX_ORE_ITEMS-1; x++ )
	{
		if ( !str_prefix( smith_ores_table[x].name, arg1 ) )
		{
			hasore = TRUE;
			ore_num = x;
			break;
		}
	}
	if ( !hasore )
	{
		stc( "That ore isnt on the list!\n\r", ch );
		return;
	}
	hasore = FALSE;
	for ( ore = ch->carrying; ore != NULL; ore = ore_next )
	{
		ore_next = ore->next_content;
		if (( ore->item_type == ITEM_SMITH_ORE )
			&& ( !str_prefix( smith_ores_table[x].name, ore->name)))
		{
			hasore = TRUE;
			ore_weight += ore->weight;
			obj_from_char( ore );
		}
	}
	if ( !hasore )
	{
		stc( "That ore isnt on the list!\n\r", ch );
		return;
	}
	sprintf(take,"take");
	item = create_object( get_obj_index( SMITH_PRODUCT ), 1 );
	item->weight = ore_weight;
	if ( ( value = flag_value( wear_flags, take ) ) != NO_FLAG )
	{
		TOGGLE_BIT(item->wear_flags, value);
	}
	sprintf(name_buf,"%s block %d",smith_ores_table[ore_num].name,ore_weight);
	sprintf(short_buf,"a %d lb block of %s",ore_weight,smith_ores_table[ore_num].name);
	sprintf(long_buf,"a %d lb block of %s lies here",ore_weight,smith_ores_table[ore_num].name);
	item->item_type = ITEM_SMITH_ORE;
	item->level = ch->level;
	free_string(item->name);
	free_string(item->short_descr);
	free_string(item->description);
	item->name = str_dup( name_buf );
	item->short_descr = str_dup( short_buf );
	item->description = str_dup( long_buf );
	obj_to_char( item, ch );
	ptc(ch,"You have smelted your %s into a %d lb block.\n\r",smith_ores_table[x].name,ore->weight);
	
}

void do_refine( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *ore;
	OBJ_DATA *ore_next;
	OBJ_DATA *item;
	char arg1[MIL],name_buf[64],short_buf[128],long_buf[256],buf[MSL];
	int ore_weight = 0;
	char search_for[MSL];
	bool hasore = FALSE,hasore_new = FALSE;
	int x;
	int ore_num;
	char take[MIL];
	int value;

	name_buf[0] = '\0';
	short_buf[0] = '\0';
	long_buf[0] = '\0';

	argument = one_argument( argument, arg1 );


	if ( arg1[0] == '\0' )
	{
		stc("Syntax:  refine <ore>\n\r", ch );
		stc("Example: refine steel\n\r", ch );
		return;
	}
	for ( x = 0; x < MAX_ORE_ITEMS-1; x++ )
	{
		if ( !str_prefix( smith_ores_table[x].name, arg1 ) )
		{
			sprintf( buf, "You heat the furnace, preparing to refine %s.\n\r",
				smith_ores_table[x].name );
			stc(buf,ch);
			hasore = TRUE;
			break;
		}
	}
	if(!hasore)
	{
		ptc(ch,"%s isn't a valid ore.\n\r",arg1);
		return;
	}
	hasore = FALSE;
	for ( ore = ch->carrying; ore != NULL; ore = ore_next )
	{
		ore_next = ore->next_content;
		if (( ore->item_type == ITEM_SMITH_ORE )
			&& ( !str_prefix( smith_ores_table[x].name, ore->name)))
		{
			hasore = TRUE;
			ore_weight += ore->weight;
			obj_from_char( ore );
		}
	}
	if ( !hasore )
	{
		ptc(ch,"You don't seem to have any %s",smith_ores_table[x].name);
		return;
	}
	sprintf(search_for,"refined %s",smith_ores_table[x].name);
	for ( ore_num = 0; ore_num < MAX_ORE_ITEMS-1; ore_num++ )
	{
		if ( !str_prefix( smith_ores_table[ore_num].name, search_for ) )
		{
			ptc(ch,"{WHaving researched this procedure in your travels, you expertly prepare to refine the ore into %s{x\n\r",
				smith_ores_table[ore_num].name );
			hasore_new = TRUE;
			break;
		}
	}
	if ( !hasore_new )
	{
		stc( "That refined ore isnt on the list!\n\r", ch );
		return;
	}
	hasore = FALSE;
	hasore_new = FALSE;

	sprintf(take,"take");
	item = create_object( get_obj_index( SMITH_PRODUCT ), 1 );
	item->weight = ore_weight/10;
	if ( ( value = flag_value( wear_flags, take ) ) != NO_FLAG )
	{
		TOGGLE_BIT(item->wear_flags, value);
	}
	sprintf(name_buf,"%s block %d",
		smith_ores_table[ore_num].name,
		ore_weight);
	sprintf(short_buf,"a %d lb block of %s %s{x",
		ore_weight/10,
		smith_ores_table[ore_num].color,
		smith_ores_table[ore_num].display);
	sprintf(long_buf,"a %d lb block of %s %s{x lies here.",
		ore_weight/10,
		smith_ores_table[ore_num].color,
		smith_ores_table[ore_num].display);
	item->item_type = ITEM_SMITH_ORE;
	item->level = ch->level;
	free_string(item->name);
	free_string(item->short_descr);
	free_string(item->description);
	item->name = str_dup( name_buf );
	item->short_descr = str_dup( short_buf );
	item->description = str_dup( long_buf );
	obj_to_char( item, ch );
	ptc(ch,"You have refined your %s into a %d lb block of %s.\n\r",
		smith_ores_table[x].name,item->weight,smith_ores_table[ore_num].name);
}

char* diff(int x)
{
	char* str;
	int i;
	x = x/28;
	if((str = malloc(x * sizeof(char))) == NULL)
	{
		bug("difficulty: str malloc failed.",0);
		return NULL;
	}
	if(x < 1)
		x=1;
	sprintf(str,"");
	for(i=0;i<x;i++)
		str[i] = '*';
	str[i] = '\0';
	return str;
}


const	struct	smith_items_type	smith_items_table	[MAX_SMITH_ITEMS]	=
{
	/*	name, prefix,display,min_level,
	 *	item_type, wear_flags,
	 *	difficulty, weight, beats, quantity,
	 *	base_v0, base_v1, base_v2,"dam_noun",
	 *	base_pierce, base_bash, base_slash, base_exotic
	 */

	{	"sword","a","sword",5,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		25, 5, 1, 1,
		1, 1, 4, "slash",
		0,0,0,0,WEAPON_OFF_HAND	},
	{	"mace","a","mace",5,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		25, 5, 1, 1,
		4, 1, 4, "crush",
		0,0,0,0,WEAPON_OFF_HAND	},
	{	"flail","a","flail",5,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		25, 5, 1, 1,
		6, 1, 4, "smash",
		0,0,0,0,WEAPON_OFF_HAND	},
	{	"dagger","a","dagger",5,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		25, 5, 1, 1,
		2, 1, 4, "pierce",
		0,0,0,0,WEAPON_OFF_HAND	},
	{	"axe","a","axe",5,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		25, 5, 1, 1,
		5, 1, 4, "chop",
		0,0,0,0,WEAPON_OFF_HAND	},
	{	"whip","a","whip",5,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		25, 5, 1, 1,
		7, 1, 4, "whip",
		0,0,0,0,WEAPON_OFF_HAND	},
	{	"bardiche","a","bardiche",5,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		25, 5, 1, 1,
		8, 1, 4, "cleave",
		0,0,0,0,WEAPON_OFF_HAND	},
	{	"spear","a","spear",5,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		25, 5, 1, 1,
		0, 1, 4, "thrust",
		0,0,0,0,WEAPON_OFF_HAND	},
	{	"battle lance","a","battle lance",70,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		80, 40, 1, 1,
		3, 3, 8, "charge",
		0,0,0,0,WEAPON_DOMINANT_HAND	},
	{	"greatsword","a","greatsword",70,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		80, 40, 1, 1,
		1, 3, 8, "slice",
		0,0,0,0,WEAPON_DOMINANT_HAND	},
	{	"morning star","a","morning star",70,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		80, 40, 1, 1,
		6, 3, 8, "crush",
		0,0,0,0,WEAPON_DOMINANT_HAND	},
	{	"great maul","a","great maul",70,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		80, 40, 1, 1,
		4, 3, 8, "pound",
		0,0,0,0,WEAPON_DOMINANT_HAND	},
	{	"misericord","a","misericord",70,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		80, 40, 1, 1,
		2, 3, 8, "stab",
		0,0,0,0,WEAPON_DOMINANT_HAND	},
	{	"great helm","a","great helm",50,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_HEAD,
		25, 5, 1, 1,
		0, 0, 0, 0,
		5, 5, 5, 5, 0	},
	{	"greaves","a pair of","greaves",50,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_LEGS,
		50, 5, 1, 1,
		0, 0, 0, 0,
		5, 5, 5, 5, 0	},
	{	"gauntlets","a pair of","gauntlets",50,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_HANDS,
		50, 5, 1, 1,
		0, 0, 0, 0,
		5, 5, 5, 5, 0	},
	{	"arm guards","a set of","arm guards",50,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_ARMS,
		50, 5, 1, 1,
		0, 0, 0, 0,
		5, 5, 5, 5, 0	},
	{	"boots","some","boots",50,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_FEET,
		25, 5, 1, 1,
		0, 0, 0, 0,
		5, 5, 5, 5, 0	},
	{	"vambrace","a","vambrace",50,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_WRIST,
		50, 5, 1, 1,
		0, 0, 0, 0,
		5, 5, 5, 5, 0	},
	{	"grand crown","a","grand crown",80,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_HEAD,
		90, 25, 5, 1,
		0, 0, 0, 0,
		30, 30, 30, 30, 0	},
	{	"spiked vambrace","a","spiked vambrace",80,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_WRIST,
		90, 20, 1, 1,
		0, 0, 0, 0,
		35, 35, 35, 35, 0	},
	{	"war boots","some","war boots",80,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_FEET,
		90, 25, 1, 1,
		0, 0, 0, 0,
		30, 30, 30, 30, 0	},
	{	"plated arm guards","a set of","plated arm guards",80,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_ARMS,
		90, 35, 1, 1,
		0, 0, 0, 0,
		50, 50, 50, 50, 0	},
	{	"leafmail greaves","a pair of","leafmail greaves",80,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_LEGS,
		90, 40, 1, 1,
		0, 0, 0, 0,
		50, 50, 50, 50, 0	},
	{	"chainmail","a shirt of","chainmail",50,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_BODY,
		65, 90, 1, 1,
		0, 0, 0, 0,
		30, 30, 30, 30, 0	},
	{	"platemail","a set of","platemail",80,
		ITEM_ARMOR, ITEM_TAKE|ITEM_WEAR_BODY,
		95, 200, 1, 1,
		0, 0, 0, 0,
		300, 300, 300, 300, 0	},
	{	"elite harness","an","elite harness",80,
		ITEM_SHEATH, ITEM_TAKE|ITEM_WEAR_ABOUT,
		80, 20, 1, 1,
		0, 0, 0, 0,
		3, 0, 0, 0, 0	},
	{	"battle rig","a","battle rig",40,
		ITEM_SHEATH, ITEM_TAKE|ITEM_WEAR_ABOUT,
		40, 20, 1, 1,
		0, 0, 0, 0,
		2, 0, 0, 0, 0	},
	{	"leather quiver","a","leather quiver",20,
		ITEM_QUIVER, ITEM_TAKE|ITEM_WEAR_WAIST,
		40, 20, 1, 1,
		0, 0, 0, 0,
		50, 50, 50, 50, 0	},
	{	"serrated broadsword","a","{rs{Der{rra{Dte{rd {rb{Droa{rds{Dwor{rd",95,
		ITEM_WEAPON, ITEM_TAKE|ITEM_WIELD,
		200, 200, 1, 1,
		1, 14, 16, "scythe",
		0, 0, 0, WEAPON_DOMINANT_HAND},
	
	{	NULL,	0,		0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0}
};

const	struct	smith_ores_type	smith_ores_table	[MAX_ORE_ITEMS]	=
{
	/*{
		name, display, color,
		armor_mod,workability,
		hit_mod,dam_mod,arrow_mod, arrow_vnum
	}*/
	
	{
		"steel","steel","polished",
			5,2,
			0, 1, 50, 10013
	},
	{
		"bronze","bronze",	"burnished",
			10,1,
			1, 1, 90, 10014
	},
	{
		"silver","silver",	"gleaming",
			15,4,
			2, 2, 130, 10015
	},
	{
		"gold","gold","tawny",
			20,5,
			3, 3, 170, 10016
	},
	{
		"platinum","platinum",	"the purest",
			30,10,
			4, 4, 210, 10017
	},
	{
		"starfire","{cst{Ca{Yrf{Ci{cre{x","shimmering",
			300,75,
			20, 20, 250, 10018
	},
	{
		"refined steel","steel","refined{D",
			40,100,
			5, 5, 90, 10019
	},
	{
		"refined bronze","bronze",	"refined{y",
			50,100,
			6, 6, 130, 10020
	},
	{
		"refined silver","silver",	"refined{w",
			60,100,
			7, 7, 170, 10021
	},
	{
		"refined gold","gold","refined{Y",
			70,100,
			8, 8, 210, 10022
	},
	{
		"refined platinum","platinum",	"refined{W",
			90,100,
			10, 10, 250, 10023
	},
	{
		"refined starfire","{cst{Ca{Yrf{Ci{cre{x","refined",
			600,100,
			40, 40, 300, 10024
	},
	{ 
		NULL,NULL,0,0
	}
};

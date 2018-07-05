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
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "olc.h"


extern char *target_name;

void spell_farsight( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
	if (IS_AFFECTED(ch,AFF_BLIND))
	{
		send_to_char("Maybe it would help if you could see?\n\r",ch);
		return;
	}

	do_function(ch, &do_scan, target_name);
}


void spell_portal( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
	CHAR_DATA *victim;
	OBJ_DATA *portal;

	if ( ( victim = get_char_world( ch, target_name ) ) == NULL
		||   victim == ch
		||   victim->in_room == NULL
		||   !can_see_room(ch,victim->in_room)
		||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
		||	 IS_SET(ch->in_room->room_flags, ROOM_ARENA)
		||	 IS_SET(victim->in_room->room_flags, ROOM_ARENA)
		||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
		||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
		||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
		||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
		||   (!IS_NPC(victim) && victim->level > LEVEL_HERO)  /* NOT trust */
		||   (IS_NPC(victim) && IS_SET(victim->imm_flags,IMM_SUMMON))
		||	(is_clan(victim) && !is_same_clan(ch,victim)))
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}   


	portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
	portal->timer = 2 + level / 25; 
	portal->value[3] = victim->in_room->vnum;

	obj_to_room(portal,ch->in_room);

	act("$p rises up from the ground.",ch,portal,NULL,TO_ROOM);
	act("$p rises up before you.",ch,portal,NULL,TO_CHAR);
}

void spell_nexus( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
	CHAR_DATA *victim;
	OBJ_DATA *portal;
	ROOM_INDEX_DATA *to_room, *from_room;

	from_room = ch->in_room;

	if ( ( victim = get_char_world( ch, target_name ) ) == NULL
		||   victim == ch
		||   (to_room = victim->in_room) == NULL
		||   !can_see_room(ch,to_room) || !can_see_room(ch,from_room)
		||   IS_SET(to_room->room_flags, ROOM_SAFE)
		||	 IS_SET(ch->in_room->room_flags, ROOM_ARENA)
		||	 IS_SET(victim->in_room->room_flags, ROOM_ARENA)
		||	 IS_SET(from_room->room_flags,ROOM_SAFE)
		||   IS_SET(to_room->room_flags, ROOM_PRIVATE)
		||   IS_SET(to_room->room_flags, ROOM_SOLITARY)
		||   IS_SET(to_room->room_flags, ROOM_NO_RECALL)
		||   IS_SET(from_room->room_flags,ROOM_NO_RECALL)
		||   (!IS_NPC(victim) && victim->level > LEVEL_HERO)  /* NOT trust */
		||   (IS_NPC(victim) && IS_SET(victim->imm_flags,IMM_SUMMON))
		||	 (is_clan(victim) && !is_same_clan(ch,victim)))
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}   

	/* portal one */ 
	portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
	portal->timer = 1 + level / 10;
	portal->value[3] = to_room->vnum;

	obj_to_room(portal,from_room);

	act("$p rises up from the ground.",ch,portal,NULL,TO_ROOM);
	act("$p rises up before you.",ch,portal,NULL,TO_CHAR);

	/* no second portal if rooms are the same */
	if (to_room == from_room)
		return;

	/* portal two */
	portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
	portal->timer = 1 + level/10;
	portal->value[3] = from_room->vnum;

	obj_to_room(portal,to_room);

	if (to_room->people != NULL)
	{
		act("$p rises up from the ground.",to_room->people,portal,NULL,TO_ROOM);
		act("$p rises up from the ground.",to_room->people,portal,NULL,TO_CHAR);
	}
}
void spell_spectral_hand( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	int dam;
	int chance;

	chance = number_range(1,100);
	if (chance <= 30)
	{ 	  if (!IS_SET(victim->affected_by,AFF_FAERIE_FIRE)) 
	{
		af.where     = TO_AFFECTS;
		af.type      = sn;
		af.level	 = level;
		af.duration  = level/10;
		af.location  = APPLY_AC;
		af.modifier  = 6 * level;
		af.bitvector = AFF_FIRESHIELD;
		affect_to_char( victim, &af );	} }

	act( "$n {Dsummons a spectral hand to crush their enemy!{x.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You summon a giant spectral hand to crush your enemy's ribs!{x\n\r", ch );
	dam		= (ch->level * 5) + dice(20, 30) + 100;
	damage( ch, victim, dam, sn, DAM_NEGATIVE,TRUE, FALSE);
	return;
}
void spell_runic_scribing( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int skill;
	int bonus;

	/* deal with the object case first */
	if (target == TARGET_OBJ)
	{
		obj = (OBJ_DATA *) vo;
		if (obj->item_type != ITEM_WEAPON)
		{
			act("$p is not a weapon.",ch,obj,NULL,TO_CHAR);
			return;
		}


		else if (IS_WEAPON_STAT(obj,WEAPON_ENCHANTED))
		{
			act("$p already has an enchantment, wait until it wears off.",ch,obj,NULL,TO_CHAR);
			return;
		}


		skill = skill_lookup( "runic scribing");
		bonus = (get_skill(ch,skill))/5 + (ch->level)/10;
		if (bonus ==18)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = (ch->level)/10;
			af.modifier = 1;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = (ch->level)/10;
			af.modifier = 1;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gone{x runic	inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==19)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = (ch->level)/10;
			af.modifier = 2;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = (ch->level)/10;
			af.modifier = 2;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gtwo{x runic	inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==20)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = (ch->level)/10;
			af.modifier = 3;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = (ch->level)/10;
			af.modifier = 3;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gthree{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==21)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 1+(ch->level)/10;
			af.modifier = 4;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 1+(ch->level)/10;
			af.modifier = 5;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gfour{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==22)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 1+(ch->level)/10;
			af.modifier = 5;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 1+(ch->level)/10;
			af.modifier = 7;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gfive{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==23)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 2+(ch->level)/10;
			af.modifier = 6;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 2+(ch->level)/10;
			af.modifier = 10;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gsix{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==24)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 2+(ch->level)/10;
			af.modifier = 7;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 2+(ch->level)/10;
			af.modifier = 13;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gseven{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==25)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 2+(ch->level)/10;
			af.modifier = 8;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 2+(ch->level)/10;
			af.modifier = 16;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);
			act("$p is enscribed with a level {Geight{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==26)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 3+(ch->level)/10;
			af.modifier = 9;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 3+(ch->level)/10;
			af.modifier = 20;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gnine{x runic inscription!",ch,obj,NULL,TO_ALL);		
			return;
		}

		else if (bonus ==27)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 4+(ch->level)/10;
			af.modifier = 10;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 4+(ch->level)/10;
			af.modifier = 24;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gten{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==28)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 4+(ch->level)/10;
			af.modifier = 12;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 4+(ch->level)/10;
			af.modifier = 28;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Geleven{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==29)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 4+(ch->level)/10;
			af.modifier = 14;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 4+(ch->level)/10;
			af.modifier = 32;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a level {Gtwelve{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==30)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 6+(ch->level)/10;
			af.modifier = 15;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 6+(ch->level)/10;
			af.modifier = 40;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type		= sn;
			af.level	= 1;
			af.duration	= 6+(ch->level)/10;
			af.location	= 0;
			af.modifier	= 0;
			af.bitvector  = WEAPON_SHARP;
			affect_to_obj(obj,&af);

			act("$p is enscribed with a {Ghero level{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else if (bonus ==31)
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 200;
			af.modifier = 50;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 200;
			af.modifier = 50;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type		= sn;
			af.level	= 1;
			af.duration	= 200;
			af.location	= 0;
			af.modifier	= 0;
			af.bitvector  = WEAPON_SHARP;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type		= sn;
			af.level	= 1;
			af.duration	= 200;
			af.location	= 0;
			af.modifier	= 0;
			af.bitvector  = WEAPON_VORPAL;
			affect_to_obj(obj,&af);

			act("$p is enscribed with an {Gimmortal level{x runic inscription!",ch,obj,NULL,TO_ALL);
			return;
		}

		else
		{
			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 1;
			af.modifier = -5;
			af.location = APPLY_DAMROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);

			af.where	= TO_WEAPON;
			af.type = sn;
			af.level = 1;
			af.duration = 1;
			af.modifier = -5;
			af.location = APPLY_HITROLL;
			af.bitvector  = WEAPON_ENCHANTED;
			affect_to_obj(obj,&af);


			act("$p is scratched by your failed attempts to enscribe runes on it.",ch,obj,NULL,TO_ALL);
			return;
		}
	}
}
void spell_rune_warding( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int skill;
	int bonus;
	char buf[256];

	/* deal with the object case first */
	if (target == TARGET_OBJ)
	{
		obj = (OBJ_DATA *) vo;
		if (obj->item_type != ITEM_ARMOR)
		{
			act("$p is not armor, and therefore cannot be protected by your rune.",ch,obj,NULL,TO_CHAR);
			return;
		}


		if (IS_OBJ_STAT(obj,ITEM_RUNE))
		{
			act("$p already is already etched with a rune, wait until it wears off.",ch,obj,NULL,TO_CHAR);
			return;
		}


		skill = skill_lookup( "rune of warding");
		bonus = (get_skill(ch,skill))/5 + (ch->level)/10;

		af.bitvector	= 0;
		af.where		= TO_OBJECT;
		af.type			= sn;
		af.level		= level;
		af.location		= APPLY_AC;

		switch (bonus)
		{
		case 18:
			af.duration = (level)/10 +10;
			af.modifier = -4;
			sprintf(buf,"$p is enscribed with a level {Gone{x rune of warding!");
			break;
		case 19:
			af.duration = (level)/10 +10;
			af.modifier = -5;
			sprintf(buf,"$p is enscribed with a level {Gtwo{x rune of warding!");
			break;

		case 20:
			af.duration = (level)/10 +10;
			af.modifier = -6;
			sprintf(buf,"$p is enscribed with a level {Gthree{x rune of arding!");
			break;

		case 21:
			af.duration = (level)/10 +12;
			af.modifier = -7;
			sprintf(buf,"$p is enscribed with a level {Gfour{x rune of warding!");
			break;

		case 22:
			af.duration = (level)/10 +12;
			af.modifier = -8;
			sprintf(buf,"$p is enscribed with a level {Gfive{x rune of warding!");
			break;

		case 23:
			af.duration = (level)/10 +14;
			af.modifier = -9;
			sprintf(buf,"$p is enscribed with a level {Gsix{x rune of warding!");
			break;

		case 24:
			af.duration = (level)/10 +14;
			af.modifier = -10;
			sprintf(buf,"$p is enscribed with a level {Gseven{x rune of warding!");
			break;

		case 25:
			af.duration = (level)/10 +14;
			af.modifier = -12;
			sprintf(buf,"$p is enscribed with a level {Geight{x rune of warding!");
			break;

		case 26:
			af.duration = (level)/10 +20;
			af.modifier = -14;
			sprintf(buf,"$p is enscribed with a level {Gnine{x rune of warding!");
			break;

		case 27:
			af.duration = (level)/10 +20;
			af.modifier = -16;
			sprintf(buf,"$p is enscribed with a level {Gten{x rune of warding!");
			break;

		case 28:
			af.duration = (level)/10 +20;
			af.modifier = -18;
			sprintf(buf,"$p is enscribed with a level {Geleven{x rune of warding!");
			break;

		case 29:
			af.duration = (level)/10 +20;
			af.modifier = -20;
			sprintf(buf,"$p is enscribed with a level {Gtwelve{x rune of warding!");
			break;

		case 30:
			af.duration = 25+(level)/10;
			af.modifier = -25;
			sprintf(buf,"$p is enscribed with a {Ghero level{x rune of warding!");
			break;

		case 31:
			af.duration = 200;
			af.modifier = -100;
			sprintf(buf,"$p is enscribed with an {Gimmortal level{x rune of warding!");
			break;

		default:
			af.duration = 1;
			af.modifier = 5;
			sprintf(buf,"$p is scratched by $n's failed attempts to enscribe runes on it.");
			break;
		}
		SET_BIT(obj->extra_flags,ITEM_RUNE);
		affect_to_obj(obj,&af);
		act(buf,ch,obj,NULL,TO_ALL);
	}
}


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

/*
 *	Draconian Skill File
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "arena.h"
#include "interp.h"
#include "tables.h"
#include "olc.h"
#include "recycle.h"
#include "lookup.h"

/* ********************************************************************
   This is the find familiar skill.
   April 4 1998 by Gothar
   
   This skill allows your players to
   have a companion like those loveable 
   pets in the pet shops.
   
   Email me if you use it. Leave this header
   to say Gothar had a great idea there.
   gothar@magma.ca
   mcco0055@algonquinc.on.ca
 * ******************************************************************** */

void do_find_dragon ( CHAR_DATA *ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *pet;
    int i;
	int chance;

	if (( chance = get_skill(ch,gsn_find_dragon)) == 0
	|| (!IS_NPC(ch)
   && ch->level < skill_table[gsn_find_dragon].skill_level[ch->class]))
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
    
   if ( ( pMobIndex = get_mob_index(MOB_VNUM_DRAGON) ) == NULL )
   {
       send_to_char( "The dragon mob doesn't exist.\n\r", ch );
       return;
   }
    /* can't cast the spell in these sectors */
    if (ch->in_room->sector_type == SECT_WATER_NOSWIM)
    {
       send_to_char("You are feeling too sick to concentrate.\n\r",ch);
       return;
    }
     	
    pet = create_mobile( pMobIndex );
    
    pet->level = number_fuzzy(ch->level / 2);
    pet->mana = pet->max_mana = 0;
    pet->hit = pet->max_hit = number_fuzzy(ch->max_hit);
    for(i = 0; i < 4; i++)
       pet->armor[i] = number_fuzzy(ch->armor[i] - 10);
    pet->hitroll = number_fuzzy(ch->level*2);
    pet->damroll = number_fuzzy(ch->level*2);
	pet->damage[DICE_NUMBER]	+= number_range((ch->level/20)-3,(ch->level/15)+3);
	pet->damage[DICE_TYPE]		+= number_range((ch->level/20)-2,(ch->level/15)+3);
    
    /* free up the old mob names */ 
    free_string(pet->description);
    free_string(pet->name);
    free_string(pet->short_descr);
    free_string(pet->long_descr);
    
    /* terrain */
    switch(ch->in_room->sector_type)
    {
	case(SECT_CITY): /*ahzi dahaka earth dragon */
	case(SECT_FIELD):
			pet->description = 
				  str_dup("You have summoned Ahzi Dahakra the Earth Dragon\n\r");
			pet->short_descr = str_dup("Ahzi Dahakra");
			pet->long_descr = str_dup("Ahzi Dahakra the Earth Dragon is here.\n\r");
			pet->name = str_dup("dragon earth ahzi dahakra");
			pet->dam_type = 10; /* bite */
	    break;
	case(SECT_FOREST):  /*tirzanth the green dragon */
	case(SECT_HILLS):
			pet->description = 
				  str_dup("You have summoned Tirzanth the Green Dragon\n\r");
			pet->short_descr = str_dup("Tirzanth");
			pet->long_descr = str_dup("Tirzanth the Green Dragon is here.\n\r");
			pet->name = str_dup("dragon green tirzanth");
			pet->dam_type = 5; /* claw */
	    break;
	case(SECT_MOUNTAIN): /*quetzalcoatl thunder dragon */
			pet->description = 
				  str_dup("You have summoned Quetzalcoatl the Thunder Dragon\n\r");
			pet->short_descr = str_dup("Quetzalcoatl");
			pet->long_descr = str_dup("Quetzalcoatl the Thunder Dragon is here.\n\r");
			pet->name = str_dup("dragon thunder Quetzalcoatl");
			pet->dam_type = 10; /* bite */
	    break;
	case(SECT_DESERT): /* sandworm */
			pet->description = 
				  str_dup("You have summoned a gigantic Sandworm\n\r");
			pet->short_descr = str_dup("Shai Hulud");
			pet->long_descr = str_dup("A gigantic Sandworm is here.\n\r");
			pet->name = str_dup("sandworm worm shai hulud");
			pet->dam_type = 12; /* suction */
		break;
	case(SECT_AIR):/*phantasmal wyvern*/
			pet->description =
				str_dup("You have summoned a majestive wyvern of the illusionary realm\n\r");
			pet->short_descr = str_dup("a Phantasmal Wyvern");
			pet->long_descr = str_dup("A phantasmal Wyvern is here.\n\r");
			pet->name = str_dup("illsuionary phantasmal wyvern");
			pet->dam_type = 6; /*blast*/
		break;
	case(SECT_WATER_SWIM):/*7-headed hydra*/
			pet->description =
				str_dup("You have summoned a 7-headed Hydra\n\r");
			pet->short_descr = str_dup("the Hydra");
			pet->long_descr = str_dup("A seven headed Hydra is here.\n\r");
			pet->name = str_dup("seven headed hydra");
			pet->dam_type = 30; /*frbite*/
		break;
	case(SECT_INSIDE):/*tiamut, dragon of death*/
			pet->description =
				str_dup("You have summoned Tiamut, the dragon of death\n\r");
			pet->short_descr = str_dup("Tiamut");
			pet->long_descr = str_dup("Tiamut, the dragon of death is here\n\r");
			pet->name = str_dup("tiamut dragon death");
			pet->dam_type = 41; /*banshee wail*/
		break;
    }
    /* player seen stuff here */
    do_sit(ch,"");
    char_to_room( pet, ch->in_room );
    act( "You begin to chant and call to $N!.",ch,NULL,pet,TO_CHAR);
    act( "$n begins to chant and calls to $N!", ch, NULL, pet, TO_ROOM );
    WAIT_STATE(ch, 2 * PULSE_MOBILE);
    add_follower( pet, ch );
    pet->leader = ch;
    ch->pet = pet;
    do_stand(ch,"");	

    SET_BIT(pet->act, ACT_PET);
    SET_BIT(pet->affected_by, AFF_CHARM);
    ch->move -= (pet->level / 2);  /* physically draining loss of move */
    check_improve(ch,gsn_find_dragon,TRUE,6);
    return;
}

void do_sever( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
	int chance;
	int percent;
	int skill;

    one_argument( argument, arg );

	if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	send_to_char("But you aren't fighting anyone!\n\r",ch);
	return;
	}
    }

	else if ((victim = get_char_room(ch,NULL,arg)) == NULL)
    {
    send_to_char("They aren't here.\n\r",ch);
    return;
    }
 
    if ( is_safe( ch, victim ) )
      return;
	if (!know_skill(ch,"sever"))
		return;
 
    if (IS_NPC(victim) 
		&& victim->fighting != NULL 
		&& !is_same_group(ch,victim->fighting) 
		&& !is_arena(victim))
    {
    send_to_char("Kill stealing is not permitted.\n\r",ch);
    return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char( "You must be fighting in order to sever.\n\r", ch );
        return;
    }

	if(victim == ch)
    {
	send_to_char("You reach your neck fine, but cant' get your ankles.\n\r",ch);
	return;
    }

	percent = 10 - (( 100 * victim->hit ) / victim->max_hit);
	skill = get_skill(ch,gsn_sever)/10;
	chance = 100 * (skill + percent);
	if ( number_percent( ) < chance )
	{
		check_killer( ch, victim );
		WAIT_STATE( ch, skill_table[gsn_sever].beats );
		if ( number_percent( ) < get_skill(ch,gsn_sever)
		|| ( get_skill(ch,gsn_sever) >= 2 && !IS_AWAKE(victim) ) )
		{
			if(!IS_NPC(victim))
			act("{W$n picks you up and rips you in half! Oh no!{x",ch,NULL,victim,TO_VICT);
			act("{W$n picks up $N and rips $S legs off!{x",ch,NULL,victim,TO_NOTVICT);
			send_to_char("{WYou grab them by the neck and legs and viciously rip them in half!{x\n\r",ch);
			check_improve(ch,gsn_sever,TRUE,1);
			one_hit( ch, victim, gsn_sever, TRUE,FALSE );

			obj = create_object( get_obj_index(OBJ_VNUM_LEGS ), 0 );
			obj->timer = 5;
			add_sever(ch,victim);

			if(IS_NPC(victim))
			sprintf(buf,"A pair of %s's legs are here, twitching.",victim->short_descr);
			else
			sprintf(buf,"A pair of %s's legs are here, twitching.",victim->name);	
			free_string(obj->description);
			obj->description = str_dup( buf );
	

			if(!IS_NPC(victim))
			sprintf(buf,"A pair of %s's legs",victim->short_descr);
			else
			sprintf(buf,"A pair of %s's legs",victim->name);    
			free_string(obj->short_descr);
			obj->short_descr = str_dup( buf );

			obj_to_char(obj,ch);
		}
	}
	else
    {
        WAIT_STATE( ch, skill_table[gsn_sever].beats );
		send_to_char("{WYou didnt manage to rip them in half, best wait till they're more damgaged.{x\n\r", ch);
		check_improve(ch,gsn_sever,FALSE,1);
        damage( ch, victim, 0, gsn_sever,DAM_NONE,TRUE,FALSE);
    }
}

void spell_venom_lance( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
	int attacks;
	int chance;
    static const sh_int dam_each[] = 
    {
	 0,
	 10,  10,  15,  20,  20, 20,  25,  28,  30,  32,
	 34,  36,  38,  40,  45, 48,  50,  52,  56,  60,
	 64,  66,  70,  72,  75, 78,  82,  86,  91,  97,
	102, 108, 111, 118, 121, 123, 128, 130, 132, 136,
	138, 140, 145, 150, 155, 160, 170, 175, 180, 200
    };
    int dam;

    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    if ( saves_spell( level, victim,DAM_ENERGY) )
	
	act("{W$n conjures a spear of {Gp{Bu{Gr{Be {Gv{Be{Gn{Bo{Gm{W and launches it at you!{x",ch,NULL,victim,TO_VICT);
	act("{W$n conjures a spear of {Gp{Bu{Gr{Be {Gv{Be{Gn{Bo{Gm{W and launches it at $N{x",ch,NULL,victim,TO_NOTVICT);
	act("{WYou conjure a spear of {Gp{Bu{Gr{Be {Gv{Be{Gn{Bo{Gm{W and launch it at $N{x",ch,NULL,victim,TO_CHAR);

	attacks = 0;
	chance = number_range(0,12);
	while ( attacks <= chance)
	{
		dam		= number_range( dam_each[level] *3, dam_each[level] * 7 );
		damage( ch, victim, dam, sn, DAM_POISON ,TRUE,FALSE);
		attacks ++;
	}
}

void spell_thermonuclear( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
	CHAR_DATA *victim_next;
    int dam;


	if (IS_SET(ch->in_room->room_flags,ROOM_ARENA)
		|| IS_SET(ch->in_room->room_flags,ROOM_HONCIRC))
	{
		send_to_char("Not in the Arena or in the Honor Circle",ch);
		return;
	}
    dam		= number_range( ch->level * 900000, ch->level * 1000000 );
	
	act("{W$n prepares to destroy EVERYTHING in a thermonuclear detonation{x",ch,NULL,victim,TO_ROOM);
	act("{WYou prepare to destroy EVERYTHING in a thermonuclear detonation{x",ch,NULL,victim,TO_CHAR);

	for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
	{
		victim_next = victim->next_in_room;
		if(is_safe(ch,victim))
			continue;
		if(victim == ch)
			continue;
		damage( ch, victim, dam, sn, DAM_NONE ,TRUE,FALSE);
	}
	damage( ch, ch, dam, sn, DAM_NONE ,TRUE,FALSE);
}


void spell_dragonfury( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim;
    AFFECT_DATA af;

	/* character target */
    victim = (CHAR_DATA *) vo;
	
	if (victim != ch)
	{
		send_to_char("You may only cast DragonFury on yourself\n\r",ch);
		return;
	}

	if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
	{
		send_to_char("You already have the dragons fury running in your blood\n\r",ch);
		return;
	}

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level / 7;
    af.location  = APPLY_HITROLL;
    af.modifier  = level / 3;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level / 7;
    af.location  = APPLY_DAMROLL;
    af.modifier  = level / 3;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char( "The rage of all the ancient dragons begins to pound in your veins\n\r", victim );
    return;
}

void spell_mystic_sight( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED2(victim, AFF_MYSTIC_SIGHT) )
    {
        if (victim == ch)
          send_to_char("You already have mystic sight.\n\r",ch);
        else
          act("$N already has mystic sight",ch,NULL,victim,TO_CHAR);
        return;
    }

    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/10;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_MYSTIC_SIGHT;
    affect_to_char( victim, &af );
    send_to_char( "The shadows around you begin to take on new meaning....\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_elemental_wrath( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	long dam;
	int fire_sn;
	int cold_sn;
	int lightning_sn;
	int acid_sn;
	int negative_sn;
	int holy_sn;


	fire_sn			= skill_lookup("wrath fire");
	cold_sn			= skill_lookup("wrath cold");
	lightning_sn	= skill_lookup("wrath lightning");
	acid_sn			= skill_lookup("wrath acid");
	negative_sn		= skill_lookup("wrath negative");
	holy_sn			= skill_lookup("wrath holy");

	act("{W$n's hands move in intricate patterns, as he chants in the language of the elder wyrms{x",
		ch,NULL,victim,TO_VICT);
	act("{W$n's hands move in intricate patterns, as he chants in the language of the elder wyrms{x",
		ch,victim,NULL,TO_NOTVICT);
	act("{WYou weave patterns of magic and call upong the wrath of the elements!{x",
		ch,victim,NULL,TO_CHAR);

	/*Fire*/
	dam = (ch->level) * number_range(1,3) + number_range(4,7) * 10;
	dam *= 2.5;
	damage( ch, victim, dam, fire_sn, DAM_FIRE ,TRUE,FALSE);

	/*Cold*/
	dam = (ch->level) * number_range(1,3) + number_range(4,7) * 10;
	dam *= 2.5;
	damage( ch, victim, dam, cold_sn, DAM_COLD ,TRUE,FALSE);

	/*Acid*/
	dam = (ch->level) * number_range(1,3) + number_range(4,7) * 10;
	dam *= 2.5;
	damage( ch, victim, dam, lightning_sn, DAM_LIGHTNING ,TRUE,FALSE);

	/*Lightning*/
	dam = (ch->level) * number_range(1,3) + number_range(4,7) * 10;
	dam *= 2.5;
	damage( ch, victim, dam, acid_sn, DAM_ACID ,TRUE,FALSE);

	/*Negative*/
	damage( ch, victim, dam, negative_sn, DAM_NEGATIVE ,TRUE,FALSE);

	/*Holy*/
	damage( ch, victim, dam, holy_sn, DAM_HOLY ,TRUE,FALSE);
}

void spell_skeleton_dance( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	long dam;
	int skeleton_sn;
	int skeleton1_sn;
	int skeleton2_sn;
	int wraith_sn;
	int chance;


	skeleton_sn		= skill_lookup("skeletal mage");
	skeleton1_sn	= skill_lookup("skeletal archer");
	skeleton2_sn	= skill_lookup("skeletal warrior");
	wraith_sn		= skill_lookup("phantasmal wraith");

	act("{D$n summons dark warriors from the depths of hell!  {x",ch,NULL,victim,TO_VICT);
	act("{D$n summons dark warriors from the depths of hell!  {x",ch,victim,NULL,TO_NOTVICT);
	act("{DYou summon dark warriors from the depths of hell!  !{x",ch,victim,NULL,TO_CHAR);

	chance = number_range(1,100);
	if (chance <= 30)
	{
		dam = ((ch->level) * number_range(1, 3)) + dice(40, 120) + number_range(200,400);
		damage( ch, victim, dam, skeleton_sn, DAM_NEGATIVE ,TRUE, FALSE);

		if (!IS_SET(victim->affected_by,AFF_WEAKEN))
		{
			af.where     = TO_AFFECTS;
			af.type      = sn;
			af.level     = level;
			af.duration  = level / 50;
			af.location  = APPLY_STR;
			af.modifier  = -1 * (level / 4);
			af.bitvector = AFF_WEAKEN;
			affect_to_char( victim, &af );
			send_to_char( "You feel your muscles deteriorating! \n\r", victim );
		}

		if (!IS_SET(victim->affected_by,AFF_BLIND))
		{
			af.where     = TO_AFFECTS;
			af.type      = sn;
			af.level     = level;
			af.location  = APPLY_HITROLL;
			af.modifier  = -50;
			af.duration  = level/50;
			af.bitvector = AFF_BLIND;
			affect_to_char( victim, &af );
			send_to_char( "Everything suddenly goes dark! \n\r", victim );
		}
	}
	else if (chance <=60)
	{
		dam = ((ch->level) * number_range(1, 3)) + dice(45, 120) + number_range(200,400);
		damage( ch, victim, dam, skeleton1_sn, DAM_NEGATIVE ,TRUE, FALSE);

		if (!IS_SET(victim->affected_by,AFF_SLOW))
		{
			af.where     = TO_AFFECTS;
			af.type      = sn;
			af.level     = level;
			af.duration  = level/50;
			af.location  = APPLY_DEX;
			af.modifier  = -1 * (level/5);
			af.bitvector = AFF_SLOW;
			affect_to_char( victim, &af );
			send_to_char( "You feel yourself slowing d o w n...\n\r", victim );
		}
	}

	else if (chance <= 90)
	{
		dam = ((ch->level) * number_range(1, 3)) + dice(50, 120) + number_range(200,400);
		damage( ch, victim, dam, skeleton2_sn, DAM_NEGATIVE ,TRUE, FALSE);

		if (!IS_SET(victim->affected_by,AFF_WEAKEN))
		{
			af.where     = TO_AFFECTS;			
			af.type      = sn;
			af.level     = level;
			af.duration  = level / 50;
			af.location  = APPLY_CON;
			af.modifier  = -1 * (level/4);
			af.bitvector = AFF_WEAKEN;
			affect_to_char( victim, &af );
			send_to_char( "You feel your defenses dropping! \n\r", victim );
		}
	}

	else
	{
		dam = ((ch->level) * number_range(1, 3)) + dice(55, 120) + number_range(200,400);
		damage( ch, victim, dam, wraith_sn, DAM_NEGATIVE ,TRUE, FALSE);
		
		if (!IS_SET(victim->affected_by,AFF_POISON))
		{
			af.where     = TO_AFFECTS;
			af.type      = sn;
			af.level     = level;
			af.duration  = level/50;
			af.location  = APPLY_DAMROLL;
			af.modifier  = -50;
			af.bitvector = AFF_POISON;
			affect_join( victim, &af );
			send_to_char( "You are seized by a fit of coughing.\n\r", victim );
		}
        
		if (!IS_SET(victim->affected_by,AFF_FAERIE_FIRE))
		{
			af.where     = TO_AFFECTS;
			af.type      = sn;
			af.level	 = level;
			af.duration  = level/50;
			af.location  = APPLY_AC;
			af.modifier  = 10 * level;
			af.bitvector = AFF_FAERIE_FIRE;
			affect_to_char( victim, &af );
			send_to_char( "You are surrounded by a bright pink outline.\n\r", victim );
		}
	}
}
void spell_exodus( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
	int random = number_range(1,4);

    act("$n clenches $s fist as $s eyes flash a deep crimson.", ch, NULL, NULL, TO_ROOM );
	act("You mentally grasp hold of $N's life force and rip it forcefully out of $S body",ch,NULL,victim,TO_CHAR);
	dam		= number_range( ch->level * 15, ch->level * 30 );
    damage( ch, victim, dam, sn, DAM_NONE ,TRUE,FALSE);
	switch (random)
	{
	case 1: break;
	case 2: break;
	case 3:
		act("$N's eyes suddenly go dark, as if $S very life has been forcefully ripped from $S body!",ch,NULL,victim,TO_NOTVICT);
		act("Suddenly a gut wrenching pain flashes through you, and you are left void of mental energies!",ch,NULL,victim,TO_VICT);
		stc("{WManaburn{x\n\r",ch);
		victim->mana = 0;
		break;
	case 4:
		act("$N crumples to the ground in a heap, as $n's exodus spell reduces $N to a spasming wretch!",ch,NULL,victim,TO_NOTVICT);
		act("Suddenly all your muscles go flacid, and you collapse to the ground like a sack of bricks",ch,NULL,victim,TO_VICT);
		stc("{BMoveburn{x\n\r",ch);
		victim->move = 0;
		break;
	default:
		break;
	}
    return;
}

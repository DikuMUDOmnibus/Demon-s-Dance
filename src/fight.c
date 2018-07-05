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

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "arena.h"
#include "olc.h"
#include "slay.h"
#include "skill_tree.h"

/*
 * Local functions.
 */
void	check_assist	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_shield_wall args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_shield_block     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_counter   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt ) );
bool	check_precision	args( ( CHAR_DATA *ch, CHAR_DATA *victim, sh_int gsn) );
void    dam_message 	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                            int dt, bool immune, bool form ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim, 
			    int total_levels ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	make_corpse	args( ( CHAR_DATA *ch ) );
void    one_hit     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary, bool form ) );
void    mob_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	raw_kill	args( ( CHAR_DATA *killer, CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_phase     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool arena_is_busy;       
bool	check_holy_armor	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	add_hero_exp	args( ( CHAR_DATA *gch, CHAR_DATA *victim, 
			    int total_levels ) );
void    pk_kill         args( ( CHAR_DATA *victim ) );

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
	OBJ_DATA *obj, *obj_next;
	bool room_trig = FALSE;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next	= ch->next;

	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	else
	    stop_fighting( ch, FALSE );

	if ( ( victim = ch->fighting ) == NULL )
	    continue;

	/*
	 * Fun for the whole family!
	 */
	check_assist(ch,victim);

	if ( IS_NPC( ch ) )
	{
	    if ( HAS_TRIGGER_MOB( ch, TRIG_FIGHT ) )
		p_percent_trigger( ch, NULL, NULL, victim, NULL, NULL, TRIG_FIGHT );
	    if ( HAS_TRIGGER_MOB( ch, TRIG_HPCNT ) )
		p_hprct_trigger( ch, victim );
	}
	for ( obj = ch->carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->wear_loc != WEAR_NONE && HAS_TRIGGER_OBJ( obj, TRIG_FIGHT ) )
		p_percent_trigger( NULL, obj, NULL, victim, NULL, NULL, TRIG_FIGHT );
	}

	if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_FIGHT ) && room_trig == FALSE )
	{
	    room_trig = TRUE;
	    p_percent_trigger( NULL, NULL, ch->in_room, victim, NULL, NULL, TRIG_FIGHT );
	}
    }

    return;
}

/* for auto assisting */
void check_assist(CHAR_DATA *ch,CHAR_DATA *victim)
{
    CHAR_DATA *rch, *rch_next;

    for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
    {
	rch_next = rch->next_in_room;
	
	if (IS_AWAKE(rch) && rch->fighting == NULL)
	{

	    /* quick check for ASSIST_PLAYER */
	    if (!IS_NPC(ch) && IS_NPC(rch) 
	    && IS_SET(rch->off_flags,ASSIST_PLAYERS)
	    &&  rch->level + 6 > victim->level)
	    {
		do_function(rch, &do_emote, "screams and attacks!");
		multi_hit(rch,victim,TYPE_UNDEFINED);
		continue;
	    }

	    /* PCs next */
	    if (!IS_NPC(ch) || IS_AFFECTED(ch,AFF_CHARM))
	    {
		if ( ( (!IS_NPC(rch) && IS_SET(rch->act,PLR_AUTOASSIST))
		||     IS_AFFECTED(rch,AFF_CHARM)) 
		&&   is_same_group(ch,rch) 
		&&   !is_safe(rch, victim))
		    multi_hit (rch,victim,TYPE_UNDEFINED);
		
		continue;
	    }
  	
	    /* now check the NPC cases */
	    
 	    if (IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM))
	
	    {
		if ( (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALL))

		||   (IS_NPC(rch) && rch->group && rch->group == ch->group)

		||   (IS_NPC(rch) && rch->race == ch->race 
		   && IS_SET(rch->off_flags,ASSIST_RACE))

		||   (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALIGN)
		   &&   ((IS_GOOD(rch)    && IS_GOOD(ch))
		     ||  (IS_EVIL(rch)    && IS_EVIL(ch))
		     ||  (IS_NEUTRAL(rch) && IS_NEUTRAL(ch)))) 

		||   (rch->pIndexData == ch->pIndexData 
		   && IS_SET(rch->off_flags,ASSIST_VNUM)))

	   	{
		    CHAR_DATA *vch;
		    CHAR_DATA *target;
		    int number;

		    if (number_bits(1) == 0)
			continue;
		
		    target = NULL;
		    number = 0;
		    for (vch = ch->in_room->people; vch; vch = vch->next)
		    {
			if (can_see(rch,vch)
			&&  is_same_group(vch,victim)
			&&  number_range(0,number) == 0)
			{
			    target = vch;
			    number++;
			}
		    }

		    if (target != NULL)
		    {
			do_function(rch, &do_emote, "screams and attacks!");
			multi_hit(rch,target,TYPE_UNDEFINED);
		    }
		}	
	    }
	}
    }
}


/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
	char buf1[256], buf2[256], buf3[256];
	int chance;
    int dam;
	int sn;
	int fshield_dam;
	OBJ_DATA *wield;
	OBJ_DATA *secondary;

    /* decrement the wait */
    if (ch->desc == NULL)
	ch->wait = UMAX(0,ch->wait - PULSE_VIOLENCE);

    if (ch->desc == NULL)
	ch->daze = UMAX(0,ch->daze - PULSE_VIOLENCE); 


    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
	return;

    if (IS_NPC(ch))
    {
	mob_hit(ch,victim,dt);
	return;
    }
	
	/*original attack*/
	check_dual_attack(ch,victim, dt);

    if (ch->fighting != victim)
	return;

    if (IS_AFFECTED(ch,AFF_HASTE))
	check_dual_attack(ch,victim, dt);


    if ( ch->fighting != victim || dt == gsn_backstab )
	return;

/*Second Attack*/
    chance = ((get_skill(ch,gsn_second_attack) * 95) / 100) ;

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance /= 2;

    if ( number_percent( ) < chance )
    {
		check_dual_attack(ch,victim, dt);
		check_improve(ch,gsn_second_attack,TRUE,5);
		if ( ch->fighting != victim )
			return;
    }

/*Third Attack*/
    chance = ((get_skill(ch,gsn_third_attack) * 3) / 4);

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance = 0;

    if ( number_percent( ) < chance )
    {
		check_dual_attack(ch,victim, dt);
		check_improve(ch,gsn_third_attack,TRUE,5);
		if ( ch->fighting != victim )
			return;
    }


/*Fourth Attack*/
	chance = get_skill(ch,gsn_fourth_attack)/2;

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance = 0;

    if ( number_percent( ) < chance )
    {
		check_dual_attack(ch,victim, dt);
		check_improve(ch,gsn_fourth_attack,TRUE,6);
		if ( ch->fighting != victim )
			return;
    }
    
/*Fifth Attack*/
	chance = get_skill(ch,gsn_fifth_attack)/3;

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance = 0;

    if ( number_percent( ) < chance )
    {
		check_dual_attack(ch,victim, dt);
		check_improve(ch,gsn_fifth_attack,TRUE,6);
		if ( ch->fighting != victim )
			return;
    }

/*Divine Right*/
    if (IS_AFFECTED2(ch,AFF_DIVINE_RIGHT))
	{
		sn = skill_lookup( "divine right");
		chance = ((ch->level*2)/3);
		if ( number_percent( ) < chance )
		{
		one_hit( ch, victim, sn, FALSE,FALSE );
		}
	
	check_improve(ch,sn,TRUE,6);
	if ( ch->fighting != victim )
	    return;
	}

/*Burst of Speed (chance for 3 attacks)*/
    if (is_affected(ch,gsn_burst_of_speed))
	{
		/*First Burst*/
		chance = ((ch->level*2)/3);
		if ( number_percent( ) < chance )
		{
			act("{W$n's speed is too much for you, as $e{W slips in another attack{x",ch,NULL,victim,TO_VICT);
			act("{W$n's speed is too much for $N, as $e{W slips in another attack{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYour speed is too much for $N, and you are ableto slip in another attack{x",ch,NULL,victim,TO_CHAR);
			check_dual_attack(ch,victim,dt);
			check_improve(ch,gsn_burst_of_speed,TRUE,6);
		}
		/*Second Burst*/
		chance = ((ch->level)/2);
		if ( number_percent( ) < chance )
		{
			act("{W$n's speed is too much for you, as $e{W slips in another attack{x",ch,NULL,victim,TO_VICT);
			act("{W$n's speed is too much for $N, as $e{W slips in another attack{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYour speed is too much for $N, and you are ableto slip in another attack{x",ch,NULL,victim,TO_CHAR);
			check_dual_attack(ch,victim,dt);
			check_improve(ch,gsn_burst_of_speed,TRUE,6);
		}
		/*Third Burst*/
		chance = ((ch->level)/4);
		if ( number_percent( ) < chance )
		{
			act("{W$n's speed is too much for you, as $e{W slips in another attack{x",ch,NULL,victim,TO_VICT);
			act("{W$n's speed is too much for $N, as $e{W slips in another attack{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYour speed is too much for $N, and you are ableto slip in another attack{x",ch,NULL,victim,TO_CHAR);
			check_dual_attack(ch,victim,dt);
			check_improve(ch,gsn_burst_of_speed,TRUE,6);
		}
	if ( ch->fighting != victim )
	    return;
	}

    if (IS_AFFECTED2(ch,AFF_FIRESHIELD))
	{
		sn = skill_lookup( "fireshield");
		fshield_dam = number_range(ch->level, ch->level * 3);
		act("{WYou are burned by $n's fireshield{x",ch,NULL,victim,TO_VICT);
		act("{W$N is burned by $n's fireshield{x",ch,NULL,victim,TO_NOTVICT);
		act("{WYour fireshield burns $N{x",ch,NULL,victim,TO_CHAR);
		damage( ch, victim, fshield_dam, sn, DAM_FIRE, TRUE,FALSE );
	if ( ch->fighting != victim )
	    return;
	}

/*Vorpal*/
	chance = ch->level/75;
	if (( number_range(0,700) < chance ) 
		&&	(victim->max_hit < 1000000))
	{
			dam = victim->hit;
			raw_kill(ch,victim);
			sprintf( buf1, "{R$n's {Wvorpal {Rdecapitates $N{x");
	    	sprintf( buf2, "{RYour {Wvorpal {Rdecapitates $N{x");
	    	sprintf( buf3, "{R$n's {Wvorpal {Rdecapitates you{x");
    		act( buf1, ch, NULL, victim, TO_NOTVICT );
    		act( buf2, ch, NULL, victim, TO_CHAR );
    		act( buf3, ch, NULL, victim, TO_VICT );
			return;
	}
	/*weapon_spell*/
	if ((wield = get_eq_char( ch, WEAR_WIELD )) != NULL)
	{
		int sn;
		int skill;

		if (wield->pIndexData->weapon_spell != -1
			&& wield->pIndexData->weapon_spell != 0)
		{
			/* get the weapon skill */
			sn = get_weapon_sn(ch);
			skill = 20 + get_weapon_skill(ch,sn);
			chance = ((ch->level + skill) / 5);
			if (chance > number_range(0,100))
			{
				weapon_spell(ch,wield,wield->pIndexData->weapon_spell);
			}
		}
	}
	if ((secondary = get_eq_char( ch, WEAR_SECONDARY )) != NULL)
	{
		int sn;
		int skill;

		if (secondary->pIndexData->weapon_spell != -1
			&& secondary->pIndexData->weapon_spell != 0)
		{
			/* get the weapon skill */
			sn = get_secondary_sn(ch);
			skill = 20 + get_weapon_skill(ch,sn);
			chance = ((ch->level + skill) / 7);
			if (chance > number_range(0,100))
			{
				weapon_spell(ch,secondary,secondary->pIndexData->weapon_spell);
			}
		}
	}
}

/* procedure for all mobile attacks */
void mob_hit (CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
	int chance,number;
	CHAR_DATA *vch, *vch_next;


	if (get_eq_char (ch, WEAR_SECONDARY))
	{
		one_hit( ch, victim, dt, FALSE,FALSE );
		one_hit( ch, victim, dt, TRUE,FALSE );
	}

	/*Second Attack*/

	if ((IS_SET(ch->act,ACT_WARRIOR))
		|| ch->hitroll >= 100)
		chance = 85;
	else
		chance = 0;
	if (IS_AFFECTED(ch,AFF_SLOW))
		chance /= 2;

	if ( number_percent( ) < chance )
	{
		check_dual_attack(ch,victim, dt);
		if ( ch->fighting != victim )
			return;
	}

	/*Third Attack*/
	if (IS_SET(ch->act,ACT_WARRIOR)
		|| ch->hitroll >= 100)
		chance = 85;
	else
		chance = 0;
	if (IS_AFFECTED(ch,AFF_SLOW))
		chance /= 2;

	if ( number_percent( ) < chance )
	{
		check_dual_attack(ch,victim, dt);
		if ( ch->fighting != victim )
			return;
	}


	/*Fourth Attack*/
	if (ch->hitroll >= 200)
		chance = 85;
	else
		chance = 0;
	if (IS_AFFECTED(ch,AFF_SLOW))
		chance /= 2;

	if ( number_percent( ) < chance )
	{
		check_dual_attack(ch,victim, dt);
		if ( ch->fighting != victim )
			return;
	}

	/*Fifth Attack*/
	if (ch->hitroll >= 200)
		chance = 85;
	else
		chance = 0;
	if (IS_AFFECTED(ch,AFF_SLOW))
		chance /= 2;

	if ( number_percent( ) < chance )
	{
		check_dual_attack(ch,victim, dt);
		if ( ch->fighting != victim )
			return;
	}
	else
		one_hit( ch, victim, dt, FALSE,FALSE );
	return;

	/*Sixth Attack*/
	if (ch->hitroll >= 300)
		chance = 85;
	else
		chance = 0;
	if (IS_AFFECTED(ch,AFF_SLOW))
		chance /= 2;

	if ( number_percent( ) < chance )
	{
		check_dual_attack(ch,victim, dt);
		if ( ch->fighting != victim )
			return;
	}
	else
		one_hit( ch, victim, dt, FALSE,FALSE );
	return;

	/*7th Attack*/
	if (ch->hitroll >= 400)
		chance = 85;
	else
		chance = 0;
	if (IS_AFFECTED(ch,AFF_SLOW))
		chance /= 2;

	if ( number_percent( ) < chance )
	{
		check_dual_attack(ch,victim, dt);
		if ( ch->fighting != victim )
			return;
	}
	else
		one_hit( ch, victim, dt, FALSE,FALSE );
	return;

	/*8th Attack*/
	if (ch->hitroll >= 450)
		chance = 85;
	else
		chance = 0;
	if (IS_AFFECTED(ch,AFF_SLOW))
		chance /= 2;

	if ( number_percent( ) < chance )
	{
		check_dual_attack(ch,victim, dt);
		if ( ch->fighting != victim )
			return;
	}
	else
		one_hit( ch, victim, dt, FALSE,FALSE );
	return;


	if (ch->fighting != victim)
		return;

	/* Area attack -- BALLS nasty! */

	if (IS_SET(ch->off_flags,OFF_AREA_ATTACK))
	{
		for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
		{
			vch_next = vch->next;
			if ((vch != victim && vch->fighting == ch))

				check_dual_attack(ch,victim, dt);
			return;

		}
	}

	if (IS_AFFECTED(ch,AFF_HASTE) 
		||  (IS_SET(ch->off_flags,OFF_FAST) && !IS_AFFECTED(ch,AFF_SLOW)))

		check_dual_attack(ch,victim, dt);
	return;


	if (ch->fighting != victim || dt == gsn_backstab)
		return;

	chance = get_skill(ch,gsn_second_attack)/2;

	if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
		chance /= 2;

	if (number_percent() < chance)
	{
		if (ch->fighting != victim)
			return;
		check_dual_attack(ch,victim, dt);
		return;
	}

	chance = get_skill(ch,gsn_third_attack)/4;

	if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
		chance = 0;

	if (number_percent() < chance)
	{
		if (ch->fighting != victim)
			return;
		check_dual_attack(ch,victim, dt);
		return;
	} 

	/* oh boy!  Fun stuff! */

	if (ch->wait > 0)
		return;

	number = number_range(0,2);

	if (number == 1 && IS_SET(ch->act,ACT_MAGE))
	{
		/*	{ mob_cast_mage(ch,victim); return; };*/
	}

	if (number == 2 && IS_SET(ch->act,ACT_CLERIC))
	{	
		/*{ mob_cast_cleric(ch,victim); return; };*/
	}

	/* now for the skills */

	number = number_range(0,8);

	switch(number) 
	{
	case (0) :
		if (IS_SET(ch->off_flags,OFF_BASH))
			do_function(ch, &do_bash, "");
		break;

	case (1) :
		if (IS_SET(ch->off_flags,OFF_BERSERK) && !IS_AFFECTED(ch,AFF_BERSERK))
			do_function(ch, &do_berserk, "");
		break;


	case (2) :
		if (IS_SET(ch->off_flags,OFF_DISARM) 
			|| (get_weapon_sn(ch) != gsn_hand_to_hand 
			&& (IS_SET(ch->act,ACT_WARRIOR)
			||  IS_SET(ch->act,ACT_THIEF))))
			do_function(ch, &do_disarm, "");
		break;

	case (3) :
		if (IS_SET(ch->off_flags,OFF_KICK))
			do_function(ch, &do_kick, "");
		break;

	case (4) :
		if (IS_SET(ch->off_flags,OFF_KICK_DIRT))
			do_function(ch, &do_dirt, "");
		break;

	case (5) :
		if (IS_SET(ch->off_flags,OFF_TAIL))
		{
			/* do_function(ch, &do_tail, "") */ ;
		}
		break; 

	case (6) :
		if (IS_SET(ch->off_flags,OFF_TRIP))
			do_function(ch, &do_trip, "");
		break;

	case (7) :
		if (IS_SET(ch->off_flags,OFF_CRUSH))
		{
			/* do_function(ch, &do_crush, "") */ ;
		}
		break;
	case (8) :
		if (IS_SET(ch->off_flags,OFF_BACKSTAB))
		{
			do_function(ch, &do_backstab, "");
		}
	}
}
	

/*
 * Hit one guy once.
 */
void one_hit ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary,bool form )
{
    OBJ_DATA *wield;
    int victim_ac,thac0,thac0_00,thac0_32,dam,diceroll;
    int sn,skill,dam_type,talent_bonus,talent_mod;
    bool result;
	bool mob = FALSE,add_talent = FALSE;

    sn = -1;
    talent_mod = 0;


    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
	return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    /*
     * Figure out the type of damage message.
     * if secondary == true, use the second weapon.
	 */
     
    if (!secondary)
        wield = get_eq_char( ch, WEAR_WIELD );
    if (secondary)
        wield = get_eq_char( ch, WEAR_SECONDARY );

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else 
	    dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
	{
    	if (wield != NULL)
		{
			if (IS_NPC(ch))
			{
				mob = TRUE;
				dam_type = attack_table[wield->value[3]].damage;
			}
			else
			{
				dam_type = attack_table[wield->value[3]].damage;
				mob = FALSE;
			}
		}
		dam_type = attack_table[ch->dam_type].damage;
	}
    else
    	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
	dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn(ch);
    skill = 20 + get_weapon_skill(ch,sn);

	talent_bonus = 0;
	if(!IS_NPC(ch) && sn_talent(sn) != -1)
	{
		debug(ch,"one_hit: sn=%d\tskill=%s\n\r",sn,skill_table[sn].name);
		talent_bonus = ch->pcdata->skills_array[sn_talent(sn)];
	}

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0_00 = 20;
	thac0_32 = -4;   /* as good as a thief */ 
	if (IS_SET(ch->act,ACT_WARRIOR))
	    thac0_32 = -10;
	else if (IS_SET(ch->act,ACT_THIEF))
	    thac0_32 = -4;
	else if (IS_SET(ch->act,ACT_CLERIC))
	    thac0_32 = 2;
	else if (IS_SET(ch->act,ACT_MAGE))
	    thac0_32 = 6;
    }
    else
    {
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0  = interpolate( ch->level, thac0_00, thac0_32 );

    if (thac0 < 0)
        thac0 = thac0/2;

    thac0 -= GET_HITROLL(ch) * skill/100;
    thac0 += 5 * (100 - skill) / 100;

    if (dt == gsn_backstab)
	thac0 -= 8 * (100 - get_skill(ch,gsn_backstab));

	if (dt == skill_lookup("mantis dance"))
	thac0 -= 8 * (100 - get_skill(ch,skill_lookup("mantis dance")));

	if (dt == skill_lookup("deadly fingertips"))
	thac0 -= 8 * (100 - get_skill(ch,skill_lookup("deadly fingertips")));

	if (dt == gsn_circle)
    thac0 -= 8 * (100 - get_skill(ch,gsn_circle));

	if (dt == gsn_strike)
	thac0 -= 6 * (100 - get_skill(ch, gsn_strike));

	if (dt == gsn_smite)
	thac0 -= 10 * (100 - get_skill(ch, gsn_smite));

	if (dt == gsn_sever)
	thac0 -= 2 * (100 - get_skill(ch, gsn_sever));

	if (dt == gsn_haduken)
	thac0 -= 9 * (100 - get_skill(ch, gsn_haduken));

	if (dt == gsn_fatality)
	thac0 -= 2 * (100 - get_skill(ch, gsn_fatality));

    switch(dam_type)
    {
	case(DAM_PIERCE):victim_ac = GET_AC(victim,AC_PIERCE)/5;	break;
	case(DAM_BASH):	 victim_ac = GET_AC(victim,AC_BASH)/5;		break;
	case(DAM_SLASH): victim_ac = GET_AC(victim,AC_SLASH)/5;	break;
	default:	 victim_ac = GET_AC(victim,AC_EXOTIC)/5;	break;
    }; 

    if (victim_ac < -15)
	victim_ac = (victim_ac + 15) / 6 - 15;
     
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    if ( victim->position < POS_FIGHTING)
	victim_ac += 4;
 
    if (victim->position < POS_RESTING)
	victim_ac += 6;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt, dam_type, TRUE,FALSE );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) && (!ch->pIndexData->new_format || wield == NULL))
	{
		if (!ch->pIndexData->new_format)
		{
			dam = number_range( ch->level / 2, ch->level * 3 / 2 );
			if ( wield != NULL )
	    		dam += dam / 2;
		}
		else
			dam = dice(ch->damage[DICE_NUMBER],ch->damage[DICE_TYPE]);
	}
    else
    {
		if (sn != -1)
			check_improve(ch,sn,TRUE,5);
		if ( wield != NULL )
		{
			if (wield->pIndexData->new_format)
			{
				if (mob)
				{
					dam = dice(wield->value[1],wield->value[2]) * skill/50;
					dam += number_range( 1 + 4 * skill/50, 2 * ch->level/3 * skill/50);
				}
				else
					dam = dice(wield->value[1],wield->value[2]) * skill/50;
			}
			else
			{
				if (mob)
				{
                    dam = number_range( wield->value[1] * skill/50,wield->value[2] * skill/50);
					dam += number_range( 1 + 4 * skill/50, 2 * ch->level/3 * skill/50);
				}
				else
					dam = number_range( wield->value[1] * skill/50,wield->value[2] * skill/50);
			}

			if (get_eq_char(ch,WEAR_SHIELD) == NULL)  /* no shield = more */
				dam = dam * 11/10;

			/* sharpness! */
			if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
			{
				int percent;

				if ((percent = number_percent()) <= (skill / 8))
					dam = 2 * dam + (dam * 2 * percent / 100);
			}
		}
		else
			dam = number_range( 1 + 4 * skill/50, 2 * ch->level/3 * skill/50);
	}
	/*
	* Bonuses.
	*/
	if ( get_skill(ch,gsn_enhanced_damage) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_enhanced_damage))
        {
            check_improve(ch,gsn_enhanced_damage,TRUE,6);
            dam += 2 * ( dam * diceroll/70);
        }
    }
    
	if ( !IS_AWAKE(victim) )
		dam *= 2;
	else if (victim->position < POS_FIGHTING)
		dam = dam * 3 / 2;
/*
 *Damage Hack by Enzo and Akmaro
 *02/05/2002 2:25 AM EST
 */
	if ( dt == gsn_backstab && wield != NULL)
	{
		if ( wield->value[0] != 2 )
			dam *= 2 + (ch->level / 30); 
		else 
			dam *= 2 + (ch->level / 15);
	}
	
	if ( dt == skill_lookup("mantis dance") && wield != NULL) 
	{
		if ( wield->value[0] != 2 )
			dam *= 2 + (ch->level / 22);
		else 
			dam *= 2 + (ch->level / 11);
	}

	if ( dt == skill_lookup("deadly fingertips"))
		dam *= 2 + (ch->level/20);

	if ( dt == gsn_circle && wield != NULL)
	{
		if ( wield->value[0] != 2 )
			dam *= 2+ (ch->level / 30);
		else
			dam *= 2 + (ch->level / 15);
	}

	if ( dt == gsn_strike && wield != NULL)
	{
		if ( wield->value[0] != 1 && wield->value[0] != 3 )
			dam *= 2+ (ch->level / 16);
		else
			dam *= 2 + (ch->level / 8);
	}

	if ( dt == gsn_smite )
		dam *= 2+ (ch->level / 20);
	
	if ( dt == gsn_sever )
		dam *= 2 + (ch->level);

	if ( dt == gsn_haduken )
		dam *= 2 + (ch->level / 20);

	if ( dt == gsn_fatality )
		dam *= 2 + (ch->level);

    dam += GET_DAMROLL(ch) * UMIN(100,skill) /50;

	if (!IS_NPC(ch))
		dam = ((dam * 3)/4);

	/*Quasar, Dec 2001*/
	if (victim_ac <= 0)
	{
		victim_ac = (victim_ac * (-1));
	}
	dam = dam/(1 + ( (sqrt(victim_ac) - 10) /95 ) );

    if ( dam <= 0 )
	dam = 1;

	if (talent_bonus > 1)
	{
		if(!IS_NPC(victim))
		{
			int parry,dodge,shield,highest;
			parry = victim->pcdata->bonus_array[com_d_p];
			dodge = victim->pcdata->bonus_array[com_d_d];
			shield = victim->pcdata->bonus_array[com_d_s];

			if(parry >= dodge && parry >= shield)
				highest = parry;
			else if(dodge >= parry && dodge >= shield)
				highest = dodge;
			else if(shield >= parry && shield >= parry)
				highest = shield;
			else
				highest = 0;

			if(talent_bonus > highest)
			{
				add_talent = TRUE;
				talent_mod = highest;
			}
		}
		else
		{
			if(talent_bonus > victim->level + victim_ac/100)
			{
				add_talent = TRUE;
				talent_mod = victim_ac/100;
			}
		}
		if(add_talent)
			dam += talent_bonus - talent_mod;
	}

    if ( !check_counter( ch, victim, dam, dt ) )
	{
		if (form)
		{
			result = damage( ch, victim, dam, dt, dam_type, TRUE,TRUE );
		}
		else
		{
			result = damage( ch, victim, dam, dt, dam_type, TRUE,FALSE );
		}
	}
	else return;
    
    /* but do we have a funky weapon? */
    if (result && wield != NULL)
    { 
	int dam;

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON))
	{
	    int level;
	    AFFECT_DATA *poison, af;

	    if ((poison = affect_find(wield->affected,gsn_poison)) == NULL)
		level = wield->level;
	    else
		level = poison->level;
	
	    if (!saves_spell(level / 2,victim,DAM_POISON)) 
	    {
		send_to_char("You feel poison coursing through your veins.\n\r",
		    victim);
		act("$n is poisoned by the venom on $p.",
		    victim,wield,NULL,TO_ROOM);

    		af.where     = TO_AFFECTS;
    		af.type      = gsn_poison;
    		af.level     = level * 3/4;
    		af.duration  = level / 2;
    		af.location  = APPLY_STR;
    		af.modifier  = -1;
    		af.bitvector = AFF_POISON;
    		affect_join( victim, &af );
	    }

	    /* weaken the poison if it's temporary */
	    if (poison != NULL)
	    {
	    	poison->level = UMAX(0,poison->level - 2);
	    	poison->duration = UMAX(0,poison->duration - 1);
	
	    	if (poison->level == 0 || poison->duration == 0)
		    act("The poison on $p has worn off.",ch,wield,NULL,TO_CHAR);
	    }
 	}


    	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC))
	{
	    dam = number_range(1, wield->level / 5 + 1);
		if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
			&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
		{
	    act("$p {Md{mr{ya{yw{Ws{x life from $n.",victim,wield,NULL,TO_ROOM);
	    act("You feel $p {Md{mr{Ca{cw{Yi{yn{Wg{x your life away.",victim,wield,NULL,TO_CHAR);
		}
	    damage(ch,victim,dam,0,DAM_NEGATIVE,FALSE,FALSE);
	    ch->hit += dam/2;
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
	{
	    dam = number_range(1,wield->level / 4 + 1);
		if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
			&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
		{
	    act("$n is {Rb{Yu{Wrn{Ye{Rd{x by $p.",victim,wield,NULL,TO_ROOM);
	    act("$p {Rs{re{Ya{rr{Rs{x your flesh.",victim,wield,NULL,TO_CHAR);
		}
	    fire_effect( (void *) victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_FIRE,FALSE,FALSE);
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST))
	{
	    dam = number_range(1,wield->level / 6 + 2);
		if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
			&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
		{
	    act("$p {Bfr{ce{Ce{cz{Bes{x $n.",victim,wield,NULL,TO_ROOM);
	    act("The {Bc{Co{cl{Bd {Ct{co{Bu{Cc{ch{x of $p surrounds you with {Bi{bc{ce{x.",victim,wield,NULL,TO_CHAR);
		}
	    cold_effect(victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_COLD,FALSE,FALSE);
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING))
	{
	    dam = number_range(1,wield->level/5 + 2);
		if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
			&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
		{
	    act("$n is struck by {Yl{yig{Wht{yni{Yng{x from $p.",victim,wield,NULL,TO_ROOM);
	    act("You are shocked by $p.",victim,wield,NULL,TO_CHAR);
		}
	    shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_LIGHTNING,FALSE,FALSE);
	}
	/*Dagashi's Weapon?*/
	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_DAGASHI))
	{
	    int level;
	    AFFECT_DATA *poison, af;

		if IS_AFFECTED(victim, AFF_DAGASHI)
			return;

	    if ((poison = affect_find(wield->affected,gsn_coat)) == NULL)
		level = wield->level;
	    else
		level = poison->level;
	
		send_to_char("You body spasms wildly as the {BD{ra{Bg{ra{Bs{rh{Bi{x poison burns you{x\n\r",victim);
		act("$n is wracked with convulsions from the {BD{ra{Bg{ra{Bs{rh{Bi{x poison on $p.{x",victim,wield,NULL,TO_ROOM);

    		af.where     = TO_AFFECTS;
    		af.type      = gsn_dagashi_poison;
    		af.level     = level;
    		af.duration  = 10;
    		af.location  = APPLY_STR;
    		af.modifier  = -5;
    		af.bitvector = AFF_DAGASHI;
    		affect_join( victim, &af );
	}
	}
    tail_chain( );
    return;

}


/*
 * Inflict damage from a hit.
 */
bool damage(CHAR_DATA *ch,CHAR_DATA *victim,int dam,int dt,int dam_type,
	    bool show,bool form) 
{
    char buf[MAX_STRING_LENGTH];
	OBJ_DATA *corpse;
    bool immune;
	static char * const he_she  [] = { "it",  "he",  "she" };
	int bit = -1;

    if ( victim->position == POS_DEAD )
	return FALSE;

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 1000000 && dt >= TYPE_HIT)
    {
	bug( "Damage: %d: more than 1000000 points!", dam );
	dam = 1200;
	if (!IS_IMMORTAL(ch))
	{
	    OBJ_DATA *obj;
	    obj = get_eq_char( ch, WEAR_WIELD );
	    send_to_char("You really shouldn't cheat.\n\r",ch);
	    if (obj != NULL)
	    	extract_obj(obj);
	}

    }

	
    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return FALSE;
	check_killer( ch, victim );

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
	    {
		set_fighting( victim, ch );
		if ( IS_NPC( victim ) && HAS_TRIGGER_MOB( victim, TRIG_KILL ) )
		    p_percent_trigger( victim, NULL, NULL, ch, NULL, NULL, TRIG_KILL );
	    }
	    if (victim->timer <= 4)
	    	victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );
    }

    /*
     * Inviso attacks ... not.
     */
    if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
    {
	affect_strip( ch, gsn_invis );
	affect_strip( ch, gsn_mass_invis );
	REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }
	/*Remove Shadowslip*/
	if ( IS_AFFECTED2(ch, AFF_SHADOW_SLIP) )
    {
	affect_strip( ch, gsn_shadow_slip );
	REMOVE_BIT( ch->affected2_by, AFF_SHADOW_SLIP );
	act( "$n suddenly appears out of the shadows", ch, NULL, NULL, TO_ROOM );
    }

    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC(victim) 
    &&   victim->pcdata->condition[COND_DRUNK]  > 10 )
	dam = 9 * dam / 10;

    if ( dam > 1 && IS_AFFECTED(victim, AFF_SANCTUARY) )
	dam /= 2;

    if ( dam > 1 && ((IS_AFFECTED(victim, AFF_PROTECT_EVIL) && IS_EVIL(ch) )
    ||		     (IS_AFFECTED(victim, AFF_PROTECT_GOOD) && IS_GOOD(ch) )))
	dam -= dam / 4;

    immune = FALSE;


    /*
     * Check for parry, and dodge, and phase.
     */
    if ( dt >= TYPE_HIT && ch != victim)
    {
		if ( check_holy_armor( ch, victim ) )
	    return FALSE;
		if ( check_shield_wall(ch, victim))
		return FALSE;
		if ( check_parry( ch, victim ) )
	    return FALSE;
		if ( check_dodge( ch, victim ) )
	    return FALSE;
		if ( check_shield_block(ch,victim))
	    return FALSE;
		if ( check_phase(ch,victim))
	    return FALSE;

    }

    switch(check_immune(victim,dam_type))
    {
	case(IS_IMMUNE):
	    immune = TRUE;
	    dam = 0;
	    break;
	case(IS_RESISTANT):	
	    dam -= dam/3;
	    break;
	case(IS_VULNERABLE):
	    dam += dam/2;
	    break;
    }

	if(check_vres(victim,dam_type))
	{
		switch (dam_type)
		{
		case(DAM_BASH):			bit = victim->vres_bash;		break;
		case(DAM_PIERCE):		bit = victim->vres_pierce;		break;
		case(DAM_SLASH):		bit = victim->vres_slash;		break;
		case(DAM_FIRE):			bit = victim->vres_fire;		break;
		case(DAM_COLD):			bit = victim->vres_cold;		break;
		case(DAM_LIGHTNING):	bit = victim->vres_lightning;	break;
		case(DAM_ACID):			bit = victim->vres_acid;		break;
		case(DAM_POISON):		bit = victim->vres_poison;		break;
		case(DAM_NEGATIVE):		bit = victim->vres_negative;	break;
		case(DAM_HOLY):			bit = victim->vres_holy;		break;
		case(DAM_ENERGY):		bit = victim->vres_energy;		break;
		case(DAM_MENTAL):		bit = victim->vres_mental;		break;
		case(DAM_DISEASE):		bit = victim->vres_disease;		break;
		case(DAM_LIGHT):		bit = victim->vres_light;		break;
		case(DAM_SOUND):		bit = victim->vres_sound;		break;
		}

		dam = (dam - ((dam * bit)/100));
	}
	
	if (!IS_NPC(ch) && !IS_NPC(victim))
	{
		dam /= 2;
	}

    if (show)
	{
		if (form)
		{
    		dam_message( ch, victim, dam, dt, immune,TRUE );
		}
		else
		{
    		dam_message( ch, victim, dam, dt, immune,FALSE );
		}
	}
	if (dam < 0)
	{
		dam = 0;
	}
    
	if (dam == 0)
	{
		return FALSE;
	}

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
	if (!IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	{
		victim->hit -= dam;
	}
    if ( !IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL && victim->hit < 1 )
	{
		victim->hit = 1;
	}
    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
	break;

    case POS_DEAD:
	act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "That really did HURT!\n\r", victim );
	if ( (victim->hit < victim->max_hit / 4) && (victim->level <= 101))
	    send_to_char( "You sure are BLEEDING!\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );


	if (victim->position == POS_DEAD && !IS_NPC(victim) && !IS_NPC(ch)
		&& (IS_SET(victim->in_room->room_flags,ROOM_HONCIRC)))
	{
		ch->pcdata->akills += 1;
		victim->pcdata->adeaths += 1;

		/* the dead man */
		stop_fighting(victim,TRUE);
		char_from_room(victim);
		char_to_room(victim,get_room_index(ROOM_VNUM_ALTAR));
		send_to_char("{RYou have been defeated in the Honor Circle{x\n\r",victim);
		victim->pcdata->hc_losses ++;
		victim->hit = victim->max_hit;
		victim->mana = victim->max_mana;
		affect_strip(victim,gsn_plague);
		affect_strip(victim,gsn_poison);
		affect_strip(victim,gsn_blindness);
		affect_strip(victim,gsn_sleep);
		affect_strip(victim,gsn_curse);
		victim->move = victim->max_move;
		update_pos( victim );
		do_look(victim, "auto");

		/* the winner :) */
		stop_fighting(ch,TRUE);
		char_from_room(ch);
		char_to_room(ch,get_room_index(ROOM_VNUM_TEMPLE));
		send_to_char("{RYou have triumphed in the Honor Circle{x\n\r",ch);
		ch->pcdata->hc_wins ++;
		ch->hit = ch->max_hit;
		ch->mana = ch->max_mana;
		affect_strip(ch,gsn_plague);
		affect_strip(ch,gsn_poison);
		affect_strip(ch,gsn_blindness);
		affect_strip(ch,gsn_sleep);
		affect_strip(ch,gsn_curse);
		ch->move = ch->max_move;
		update_pos( ch );
		do_look(ch, "auto");
		sprintf(buf, "{W%s has defeated %s!",ch->name,victim->name);
		do_hctalk(ch,buf);

		if (IS_SET(ch->plr2,PLR2_CHALLENGER))
			REMOVE_BIT(ch->plr2,PLR2_CHALLENGER);
		if (IS_SET(victim->plr2,PLR2_CHALLENGER))
			REMOVE_BIT(victim->plr2,PLR2_CHALLENGER);
		if (IS_SET(victim->plr2,PLR2_CHALLENGED))
			REMOVE_BIT(victim->plr2,PLR2_CHALLENGED);
		if (IS_SET(ch->plr2,PLR2_CHALLENGED))
			REMOVE_BIT(ch->plr2,PLR2_CHALLENGED);
		REMOVE_BIT(ch->comm,COMM_NOCHANNELS);
		REMOVE_BIT(victim->comm,COMM_NOCHANNELS);
		arena_is_busy = FALSE;
		return TRUE;
	}


    /*
     * Payoff for killing things.
     */
	if ( victim->position == POS_DEAD )
	{
		group_gain( ch, victim );

		if (!IS_NPC(victim) && 
			!is_arena(victim))
		{
			sprintf( buf, ch->short_descr);
			free_string( victim->slain_by );
			victim->slain_by = str_dup( buf );
			make_stone( victim );
			sprintf( log_buf, "%s killed by %s at %d", victim->name,
				(IS_NPC(ch) ? ch->short_descr : ch->name),
				ch->in_room->vnum );
			log_string( log_buf );

			/*
			* Dying penalty:
			* 2/3 way back to previous level.
			*/
			if ( victim->exp > exp_per_level(victim,victim->pcdata->points) * victim->level )
			{
				gain_exp( victim, (2 * (exp_per_level(victim,victim->pcdata->points)* victim->level - victim->exp)/3) + 50 );
				victim->pcdata->sp = 0;
			}
				
		}
		if (!is_arena(victim) 
			&& !is_raider(victim) 
			&& !is_raided(victim)) 
		{
			sprintf( log_buf, "%s got toasted by %s at %s [room %d]",
				(IS_NPC(victim) ? victim->short_descr : victim->name),
				(IS_NPC(ch) ? ch->short_descr : ch->name),
				ch->in_room->name, ch->in_room->vnum);
			if (IS_NPC(ch) && !IS_NPC(victim))
			{
				victim->pcdata->gamestat[MOB_DEATHS]++;
				//stc("Adding one to gamestat[MOB_DEATHS]\n\r",victim);
			}
			if (!IS_NPC(ch) && !IS_NPC(victim))
			{
				ch->pcdata->pk_kills++;
				sprintf(buf, "You now have %d pk kills{x\n\r",ch->pcdata->pk_kills);
				send_to_char(buf, ch);
				ch->pcdata->gamestat[PK_KILLS]++;
				//stc("Adding one to gamestat[PK_KILLS]\n\r",ch);
				victim->pcdata->pk_deaths++;
				sprintf(buf, "You now have %d pk deaths{x\n\r",victim->pcdata->pk_deaths);
				send_to_char(buf, victim);
				victim->pcdata->gamestat[PK_DEATHS]++;
				//stc("Adding one to gamestat[PK_DEATHS]\n\r",victim);
			}
		}

		if (is_arena(victim))
		{
			REMOVE_BIT(victim->plr2, PLR2_ARENA);
			if (in_single_war(victim))
			{
				REMOVE_BIT(victim->plr2, PLR2_SINGLE_WAR);
				victim->pcdata->arena_losses ++;
			}
			stop_fighting(victim, TRUE);
			char_from_room(victim);
			char_to_room(victim,get_room_index(ROOM_VNUM_ARENA));
			victim->hit = victim->max_hit;
			victim->mana = victim->max_mana;
			victim->move = victim->max_move;
			do_stop_aff(victim, " ");
			update_pos( victim );
			do_look(victim, "auto");
			inwar--;
			sprintf(buf, "{R%s{W was killed by {G%s{W in the arena.{x", victim->name, ch->name);
			do_wartalk(ch,buf);
			sprintf(buf, "{Y%d{W players remaining.{x", inwar);
			do_wartalk(ch,buf);
			if (wartype == 2 && IS_SET(victim->plr2,PLR2_RED_TEAM))
			{
				red_num --;
				sprintf(buf,"{RRed Team: {W%d {BBlue Team: {W%d{x\n\r",red_num, blue_num);
				do_wartalk(ch,buf);
			}
			if (wartype == 2 && IS_SET(victim->plr2,PLR2_BLUE_TEAM))
			{
				blue_num --;
				sprintf(buf,"{RRed Team: {W%d {BBlue Team: {W%d{x\n\r",red_num, blue_num);
				do_wartalk(ch,buf);
			}
			if (inwar == 1 && wartype == 1)
			{
				sprintf(buf, "{W{G%s {Wis victorious in the arena!{x", ch->name);
				do_wartalk(ch,buf);
				war_prize(ch);
				wartype = 0;
				min_level = 0;           
				max_level = 0;
				iswar = FALSE;
				inwar = 0;
				wartimer = 0;
				time_left = 0;
				warprize = 0;
				ch->hit = ch->max_hit;
				ch->mana = ch->max_mana;
				ch->move = ch->max_move;
				REMOVE_BIT(ch->plr2, PLR2_ARENA);
				REMOVE_BIT(ch->plr2, PLR2_SINGLE_WAR);
				REMOVE_BIT(victim->plr2, PLR2_ARENA);
				REMOVE_BIT(victim->plr2, PLR2_SINGLE_WAR);
				char_from_room(ch);
				char_to_room(ch, get_room_index(ROOM_VNUM_ARENA));
				do_look(ch, "auto");
				char_from_room(victim);
				char_to_room(victim, get_room_index(ROOM_VNUM_ARENA));
				do_look(victim, "auto");
			}
			if (wartype == 2 && (red_num == 0 || blue_num == 0))
			{
				team_win(ch,victim);
			}
			return	TRUE;
		}
		if (is_raider(victim) && (raid_info.raiding))
		{
			sprintf(buf,"%s{W was killed in the raid, %s is a disgrace to the %s{W!",
				victim->short_descr,he_she  [URANGE(0,victim->sex,2)],clan_table[ch->clan].who_name);
			raid_channel(buf);
			REMOVE_BIT(victim->plr2,PLR2_RAIDER);
			raid_info.raiders--;
			if (raid_info.raiders <= 0)
			{
				sprintf(buf,"{WAll of the %s{W raiders have been killed, the %s are victorious!",
					clan_table[raid_info.clan_raider].who_name,
					clan_table[raid_info.clan_defender].who_name);
				defenders_win( );
			}
		}
		if (is_raided(victim) && (raid_info.raiding))
		{
			sprintf(buf,"%s{W was killed defending the %s{W's hall from the mauraders!",
				victim->short_descr,clan_table[ch->clan].who_name);
			raid_channel(buf);
			REMOVE_BIT(victim->plr2,PLR2_RAIDED);
			raid_info.defenders--;
		}


		if (IS_NPC(victim))
			wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
		else
			wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);

		/*
		* Death trigger
		*/
		if ( IS_NPC( victim ) && HAS_TRIGGER_MOB( victim, TRIG_DEATH) )
		{
			victim->position = POS_STANDING;
			p_percent_trigger( victim, NULL, NULL, ch, NULL, NULL, TRIG_DEATH );
		}
		if (!IS_NPC(victim))
		{
			if (IS_NPC(ch))
			{
				kill_table[URANGE(0, ch->level, MAX_LEVEL - 1)].kills++;
				ch->pIndexData->kills++;
				victim->pcdata->gamestat[MOB_DEATHS]++;
			}
			else
			{
				victim->pcdata->gamestat[PK_DEATHS]++;
				ch->pcdata->gamestat[PK_KILLS]++;
			}
		}
		if (IS_NPC(victim))
		{
			if (!IS_NPC(ch))
			{
				ch->pcdata->gamestat[MOB_KILLS]++;
				//stc("Adding one to gamestat[MOB_KILLS]\n\r",ch);
			}
			else
			{
				kill_table[URANGE(0, ch->level, MAX_LEVEL - 1)].kills++;
				ch->pIndexData->kills++;
			}
		}
		raw_kill( ch,victim );
		/* dump the flags */
		if (ch != victim && !IS_NPC(ch) && !is_same_clan(ch,victim) && !IS_SET(victim->plr2, PLR2_ARENA))
		{
			REMOVE_BIT(victim->act,PLR_THIEF);
		}

		/* RT new auto commands */

		if (!IS_NPC(ch)
			&&  (corpse = get_obj_list(ch,"corpse",ch->in_room->contents)) != NULL
			&&  corpse->item_type == ITEM_CORPSE_NPC && can_see_obj(ch,corpse))
		{
			OBJ_DATA *coins;

			corpse = get_obj_list( ch, "corpse", ch->in_room->contents ); 

			if ( IS_SET(ch->act, PLR_AUTOLOOT)
				&& corpse && corpse->contains) /* exists and not empty */
			{
				do_function(ch, &do_get, "all corpse");
			}

			if (IS_SET(ch->act,PLR_AUTOGOLD) &&
				corpse && corpse->contains  && /* exists and not empty */
				!IS_SET(ch->act,PLR_AUTOLOOT))
			{
				if ((coins = get_obj_list(ch,"gcash",corpse->contains))
					!= NULL)
				{
					do_function(ch, &do_get, "all.gcash corpse");
				}
			}

			if (IS_SET(ch->act, PLR_AUTOSAC))
			{
				if (IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains)
				{
					return TRUE;  /* leave if corpse has treasure */
				}
				else
				{
					do_function(ch, &do_sacrifice, "corpse");
				}
			}
		}

		return TRUE;
	}

	if ( victim == ch )
		return TRUE;

	/*
	* Take care of link dead people.
	*/
	if ( !IS_NPC(victim) && victim->desc == NULL )
	{
		if ( number_range( 0, victim->wait ) == 0 )
		{
			do_function(victim, &do_recall, "" );
			return TRUE;
		}
	}

	/*
	* Wimp out?
	*/
	if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
	{
		if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0
			&&   victim->hit < victim->max_hit / 5) 
			||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
			&&     victim->master->in_room != victim->in_room ) )
		{
			do_function(victim, &do_flee, "" );
		}
	}

	if ( !IS_NPC(victim)
		&&   victim->hit > 0
		&&   victim->hit <= victim->wimpy
		&&   victim->wait < PULSE_VIOLENCE / 2 )
	{
		do_function (victim, &do_flee, "" );
	}

	tail_chain( );
	return TRUE;
}

bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim)
{
	int trainer,healer,changer;
	if(ch != victim)
	{
		debug(ch,"is_safe(ch<-,victim)\n\r");
		/*debug(victim,"is_safe(ch,victim<-)\n\r");*/
	}
	else
		debug(ch,"is_safe(ch<-,ch<-)\n\r");

	if (victim->in_room == NULL || ch->in_room == NULL)
	{
		debug(ch,"in_room == NULL\n\r");
		/*debug(victim,"in_room == NULL\n\r");*/
		return TRUE;
	}

	if (victim->fighting == ch || victim == ch)
	{
		debug(ch,"victim is fighting ch || victim is ch\n\r");
		return FALSE;
	}

	if (IS_IMMORTAL(ch))
	{
		debug(ch,"ch is immortal(always allowed to kill people)\n\r");
		return FALSE;
	}

	if(IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
	{
		debug(ch,"victim is immortal, ch is NOT\n\r");
		return TRUE;
	}

	/* killing mobiles */	if (IS_NPC(victim))
	{
		/* safe room? */
		if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
		{
			send_to_char("Not in this room.\n\r",ch);
			debug(ch,"victim in ROOM_SAFE\n\r");
			/* debug(victim,"victim in ROOM_SAFE\n\r"); */
			return TRUE;
		}
		if (victim->spec_fun == spec_lookup( "spec_questmaster" ))
		{
			ptc(ch,"%s {xwouldnt like that.\n\r",victim->short_descr);
			debug(ch,"victim is a questmaster\n\r");
			/* debug(victim,"victim is a questmaster\n\r"); */
			return TRUE;
		}
		if (victim->pIndexData->pShop != NULL)
		{
			send_to_char("The shopkeeper wouldn't like that.\n\r",ch);
			debug(ch,"victim is a shopkeeper\n\r");
			/* debug(victim,"victim is a shopkeeper\n\r"); */
			return TRUE;
		}
		trainer = IS_SET(victim->act,ACT_TRAIN);
		healer = IS_SET(victim->act,ACT_IS_HEALER);
		changer = IS_SET(victim->act,ACT_IS_CHANGER);
		debug(ch,"{t,h,c}={%d,%d,%d}\n\r",trainer,healer,changer);
		/* debug(victim,"{t,h,c}={%d,%d,%d}\n\r",trainer,healer,changer); */
		/* no killing healers, trainers, etc */
		if (IS_SET(victim->act,ACT_TRAIN)
			||  IS_SET(victim->act,ACT_IS_HEALER)
			||  IS_SET(victim->act,ACT_IS_CHANGER))
		{
			send_to_char("I don't think {RE{rn{wz{Do{x would approve.\n\r",ch);
			debug(ch,"victim is trainer, healer, or changer\n\r");
			/* debug(victim,"victim is trainer, healer, or changer\n\r"); */
			return TRUE;
		}

		if (!IS_NPC(ch))
		{
			/* no pets (disabled for now)*/
			if (IS_SET(victim->act,ACT_PET))
			{
				act("But $N looks so cute and cuddly...",
					ch,NULL,victim,TO_CHAR);
				debug(ch,"victim is pet\n\r");
				/* debug(victim,"victim is pet\n\r"); */
				return TRUE;
			}
			/* no charmed creatures unless owner */
			if (IS_AFFECTED(victim,AFF_CHARM) && ch != victim->master)
			{
				send_to_char("You don't own that monster.\n\r",ch);
				debug(ch,"victim charmed, ch NOT master\n\r");
				/* debug(victim,"victim charmed, ch NOT master\n\r"); */
				return TRUE;
			}
		}
	}
	else
	{
		/* NPC doing the killing */
		if (IS_NPC(ch))
		{
			/* safe room check */
			if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
			{
				debug(ch,"in_room == NULL\n\r");
				/* debug(victim,"in_room == NULL\n\r"); */
				return TRUE;
			}

			/* charmed mobs and pets cannot attack players while owned */
			if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
				&&  ch->master->fighting != victim)
			{
				send_to_char("Players are your friends!\n\r",ch);
				debug(ch,"ch charmed, master exists, and is NOT fighting victim\n\r");
				/* debug(victim,"ch charmed, master exists, and is NOT fighting victim\n\r"); */
				return TRUE;
			}
		}
		/* player doing the killing */
		else
		{
			if (!can_see(ch,victim))
			{
				debug(ch,"ch cannot see victim");
				/* debug(victim,"ch cannot see victim"); */
				return TRUE;
			}
			if (is_same_team(ch,victim))
			{
				debug(ch,"on the same team\n\r");
				/* debug(victim,"on the same team\n\r"); */
				return TRUE;
			}
			if (is_arena(victim) && is_arena(ch))
			{
				debug(ch,"is_arena(victim && ch)\n\r");
				return FALSE;
			}
			if (is_honor(victim) && is_honor(ch))
			{
				debug(ch,"is_honor(victim && ch)\n\r");
				return FALSE;
			}
			if (is_raider(ch) && is_raided(victim))
			{
				debug(ch,"is_raider(ch) && is_raided(victim)\n\r");
				return FALSE;
			}
			if (victim->in_room->vnum == ROOM_VNUM_ARENA)
			{
				debug(ch,"victim in war prep room");
				/* debug(victim,"victim in war prep room"); */
				return TRUE;
			}

			if (ch->in_room->vnum == ROOM_VNUM_ARENA)
			{
				debug(ch,"ch in war prep room");
				/* debug(victim,"ch in war prep room"); */
				return TRUE;
			}

			if (IS_SET(victim->act,PLR_THIEF))
			{
				debug(ch,"victim is a thief (always allowed to kill)\n\r");
				return FALSE;
			}

			if (IS_SET(victim->act,PLR_KILLER) && IS_SET(ch->act,PLR_KILLER))
			{
				debug(ch,"victim is PLR_KILLER, ch is PLR_KILLER\n\r");
				return FALSE;
			}

			if (!IS_SET(ch->act,PLR_PROKILLER))
			{
				send_to_char("You must be {RPK{x if you want to kill players.\n\r",ch);
				debug(ch,"ch is not a PROKILLER\n\r");
				/* debug(victim,"ch is not a PROKILLER\n\r"); */
				return TRUE;
			}

			if (IS_SET(ch->act,PLR_PROKILLER)
				&& !IS_SET(victim->act, PLR_PROKILLER))
			{
				send_to_char("You can only attack other {RPK{x people.\n\r",ch);
				debug(ch,"ch is PROKILLER, victim is NOT\n\r");
				/* debug(victim,"ch is PROKILLER, victim is NOT\n\r"); */
				return TRUE;
			}
		}
	}
	debug(ch,"default, victim is NOT safe\n\r");
	/* debug(victim,"default, victim is NOT safe\n\r"); */
	return FALSE;
}

bool is_safe_spell(CHAR_DATA *ch, CHAR_DATA *victim, bool area )
{
    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim == ch && area)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if (IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL && !area)
	return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {
	/* safe room? */
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	    return TRUE;

	if (victim->pIndexData->pShop != NULL)
	    return TRUE;

	/* no killing healers, trainers, etc */
	if (IS_SET(victim->act,ACT_TRAIN)
	||  IS_SET(victim->act,ACT_PRACTICE)
	||  IS_SET(victim->act,ACT_IS_HEALER)
	||  IS_SET(victim->act,ACT_IS_CHANGER))
	    return TRUE;

	if (!IS_NPC(ch))
	{
	    /* no pets */
	    if (IS_SET(victim->act,ACT_PET))
	   	return TRUE;

	    /* no charmed creatures unless owner */
	    if (IS_AFFECTED(victim,AFF_CHARM) && (area || ch != victim->master))
		return TRUE;

	    /* legal kill? -- cannot hit mob fighting non-group member */
	    if (victim->fighting != NULL && !is_same_group(ch,victim->fighting))
		return TRUE;
	}
	else
	{
	    /* area effect spells do not hit other mobs */
	    if (area && !is_same_group(victim,ch->fighting))
		return TRUE;
	}
    }
    /* killing players */
    else
    {
	if (area && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL)
	    return TRUE;

	/* NPC doing the killing */
	if (IS_NPC(ch))
	{
	    /* charmed mobs and pets cannot attack players while owned */
	    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
	    &&  ch->master->fighting != victim)
		return TRUE;
	
	    /* safe room? */
	    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
		return TRUE;

	    /* legal kill? -- mobs only hit players grouped with opponent*/
	    if (ch->fighting != NULL && !is_same_group(ch->fighting,victim))
		return TRUE;
	}

	/* player doing the killing */
	else
	{
	    if (is_arena(victim))
		return FALSE;

	    if (IS_SET(ch->in_room->room_flags,ROOM_HONCIRC)
		&& IS_SET(victim->in_room->room_flags,ROOM_HONCIRC))
		{
	    return FALSE;
		}

		if (!IS_SET(ch->act,PLR_PROKILLER))
		return TRUE;
            
		if (IS_SET(victim->act,PLR_PROKILLER))
		return FALSE;
        
		if (!IS_SET(victim->act,PLR_PROKILLER))
		return TRUE;

		if (!is_clan(ch))
		return TRUE;

	    if (IS_SET(victim->act,PLR_KILLER) || IS_SET(victim->act,PLR_THIEF))
		return FALSE;

	    if (!is_clan(victim))
		return TRUE;

	    if (ch->level > victim->level + 8)
		return TRUE;
	}

    }
    return FALSE;
}
/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    /*
     * Follow charm thread to responsible character.
     * Attacking someone's charmed char is hostile!
     */
    while ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	victim = victim->master;

    /*
     * NPC's are fair game.
     * So are killers and thieves.
     */
    if ( IS_NPC(victim)
    ||   IS_SET(victim->act, PLR_KILLER)
    ||   IS_SET(victim->act, PLR_THIEF))
	return;
	
	if (is_arena(victim))
	return;
    
	if (IS_SET(victim->in_room->room_flags, ROOM_HONCIRC))
	return;

	act("CHECK_KILLER",NULL,NULL,NULL,TO_ROOM);

	/*
     * Charm-o-rama.
     */
/*    if ( IS_SET(ch->affected_by, AFF_CHARM) )
    {
		if ( ch->master == NULL )
		{
			char buf[MAX_STRING_LENGTH];

			sprintf( buf, "Check_killer: %s bad AFF_CHARM",
			IS_NPC(ch) ? ch->short_descr : ch->name );
			bug( buf, 0 );
			affect_strip( ch, gsn_charm_person );
			REMOVE_BIT( ch->affected_by, AFF_CHARM );
			return;
		}
	stop_follower( ch );
	return;
    }*/

    /*
     * NPC's are cool of course (as long as not charmed).
     * Hitting yourself is cool too (bleeding).
     * So is being immortal (Alander's idea).
     * And current killers stay as they are.
     */
    if ( IS_NPC(ch)
    ||   ch == victim
    ||   ch->level >= LEVEL_IMMORTAL
    ||   !is_clan(ch)
    ||   IS_SET(ch->act, PLR_KILLER) 
    ||	 ch->fighting  == victim)
	return;

    sprintf(buf,"$N is attempting to murder %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
    save_char_obj( ch );
    return;
}



/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance,weapon=0,parry=0;

    if ( !IS_AWAKE(victim) )
	return FALSE;

	chance = get_skill(victim,gsn_parry) / 3;
	debug(victim,"check_parry: %s: chance: %d",victim->short_descr,chance);
	chance += ( victim->level - ch->level ) / 2;
	debug(victim," %d",chance);
	chance -= ch->hitroll / 20;
	debug(victim," %d",chance);
	chance += 2 * (get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX));
	debug(victim," %d",chance);
	chance += get_weapon_skill(victim,get_weapon_sn(victim)) -
		get_weapon_skill(ch,get_weapon_sn(ch));
	debug(victim," %d",chance);
	chance += (get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR) );
	debug(victim," %d",chance);
	chance = chance/4;
	debug(victim," %d\n\r",chance);
	if ( get_eq_char( victim, WEAR_WIELD ) == NULL )
	{
		if (IS_NPC(victim))
			chance /= 2;
		else
			return FALSE;
	}
	if(!IS_NPC(victim))
	{
		parry = victim->pcdata->bonus_array[com_d_p];
		if(IS_NPC(ch))
		{
			chance += parry;
			chance -= ch->hitroll/10;
		}
		if(!IS_NPC(ch))
		{
			weapon = ch->pcdata->bonus_array[get_weapon_sn(ch)];
			chance -= weapon - parry;
		}
	}
    if (!can_see(ch,victim))
		chance /= 2;

	if( parry <= 0 )
		parry = 1;

    if ( number_range(1,weapon+parry) >= chance)
		return FALSE;

	if (check_precision(ch,victim,gsn_parry))
		return FALSE;

	if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
		&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
	{
		act( "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
		act( "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
	}
    check_improve(victim,gsn_parry,TRUE,6);
    return TRUE;
}

/*
	Check for counter
*/

bool check_counter( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt)
{
	int chance = -1,weapon = -1,counter = -1;
	int dam_type;
	OBJ_DATA *wield;

	if (    ( get_eq_char(victim, WEAR_WIELD) == NULL ) ||
		( !IS_AWAKE(victim) ) ||
		( !can_see(victim,ch) ) ||
		( get_skill(victim,gsn_counter) < 1 )
		)
		return FALSE;

	wield = get_eq_char(victim,WEAR_WIELD);

	chance = get_skill(victim,gsn_counter) / 4;
	chance += ( victim->level - ch->level ) / 2;
	chance += 2 * (get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX));
	chance += get_weapon_skill(victim,get_weapon_sn(victim)) -
		get_weapon_skill(ch,get_weapon_sn(ch));
	chance += (get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR) );

	chance = chance/5;

	if(!IS_NPC(victim))
	{
		counter = victim->pcdata->bonus_array[com_d_c];
		if(IS_NPC(ch))
		{
			chance += counter;
			chance -= ch->hitroll/10;
		}
		if(!IS_NPC(ch))
		{
			weapon = ch->pcdata->bonus_array[get_weapon_sn(ch)];
			chance -= weapon - counter;
		}
	}
	if ( number_range(1,weapon+counter) >= chance)
		return FALSE;

	dt = gsn_counter;

	if ( dt == TYPE_UNDEFINED )
	{
		dt = TYPE_HIT;
		if ( wield != NULL && wield->item_type == ITEM_WEAPON )
			dt += wield->value[3];
		else 
			dt += ch->dam_type;
	}

	if (dt < TYPE_HIT)
		if (wield != NULL)
			dam_type = attack_table[wield->value[3]].damage;
		else
			dam_type = attack_table[ch->dam_type].damage;
	else
		dam_type = attack_table[dt - TYPE_HIT].damage;

	if (dam_type == -1)
		dam_type = DAM_BASH;

	if (check_precision(ch,victim,gsn_counter))
		return FALSE;

	if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
		&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
	{
		act( "You counter $n's attack!", ch, NULL, victim, TO_VICT    );
		act( "$N counters your attack!", ch, NULL, victim, TO_CHAR    );
	}

	damage(victim,ch,dam/2, gsn_counter , dam_type ,TRUE,FALSE ); /* DAM MSG NUMBER!! */

	check_improve(victim,gsn_counter,TRUE,6);

	return TRUE;
}



/*
 * Check for shield block.
 */
bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance = -1,weapon = -1,shield = -1;

    if ( !IS_AWAKE(victim) )
        return FALSE;


    chance = get_skill(victim,gsn_shield_block) / 5;

	if(!IS_NPC(victim))
	{
		shield = victim->pcdata->bonus_array[com_d_s];
		if(IS_NPC(ch))
		{
			chance += shield;
			chance -= ch->hitroll/10;
		}
		if(!IS_NPC(ch))
		{
			weapon = ch->pcdata->bonus_array[get_weapon_sn(ch)];
			chance -= weapon - shield;
		}
	}

	if ( get_eq_char( victim, WEAR_SHIELD ) == NULL )
		return FALSE;

	if ( number_range(1,weapon+shield) >= chance)
		return FALSE;

	if (check_precision(ch,victim,gsn_shield_block))
		return FALSE;

	if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
		&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
	{
		act( "You block $n's attack with your shield.",  ch, NULL, victim,TO_VICT    );
		act( "$N blocks your attack with a shield.", ch, NULL, victim,TO_CHAR    );
	}
	check_improve(victim,gsn_shield_block,TRUE,6);
	return TRUE;
}


/*
* Check for dodge.
*/
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int chance = -1, weapon = -1, dodge = -1;

	if ( !IS_AWAKE(victim) )
		return FALSE;

	chance = get_skill(victim,gsn_dodge) / 3;
	chance += ( victim->level - ch->level ) / 2;
	chance += 2 * (get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX));

	chance -= ch->hitroll / 20;
	chance = chance/4;

	if (!can_see(victim,ch))
		chance /= 2;

	if(!IS_NPC(victim))
	{
		dodge = victim->pcdata->bonus_array[com_d_d];
		if(IS_NPC(ch))
		{
			weapon = ch->hitroll/10;
			chance += dodge - weapon;
		}
		if(!IS_NPC(ch))
		{
			weapon = ch->pcdata->bonus_array[get_weapon_sn(ch)];
			chance -= weapon - dodge;
		}
	}

	if ( number_range(1,(weapon+dodge)) >= chance)
		return FALSE;

	if (check_precision(ch,victim,gsn_dodge))
		return FALSE;

	if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
		&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
	{
		act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
		act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );
	}
	check_improve(victim,gsn_dodge,TRUE,6);
	return TRUE;
}

bool check_shield_wall( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int chance = -1,weapon = -1,shield = -1;

	if ( !IS_AWAKE(victim) )
		return FALSE;

	if ( !is_affected(victim,gsn_shield_wall) )
	{
		return FALSE;
	}

	chance = get_skill(victim,gsn_shield_wall) / 2;

	if(!IS_NPC(victim))
	{
		shield = victim->pcdata->bonus_array[sn_talent(gsn_shield_wall)];
		if(IS_NPC(ch))
		{
			weapon = ch->hitroll/10;
			chance += shield - weapon;
		}
		if(!IS_NPC(ch))
		{
			weapon = ch->pcdata->bonus_array[get_weapon_sn(ch)];
			chance -= weapon - shield;
		}
	}

	if ( number_range(1,weapon+shield) >= chance)
		return FALSE;

	if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
		&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
	{
		act( "You shelter from $n's attack behind your shield", ch, NULL, victim, TO_VICT    );
		act( "$N blocks your attack by sheltering behind their shield", ch, NULL, victim, TO_CHAR    );
	}
	check_improve(victim,gsn_dodge,TRUE,6);
	return TRUE;
}

bool check_phase( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance,weapon,phase;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    chance = get_skill(victim,gsn_phase) / 10;

	if(!IS_NPC(victim))
	{
		phase = victim->pcdata->bonus_array[sn_talent(gsn_phase)];
		if(IS_NPC(ch))
		{
			weapon = ch->hitroll/10;
			chance += phase - weapon;
		}
		if(!IS_NPC(ch))
		{
			weapon = ch->pcdata->bonus_array[get_weapon_sn(ch)];
			chance -= weapon - phase;
		}
	}

    if (!can_see(victim,ch))
	chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

	if (check_precision(ch,victim,gsn_phase))
		return FALSE;

	if	((!IS_SET(ch->comm,COMM_SHORT_FIGHTS))
		&& !IS_SET(victim->comm,COMM_SHORT_FIGHTS))
	{
    act( "Your body phases to avoid $n's attack.", ch, NULL, victim, TO_VICT    );
    act( "$N's body phases to avoid your attack.", ch, NULL, victim, TO_CHAR    );
	}
    check_improve(victim,gsn_phase,TRUE,6);
    return TRUE;
}

/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC(victim) && victim->hit < 1 )
    {
	victim->position = POS_DEAD;
	return;
    }

    if ( victim->hit <= -11 )
    {
	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fighting != NULL )
    {
	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= IS_NPC(fch) ? fch->default_pos : POS_STANDING;
	    update_pos( fch );
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    if ( IS_NPC(ch) )
    {
	name		= ch->short_descr;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
	corpse->timer	= number_range( 3, 6 );
	if ( ch->gold > 0 )
	{
	    obj_to_obj( create_money( ch->gold, ch->silver ), corpse );
	    ch->gold = 0;
	    ch->silver = 0;
	}
	corpse->cost = 0;
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->timer	= number_range( 25, 40 );
	REMOVE_BIT(ch->act,PLR_CANLOOT);
	if (!is_clan(ch))
	    corpse->owner = str_dup(ch->name);
	else
	{
	    corpse->owner = NULL;
	    if (ch->gold > 1 || ch->silver > 1)
	    {
		obj_to_obj(create_money(ch->gold / 2, ch->silver/2), corpse);
		ch->gold -= ch->gold/2;
		ch->silver -= ch->silver/2;
	    }
	}
		
	corpse->cost = 0;
    }

    corpse->level = ch->level;

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	bool floating = FALSE;

	obj_next = obj->next_content;
	if (obj->wear_loc == WEAR_FLOAT)
	    floating = TRUE;
	obj_from_char( obj );
	if (obj->item_type == ITEM_POTION)
	    obj->timer = number_range(500,1000);
	if (obj->item_type == ITEM_SCROLL)
	    obj->timer = number_range(1000,2500);
	if ( obj->pIndexData->vnum == 3034 )
	{
		obj_to_room(obj,ch->in_room);
		sprintf(buf,"obj %d dropped in room %d",obj->pIndexData->vnum,obj->in_room->vnum);
		send_to_char(buf,ch);
	}	
	if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH) && !floating)
	{
	    obj->timer = number_range(5,10);
	    REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
	}
	REMOVE_BIT(obj->extra_flags,ITEM_VIS_DEATH);


	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    extract_obj( obj );
	else if (floating)
	{
	    if (IS_OBJ_STAT(obj,ITEM_ROT_DEATH)) /* get rid of it! */
	    { 
		if (obj->contains != NULL)
		{
		    OBJ_DATA *in, *in_next;

		    act("$p evaporates,scattering its contents.",
			ch,obj,NULL,TO_ROOM);
		    for (in = obj->contains; in != NULL; in = in_next)
		    {
			in_next = in->next_content;
			obj_from_obj(in);
			obj_to_room(in,ch->in_room);
		    }
		 }
		 else
		    act("$p evaporates.",
			ch,obj,NULL,TO_ROOM);
		 extract_obj(obj);
	    }
	    else
	    {
		act("$p falls to the floor.",ch,obj,NULL,TO_ROOM);
		obj_to_room(obj,ch->in_room);
	    }
	}
	else
	    obj_to_obj( obj, corpse );
    }

    obj_to_room( corpse, ch->in_room );
    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    msg = "You hear $n's death cry.";

    switch ( number_bits(4))
    {
    case  0: msg  = "$n hits the ground ... DEAD.";			break;
    case  1: 
	if (ch->material == 0)
	{
	    msg  = "$n splatters blood on your armor.";		
	    break;
	}
    case  2: 							
	if (IS_SET(ch->parts,PART_GUTS))
	{
	    msg = "$n spills $s guts all over the floor.";
	    vnum = OBJ_VNUM_GUTS;
	}
	break;
    case  3: 
	if (IS_SET(ch->parts,PART_HEAD))
	{
	    msg  = "$n's severed head plops on the ground.";
	    vnum = OBJ_VNUM_SEVERED_HEAD;				
	}
	break;
    case  4: 
	if (IS_SET(ch->parts,PART_HEART))
	{
	    msg  = "$n's heart is torn from $s chest.";
	    vnum = OBJ_VNUM_TORN_HEART;				
	}
	break;
    case  5: 
	if (IS_SET(ch->parts,PART_ARMS))
	{
	    msg  = "$n's arm is sliced from $s dead body.";
	    vnum = OBJ_VNUM_SLICED_ARM;				
	}
	break;
    case  6: 
	if (IS_SET(ch->parts,PART_LEGS))
	{
	    msg  = "$n's leg is sliced from $s dead body.";
	    vnum = OBJ_VNUM_SLICED_LEG;				
	}
	break;
    case 7:
	if (IS_SET(ch->parts,PART_BRAINS))
	{
	    msg = "$n's head is shattered, and $s brains splash all over you.";
	    vnum = OBJ_VNUM_BRAINS;
	}
    }

    act( msg, ch, NULL, NULL, TO_ROOM );

    if ( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), 0 );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

	if (obj->item_type == ITEM_FOOD)
	{
	    if (IS_SET(ch->form,FORM_POISON))
		obj->value[3] = 1;
	    else if (!IS_SET(ch->form,FORM_EDIBLE))
		obj->item_type = ITEM_TRASH;
	}

	obj_to_room( obj, ch->in_room );
    }

    if ( IS_NPC(ch) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->u1.to_room != NULL
	&&   pexit->u1.to_room != was_in_room )
	{
	    ch->in_room = pexit->u1.to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;
    return;
}



void raw_kill( CHAR_DATA *killer, CHAR_DATA *victim )
{
	OBJ_DATA *obj;
    int i;
	//bool had_locker = FALSE;
	OBJ_DATA *bag;
	OBJ_DATA *bag_next;

    stop_fighting( victim, TRUE );
	if (drop_golem(victim,victim->in_room))
	{
		if (IS_SET(killer->act,PLR_AUTOGOLEM))
		{
			do_get(killer,"golem");
			/*check for golem bag*/
			for ( bag = killer->carrying; bag != NULL; bag = bag_next )
			{
				bag_next = bag->next_content;
				if (bag->item_type == ITEM_GOLEM_BAG)
				{
					do_put(killer,"golem bag");
					break;
				}
		   }
		}
	}
    death_cry( victim );
    if (!is_arena(victim) 
		&& (victim->thermo_death != TRUE))
	make_corpse( victim );
	affect_strip( victim, gsn_shadow_slip );
	REMOVE_BIT	 ( victim->affected2_by, AFF_SHADOW_SLIP);

   /* GM, transfer the corpse to the clan morgue... */
   if ( !IS_NPC(victim) && (is_clan(victim) 
	   && clan_table[victim->clan].room[1] != ROOM_VNUM_ALTAR 
	   && clan_table[victim->clan].room[1] > 0) )
   {
		OBJ_DATA *corpse;
		ROOM_INDEX_DATA *morgue;

		corpse = get_obj_list(victim, "corpse", victim->in_room->contents );
		morgue = get_room_index(clan_table[victim->clan].room[1]);
    
		/* Bug fix provided by Mat Reda.  Thanks Matt! */
		if (corpse != NULL && morgue != NULL)
		{
			obj_from_room(corpse);
			obj_to_room(corpse, morgue);
			if (victim->alignment > 0)
				act("The Creator protects this one...",victim,NULL,NULL,TO_ROOM);
			else
				act("The Lord of the Grave claims his own!",victim,NULL,NULL,TO_ROOM);
		}
	}


/*
 *  Joe:Lockers	Ok locker should have been dropped in room. 
 *	Recover locker before char sent to death room
 */
	for ( obj = victim->in_room->contents; obj; obj = obj->next_content )
	{
		if ( obj->pIndexData->vnum == 3034 )
		{
			stc("locker found\n\r",victim);
			/*obj_from_room( obj );Get locker from room*/
		}
	}

	if (!IS_NPC (victim))
	{
		if (IS_NPC (killer))  /* CHAR_DATA *killer, added to raw_kill()  */
		{
			kill_table[URANGE (0, killer->level, MAX_LEVEL - 1)].kills++;
			killer->pIndexData->kills++;
		}
	}
	else
	{
		victim->pIndexData->killed++;
		kill_table[URANGE (0, victim->level, MAX_LEVEL - 1)].killed++;
		extract_char (victim, TRUE);
		return;
	}
	
	if (!is_arena(victim))
    extract_char( victim, FALSE );
    else
	{
	char_from_room(victim);
	char_to_room(victim, (get_room_index(ROOM_VNUM_TEMPLE)));
	do_look(victim, "auto");
	}
	while ( victim->affected )
	affect_remove( victim, victim->affected );
    victim->affected_by	= race_table[victim->race].aff;
    for (i = 0; i < 4; i++)
    	victim->armor[i]= 100;
    victim->position	= POS_RESTING;
    victim->hit		= UMAX( 1, victim->hit  );
    victim->mana	= UMAX( 1, victim->mana );
    victim->move	= UMAX( 1, victim->move );
	affect_strip(victim,gsn_fortify);
	REMOVE_BIT(victim->affected2_by, AFF_FORTIFY);
/*Joe:Lockers  Now char is in death room etc place locker back on him*/
	if (obj->pIndexData->vnum==3034)
		/*obj_to_char( obj, victim);*/
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( victim == ch )
	return;
    
    members = 0;
    group_levels = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
        {
	    members++;
	    group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
	}
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
	group_levels = ch->level ;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !is_same_group( gch, ch ) || IS_NPC(gch))
	    continue;

/*	Taken out, add it back if you want it
	if ( gch->level - lch->level >= 5 )
	{
	    send_to_char( "You are too high for this group.\n\r", gch );
	    continue;
	}

	if ( gch->level - lch->level <= -5 )
	{
	    send_to_char( "You are too low for this group.\n\r", gch );
	    continue;
	}
*/

	if ((gch->level != 101) && (victim->level <= 101))
	{
		xp = xp_compute( gch, victim, group_levels );  
		sprintf( buf, "You receive %d experience points.\n\r", xp );
		send_to_char( buf, gch );
		gain_exp( gch, xp );
		gain_sp(gch,victim);
	}
	else if ((gch->level == 101) && (victim->level >= 101))
	{
		add_hero_exp( gch, victim, group_levels );
	}

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	    {
		act( "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
		act( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
	    }
	}
    }
    if (IS_SET(ch->act, PLR_QUESTOR)&&IS_NPC(victim))
        {
            if (ch->questmob == victim->pIndexData->vnum)
            {
		send_to_char("You have almost completed your QUEST!\n\r",ch);
                send_to_char("Return to the questmaster before your time runs out!\n\r",ch);
                ch->questmob = -1;
            }
        }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim, int total_levels )
{
    int xp,base_exp;
    int level_range;

    level_range = victim->level - gch->level;
	
	/* compute the base exp */
	base_exp = (16 * level_range) + 160;

	if (level_range <= -10)
		base_exp = 0;
	if (level_range >= 14)
		base_exp = 400;

/*	else if (gch->alignment > 500)
    {
		if (victim->alignment < -750)
			xp = (base_exp *4)/3;
 		else if (victim->alignment < -500)
			xp = (base_exp * 5)/4;
		else if (victim->alignment > 750)
			xp = base_exp / 4;
   		else if (victim->alignment > 500)
			xp = base_exp / 2;
		else if (victim->alignment > 250)
			xp = (base_exp * 3)/4; 
		else
			xp = base_exp;
    }

    else if (gch->alignment < -500)
    {
		if (victim->alignment > 750)
			xp = (base_exp * 5)/4;
  		else if (victim->alignment > 500)
			xp = (base_exp * 11)/10; 
   		else if (victim->alignment < -750)
			xp = base_exp/2;
		else if (victim->alignment < -500)
			xp = (base_exp * 3)/4;
		else if (victim->alignment < -250)
			xp = (base_exp * 9)/10;
		else
			xp = base_exp;
    }

    else if (gch->alignment > 200)
    {
		if (victim->alignment < -500)
			xp = (base_exp * 6)/5;
 		else if (victim->alignment > 750)
			xp = base_exp/2;
		else if (victim->alignment > 0)
			xp = (base_exp * 3)/4; 
		else
			xp = base_exp;
    }

    else if (gch->alignment < -200)
    {
		if (victim->alignment > 500)
			xp = (base_exp * 6)/5;
		else if (victim->alignment < -750)
			xp = base_exp/2;
		else if (victim->alignment < 0)
			xp = (base_exp * 3)/4;
		else
			xp = base_exp;
    }

    else
    {
		if (victim->alignment > 500 || victim->alignment < -500)
			xp = (base_exp * 4)/3;
		else if (victim->alignment < 200 && victim->alignment > -200)
			xp = base_exp/2;
 		else
			xp = base_exp;
    }*/
	xp = base_exp;

	if( xp <= 0)
		xp = 0;
	if (xp > 400)
		xp = 400;

	xp = xp / 2;

    /* adjust for grouping */
    xp = xp * gch->level/( UMAX(1,total_levels -1) );
    return xp;

}


void dam_message( CHAR_DATA *ch, CHAR_DATA *victim,int dam,int dt,bool immune, bool form )
{
    char buf1[256], buf2[256], buf3[256];
    const char *vs;
    const char *vp;
    const char *attack;

    if (ch == NULL || victim == NULL)
	return;

	if ( dam ==   0 ) { vs = "miss";	vp = "misses";		}
	else if ( dam <=   4 ) { vs = "{Gscratch{x";	vp = "{Gscratches{x";	}
	else if ( dam <=   8 ) { vs = "{Ggraze{x";	vp = "{Ggrazes{x";		}
	else if ( dam <=  12 ) { vs = "{Yhit{x";	vp = "{Yhits{x";		}
	else if ( dam <=  16 ) { vs = "{yinjure{x";	vp = "{Yinjures{x";		}
	else if ( dam <=  20 ) { vs = "{Ywound{x";	vp = "{Ywounds{x";		}
	else if ( dam <=  24 ) { vs = "{rmaul{x";       vp = "{rmauls{x";		}
	else if ( dam <=  28 ) { vs = "{rdecimate{x";	vp = "{rdecimates{x";	}
	else if ( dam <=  32 ) { vs = "{rdevastate{x";	vp = "{rdevastates{x";	}
	else if ( dam <=  36 ) { vs = "{rmaim{x";	vp = "{rmaims{x";		}
	else if ( dam <=  40 ) { vs = "{BMUTILATE{x";	vp = "{BMUTILATES{x";	}
	else if ( dam <=  44 ) { vs = "{BDISEMBOWEL{x";	vp = "{BDISEMBOWELS{x";	}
	else if ( dam <=  48 ) { vs = "{RDISMEMBER{x";	vp = "{RDISMEMBERS{x";	}
	else if ( dam <=  52 ) { vs = "{RMASSACRE{x";	vp = "{RMASSACRES{x";	}
	else if ( dam <=  56 ) { vs = "{RMANGLE{x";	vp = "{RMANGLES{x";		}
	else if ( dam <=  60 ) { vs = "{W*{w*{W* {rDEMOLISH {W*{w*{W*{x";
							 vp = "{W*{w*{W* {rDEMOLISHES {W*{w*{W*{x";			}
	else if ( dam <=  75 ) { vs = "{w*{W*{w* {RDEVASTATE {w*{W*{w*{x";
							 vp = "{w*{W*{w* {RDEVASTATES {w*{W*{w*{x";			}
	else if ( dam <= 100)  { vs = "{g={G={g= {BOBLITERATE {g={G={g={x";
							 vp = "{g={G={g= {BOBLITERATES {g={G={g={x";		}
	else if ( dam <= 125)  { vs = "{y>{Y>{y> {GANNIHILATE {y<{Y<{y<{x";
							 vp = "{y>{Y>{y> {GANNIHILATES {y<{Y<{y<{x";		}
	else if ( dam <= 150)  { vs = "{r<{R<{r< {rER{RAD{WI{RCA{rTE {r>{R>{r>{x";
							 vp = "{r<{R<{r< {rER{RAD{WI{RCA{rTES {r>{R>{r>{x";			}
	else if ( dam <= 200)  { vs = "{g>{B<>{g< {WDE{wS{GTR{wO{WY {g>{B<>{g<{x";
							 vp = "{g>{B<>{g< {WDE{wS{GTR{wO{WYS {g>{B<>{g<{x{x";			}
	else if ( dam <= 350)  { vs = "{c<{C<{w< {WEVISCERATE {w>{C>{c>{x";
							 vp = "{c<{C<{w< {WEVISCERATES {w>{C>{c>{x";			}    
	else if ( dam <= 400)  { vs = "{W-{B<{b( {DCRUSH {b){B>{W-{x";
							 vp = "{W-{B<{b( {DCRUSHES {b){B>{w-{x";			}
	else if ( dam <= 500)  { vs = "{w-{g={G( {DCLEAVE {G){g={w-{x";
							 vp = "{w-{g={G( {DCLEAVES {G){g={w-{x";			}
	else if ( dam <= 700)  { vs = "{Y|{y|{w|{WTHRASH{w|{y|{Y|{x";
							 vp = "{Y|{y|{w|{WTHRASHES{w|{y|{Y|{x";			}	
	else if ( dam <= 850)  { vs = "{r<Rr<{D(( {WELIMINATE {D)){R>{r>{x";
							 vp = "{r<{R<{D(( {WELIMINATES {D)){R>{r>{x";		}
	else if ( dam <= 1000) { vs = "{g<{D|{G><{D|{g> {WVAPORIZE {g<{D|{G><{D|{g>{x";
							 vp = "{g<{D|{G><{D|{g> {WVAPORIZES {g<{D|{G><{D|{g>{x";		}
	else if ( dam <= 1250) { vs = "{C<{B-|-{C> {WATOMIZE {C<{B-|-{C>{x";					
							 vp = "{C<{B-|-{C> {WATOMIZES {C<{B-|-{C>{x";				}
	else if ( dam <= 1500) { vs = "{Y><{G({R-{G){Y>< {WBANISH {Y><{G({R-{G){Y><{x";
							 vp =  "{Y><{G({R-{G){Y>< {WBANISHES {Y><{G({R-{G){Y><{x";}
	else if (dam <=  1800) { vs = "{B={Y]{Dx{Y[{B= {WSHATTER {B={Y]{Dx{Y[{B={x";
							 vp =  "{B={Y]{Dx{Y[{B= {WSHATTERS {B={Y]{Dx{Y[{B={x"; }
	else if (dam <=  2100) { vs = "{y<{B|{w+{B|{Y={B|{w+{B|{y> {WSUNDER {y<{B|{w+{B|{Y={B|{w+{B|{y>{x";
							 vp =  "{y<{B|{w+{B|{Y={B|{w+{B|{y> {WSUNDERS {y<{B|{w+{B|{Y={B|{w+{B|{y>{x"; }
	else if (dam <=  2500) { vs = "{G[]{g={b_{Y|{b_{g={G[] {WRAVAGE {G[]{g={b_{Y|{b_{g={G[]{x";
							 vp =  "{G[]{g={b_{Y|{b_{g={G[] {WRAVAGES {G[]{g={b_{Y|{b_{g={G[]{x";}
	else					{ vs = "{D[{Y-{r|{R={r|{Y-{D] {WSLAUGHTER {D[{Y-{r|{R={r|{Y-{D]{x";
							  vp =  "{D[{Y-{r|{R={r|{Y-{D] {WSLAUGHTERS {D[{Y-{r|{R={r|{Y-{D]{x";}

    if ( dt == TYPE_HIT )
    {
	if (ch  == victim)
	{
	    sprintf( buf1, "$n %s $melf",vp );
	    sprintf( buf2, "You %s yourself",vs);
	}
	else
	{
	    sprintf( buf1, "$n %s $N	{C({Y%d dmg{C){x",  vp, dam );
	    sprintf( buf2, "You %s $N	{G({Y%d dmg{G){x", vs, dam );
	    sprintf( buf3, "$n %s you	{R({Y%d dmg{R){x", vp, dam );
	}
    }
    else
    {
		if ( dt >= 0 && dt < MAX_SKILL )
			attack	= skill_table[dt].noun_damage;
		else if ( dt >= TYPE_HIT
		&& dt < TYPE_HIT + MAX_DAMAGE_MESSAGE) 
			attack	= attack_table[dt - TYPE_HIT].noun;
		else
		{
			bug( "Dam_message: bad dt %d.", dt );
			dt  = TYPE_HIT;
			attack  = attack_table[0].name;
		}
		if (immune)
		{
			if (ch == victim)
			{
				sprintf(buf1,"$n is unaffected by $s own %s.",attack);
				sprintf(buf2,"Luckily, you are immune to that.");
			} 
			else
			{
    			sprintf(buf1,"$N is unaffected by $n's %s!",attack);
    			sprintf(buf2,"$N is unaffected by your %s!",attack);
    			sprintf(buf3,"$n's %s is powerless against you.",attack);
			}
		}
	else
	{
	    if (ch == victim)
	    {
		sprintf( buf1, "$n's %s %s $m	{C({Y%d dmg{C){x",attack,vp, dam);
		sprintf( buf2, "Your %s %s you	{R({Y%d dmg{R){x",attack,vp, dam);
	    }
	    else
	    {
	    	sprintf( buf1, "$n's %s %s $N	{C({Y%d dmg{C){x",  attack, vp, dam );
	    	sprintf( buf2, "Your %s %s $N	{G({Y%d dmg{G){x",  attack, vp, dam );
	    	sprintf( buf3, "$n's %s %s you	{R({Y%d dmg{R){x", attack, vp, dam );
	    }
	}
    }

    if (ch == victim)
    {
		act(buf1,ch,NULL,NULL,TO_ROOM);
		act(buf2,ch,NULL,NULL,TO_CHAR);
    }
    else
    {
    	act( buf1, ch, NULL, victim, TO_NOTVICT );
    	act( buf2, ch, NULL, victim, TO_CHAR );
    	act( buf3, ch, NULL, victim, TO_VICT );
    }

    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	return;

    if ( IS_OBJ_STAT(obj,ITEM_NOREMOVE))
    {
	act("$S weapon won't budge!",ch,NULL,victim,TO_CHAR);
	act("$n tries to disarm you, but your weapon won't budge!",
	    ch,NULL,victim,TO_VICT);
	act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
	return;
    }

    act( "$n DISARMS you and sends your weapon flying!", 
	 ch, NULL, victim, TO_VICT    );
    act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n disarms $N!",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_OBJ_STAT(obj,ITEM_NODROP) 
		|| IS_OBJ_STAT(obj,ITEM_INVENTORY) 
		|| IS_SET(ch->in_room->room_flags,ROOM_ARENA)
		|| IS_SET(ch->in_room->room_flags,ROOM_HONCIRC) )
	{
		obj_to_char( obj, victim );
	}
    else
    {
	obj_to_room( obj, victim->in_room );
	if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
	    get_obj(victim,obj,NULL);
    }

    return;
}

void do_berserk( CHAR_DATA *ch, char *argument)
{
    int chance, hp_percent;

    if ((chance = get_skill(ch,gsn_berserk)) == 0
    ||  (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BERSERK))
    ||  (!IS_NPC(ch)
    &&   ch->level < skill_table[gsn_berserk].skill_level[ch->class]))
    {
	send_to_char("You turn red in the face, but nothing happens.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_BERSERK) || is_affected(ch,gsn_berserk)
    ||  is_affected(ch,skill_lookup("frenzy")))
    {
	send_to_char("You get a little madder.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_CALM))
    {
	send_to_char("You're feeling to mellow to berserk.\n\r",ch);
	return;
    }

    if (ch->mana < 50)
    {
	send_to_char("You can't get up enough energy.\n\r",ch);
	return;
    }

    /* modifiers */

    /* fighting */
    if (ch->position == POS_FIGHTING)
	chance += 10;

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit/ch->max_hit;
    chance += 25 - hp_percent/2;

    if (number_percent() < chance)
    {
	AFFECT_DATA af;

	WAIT_STATE(ch,PULSE_VIOLENCE);
	ch->mana -= 50;
	ch->move /= 2;

	/* heal a little damage */
	ch->hit += ch->level * 2;
	ch->hit = UMIN(ch->hit,ch->max_hit);

	send_to_char("Your pulse races as you are consumed by rage!\n\r",ch);
	act("$n gets a wild look in $s eyes.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_berserk,TRUE,2);

	af.where	= TO_AFFECTS;
	af.type		= gsn_berserk;
	af.level	= ch->level;
	af.duration	= number_fuzzy(ch->level / 8);
	af.modifier	= UMAX(1,ch->level/5);
	af.bitvector 	= AFF_BERSERK;

	af.location	= APPLY_HITROLL;
	affect_to_char(ch,&af);

	af.location	= APPLY_DAMROLL;
	affect_to_char(ch,&af);

	af.modifier	= UMAX(10,10 * (ch->level/5));
	af.location	= APPLY_AC;
	affect_to_char(ch,&af);
    }

    else
    {
	WAIT_STATE(ch,3 * PULSE_VIOLENCE);
	ch->mana -= 25;
	ch->move /= 2;

	send_to_char("Your pulse speeds up, but nothing happens.\n\r",ch);
	check_improve(ch,gsn_berserk,FALSE,2);
    }
}

void do_bash( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);
 
    if ( (chance = get_skill(ch,gsn_bash)) == 0
    ||	 (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BASH))
    ||	 (!IS_NPC(ch)
    &&	  ch->level < skill_table[gsn_bash].skill_level[ch->class]))
    {	
	send_to_char("Bashing? What's that?\n\r",ch);
	return;
    }
 
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

    if (victim->position < POS_FIGHTING)
    {
	act("You'll have to let $M get back up first.",ch,NULL,victim,TO_CHAR);
	return;
    } 

    if (victim == ch)
    {
	send_to_char("You try to bash your brains out, but fail.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if ( IS_NPC(victim) && 
	victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting) && !is_arena(victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is your friend!",ch,NULL,victim,TO_CHAR);
	return;
    }

    /* modifiers */

    /* size  and weight */
    chance += ch->carry_weight / 250;
    chance -= victim->carry_weight / 200;

    if (ch->size < victim->size)
	chance += (ch->size - victim->size) * 15;
    else
	chance += (ch->size - victim->size) * 10; 


    /* stats */
    chance += get_curr_stat(ch,STAT_STR);
    chance -= (get_curr_stat(victim,STAT_DEX) * 4)/3;
    chance -= GET_AC(victim,AC_BASH) /25;
    /* speed */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
        chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
        chance -= 30;

    /* level */
    chance += (ch->level - victim->level);

    if (!IS_NPC(victim) 
	&& chance < get_skill(victim,gsn_dodge) )
    {	
        act("$n tries to bash you, but you dodge it.",ch,NULL,victim,TO_VICT);
        act("$N dodges your bash, you fall flat on your face.",ch,NULL,victim,TO_CHAR);
        WAIT_STATE(ch,skill_table[gsn_bash].beats);
        return;
	chance -= 3 * (get_skill(victim,gsn_dodge) - chance);
    }

    /* now the attack */
    if (number_percent() < chance )
    {
    
	act("$n sends you sprawling with a powerful bash!",
		ch,NULL,victim,TO_VICT);
	act("You slam into $N, and send $M flying!",ch,NULL,victim,TO_CHAR);
	act("$n sends $N sprawling with a powerful bash.",
		ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_bash,TRUE,1);

	DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
	WAIT_STATE(ch,skill_table[gsn_bash].beats);
	victim->position = POS_RESTING;
	damage(ch,victim,number_range(2,2 + 2 * ch->size + chance/20),gsn_bash,
	    DAM_BASH,FALSE,FALSE);
	
    }
    else
    {
	damage(ch,victim,0,gsn_bash,DAM_BASH,FALSE,FALSE);
	act("You fall flat on your face!",
	    ch,NULL,victim,TO_CHAR);
	act("$n falls flat on $s face.",
	    ch,NULL,victim,TO_NOTVICT);
	act("You evade $n's bash, causing $m to fall flat on $s face.",
	    ch,NULL,victim,TO_VICT);
	check_improve(ch,gsn_bash,FALSE,1);
	ch->position = POS_RESTING;
	WAIT_STATE(ch,skill_table[gsn_bash].beats * 3/2); 
    }
	check_killer(ch,victim);
}

void do_dirt( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_dirt)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK_DIRT))
    ||   (!IS_NPC(ch)
    &&    ch->level < skill_table[gsn_dirt].skill_level[ch->class]))
    {
	send_to_char("You get your feet dirty.\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't in combat!\n\r",ch);
	    return;
	}
    }

    else if ((victim = get_char_room(ch,NULL,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(victim,AFF_BLIND))
    {
	act("$E's already been blinded.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("Very funny.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting)&& !is_arena(victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
	return;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_DEX);

    /* speed  */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
	chance -= 25;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
	chance += 1;

    /* terrain */

    switch(ch->in_room->sector_type)
    {
	case(SECT_INSIDE):		chance -= 20;	break;
	case(SECT_CITY):		chance -= 10;	break;
	case(SECT_FIELD):		chance +=  5;	break;
	case(SECT_FOREST):				break;
	case(SECT_HILLS):				break;
	case(SECT_MOUNTAIN):		chance -= 10;	break;
	case(SECT_WATER_SWIM):		chance  =  0;	break;
	case(SECT_WATER_NOSWIM):	chance  =  0;	break;
	case(SECT_AIR):			chance  =  0;  	break;
	case(SECT_DESERT):		chance += 10;   break;
    }

    if (chance == 0)
    {
	send_to_char("There isn't any dirt to kick.\n\r",ch);
	return;
    }

    /* now the attack */
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	act("$n is blinded by the dirt in $s eyes!",victim,NULL,NULL,TO_ROOM);
	act("$n kicks dirt in your eyes!",ch,NULL,victim,TO_VICT);
	damage(ch,victim,number_range(2,5),gsn_dirt,DAM_NONE,FALSE,FALSE);
	send_to_char("You can't see a thing!\n\r",victim);
	check_improve(ch,gsn_dirt,TRUE,2);
	WAIT_STATE(ch,skill_table[gsn_dirt].beats);

	af.where	= TO_AFFECTS;
	af.type 	= gsn_dirt;
	af.level 	= ch->level;
	af.duration	= 0;
	af.location	= APPLY_HITROLL;
	af.modifier	= -4;
	af.bitvector 	= AFF_BLIND;

	affect_to_char(victim,&af);
    }
    else
    {
	damage(ch,victim,0,gsn_dirt,DAM_NONE,TRUE,FALSE);
	check_improve(ch,gsn_dirt,FALSE,2);
	WAIT_STATE(ch,skill_table[gsn_dirt].beats);
    }
	check_killer(ch,victim);
}

void do_trip( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_trip)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_TRIP))
    ||   (!IS_NPC(ch) 
	  && ch->level < skill_table[gsn_trip].skill_level[ch->class]))
    {
	send_to_char("Tripping?  What's that?\n\r",ch);
	return;
    }


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

    if (is_safe(ch,victim))
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting) && !is_arena(victim))
    {
	send_to_char("Kill stealing is not permitted.\n\r",ch);
	return;
    }
    
    if (IS_AFFECTED(victim,AFF_FLYING))
    {
	act("$S feet aren't on the ground.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim->position < POS_FIGHTING)
    {
	act("$N is already down.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("You fall flat on your face!\n\r",ch);
	WAIT_STATE(ch,2 * skill_table[gsn_trip].beats);
	act("$n trips over $s own feet!",ch,NULL,NULL,TO_ROOM);
	return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("$N is your beloved master.",ch,NULL,victim,TO_CHAR);
	return;
    }

    /* modifiers */

    /* size */
    if (ch->size < victim->size)
        chance += (ch->size - victim->size) * 10;  /* bigger = harder to trip */

    /* dex */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= get_curr_stat(victim,STAT_DEX) * 3 / 2;

    /* speed */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
	chance -= 20;

    /* level */
    chance += (ch->level - victim->level) * 2;


    /* now the attack */
    if (number_percent() < chance)
    {
	act("$n trips you and you go down!",ch,NULL,victim,TO_VICT);
	act("You trip $N and $N goes down!",ch,NULL,victim,TO_CHAR);
	act("$n trips $N, sending $M to the ground.",ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_trip,TRUE,1);

	DAZE_STATE(victim,2 * PULSE_VIOLENCE);
        WAIT_STATE(ch,skill_table[gsn_trip].beats);
	victim->position = POS_RESTING;
	damage(ch,victim,number_range(2, 2 +  2 * victim->size),gsn_trip,
	    DAM_BASH,TRUE,FALSE);
    }
    else
    {
	damage(ch,victim,0,gsn_trip,DAM_BASH,TRUE,FALSE);
	WAIT_STATE(ch,skill_table[gsn_trip].beats*2/3);
	check_improve(ch,gsn_trip,FALSE,1);
    } 
	check_killer(ch,victim);
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
/*  Allow player killing*/
    if ( !IS_NPC(victim)
		&& !IS_NPC(ch)
		&& !is_arena(victim)  
		&& !IS_SET(victim->in_room->room_flags, ROOM_HONCIRC))
    {
        if ( !IS_SET(victim->act, PLR_KILLER)
        &&   !IS_SET(victim->act, PLR_THIEF) )
        {
            send_to_char( "You must MURDER a player.\n\r", ch );
            return;
        }
    }

    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting) && !is_arena(victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to MURDER, spell it out.\n\r", ch );
    return;
}



void do_murder( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) || (IS_NPC(ch) && IS_SET(ch->act,ACT_PET)))
	return;

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting) && !is_arena(victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    if (IS_NPC(ch))
	sprintf(buf, "Help! I am being attacked by %s!",ch->short_descr);
    else
    	sprintf( buf, "Help!  I am being attacked by %s!", ch->name );
    do_function(victim, &do_yell, buf );
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if (arg[0] == '\0')
    {
        send_to_char("Backstab whom?\n\r",ch);
        return;
    }

    if (ch->fighting != NULL)
    {
	send_to_char("You're facing the wrong end.\n\r",ch);
	return;
    }
 
    else if ((victim = get_char_room(ch,NULL,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

	if (!know_skill(ch,"backstab"))
		return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting) && !is_arena(victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
	send_to_char( "You need to wield a weapon to backstab.\n\r", ch );
	return;
    }

    if ( victim->hit < victim->max_hit / 3)
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    check_killer( ch, victim );
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if ( number_percent( ) < get_skill(ch,gsn_backstab)
    || ( get_skill(ch,gsn_backstab) >= 2 && !IS_AWAKE(victim) ) )
    {
	check_improve(ch,gsn_backstab,TRUE,1);
	check_dual_attack(ch,victim, gsn_backstab);
    }
    else
    {
	check_improve(ch,gsn_backstab,FALSE,1);
	damage( ch, victim, 0, gsn_backstab,DAM_NONE,TRUE,FALSE);
    }

    return;
}

void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
        if ( ch->position == POS_FIGHTING )
            ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if (IS_AFFECTED2(ch, AFF_FORTIFY) || (is_affected(ch, gsn_fortify )))
	{
		send_to_char("You cant flee while holed up in a fort\n\r",ch);
		act( "$n attempts to flee, but realizes at the last moment that they're stuck in a fort", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if (IS_AFFECTED2(ch, AFF_HOLD_PERSON))
	{
		send_to_char("You cant flee becuase your legs are stuck to the ground!\n\r",ch);
		act( "$n tries to flee, but is held to firmly to run anywhere", ch, NULL, NULL, TO_ROOM );
		return;
	}

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	||   pexit->u1.to_room == NULL
	||   IS_SET(pexit->exit_info, EX_CLOSED)
	||   number_range(0,ch->daze) != 0
	|| ( IS_NPC(ch)
	&&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
	    continue;

	move_char( ch, door, FALSE );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;

	if ( !IS_NPC(ch) )
	{
	    send_to_char( "You flee from combat!\n\r", ch );
	if( (ch->class == 2) 
	    && (number_percent() < 3*(ch->level/2) ) )
		send_to_char( "You snuck away safely.\n\r", ch);
	else
	    {
	    send_to_char( "You lost 10 exp.\n\r", ch); 
	    gain_exp( ch, -10 );
	    }
	}

	stop_fighting( ch, TRUE );
	return;
    }

    send_to_char( "PANIC! You couldn't escape!\n\r", ch );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    if ( IS_NPC(fch) && !is_same_group(ch,victim) && !is_arena(victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( number_percent( ) > get_skill(ch,gsn_rescue))
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	check_improve(ch,gsn_rescue,FALSE,1);
	return;
    }

    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );
    check_improve(ch,gsn_rescue,TRUE,1);

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    check_killer( ch, fch );
    set_fighting( ch, fch );
    set_fighting( fch, ch );
    return;
}



void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_kick].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK))
	return;

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( get_skill(ch,gsn_kick) > number_percent())
    {
	damage(ch,victim,number_range( 1, ch->level ), gsn_kick,DAM_BASH,TRUE,FALSE);
	check_improve(ch,gsn_kick,TRUE,1);
    }
    else
    {
	damage( ch, victim, 0, gsn_kick,DAM_BASH,TRUE,FALSE);
	check_improve(ch,gsn_kick,FALSE,1);
    }
	check_killer(ch,victim);
    return;
}




void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
	char buf[MSL];
    int chance,hth,ch_weapon,vict_weapon,ch_vict_weapon;

    hth = 0;

    if ((chance = get_skill(ch,gsn_disarm)) == 0)
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL 
    &&   ((hth = get_skill(ch,gsn_hand_to_hand)) == 0
    ||    (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM))))
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

	if (IS_AFFECTED2(victim, AFF_VISE_GRIP))
	{
		sprintf(buf,"%s's grip on their weapon is just to strong for you to break\n\r",victim->name);
		send_to_char(buf,ch);
		return;
	}

    /* find weapon skills */
    ch_weapon = get_weapon_skill(ch,get_weapon_sn(ch));
    vict_weapon = get_weapon_skill(victim,get_weapon_sn(victim));
    ch_vict_weapon = get_weapon_skill(ch,get_weapon_sn(victim));

    /* modifiers */

    /* skill */
    if ( get_eq_char(ch,WEAR_WIELD) == NULL)
	chance = chance * hth/150;
    else
	chance = chance * ch_weapon/100;

    chance += (ch_vict_weapon/2 - vict_weapon) / 2; 

    /* dex vs. strength */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_STR);

    /* level */
    chance += (ch->level - victim->level) * 2;
 

	if(IS_NPC(victim) && (victim->level >= 101))
	{
		chance = 1;
	}
    /* and now the attack */
    if (number_percent() < chance)
    {
    	WAIT_STATE( ch, skill_table[gsn_disarm].beats );
	disarm( ch, victim );
	check_improve(ch,gsn_disarm,TRUE,1);
    }
    else
    {
	WAIT_STATE(ch,skill_table[gsn_disarm].beats);
	act("You fail to disarm $N.",ch,NULL,victim,TO_CHAR);
	act("$n tries to disarm you, but fails.",ch,NULL,victim,TO_VICT);
	act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_disarm,FALSE,1);
    }
    check_killer(ch,victim);
    return;
}

void do_surrender( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    if ( (mob = ch->fighting) == NULL )
    {
	send_to_char( "But you're not fighting!\n\r", ch );
	return;
    }
    act( "You surrender to $N!", ch, NULL, mob, TO_CHAR );
    act( "$n surrenders to you!", ch, NULL, mob, TO_VICT );
    act( "$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT );
    stop_fighting( ch, TRUE );

    if ( !IS_NPC( ch ) && IS_NPC( mob ) 
    &&   ( !HAS_TRIGGER_MOB( mob, TRIG_SURR ) 
        || !p_percent_trigger( mob, NULL, NULL, ch, NULL, NULL, TRIG_SURR ) ) )
    {
	act( "$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR );
	multi_hit( mob, ch, TYPE_UNDEFINED );
    }
}

void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



/* -----------------------------------------------------------------------
The following snippet was written by Gary McNickle (dharvest) for
Rom 2.4 specific MUDs and is released into the public domain. My thanks to
the originators of Diku, and Rom, as well as to all those others who have
released code for this mud base.  Goes to show that the freeware idea can
actually work. ;)  In any case, all I ask is that you credit this code
properly, and perhaps drop me a line letting me know it's being used.

from: gary@dharvest.com
website: http://www.dharvest.com
or http://www.dharvest.com/resource.html (rom related)

Send any comments, flames, bug-reports, suggestions, requests, etc... 
to the above email address.
----------------------------------------------------------------------- */



/** Function: do_slay
  * Descr   : Slays (kills) a player, optionally sending one of several
  *           predefined "slay option" messages to those involved.
  * Returns : (void)
  * Syntax  : slay (who) [option]
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void do_slay (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg2);

	if (arg[0] == '\0') {
		send_to_char ("Slay whom?\n\r", ch);
		return;
	}

	if ((victim = get_char_room(ch,NULL,arg)) == NULL)
	{
		if (ch->level < ML)
		{
			send_to_char ("They aren't here.\n\r", ch);
			return;
		}
		else
		{
			if ((victim = get_char_world (ch, arg)) == NULL)
			{
				send_to_char ("They aren't here.\n\r", ch);
				return;
			}
		}

	}

	if (ch == victim) 
	{
		send_to_char ("Suicide is a mortal sin.\n\r", ch);
		return;
	}

	if (!IS_NPC (victim) && victim->level >= get_trust (ch)) 
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	/* check slay message list */
	if(arg2[0] != '\0' && ALLOW_MULTI_SLAY)
	{
		SLAY_MESSAGE *smIndex;
		char buf[MSL];
		char *sName;
		int cnt = 0;

		if(!str_cmp(arg2, "list"))
		{
			send_to_char("Available slay types to you are:\n\r", ch);
			/*
			 * Loop through the slay index
			 * Show all slays with Character name or no name
			 * Edit slay name to 19 characters excluding color
			 * Show list as 4 across
			 */
			for(smIndex=SMHead; smIndex != NULL; smIndex=smIndex->SMnext)
			{
				if(!str_cmp(smIndex->Char_Name, ch->name)
					|| smIndex->Char_Name[0] == '\0')
				{
					sName = str_dup(smIndex->Slay_Name);
					strcpy (buf, sName);

					sprintf(buf,"%s ", smIndex->Slay_Name);
					stc(buf,ch);
					free(sName);
					cnt++;

					if(cnt >= 4)
					{
						send_to_char("\n\r", ch);
						cnt = 0;
					}
				}
			}
			send_to_char("\n\r", ch);
			return;
		}


		for(smIndex=SMHead; smIndex != NULL; smIndex=smIndex->SMnext)
		{
			if(!str_cmp(arg2, smIndex->Slay_Name)
				&& (smIndex->Char_Name[0] == '\0'
				|| (!str_cmp(arg2, smIndex->Slay_Name)
				&& !str_cmp(smIndex->Char_Name, ch->name))))
			{
				act (smIndex->Show_Char, ch, NULL, victim, TO_CHAR);
				act (smIndex->Show_Vict, ch, NULL, victim, TO_VICT);
				act (smIndex->Show_Room, ch, NULL, victim, TO_NOTVICT);

				if(!IS_NPC(victim))
					victim->pcdata->slay_cnt += 1;
				raw_kill (ch,victim);
				return;
			}
		}
	}

	act ("{1You slay $M in cold blood!{x", ch, NULL, victim, TO_CHAR);
	act ("{1$n slays you in cold blood!{x", ch, NULL, victim, TO_VICT);
	act ("{1$n slays $N in cold blood!{x", ch, NULL, victim, TO_NOTVICT);

	if(!IS_NPC(victim))
		victim->pcdata->slay_cnt += 1;

	raw_kill (ch,victim);
	return;
}

void do_whirlwind( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *pChar;
   CHAR_DATA *pChar_next;
   OBJ_DATA *wield;
   bool found = FALSE;

   if (!IS_NPC( ch )
        && ch->level < skill_table[gsn_whirlwind].skill_level[ch->class] )
   {
      send_to_char( "You don't know how to do that...\n\r", ch );
      return;
   }

   if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
   {
      send_to_char( "You need to wield a weapon first...\n\r", ch );
      return;
   }

	if (!know_skill(ch,"whirlwind"))
		return;

   WAIT_STATE( ch, skill_table[gsn_whirlwind].beats );
   act( "$n holds $p firmly, and starts spinning round...", ch, wield, NULL, TO_ROOM );
   act( "You hold $p firmly, and start spinning round...",  ch, wield, NULL, TO_CHAR );
   
   pChar_next = NULL;   
   for ( pChar = ch->in_room->people; pChar; pChar = pChar_next )
   {
      pChar_next = pChar->next_in_room;
		if (is_safe(ch,pChar))
			continue;
		if ( IS_NPC(pChar)
			|| !is_same_team(ch,pChar)
			|| in_single_war(pChar))

		{
			found = TRUE;
			act( "$n turns toward YOU!", ch, NULL, pChar, TO_VICT    );
			check_dual_attack(ch,pChar,gsn_whirlwind);
		}
   }

   if ( !found )
   {
      act( "$n looks dizzy, and a tiny bit embarassed.", ch, NULL, NULL, TO_ROOM );
      act( "You feel dizzy, and a tiny bit embarassed.", ch, NULL, NULL, TO_CHAR );
   }
   

   
   if ( !found && number_percent() < 25 )
   {
      act( "$n loses $s balance and falls into a heap.",  ch, NULL, NULL, TO_ROOM );
      act( "You lose your balance and fall into a heap.", ch, NULL, NULL, TO_CHAR );
      ch->position = POS_STUNNED;
   }
   
   return;
}      


/* Create a tombstone and put it in the graveyard. ONE per character
 * per reboot.
 * - Talon of TOTG MUD tgods.orisis.net 3333
 * Email: tgods@newhaven.orisis.net. All code copyrighted (c) 2000 NML
 */
void make_stone( CHAR_DATA *ch )
{
    OBJ_DATA *stone;
    ROOM_INDEX_DATA *grave;
    char buf [ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
        return;
	
	/* Define the name of the object we're looking for. */
	sprintf( buf, "%s tombstone tomb stone", capitalize( ch->name ) );

     /* Let's make the tombstone! */
	stone = create_object( get_obj_index( OBJ_VNUM_GRAVESTONE ), 0 );
	sprintf( buf, "%s tombstone tomb stone", ch->name );
    free_string( stone->name );
    stone->name = str_dup( buf ); /*Set Name*/
    sprintf( buf, "%s's tombstone", ch->name );
    free_string( stone->short_descr );
    stone->short_descr = str_dup( buf ); /*Set Short Desc*/
    sprintf( buf, "{w%s, {DRIP {w({cSlain by: %s{w){x", ch->name, ch->slain_by );
    free_string( stone->description );
    stone->description = str_dup( buf ); /*Set Desc*/


	/* Put the stone into the cemetary */
    grave = get_room_index( number_range( 12050, 12053 ) );

    obj_to_room( stone, grave );
    send_to_char( "{WYour tombstone has been placed in the cemetary{x.\n\r", ch );
    act( "{WA gravedigger throws the last pile of dirt into $n's grave{x.\n\r",
		ch, NULL, NULL, TO_ROOM );
    return;
}

/* Bowfire code -- actual firing function */
void do_fire( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim = NULL;
    OBJ_DATA *arrow;
	OBJ_DATA *bow;
    ROOM_INDEX_DATA *was_in_room;
    EXIT_DATA *pexit;
    int dam ,door ,chance;
	int value;
	int item_lodged;
	int random;


    bow = get_eq_char(ch, WEAR_WIELD);
    if (bow == NULL)
	{
	send_to_char("{WWhat are you going to do, throw the arrow at them?{x\n\r", ch);
	return;
	}

    if (bow->value[0] != WEAPON_BOW)
	{
	send_to_char("{WYou might want to use a bow to fire that arrow with{x\n\r", ch);
	return;
	}    

    one_argument( argument, arg );
    if ( arg[0] == '\0' && ch->fighting == NULL )
    {
        send_to_char( "{WFire an arrow at who?{x\n\r", ch );
        return;
    }

	if (!str_cmp(arg, "none") || !str_cmp(arg, "self") || victim == ch)
	{
		send_to_char("{WHow exactly did you plan on firing an arrow at yourself?{x\n\r", ch );
		return;
	}

    if ( ( arrow = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
        send_to_char( "{WYou hold nothing in your hand.{x\n\r", ch );
        return;
    }


    if ( arrow->item_type != ITEM_ARROW )
    {
        send_to_char( "{WYou can only a fire arrows or quarrels.{x\n\r", ch );
        return;
    }
	
    if ( arg[0] == '\0' )
    {
        if ( ch->fighting != NULL )
        {
            victim = ch->fighting;
        }
        else
        {
            send_to_char( "{WFire at whom or what?{x\n\r", ch );
            return;
        }
    }
    else
    {
        
	/* See if who you are trying to shoot at is nearby... */

        if ( ( victim = get_char_room ( ch,NULL, arg ) ) == NULL)
        {
            was_in_room=ch->in_room;


            for( door=0 ; door<=5 && victim==NULL ; door++ )
             { 
                if ( (  pexit = was_in_room->exit[door] ) != NULL
                   &&   pexit->u1.to_room != NULL
                   &&   pexit->u1.to_room != was_in_room 
                   &&   !strcmp( pexit->u1.to_room->area->name ,
                                was_in_room->area->name ) )
                   { 
                     ch->in_room = pexit->u1.to_room;
                     victim = get_char_room ( ch,NULL, arg ); 
                    }
               
              }


            ch->in_room=was_in_room;
            if(victim==NULL)
              {
               send_to_char( "{WYou can't find it.{x\n\r", ch );
               return;
              }
            else
              {  if(IS_SET(pexit->exit_info,EX_CLOSED))
                    { send_to_char("{WYou can't fire through a door.{x",ch);
                      return;
                     } 
             }
        }
    }




	if((ch->in_room) == (victim->in_room))
	{
	send_to_char("{WDon't you think that standing a bit further away would be wise?{x\n\r", ch);
	return;
	}

	/* Lag the bowman... */
	WAIT_STATE( ch, 2 * PULSE_VIOLENCE ); 

	/* Fire the damn thing finally! */


	if(arrow->item_type== ITEM_ARROW )
	{
        
      /* Valid target? */
        
        if ( victim != NULL )
        {
            act( "{W$n {Wfires $p {Wat $N{W.{x", ch, arrow, victim, TO_NOTVICT );
            act( "{WYou fire $p {Wat $N{W.{x", ch, arrow, victim, TO_CHAR );
			act( "{W$n {Wfires $p {Wat you.{x", ch, arrow, victim, TO_VICT );		
		}


      /* Did it hit? */
      
        if (ch->level <   arrow->level
        ||  number_percent() >= 20 + get_skill(ch,gsn_bow) * 4/5 )
        {       
             /* denied... */
                     
              act( "{WYou fire $p {Wmissing, and it lands harmlessly on the ground.{x",
                 ch,  arrow,NULL,TO_CHAR);
              act( "{W$r fires $p {Wmissing, and it lands harmlessly on the ground.{x",
                 ch,  arrow,NULL,TO_ROOM);
              obj_from_char(arrow);
			  obj_to_room(arrow, victim->in_room);
          check_improve(ch,gsn_bow,FALSE,2);
        }
        else
        {      
               /* Shawing battah!  Now, where did it thud into? */


	chance=dice(1,10);
	item_lodged = flag_value( extra_flags, "lodged" );
	random = number_range(0,2);
    switch (random)
    {
	case 0 :/*leg*/
            obj_from_char(arrow);
			obj_to_char(arrow, victim);
			value = flag_value( wear_flags, "lodge_leg" );
			TOGGLE_BIT(arrow->wear_flags, value);
			value = flag_value( wear_flags, "hold" );
			TOGGLE_BIT(arrow->wear_flags, value);
			wear_obj(victim, arrow,TRUE);
			TOGGLE_BIT(arrow->extra_flags, item_lodged);
            dam =  dice(arrow->value[1],arrow->value[2]);
			damage( ch, victim, dam, gsn_bow, DAM_PIERCE, TRUE,FALSE );						
			check_improve(ch,gsn_bow,TRUE,2);
			break;
	case 1 :/*chest*/
            obj_from_char(arrow);
			obj_to_char(arrow, victim);
			value = flag_value( wear_flags, "lodge_rib" );
			TOGGLE_BIT(arrow->wear_flags, value);
			value = flag_value( wear_flags, "hold" );
			TOGGLE_BIT(arrow->wear_flags, value);
			wear_obj(victim, arrow,TRUE);
			TOGGLE_BIT(arrow->extra_flags, item_lodged);
            dam = 3*( dice(arrow->value[1],arrow->value[2]))/2;
			damage( ch, victim, dam, gsn_bow, DAM_PIERCE, TRUE,FALSE );						
			check_improve(ch,gsn_bow,TRUE,2);
			break;
	case 2 :/*arm*/
            obj_from_char(arrow);
			obj_to_char(arrow, victim);
			value = flag_value( wear_flags, "lodge_arm" );
			TOGGLE_BIT(arrow->wear_flags, value);
			value = flag_value( wear_flags, "hold" );
			TOGGLE_BIT(arrow->wear_flags, value);
			wear_obj(victim, arrow,TRUE);
			TOGGLE_BIT(arrow->extra_flags, item_lodged);
            dam = 2*( dice(arrow->value[1],arrow->value[2]));
			damage( ch, victim, dam, gsn_bow, DAM_PIERCE, TRUE,FALSE );						
			check_improve(ch,gsn_bow,TRUE,2);
			break;
	}		
   }

  } 

    return;
}


bool check_holy_armor( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int sn;
    int chance;
	int random;

	random = number_range(0,100 );
	sn = skill_lookup( "holy armor");
    
	chance = get_skill(victim,sn)/7.5 + get_curr_stat(ch,STAT_WIS)/8;

	if ( !is_affected(victim,sn) )
	{
		return FALSE;
	}

	if ( random >= chance + victim->level - (ch->level) )
	{
		return FALSE;
	}
	else
	{
		act( "Your {Dh{wo{Wl{Yy ar{Wm{wo{Dr{x absorbs $n's attack.",  ch, NULL, victim, TO_VICT    );
		act( "$N's {Dh{wo{Wl{Yy ar{Wm{wo{Dr{x absorbs your attack.", ch, NULL, victim, TO_CHAR    );
		ch->mana -= skill_table[sn].min_mana;
	return TRUE;
	}

}


void check_dual_attack( CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
	int chance;

	chance = ch->level/2;

	if (get_eq_char (ch, WEAR_SECONDARY))
	{
		if (number_percent( ) < chance)
		{
			one_hit( ch, victim, dt, FALSE,FALSE );
			one_hit( ch, victim, dt, TRUE,FALSE );
			return;
		}
		else
		{
			one_hit( ch, victim, dt, FALSE,FALSE );
			return;
		}
	}
    else
	{
		one_hit( ch, victim, dt, FALSE ,FALSE);
		return;
	}
}

void team_win( CHAR_DATA *ch, CHAR_DATA *victim)
{
	char buf[MSL];
	DESCRIPTOR_DATA *d;

	if (red_num == 0)
	{
		sprintf(buf, "{WThe {BB{bl{Bu{be{W team is victorious in the arena!{x");
		do_wartalk(ch,buf);
		wartype = 0;
		min_level = 0;           
		max_level = 0;
		iswar = FALSE;
		inwar = 0;
		wartimer = 0;
		time_left = 0;
		REMOVE_BIT(victim->plr2,PLR2_ARENA);
		remove_teams(victim);
		char_from_room(victim);
		char_to_room(victim, get_room_index(ROOM_VNUM_ARENA));
		do_look(victim, "auto");
		for ( d = descriptor_list; d != NULL; d = d->next)
		{
			if IS_SET(d->character->plr2,PLR2_BLUE_TEAM)
			{
				war_prize(d->character);
				d->character->hit = d->character->max_hit;
				d->character->mana = d->character->max_mana;
				d->character->move = d->character->max_move;
				REMOVE_BIT(d->character->plr2, PLR2_ARENA);
				remove_teams(d->character);
				char_from_room(d->character);
				char_to_room(d->character, get_room_index(ROOM_VNUM_ARENA));
				do_look(d->character, "auto");
			}
			if IS_SET(d->character->plr2,PLR2_RED_TEAM)
			{
				d->character->pcdata->arena_losses ++;
				remove_teams(d->character);
			}
		}
		warprize = 0;
	}
	else if (blue_num == 0)
	{
		sprintf(buf, "{WThe {RR{re{Rd{W team is victorious in the arena!{x");
		do_wartalk(ch,buf);
		wartype = 0;
		min_level = 0;           
		max_level = 0;
		iswar = FALSE;
		inwar = 0;
		wartimer = 0;
		time_left = 0;
		REMOVE_BIT(victim->plr2,PLR2_ARENA);
		remove_teams(victim);
		char_from_room(victim);
		char_to_room(victim, get_room_index(ROOM_VNUM_ARENA));
		do_look(victim, "auto");
		for ( d = descriptor_list; d != NULL; d = d->next)
		{
			if IS_SET(d->character->plr2,PLR2_RED_TEAM)
			{
				war_prize(d->character);
				d->character->hit = d->character->max_hit;
				d->character->mana = d->character->max_mana;
				d->character->move = d->character->max_move;
				REMOVE_BIT(d->character->plr2, PLR2_ARENA);
				remove_teams(d->character);
				char_from_room(d->character);
				char_to_room(d->character, get_room_index(ROOM_VNUM_ARENA));
				do_look(d->character, "auto");
			}
			if IS_SET(d->character->plr2,PLR2_BLUE_TEAM)
			{
				d->character->pcdata->arena_losses ++;
				remove_teams(d->character);
			}
		}
		warprize = 0;
	}
	else
		return;
}


bool 	know_skill	(CHAR_DATA *ch, char *argument)
{
	int sn;

	if ( ( sn = find_spell( ch,argument ) ) < 0
	|| ( !IS_NPC(ch)
	&&   (ch->level < skill_table[sn].skill_level[ch->class] 
 	||    ch->pcdata->learned[sn] < 1))) /* skill is not known */
	{
	    return FALSE;
	}
	else
		return TRUE;
}



void add_hero_exp( CHAR_DATA *gch, CHAR_DATA *victim, int total_levels )
{
    int xp,base_exp;
	char buf[MSL];

	base_exp = number_range(250,600);
	xp = base_exp;
	if( xp <= 0)
	{
		xp = 0;
	}

	xp += (victim->max_hit - gch->max_hit)/gch->max_hit;

    /* adjust for grouping */
    xp = xp * gch->level/( UMAX(1,total_levels -1) );
	if (gch->level == LEVEL_HERO)
	{
		sprintf(buf,"You add %d exp to your Hero's Exp.\n\r",xp);
		send_to_char(buf,gch);
		gch->pcdata->hero_exp += xp;
	}
}

bool check_precision ( CHAR_DATA *ch, CHAR_DATA *victim, sh_int gsn_called)
{
	char buf[MSL];
	char gsn_buf[MSL];
	int precision;
	int block_type;

/*	switch (gsn_called)
	{
	case gsn_dodge:	sprintf(gsn_buf,"dodge");	break;
	case (gsn_parry):	sprintf(gsn_buf,"parry");	break;
	case (gsn_counter):	sprintf(gsn_buf,"counter");	break;
	case (gsn_phase):	sprintf(gsn_buf,"phase");	break;
		break;
	}*/

	if (gsn_called == gsn_dodge)
		sprintf(gsn_buf,"dodge");
	if (gsn_called == gsn_parry)
		sprintf(gsn_buf,"parry");
	if (gsn_called == gsn_counter)
		sprintf(gsn_buf,"counter");
	if (gsn_called == gsn_phase)
		sprintf(gsn_buf,"phase");
	if (gsn_called == gsn_shield_block)
		sprintf(gsn_buf,"shield block");
/*If PC, set precision equal to char's precision skill*/
	if (!IS_NPC(ch))
        precision = get_skill(ch,gsn_precision);
/*
 *If mob, with hitroll >= 1000, set precision to 2*hitroll/100, giving mobs
 *the ability to use precision
 */
	else if (ch->hitroll >= 500)
		precision = (2 * ch->hitroll / 10);
	else 
		precision = 0;

	block_type = get_skill(ch,gsn_called);

	if (precision * number_range(1, 4) > block_type)
	{
		sprintf(buf,"{WYou land a blow through $N's %s{x",gsn_buf);
		act(buf,ch,NULL,victim,TO_CHAR);
		sprintf(buf,"{W$n lands a blow through your %s{x",gsn_buf);
		act(buf,ch,NULL,victim,TO_VICT);
		check_improve(ch,gsn_precision,TRUE,9);
		return TRUE;
	}
	return FALSE;
}


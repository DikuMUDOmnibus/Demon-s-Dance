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

/*Dagashi Skill File*/
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


void do_burst_of_speed( CHAR_DATA *ch, char *argument )
{
	int skill;
	int percent;
    AFFECT_DATA af;
 
	if (!know_skill(ch,"burst of speed"))
	{
		stc("Your class cannot mentally increase their speed\n\r",ch);
		return;
	}

    if ((skill = get_skill(ch,gsn_burst_of_speed)) < 1)
    {
	send_to_char("You don't have nearly enough training to mentally increase your speed\n\r",ch);
	return;
    }

    if ( is_affected( ch, gsn_burst_of_speed ))
    {
		send_to_char("If you pumped any more adrenaline into your system, you would die.\n\r",ch);
		return;		
    }

	percent = number_percent();
	if (percent < skill)
	{
		af.where     = TO_AFFECTS2;
		af.type      = gsn_burst_of_speed;
		af.level     = ch->level;
		af.duration  = 1;
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_BURST_OF_SPEED;
		affect_to_char( ch, &af );
		send_to_char( "Adreneline pumps through your system as you begin moving at superhuman speeds\n\r", ch );
		act("$n's movements begin to blur as his burst of speed takes affect.",ch,NULL,NULL,TO_ROOM);
		return;
	}
}

void do_coat(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int percent,skill;

    /* find out what */
    if (argument[0] == '\0')
    {
		send_to_char("Coat what weapon with poison?\n\r",ch);
		return;
    }

    obj =  get_obj_list(ch,argument,ch->carrying);

    if (obj== NULL)
    {
		send_to_char("You don't have that weapon\n\r",ch);
		return;
    }

    if ((skill = get_skill(ch,gsn_coat)) < 1)
    {
		send_to_char("Are you crazy? You'd poison yourself!\n\r",ch);
		return;
    }

    if (obj->item_type == ITEM_WEAPON)
    {
        if (IS_WEAPON_STAT(obj,WEAPON_DAGASHI))
        {
            act("$p has already been coated with lethal poisons",ch,obj,NULL,TO_CHAR);
            return;
        }

		percent = number_percent();
		if (percent < skill)
		{
 
			af.where     = TO_WEAPON;
			af.type      = gsn_coat;
			af.level     = ch->level * percent / 100;
			af.duration  = ch->level/2 * percent / 100;
			af.location  = 0;
			af.modifier  = 0;
			af.bitvector = WEAPON_DAGASHI;
			affect_to_obj(obj,&af);


			act("$n coats $p with a lethal poison, using ancient {BD{ra{Bg{ra{Bs{rh{Bi{x techniques",ch,obj,NULL,TO_ROOM);
			act("You coat $p with lethal poisons, using ancient {BD{ra{Bg{ra{Bs{rh{Bi{x techniques",ch,obj,NULL,TO_CHAR);
			check_improve(ch,gsn_coat,TRUE,3);
			WAIT_STATE(ch,skill_table[gsn_coat].beats);
			return;
		}
		else
		{
			act("You fail to envenom $p.",ch,obj,NULL,TO_CHAR);
			check_improve(ch,gsn_coat,FALSE,3);
			WAIT_STATE(ch,skill_table[gsn_coat].beats);
			return;
		}
    }
 
    act("You can't poison $p.",ch,obj,NULL,TO_CHAR);
    return;
}

void do_circle( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
	int chance;
 
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
	if (!know_skill(ch,"circle"))
		return;
	
	if (check_blind(victim))
	{
		stc("{RThey'd see you sneaking up from a mile away, best to blind them first{x",ch);
		return;
	}
 
    if (IS_NPC(victim) 
		&& victim->fighting != NULL
		&& !is_same_group(ch,victim->fighting) 
		&& !is_arena(victim))
 
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }
 
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
        send_to_char( "You need to wield a weapon to circle.\n\r", ch );
        return;
    }
 
    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char( "You must be fighting in order to circle.\n\r", ch );
        return;
    }
 
    check_killer( ch, victim );
	WAIT_STATE( ch, skill_table[gsn_circle].beats );
    if ( number_percent( ) < get_skill(ch,gsn_circle)
    || ( get_skill(ch,gsn_circle) >= 2 && !IS_AWAKE(victim) ) )
    {
        check_improve(ch,gsn_circle,TRUE,1);
        check_dual_attack(ch,victim, gsn_circle);
		if (is_affected(ch,gsn_burst_of_speed))
		{
			chance = ((ch->level*2)/3);
			if ( number_percent( ) < chance )
			{
				act("{W$n's speed is too much for you, as $e circles you once again!{x",ch,NULL,victim,TO_VICT);
				act("{W$n's speed is too much for $N{W, as $e circles you once again{x",ch,NULL,victim,TO_NOTVICT);
				act("{WYour speed is too much for $N{W, and you are able to circle $M once again!{x",ch,NULL,victim,TO_CHAR);
				check_dual_attack(ch,victim,gsn_circle);
				check_improve(ch,gsn_burst_of_speed,TRUE,6);
			}
		}
    }
    else
    {
        check_improve(ch,gsn_circle,FALSE,1);
        damage( ch, victim, 0, gsn_circle,DAM_NONE,TRUE,FALSE);
    }
 
    return;
}

void do_gouge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;
	AFFECT_DATA af;

    one_argument(argument,arg);

    if ((chance = get_skill(ch,gsn_gouge)) == 0
		|| (!IS_NPC(ch)
		&& ch->level < skill_table[gsn_gouge].skill_level[ch->class]))
    {
		send_to_char("You suck at gouging, but nice try.\n\r",ch);
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
	{
		stc("Not on that target.\n\r",ch);
		return;
	}

    if (IS_NPC(victim) 
		&& victim->fighting != NULL 
		&& !is_same_group(ch,victim->fighting)
		&& !is_arena(victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) 
		&& ch->master == victim)
    {
		act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
		return;
    }

	act("$n lunges forward, $s clawed hands gouging at $N's eyes!",ch,NULL,victim,TO_NOTVICT);
	act("$n lunges forward, $s clawed hands gouging at your eyes!",ch,NULL,victim,TO_VICT);
	act("You lunge forward, rage burning in your mind, as you rip ferociously at $N's eyes!",ch,NULL,victim,TO_CHAR);

	af.where		= TO_AFFECTS2;
	af.type 		= gsn_gouge;
	af.level 		= ch->level;
	af.duration		= 3;
	af.modifier		= -20;
	af.bitvector	= AFF_GOUGE;

	af.location		= APPLY_HITROLL;
	affect_to_char(victim,&af);

	af.location		= APPLY_DAMROLL;
	affect_to_char(victim,&af);

	chance = number_range(1,5) * ch->level;
	one_hit(ch,victim,gsn_gouge,FALSE,FALSE);
	check_improve(ch,gsn_gouge,TRUE,2);
	WAIT_STATE(ch,skill_table[gsn_gouge].beats);
	check_killer(ch,victim);
}
void form_mantis_dance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj;
	OBJ_DATA *dual;
	char weapons_held_char[MSL];
	char weapons_held_victim[MSL];
	char buf[MSL];

	obj = get_eq_char( ch, WEAR_WIELD );
	dual = get_eq_char( ch, WEAR_SECONDARY );
    if ( obj == NULL 
		|| obj->value[0] != 2
		|| dual == NULL
		|| dual->value[0] != 2)
    {
	send_to_char( "You must be wielding 2 daggers to perform the {GM{ga{Dnt{gi{Gs {DD{wa{Gn{wc{De{x\n\r", ch );
	return;
    }

	sprintf(weapons_held_char,"your %s and your %s",obj->short_descr,dual->short_descr);
	sprintf(buf,"Twirling %s into an overhand grip, you prepare to slam them into $N's flesh!",weapons_held_char);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(weapons_held_victim,"$s %s and %s",obj->short_descr,dual->short_descr);
	sprintf(buf,"$n suddenly shifts %s, $s body launching towards $N, deadly intent obvious in $s lightning fast lunge",weapons_held_victim);
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"$n suddenly shifts %s, $s body launches towards you, deadly intent obvious in $s lightning fast lunge",weapons_held_victim);
	act(buf,ch,NULL,victim,TO_VICT);
	

	act("Stepping into the {GM{ga{Dnt{gi{Gs {DD{wa{Gn{wc{Ge{x form, your blades slam into $N's body, shearing flesh and bone",ch,obj,victim,TO_CHAR);
	act("$n flows into the {GM{ga{Dnt{gi{Gs {DD{wa{Gn{wc{Ge{x form, $s blades shearing through your skin to rip into your internal organs",ch,obj,victim,TO_VICT);
	act("$n flows into the {GM{ga{Dnt{gi{Gs {DD{wa{Gn{wc{Ge{x form, the epitome of deadly grace, as $s blades bite deep in $N's body",ch,obj,victim,TO_ROOM);
	sn = skill_lookup( "mantis dance");
    if ( number_percent( ) < get_skill(ch,sn))
    {
		check_dual_attack(ch,victim,sn);
    }
    else
    {
		damage( ch, victim, 0, sn,DAM_NONE,TRUE,FALSE);
    }
    return;
}


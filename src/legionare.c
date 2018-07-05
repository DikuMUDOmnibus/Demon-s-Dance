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
*	Legionare Skill File
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

void do_strike( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
 
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
	if (!know_skill(ch,"strike"))
		return;
 
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
        send_to_char( "You must wield a weapon to strike your enemy.\n\r", ch );
        return;
    }
 
    check_killer( ch, victim );
	WAIT_STATE( ch, skill_table[gsn_strike].beats );
    if ( number_percent( ) < get_skill(ch,gsn_strike)
    || ( get_skill(ch,gsn_strike) >= 2 && !IS_AWAKE(victim) ) )
    {
        check_improve(ch,gsn_strike,TRUE,1);
		check_dual_attack(ch,victim, gsn_strike);
	}
    else
    {
        check_improve(ch,gsn_strike,FALSE,1);
        damage( ch, victim, 0, gsn_strike,DAM_NONE,TRUE,FALSE);
    }
 
    return;
}

void do_phalanx( CHAR_DATA *ch, char *argument)
{
	int chance, hp_percent;

    if ((chance = get_skill(ch,gsn_phalanx)) == 0)
    {
	send_to_char("You are unable to join the phalanx formation, and you hang your head in shame\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_CALM))
    {
	send_to_char("You're feeling to mellow to move int a phalanx.\n\r",ch);
	return;
    }

	if ( is_affected(ch,gsn_phalanx) )
	{
		send_to_char("You're already in the Phalanx formation dummy.",ch);
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
	WAIT_STATE(ch,skill_table[gsn_phalanx].beats);
	ch->mana -= 50;

	/* heal a little damage */
	ch->hit += ch->level * 2;
	ch->hit = UMIN(ch->hit,ch->max_hit);

	send_to_char("You shift into the Phalanx position, ready for attack\n\r",ch);
	act("$n moves into a phalanx position, readying for attack",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_phalanx,TRUE,2);

	af.where	= TO_AFFECTS2;
	af.type		= gsn_phalanx;
	af.level	= ch->level;
	af.duration	= number_fuzzy(ch->level / 30);
	af.modifier	= UMAX(1, ch->level / 1.5);
	af.bitvector = AFF_PHALANX;

	af.location	= APPLY_HITROLL;
	affect_to_char(ch,&af);

	af.location	= APPLY_DAMROLL;
	affect_to_char(ch,&af);

	af.modifier	= (-1 * UMAX(10,15 * (ch->level/3)));
	af.location	= APPLY_AC;
	affect_to_char(ch,&af);
    }

    else
    {
	WAIT_STATE(ch,skill_table[gsn_phalanx].beats);
	ch->mana -= 25;

	send_to_char("Your movements are to slow, and the phalanx moves off without you\n\r",ch);
	check_improve(ch,gsn_phalanx,FALSE,2);
    }
}


void do_shield_wall ( CHAR_DATA *ch, char *argument)
{
	int chance;

    AFFECT_DATA af;

    if ( IS_AFFECTED2(ch, AFF_SHIELD_WALL) )
    {
	send_to_char("You are already protected by your shield\n\r",ch);
	return;
    }

	if ( (get_eq_char (ch,WEAR_SHIELD)) == NULL)
	{
		send_to_char("You can't very well hide behind something you aren't using now can you\n\r",ch);
		return;
	}

	if ((chance = get_skill(ch,gsn_shield_wall)) == 0
    ||  (!IS_NPC(ch)
    &&   ch->level < skill_table[gsn_shield_wall].skill_level[ch->class]))
    {
	send_to_char("You can't hide behind your shield because you are to oafish.\n\r",ch);
	return;
    }

    if (ch->position < POS_STANDING)
	{
		send_to_char("How do you expect to hide behind your shield when you're sleeping?\n\r", ch);
		return;
	}
	/*set the shieldwall flag*/
	af.where     = TO_AFFECTS2;
	af.type		 = gsn_shield_wall;
    af.level     = ch->level;
    af.duration  = number_fuzzy( ch->level / 30 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SHIELD_WALL;
	affect_to_char(ch,&af);

	/*negaive hit and dam affect*/
	af.modifier	= (UMAX(1, ch->level / 2))*-1;
	af.bitvector 	= AFF_SHIELD_WALL;

	af.location	= APPLY_HITROLL;
	affect_to_char(ch,&af);

	af.location	= APPLY_DAMROLL;
	affect_to_char(ch,&af);
    act( "$n hunkers down behind $s shield", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You sucsessfully protect your entire body behind your shield\n\r", ch );
	check_improve(ch,gsn_shield_wall,TRUE,2);
    return;
}


void do_emerge ( CHAR_DATA *ch, char *argument)
{

    if ( !is_affected(ch,gsn_shield_wall) || !(IS_AFFECTED2(ch, AFF_SHIELD_WALL)))
	{
	send_to_char("You moron, you arent hiding behind a shield right now\n\r", ch);
	return;
	}

	else
	{
	affect_strip(ch,gsn_shield_wall);
	REMOVE_BIT(ch->affected2_by,AFF_SHIELD_WALL);
	send_to_char("You emerge from behind your shield, ready once again to battle at your fullest potential\n\r",ch);
	act( "$n pops out from behind $s shield, ready to fight you man to man\n\r", ch, NULL, NULL, TO_ROOM );
	return;
	}
}


void do_fortify ( CHAR_DATA *ch, char *argument)
{
	int chance;
	OBJ_DATA *fort;
	char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

	argument = one_arg_caps( argument, arg1 );
    argument = one_arg_caps( argument, arg2 );

	if ((chance = get_skill(ch,gsn_fortify)) == 0
    ||  (!IS_NPC(ch)
    &&   ch->level < skill_table[gsn_fortify].skill_level[ch->class]))
    {
	send_to_char("You are totally lost when it comes to building fortifications\n\r",ch);
	return;
    }
	
	send_to_char("You begin building a protective wooden fort\n\r", ch);
	act("$n begins to build a wooden fort\n\r", ch, NULL, NULL, TO_ROOM );
	check_improve(ch,gsn_fortify,TRUE,2);
	WAIT_STATE(ch,skill_table[gsn_fortify].beats);

	if ((arg1[0] == '\0') || (arg2[0] == '\0'))
	{
	fort = create_object(get_obj_index(OBJ_VNUM_FORT), 0);
	}

	else /*customize*/ 
	{
	WAIT_STATE(ch,skill_table[gsn_fortify].beats);
	fort = create_object(get_obj_index(OBJ_VNUM_FORT), 0);
	sprintf( buf, arg1);
    free_string( fort->short_descr );
    fort->short_descr = str_dup( buf ); /*Set Short Desc*/
    sprintf( buf, arg2 );
    free_string( fort->description );
    fort->description = str_dup( buf ); /*Set Long Desc*/
	}/*done customizing*/
    
	obj_to_room( fort, ch->in_room );
	check_improve(ch,gsn_fortify,TRUE,2);
	return;
}


void do_enter_fort ( CHAR_DATA *ch, char *argument)
{
	OBJ_DATA *fort;
	char arg[MAX_INPUT_LENGTH];
	AFFECT_DATA af;

	one_argument(argument,arg);
	fort = get_obj_list( ch, argument,  ch->in_room->contents );

	if (arg[0] == '\0')
    {
	send_to_char("Which fort would you like to enter?",ch);
	return;
	}

	if (fort == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}

	if ( IS_AFFECTED2(ch, AFF_FORTIFY) )
    {
	send_to_char("You are already in a fort you bastard\n\r",ch);
	return;
    }

/*charcater enters a fort*/
	if (fort->item_type == ITEM_FORT)
	{
		af.where     = TO_AFFECTS2;
		af.type		 = gsn_fortify;
		af.level     = ch->level;
		af.duration  = number_fuzzy( ch->level / 5 );
		af.location  = APPLY_NONE;
		af.modifier  = 0;
		af.bitvector = AFF_FORTIFY;
		affect_to_char(ch,&af);
		
		/*hit & dam affects*/
		af.modifier	= (UMAX(1, ch->level / 2));
		af.bitvector 	= AFF_FORTIFY;
		af.location	= APPLY_HITROLL;
		affect_to_char(ch,&af);
		af.location	= APPLY_DAMROLL;
		affect_to_char(ch,&af);

		act( "{W$n enters the fortification{x", ch, NULL, NULL, TO_ROOM );
		send_to_char( "{WYou enter the fort, prepared to defend it with your life{x\n\r", ch );
		return;
	}
}

void do_exit_fort ( CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	

	if(!is_affected(ch,gsn_fortify))
	{
		send_to_char("You aren't in a fortified position at the moment.\n\r",ch);
		return;
	}
	
	else
	{
	affect_strip(ch,gsn_fortify);
	REMOVE_BIT(ch->affected2_by,AFF_FORTIFY);
	send_to_char("{WYou exit the fort, once again able to move freely{x\n\r",ch);
	act( "{W$n exits the fort $e was hiding in{x\n\r", ch, NULL, NULL, TO_ROOM );
	free_string( ch->long_descr );
	sprintf(buf, "%s is here.\n\r", ch->name);
	ch->long_descr = str_dup(buf);
	return;
	}
}


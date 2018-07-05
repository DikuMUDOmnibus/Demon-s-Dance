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
 *	Zealot Skill File
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
#include "magic.h"


void do_smite( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
 
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

	else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
    send_to_char("They aren't here.\n\r",ch);
    return;
    }
 
    if ( is_safe( ch, victim ) )
      return;
	if (!know_skill(ch,"smite"))
		return;
 
    if (IS_NPC(victim) 
		&& victim->fighting != NULL 
		&& !is_same_group(ch,victim->fighting) 
		&& !is_arena(victim))
    {
    send_to_char("Kill stealing is not permitted.\n\r",ch);
    return;
    }
 
    check_killer( ch, victim );
	WAIT_STATE( ch, skill_table[gsn_smite].beats );
    if ( number_percent( ) < get_skill(ch,gsn_smite)
    || ( get_skill(ch,gsn_smite) >= 2 && !IS_AWAKE(victim) ) )
    {
        check_improve(ch,gsn_smite,TRUE,1);
        one_hit( ch, victim, gsn_smite, FALSE,FALSE );
    }
    else
    {
        check_improve(ch,gsn_smite,FALSE,1);
        damage( ch, victim, 0, gsn_smite,DAM_NONE,TRUE,FALSE);
    }
 
    return;
}

void spell_holy_armor(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (victim != ch)
   {
	   stc("You may only encase yourself in a set of {Dh{wo{Wl{Yy ar{Wm{wo{Dr{x\r\n",ch);
	   return;
   }

   if (IS_AFFECTED2(victim, AFF_HOLY_ARMOR))
   {
      if (victim == ch)
         send_to_char("You are already encased in {Dh{wo{Wl{Yy ar{Wm{wo{Dr{x\r\n", ch);
      else
         act("$N is already encased in {Dh{wo{Wl{Yy ar{Wm{wo{Dr{x",ch,NULL,victim,TO_CHAR);
      return;
   }
   
   af.where 	= TO_AFFECTS2;
   af.type      = sn;
   af.level     = level;
   af.duration  = level/6;
   af.location  = APPLY_AC;
   af.modifier  = -200;
   af.bitvector = AFF_HOLY_ARMOR;

   affect_to_char(victim, &af);
   act("$n is encased in {Dh{wo{Wl{Yy ar{Wm{wo{Dr{x",victim, NULL,NULL,TO_ROOM);
   send_to_char("You are encased in {Dh{wo{Wl{Yy ar{Wm{wo{Dr{x\n\r", victim);  
   return;

}


void spell_divine_right( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

	if (victim != ch)
	{
		stc("You can only give yourself the divine right to combat\n\r",ch);
		return;
	}

    if ( is_affected( victim, sn ) )
    {
		send_to_char("You already have the divine right to combat\n\r",ch);
		return;
    }

	if (IS_AFFECTED2(victim, AFF_DIVINE_RIGHT))
	{
		send_to_char("You already have the divine right to combat\n\r",ch);
		return;
	}

    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/2;
    af.location  = APPLY_HIT;
    af.modifier  = level * 5;
    af.bitvector = AFF_DIVINE_RIGHT;
    affect_to_char( ch, &af );
    send_to_char( "Your god has given you the right to combat\n\r", ch );
    act("$n has recieved the right to combat from $s god.",ch,NULL,NULL,TO_ROOM);
    return;
}

void spell_greater_healing( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    victim->hit = UMIN( victim->hit + 700, victim->max_hit );
    update_pos( victim );
    send_to_char( "The powers of greater healing knit your wounds\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_fanatic_flame( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;

    /* deal with the object case first */
	if (target == TARGET_OBJ)
    {
		obj = (OBJ_DATA *) vo;
		if (obj->item_type != ITEM_WEAPON)
		{
			act("$p is not a weapon, and cannot be given the fanatic's flame",ch,obj,NULL,TO_CHAR);
			return;
		}

		if (IS_WEAPON_STAT(obj,WEAPON_FLAMING))
		{
			act("$p already is a flaming weapon",ch,obj,NULL,TO_CHAR);
			return;
		}

		
		af.where	= TO_WEAPON;
		af.type		= sn;
		af.level	= ch->level;
		af.duration	= 40 + level;
		af.location	= 0;
		af.modifier	= 0;
		af.bitvector  = WEAPON_FLAMING;
		affect_to_obj(obj,&af);

		act("$p lights up with a {rf{Ra{yn{Ya{Wt{Yi{yc{Ra{rl {rf{yl{Ya{ym{re{x",ch,obj,NULL,TO_ALL);
		return;
	}
	else
		return;
}

void spell_heavenly_justice (int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
 
    if (victim != ch)
    {
        act("$n raises $s hand, and a thunder clap is heard as $e calls upon Heaven's Justice",
            ch,NULL,NULL,TO_ROOM);
        send_to_char(
	   "You raise your hand and a thunder clap echos as you call upon Heaven's Justice\n\r",
	   ch);
    }

    dam = number_range(ch->level, ch->level/2 * 100);
    if ( saves_spell( level, victim,DAM_HOLY) )
        dam /= 2;

    damage( ch, victim, dam, sn, DAM_HOLY ,TRUE,FALSE);
    spell_blindness(gsn_blindness, 3 * level / 4, ch, (void *) victim,TARGET_CHAR);
}
void spell_judgement( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

	if (IS_GOOD(ch))
	{
		act( "$n {Wcalls upon the strength of the heavens{x.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "You call upon the strength of the heavens{x.\n\r", ch );
		dam		= (ch->level * 4) + dice(25, 35);
		damage( ch, victim, dam, sn, DAM_HOLY ,TRUE,FALSE);
	}

	else if (IS_EVIL(ch))
	{
		act( "$n {Wcalls upon the powers of darkness{x.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "You call upon the powers of darkness{x.\n\r", ch );
		dam		= (ch->level * 4) + dice(25, 35);
		damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE,FALSE);
	}

	else if (IS_NEUTRAL(ch))
	{
		act( "$n {Wcalls upon the forces of balance{x.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "You call upon the forces of balance{x.\n\r", ch );
		dam		= (ch->level * 4) + dice(10, 35);
		damage( ch, victim, dam, sn, DAM_NONE ,TRUE, FALSE);
	}
}

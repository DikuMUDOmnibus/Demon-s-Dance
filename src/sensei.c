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
 *	Sensei Skill File
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

void do_haduken( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
	CHAR_DATA *sch;
 
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

	if ((victim = get_char_room(ch,NULL,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
 
    if ( is_safe( ch, victim ) )
      return;

	if (!know_skill(ch,"haduken"))
		return;
 
    if (IS_NPC(victim) &&
         victim->fighting != NULL &&
        !is_same_group(ch,victim->fighting) && !is_arena(victim))
 
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }
 
    check_killer( ch, victim );
	WAIT_STATE( ch, skill_table[gsn_haduken].beats );
    if ( number_percent( ) < (get_skill(ch,gsn_haduken) + ch->level)
    || ( get_skill(ch,gsn_haduken) >= 2 && !IS_AWAKE(victim) ) )
    {
        check_improve(ch,gsn_haduken,TRUE,1);
		one_hit( ch, victim, gsn_haduken, FALSE,FALSE );
		for ( sch = ch->in_room->people; sch; sch = sch->next_in_room )
		{
			send_sound(sch,0,"sounds/hadoken.wav",100,100);
		}
    }
    else
    {
        check_improve(ch,gsn_haduken,FALSE,1);
        damage( ch, victim, 0, gsn_haduken,DAM_NONE,TRUE,FALSE);
    }
 
    return;
}

void do_triple_kick( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *pChar;
   CHAR_DATA *pChar_next;
   bool found = FALSE;

   if (    !IS_NPC( ch ) 
        && ch->pcdata->learned[gsn_triple_kick] <= 0 )
   {
      send_to_char( "You don't know how to do that...\n\r", ch );
      return;
   }
   
   WAIT_STATE( ch, skill_table[gsn_triple_kick].beats );
   act( "{W$n jumps into the air, and launches a rapid barage of snap-kicks!{x", ch, NULL, NULL, TO_ROOM );
   act( "{WYou jump into the air and launch a rapid barage of snap-kicks!{x",  ch, NULL, NULL, TO_CHAR );
   
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
			act( "$n kicks out at YOU!", ch, NULL, pChar, TO_VICT    );
			one_hit( ch, pChar, gsn_triple_kick, FALSE,FALSE );
			one_hit( ch, pChar, gsn_triple_kick, FALSE,FALSE);
			one_hit( ch, pChar, gsn_triple_kick, FALSE,FALSE );
			check_improve(ch,gsn_triple_kick,FALSE,1);
		}
   }
   
   if ( !found )
   {
      act( "$n has no one to attack with his patented Triple Kick move", ch, NULL, NULL, TO_ROOM );
      act( "You land on the ground, having no one to attack with your Triple Kick move", ch, NULL, NULL, TO_CHAR );
   }
        
   if ( !found && number_percent() < 25 )
   {
      act( "$n loses $s balance and falls into a heap.",  ch, NULL, NULL, TO_ROOM );
      act( "You lose your balance and fall into a heap.", ch, NULL, NULL, TO_CHAR );
      ch->position = POS_STUNNED;
   }
   
   return;
}

void do_shadow_slip( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    send_to_char( "You attempt to slide into the shadows\n\r", ch );
    affect_strip( ch, gsn_shadow_slip );

    if (IS_AFFECTED2(ch,AFF_SHADOW_SLIP))
	return;

    if ( number_percent( ) <= get_skill(ch,gsn_shadow_slip))
    {
	send_to_char( "{WYour attempt was succsessful!{x\n\r", ch);
	check_improve(ch,gsn_shadow_slip,TRUE,3);
	af.where     = TO_AFFECTS2;
	af.type      = gsn_shadow_slip;
	af.level     = ch->level; 
	af.duration  = ch->level/50;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SHADOW_SLIP;
	affect_to_char( ch, &af );
    }
    else
	check_improve(ch,gsn_shadow_slip,FALSE,3);

    return;
}

void do_fatality( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
	int chance;
	int percent;
	int skill;
	int vnum;
	int addchance;

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
	if (!know_skill(ch,"fatality"))
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
        send_to_char( "You must be fighting in order to enact a fatality.\n\r", ch );
        return;
    }

    if(victim == ch)
    {
	send_to_char("Self-Fatalities may be fun in your mind, but not for the rest of us\n\r",ch);
	return;
    }

	percent = 10 - (( 100 * victim->hit ) / victim->max_hit);
	skill = get_skill(ch,gsn_fatality)/10;
	addchance = (number_percent( ))/10;
	chance = ((100 * (skill + percent)) + addchance);
	if ( number_percent( ) < chance )
	{
		check_killer( ch, victim );
		WAIT_STATE( ch, skill_table[gsn_fatality].beats );
		if ( number_percent( ) < get_skill(ch,gsn_fatality)
		|| ( get_skill(ch,gsn_fatality) >= 2 && !IS_AWAKE(victim) ) )
		{
			act("{W$n calls upon their chi, and prepares to extinguish your life{x",ch,NULL,victim,TO_VICT);
			act("{W$ncalls upon their chi, and prepares to extinguish $N's life{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou call upon your chi, and prepare to finish $N off.{x\n\r",ch,NULL,victim,TO_CHAR);
		/*Do the neat death messages*/
			switch ( number_range(0,9))
			{
			case  0:
			{
			act("{W$n rips off your {carm{W, shoves it down your {Dthroat{W, and uses it to constrict your {rheart{W until you {ydie{W!{x",ch,NULL,victim,TO_VICT);
			act("{W$n rips off $N's {carm{W, shoves it down $N's {Dthroat{W, and uses it to constrict $N{W's {rheart{W until $E {ydies{W!{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou rip off $N's {carm{W, shove it down $S {Dthroat{W, and use it to constrict $S {rheart{W until $E {ydies{W!{x\n\r",ch,NULL,victim,TO_CHAR);
			}
			break;
			case  1: 
			{
			act("{W$n rips off $s mask and spits a {Gs{ct{gr{Ge{ca{Gm {co{gf {Ga{cc{gi{Gd{W at you, melting you into a {Gp{Yu{Gd{Yd{Gl{Ye {Go{Yf {Gg{Yo{Go{W!{x",ch,NULL,victim,TO_VICT);
			act("{W$n rips off $s mask and spits a {Gs{ct{gr{Ge{ca{Gm {co{gf {Ga{cc{gi{Gd{W at $N{W, melting $M into a {Gp{Yu{Gd{Yd{Gl{Ye {Go{Yf {Gg{Yo{Go{W!{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou rip off your mask and spit a {Gs{ct{gr{Ge{ca{Gm {co{gf {Ga{cc{gi{Gd{W at $N{W, melting $M into a {Gp{Yu{Gd{Yd{Gl{Ye {Go{Yf {Gg{Yo{Go{W!{x\n\r",ch,NULL,victim,TO_CHAR);
			act("{WBrought to you by {GReptile{D({WTM){x",ch,NULL,NULL,TO_ROOM);
			act("{WBrought to you by {GReptile{D({WTM){x",ch,NULL,NULL,TO_CHAR);
			}
			break;
			case 2:
			{
			act("{W$n tears off $s mask and breathes a {Rb{rl{Ya{Rs{rt {Yo{Rf {rf{Yi{Rr{re {Won you, burning you to a {yc{Yr{yi{Ys{yp{W!{x",ch,NULL,victim,TO_VICT);
			act("{W$n tears off $s mask and breathes a {Rb{rl{Ya{Rs{rt {Yo{Rf {rf{Yi{Rr{re {Won $N{W, burning $M to a {yc{Yr{yi{Ys{yp{W!{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou tear off your mask and breath a {Rb{rl{Ya{Rs{rt {Yo{Rf {rf{Yi{Rr{re {Won $N{W, burning $M to a {yc{Yr{yi{Ys{yp{W!{x\n\r",ch,NULL,victim,TO_CHAR);
			act("{WBrought to you by {RScorpion{D({WTM){x",ch,NULL,NULL,TO_ROOM);
			act("{WBrought to you by {RScorpion{D({WTM){x",ch,NULL,NULL,TO_CHAR);
			}
			break;
			case 3: 							
			{
			act("{W$n slams $s hands onto either side of your head, squashing it like a {Mgrape {Wand sending {wb{Do{wn{De {wf{Dr{wa{Dg{wm{De{wn{Dt{ws {Weverywhere!{x",ch,NULL,victim,TO_VICT);
			act("{W$n slams $s hands onto either side of $N{W's head, squashing it like a {Mgrape {Wand sending {wb{Do{wn{De {wf{Dr{wa{Dg{wm{De{wn{Dt{ws {Weverywhere!{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou slam your hands onto either side of $N{W's head, squashing it like a {Mgrape {Wand sending {wb{Do{wn{De {wf{Dr{wa{Dg{wm{De{wn{Dt{ws {Weverywhere!{x\n\r",ch,NULL,victim,TO_CHAR);
			vnum = OBJ_VNUM_FRAGMENTS;
			}
			break;
			case  4:
			{
			act("{W$n plunges $s hands into your back, ripping your {Ds{Wp{wi{Wn{De {Wout in a shower of {rb{Rl{ro{Ro{rd{W!{x",ch,NULL,victim,TO_VICT);
			act("{W$n plunges $s hands into $N{W's back, ripping $N{W's {Ds{Wp{wi{Wn{De {Wout in a shower of {rb{Rl{ro{Ro{rd{W!{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou plunge your hand into $N{W's back, ripping $S {Ds{Wp{wi{Wn{De {Wout in a shower of {rb{Rl{ro{Ro{rd{W!{x\n\r",ch,NULL,victim,TO_CHAR);
			act("{WBrought to you by {CSub{B-{CZero{D({WTM){X",ch,NULL,NULL,TO_ROOM);
			vnum = OBJ_VNUM_SPINE;				
			}
			break;
			case  5: 
			{
			act("{WWith a well-placed jab, $n thrusts $s fist into your chest and yank your {ystill{D-{ybeating {Rh{re{Ra{rr{Rt {Wfrom within{W!{x",ch,NULL,victim,TO_VICT);
			act("{WWith a well-placed jab, $n thrusts $s fist into $N{W's chest and yank $N's {ystill{D-{ybeating {Rh{re{Ra{rr{Rt {Wfrom within{W!{x",ch,NULL,victim,TO_NOTVICT);
			act("{WWith a well-placed jab, you thrust your fist into $N{W's chest and yank $S {ystill{D-{ybeating {Rh{re{Ra{rr{Rt {Wfrom within{W!{x\n\r",ch,NULL,victim,TO_CHAR);
			vnum = OBJ_VNUM_TORN_HEART;				
			}	
			break;
			case  6: 
			{
			act("{W$n rams two fingers {Gi{cn{Gs{ci{Gd{ce{W your throat, {Yb{Rl{Ya{Rs{Yt{Ri{Yn{Rg{W your neck violently apart{W!{x",ch,NULL,victim,TO_VICT);
			act("{W$n rams two fingers {Gi{cn{Gs{ci{Gd{ce{W $N{W's throat, {Yb{Rl{Ya{Rs{Yt{Ri{Yn{Rg{W $N's neck violently apart{W!{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou ram two fingers {Gi{cn{Gs{ci{Gd{ce{W $N{W's throat, {Yb{Rl{Ya{Rs{Yt{Ri{Yn{Rg{W $S neck violently apart{W!{x\n\r",ch,NULL,victim,TO_CHAR);
			vnum = OBJ_VNUM_THROAT;				
			}
			break;
			case  7: 
			{
			act("{W$n grabs you around the waist, and {Ms{Rq{Mu{Re{Me{Rz{Me{W.......until your head {Re{Yx{rp{Rl{Yo{rd{Re{Ys{W!{x",ch,NULL,victim,TO_VICT);
			act("{W$n grabs $N{W around the waist, and {Ms{Rq{Mu{Re{Me{Rz{Me{W.......until $N{W's head {Re{Yx{rp{Rl{Yo{rd{Re{Ys{W!{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou grab $N{W around the waist, and {Ms{Rq{Mu{Re{Me{Rz{Me{W.......until $S head {Re{Yx{rp{Rl{Yo{rd{Re{Ys{W!{x\n\r",ch,NULL,victim,TO_CHAR);
			}
			break;
			case 8:
			{
			act("{W$n {Rtears{W your {ms{rt{mo{rm{ma{rc{mh{W open with $s hands, allowing your {Gi{Cn{Gt{Ce{Gs{Ct{Gi{Cn{Ge{Cs{W to spill onto the ground!{x",ch,NULL,victim,TO_VICT);
			act("{W$n {Rtear{W $N{W's {ms{rt{mo{rm{ma{rc{mh{W open with $s hands, allowing $N's {Gi{Cn{Gt{Ce{Gs{Ct{Gi{Cn{Ge{Cs{W to spill onto the ground!{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou {Rtear{W $N{W's {ms{rt{mo{rm{ma{rc{mh{W open with your hands, allowing $S {Gi{Cn{Gt{Ce{Gs{Ct{Gi{Cn{Ge{Cs{W to spill onto the ground!{x\n\r",ch,NULL,victim,TO_CHAR);
			}
			break;
			case 9:
			{
			act("{W$n stares intently at your chest.  As $n focuses $s energies, your chest {Re{wx{Yp{Rl{wo{Yd{Re{ws {Woutwards, spewing {Rb{rl{Ro{ro{Rd{ry {Wchunks of your {Gr{gu{Gp{gt{Gu{gr{Ge{gd {Gs{gp{Gl{ge{Ge{gn{W.{x",ch,NULL,victim,TO_VICT);
			act("{W$n stares intently at $N{W's chest.  As $n focuses $s energies, $N{W's chest {Re{wx{Yp{Rl{wo{Yd{Re{ws {Woutwards, spewing {Rb{rl{Ro{ro{Rd{ry {Wchunks of $S {Gr{gu{Gp{gt{Gu{gr{Ge{gd {Gs{gp{Gl{ge{Ge{gn{W.{x",ch,NULL,victim,TO_NOTVICT);
			act("{WYou stare intently at $N{W's chest.  As you focus your energies, $S chest {Re{wx{Yp{Rl{wo{Yd{Re{ws {Woutwards, spewing {Rb{rl{Ro{ro{Rd{ry {Wchunks of $S {Gr{gu{Gp{gt{Gu{gr{Ge{gd {Gs{gp{Gl{ge{Ge{gn{W.{x\n\r",ch,NULL,victim,TO_CHAR);
			}
			break;
			}
		/*Create the body piece
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
				obj_to_room( obj, ch->in_room );
				}*/
		/*Do the damage*/
			check_improve(ch,gsn_fatality,TRUE,1);
			one_hit( ch, victim, gsn_fatality, TRUE,FALSE );
			add_fatality(ch,victim);
			WAIT_STATE( ch, skill_table[gsn_fatality].beats );
		}
		WAIT_STATE( ch, skill_table[gsn_fatality].beats/2 );
		send_to_char("{WYou were about to crush them, but your form was off at the last moment, and you missed!{x\n\r", ch);
		check_improve(ch,gsn_fatality,FALSE,1);
        damage( ch, victim, 0, gsn_fatality,DAM_NONE,TRUE,FALSE);

	}
	else
    {
        WAIT_STATE( ch, skill_table[gsn_fatality].beats );
		send_to_char("{WYou weren't able to snuff their life just yet, hurt them some more first.{x\n\r", ch);
		check_improve(ch,gsn_fatality,FALSE,1);
        damage( ch, victim, 0, gsn_fatality,DAM_NONE,TRUE,FALSE);
    }
}
void form_deadly_fingertips( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj;
	OBJ_DATA *dual;
	char buf[MSL];

	obj = get_eq_char( ch, WEAR_WIELD );
	dual = get_eq_char( ch, WEAR_SECONDARY );
    if ( obj != NULL || dual != NULL)
    {
		stop_fighting( ch, TRUE );
		stc( "The Deadly Fingertips attack can only be performed unarmed\n\r", ch );
		return;
    }

	sprintf(buf,"Gathering your chi, your body flows into the Deadly Fingertips form, your{x\n\r  hands flash towards $N, fingertips glowing a fierce red{x");
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"$n's eyes begin to flash a vivid turqoise, $s body tenses,\n\r  and uncoils in a blast of violent action, streaking towards $N{x");
	act(buf,ch,NULL,victim,TO_NOTVICT);
	sprintf(buf,"$n's eyes begin to flash a vivid turqoise, $s body tenses,\n\r  and suddenly $s glowing red fingertips streak towards your vital organs{x");
	act(buf,ch,NULL,victim,TO_VICT);
	

	act("Your fingertips pulse with arcane energy as they blast through $N's\n\r  armor and skin like paper, wreaking horrible carnage on $S body",ch,NULL,victim,TO_CHAR);
	act("$n's glowing fingertips embed themselves in $N, wrenching a\n\r  scream of agony from $N's lips as $S lifeblood flows from the eight wounds",ch,obj,victim,TO_VICT);
	act("$n's glowing fingertips tear through your armor and skin, blasting\n\r  jagged holes in your body, and ripping your innards asunder!",ch,obj,victim,TO_ROOM);
	sn = skill_lookup( "deadly fingertips");
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


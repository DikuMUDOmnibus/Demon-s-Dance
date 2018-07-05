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
#include <time.h>
#elif defined(WIN32)
#include <sys/types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "magic.h"

void do_heal(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
    int cost,sn;
    SPELL_FUN *spell;
    char *words;	

    /* check for healer */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act, ACT_IS_HEALER) )
            break;
    }
 
    if ( mob == NULL )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }

    one_argument(argument,arg);

    if (arg[0] == '\0')
    {
        /* display price list */
	act("$N says 'I offer the following spells:'",ch,NULL,mob,TO_CHAR);
	send_to_char("  {Glight{x			{Y10 {Wgold{x\n\r",ch);
	send_to_char("  {Gserious{x		{Y15 {Wgold{x\n\r",ch);
	send_to_char("  {Gcritical{x		{Y25 {Wgold{x\n\r",ch);
	send_to_char("  {Gheal{x			{Y50 {Wgold{x\n\r",ch);
	send_to_char("  {Gblind{x			{Y20 {Wgold{x\n\r",ch);
	send_to_char("  {Gdisease{x		{Y15 {Wgold{x\n\r",ch);
	send_to_char("  {Gpoison{x		{Y25 {Wgold{x\n\r",ch); 
	send_to_char("  {Guncurse{x		{Y50 {Wgold{x\n\r",ch);
	send_to_char("  {Grefresh{x		{Y5  {Wgold{x\n\r",ch);
	send_to_char("  {Gmana{x			{Y10 {Wgold{x\n\r",ch);
	send_to_char("	{Gsanctuary{x		{Y10 {Wgold{x\n\r",ch);
	send_to_char("  {Gprecover{x		{Y10 {Wgold{x\n\r",ch);
	send_to_char("  {Gerecover{x		{Y10 {Wgold{x\n\r",ch);
	send_to_char("  {Gmrecover{x		{Y10 {Wgold{x\n\r",ch);
	send_to_char(" {CType {w'{Gheal {D<{Btype{D>{w' {Cto be healed.{x\n\r",ch);
	return;
    }

    if (!str_prefix(arg,"light"))
    {
        spell = spell_cure_light;
	sn    = skill_lookup("cure light");
	words = "{cjudicandus dies{x";
	 cost  = 1000;
    }

    else if (!str_prefix(arg,"serious"))
    {
	spell = spell_cure_serious;
	sn    = skill_lookup("cure serious");
	words = "{cjudicandus gzfuajg{x";
	cost  = 1600;
    }

    else if (!str_prefix(arg,"critical"))
    {
	spell = spell_cure_critical;
	sn    = skill_lookup("cure critical");
	words = "{cjudicandus qfuhuqar{x";
	cost  = 2500;
    }

    else if (!str_prefix(arg,"heal"))
    {
	spell = spell_heal;
	sn = skill_lookup("heal");
	words = "{cpzar{x";
	cost  = 5000;
    }

    else if (!str_prefix(arg,"blindness"))
    {
	spell = spell_cure_blindness;
	sn    = skill_lookup("cure blindness");
      	words = "{cjudicandus noselacri{x";		
        cost  = 2000;
    }

    else if (!str_prefix(arg,"disease"))
    {
	spell = spell_cure_disease;
	sn    = skill_lookup("cure disease");
	words = "{cjudicandus eugzagz{x";
	cost = 1500;
    }

    else if (!str_prefix(arg,"poison"))
    {
	spell = spell_cure_poison;
	sn    = skill_lookup("cure poison");
	words = "{cjudicandus sausabru{x";
	cost  = 2500;
    }
	
    else if (!str_prefix(arg,"uncurse") || !str_prefix(arg,"curse"))
    {
	spell = spell_remove_curse; 
	sn    = skill_lookup("remove curse");
	words = "{ccandussido judifgz{x";
	cost  = 5000;
    }

    else if (!str_prefix(arg,"mana") || !str_prefix(arg,"energize"))
    {
        spell = NULL;
        sn = -1;
        words = "{cenergizer{x";
        cost = 1000;
    }

	
    else if (!str_prefix(arg,"refresh") || !str_prefix(arg,"moves"))
    {
	spell =  spell_refresh;
	sn    = skill_lookup("refresh");
	words = "{candusima{x"; 
	cost  = 500;
    }

    else if (!str_prefix(arg,"precover") || !str_prefix(arg,"fullhp"))
    {
	spell =  spell_physical_recover;
	sn    = skill_lookup("precover");
	words = "{cfullhp{x"; 
	cost  = 1000;
    }

    else if (!str_prefix(arg,"erecover") || !str_prefix(arg,"fullmoves"))
    {
	spell =  spell_energy_recover;
	sn    = skill_lookup("erecover");
	words = "{cfullmov{x"; 
	cost  = 1000;
    }

    else if (!str_prefix(arg,"mrecover") || !str_prefix(arg,"fullmana"))
    {
	spell =  spell_mental_recover;
	sn    = skill_lookup("mrecover");
	words = "{cfullmana{x"; 
	cost  = 1000;
    }

	else if (!str_prefix(arg,"sanctuary"))
    {
        spell = spell_sanctuary;
	sn    = skill_lookup("sanctuary");
	words = "{cgaiqhjabral{x";
	 cost  = 1000;
    }

    else 
    {
	act("$N says 'Type '{cheal{x' for a list of spells.'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    if (cost > (ch->gold * 100 + ch->silver))
    {
	act("$N says 'You do not have enough gold for my services.'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    WAIT_STATE(ch,PULSE_VIOLENCE);

    deduct_cost(ch,cost);
    mob->gold += cost / 100;
    mob->silver += cost % 100;
    act("$n utters the words {R'$T{R'{x.",mob,NULL,words,TO_ROOM);
  
    if (spell == NULL)  /* restore mana trap...kinda hackish */
    {
	ch->mana += dice(2,8) + mob->level / 3;
	ch->mana = UMIN(ch->mana,ch->max_mana);
	send_to_char("A warm glow passes through you.\n\r",ch);
	return;
     }

     if (sn == -1)
	return;
    
     spell(sn,mob->level,mob,ch,TARGET_CHAR);
}

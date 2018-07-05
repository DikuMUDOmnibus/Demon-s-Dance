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

/*TaoMaster File*/

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
#include "math.h"
#include "skill_tree.h"

void spell_vise_grip( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;

	if ( IS_AFFECTED2(victim, AFF_VISE_GRIP) )
	{
		if (victim == ch)
			send_to_char("You already have a vise grip on your weapon\n\r",ch);
		else
			act("$N already has a vise grip on their weapon",ch,NULL,victim,TO_CHAR);
		return;
	}

	af.where     = TO_AFFECTS2;
	af.type      = sn;
	af.level     = level;
	af.duration  = level / 6;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_VISE_GRIP;
	affect_to_char( victim, &af );
	act( "$n grips his weapon until his knuckles turn white{x.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "You you take a deathgrip on your weapon{x.\n\r", victim );
	return;
}

void spell_elemental_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	short skill_add;
	char buf[MSL];

	if ( IS_AFFECTED2(victim, AFF_ELEMENT) )
	{
		if (victim == ch)
			send_to_char("You are already protected by a shield of the elements\n\r",ch);
		else
			act("$N is already protected by an elemental shield",ch,NULL,victim,TO_CHAR);
		return;
	}
	skill_add = sqrt(ch->pcdata->skills_array[match_skill("arcane.protection.magical","elemental shield")]);

	sprintf(buf,"skill_add = %d",skill_add);
	stc(buf,ch);

	af.where     = TO_AFFECTS2;
	af.type      = sn;
	af.level     = level;
	af.duration  = level / 6;
	af.location  = VRES_COLD;
	af.modifier  = ch->level/15 + skill_add;
	af.bitvector = AFF_ELEMENT;
	affect_to_char( victim, &af );

	af.location	= VRES_FIRE;
	affect_to_char(victim,&af);

	af.location	= VRES_ACID;
	affect_to_char(victim,&af);

	af.location	= VRES_LIGHTNING;
	affect_to_char(victim,&af);

	do_say(ch,"Pagu ba zan fa, pagu ba ZAN FA!");
	act( "$n is covered in a transparent rainbow{x.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "You are surrounded by a transparent rainbow{x.\n\r", victim );
	return;
}

void spell_hold_person( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	char buf[MSL];

	if (victim == ch)
	{
		stc("You cant cast this spell on yourself",ch);
		return;
	}

	if ( IS_AFFECTED2(victim, AFF_HOLD_PERSON) )
	{
		sprintf(buf,"%s is already held to the ground",victim->name);
		stc(buf,ch);
		check_killer(victim,ch);
		multi_hit(victim,ch,TYPE_UNDEFINED);
		return;
	}

	af.where     = TO_AFFECTS2;
	af.type      = sn;
	af.level     = level;
	af.duration  = 0;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_HOLD_PERSON;
	affect_to_char( victim, &af );
	act( "$n is held to the ground as if stuck there{x.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Your legs feel like 18 tons of lead, and you cant move!!{x.\n\r", victim );
	check_killer(victim,ch);
	multi_hit(victim,ch,TYPE_UNDEFINED);
	return;
}

void spell_giga_blast( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	int dam;

	act( "$n chants in a low voice, and you feel the energies of {CT{Ba{Co {Gm{Ma{Gg{Mi{Gc{x coalescing around you{x.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You chant the ancient rite to gather your {CT{Ba{Co {Gm{Ma{Gg{Mi{Gc{x, and prepare to release a giga blast{x.\n\r", ch );
	do_say(ch,"Pagu ba zan fa, pagu ba ZAN FA!");
	dam		= number_range( ch->level * 15, ch->level * 30 );
	damage( ch, victim, dam, sn, DAM_FIRE ,TRUE,FALSE);
	return;
}

void spell_supernova( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
	CHAR_DATA *pChar,*pChar_next = NULL;
	int dam,chance,attacks,mana_loss,dam_gain;
	bool found = FALSE;
	bool has_hit_char = FALSE;

	act( "$n begins channeling the energy of the cosmos with his {CT{Ba{Co {Gm{Ma{Gg{Mi{Gc{x", ch, NULL, NULL, TO_ROOM );
	act( "You begin channeling the energy of the cosmos with your {CT{Ba{Co {Gm{Ma{Gg{Mi{Gc{x",  ch, NULL, NULL, TO_CHAR );
	do_say(ch,"Pagu ba zan fa, pagu ba ZAN FA!");

	for ( pChar = ch->in_room->people; pChar; pChar = pChar_next )
	{
		pChar_next = pChar->next_in_room;
		/* debug(ch,"is_safe(%s,%s)=%d\n\r",
		ch->short_descr,
		pChar->short_descr,
		is_safe(ch,pChar));*/
		if(is_safe(ch,pChar))
			continue;
		else
		{
			found = TRUE;
			act( "$n's supernova burns into your body!", ch, NULL, pChar, TO_VICT    );
			attacks = 0;
			chance = number_range(1,3);
			while( attacks <= chance)
			{
				dam = number_range( level * 4, level * 5 );
				dam_gain = return_bonus_sn(ch,sn) / number_range(3,10);
				debug(ch,"spell_supernova: dam_gain=%d\n\r",dam_gain);
				dam += dam_gain;
				damage( ch, pChar, dam, sn, DAM_LIGHT ,TRUE,FALSE);
				mana_loss = (dam/10) - (return_bonus_sn(ch,sn)/100);
				debug(ch,"spell_supernova: mana_loss=%d\n\r",mana_loss);
				ch->mana -= mana_loss;
				attacks++;
			}
		}
	}
}

void spell_repulsar( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	short skill_add;
	char buf[MSL];

	if (victim != ch)
	{
		stc("You may only give yourself a repulsion wave, no one else",ch);
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		send_to_char("You are already protected by a wave of repulsion\n\r",ch);
		return;
	}

	if (IS_AFFECTED2(victim, AFF_REPULSAR))
	{
		send_to_char("You are already protected by a wave of repulsion\n\r",ch);
		return;
	}

	act( "$n chants in a low voice, and you feel the energies of {CT{Ba{Co {Gm{Ma{Gg{Mi{Gc{x coalescing around $m{x.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "You chant the ancient rite to gather your {CT{Ba{Co {Gm{Ma{Gg{Mi{Gc{x, and begin to weave a repulsion wave{x.\n\r", ch );
	do_say(ch,"Pagu ba zan fa, pagu ba ZAN FA!");

	skill_add = sqrt(ch->pcdata->skills_array[match_skill("arcane.protection.physical","elemental shield")]);
	sprintf(buf,"skill_add = %d",skill_add);
	stc(buf,ch);
	af.where     = TO_AFFECTS2;
	af.type      = sn;
	af.level     = level;
	af.duration  = level / 6;
	af.location  = VRES_SLASH;
	af.modifier  = ch->level/15 + skill_add;
	af.bitvector = AFF_REPULSAR;
	affect_to_char( victim, &af );

	af.location	= VRES_BASH;
	affect_to_char(victim,&af);

	af.location	= VRES_PIERCE;
	affect_to_char(victim,&af);

	act( "$n is covered by a rippling pulsar wave", victim, NULL, NULL, TO_ROOM );
	send_to_char( "You covered by a rippling pulsar wave.\n\r", victim );
	return;
}


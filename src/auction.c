/***************************************************************************
 *  This file contains auction code developed by Brian Babey, and any      *
 *  communication regarding it should be sent to [bbabey@iname.com]        *
 *  Web Address: http://www.erols.com/bribe/                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
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
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"

void show_obj_stats( CHAR_DATA *ch, OBJ_DATA *obj );
void auction_channel( char * msg );

void auction_update( )
{
    char buf[MAX_STRING_LENGTH];
	char temp1[MAX_STRING_LENGTH];

    if ( auction_info.item == NULL )
	return;

    auction_info.status++;

    if ( auction_info.status == AUCTION_LENGTH )
    {
	if ( auction_info.current_bid == 0 )
	{
	    sprintf(buf, "No bids on {W'{c%s{W'{c - item removed.\n\r",
		auction_info.item->short_descr);
	    auction_channel( buf );

	    obj_to_char( auction_info.item, auction_info.owner );

		sprintf(buf, "%s is returned to you.\n\r",
			auction_info.item->short_descr );
		send_to_char( buf, auction_info.owner );
	
	    auction_info.item           = NULL;
	    auction_info.owner          = NULL;
	    auction_info.current_bid    = 0;
	    auction_info.status         = 0;

	    return;
	}
	sprintf(buf,"{W'{c%s{W' {cSOLD to {C%s {cfor {Y%ld {cgold.{x\n\r",
	    auction_info.item->short_descr,
	    auction_info.high_bidder->name,
	    auction_info.current_bid );
	auction_channel( buf );

	auction_info.owner->gold += auction_info.gold_held;

	sprintf(temp1, "{Y%ld gold", auction_info.gold_held );
	sprintf(buf, "You receive %s {xcoins.\n\r",temp1);
	send_to_char( buf, auction_info.owner );
		
	obj_to_char( auction_info.item, auction_info.high_bidder );

	sprintf(buf, "%s appears in your hands.\n\r",
		auction_info.item->short_descr );
	send_to_char( buf, auction_info.high_bidder );
	
	auction_info.item		= NULL;
	auction_info.owner		= NULL;
	auction_info.high_bidder	= NULL;
	auction_info.current_bid	= 0;
	auction_info.status		= 0;
	auction_info.gold_held		= 0;

	return;
    }

    if ( auction_info.status == AUCTION_LENGTH - 1 )
    {
	sprintf(buf, "{W'{c%s{W' {c- going twice for {Y%ld {cgold.{x\n\r",
		auction_info.item->short_descr,
		auction_info.current_bid );
	auction_channel( buf );
	return;
    }

    if ( auction_info.status == AUCTION_LENGTH - 2 )
    {
        sprintf(buf, "{W'{c%s{W' {c- going once for {Y%ld {cgold.{x\n\r",
                auction_info.item->short_descr,
                auction_info.current_bid );
        auction_channel( buf );
        return;
    }

    return;

}

void auction_channel( char * msg )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    sprintf(buf, "\n\r{D[{CAUCTION{D]{c %s", msg ); /* Add color if you wish */

      for ( d = descriptor_list; d != NULL; d = d->next )
      {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     !IS_SET(victim->comm,COMM_NOAUCTION) &&
	     !IS_SET(victim->comm,COMM_QUIET) )
	      {
		send_to_char( buf, victim );
	      }
      }

    return;
}

/*
 * Show_obj_stats: code taken from stock identify spell (-Brian)
 */
void show_obj_stats( CHAR_DATA *ch, OBJ_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;

    sprintf( buf,
	"{CObject '{W%s{C' is type {W%s{C, extra flags {W%s{C.\n\r{CWeight is {W%d{C, value is {Y%d{C, level is {W%d{C.{x\n\r",

	obj->name,
	item_name(obj->item_type),
	extra_bit_name( obj->extra_flags ),
	obj->weight / 10,
	obj->cost,
	obj->level
	);
    send_to_char( buf, ch );

    switch ( obj->item_type )
    {
    case ITEM_SCROLL: 
    case ITEM_POTION:
    case ITEM_PILL:
	sprintf( buf, "{CLevel {W%d{C spells of:{W", obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
	{
	    send_to_char( " '{x", ch );
	    send_to_char( skill_table[obj->value[1]].name, ch );
	    send_to_char( "'{x", ch );
	}

	if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
	{
	    send_to_char( " '{x", ch );
	    send_to_char( skill_table[obj->value[2]].name, ch );
	    send_to_char( "'{x", ch );
	}

	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '{x", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'{x", ch );
	}

	if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL)
	{
	    send_to_char(" '{x",ch);
	    send_to_char(skill_table[obj->value[4]].name,ch);
	    send_to_char("'{x",ch);
	}

	send_to_char( "{C.{x\n\r", ch );
	break;

    case ITEM_WAND: 
    case ITEM_STAFF: 
	sprintf( buf, "{CHas {W%d {Ccharges of level {W%d{x",
	    obj->value[2], obj->value[0] );
	send_to_char( buf, ch );
      
	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '{x", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'{x", ch );
	}

	send_to_char( "{C.{x\n\r", ch );
	break;

    case ITEM_DRINK_CON:
        sprintf(buf,"{CIt holds {W%s{C-colored {w%s.{x\n\r",
            liq_table[obj->value[2]].liq_color,
            liq_table[obj->value[2]].liq_name);
        send_to_char(buf,ch);
        break;

    case ITEM_CONTAINER:
	case ITEM_GOLEM_BAG:
	sprintf(buf,"{CCapacity: {W%d#  {CMaximum weight: {W%d#  {Cflags: {W%s{x\n\r",
	    obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
	send_to_char(buf,ch);
	if (obj->value[4] != 100)
	{
	    sprintf(buf,"{CWeight multiplier: {W%d%%{C\n\r",
		obj->value[4]);
	    send_to_char(buf,ch);
	}
	break;

    case ITEM_GOLEM_PART:
 	send_to_char("{CGolem Part is type {W",ch);
	switch (obj->value[0])
	{
	    case(GOLEM_HEART)	: send_to_char("heart{x.\n\r",ch);		break;
	    case(GOLEM_HEAD)	: send_to_char("shead{x.\n\r",ch);		break;	
	    case(GOLEM_LEG)		: send_to_char("arm{x.\n\r",ch);		break;
	    case(GOLEM_ARM)		: send_to_char("leg{x.\n\r",ch);		break;
	    default				: send_to_char("unknown{x.\n\r",ch);	break;
 	}
	break;

    case ITEM_WEAPON:
 	send_to_char("{CWeapon type is {W",ch);
	switch (obj->value[0])
	{
	    case(WEAPON_EXOTIC) : send_to_char("exotic{x.\n\r",ch);	break;
	    case(WEAPON_SWORD)  : send_to_char("sword{x.\n\r",ch);	break;	
	    case(WEAPON_DAGGER) : send_to_char("dagger{x.\n\r",ch);	break;
	    case(WEAPON_SPEAR)	: send_to_char("spear/staff{x.\n\r",ch);	break;
	    case(WEAPON_MACE) 	: send_to_char("mace/club{x.\n\r",ch);	break;
	    case(WEAPON_AXE)	: send_to_char("axe{x.\n\r",ch);		break;
	    case(WEAPON_FLAIL)	: send_to_char("flail{x.\n\r",ch);	break;
	    case(WEAPON_WHIP)	: send_to_char("whip{x.\n\r",ch);		break;
	    case(WEAPON_POLEARM): send_to_char("polearm{x.\n\r",ch);	break;
		case(WEAPON_BOW)	: send_to_char("bow{x.\n\r",ch);		break;
	    default		: send_to_char("unknown{x.\n\r",ch);	break;
 	}
	if (obj->pIndexData->new_format)
	    sprintf(buf,"{CDamage is {W%dd%d {D({Cavg {W%d{D){x.\n\r",
		obj->value[1],obj->value[2],
		(1 + obj->value[2]) * obj->value[1] / 2);
	else
	    sprintf( buf, "{CDamage is {W%d {Cto {W%d {D({Cavg {W%d{D){x.\n\r",
	    	obj->value[1], obj->value[2],
	    	( obj->value[1] + obj->value[2] ) / 2 );
	send_to_char( buf, ch );
        if (obj->value[4])  /* weapon flags */
        {
            sprintf(buf,"{CWeapons flags: {W%s{x\n\r",weapon_bit_name(obj->value[4]));
            send_to_char(buf,ch);
        }
		if (obj->pIndexData->weapon_spell)
		{
			sprintf(buf,"{WSpellcast: %s\n\r{x",
				skill_table[obj->pIndexData->weapon_spell].name);
			stc(buf,ch);
		}
	break;

    case ITEM_ARMOR:
	sprintf( buf, 
	"{CArmor class is {W%d {Cpierce, {W%d {Cbash, {W%d {Cslash, and {W%d{C magic{x.\n\r", 
	    obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	send_to_char( buf, ch );
	break;
    }

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "{CAffects {W%s {Cby {W%d.{x\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char(buf,ch);
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                    case TO_AFFECTS:
                        sprintf(buf,"{CAdds {W%s {Caffect{c.\n",
                            affect_bit_name(paf->bitvector));
                        break;
                    case TO_OBJECT:
                        sprintf(buf,"{CAdds {W%s {Cobject flag{x.\n",
                            extra_bit_name(paf->bitvector));
                        break;
                    case TO_IMMUNE:
                        sprintf(buf,"{CAdds immunity to {W%s{x.\n",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_RESIST:
                        sprintf(buf,"{CAdds resistance to {W%s{x.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_VULN:
                        sprintf(buf,"{CAdds vulnerability to {W%s{x.\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    default:
                        sprintf(buf,"{RUnknown bit {W%d{R: {W%d{x\n\r",
                            paf->where,paf->bitvector);
                        break;
                }
	        send_to_char( buf, ch );
	    }
	}
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "{CAffects {W%s {Cby {W%d{x",
	    	affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
            if ( paf->duration > -1)
                sprintf(buf,"{C, {W%d {Chours.{x\n\r",paf->duration);
            else
                sprintf(buf,".{x\n\r");
	    send_to_char(buf,ch);
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                    case TO_AFFECTS:
                        sprintf(buf,"{CAdds {W%s {Caffect.{x\n",
                            affect_bit_name(paf->bitvector));
                        break;
                    case TO_OBJECT:
                        sprintf(buf,"{CAdds {W%s {Cobject flag.{x\n",
                            extra_bit_name(paf->bitvector));
                        break;
		    case TO_WEAPON:
			sprintf(buf,"{CAdds {W%s {Cweapon flags.{x\n",
			    weapon_bit_name(paf->bitvector));
			break;
                    case TO_IMMUNE:
                        sprintf(buf,"{CAdds immunity to {W%s.{x\n",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_RESIST:
                        sprintf(buf,"{CAdds resistance to {W%s.{x\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    case TO_VULN:
                        sprintf(buf,"{CAdds vulnerability to {W%s.{x\n\r",
                            imm_bit_name(paf->bitvector));
                        break;
                    default:
                        sprintf(buf,"{RUnknown bit {W%d{R: {W%d{x\n\r",
                            paf->where,paf->bitvector);
                        break;
                }
                send_to_char(buf,ch);
            }
	}
    }

    return;
}

void do_auction( CHAR_DATA *ch, char * argument )
{
    long gold;
    OBJ_DATA *	obj;
	CHAR_DATA * owner;
    char arg1[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );

    if ( ch == NULL || IS_NPC(ch) )
	return;

    if ( arg1[0] == '\0')
    {
	obj = auction_info.item;

	if ( !obj )
	{
	    send_to_char("{WThere is nothing up for auction right now.\n\r{x",ch);
	    return;
	}

	else 
	    show_obj_stats( ch, obj );

	return;
    }

    if ( !str_cmp( arg1, "info" ) )
    {
	obj = auction_info.item;

	if ( !obj )
	{
	    send_to_char("{WThere is nothing up for auction right now.{x\n\r",ch);
	    return;
	}

	else 
	    show_obj_stats( ch, obj );

	return;
    }

    if ( !str_cmp( arg1, "bid" ) )
    {
	long bid;
        obj = auction_info.item;
		owner = auction_info.owner;

        if ( !obj )
        {
            send_to_char("{WThere is nothing up for auction right now.{x\n\r",ch);
            return;
        }
	if (owner == ch)
	{
		send_to_char("{WBidding on your own item is illegal!{x\n\r",ch);
		return;
	}

	if ( argument[0] == '\0' )
	{
	    send_to_char("{WYou must enter an amount to bid.\n\r{x",ch);
	    return;
	}

	bid = atol( argument );

	if ( bid <= auction_info.current_bid )
	{
	    sprintf(buf, "{WYou must bid above the current bid of {Y%ld gold{W.{x\n\r",
		auction_info.current_bid );
	    return;
	}

	if ( bid < MINIMUM_BID )
	{
	    sprintf( buf, "{WThe minimum bid is {Y%d gold{W.{x\n\r",MINIMUM_BID);
	    send_to_char(buf,ch);
	    return;
	}

	if ( (ch->gold) < bid )
	{
	    send_to_char("{WYou can't cover that bid.\n\r",ch);
	    return;
	}

	sprintf(buf, "{cA bid of {Y%ld gold{c has been recieved on {W'{c%s{W'{x\n\r",bid,
		auction_info.item->short_descr);
	auction_channel( buf );
	
	if ( auction_info.high_bidder != NULL )
	{
	    auction_info.high_bidder->gold += auction_info.gold_held;
	}
	gold = UMIN( ch->gold, bid );

	if ( gold < bid )
	{
	    gold = ((bid - gold + 99 ) / 100 );
	}

	ch->gold -= gold;
	auction_info.gold_held		= gold;
	auction_info.high_bidder	= ch;
	auction_info.current_bid	= bid;
	auction_info.status	 	= 0;

	return;	
    }

    if ( auction_info.item != NULL )
    {
	send_to_char("There is already another item up for bid.\n\r",ch);
	return;
    }

    if ( (obj = get_obj_carry( ch, arg1, ch )) == NULL )
    {
	send_to_char("You aren't carrying that item.\n\r",ch);
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
    {
	send_to_char("You can't let go of that item.\n\r",ch);
	return;
    }

    auction_info.owner		= ch;
    auction_info.item		= obj;
    auction_info.current_bid	= 0;
    auction_info.status		= 0;

    sprintf(buf,"{W'{c%s{W'{c has been put up for auction.{x\n\r", obj->short_descr );
    auction_channel( buf );

    obj_from_char( obj );

    return;
}


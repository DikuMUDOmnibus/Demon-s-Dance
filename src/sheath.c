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


/*Sheath idea thanks to Blaze January 22, 2002*/
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

 
void do_sheath( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
	char arg3[MIL];
	char buf[MSL];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3	);
	
	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Put what in what?\n\r", ch );
	return;
    }

	if ( str_cmp( arg2, "in" ))
	{
		stc("Syntax: sheath <weapon> in <sheath>",ch);
		return;
	}

	if ( !str_cmp( arg1, "all" ) || !str_prefix( "all.", arg1 ) )
    {
		send_to_char( "You can't do that.\n\r", ch );
		return;
    }

    if ( !str_cmp( arg3, "all" ) || !str_prefix( "all.", arg3 ) )
    {
		send_to_char( "You can't do that.\n\r", ch );
		return;
    }

    if ( ( container = get_obj_here( ch, NULL, arg3 ) ) == NULL )
    {
		act( "I see no $T here.", ch, NULL, arg3, TO_CHAR );
		return;
    }

    if ( (container->item_type != ITEM_SHEATH) )
    {
		send_to_char( "That's not a sheath.\n\r", ch );
		return;
    }

	/* 'put obj container' */
	if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}

	if ( obj == container )
	{
	    send_to_char( "You can't fold it into itself.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}
	pObjIndex = get_obj_index(container->pIndexData->vnum);
	

/*	if (container->value[2] >= num_sheathed)
	{
		stc("{WThat sheath already has its maximum number of weapons in it{x\n\r",ch);
		return;
	}*/



	obj_from_char( obj );
	obj_to_obj( obj, container );
    act( "{W$n {Wsheaths $p {Win $P{W.{x", ch, obj, container, TO_ROOM );
    act( "{WYou sheath $p {Win $P{W.{x", ch, obj, container, TO_CHAR );
	if ((pObjIndex->vnum != 10011) && (pObjIndex->vnum != 10012))
	{
		sprintf(buf,"%s{x holding: %s",pObjIndex->orig_short,obj->short_descr);
	}
	else if ((pObjIndex->vnum == 10011) || (pObjIndex->vnum == 10012))
	{
		sprintf(buf,"%s{x holding: %s",container->orig_short,obj->short_descr);
	}
	free_string(container->short_descr);
	container->short_descr = str_dup(buf);
	return;
}

void do_pull( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
	char arg3[MIL];
	char buf[MSL];
    OBJ_DATA *container;
    OBJ_DATA *obj;
	OBJ_INDEX_DATA *pObjIndex;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3	);
	
	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Pull what from what?\n\r", ch );
	return;
    }

	if ( str_cmp( arg2, "from" ))
	{
		stc("Syntax: pull <weapon> from <sheath>",ch);
		return;
	}

	if ( !str_cmp( arg1, "all" ) || !str_prefix( "all.", arg1 ) )
    {
		send_to_char( "You can't do that.\n\r", ch );
		return;
    }

    if ( !str_cmp( arg3, "all" ) || !str_prefix( "all.", arg3 ) )
    {
		send_to_char( "You can't do that.\n\r", ch );
		return;
    }

    if ( ( container = get_obj_here( ch, NULL, arg3 ) ) == NULL )
    {
		act( "I see no $T here.", ch, NULL, arg3, TO_CHAR );
		return;
    }

    if ( (container->item_type != ITEM_SHEATH) )
    {
		send_to_char( "That's not a sheath.\n\r", ch );
		return;
    }

	obj = get_obj_list( ch, arg1, container->contains );
	if ( obj == NULL )
	{
		act( "I see nothing like that in the $T.",ch, NULL, arg3, TO_CHAR );
		return;
	}

	if ( obj == container )
	{
	    send_to_char( "You can't fold it into itself.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "You can't let go of it.\n\r", ch );
	    return;
	}

	pull_obj( ch, obj, container );
	pObjIndex = get_obj_index(container->pIndexData->vnum);
	if ((pObjIndex->vnum != 10011) && (pObjIndex->vnum != 10012))
	{
		sprintf(buf,"%s",pObjIndex->orig_short);
	}
	else if ((pObjIndex->vnum == 10011) || (pObjIndex->vnum == 10012))
	{
		sprintf(buf,"%s",container->orig_short);
	}
	free_string(container->short_descr);
	container->short_descr = str_dup(buf);
	return;
}

void pull_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	send_to_char( "You can't take that.\n\r", ch );
	return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	act( "$d: you can't carry that many items.",
	    ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if ((!obj->in_obj || obj->in_obj->carried_by != ch)
    &&  (get_carry_weight(ch) + get_obj_weight(obj) > can_carry_w(ch)))
    {
	act( "$d: you can't carry that much weight.",
	    ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if (!can_loot(ch,obj))
    {
	act("Corpse looting is not permitted.",ch,NULL,NULL,TO_CHAR );
	return;
    }

    if ( container != NULL )
    {
	    if ( (container->item_type != ITEM_SHEATH) )
	    {
			send_to_char( "That's not a sheath.\n\r", ch );
			return;
		}

    	if (container->pIndexData->vnum == OBJ_VNUM_PIT
		&&  !CAN_WEAR(container, ITEM_TAKE)
		&&  !IS_OBJ_STAT(obj,ITEM_HAD_TIMER))
			obj->timer = 0;	
		act( "{WYou slide $p {Wfrom its sheath, ready to do battle.{x", ch, obj, container, TO_CHAR );
		act( "{W$n {Wslides $p {Wfrom $P {Wwith a flourish, looking even more deadly now.{x", ch, obj, container, TO_ROOM );
		REMOVE_BIT(obj->extra_flags,ITEM_HAD_TIMER);
		obj_from_obj( obj );
    }
    else
    {
		act( "You get $p.", ch, obj, container, TO_CHAR );
		act( "$n gets $p.", ch, obj, container, TO_ROOM );
		obj_from_room( obj );
    }
	obj_to_char( obj, ch );
    return;
}


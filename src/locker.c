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
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "merc.h"
#include "interp.h"

void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,bool fShort, bool fShowNothing ) );

//Joe:Lockers	Check if theres a locker in the room and if so return the obj data
OBJ_DATA *get_obj_locker_room( CHAR_DATA *ch, sh_int vnum )
{
    OBJ_DATA *obj;
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
		if ( obj->pIndexData->vnum == 3034 )
			return obj;
		else
			return NULL;
	}
	return NULL;
}

/*
 * Find an obj in player's inventory without checking for invis blind etc.... Used for lockers
 */
OBJ_DATA *get_locker( CHAR_DATA *ch)
{
    OBJ_DATA *obj;
    
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
		if (obj->pIndexData->vnum==LOCKER_VNUM)
			return obj;
    }

    return NULL;
}


void do_locker (CHAR_DATA *ch,char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	OBJ_DATA *obj;
	OBJ_DATA *container;

	argument=one_argument(argument,arg1);
	argument=one_argument(argument,arg2);

	if ( ( container=get_locker(ch))==NULL)
	{
		if (!str_cmp(arg1,"buy"))
		{
			/* give player a box */
			obj = create_object( get_obj_index( LOCKER_VNUM ), 0 );
			obj_to_char(obj,ch);
			send_to_char("You buy a box",ch);
			return;
		}
		else
		{
			send_to_char("You already own a locker",ch);
			return;
		}
	}

	if(!str_cmp(arg1,"list") || ( arg1[0] == '\0'))
	{
		//do the locker list command
		send_to_char("Your locker contains:\n\r",ch);
		show_list_to_char(container->contains,ch,TRUE,TRUE);
		return;
	}
	
	if (ch->in_room->vnum!=LOCKER_ROOM)
	{
		send_to_char("Go the locker room eeeediot!!\n\r",ch);
		return;
	}


	if (!str_cmp(arg1,"get"))
	{
		//do the locker get command


	  /* 'get obj container' */
		obj = get_obj_list( ch, arg2, container->contains );
	   if ( obj == NULL )
	   {
			act( "Your locker doesn't contain that.",ch, NULL, arg2, TO_CHAR );
			return;
		}
	   get_obj( ch, obj, container );
	}
	else if(!str_cmp(arg1,"put") || !str_cmp(arg1,"put"))
	{
		//you guessed it, do the the locker put command
		
		//First get the object from inventory
		if ( ( obj = get_obj_carry( ch, arg2, ch ) ) == NULL )
		{
			send_to_char( "You do not have that item.\n\r", ch );
			return;
		}

		//Now check to see if meets conditions
		if ( obj->item_type==ITEM_CONTAINER || obj->item_type == ITEM_GOLEM_BAG)
		{
			send_to_char( "Containers are not allowed in lockers.\n\r", ch );
			return;
		}


		if ( !can_drop_obj( ch, obj ) )
		{
			send_to_char( "You can't let go of it.\n\r", ch );
			return;
		}

		//Put obj in locker
		obj_from_char( obj );
		obj_to_obj( obj, container );
		act( "$n puts $p in $s locker.", ch, obj, container, TO_ROOM );
	   act( "You put $p in your locker.", ch, obj, container, TO_CHAR );

	}
	else
	{
		//dumb fucks cant spell and need teaching :)
		send_to_char("Syntax:\nlocker store <object>\nlocker get <object>\nlocker list\n",ch);
	}
	return;	
}

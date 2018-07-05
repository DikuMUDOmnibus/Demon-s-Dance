From: Sardu <sardu@v-wave.com>

/*
This is forwarded from one of our coders on Aeon, it's a revision
 from his previous post to this list. He can be reached at
 feist@nortel.ca

                                               -- Sardu
*/



I have completed the set of OLC delete commands.
There are also some bug fixes for mem.c all of which are related
to for loops which have x->next as the increment condition but
reassign x->next during the body of the loop thus causing an
infinite loop. Compare your free_mob/room/obj_index functions
to the new ones. 

All the functions now free up memory and use the OLC memory
management scheme. All instances of whatever your deleting
are extracted from the mud. In the case of a room this
is includes any exits which lead to this room.

There are also a few handy utility functions that some of you
may find useful.

Please note that none of these functions prevent you from
deleting things which may be hardcoded into the src
as #defines such as OBJ_VNUM_SCHOOL_DAGGER etc...
Doing so would be too much hassle and I assume that the
person using the functions knows the code well enough
to realize these things.

Installation instructions and code follow. Note, I haven't 
actually tested these instructions; if you have significant
problems use the e-mail address at the bottom.

Enjoy,
Chris




--------------------- CUT HERE ----------------------------------

Delete Commands for Ivan's OLC

Usage and limitations:

The following code is provided "as is". I make no claims to
its suitability for a particular purpose. I am not responsible
if your machine spontaneously explodes while using this code.

Normally I frown on usage limitations however since this code
took a significant amount of coding and debugging to write I
would ask that if you use this you send a quick mail to
feist@nortel.ca saying you are using it. This provision becomes
void if this code ever makes it into the actual OLC dist.

Hack away. :)


To install the OLC delete commands into your mud follow the
following steps. 

Please note that on Aeon (telnet://mud.aeon.org:4000) the ^ character
is used for color. You may have to change some strings if your mud
doesen't use ANSI color or uses another character for the control
character.

1.	In mem.c replace free_mob/obj/room_index with the following
routines. (Or just compare the old and new versions. There are only
minor changes.)

void free_mob_index( MOB_INDEX_DATA *pMob )
{
	PROG_LIST *list, *mp_next;

	free_string( pMob->player_name );
	free_string( pMob->short_descr );
	free_string( pMob->long_descr );
	free_string( pMob->description );

	for( list = pMob->mprogs; list; list = mp_next )
	{
		mp_next = list->next;
		free_mprog( pMob->mprogs );
	}

	if( pMob->pShop )
	{
		free_shop( pMob->pShop );
	}

	pMob->next = mob_index_free;
	mob_index_free = pMob;
	return;
}

void free_obj_index( OBJ_INDEX_DATA *pObj )
{
	EXTRA_DESCR_DATA *pExtra, *wExtra;
	AFFECT_DATA *pAf, *wAf;

	free_string( pObj->name );
	free_string( pObj->short_descr );
	free_string( pObj->description );

	for ( pAf = pObj->affected; pAf; pAf = wAf )
	{
		wAf = pAf->next;
		free_affect( pAf );
	}

	for ( pExtra = pObj->extra_descr; pExtra; pExtra = wExtra )
	{
		wExtra = pExtra->next;
		free_extra_descr( pExtra );
	}

	pObj->next = obj_index_free;
	obj_index_free = pObj;
	return;
}


void free_room_index( ROOM_INDEX_DATA *pRoom )
{
	int door;
	EXTRA_DESCR_DATA *pExtra, *wExtra;
	RESET_DATA *pReset, *wReset;

	free_string( pRoom->name );
	free_string( pRoom->description );
	free_string( pRoom->owner );

	for ( door = 0; door < MAX_DIR; door++ )
	{
		if ( pRoom->exit[door] )
		{
			free_exit( pRoom->exit[door] );
			pRoom->exit[door] = NULL;
		}

	}

	for ( pExtra = pRoom->extra_descr; pExtra; pExtra = wExtra )
	{
		wExtra = pExtra->next;
		free_extra_descr( pExtra );
	}

	for ( pReset = pRoom->reset_first; pReset; pReset = wReset )
	{
		wReset = pReset->next;
		free_reset_data( pReset );
	}

	pRoom->next = room_index_free;
	room_index_free = pRoom;
	return;
}


2.	In olc.c add an entry in each of reidit/medit/oedit_table
for the new delete function.

(look for the RIGHT HERE)

Example:
	
const struct olc_cmd_type redit_table[] =
{
/*	{	command,	function		}, */
	{	"commands",	show_commands	},
	{	"create",	redit_create	},
	{	"desc",		redit_desc		},
	{	"ed",		redit_ed		},
	{	"format",	redit_format	},
	{	"name",		redit_name		},
	{	"show",		redit_show		},
	{	"heal",		redit_heal		},
	{	"mana",		redit_mana		},
	{	"clan",		redit_clan		},
	{	"north",	redit_north		},
	{	"south",	redit_south		},
	{	"east",		redit_east		},
	{	"west",		redit_west		},
	{	"up",		redit_up		},
	{	"down",		redit_down		},
	{	"copy",		redit_copy		},
	{	"delete",	redit_delete	},  /* <----RIGHT HERE */

    /* New reset commands. */
    
	{	"mreset",	redit_mreset	},
	{	"oreset",	redit_oreset	},
	{	"mlist",	redit_mlist		},
	{	"rlist",	redit_rlist		},
	{	"olist",	redit_olist		},
	{	"mshow",	redit_mshow		},
	{	"oshow",	redit_oshow		},
	{	"owner",	redit_owner		},
	{	"room",		redit_room		},
	{	"sector",	redit_sector	},
	{	"?",		show_help		},
	{	"version",	show_version	},
    {	NULL,		0				}
};

3.	In olc.c, in each of the functions do_redit/medit/oedit
add a hook for the new command such as the following example:

(look for the RIGHT HERE)

void do_redit( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *pRoom;
	char arg1[MAX_STRING_LENGTH];

	if ( IS_NPC(ch) )
		return;

	argument = one_argument( argument, arg1 );

	pRoom = ch->in_room;

	if ( !str_cmp( arg1, "reset" ) )
	{
		if ( !IS_BUILDER( ch, pRoom->area ) )
		{
			send_to_char( "Insufficient security to reset rooms.\n\r" , ch );
			return;
		}

		reset_room( pRoom, TRUE);
		send_to_char( "Room reset.\n\r", ch );
		return;
	}
	else if( !str_cmp( arg1, "delete" ) )
	{
		redit_delete( ch, argument ); /* <--- RIGHT HERE */
			return;
	}
	else if ( !str_cmp( arg1, "create" ) )
	{
		if ( ( argument[0] == '\0' || atoi( argument ) == 0 )
		 && str_cmp( argument, "next" ) )
		{
			send_to_char( "Syntax:  edit room create [vnum | next]\n\r", ch );
			return;
		}

		if ( redit_create( ch, argument ) )
		{
			ch->desc->editor = ED_ROOM;
			char_from_room( ch );
			char_to_room( ch, ch->desc->pEdit );
			SET_BIT( ((ROOM_INDEX_DATA *)ch->desc->pEdit)->area->area_flags,
			AREA_CHANGED );
		}

		return;
	}

(rest of function not included for brevity. note: my do_redit probably looks
slightly different then yours)

4.	In olc_act.c paste in the following functions:

OEDIT( oedit_delete )
{
	CHAR_DATA *wch, *ch_next;
	OBJ_DATA *obj, *obj_next;
	OBJ_INDEX_DATA *pObj;
	RESET_DATA *pReset, *wReset, *tReset;
	ROOM_INDEX_DATA *pRoom;
	char arg[MIL];
	char buf[MSL];
	int index, rcount, ocount, i, iHash;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Syntax:  oedit delete [vnum]\n\r", ch );
		return FALSE;
	}

	one_argument( argument, arg );

	if( is_number( arg ) )
	{
		index = atoi( arg );
		pObj = get_obj_index( index );
	}
	else
	{
		send_to_char( "That is not a number.\n\r", ch );
		return FALSE;
	}

	if( !pObj )
	{
		send_to_char( "No such object.\n\r", ch );
		return FALSE;
	}

	SET_BIT( pObj->area->area_flags, AREA_CHANGED );

	if( top_vnum_obj == index )
		for( i = 1; i < index; i++ )
			if( get_obj_index( i ) )
				top_vnum_obj = i;


	top_obj_index--;

	/* remove objects */
	ocount = 0;
	for( obj = object_list; obj; obj = obj_next )
	{
		obj_next = obj->next;

		if( obj->pIndexData == pObj )
		{
			extract_obj( obj );
			ocount++;
		}
	}

	/* crush resets */
	rcount = 0;
	for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
		for( pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next )
		{
			for( pReset = pRoom->reset_first; pReset; pReset = wReset )
			{
				wReset = pReset->next;
				switch( pReset->command )
				{
				case 'O':
				case 'E':
				case 'P':
				case 'G':
					if( ( pReset->arg1 == index ) ||
						( ( pReset->command == 'P' ) && (
					pReset->arg3 == index ) ) )
					{
						unlink_reset( pRoom, pReset );
						free_reset_data( pReset );

						rcount++;
						SET_BIT( pRoom->area->area_flags,
						AREA_CHANGED );

					}
				}
			}
		}
	}

	unlink_obj_index( pObj );

	pObj->area = NULL;
	pObj->vnum = 0;

	free_obj_index( pObj );

	sprintf( buf, "Removed object vnum ^C%d^x and"
		" ^C%d^x resets.\n\r", index,rcount );

	send_to_char( buf, ch );

	sprintf( buf, "^C%d^x occurences of the object"
		" were extracted from the mud.\n\r", ocount );

	send_to_char( buf, ch );

	return TRUE;
}


MEDIT( medit_delete )
{
	CHAR_DATA *wch, *wnext;
	MOB_INDEX_DATA *pMob;
	RESET_DATA *pReset, *wReset;
	ROOM_INDEX_DATA *pRoom;
	char arg[MIL];
	char buf[MSL];
	int index, mcount, rcount, iHash, i;
	bool foundmob = FALSE;
	bool foundobj = FALSE;

	if( argument[0] == '\0' )
	{
		send_to_char( "Syntax:  medit delete [vnum]\n\r", ch );
		return FALSE;
	}

	one_argument( argument, arg );

	if( is_number( arg ) )
	{
		index = atoi( arg );
		pMob = get_mob_index( index );
	}
	else
	{
		send_to_char( "That is not a number.\n\r", ch );
		return FALSE;
	}

	if( !pMob )
	{
		send_to_char( "No such mobile.\n\r", ch );
		return FALSE;
	}

	SET_BIT( pMob->area->area_flags, AREA_CHANGED );

	if( top_vnum_mob == index )
		for( i = 1; i < index; i++ )
			if( get_mob_index( i ) )
				top_vnum_mob = i;

	top_mob_index--;

	/* Now crush all resets and take out mobs while were at it */
	rcount = 0;
	mcount = 0;
	
	for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
		for( pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next )
		{

			for( wch = pRoom->people; wch; wch = wnext )
			{
				wnext = wch->next_in_room;
				if( wch->pIndexData == pMob )
				{
					extract_char( wch, TRUE );
					mcount++;
				}
			}

			for( pReset = pRoom->reset_first; pReset; pReset = wReset )
			{
				wReset = pReset->next;
				switch( pReset->command )
				{
				case 'M':
					if( pReset->arg1 == index )
					{
						foundmob = TRUE;

						unlink_reset( pRoom, pReset );
						free_reset_data( pReset );

						rcount++;
						SET_BIT( pRoom->area->area_flags,
						AREA_CHANGED );

					}
					else
						foundmob = FALSE;

					break;
				case 'E':
				case 'G':
					if( foundmob )
					{
						foundobj = TRUE;

						unlink_reset( pRoom, pReset );
						free_reset_data( pReset );

						rcount++;
						SET_BIT( pRoom->area->area_flags,
						AREA_CHANGED );

					}
					else
						foundobj = FALSE;

					break;
				case '0':
					foundobj = FALSE;
					break;
				case 'P':
					if( foundobj && foundmob )
					{
						unlink_reset( pRoom, pReset );
						free_reset_data( pReset );

						rcount++;
						SET_BIT( pRoom->area->area_flags,
						AREA_CHANGED );
					}
				}
			}
		}
	}

	unlink_mob_index( pMob );

	pMob->area = NULL;
	pMob->vnum = 0;

	free_mob_index( pMob );

	printf_to_char( ch, "Removed mobile vnum ^C%d^x and"
		" ^C%d^x resets.\n\r", index, rcount );
	printf_to_char( ch, "^C%d^x mobiles were extracted"
		" from the mud.\n\r",mcount );
	return TRUE;
}

REDIT( redit_delete )
{
	ROOM_INDEX_DATA *pRoom, *pRoom2;
	RESET_DATA *pReset;
	EXIT_DATA *ex;
	OBJ_DATA *Obj, *obj_next;
	CHAR_DATA *wch, *wnext;
	EXTRA_DESCR_DATA *pExtra;
	char arg[MIL];
	char buf[MSL];
	int index, i, iHash, rcount, ecount, mcount, ocount, edcount;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Syntax:  redit delete [vnum]\n\r", ch );
		return FALSE;
	}

	one_argument( argument, arg );

	if( is_number( arg ) )
	{
		index = atoi( arg );
		pRoom = get_room_index( index );
	}
	else
		{
		send_to_char( "That is not a number.\n\r", ch );
		return FALSE;
	}

	if( !pRoom )
	{
		send_to_char( "No such room.\n\r", ch );
		return FALSE;
	}

	/* Move the player out of the room. */
	if( ch->in_room->vnum == index )
	{
		send_to_char( "Moving you out of the room"
			" you are deleting.\n\r", ch);
		if( ch->fighting != NULL )
			stop_fighting( ch, TRUE );

		char_from_room( ch );
		char_to_room( ch, get_room_index( 3 ) ); /* limbo */
		ch->was_in_room = ch->in_room;
		ch->from_room = ch->in_room;
	}

	SET_BIT( pRoom->area->area_flags, AREA_CHANGED );

	/* Count resets. They are freed by free_room_index. */
	rcount = 0;

	for( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
	{
		rcount++;
	}

	/* Now contents */
	ocount = 0;
	for( Obj = pRoom->contents; Obj; Obj = obj_next )
	{
		obj_next = Obj->next_content;

		extract_obj( Obj );
		ocount++;
	}

	/* Now PCs and Mobs */
	mcount = 0;
	for( wch = pRoom->people; wch; wch = wnext )
	{
		wnext = wch->next_in_room;
		if( IS_NPC( wch ) )
		{
			extract_char( wch, TRUE );
			mcount++;
		}
		else
			{
			send_to_char( "This room is being deleted. Moving" 
				" you somewhere safe.\n\r", ch );
			if( wch->fighting != NULL )
				stop_fighting( wch, TRUE );

			char_from_room( wch );

			/* Midgaard Temple */
			char_to_room( wch, get_room_index( 3054 ) ); 
			wch->was_in_room = wch->in_room;
			wch->from_room = wch->in_room;
		}
	}

	/* unlink all exits to the room. */
	ecount = 0;
	for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
		for( pRoom2 = room_index_hash[iHash]; pRoom2; pRoom2 = pRoom2->next )
		{
			for( i = 0; i <= MAX_DIR; i++ )
			{
				if( !( ex = pRoom2->exit[i] ) )
					continue;

				if( pRoom2 == pRoom )
				{
					/* these are freed by free_room_index */
					ecount++;
					continue;
				}

				if( ex->u1.to_room == pRoom )
				{
					free_exit( pRoom2->exit[i] );
					pRoom2->exit[i] = NULL;
					SET_BIT( pRoom2->area->area_flags, AREA_CHANGED );
					ecount++;
				}
			}
		}
	}

	/* count extra descs. they are freed by free_room_index */
	edcount = 0;
	for ( pExtra = pRoom->extra_descr; pExtra; pExtra = pExtra->next )
	{
		edcount++;
	}

	if( top_vnum_room == index )
		for( i = 1; i < index; i++ )
			if( get_room_index( i ) )
				top_vnum_room = i;

	top_room--;

	unlink_room_index( pRoom );

	pRoom->area = NULL;
	pRoom->vnum = 0;

	free_room_index( pRoom );

	/* Na na na na! Hey Hey Hey, Good Bye! */

	sprintf( buf, "Removed room vnum ^C%d^x, %d resets, %d extra "
		"descriptions and %d exits.\n\r", index, rcount, edcount, ecount );
	send_to_char( buf, ch );
	sprintf( buf, "^C%d^x objects and ^C%d^x mobiles were extracted "
		"from the room.\n\r", ocount, mcount );
	send_to_char( buf, ch );

	return TRUE;
}


/* unlink a given reset from a given room */
void unlink_reset( ROOM_INDEX_DATA *pRoom, RESET_DATA *pReset )
{
	RESET_DATA *prev, *wReset;

	prev = pRoom->reset_first;
	for( wReset = pRoom->reset_first; wReset; wReset = wReset->next )
	{
		if( wReset == pReset )
		{
			if( pRoom->reset_first == pReset )
			{
				pRoom->reset_first = pReset->next;
				if( !pRoom->reset_first )
					pRoom->reset_last = NULL;
			}
			else if( pRoom->reset_last == pReset )
			{
				pRoom->reset_last = prev;
				prev->next = NULL;
			}
			else
				prev->next = prev->next->next;

			if( pRoom->area->reset_first == pReset )
				pRoom->area->reset_first = pReset->next;

			if( !pRoom->area->reset_first )
				pRoom->area->reset_last = NULL;
		}

		prev = wReset;
	}
}


void unlink_obj_index( OBJ_INDEX_DATA *pObj )
{
	int iHash;
	OBJ_INDEX_DATA *iObj, *sObj;

	iHash = pObj->vnum % MAX_KEY_HASH;

	sObj = obj_index_hash[iHash];

	if( sObj->next == NULL ) /* only entry */
		obj_index_hash[iHash] = NULL;
	else if( sObj == pObj ) /* first entry */
		obj_index_hash[iHash] = pObj->next;
	else /* everything else */
	{
		for( iObj = sObj; iObj != NULL; iObj = iObj->next )
		{
			if( iObj == pObj )
			{
				sObj->next = pObj->next;
				break;
			}
			sObj = iObj;
		}
	}
}


void unlink_room_index( ROOM_INDEX_DATA *pRoom )
{
	int iHash;
	ROOM_INDEX_DATA *iRoom, *sRoom;

	iHash = pRoom->vnum % MAX_KEY_HASH;

	sRoom = room_index_hash[iHash];

	if( sRoom->next == NULL ) /* only entry */
		room_index_hash[iHash] = NULL;
	else if( sRoom == pRoom ) /* first entry */
		room_index_hash[iHash] = pRoom->next;
	else /* everything else */
	{
		for( iRoom = sRoom; iRoom != NULL; iRoom = iRoom->next )
		{
			if( iRoom == pRoom )
			{
				sRoom->next = pRoom->next;
				break;
			}
			sRoom = iRoom;
		}
	}
}


void unlink_mob_index( MOB_INDEX_DATA *pMob )
{
	int iHash;
	MOB_INDEX_DATA *iMob, *sMob;

	iHash = pMob->vnum % MAX_KEY_HASH;

	sMob = mob_index_hash[iHash];

	if( sMob->next == NULL ) /* only entry */
		mob_index_hash[iHash] = NULL;
	else if( sMob == pMob ) /* first entry */
		mob_index_hash[iHash] = pMob->next;
	else /* everything else */
	{
		for( iMob = sMob; iMob != NULL; iMob = iMob->next )
		{
			if( iMob == pMob )
			{
				sMob->next = pMob->next;
				break;
			}
			sMob = iMob;
		}
	}
}


5.	In olc.h add the following DECLARES in the appropriate locations:

DECLARE_OLC_FUN( redit_delete             );
DECLARE_OLC_FUN( oedit_delete             );
DECLARE_OLC_FUN( medit_delete             );


6.	Recompile and all should be well. I STRONGLY suggest either running
a backup copy or running a second mud on a different port logging in
and deleting a few things then saving and rebooting just to make sure
things are working well. (If you do the second mud on the same machine
idea DO NOT FORGET to asave world on the main mud to restore the area
files).

If you have any problems/bugs to report you can mail me at feist@nortel.ca

Thats it!

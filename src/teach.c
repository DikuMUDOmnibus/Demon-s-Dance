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

bool check_teach ( CHAR_DATA *ch,char* skill, char* teach_required, CHAR_DATA *teacher  )
{
	int teach_bit;

	if (teacher->teach_flags == 0)
	{
		return FALSE;
	}
	if (teach_compare( teach_required ) == -1)
	{
		return FALSE;
	}
	teach_bit = teach_compare(teach_required);
	if (!IS_SET(teacher->teach_flags,teach_bit))
	{
		return FALSE;
	}
	if (IS_SET(teacher->teach_flags,teach_bit))
	{
		return TRUE;
	}
	return FALSE;
}

/*
 * Lookup a skill's teacher bit
 */
int teach_compare( const char *teach_required )
{
	int i;

    for ( i = 0; i < MAX_TEACH_TYPE; i++ )
    {
		if ( !str_cmp( teach_required, teach_table[i].teach_type ) )
		{
			return teach_table[i].teach_bit;
		}
    }

    return -1;
}

const	struct	teach_type teach_table	[MAX_TEACH_TYPE]	=
{
	/*{"TEACH_TYPE", teach_bit}*/
	{"TEACH_NONE",			A},
	{"TEACH_MULTI_HIT",		B},
	{"TEACH_LEGIONARE_1",	C},
	{"TEACH_LEGIONARE_2",	D},
	{"TEACH_SENSEI_1",		E},
	{"TEACH_SENSEI_2",		F},
	{"TEACH_DRACONIAN_1",	G},
	{"TEACH_DRACONIAN_2",	H},
	{"TEACH_TAO_1",			I},
	{"TEACH_TAO_2",			J},
	{"TEACH_ZEALOT_1",		K},
	{"TEACH_ZEALOT_2",		L},
	{"TEACH_DAGASHI_1",		M},
	{"TEACH_DAGASHI_2",		N},
	{"TEACH_MUTLI_CAST",	O},
	{NULL,					0}

};


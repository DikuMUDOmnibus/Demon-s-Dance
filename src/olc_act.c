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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "recycle.h"
#include "lookup.h"
#include "interp.h"

char * prog_type_to_name ( int type );

/* Return TRUE if area changed, FALSE if not. */
#define REDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define OEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define MEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define AEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define HEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define GEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )



struct olc_help_type
{
    char *command;
    const void *structure;
    char *desc;
};



bool show_version( CHAR_DATA *ch, char *argument )
{
    send_to_char( VERSION, ch );
    send_to_char( "\n\r", ch );
    send_to_char( AUTHOR, ch );
    send_to_char( "\n\r", ch );
    send_to_char( DATE, ch );
    send_to_char( "\n\r", ch );
    send_to_char( CREDITS, ch );
    send_to_char( "\n\r", ch );

    return FALSE;
}    

/*
 * This table contains help commands and a brief description of each.
 * ------------------------------------------------------------------
 */
const struct olc_help_type help_table[] =
{
    {	"area",		area_flags,	 "Area attributes."		 },
    {	"room",		room_flags,	 "Room attributes."		 },
    {	"sector",	sector_flags,	 "Sector types, terrain."	 },
    {	"exit",		exit_flags,	 "Exit types."			 },
    {	"type",		type_flags,	 "Types of objects."		 },
    {	"extra",	extra_flags,	 "Object attributes."		 },
    {	"wear",		wear_flags,	 "Where to wear object."	 },
    {	"spec",		spec_table,	 "Available special programs." 	 },
    {	"sex",		sex_flags,	 "Sexes."			 },
    {	"act",		act_flags,	 "Mobile attributes."		 },
    {	"affect",	affect_flags,	 "Mobile affects."		 },
	{	"affect2",	affect2_flags,	 "Mobile Affects Set 2"			},
    {	"wear-loc",	wear_loc_flags,	 "Where mobile wears object."	 },
    {	"spells",	skill_table,	 "Names of current spells." 	 },
    {	"container",	container_flags, "Container status."		 },
	{   "guild",        guild_flags,     "Guild flags."			 },
	{	"oprog",	oprog_flags,	 "ObjProgram flags."		 },
    {	"rprog",	rprog_flags,	 "RoomProgram flags."		 },


/* ROM specific bits: */

    {	"armor",	ac_type,	 "Ac for different attacks."	 },
    {   "apply",	apply_flags,	 "Apply flags"			 },
    {	"form",		form_flags,	 "Mobile body form."	         },
    {	"part",		part_flags,	 "Mobile body parts."		 },
    {	"imm",		imm_flags,	 "Mobile immunity."		 },
    {	"res",		res_flags,	 "Mobile resistance."	         },
    {	"vuln",		vuln_flags,	 "Mobile vulnerability."	 },
    {	"off",		off_flags,	 "Mobile offensive behaviour."	 },
	{	"teach",	teach_flags, "Mobile teaching abilities."	 },
    {	"size",		size_flags,	 "Mobile size."			 },
    {   "position",     position_flags,  "Mobile positions."             },
    {   "wclass",       weapon_class,    "Weapon class."                 }, 
    {   "wtype",        weapon_type2,    "Special weapon type."          },
    {	"portal",	portal_flags,	 "Portal types."		 },
    {	"furniture",	furniture_flags, "Furniture types."		 },
    {   "liquid",	liq_table,	 "Liquid types."		 },
    {	"apptype",	apply_types,	 "Apply types."			 },
    {	"weapon",	attack_table,	 "Weapon types."		 },
    {	"mprog",	mprog_flags,	 "MobProgram flags."		 },
    {	NULL,		NULL,		 NULL				 }
};



/*****************************************************************************
 Name:		show_flag_cmds
 Purpose:	Displays settable flags and stats.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  flag;
    int  col;
 
    buf1[0] = '\0';
    col = 0;
    for (flag = 0; flag_table[flag].name != NULL; flag++)
    {
	if ( flag_table[flag].settable )
	{
	    sprintf( buf, "%-19.18s", flag_table[flag].name );
	    strcat( buf1, buf );
	    if ( ++col % 4 == 0 )
		strcat( buf1, "\n\r" );
	}
    }
 
    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}


/*****************************************************************************
 Name:		show_skill_cmds
 Purpose:	Displays all skill functions.
 		Does remove those damn immortal commands from the list.
 		Could be improved by:
 		(1) Adding a check for a particular class.
 		(2) Adding a check for a level range.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_skill_cmds( CHAR_DATA *ch, int tar )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH*2 ];
    int  sn;
    int  col;
 
    buf1[0] = '\0';
    col = 0;
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
	if ( !skill_table[sn].name )
	    break;

	if ( !str_cmp( skill_table[sn].name, "reserved" )
	  || skill_table[sn].spell_fun == spell_null )
	    continue;

	if ( tar == -1 || skill_table[sn].target == tar )
	{
	    sprintf( buf, "%-19.18s", skill_table[sn].name );
	    strcat( buf1, buf );
	    if ( ++col % 4 == 0 )
		strcat( buf1, "\n\r" );
	}
    }
 
    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}



/*****************************************************************************
 Name:		show_spec_cmds
 Purpose:	Displays settable special functions.
 Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_spec_cmds( CHAR_DATA *ch )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  spec;
    int  col;
 
    buf1[0] = '\0';
    col = 0;
    send_to_char( "Preceed special functions with 'spec_'\n\r\n\r", ch );
    for (spec = 0; spec_table[spec].function != NULL; spec++)
    {
	sprintf( buf, "%-19.18s", &spec_table[spec].name[5] );
	strcat( buf1, buf );
	if ( ++col % 4 == 0 )
	    strcat( buf1, "\n\r" );
    }
 
    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}



/*****************************************************************************
 Name:		show_help
 Purpose:	Displays help for many tables used in OLC.
 Called by:	olc interpreters.
 ****************************************************************************/
bool show_help( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char spell[MAX_INPUT_LENGTH];
    int cnt;

    argument = one_argument( argument, arg );
    one_argument( argument, spell );

    /*
     * Display syntax.
     */
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  ? [command]\n\r\n\r", ch );
	send_to_char( "[command]  [description]\n\r", ch );
	for (cnt = 0; help_table[cnt].command != NULL; cnt++)
	{
	    sprintf( buf, "%-10.10s -{W%s\n\r{x",
	        capitalize( help_table[cnt].command ),
		help_table[cnt].desc );
	    send_to_char( buf, ch );
	}
	return FALSE;
    }

    /*
     * Find the command, show changeable data.
     * ---------------------------------------
     */
    for (cnt = 0; help_table[cnt].command != NULL; cnt++)
    {
        if (  arg[0] == help_table[cnt].command[0]
          && !str_prefix( arg, help_table[cnt].command ) )
	{
	    if ( help_table[cnt].structure == spec_table )
	    {
		show_spec_cmds( ch );
		return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == liq_table )
	    {
	        show_liqlist( ch );
	        return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == attack_table )
	    {
	        show_damlist( ch );
	        return FALSE;
	    }
	    else
	    if ( help_table[cnt].structure == skill_table )
	    {

		if ( spell[0] == '\0' )
		{
		    send_to_char( "Syntax:  ? spells "
		        "[ignore/attack/defend/self/object/all]\n\r", ch );
		    return FALSE;
		}

		if ( !str_prefix( spell, "all" ) )
		    show_skill_cmds( ch, -1 );
		else if ( !str_prefix( spell, "ignore" ) )
		    show_skill_cmds( ch, TAR_IGNORE );
		else if ( !str_prefix( spell, "attack" ) )
		    show_skill_cmds( ch, TAR_CHAR_OFFENSIVE );
		else if ( !str_prefix( spell, "defend" ) )
		    show_skill_cmds( ch, TAR_CHAR_DEFENSIVE );
		else if ( !str_prefix( spell, "self" ) )
		    show_skill_cmds( ch, TAR_CHAR_SELF );
		else if ( !str_prefix( spell, "object" ) )
		    show_skill_cmds( ch, TAR_OBJ_INV );
		else
		    send_to_char( "Syntax:  ? spell "
		        "[ignore/attack/defend/self/object/all]\n\r", ch );
		    
		return FALSE;
	    }
	    else
	    {
		show_flag_cmds( ch, help_table[cnt].structure );
		return FALSE;
	    }
	}
    }

    show_help( ch, "" );
    return FALSE;
}

REDIT( redit_rlist )
{
    ROOM_INDEX_DATA	*pRoomIndex;
    AREA_DATA		*pArea;
    char		buf  [ MAX_STRING_LENGTH   ];
    BUFFER		*buf1;
    char		arg  [ MAX_INPUT_LENGTH    ];
    bool found;
    int vnum;
    int  col = 0;

    one_argument( argument, arg );

    pArea = ch->in_room->area;
    buf1=new_buf();
/*    buf1[0] = '\0'; */
    found   = FALSE;

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pRoomIndex = get_room_index( vnum ) ) )
	{
		found = TRUE;
		sprintf( buf, "[%5d] %-17.16s",
		    vnum, capitalize( pRoomIndex->name ) );
		add_buf( buf1, buf );
		if ( ++col % 3 == 0 )
		    add_buf( buf1, "\n\r" );
	}
    }

    if ( !found )
    {
	send_to_char( "Room(s) not found in this area.\n\r", ch);
	return FALSE;
    }

    if ( col % 3 != 0 )
	add_buf( buf1, "\n\r" );

    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return FALSE;
}

REDIT( redit_mlist )
{
    MOB_INDEX_DATA	*pMobIndex;
    AREA_DATA		*pArea;
    char		buf  [ MAX_STRING_LENGTH   ];
    BUFFER		*buf1;
    char		arg  [ MAX_INPUT_LENGTH    ];
    bool fAll, found;
    int vnum;
    int  col = 0;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  mlist <all/name>\n\r", ch );
	return FALSE;
    }

    buf1=new_buf();
    pArea = ch->in_room->area;
/*    buf1[0] = '\0'; */
    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	{
	    if ( fAll || is_name( arg, pMobIndex->player_name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %-17.16s",
		    pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
		add_buf( buf1, buf );
		if ( ++col % 3 == 0 )
		    add_buf( buf1, "\n\r" );
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "Mobile(s) not found in this area.\n\r", ch);
	return FALSE;
    }

    if ( col % 3 != 0 )
	add_buf( buf1, "\n\r" );

    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return FALSE;
}



REDIT( redit_olist )
{
    OBJ_INDEX_DATA	*pObjIndex;
    AREA_DATA		*pArea;
    char		buf  [ MAX_STRING_LENGTH   ];
    BUFFER		*buf1;
    char		arg  [ MAX_INPUT_LENGTH    ];
    bool fAll, found;
    int vnum;
    int  col = 0;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  olist <all/name/item_type>\n\r", ch );
	return FALSE;
    }

    pArea = ch->in_room->area;
    buf1=new_buf();
/*    buf1[0] = '\0'; */
    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) )
	{
	    if ( fAll || is_name( arg, pObjIndex->name )
	    || flag_value( type_flags, arg ) == pObjIndex->item_type )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %-17.16s",
		    pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
		add_buf( buf1, buf );
		if ( ++col % 3 == 0 )
		    add_buf( buf1, "\n\r" );
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "Object(s) not found in this area.\n\r", ch);
	return FALSE;
    }

    if ( col % 3 != 0 )
	add_buf( buf1, "\n\r" );

    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return FALSE;
}



REDIT( redit_mshow )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  mshow <vnum>\n\r", ch );
	return FALSE;
    }

    if ( !is_number( argument ) )
    {
       send_to_char( "REdit: Ingresa un numero.\n\r", ch);
       return FALSE;
    }

    if ( is_number( argument ) )
    {
	value = atoi( argument );
	if ( !( pMob = get_mob_index( value ) ))
	{
	    send_to_char( "REdit:  That mobile does not exist.\n\r", ch );
	    return FALSE;
	}

	ch->desc->pEdit = (void *)pMob;
    }
 
    medit_show( ch, argument );
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE; 
}



REDIT( redit_oshow )
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  oshow <vnum>\n\r", ch );
	return FALSE;
    }

    if ( !is_number( argument ) )
    {
       send_to_char( "REdit: Ingresa un numero.\n\r", ch);
       return FALSE;
    }

    if ( is_number( argument ) )
    {
	value = atoi( argument );
	if ( !( pObj = get_obj_index( value ) ))
	{
	    send_to_char( "REdit:  That object does not exist.\n\r", ch );
	    return FALSE;
	}

	ch->desc->pEdit = (void *)pObj;
    }
 
    oedit_show( ch, argument );
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE; 
}



/*****************************************************************************
 Name:		check_range( lower vnum, upper vnum )
 Purpose:	Ensures the range spans only one area.
 Called by:	aedit_vnum(olc_act.c).
 ****************************************************************************/
bool check_range( int lower, int upper )
{
    AREA_DATA *pArea;
    int cnt = 0;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	/*
	 * lower < area < upper
	 */
        if ( ( lower <= pArea->min_vnum && pArea->min_vnum <= upper )
	||   ( lower <= pArea->max_vnum && pArea->max_vnum <= upper ) )
	    ++cnt;

	if ( cnt > 1 )
	    return FALSE;
    }
    return TRUE;
}



AREA_DATA *get_vnum_area( int vnum )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        if ( vnum >= pArea->min_vnum
          && vnum <= pArea->max_vnum )
            return pArea;
    }

    return 0;
}



/*
 * Area Editor Functions.
 */
AEDIT( aedit_show )
{
    AREA_DATA *pArea;
    char buf  [MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    sprintf( buf, "{YName:     {D[{W%5d{D] {W{W%s\n\r{x{x", pArea->vnum, pArea->name );
    send_to_char( buf, ch );

#if 0  /* ROM OLC */
    sprintf( buf, "{YRecall:   {D[{W%5d{D] {W{W%s\n\r{x{x", pArea->recall,
	get_room_index( pArea->recall )
	? get_room_index( pArea->recall )->name : "none" );
    send_to_char( buf, ch );
#endif /* ROM */

    sprintf( buf, "{YFile:     {W{W%s\n\r{x{x", pArea->file_name );
    send_to_char( buf, ch );

    sprintf( buf, "{YVnums:    {D[{W%d-{Y%d{D]{x\n\r", pArea->min_vnum, pArea->max_vnum );
    send_to_char( buf, ch );

    sprintf( buf, "{YAge:      {D[{W%d{D]{x\n\r",	pArea->age );
    send_to_char( buf, ch );

    sprintf( buf, "{YPlayers:  {D[{W%d{D]{x\n\r", pArea->nplayer );
    send_to_char( buf, ch );

    sprintf( buf, "{YSecurity: {D[{W%d{D]{x\n\r", pArea->security );
    send_to_char( buf, ch );

    sprintf( buf, "{YBuilders: {D[{W%s{D]{x\n\r", pArea->builders );
    send_to_char( buf, ch );

    sprintf( buf, "{YCredits : {D[{W%s{D]{x\n\r", pArea->credits );
    send_to_char( buf, ch );

    sprintf( buf, "{YFlags:    {D[{W%s{D]{x\n\r", flag_string( area_flags, pArea->area_flags ) );
    send_to_char( buf, ch );

    return FALSE;
}



AEDIT( aedit_reset )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    reset_area( pArea );
    send_to_char( "Area reset.\n\r", ch );

    return FALSE;
}



AEDIT( aedit_create )
{
    AREA_DATA *pArea;

    pArea               =   new_area();
    area_last->next     =   pArea;
    area_last		=   pArea;	/* Thanks, Walker. */
    ch->desc->pEdit     =   (void *)pArea;

    SET_BIT( pArea->area_flags, AREA_ADDED );
    send_to_char( "Area Created.\n\r", ch );
    return FALSE;
}



AEDIT( aedit_name )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:   name [$name]\n\r", ch );
	return FALSE;
    }

    free_string( pArea->name );
    pArea->name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_credits )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:   credits [$credits]\n\r", ch );
	return FALSE;
    }

    free_string( pArea->credits );
    pArea->credits = str_dup( argument );

    send_to_char( "Credits set.\n\r", ch );
    return TRUE;
}


AEDIT( aedit_file )
{
    AREA_DATA *pArea;
    char file[MAX_STRING_LENGTH];
    int i, length;

    EDIT_AREA(ch, pArea);

    one_argument( argument, file );	/* Forces Lowercase */

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  filename [$file]\n\r", ch );
	return FALSE;
    }

    /*
     * Simple Syntax Check.
     */
    length = STRLEN( argument );
    if ( length > 8 )
    {
	send_to_char( "No more than eight characters allowed.\n\r", ch );
	return FALSE;
    }
    
    /*
     * Allow only letters and numbers.
     */
    for ( i = 0; i < length; i++ )
    {
	if ( !isalnum( file[i] ) )
	{
	    send_to_char( "Only letters and numbers are valid.\n\r", ch );
	    return FALSE;
	}
    }    

    free_string( pArea->file_name );
    strcat( file, ".are" );
    pArea->file_name = str_dup( file );

    send_to_char( "Filename set.\n\r", ch );
    return TRUE;
}



AEDIT( aedit_age )
{
    AREA_DATA *pArea;
    char age[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument( argument, age );

    if ( !is_number( age ) || age[0] == '\0' )
    {
	send_to_char( "Syntax:  age [#xage]\n\r", ch );
	return FALSE;
    }

    pArea->age = atoi( age );

    send_to_char( "Age set.\n\r", ch );
    return TRUE;
}


#if 0 /* ROM OLC */
AEDIT( aedit_recall )
{
    AREA_DATA *pArea;
    char room[MAX_STRING_LENGTH];
    int  value;

    EDIT_AREA(ch, pArea);

    one_argument( argument, room );

    if ( !is_number( argument ) || argument[0] == '\0' )
    {
	send_to_char( "Syntax:  recall [#xrvnum]\n\r", ch );
	return FALSE;
    }

    value = atoi( room );

    if ( !get_room_index( value ) )
    {
	send_to_char( "AEdit:  Room vnum does not exist.\n\r", ch );
	return FALSE;
    }

    pArea->recall = value;

    send_to_char( "Recall set.\n\r", ch );
    return TRUE;
}
#endif /* ROM OLC */


AEDIT( aedit_security )
{
    AREA_DATA *pArea;
    char sec[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int  value;

    EDIT_AREA(ch, pArea);

    one_argument( argument, sec );

    if ( !is_number( sec ) || sec[0] == '\0' )
    {
	send_to_char( "Syntax:  security [#xlevel]\n\r", ch );
	return FALSE;
    }

    value = atoi( sec );

    if ( value > ch->pcdata->security || value < 0 )
    {
	if ( ch->pcdata->security != 0 )
	{
	    sprintf( buf, "Security is 0-%d.\n\r", ch->pcdata->security );
	    send_to_char( buf, ch );
	}
	else
	    send_to_char( "Security is 0 only.\n\r", ch );
	return FALSE;
    }

    pArea->security = value;

    send_to_char( "Security set.\n\r", ch );
    return TRUE;
}



AEDIT( aedit_builder )
{
    AREA_DATA *pArea;
    char name[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument( argument, name );

    if ( name[0] == '\0' )
    {
	send_to_char( "Syntax:  builder [$name]  -toggles builder\n\r", ch );
	send_to_char( "Syntax:  builder All      -allows everyone\n\r", ch );
	return FALSE;
    }

    name[0] = UPPER( name[0] );

    if ( strstr( pArea->builders, name ) != '\0' )
    {
	pArea->builders = string_replace( pArea->builders, name, "\0" );
	pArea->builders = string_unpad( pArea->builders );

	if ( pArea->builders[0] == '\0' )
	{
	    free_string( pArea->builders );
	    pArea->builders = str_dup( "None" );
	}
	send_to_char( "Builder removed.\n\r", ch );
	return TRUE;
    }
    else
    {
	buf[0] = '\0';
	if ( strstr( pArea->builders, "None" ) != '\0' )
	{
	    pArea->builders = string_replace( pArea->builders, "None", "\0" );
	    pArea->builders = string_unpad( pArea->builders );
	}

	if (pArea->builders[0] != '\0' )
	{
	    strcat( buf, pArea->builders );
	    strcat( buf, " " );
	}
	strcat( buf, name );
	free_string( pArea->builders );
	pArea->builders = string_proper( str_dup( buf ) );

	send_to_char( "Builder added.\n\r", ch );
	send_to_char( pArea->builders,ch);
	return TRUE;
    }

    return FALSE;
}



AEDIT( aedit_vnum )
{
    AREA_DATA *pArea;
    char lower[MAX_STRING_LENGTH];
    char upper[MAX_STRING_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    argument = one_argument( argument, lower );
    one_argument( argument, upper );

    if ( !is_number( lower ) || lower[0] == '\0'
    || !is_number( upper ) || upper[0] == '\0' )
    {
	send_to_char( "Syntax:  vnum [#xlower] [#xupper]\n\r", ch );
	return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
	return FALSE;
    }
    
    if ( !check_range( atoi( lower ), atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
	send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
	return FALSE;
    }

    pArea->min_vnum = ilower;
    send_to_char( "Lower vnum set.\n\r", ch );

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
	send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
	return TRUE;	/* The lower value has been set. */
    }

    pArea->max_vnum = iupper;
    send_to_char( "Upper vnum set.\n\r", ch );

    return TRUE;
}



AEDIT( aedit_lvnum )
{
    AREA_DATA *pArea;
    char lower[MAX_STRING_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    one_argument( argument, lower );

    if ( !is_number( lower ) || lower[0] == '\0' )
    {
	send_to_char( "Syntax:  min_vnum [#xlower]\n\r", ch );
	return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = pArea->max_vnum ) )
    {
	send_to_char( "AEdit:  Value must be less than the max_vnum.\n\r", ch );
	return FALSE;
    }
    
    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
	send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
	return FALSE;
    }

    pArea->min_vnum = ilower;
    send_to_char( "Lower vnum set.\n\r", ch );
    return TRUE;
}



AEDIT( aedit_uvnum )
{
    AREA_DATA *pArea;
    char upper[MAX_STRING_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    one_argument( argument, upper );

    if ( !is_number( upper ) || upper[0] == '\0' )
    {
	send_to_char( "Syntax:  max_vnum [#xupper]\n\r", ch );
	return FALSE;
    }

    if ( ( ilower = pArea->min_vnum ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
	return FALSE;
    }
    
    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
	send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
	return FALSE;
    }

    pArea->max_vnum = iupper;
    send_to_char( "Upper vnum set.\n\r", ch );

    return TRUE;
}

/*
 * Room Editor Functions.
 */
REDIT( redit_show )
{
    ROOM_INDEX_DATA	*pRoom;
    char		buf  [MAX_STRING_LENGTH];
    char		buf1 [2*MAX_STRING_LENGTH];
    OBJ_DATA		*obj;
    CHAR_DATA		*rch;
	PROG_LIST		*list;
    int			door;
    bool		fcnt;
    
    EDIT_ROOM(ch, pRoom);

    buf1[0] = '\0';
    
    sprintf( buf, "{YDescription:\n\r{W%s{x", pRoom->description );
    strcat( buf1, buf );

    sprintf( buf, "{YName:       {D[{W%s{D]{x\n\rArea:       [%5d] {W%s\n\r{x",
	    pRoom->name, pRoom->area->vnum, pRoom->area->name );
    strcat( buf1, buf );

    sprintf( buf, "{YVnum:       {D[{W%5d{D]{x\n\r{YSector:     {D[{W%s{D]{x\n\r",
	    pRoom->vnum, flag_string( sector_flags, pRoom->sector_type ) );
    strcat( buf1, buf );

    sprintf( buf, "{YRoom flags: {D[{W%s{D]{x\n\r",
	    flag_string( room_flags, pRoom->room_flags ) );
    strcat( buf1, buf );

    if ( pRoom->heal_rate != 100 || pRoom->mana_rate != 100 )
    {
	sprintf( buf, "{YHealth rec: {D[{W%d{D]\n\r{YMana rec  : {D[{W%d{D]{x\n\r",
		pRoom->heal_rate , pRoom->mana_rate );
	strcat( buf1, buf );
    }

    if ( pRoom->clan > 0 )
    {
	sprintf( buf, "{YClan      : {D[{Y%d{D]{x {W%s\n\r{x",
		pRoom->clan,
		clan_table[pRoom->clan].name );
	strcat( buf1, buf );
    }

    if ( !IS_NULLSTR(pRoom->owner) )
    {
	sprintf( buf, "{YOwner     : {D[{W%s{D]{x\n\r", pRoom->owner );
	strcat( buf1, buf );
    }

    if ( pRoom->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	strcat( buf1, "{YDesc Kwds:  {D[{W" );
	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}
	strcat( buf1, "{D]{x\n\r" );
    }

    strcat( buf1, "{YCharacters: {D[{W" );
    fcnt = FALSE;
    for ( rch = pRoom->people; rch; rch = rch->next_in_room )
    {
	one_argument( rch->name, buf );
	strcat( buf1, buf );
	strcat( buf1, " {D" );
	fcnt = TRUE;
    }

    if ( fcnt )
    {
	int end;

	end = STRLEN(buf1) - 1;
	buf1[end] = ']';
	strcat( buf1, "\n\r{x" );
    }
    else
	strcat( buf1, "{Wnone{D]{x\n\r" );

    strcat( buf1, "{YObjects:    {D[{W" );
    fcnt = FALSE;
    for ( obj = pRoom->contents; obj; obj = obj->next_content )
    {
	one_argument( obj->name, buf );
	strcat( buf1, buf );
	strcat( buf1, " {D" );
	fcnt = TRUE;
    }

    if ( fcnt )
    {
	int end;

	end = STRLEN(buf1) - 1;
	buf1[end] = ']';
	strcat( buf1, "\n\r{x" );
    }
    else
	strcat( buf1, "{Wnone{D]{x\n\r" );

    for ( door = 0; door < MAX_DIR; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = pRoom->exit[door] ) )
	{
	    char word[MAX_INPUT_LENGTH];
	    char reset_state[MAX_STRING_LENGTH];
	    char *state;
	    int i, length;

	    sprintf( buf, "{Y-%-5s {Wto {D[{W%5d{D] {YKey: {D[{W%5d{D]{x ",
		capitalize(dir_name[door]),
		pexit->u1.to_room ? pexit->u1.to_room->vnum : 0,      /* ROM OLC */
		pexit->key );
	    strcat( buf1, buf );

	    /*
	     * Format up the exit info.
	     * Capitalize all flags that are not part of the reset info.
	     */
	    strcpy( reset_state, flag_string( exit_flags, pexit->rs_flags ) );
	    state = flag_string( exit_flags, pexit->exit_info );
	    strcat( buf1, " {YExit flags: {D[{W" );
	    for (; ;)
	    {
			state = one_argument( state, word );

			if ( word[0] == '\0' )
			{
				int end;

				end = STRLEN(buf1) - 1;
				buf1[end] = ']';
				strcat( buf1, "\n\r{x" );
				break;
			}

			if ( str_infix( word, reset_state ) )
			{
				length = STRLEN(word);
				for (i = 0; i < length; i++)
				word[i] = UPPER(word[i]);
			}
			strcat( buf1, word );
			strcat( buf1, " " );
	    }

	    if ( pexit->keyword && pexit->keyword[0] != '\0' )
	    {
		sprintf( buf, "{YKwds: {D[{W%s{D]{x\n\r", pexit->keyword );
		strcat( buf1, buf );
	    }
	    if ( pexit->description && pexit->description[0] != '\0' )
	    {
		sprintf( buf, "{W%s{x", pexit->description );
		strcat( buf1, buf );
		}
/*	sprintf(buf,"     {YEnter String:{W %s{x\n\r",pexit->enter_string);
	strcat(buf1,buf);*/
	sprintf(buf,"     {YExit String:{W  %s{x\n\r",pexit->exit_string);
	strcat(buf1,buf);
	}
    }

    send_to_char( buf1, ch );
    if ( pRoom->rprogs )
    {
	int cnt;

	sprintf(buf, "\n\rROOMPrograms for [%5d]:\n\r", pRoom->vnum);
	send_to_char( buf, ch );

	for (cnt=0, list=pRoom->rprogs; list; list=list->next)
	{
		if (cnt ==0)
		{
			send_to_char ( " Number Vnum Trigger Phrase\n\r", ch );
			send_to_char ( " ------ ---- ------- ------\n\r", ch );
		}

		sprintf(buf, "[%5d] %4d %7s %s\n\r", cnt,
			list->vnum,prog_type_to_name(list->trig_type),
			list->trig_phrase);
		send_to_char( buf, ch );
		cnt++;
	}
    }
    return FALSE;
}




/* Local function. */
bool change_exit( CHAR_DATA *ch, char *argument, int door )
{
    ROOM_INDEX_DATA *pRoom;
	ROOM_INDEX_DATA *pRoomOrig;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int  value;
	char buf[MSL];

    EDIT_ROOM(ch, pRoom);

    /*
     * Set the exit flags, needs full argument.
     * ----------------------------------------
     */
    if ( ( value = flag_value( exit_flags, argument ) ) != NO_FLAG )
    {
	ROOM_INDEX_DATA *pToRoom;
	sh_int rev;                                    /* ROM OLC */

	if ( !pRoom->exit[door] )
	   {
	   	send_to_char("Exit does not exist\n\r",ch);
	   	return FALSE;
	   }
	 /*   pRoom->exit[door] = new_exit(); */

	/*
	 * This room.
	 */
	TOGGLE_BIT(pRoom->exit[door]->rs_flags,  value);
	/* Don't toggle exit_info because it can be changed by players. */
	pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;

	/*
	 * Connected room.
	 */
	pToRoom = pRoom->exit[door]->u1.to_room;     /* ROM OLC */
	rev = rev_dir[door];

	if (pToRoom->exit[rev] != NULL)
	{
	   TOGGLE_BIT(pToRoom->exit[rev]->rs_flags,  value);
	   TOGGLE_BIT(pToRoom->exit[rev]->exit_info, value);
	}

	send_to_char( "Exit flag toggled.\n\r", ch );
	return TRUE;
    }

    /*
     * Now parse the arguments.
     */
    argument = one_argument( argument, command );
	if ( (str_cmp( command, "enter_string"))  && (str_cmp( command, "exit_string")) )
	{
		one_argument( argument, arg );
	}
	if ( (!str_cmp( command, "enter_string"))  || (!str_cmp( command, "exit_string")) )
	{
		one_arg_caps(argument,arg);
	}

    if ( command[0] == '\0' && argument[0] == '\0' )	/* Move command. */
    {
	move_char( ch, door, TRUE );                    /* ROM OLC */
	return FALSE;
    }

    if ( command[0] == '?' )
    {
	do_help( ch, "EXIT" );
	return FALSE;
    }

    if ( !str_cmp( command, "delete" ) )
    {
	ROOM_INDEX_DATA *pToRoom;
	sh_int rev;                                     /* ROM OLC */
	
	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Cannot delete a null exit.\n\r", ch );
	    return FALSE;
	}

	/*
	 * Remove ToRoom Exit.
	 */
	rev = rev_dir[door];
	pToRoom = pRoom->exit[door]->u1.to_room;       /* ROM OLC */
	
	if ( pToRoom->exit[rev] )
	{
	    free_exit( pToRoom->exit[rev] );
	    pToRoom->exit[rev] = NULL;
	}

	/*
	 * Remove this exit.
	 */
	free_exit( pRoom->exit[door] );
	pRoom->exit[door] = NULL;

	send_to_char( "Exit unlinked.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "link" ) )
    {
	EXIT_DATA *pExit;

	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] link [vnum]\n\r", ch );
	    return FALSE;
	}

	value = atoi( arg );

	if ( !get_room_index( value ) )
	{
	    send_to_char( "REdit:  Cannot link to non-existant room.\n\r", ch );
	    return FALSE;
	}

	if ( !IS_BUILDER( ch, get_room_index( value )->area ) )
	{
	    send_to_char( "REdit:  Cannot link to that area.\n\r", ch );
	    return FALSE;
	}

	if ( get_room_index( value )->exit[rev_dir[door]] )
	{
	    send_to_char( "REdit:  Remote side's exit already exists.\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	{
	    pRoom->exit[door] = new_exit();
	}

	pRoomOrig = pRoom;
	pRoom->exit[door]->u1.to_room = get_room_index( value );   /* ROM OLC */
	pRoom->exit[door]->orig_door = door;
	
/*	pRoom->exit[door]->vnum = value;                Can't set vnum in ROM */

	pRoom                   = get_room_index( value );
	door                    = rev_dir[door];
	pExit                   = new_exit();
	pExit->u1.to_room       = ch->in_room;
/*	pExit->vnum             = ch->in_room->vnum;    Can't set vnum in ROM */
	pExit->orig_door	= door;
	pRoom->exit[door]       = pExit;

	/*Set exit_string from new room to old room*/
	sprintf(buf,"leaves %s",dir_name[door]);
	pRoom->exit[door]->exit_string = str_dup(buf);
	
	/*Set exit_string from old room to new room*/
	sprintf(buf,"leaves %s",dir_name[rev_dir[door]]);
	pRoomOrig->exit[rev_dir[door]]->exit_string = str_dup(buf);

	send_to_char( "Two-way link established.\n\r", ch );
	return TRUE;
    }
        
    if ( !str_cmp( command, "dig" ) )
    {
	char buf[MAX_STRING_LENGTH];
	
	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax: [direction] dig <vnum>\n\r", ch );
	    return FALSE;
	}
	
	redit_create( ch, arg );
	sprintf( buf, "link %s", arg );
	change_exit( ch, buf, door);
	return TRUE;
    }

    if ( !str_cmp( command, "room" ) )
    {
	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] room [vnum]\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	{
	    pRoom->exit[door] = new_exit();
	}

	value = atoi( arg );

	if ( !get_room_index( value ) )
	{
	    send_to_char( "REdit:  Cannot link to non-existant room.\n\r", ch );
	    return FALSE;
	}

	pRoom->exit[door]->u1.to_room = get_room_index( value );    /* ROM OLC */
	pRoom->exit[door]->orig_door = door;
/*	pRoom->exit[door]->vnum = value;                 Can't set vnum in ROM */

	send_to_char( "One-way link established.\n\r", ch );
	return TRUE;
    }

    if ( !str_cmp( command, "key" ) )
    {
		if ( arg[0] == '\0' || !is_number( arg ) )
		{
			send_to_char( "Syntax:  [direction] key [vnum]\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		   {
	   		send_to_char("Exit does not exist.\n\r",ch);
	   		return FALSE;
		   }

	/*	if ( !pRoom->exit[door] )
		{
			pRoom->exit[door] = new_exit();
		} */

		value = atoi( arg );

		if ( !get_obj_index( value ) )
		{
			send_to_char( "REdit:  Item doesn't exist.\n\r", ch );
			return FALSE;
		}

		if (!(get_obj_index( atoi( argument ) )))
		{
			send_to_char( "REdit:  Key doesn't exist.\n\r", ch );
			return FALSE;
		}

		pRoom->exit[door]->key = value;

		send_to_char( "Exit key set.\n\r", ch );
		return TRUE;
    }

    if ( !str_cmp( command, "enter_string" ) )
    {
		if ( arg[0] == '\0' )
		{
			send_to_char( "Syntax:  [direction] enter_string <enter message>\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{
			send_to_char("Exit does not exist.\n\r",ch);
			return FALSE;
		}
		sprintf(buf,arg);
/*		pRoom->exit[door]->enter_string = str_dup(buf);*/
		stc("Enter_string set\n\r",ch);
		return TRUE;
	}
    if ( !str_cmp( command, "exit_string" ) )
    {
		if ( arg[0] == '\0' )
		{
			send_to_char( "Syntax:  [direction] exit_string <exit message>\n\r", ch );
			return FALSE;
		}
		if ( !pRoom->exit[door] )
		{
			send_to_char("Exit does not exist.\n\r",ch);
			return FALSE;
		}
		sprintf(buf,arg);
		pRoom->exit[door]->exit_string = str_dup(buf);
		stc("Exit_string set\n\r",ch);
		return TRUE;
	}


    if ( !str_cmp( command, "name" ) )
    {
	if ( arg[0] == '\0' )
	{
	    send_to_char( "Syntax:  [direction] name [string]\n\r", ch );
	    send_to_char( "         [direction] name none\n\r", ch );
	    return FALSE;
	}

	if ( !pRoom->exit[door] )
	   {
	   	send_to_char("Salida no existe.\n\r",ch);
	   	return FALSE;
	   }

/*	if ( !pRoom->exit[door] )
	{
	    pRoom->exit[door] = new_exit();
	} */

	free_string( pRoom->exit[door]->keyword );
	if (str_cmp(arg,"none"))
		pRoom->exit[door]->keyword = str_dup( arg );
	else
		pRoom->exit[door]->keyword = str_dup( "" );

	send_to_char( "Exit name set.\n\r", ch );
	return TRUE;
    }

    if ( !str_prefix( command, "description" ) )
    {
	if ( arg[0] == '\0' )
	{
	   if ( !pRoom->exit[door] )
	   {
	   	send_to_char("Salida no existe.\n\r",ch);
	   	return FALSE;
	   }

/*	    if ( !pRoom->exit[door] )
	    {
	        pRoom->exit[door] = new_exit();
	    } */

	    string_append( ch, &pRoom->exit[door]->description );
	    return TRUE;
	}

	send_to_char( "Syntax:  [direction] desc\n\r", ch );
	return FALSE;
    }

    return FALSE;
}



REDIT( redit_north )
{
    if ( change_exit( ch, argument, DIR_NORTH ) )
	return TRUE;

    return FALSE;
}



REDIT( redit_south )
{
    if ( change_exit( ch, argument, DIR_SOUTH ) )
	return TRUE;

    return FALSE;
}



REDIT( redit_east )
{
    if ( change_exit( ch, argument, DIR_EAST ) )
	return TRUE;

    return FALSE;
}



REDIT( redit_west )
{
    if ( change_exit( ch, argument, DIR_WEST ) )
	return TRUE;

    return FALSE;
}



REDIT( redit_up )
{
    if ( change_exit( ch, argument, DIR_UP ) )
	return TRUE;

    return FALSE;
}



REDIT( redit_down )
{
    if ( change_exit( ch, argument, DIR_DOWN ) )
	return TRUE;

    return FALSE;
}



REDIT( redit_ed )
{
    ROOM_INDEX_DATA *pRoom;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, command );
    one_argument( argument, keyword );

    if ( command[0] == '\0' || keyword[0] == '\0' )
    {
	send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	send_to_char( "         ed edit [keyword]\n\r", ch );
	send_to_char( "         ed delete [keyword]\n\r", ch );
	send_to_char( "         ed format [keyword]\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	    return FALSE;
	}

	ed			=   new_extra_descr();
	ed->keyword		=   str_dup( keyword );
	ed->description		=   str_dup( "" );
	ed->next		=   pRoom->extra_descr;
	pRoom->extra_descr	=   ed;

	string_append( ch, &ed->description );

	return TRUE;
    }


    if ( !str_cmp( command, "edit" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	string_append( ch, &ed->description );

	return TRUE;
    }


    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	if ( !ped )
	    pRoom->extra_descr = ed->next;
	else
	    ped->next = ed->next;

	free_extra_descr( ed );

	send_to_char( "Extra description deleted.\n\r", ch );
	return TRUE;
    }


    if ( !str_cmp( command, "format" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed format [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	ed->description = format_string( ed->description );

	send_to_char( "Extra description formatted.\n\r", ch );
	return TRUE;
    }

    redit_ed( ch, "" );
    return FALSE;
}



REDIT( redit_create )
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int value;
    int iHash;
    
    EDIT_ROOM(ch, pRoom);

    value = atoi( argument );

    if ( argument[0] == '\0' || value <= 0 )
    {
	send_to_char( "Syntax:  create [vnum > 0]\n\r", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char( "REdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "REdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_room_index( value ) )
    {
	send_to_char( "REdit:  Room vnum already exists.\n\r", ch );
	return FALSE;
    }

    pRoom			= new_room_index();
    pRoom->area			= pArea;
    pRoom->vnum			= value;

    if ( value > top_vnum_room )
        top_vnum_room = value;

    iHash			= value % MAX_KEY_HASH;
    pRoom->next			= room_index_hash[iHash];
    room_index_hash[iHash]	= pRoom;
    ch->desc->pEdit		= (void *)pRoom;

    send_to_char( "Room created.\n\r", ch );
    return TRUE;
}



REDIT( redit_name )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [name]\n\r", ch );
	return FALSE;
    }

    free_string( pRoom->name );
    pRoom->name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}



REDIT( redit_desc )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pRoom->description );
	return TRUE;
    }

    send_to_char( "Syntax:  desc\n\r", ch );
    return FALSE;
}

REDIT( redit_heal )
{
    ROOM_INDEX_DATA *pRoom;
    
    EDIT_ROOM(ch, pRoom);
    
    if (is_number(argument))
       {
          pRoom->heal_rate = atoi ( argument );
          send_to_char ( "Heal rate set.\n\r", ch);
          return TRUE;
       }

    send_to_char ( "Syntax : heal <#xnumber>\n\r", ch);
    return FALSE;
}       

REDIT( redit_mana )
{
    ROOM_INDEX_DATA *pRoom;
    
    EDIT_ROOM(ch, pRoom);
    
    if (is_number(argument))
       {
          pRoom->mana_rate = atoi ( argument );
          send_to_char ( "Mana rate set.\n\r", ch);
          return TRUE;
       }

    send_to_char ( "Syntax : mana <#xnumber>\n\r", ch);
    return FALSE;
}       

REDIT( redit_clan )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);
    
    pRoom->clan = clan_lookup(argument);
    
    send_to_char ( "Clan set.\n\r", ch);
    return TRUE;
}
      
REDIT( redit_format )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    pRoom->description = format_string( pRoom->description );

    send_to_char( "String formatted.\n\r", ch );
    return TRUE;
}



REDIT( redit_mreset )
{
    ROOM_INDEX_DATA	*pRoom;
    MOB_INDEX_DATA	*pMobIndex;
    CHAR_DATA		*newmob;
    char		arg [ MAX_INPUT_LENGTH ];
    char		arg2 [ MAX_INPUT_LENGTH ];

    RESET_DATA		*pReset;
    char		output [ MAX_STRING_LENGTH ];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
	send_to_char ( "Syntax:  mreset <vnum> <max #x> <mix #x>\n\r", ch );
	return FALSE;
    }

    if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
    {
	send_to_char( "REdit: No mobile has that vnum.\n\r", ch );
	return FALSE;
    }

    if ( pMobIndex->area != pRoom->area )
    {
	send_to_char( "REdit: No such mobile in this area.\n\r", ch );
	return FALSE;
    }

    /*
     * Create the mobile reset.
     */
    pReset              = new_reset_data();
    pReset->command	= 'M';
    pReset->arg1	= pMobIndex->vnum;
    pReset->arg2	= is_number( arg2 ) ? atoi( arg2 ) : MAX_MOB;
    pReset->arg3	= pRoom->vnum;
    pReset->arg4	= is_number( argument ) ? atoi (argument) : 1;
    add_reset( pRoom, pReset, 0/* Last slot*/ );

    /*
     * Create the mobile.
     */
    newmob = create_mobile( pMobIndex );
    char_to_room( newmob, pRoom );

    sprintf( output, "%s (%d) has been loaded and added to resets.\n\r"
	"There will be a maximum of %d loaded to this room.\n\r",
	capitalize( pMobIndex->short_descr ),
	pMobIndex->vnum,
	pReset->arg2 );
    send_to_char( output, ch );
    act( "$n has created $N!", ch, NULL, newmob, TO_ROOM );
    return TRUE;
}



struct wear_type
{
    int	wear_loc;
    int	wear_bit;
};



const struct wear_type wear_table[] =
{
    {	WEAR_NONE,	ITEM_TAKE		},
    {	WEAR_LIGHT,	ITEM_LIGHT		},
    {	WEAR_FINGER_L,	ITEM_WEAR_FINGER	},
    {	WEAR_FINGER_R,	ITEM_WEAR_FINGER	},
    {	WEAR_NECK_1,	ITEM_WEAR_NECK		},
    {	WEAR_NECK_2,	ITEM_WEAR_NECK		},
    {	WEAR_BODY,	ITEM_WEAR_BODY		},
    {	WEAR_HEAD,	ITEM_WEAR_HEAD		},
    {	WEAR_LEGS,	ITEM_WEAR_LEGS		},
    {	WEAR_FEET,	ITEM_WEAR_FEET		},
    {	WEAR_HANDS,	ITEM_WEAR_HANDS		},
    {	WEAR_ARMS,	ITEM_WEAR_ARMS		},
    {	WEAR_SHIELD,	ITEM_WEAR_SHIELD	},
    {	WEAR_ABOUT,	ITEM_WEAR_ABOUT		},
    {	WEAR_WAIST,	ITEM_WEAR_WAIST		},
    {	WEAR_WRIST_L,	ITEM_WEAR_WRIST		},
    {	WEAR_WRIST_R,	ITEM_WEAR_WRIST		},
    {	WEAR_WIELD,	ITEM_WIELD		},
    {	WEAR_HOLD,	ITEM_HOLD		},
	{	WEAR_LODGE_LEG, ITEM_LODGE_LEG		},
	{	WEAR_LODGE_ARM, ITEM_LODGE_ARM		},
	{	WEAR_LODGE_RIB, ITEM_LODGE_RIB		},
	{	WEAR_FACE,  ITEM_WEAR_FACE	},
	{	WEAR_NOCKED,	ITEM_WEAR_NOCKED	},
    {	NO_FLAG,	NO_FLAG			}
};



/*****************************************************************************
 Name:		wear_loc
 Purpose:	Returns the location of the bit that matches the count.
 		1 = first match, 2 = second match etc.
 Called by:	oedit_reset(olc_act.c).
 ****************************************************************************/
int wear_loc(int bits, int count)
{
    int flag;
 
    for (flag = 0; wear_table[flag].wear_bit != NO_FLAG; flag++)
    {
        if ( IS_SET(bits, wear_table[flag].wear_bit) && --count < 1)
            return wear_table[flag].wear_loc;
    }
 
    return NO_FLAG;
}



/*****************************************************************************
 Name:		wear_bit
 Purpose:	Converts a wear_loc into a bit.
 Called by:	redit_oreset(olc_act.c).
 ****************************************************************************/
int wear_bit(int loc)
{
    int flag;
 
    for (flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++)
    {
        if ( loc == wear_table[flag].wear_loc )
            return wear_table[flag].wear_bit;
    }
 
    return 0;
}



REDIT( redit_oreset )
{
    ROOM_INDEX_DATA	*pRoom;
    OBJ_INDEX_DATA	*pObjIndex;
    OBJ_DATA		*newobj;
    OBJ_DATA		*to_obj;
    CHAR_DATA		*to_mob;
    char		arg1 [ MAX_INPUT_LENGTH ];
    char		arg2 [ MAX_INPUT_LENGTH ];
    int			olevel = 0;

    RESET_DATA		*pReset;
    char		output [ MAX_STRING_LENGTH ];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
	send_to_char ( "Syntax:  oreset <vnum> <args>\n\r", ch );
	send_to_char ( "        -no_args               = into room\n\r", ch );
	send_to_char ( "        -<obj_name>            = into obj\n\r", ch );
	send_to_char ( "        -<mob_name> <wear_loc> = into mob\n\r", ch );
	return FALSE;
    }

    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
	send_to_char( "REdit: No object has that vnum.\n\r", ch );
	return FALSE;
    }

    if ( pObjIndex->area != pRoom->area )
    {
	send_to_char( "REdit: No such object in this area.\n\r", ch );
	return FALSE;
    }

    /*
     * Load into room.
     */
    if ( arg2[0] == '\0' )
    {
	pReset		= new_reset_data();
	pReset->command	= 'O';
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= 0;
	pReset->arg3	= pRoom->vnum;
	pReset->arg4	= 0;
	add_reset( pRoom, pReset, 0/* Last slot*/ );

	newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
	obj_to_room( newobj, pRoom );

	sprintf( output, "%s (%d) has been loaded and added to resets.\n\r",
	    capitalize( pObjIndex->short_descr ),
	    pObjIndex->vnum );
	send_to_char( output, ch );
    }
    else
    /*
     * Load into object's inventory.
     */
    if ( argument[0] == '\0'
    && ( ( to_obj = get_obj_list( ch, arg2, pRoom->contents ) ) != NULL ) )
    {
	pReset		= new_reset_data();
	pReset->command	= 'P';
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= 0;
	pReset->arg3	= to_obj->pIndexData->vnum;
	pReset->arg4	= 1;
	add_reset( pRoom, pReset, 0/* Last slot*/ );

	newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
	newobj->cost = 0;
	obj_to_obj( newobj, to_obj );

	sprintf( output, "%s (%d) has been loaded into "
	    "%s (%d) and added to resets.\n\r",
	    capitalize( newobj->short_descr ),
	    newobj->pIndexData->vnum,
	    to_obj->short_descr,
	    to_obj->pIndexData->vnum );
	send_to_char( output, ch );
    }
    else
    /*
     * Load into mobile's inventory.
     */
    if ( ( to_mob = get_char_room( ch,NULL,arg2 ) ) != NULL )
    {
	int	wear_loc;

	/*
	 * Make sure the location on mobile is valid.
	 */
	if ( (wear_loc = flag_value( wear_loc_flags, argument )) == NO_FLAG )
	{
	    send_to_char( "REdit: Invalid wear_loc.  '? wear-loc'\n\r", ch );
	    return FALSE;
	}

	/*
	 * Disallow loading a sword(WEAR_WIELD) into WEAR_HEAD.
	 */
	if ( !IS_SET( pObjIndex->wear_flags, wear_bit(wear_loc) ) )
	{
	    sprintf( output,
	        "%s (%d) has wear flags: [%s]\n\r",
	        capitalize( pObjIndex->short_descr ),
	        pObjIndex->vnum,
		flag_string( wear_flags, pObjIndex->wear_flags ) );
	    send_to_char( output, ch );
	    return FALSE;
	}

	/*
	 * Can't load into same position.
	 */
	if ( get_eq_char( to_mob, wear_loc ) )
	{
	    send_to_char( "REdit:  Object already equipped.\n\r", ch );
	    return FALSE;
	}

	pReset		= new_reset_data();
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= wear_loc;
	if ( pReset->arg2 == WEAR_NONE )
	    pReset->command = 'G';
	else
	    pReset->command = 'E';
	pReset->arg3	= wear_loc;

	add_reset( pRoom, pReset, 0/* Last slot*/ );

	olevel  = URANGE( 0, to_mob->level - 2, LEVEL_HERO );
        newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

	if ( to_mob->pIndexData->pShop )	/* Shop-keeper? */
	{
	    switch ( pObjIndex->item_type )
	    {
	    default:		olevel = 0;				break;
	    case ITEM_PILL:	olevel = number_range(  0, 10 );	break;
	    case ITEM_POTION:	olevel = number_range(  0, 10 );	break;
	    case ITEM_SCROLL:	olevel = number_range(  5, 15 );	break;
	    case ITEM_WAND:	olevel = number_range( 10, 20 );	break;
	    case ITEM_STAFF:	olevel = number_range( 15, 25 );	break;
	    case ITEM_ARMOR:	olevel = number_range(  5, 15 );	break;
	    case ITEM_WEAPON:	if ( pReset->command == 'G' )
	    			    olevel = number_range( 5, 15 );
				else
				    olevel = number_fuzzy( olevel );
		break;
	    }

	    newobj = create_object( pObjIndex, olevel );
	    if ( pReset->arg2 == WEAR_NONE )
		SET_BIT( newobj->extra_flags, ITEM_INVENTORY );
	}
	else
	    newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

	obj_to_char( newobj, to_mob );
	if ( pReset->command == 'E' )
	    equip_char( to_mob, newobj, pReset->arg3 );

	sprintf( output, "%s (%d) has been loaded "
	    "%s of %s (%d) and added to resets.\n\r",
	    capitalize( pObjIndex->short_descr ),
	    pObjIndex->vnum,
	    flag_string( wear_loc_strings, pReset->arg3 ),
	    to_mob->short_descr,
	    to_mob->pIndexData->vnum );
	send_to_char( output, ch );
    }
    else	/* Display Syntax */
    {
	send_to_char( "REdit:  That mobile isn't here.\n\r", ch );
	return FALSE;
    }

    act( "$n has created $p!", ch, newobj, NULL, TO_ROOM );
    return TRUE;
}



/*
 * Object Editor Functions.
 */
void show_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];

    switch( obj->item_type )
    {
	default:	/* No values. */
	    break;
            
	case ITEM_LIGHT:
            if ( obj->value[2] == -1 || obj->value[2] == 999 ) /* ROM OLC */
		sprintf( buf, "{D[{Yv2{D]{Y Light:  Infinite[-1]\n\r" );
            else
		sprintf( buf, "{D[{Yv2{D]{Y Light:  {D[{W%d{D]\n\r{x", obj->value[2] );
	    send_to_char( buf, ch );
	    break;

	case ITEM_TOKEN:
	    sprintf( buf,
		"{D[{Yv0{D]{Y Quest Points:	{D[{W%d{D]\n\r{x", obj->value[0]);
	    send_to_char( buf, ch );
	    break;

	case ITEM_WAND:
	case ITEM_STAFF:
            sprintf( buf,
		"{D[{Yv0{D]{Y Level:          {D[{W%d{D]\n\r{x"
		"{D[{Yv1{D]{Y Charges Total:  {D[{W%d{D]\n\r{x"
		"{D[{Yv2{D]{Y Charges Left:   {D[{W%d{D]\n\r{x"
		"{D[{Yv3{D]{Y Spell:          {W%s\n\r{x",
		obj->value[0],
		obj->value[1],
		obj->value[2],
		obj->value[3] != -1 ? skill_table[obj->value[3]].name
		                    : "none" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_PORTAL:
	    sprintf( buf,
	        "{D[{Yv0{D]{Y Charges:        {D[{W%d{D]\n\r{x"
	        "{D[{Yv1{D]{Y Exit Flags:     {W%s\n\r{x"
	        "{D[{Yv2{D]{Y Portal Flags:   {W%s\n\r{x"
	        "{D[{Yv3{D]{Y Goes to (vnum): {D[{W%d{D]\n\r{x",
	        obj->value[0],
	        flag_string( exit_flags, obj->value[1]),
	        flag_string( portal_flags , obj->value[2]),
	        obj->value[3] );
	    send_to_char( buf, ch);
	    break;
	    
	case ITEM_FURNITURE:          
	    sprintf( buf,
	        "{D[{Yv0{D]{Y Max people:      {D[{W%d{D]\n\r{x"
	        "{D[{Yv1{D]{Y Max weight:      {D[{W%d{D]\n\r{x"
	        "{D[{Yv2{D]{Y Furniture Flags: {W%s\n\r{x"
	        "{D[{Yv3{D]{Y Heal bonus:      {D[{W%d{D]\n\r{x"
	        "{D[{Yv4{D]{Y Mana bonus:      {D[{W%d{D]\n\r{x",
	        obj->value[0],
	        obj->value[1],
	        flag_string( furniture_flags, obj->value[2]),
	        obj->value[3],
	        obj->value[4] );
	    send_to_char( buf, ch );
	    break;

	case ITEM_SCROLL:
	case ITEM_POTION:
	case ITEM_PILL:
            sprintf( buf,
		"{D[{Yv0{D]{Y Level:  {D[{W%d{D]\n\r{x"
		"{D[{Yv1{D]{Y Spell:  {W%s\n\r{x"
		"{D[{Yv2{D]{Y Spell:  {W%s\n\r{x"
		"{D[{Yv3{D]{Y Spell:  {W%s\n\r{x"
		"{D[{Yv4{D]{Y Spell:  {W%s\n\r{x",
		obj->value[0],
		obj->value[1] != -1 ? skill_table[obj->value[1]].name
		                    : "none",
		obj->value[2] != -1 ? skill_table[obj->value[2]].name
                                    : "none",
		obj->value[3] != -1 ? skill_table[obj->value[3]].name
		                    : "none",
		obj->value[4] != -1 ? skill_table[obj->value[4]].name
		                    : "none" );
	    send_to_char( buf, ch );
	    break;

/* ARMOR for ROM */

        case ITEM_ARMOR:
	    sprintf( buf,
		"{D[{Yv0{D]{Y Ac pierce       {D[{W%d{D]\n\r{x"
		"{D[{Yv1{D]{Y Ac bash         {D[{W%d{D]\n\r{x"
		"{D[{Yv2{D]{Y Ac slash        {D[{W%d{D]\n\r{x"
		"{D[{Yv3{D]{Y Ac exotic       {D[{W%d{D]\n\r{x",
		obj->value[0],
		obj->value[1],
		obj->value[2],
		obj->value[3] );
	    send_to_char( buf, ch );
	    break;

/* WEAPON changed in ROM: */
/* I had to split the output here, I have no idea why, but it helped -- Hugin */
/* It somehow fixed a bug in showing scroll/pill/potions too ?! */
	case ITEM_WEAPON:
		sprintf( buf, "{D[{Yv0{D]{Y Weapon class:   {W%s\n\r{x",
		     flag_string( weapon_class, obj->value[0] ) );
	    send_to_char( buf, ch );
	    sprintf( buf, "{D[{Yv1{D]{Y Number of dice: {D[{W%d{D]\n\r{x", obj->value[1] );
	    send_to_char( buf, ch );
	    sprintf( buf, "{D[{Yv2{D]{Y Type of dice:   {D[{W%d{D]\n\r{x", obj->value[2] );
	    send_to_char( buf, ch );
	    sprintf( buf, "{D[{Yv3{D]{Y Type:           {W%s\n\r{x",
		    attack_table[obj->value[3]].name );
	    send_to_char( buf, ch );
 	    sprintf( buf, "{D[{Yv4{D]{Y Special type:   {W%s\n\r{x",
		     flag_string( weapon_type2,  obj->value[4] ) );
	    send_to_char( buf, ch );
		if (obj->weapon_spell != -1
			&& obj->weapon_spell != 0)
		{
			sprintf(buf,"{D[{Yv5{D]{Y Spell:          {W%s\n\r{x",
			obj->weapon_spell != -1 ? skill_table[obj->weapon_spell].name: "none" );
			stc(buf,ch);
		}
	    break;

	case ITEM_GOLEM_PART:
		sprintf( buf, "{D[{Yv0{D]{Y Golem Part:   {W%s\n\r{x",
			flag_string( golem_parts, obj->value[0] ) );
	    send_to_char( buf, ch );
		break;

	case ITEM_SMITH_ORE:
		sprintf( buf, "{D[{Yv0{D]{Y Smithing Progress (must be 0):   {W%d\n\r{x",
			obj->value[0] );
	    send_to_char( buf, ch );
		break;

	case ITEM_CONTAINER:
	case ITEM_GOLEM_BAG:
	    sprintf( buf,
		"{D[{Yv0{D]{Y Weight:     {D[{W%d kg{D]\n\r{x"
		"{D[{Yv1{D]{Y Flags:      {D[{W%s{D]\n\r{x"
		"{D[{Yv2{D]{Y Key:     %s {D[{W%d{D]\n\r{x"
		"{D[{Yv3{D]{Y Capacity    {D[{W%d{D]\n\r{x"
		"{D[{Yv4{D]{Y Weight Mult {D[{W%d{D]\n\r{x",
		obj->value[0],
		flag_string( container_flags, obj->value[1] ),
                get_obj_index(obj->value[2])
                    ? get_obj_index(obj->value[2])->short_descr
                    : "none",
                obj->value[2],
                obj->value[3],
                obj->value[4] );
	    send_to_char( buf, ch );
	    break;

	case ITEM_DRINK_CON:
	    sprintf( buf,
	        "{D[{Yv0{D]{Y Liquid Total: {D[{W%d{D]\n\r{x"
	        "{D[{Yv1{D]{Y Liquid Left:  {D[{W%d{D]\n\r{x"
	        "{D[{Yv2{D]{Y Liquid:       {W%s\n\r{x"
	        "{D[{Yv3{D]{Y Poisoned:     {W%s\n\r{x",
	        obj->value[0],
	        obj->value[1],
	        liq_table[obj->value[2]].liq_name,
	        obj->value[3] != 0 ? "Yes" : "No" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_FOUNTAIN:
	    sprintf( buf,
	        "{D[{Yv0{D]{Y Liquid Total: {D[{W%d{D]\n\r{x"
	        "{D[{Yv1{D]{Y Liquid Left:  {D[{W%d{D]\n\r{x"
	        "{D[{Yv2{D]{Y Liquid:	    {W%s\n\r{x",
	        obj->value[0],
	        obj->value[1],
	        liq_table[obj->value[2]].liq_name );
	    send_to_char( buf,ch );
	    break;
	        
	case ITEM_FOOD:
	    sprintf( buf,
		"{D[{Yv0{D]{Y Food hours:	{D[{W%d{D]\n\r{x"
		"{D[{Yv1{D]{Y Full hours:	{D[{W%d{D]\n\r{x"
		"{D[{Yv3{D]{Y Poisoned:		{W%s\n\r{x"
		"{D[{Yv4{D]{Y Quest Points:	{D[{W%d{D]\n\r{x",
		obj->value[0],
		obj->value[1],
		obj->value[3] != 0 ? "Yes" : "No",
		obj->value[4]);
	    send_to_char( buf, ch );
	    break;

	case ITEM_MONEY:
            sprintf( buf, "{D[{Yv0{D]{Y Gold:   {D[{W%d{D]\n\r{x", obj->value[0] );
	    send_to_char( buf, ch );
	    break;

	case ITEM_QUIVER:
		sprintf( buf, "[v0] Number of Arrows:     %d\n\r", obj->value[0] );
		send_to_char( buf, ch);
		sprintf( buf, "[v1] Vnum of Arrows:       %d\n\r", obj->value[1] );
		send_to_char( buf, ch);
		sprintf( buf, "[v2] Type of Dice:         %d\n\r", obj->value[2] );
		send_to_char( buf, ch);		
		sprintf( buf, "[v3] Spell:                %s\n\r",
		obj->value[3] != -1 ? skill_table[obj->value[3]].name
				                    : "none" );
		send_to_char( buf, ch);
		break;

	case ITEM_ARROW:
		sprintf( buf, "[v1] Number of Dice:       %d\n\r", obj->value[1] );
		send_to_char( buf, ch);
		sprintf( buf, "[v2] Type of Dice:         %d\n\r", obj->value[2] );
		send_to_char( buf, ch);		
		sprintf( buf, "[v3] Spell:                %s\n\r",
		obj->value[3] != -1 ? skill_table[obj->value[3]].name
				                    : "none" );
		send_to_char( buf, ch);
		break;
    }

    return;
}



bool set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, char *argument)
{

    switch( pObj->item_type )
    {
        default:
            break;
            
        case ITEM_LIGHT:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_LIGHT" );
	            return FALSE;
	        case 2:
	            send_to_char( "HOURS OF LIGHT SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	    }
            break;

	case ITEM_TOKEN:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_TOKEN" );
	            return FALSE;
	        case 0:
	            send_to_char( "QUEST POINTS SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	    }
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_STAFF_WAND" );
	            return FALSE;
	        case 0:
	            send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "TOTAL NUMBER OF CHARGES SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "CURRENT NUMBER OF CHARGES SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	        case 3:
	            send_to_char( "SPELL TYPE SET.\n\r", ch );
	            pObj->value[3] = skill_lookup( argument );
	            break;
	    }
            break;

        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_SCROLL_POTION_PILL" );
	            return FALSE;
	        case 0:
	            send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "SPELL TYPE 1 SET.\n\r\n\r", ch );
	            pObj->value[1] = skill_lookup( argument );
	            break;
	        case 2:
	            send_to_char( "SPELL TYPE 2 SET.\n\r\n\r", ch );
	            pObj->value[2] = skill_lookup( argument );
	            break;
	        case 3:
	            send_to_char( "SPELL TYPE 3 SET.\n\r\n\r", ch );
	            pObj->value[3] = skill_lookup( argument );
	            break;
	        case 4:
	            send_to_char( "SPELL TYPE 4 SET.\n\r\n\r", ch );
	            pObj->value[4] = skill_lookup( argument );
	            break;
 	    }
	    break;

/* ARMOR for ROM: */

        case ITEM_ARMOR:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_ARMOR" );
		    return FALSE;
	        case 0:
		    send_to_char( "AC PIERCE SET.\n\r\n\r", ch );
		    pObj->value[0] = atoi( argument );
		    break;
	        case 1:
		    send_to_char( "AC BASH SET.\n\r\n\r", ch );
		    pObj->value[1] = atoi( argument );
		    break;
	        case 2:
		    send_to_char( "AC SLASH SET.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    break;
	        case 3:
		    send_to_char( "AC EXOTIC SET.\n\r\n\r", ch );
		    pObj->value[3] = atoi( argument );
		    break;
	    }
	    break;

/* WEAPONS changed in ROM */

        case ITEM_WEAPON:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_WEAPON" );
	            return FALSE;
	        case 0:
		    send_to_char( "WEAPON CLASS SET.\n\r\n\r", ch );
		    pObj->value[0] = flag_value( weapon_class, argument );
		    break;
	        case 1:
	            send_to_char( "NUMBER OF DICE SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "TYPE OF DICE SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	        case 3:
	            send_to_char( "WEAPON TYPE SET.\n\r\n\r", ch );
	            pObj->value[3] = attack_lookup( argument );
	            break;
	        case 4:
                    send_to_char( "SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch );
		    pObj->value[4] ^= (flag_value( weapon_type2, argument ) != NO_FLAG
		    ? flag_value( weapon_type2, argument ) : 0 );
		    break;

/*			case 5:
				stc("Working on this....\n\r",ch);
				send_to_char( "WEAPON SPELL SET.\n\r", ch );
				pObj->value[5] = skill_lookup( argument );
				sprintf(buf,"%d\n\r",pObj->value[5]);
				stc(buf,ch);
				break;*/
	    }
            break;

        case ITEM_GOLEM_PART:
	    switch ( value_num )
	    {
	        default:	return FALSE;
	        case 0:
		    send_to_char( "GOLEM PART SET\n\r\n\r", ch );
		    pObj->value[0] = flag_value( golem_parts, argument );
		    break;
		}
		break;

        case ITEM_SMITH_ORE:
	    switch ( value_num )
	    {
	        default:	return FALSE;
	        case 0:
		    send_to_char( "Smithing Progress set (must be 0, never change)\n\r\n\r", ch );
		    pObj->value[0] = atoi( argument );
		    break;
		}
		break;

	case ITEM_PORTAL:
	    switch ( value_num )
	    {
	        default:
	            do_help(ch, "ITEM_PORTAL" );
	            return FALSE;
	            
	    	case 0:
	    	    send_to_char( "CHARGES SET.\n\r\n\r", ch);
	    	    pObj->value[0] = atoi ( argument );
	    	    break;
	    	case 1:
	    	    send_to_char( "EXIT FLAGS SET.\n\r\n\r", ch);
	    	    pObj->value[1] = flag_value( exit_flags, argument );
	    	    break;
	    	case 2:
	    	    send_to_char( "PORTAL FLAGS SET.\n\r\n\r", ch);
	    	    pObj->value[2] = flag_value( portal_flags, argument );
	    	    break;
	    	case 3:
	    	    send_to_char( "EXIT VNUM SET.\n\r\n\r", ch);
	    	    pObj->value[3] = atoi ( argument );
	    	    break;
	   }
	   break;

	case ITEM_FURNITURE:
	    switch ( value_num )
	    {
	        default:
	            do_help( ch, "ITEM_FURNITURE" );
	            return FALSE;
	            
	        case 0:
	            send_to_char( "NUMBER OF PEOPLE SET.\n\r\n\r", ch);
	            pObj->value[0] = atoi ( argument );
	            break;
	        case 1:
	            send_to_char( "MAX WEIGHT SET.\n\r\n\r", ch);
	            pObj->value[1] = atoi ( argument );
	            break;
	        case 2:
	            send_to_char( "FURNITURE FLAGS TOGGLED.\n\r\n\r", ch);
	            pObj->value[2] ^= (flag_value( furniture_flags, argument ) != NO_FLAG
	            ? flag_value( furniture_flags, argument ) : 0);
	            break;
	        case 3:
	            send_to_char( "HEAL BONUS SET.\n\r\n\r", ch);
	            pObj->value[3] = atoi ( argument );
	            break;
	        case 4:
	            send_to_char( "MANA BONUS SET.\n\r\n\r", ch);
	            pObj->value[4] = atoi ( argument );
	            break;
	    }
	    break;
	   
		case ITEM_SHEATH:
			switch ( value_num )
			{
				case 0:
	            send_to_char( "SHEATH SLOTS SET\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
			}
			break;


        case ITEM_CONTAINER:
		case ITEM_GOLEM_BAG:
	    switch ( value_num )
	    {
		int value;
		
		default:
		    do_help( ch, "ITEM_CONTAINER" );
	            return FALSE;
		case 0:
	            send_to_char( "WEIGHT CAPACITY SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
		case 1:
	            if ( ( value = flag_value( container_flags, argument ) )
	              != NO_FLAG )
	        	TOGGLE_BIT(pObj->value[1], value);
		    else
		    {
			do_help ( ch, "ITEM_CONTAINER" );
			return FALSE;
		    }
	            send_to_char( "CONTAINER TYPE SET.\n\r\n\r", ch );
	            break;
		case 2:
		    if ( atoi(argument) != 0 )
		    {
			if ( !get_obj_index( atoi( argument ) ) )
			{
			    send_to_char( "THERE IS NO SUCH ITEM.\n\r\n\r", ch );
			    return FALSE;
			}

			if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
			{
			    send_to_char( "THAT ITEM IS NOT A KEY.\n\r\n\r", ch );
			    return FALSE;
			}
		    }
		    send_to_char( "CONTAINER KEY SET.\n\r\n\r", ch );
		    pObj->value[2] = atoi( argument );
		    break;
		case 3:
		    send_to_char( "CONTAINER MAX WEIGHT SET.\n\r", ch);
		    pObj->value[3] = atoi( argument );
		    break;
		case 4:
		    send_to_char( "WEIGHT MULTIPLIER SET.\n\r\n\r", ch );
		    pObj->value[4] = atoi ( argument );
		    break;
	    }
	    break;

	case ITEM_DRINK_CON:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_DRINK" );
/* OLC		    do_help( ch, "liquids" );    */
	            return FALSE;
	        case 0:
	            send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "LIQUID TYPE SET.\n\r\n\r", ch );
	            pObj->value[2] = ( liq_lookup(argument) != -1 ?
	            		       liq_lookup(argument) : 0 );
	            break;
	        case 3:
	            send_to_char( "POISON VALUE TOGGLED.\n\r\n\r", ch );
	            pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
	            break;
	    }
            break;

	case ITEM_FOUNTAIN:
	    switch (value_num)
	    {
	    	default:
		    do_help( ch, "ITEM_FOUNTAIN" );
/* OLC		    do_help( ch, "liquids" );    */
	            return FALSE;
	        case 0:
	            send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "LIQUID TYPE SET.\n\r\n\r", ch );
	            pObj->value[2] = ( liq_lookup( argument ) != -1 ?
	            		       liq_lookup( argument ) : 0 );
	            break;
            }
	break;
		    	
	case ITEM_FOOD:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_FOOD" );
	            return FALSE;
	        case 0:
	            send_to_char( "HOURS OF FOOD SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "HOURS OF FULL SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 3:
	            send_to_char( "POISON VALUE TOGGLED.\n\r\n\r", ch );
	            pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
	            break;

	        case 4:
	            send_to_char( "QUEST POINTS SET.\n\r\n\r", ch );
	            pObj->value[4] = atoi( argument );
	            break;
	    }
            break;

	case ITEM_MONEY:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_MONEY" );
	            return FALSE;
	        case 0:
	            send_to_char( "GOLD AMOUNT SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
		case 1:
		    send_to_char( "SILVER AMOUNT SET.\n\r\n\r", ch );
		    pObj->value[1] = atoi( argument );
		    break;
	    }
            break;

	case ITEM_QUIVER:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_QUIVER" );
	            return FALSE;
	        case 0:
	            send_to_char( "NUMBER OF ARROWS SET.\n\r\n\r", ch );
	            pObj->value[0] = atoi( argument );
	            break;
	        case 1:
	            send_to_char( "VNUM OF ARROWS SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "TYPE OF DICE SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	    }
            break;

	case ITEM_ARROW:
	    switch ( value_num )
	    {
	        default:
		    do_help( ch, "ITEM_ARROW" );
	            return FALSE;
				
	        case 1:
	            send_to_char( "NUMBER OF DICE SET.\n\r\n\r", ch );
	            pObj->value[1] = atoi( argument );
	            break;
	        case 2:
	            send_to_char( "TYPE OF DICE SET.\n\r\n\r", ch );
	            pObj->value[2] = atoi( argument );
	            break;
	    }
            break;

    }

    show_obj_values( ch, pObj );

    return TRUE;
}



OEDIT( oedit_show )
{
    OBJ_INDEX_DATA *pObj;
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
	PROG_LIST		*list;
    int cnt;

    EDIT_OBJ(ch, pObj);

    sprintf( buf, "{YName:        {D[{W%s{D]\n\r{YArea:        {D[{W%5d{D] {W%s\n\r{x",
	pObj->name,
	!pObj->area ? -1        : pObj->area->vnum,
	!pObj->area ? "No Area" : pObj->area->name );
    send_to_char( buf, ch );


    sprintf( buf, "{YVnum:        {D[{W%5d{D]{X\n\r{YType:        {D[{W%s{D]{x\n\r",
	pObj->vnum,
	flag_string( type_flags, pObj->item_type ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YLevel:       {D[{W%5d{D]{x\n\r", pObj->level );
    send_to_char( buf, ch );

    sprintf( buf, "{YWear flags:  {D[{W%s{D]{x\n\r",
	flag_string( wear_flags, pObj->wear_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YExtra flags: {D[{W%s{D]{x\n\r",
	flag_string( extra_flags, pObj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YMaterial:    {D[{W%s{D]{x\n\r",                /* ROM */
	pObj->material );
    send_to_char( buf, ch );

    sprintf( buf, "{YCondition:   {D[{W%5d{D]{x\n\r",               /* ROM */
	pObj->condition );
    send_to_char( buf, ch );

    sprintf( buf, "{YWeight:      {D[{W%5d{D]{Y\n\rCost:        {D[{W%5d{D]{x\n\r",
	pObj->weight, pObj->cost );
    send_to_char( buf, ch );

    if ( pObj->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char( "{YEx desc kwd: ", ch );

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    send_to_char( "{D[{W", ch );
	    send_to_char( ed->keyword, ch );
	    send_to_char( "{D]", ch );
	}

	send_to_char( "{x\n\r", ch );
    }

    sprintf( buf, "{YShort desc:  {W%s\n\r{x{YLong desc:\n\r     {W%s\n\r{x",
	pObj->short_descr, pObj->description );
	stc(buf,ch);
    sprintf( buf, "{YOriginal SHort:  {W%s\n\r{x", pObj->orig_short);
    send_to_char( buf, ch );

    for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next )
    {
	if ( cnt == 0 )
	{
	    send_to_char( "{GNumber {YModifier {RAffects\n\r", ch );
	    send_to_char( "{G------ -------- -------\n\r", ch );
	}
	sprintf( buf, "{D[{G%4d{D] {Y%-8d {R%s\n\r{x", cnt,
	    paf->modifier,
	    flag_string( apply_flags, paf->location ) );
	send_to_char( buf, ch );
	cnt++;
    }

    show_obj_values( ch, pObj );

	    if ( pObj->oprogs )
    {
	int cnt;

	sprintf(buf, "\n\rOBJPrograms for [%5d]:\n\r", pObj->vnum);
	send_to_char( buf, ch );

	for (cnt=0, list=pObj->oprogs; list; list=list->next)
	{
		if (cnt ==0)
		{
			send_to_char ( " Number Vnum Trigger Phrase\n\r", ch );
			send_to_char ( " ------ ---- ------- ------\n\r", ch );
		}

		sprintf(buf, "[%5d] %4d %7s %s\n\r", cnt,
			list->vnum,prog_type_to_name(list->trig_type),
			list->trig_phrase);
		send_to_char( buf, ch );
		cnt++;
	}
    }
    return FALSE;
}


/*
 * Need to issue warning if flag isn't valid. -- does so now -- Hugin.
 */
OEDIT( oedit_addaffect )
{
    int value;
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, loc );
    one_argument( argument, mod );

    if ( loc[0] == '\0' || mod[0] == '\0' || !is_number( mod ) )
    {
	send_to_char( "Syntax:  addaffect [location] [#xmod]\n\r", ch );
	return FALSE;
    }

    if ( ( value = flag_value( apply_flags, loc ) ) == NO_FLAG ) /* Hugin */
    {
        send_to_char( "Valid affects are:\n\r", ch );
	show_help( ch, "apply" );
	return FALSE;
    }

    pAf             =   new_affect();
    pAf->location   =   value;
    pAf->modifier   =   atoi( mod );
    pAf->where	    =   TO_OBJECT;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->level      =	pObj->level;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;

    send_to_char( "Affect added.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_addapply )
{
    int value,bv,typ;
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];
    char type[MAX_STRING_LENGTH];
    char bvector[MAX_STRING_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, type );
    argument = one_argument( argument, loc );
    argument = one_argument( argument, mod );
    one_argument( argument, bvector );

    if ( type[0] == '\0' || ( typ = flag_value( apply_types, type ) ) == NO_FLAG )
    {
    	send_to_char( "Invalid apply type. Valid apply types are:\n\r", ch);
    	show_help( ch, "apptype" );
    	return FALSE;
    }

    if ( loc[0] == '\0' || ( value = flag_value( apply_flags, loc ) ) == NO_FLAG )
    {
        send_to_char( "Valid applys are:\n\r", ch );
	show_help( ch, "apply" );
	return FALSE;
    }

    if ( bvector[0] == '\0' || ( bv = flag_value( bitvector_type[typ].table, bvector ) ) == NO_FLAG )
    {
    	send_to_char( "Invalid bitvector type.\n\r", ch );
	send_to_char( "Valid bitvector types are:\n\r", ch );
	show_help( ch, bitvector_type[typ].help );
    	return FALSE;
    }

    if ( mod[0] == '\0' || !is_number( mod ) )
    {
	send_to_char( "Syntax:  addapply [type] [location] [#xmod] [bitvector]\n\r", ch );
	return FALSE;
    }

    pAf             =   new_affect();
    pAf->location   =   value;
    pAf->modifier   =   atoi( mod );
    pAf->where	    =   apply_types[typ].bit;
    pAf->type	    =	-1;
    pAf->duration   =   -1;
    pAf->bitvector  =   bv;
    pAf->level      =	pObj->level;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;

    send_to_char( "Apply added.\n\r", ch);
    return TRUE;
}

/*
 * My thanks to Hans Hvidsten Birkeland and Noam Krendel(Walker)
 * for really teaching me how to manipulate pointers.
 */
OEDIT( oedit_delaffect )
{
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    AFFECT_DATA *pAf_next;
    char affect[MAX_STRING_LENGTH];
    int  value;
    int  cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument( argument, affect );

    if ( !is_number( affect ) || affect[0] == '\0' )
    {
	send_to_char( "Syntax:  delaffect [#xaffect]\n\r", ch );
	return FALSE;
    }

    value = atoi( affect );

    if ( value < 0 )
    {
	send_to_char( "Only non-negative affect-numbers allowed.\n\r", ch );
	return FALSE;
    }

    if ( !( pAf = pObj->affected ) )
    {
	send_to_char( "OEdit:  Non-existant affect.\n\r", ch );
	return FALSE;
    }

    if( value == 0 )	/* First case: Remove first affect */
    {
	pAf = pObj->affected;
	pObj->affected = pAf->next;
	free_affect( pAf );
    }
    else		/* Affect to remove is not the first */
    {
	while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
	     pAf = pAf_next;

	if( pAf_next )		/* See if it's the next affect */
	{
	    pAf->next = pAf_next->next;
	    free_affect( pAf_next );
	}
	else                                 /* Doesn't exist */
	{
	     send_to_char( "No such affect.\n\r", ch );
	     return FALSE;
	}
    }

    send_to_char( "Affect removed.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_name )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [string]\n\r", ch );
	return FALSE;
    }

    free_string( pObj->name );
    pObj->name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_short )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  short [string]\n\r", ch );
	return FALSE;
    }

    free_string( pObj->short_descr );
    pObj->short_descr = str_dup( argument );
	free_string( pObj->orig_short	);
	pObj->orig_short = str_dup ( argument );

    send_to_char( "Short description set.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_long )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  long [string]\n\r", ch );
	return FALSE;
    }
        
    free_string( pObj->description );
    pObj->description = str_dup( argument );
    pObj->description[0] = UPPER( pObj->description[0] );

    send_to_char( "Long description set.\n\r", ch);
    return TRUE;
}



bool set_value( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, char *argument, int value )
{
    if ( argument[0] == '\0' )
    {
	set_obj_values( ch, pObj, -1, "" );     /* '\0' changed to "" -- Hugin */
	return FALSE;
    }

    if ( set_obj_values( ch, pObj, value, argument ) )
	return TRUE;

    return FALSE;
}



/*****************************************************************************
 Name:		oedit_values
 Purpose:	Finds the object and sets its value.
 Called by:	The four valueX functions below. (now five -- Hugin )
 ****************************************************************************/
bool oedit_values( CHAR_DATA *ch, char *argument, int value )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( set_value( ch, pObj, argument, value ) )
        return TRUE;

    return FALSE;
}


OEDIT( oedit_value0 )
{
    if ( oedit_values( ch, argument, 0 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_value1 )
{
    if ( oedit_values( ch, argument, 1 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_value2 )
{
    if ( oedit_values( ch, argument, 2 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_value3 )
{
    if ( oedit_values( ch, argument, 3 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_value4 )
{
    if ( oedit_values( ch, argument, 4 ) )
        return TRUE;

    return FALSE;
}

OEDIT( oedit_value5 )
{
    if ( oedit_values( ch, argument, 5 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_weight )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  weight [number]\n\r", ch );
	return FALSE;
    }

    pObj->weight = atoi( argument );

    send_to_char( "Weight set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_cost )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  cost [number]\n\r", ch );
	return FALSE;
    }

    pObj->cost = atoi( argument );

    send_to_char( "Cost set.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_create )
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    int  value;
    int  iHash;

    value = atoi( argument );
    if ( argument[0] == '\0' || value == 0 )
    {
	send_to_char( "Syntax:  oedit create [vnum]\n\r", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "OEdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_obj_index( value ) )
    {
	send_to_char( "OEdit:  Object vnum already exists.\n\r", ch );
	return FALSE;
    }
        
    pObj			= new_obj_index();
    pObj->vnum			= value;
    pObj->area			= pArea;
        
    if ( value > top_vnum_obj )
	top_vnum_obj = value;

    iHash			= value % MAX_KEY_HASH;
    pObj->next			= obj_index_hash[iHash];
    obj_index_hash[iHash]	= pObj;
    ch->desc->pEdit		= (void *)pObj;

    send_to_char( "Object Created.\n\r", ch );
    return TRUE;
}



OEDIT( oedit_ed )
{
    OBJ_INDEX_DATA *pObj;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, command );
    one_argument( argument, keyword );

    if ( command[0] == '\0' )
    {
	send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	send_to_char( "         ed delete [keyword]\n\r", ch );
	send_to_char( "         ed edit [keyword]\n\r", ch );
	send_to_char( "         ed format [keyword]\n\r", ch );
	return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	    return FALSE;
	}

	ed                  =   new_extra_descr();
	ed->keyword         =   str_dup( keyword );
	ed->next            =   pObj->extra_descr;
	pObj->extra_descr   =   ed;

	string_append( ch, &ed->description );

	return TRUE;
    }

    if ( !str_cmp( command, "edit" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	}

	if ( !ed )
	{
	    send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	string_append( ch, &ed->description );

	return TRUE;
    }

    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
	    send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}

	if ( !ped )
	    pObj->extra_descr = ed->next;
	else
	    ped->next = ed->next;

	free_extra_descr( ed );

	send_to_char( "Extra description deleted.\n\r", ch );
	return TRUE;
    }


    if ( !str_cmp( command, "format" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;

	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed format [keyword]\n\r", ch );
	    return FALSE;
	}

	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}

	if ( !ed )
	{
                send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
                return FALSE;
	}

	ed->description = format_string( ed->description );

	send_to_char( "Extra description formatted.\n\r", ch );
	return TRUE;
    }

    oedit_ed( ch, "" );
    return FALSE;
}





/* ROM object functions : */

OEDIT( oedit_extra )      /* Moved out of oedit() due to naming conflicts -- Hugin */
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_OBJ(ch, pObj);

	if ( ( value = flag_value( extra_flags, argument ) ) != NO_FLAG )
	{
	    TOGGLE_BIT(pObj->extra_flags, value);

	    send_to_char( "Extra flag toggled.\n\r", ch);
	    return TRUE;
	}
    }

    send_to_char( "Syntax:  extra [flag]\n\r"
		  "Type '? extra' for a list of flags.\n\r", ch );
    return FALSE;
}


OEDIT( oedit_wear )      /* Moved out of oedit() due to naming conflicts -- Hugin */
{
    OBJ_INDEX_DATA *pObj;
    int value;

     if ( argument[0] != '\0' )
    {
	EDIT_OBJ(ch, pObj);

	if ( ( value = flag_value( wear_flags, argument ) ) != NO_FLAG )
	{
	    TOGGLE_BIT(pObj->wear_flags, value);

	    send_to_char( "Wear flag toggled.\n\r", ch);
	    return TRUE;
	}
    }

    send_to_char( "Syntax:  wear [flag]\n\r"
		  "Type '? wear' for a list of flags.\n\r", ch );
    return FALSE;
}


OEDIT( oedit_type )      /* Moved out of oedit() due to naming conflicts -- Hugin */
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_OBJ(ch, pObj);

	if ( ( value = flag_value( type_flags, argument ) ) != NO_FLAG )
	{
	    pObj->item_type = value;

	    send_to_char( "Type set.\n\r", ch);

	    /*
	     * Clear the values.
	     */
	    pObj->value[0] = 0;
	    pObj->value[1] = 0;
	    pObj->value[2] = 0;
	    pObj->value[3] = 0;
	    pObj->value[4] = 0;     /* ROM */

	    return TRUE;
	}
    }

    send_to_char( "Syntax:  type [flag]\n\r"
		  "Type '? type' for a list of flags.\n\r", ch );
    return FALSE;
}

OEDIT( oedit_material )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  material [string]\n\r", ch );
	return FALSE;
    }

    free_string( pObj->material );
    pObj->material = str_dup( argument );

    send_to_char( "Material set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_level )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }

    pObj->level = atoi( argument );

    send_to_char( "Level set.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_condition )
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] != '\0'
    && ( value = atoi (argument ) ) >= 0
    && ( value <= 100 ) )
    {
	EDIT_OBJ( ch, pObj );

	pObj->condition = value;
	send_to_char( "Condition set.\n\r", ch );

	return TRUE;
    }

    send_to_char( "Syntax:  condition [number]\n\r"
		  "Where number can range from 0 (ruined) to 100 (perfect).\n\r",
		  ch );
    return FALSE;
}





/*
 * Mobile Editor Functions.
 */
MEDIT( medit_show )
{
    MOB_INDEX_DATA *pMob;
    char buf[MAX_STRING_LENGTH];
    PROG_LIST *list;

    EDIT_MOB(ch, pMob);

    sprintf( buf, "{YName:        {D[{W%s{D]{x\n\r{YArea:        {D[{W%5d{D] {W%s\n\r{x",
	pMob->player_name,
	!pMob->area ? -1        : pMob->area->vnum,
	!pMob->area ? "No Area" : pMob->area->name );
    send_to_char( buf, ch );

    sprintf( buf, "{YAct:         {D[{W%s{D]{x\n\r",
	flag_string( act_flags, pMob->act ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YVnum:        {D[{W%5d{D]{y Sex:   {D[{W%s{D]   {YRace: {D[{W%s{D]{x\n\r",
	pMob->vnum,
	pMob->sex == SEX_MALE    ? "male   " :
	pMob->sex == SEX_FEMALE  ? "female " : 
	pMob->sex == 3           ? "random " : "neutral",
	race_table[pMob->race].name );
    send_to_char( buf, ch );

    sprintf( buf,
		  "{YLevel:       {D[{W%2d{D]    {YAlign: {D[{W%4d{D]      {YHitroll: {D[{W%2d{D] {YDam Type:    {D[{W%s{D]{x\n\r",
	pMob->level,	pMob->alignment,
	pMob->hitroll,	attack_table[pMob->dam_type].name );
    send_to_char( buf, ch );

    if ( pMob->group )
    {
	sprintf( buf, "{YGroup:       {D[{W%5d{D]{x\n\r", pMob->group );
	send_to_char( buf, ch );
    }

    sprintf( buf, "{YHit dice:    {D[{W%2ldd%-3ld+%4ld{D]{x ",
	     pMob->hit[DICE_NUMBER],
	     pMob->hit[DICE_TYPE],
	     pMob->hit[DICE_BONUS] );
    send_to_char( buf, ch );

    sprintf( buf, "{YDamage dice: {D[{W%2dd%-3d+%4d{D]{x",
	     pMob->damage[DICE_NUMBER],
	     pMob->damage[DICE_TYPE],
	     pMob->damage[DICE_BONUS] );
    send_to_char( buf, ch );

    sprintf( buf, "{YMana dice:   {D[{W%2dd%-3d+%4d{D]{x\n\r",
	     pMob->mana[DICE_NUMBER],
	     pMob->mana[DICE_TYPE],
	     pMob->mana[DICE_BONUS] );
    send_to_char( buf, ch );

/* ROM values end */

    sprintf( buf, "{YAffected by: {D[{W%s{D]{x\n\r",
	flag_string( affect_flags, pMob->affected_by ) );
    send_to_char( buf, ch );

	sprintf( buf, "{YSecondary Affects: {D[{W%s{D]{x\n\r",
	flag_string( affect2_flags, pMob->affected2_by ) );
    send_to_char( buf, ch );

/* ROM values: */

    sprintf( buf, "{YArmor:       {D[{YPierce: {W%d  {YBash: {W%d  {YSlash: {W%d  {YMagic: {W%d{D]{x\n\r",
	pMob->ac[AC_PIERCE], pMob->ac[AC_BASH],
	pMob->ac[AC_SLASH], pMob->ac[AC_EXOTIC] );
    send_to_char( buf, ch );

    sprintf( buf, "{YForm:        {D[{W%s{D]{x\n\r",
	flag_string( form_flags, pMob->form ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YParts:       {D[{W%s{D]{x\n\r",
	flag_string( part_flags, pMob->parts ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YImm:         {D[{W%s{D]{W\n\r",
	flag_string( imm_flags, pMob->imm_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YRes:         {D[{W%s{D]{x\n\r",
	flag_string( res_flags, pMob->res_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YVuln:        {D[{W%s{D]{x\n\r",
	flag_string( vuln_flags, pMob->vuln_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YOff:         {D[{W%s{D]{x\n\r",
	flag_string( off_flags,  pMob->off_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YTeach:         {D[{W%s{D]{x\n\r",
	flag_string( teach_flags,  pMob->teach_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YSize:        {D[{W%s{D]{x\n\r",
	flag_string( size_flags, pMob->size ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YMaterial:    {D[{W%s{D]{x\n\r",
        pMob->material );
    send_to_char( buf, ch );

    sprintf( buf, "{YStart pos:	{D[{W%s{D]{x\n\r",
	flag_string( position_flags, pMob->start_pos ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YDefault pos:	{D[{W%s{D]{x\n\r",
	flag_string( position_flags, pMob->default_pos ) );
    send_to_char( buf, ch );

    sprintf( buf, "{YWealth:      {D[{W%5ld{D]{x\n\r",
	pMob->wealth );
    send_to_char( buf, ch );

/* ROM values end */

    if ( pMob->spec_fun )
    {
	sprintf( buf, "{YSpec fun:    {D[{W%s{D]{x\n\r",  spec_name( pMob->spec_fun ) );
	send_to_char( buf, ch );
    }

    if ( pMob->clan )
    {
        sprintf( buf, "{YClan:        {W[%s]\n\r", clan_table[pMob->clan].name );
		send_to_char( buf, ch );
    }

    sprintf( buf, "{YShort descr: {W%s\n\r{x{YLong descr:\n\r%s{x",
	pMob->short_descr,
	pMob->long_descr );
    send_to_char( buf, ch );

    sprintf( buf, "{YDescription:{W\n\r%s{x", pMob->description );
    send_to_char( buf, ch );

    if ( pMob->pShop )
    {
	SHOP_DATA *pShop;
	int iTrade;

	pShop = pMob->pShop;

	sprintf( buf,
	  "{YShop data for {D[{W%5d{D]:\n\r{x"
	  "  {YMarkup for purchaser: {W%d%%\n\r{x"
	  "  {YMarkdown for seller:  {W%d%%\n\r{x",
	    pShop->keeper, pShop->profit_buy, pShop->profit_sell );
	send_to_char( buf, ch );
	sprintf( buf, "  {YHours: {W%d to %d.\n\r{x",
	    pShop->open_hour, pShop->close_hour );
	send_to_char( buf, ch );

	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	{
	    if ( pShop->buy_type[iTrade] != 0 )
	    {
		if ( iTrade == 0 ) {
		    send_to_char( "  {GNumber {YTrades Type\n\r", ch );
		    send_to_char( "  {G------ {Y-----------\n\r", ch );
		}
		sprintf( buf, "  {D[{G%4d{D] {Y{W%s\n\r{x", iTrade,
		    flag_string( type_flags, pShop->buy_type[iTrade] ) );
		send_to_char( buf, ch );
	    }
	}
    }

    if ( pMob->mprogs )
    {
	int cnt;

	sprintf(buf, "\n\rMOBPrograms for [%5d]:\n\r", pMob->vnum);
	send_to_char( buf, ch );

	for (cnt=0, list=pMob->mprogs; list; list=list->next)
	{
		if (cnt ==0)
		{
			send_to_char ( " {GNumber {YVnum {RTrigger {CPhrase\n\r", ch );
			send_to_char ( " {G------ {Y---- {R------- {C------\n\r", ch );
		}

		sprintf(buf, "{D[{G%5d{D] {Y%4d {R%7s {C%s\n\r{x", cnt,
			list->vnum,prog_type_to_name(list->trig_type),
			list->trig_phrase);
		send_to_char( buf, ch );
		cnt++;
	}
    }

    return FALSE;
}



MEDIT( medit_create )
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int  value;
    int  iHash;

    value = atoi( argument );
    if ( argument[0] == '\0' || value == 0 )
    {
	send_to_char( "Syntax:  medit create [vnum]\n\r", ch );
	return FALSE;
    }

    pArea = get_vnum_area( value );

    if ( !pArea )
    {
	send_to_char( "MEdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }

    if ( get_mob_index( value ) )
    {
	send_to_char( "MEdit:  Mobile vnum already exists.\n\r", ch );
	return FALSE;
    }

    pMob				= new_mob_index();
    pMob->vnum			= value;
    pMob->area			= pArea;
        
    if ( value > top_vnum_mob )
	top_vnum_mob = value;        

    pMob->act			= ACT_IS_NPC;
	//pMob->act ^= flag_value( act_flags, ACT_NO_QUEST );
    iHash			= value % MAX_KEY_HASH;
    pMob->next			= mob_index_hash[iHash];
    mob_index_hash[iHash]	= pMob;
    ch->desc->pEdit		= (void *)pMob;

    send_to_char( "Mobile Created.\n\r", ch );
/*    do_function(ch, &medit_act, "no_quest"); */
    return TRUE;
}


MEDIT( medit_spec )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  spec [special function]\n\r", ch );
	return FALSE;
    }


    if ( !str_cmp( argument, "none" ) )
    {
        pMob->spec_fun = NULL;

        send_to_char( "Spec removed.\n\r", ch);
        return TRUE;
    }

    if ( spec_lookup( argument ) )
    {
	pMob->spec_fun = spec_lookup( argument );
	send_to_char( "Spec set.\n\r", ch);
	return TRUE;
    }

    send_to_char( "MEdit: No such special function.\n\r", ch );
    return FALSE;
}

MEDIT( medit_damtype )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  damtype [damage message]\n\r", ch );
	send_to_char( "For a list of weapon damtypes, type '? weapon'.\n\r", ch );
	return FALSE;
    }

    pMob->dam_type = attack_lookup(argument);
    send_to_char( "Damage type set.\n\r", ch);
    return TRUE;
}


MEDIT( medit_align )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  alignment [number]\n\r", ch );
	return FALSE;
    }

    pMob->alignment = atoi( argument );

    send_to_char( "Alignment set.\n\r", ch);
    return TRUE;
}



MEDIT( medit_level )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }

    pMob->level = atoi( argument );

    send_to_char( "Level set.\n\r", ch);
    return TRUE;
}



MEDIT( medit_desc )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	string_append( ch, &pMob->description );
	return TRUE;
    }

    send_to_char( "Syntax:  desc    - line edit\n\r", ch );
    return FALSE;
}




MEDIT( medit_long )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  long [string]\n\r", ch );
	return FALSE;
    }

    free_string( pMob->long_descr );
    strcat( argument, "\n\r" );
    pMob->long_descr = str_dup( argument );
    pMob->long_descr[0] = UPPER( pMob->long_descr[0]  );

    send_to_char( "Long description set.\n\r", ch);
    return TRUE;
}



MEDIT( medit_short )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  short [string]\n\r", ch );
	return FALSE;
    }

    free_string( pMob->short_descr );
    pMob->short_descr = str_dup( argument );

    send_to_char( "Short description set.\n\r", ch);
    return TRUE;
}



MEDIT( medit_name )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [string]\n\r", ch );
	return FALSE;
    }

    free_string( pMob->player_name );
    pMob->player_name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_shop )
{
    MOB_INDEX_DATA *pMob;
    char command[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );
    argument = one_argument( argument, arg1 );

    EDIT_MOB(ch, pMob);

    if ( command[0] == '\0' )
    {
	send_to_char( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
	send_to_char( "         shop profit [#xbuying%] [#xselling%]\n\r", ch );
	send_to_char( "         shop type [#x0-4] [item type]\n\r", ch );
	send_to_char( "         shop assign\n\r", ch );
	send_to_char( "         shop remove\n\r", ch );
	return FALSE;
    }


    if ( !str_cmp( command, "hours" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    send_to_char( "MEdit:  Debes crear un shop primero (shop assign).\n\r", ch );
	    return FALSE;
	}

	pMob->pShop->open_hour = atoi( arg1 );
	pMob->pShop->close_hour = atoi( argument );

	send_to_char( "Shop hours set.\n\r", ch);
	return TRUE;
    }


    if ( !str_cmp( command, "profit" ) )
    {
	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char( "Syntax:  shop profit [#xbuying%] [#xselling%]\n\r", ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    send_to_char( "MEdit:  Debes crear un shop primero (shop assign).\n\r", ch );
	    return FALSE;
	}

	pMob->pShop->profit_buy     = atoi( arg1 );
	pMob->pShop->profit_sell    = atoi( argument );

	send_to_char( "Shop profit set.\n\r", ch);
	return TRUE;
    }


    if ( !str_cmp( command, "type" ) )
    {
	char buf[MAX_INPUT_LENGTH];
	int value;

	if ( arg1[0] == '\0' || !is_number( arg1 )
	|| argument[0] == '\0' )
	{
	    send_to_char( "Syntax:  shop type [#x0-4] [item type]\n\r", ch );
	    return FALSE;
	}

	if ( atoi( arg1 ) >= MAX_TRADE )
	{
	    sprintf( buf, "MEdit:  May sell %d items max.\n\r", MAX_TRADE );
	    send_to_char( buf, ch );
	    return FALSE;
	}

	if ( !pMob->pShop )
	{
	    send_to_char( "MEdit:  Debes crear un shop primero (shop assign).\n\r", ch );
	    return FALSE;
	}

	if ( ( value = flag_value( type_flags, argument ) ) == NO_FLAG )
	{
	    send_to_char( "MEdit:  That type of item is not known.\n\r", ch );
	    return FALSE;
	}

	pMob->pShop->buy_type[atoi( arg1 )] = value;

	send_to_char( "Shop type set.\n\r", ch);
	return TRUE;
    }

    /* shop assign && shop delete by Phoenix */

    if ( !str_prefix(command, "assign") )
    {
    	if ( pMob->pShop )
    	{
        	send_to_char("Mob already has a shop assigned to it.\n\r", ch);
        	return FALSE;
	}

	pMob->pShop		= new_shop();
	if ( !shop_first )
        	shop_first	= pMob->pShop;
	if ( shop_last )
		shop_last->next	= pMob->pShop;
	shop_last		= pMob->pShop;

	pMob->pShop->keeper	= pMob->vnum;

	send_to_char("New shop assigned to mobile.\n\r", ch);
	return TRUE;
    }

    if ( !str_prefix(command, "remove") )
    {
	SHOP_DATA *pShop;

	pShop		= pMob->pShop;
	pMob->pShop	= NULL;

	if ( pShop == shop_first )
	{
		if ( !pShop->next )
		{
			shop_first = NULL;
			shop_last = NULL;
		}
		else
			shop_first = pShop->next;
	}
	else
	{
		SHOP_DATA *ipShop;

		for ( ipShop = shop_first; ipShop; ipShop = ipShop->next )
		{
			if ( ipShop->next == pShop )
			{
				if ( !pShop->next )
				{
					shop_last = ipShop;
					shop_last->next = NULL;
				}
				else
					ipShop->next = pShop->next;
			}
		}
	}

	free_shop(pShop);

	send_to_char("Mobile is no longer a shopkeeper.\n\r", ch);
	return TRUE;
    }

    medit_shop( ch, "" );
    return FALSE;
}


/* ROM medit functions: */


MEDIT( medit_sex )          /* Moved out of medit() due to naming conflicts -- Hugin */
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( sex_flags, argument ) ) != NO_FLAG )
	{
	    pMob->sex = value;

	    send_to_char( "Sex set.\n\r", ch);
	    return TRUE;
	}
    }

    send_to_char( "Syntax: sex [sex]\n\r"
		  "Type '? sex' for a list of flags.\n\r", ch );
    return FALSE;
}


MEDIT( medit_act )          /* Moved out of medit() due to naming conflicts -- Hugin */
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( act_flags, argument ) ) != NO_FLAG )
	{
	    pMob->act ^= value;
	    SET_BIT( pMob->act, ACT_IS_NPC );

	    send_to_char( "Act flag toggled.\n\r", ch);
	    return TRUE;
	}
    }

    send_to_char( "Syntax: act [flag]\n\r"
		  "Type '? act' for a list of flags.\n\r", ch );
    return FALSE;
}


MEDIT( medit_affect )      /* Moved out of medit() due to naming conflicts -- Hugin */
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( affect_flags, argument ) ) != NO_FLAG )
	{
	    pMob->affected_by ^= value;

	    send_to_char( "Affect flag toggled.\n\r", ch);
	    return TRUE;
	}
    }

    send_to_char( "Syntax: affect [flag]\n\r"
		  "Type '? affect' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_affect2 )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( affect2_flags, argument ) ) != NO_FLAG )
	{
	    pMob->affected2_by ^= value;

	    send_to_char( "Affect2 flag toggled.\n\r", ch);
	    return TRUE;
	}
    }

    send_to_char( "Syntax: affect2 [flag]\n\r"
		  "Type '? affect2' for a list of flags.\n\r", ch );
    return FALSE;
}



MEDIT( medit_ac )
{
    MOB_INDEX_DATA *pMob;
    char arg[MAX_INPUT_LENGTH];
    int pierce, bash, slash, exotic;

    do   /* So that I can use break and send the syntax in one place */
    {
	if ( argument[0] == '\0' )  break;

	EDIT_MOB(ch, pMob);
	argument = one_argument( argument, arg );

	if ( !is_number( arg ) )  break;
	pierce = atoi( arg );
	argument = one_argument( argument, arg );

	if ( arg[0] != '\0' )
	{
	    if ( !is_number( arg ) )  break;
	    bash = atoi( arg );
	    argument = one_argument( argument, arg );
	}
	else
	    bash = pMob->ac[AC_BASH];

	if ( arg[0] != '\0' )
	{
	    if ( !is_number( arg ) )  break;
	    slash = atoi( arg );
	    argument = one_argument( argument, arg );
	}
	else
	    slash = pMob->ac[AC_SLASH];

	if ( arg[0] != '\0' )
	{
	    if ( !is_number( arg ) )  break;
	    exotic = atoi( arg );
	}
	else
	    exotic = pMob->ac[AC_EXOTIC];

	pMob->ac[AC_PIERCE] = pierce;
	pMob->ac[AC_BASH]   = bash;
	pMob->ac[AC_SLASH]  = slash;
	pMob->ac[AC_EXOTIC] = exotic;
	
	send_to_char( "Ac set.\n\r", ch );
	return TRUE;
    } while ( FALSE );    /* Just do it once.. */

    send_to_char( "Syntax:  ac [ac-pierce [ac-bash [ac-slash [ac-exotic]]]]\n\r"
		  "help MOB_AC  gives a list of reasonable ac-values.\n\r", ch );
    return FALSE;
}

MEDIT( medit_form )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( form_flags, argument ) ) != NO_FLAG )
	{
	    pMob->form ^= value;
	    send_to_char( "Form toggled.\n\r", ch );
	    return TRUE;
	}
    }

    send_to_char( "Syntax: form [flags]\n\r"
		  "Type '? form' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_part )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( part_flags, argument ) ) != NO_FLAG )
	{
	    pMob->parts ^= value;
	    send_to_char( "Parts toggled.\n\r", ch );
	    return TRUE;
	}
    }

    send_to_char( "Syntax: part [flags]\n\r"
		  "Type '? part' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_imm )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( imm_flags, argument ) ) != NO_FLAG )
	{
	    pMob->imm_flags ^= value;
	    send_to_char( "Immunity toggled.\n\r", ch );
	    return TRUE;
	}
    }

    send_to_char( "Syntax: imm [flags]\n\r"
		  "Type '? imm' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_res )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( res_flags, argument ) ) != NO_FLAG )
	{
	    pMob->res_flags ^= value;
	    send_to_char( "Resistance toggled.\n\r", ch );
	    return TRUE;
	}
    }

    send_to_char( "Syntax: res [flags]\n\r"
		  "Type '? res' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_vuln )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( vuln_flags, argument ) ) != NO_FLAG )
	{
	    pMob->vuln_flags ^= value;
	    send_to_char( "Vulnerability toggled.\n\r", ch );
	    return TRUE;
	}
    }

    send_to_char( "Syntax: vuln [flags]\n\r"
		  "Type '? vuln' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_material )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  material [string]\n\r", ch );
	return FALSE;
    }

    free_string( pMob->material );
    pMob->material = str_dup( argument );

    send_to_char( "Material set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_off )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( off_flags, argument ) ) != NO_FLAG )
	{
	    pMob->off_flags ^= value;
	    send_to_char( "Offensive behaviour toggled.\n\r", ch );
	    return TRUE;
	}
    }

    send_to_char( "Syntax: off [flags]\n\r"
		  "Type '? off' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_teach )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( teach_flags, argument ) ) != NO_FLAG )
	{
	    pMob->teach_flags ^= value;
	    send_to_char( "Teaching ability toggled.\n\r", ch );
	    return TRUE;
	}
    }

    send_to_char( "Syntax: off [flags]\n\r"
		  "Type '? off' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_size )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );

	if ( ( value = flag_value( size_flags, argument ) ) != NO_FLAG )
	{
	    pMob->size = value;
	    send_to_char( "Size set.\n\r", ch );
	    return TRUE;
	}
    }

    send_to_char( "Syntax: size [size]\n\r"
		  "Type '? size' for a list of sizes.\n\r", ch );
    return FALSE;
}

MEDIT( medit_hitdice )
{
    static char syntax[] = "Syntax:  hitdice <number> d <type> + <bonus>\n\r";
    char *num, *type, *bonus, *cp;
    MOB_INDEX_DATA *pMob;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }

    num = cp = argument;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

    type = cp;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

    bonus = cp;

    while ( isdigit( *cp ) ) ++cp;
    if ( *cp != '\0' ) *cp = '\0';

    if ( ( !is_number( num   ) || atoi( num   ) < 1 )
    ||   ( !is_number( type  ) || atoi( type  ) < 1 ) 
    ||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }

    pMob->hit[DICE_NUMBER] = atoi( num   );
    pMob->hit[DICE_TYPE]   = atoi( type  );
    pMob->hit[DICE_BONUS]  = atoi( bonus );

    send_to_char( "Hitdice set.\n\r", ch );
    return TRUE;
}

MEDIT( medit_manadice )
{
    static char syntax[] = "Syntax:  manadice <number> d <type> + <bonus>\n\r";
    char *num, *type, *bonus, *cp;
    MOB_INDEX_DATA *pMob;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }

    num = cp = argument;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

    type = cp;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

    bonus = cp;

    while ( isdigit( *cp ) ) ++cp;
    if ( *cp != '\0' ) *cp = '\0';

    if ( !( is_number( num ) && is_number( type ) && is_number( bonus ) ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }

    if ( ( !is_number( num   ) || atoi( num   ) < 1 )
    ||   ( !is_number( type  ) || atoi( type  ) < 1 ) 
    ||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }

    pMob->mana[DICE_NUMBER] = atoi( num   );
    pMob->mana[DICE_TYPE]   = atoi( type  );
    pMob->mana[DICE_BONUS]  = atoi( bonus );

    send_to_char( "Manadice set.\n\r", ch );
    return TRUE;
}

MEDIT( medit_damdice )
{
    static char syntax[] = "Syntax:  damdice <number> d <type> + <bonus>\n\r";
    char *num, *type, *bonus, *cp;
    MOB_INDEX_DATA *pMob;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }

    num = cp = argument;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

    type = cp;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

    bonus = cp;

    while ( isdigit( *cp ) ) ++cp;
    if ( *cp != '\0' ) *cp = '\0';

    if ( !( is_number( num ) && is_number( type ) && is_number( bonus ) ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }

    if ( ( !is_number( num   ) || atoi( num   ) < 1 )
    ||   ( !is_number( type  ) || atoi( type  ) < 1 ) 
    ||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }

    pMob->damage[DICE_NUMBER] = atoi( num   );
    pMob->damage[DICE_TYPE]   = atoi( type  );
    pMob->damage[DICE_BONUS]  = atoi( bonus );

    send_to_char( "Damdice set.\n\r", ch );
    return TRUE;
}


MEDIT( medit_race )
{
    MOB_INDEX_DATA *pMob;
    int race;

    if ( argument[0] != '\0'
    && ( race = race_lookup( argument ) ) != 0 )
    {
	EDIT_MOB( ch, pMob );

	pMob->race = race;
	pMob->act	      |= race_table[race].act;
	pMob->affected_by |= race_table[race].aff;
	pMob->off_flags   |= race_table[race].off;
	pMob->imm_flags   |= race_table[race].imm;
	pMob->res_flags   |= race_table[race].res;
	pMob->vuln_flags  |= race_table[race].vuln;
	pMob->form        |= race_table[race].form;
	pMob->parts       |= race_table[race].parts;

	send_to_char( "Race set.\n\r", ch );
	return TRUE;
    }

    if ( argument[0] == '?' )
    {
	char buf[MAX_STRING_LENGTH];

	send_to_char( "Available races are:", ch );

	for ( race = 0; race_table[race].name != NULL; race++ )
	{
	    if ( ( race % 3 ) == 0 )
		send_to_char( "\n\r", ch );
	    sprintf( buf, " %-15s", race_table[race].name );
	    send_to_char( buf, ch );
	}

	send_to_char( "\n\r", ch );
	return FALSE;
    }

    send_to_char( "Syntax:  race [race]\n\r"
		  "Type 'race ?' for a list of races.\n\r", ch );
    return FALSE;
}


MEDIT( medit_position )
{
    MOB_INDEX_DATA *pMob;
    char arg[MAX_INPUT_LENGTH];
    int value;

    argument = one_argument( argument, arg );

    switch ( arg[0] )
    {
    default:
	break;

    case 'S':
    case 's':
	if ( str_prefix( arg, "start" ) )
	    break;

	if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
	    break;

	EDIT_MOB( ch, pMob );

	pMob->start_pos = value;
	send_to_char( "Start position set.\n\r", ch );
	return TRUE;

    case 'D':
    case 'd':
	if ( str_prefix( arg, "default" ) )
	    break;

	if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
	    break;

	EDIT_MOB( ch, pMob );

	pMob->default_pos = value;
	send_to_char( "Default position set.\n\r", ch );
	return TRUE;
    }

    send_to_char( "Syntax:  position [start/default] [position]\n\r"
		  "Type '? position' for a list of positions.\n\r", ch );
    return FALSE;
}


MEDIT( medit_gold )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  wealth [number]\n\r", ch );
	return FALSE;
    }

    pMob->wealth = atoi( argument );

    send_to_char( "Wealth set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_hitroll )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  hitroll [number]\n\r", ch );
	return FALSE;
    }

    pMob->hitroll = atoi( argument );

    send_to_char( "Hitroll set.\n\r", ch);
    return TRUE;
}


MEDIT( medit_clan )  /* Feydrex - clan guards */
{
    MOB_INDEX_DATA *pMob;
    int clan;

    if (!str_prefix(argument,"none"))
    {
        EDIT_MOB( ch, pMob );

	send_to_char("They are now clanless.\n\r",ch);
        pMob->clan = 0;
	return TRUE;
    }

    if ( argument[0] != '\0'
    && ( clan = clan_lookup( argument ) ) != 0 )
    {
        EDIT_MOB( ch, pMob );

	pMob->clan = clan_lookup(argument);

	send_to_char( "Clan set.\n\r", ch );
	return TRUE;
    }

    if ( argument[0] == '?' )
    {
	char buf[MAX_STRING_LENGTH];

        send_to_char( "Available clans are:", ch );

	for ( clan = 0; clan < MAX_CLAN; clan++ )
	{
            if ( ( clan % 3 ) == 0 )
		send_to_char( "\n\r", ch );
	    sprintf( buf, " %-15s", clan_table[clan].name );
	    send_to_char( buf, ch );
	}

        send_to_char( "\n\r", ch );
	return FALSE;
    }

    send_to_char( "Syntax:  clan [clan]\n\r"
		  "Type 'clan ?' for a list of clans.\n\r", ch );
    return FALSE;
}



void show_liqlist(CHAR_DATA *ch)
{
    int liq;
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];
    
    buffer = new_buf();
    
    for ( liq = 0; liq_table[liq].liq_name != NULL; liq++)
    {
	if ( (liq % 21) == 0 )
	    add_buf(buffer,"Name                 Color          Proof Full Thirst Food Ssize\n\r");

	sprintf(buf, "%-20s %-14s %5d %4d %6d %4d %5d\n\r",
		liq_table[liq].liq_name,liq_table[liq].liq_color,
		liq_table[liq].liq_affect[0],liq_table[liq].liq_affect[1],
		liq_table[liq].liq_affect[2],liq_table[liq].liq_affect[3],
		liq_table[liq].liq_affect[4] );
	add_buf(buffer,buf);
    }

    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);

return;
}

void show_damlist(CHAR_DATA *ch)
{
    int att;
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];
    
    buffer = new_buf();
    
    for ( att = 0; attack_table[att].name != NULL; att++)
    {
	if ( (att % 21) == 0 )
	    add_buf(buffer,"Name                 Noun\n\r");

	sprintf(buf, "%-20s %-20s\n\r",
		attack_table[att].name,attack_table[att].noun );
	add_buf(buffer,buf);
    }

    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);

return;
}

MEDIT( medit_group )
{
    MOB_INDEX_DATA *pMob;
    MOB_INDEX_DATA *pMTemp;
    char arg[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int temp;
    BUFFER *buffer;
    bool found = FALSE;
    
    EDIT_MOB(ch, pMob);
    
    if ( argument[0] == '\0' )
    {
    	send_to_char( "Syntax: group [number]\n\r", ch);
    	send_to_char( "        group show [number]\n\r", ch);
    	return FALSE;
    }
    
    if (is_number(argument))
    {
	pMob->group = atoi(argument);
    	send_to_char( "Group set.\n\r", ch );
	return TRUE;
    }
    
    argument = one_argument( argument, arg );
    
    if ( !strcmp( arg, "show" ) && is_number( argument ) )
    {
	if (atoi(argument) == 0)
	{
		send_to_char( "Are you crazy?\n\r", ch);
		return FALSE;
	}

	buffer = new_buf ();

    	for (temp = 0; temp < 65536; temp++)
    	{
    		pMTemp = get_mob_index(temp);
    		if ( pMTemp && ( pMTemp->group == atoi(argument) ) )
    		{
			found = TRUE;
    			sprintf( buf, "[%5d] {W%s\n\r{x", pMTemp->vnum, pMTemp->player_name );
			add_buf( buffer, buf );
    		}
    	}

	if (found)
		page_to_char( buf_string(buffer), ch );
	else
		send_to_char( "No mobs in that group.\n\r", ch );

	free_buf( buffer );
        return FALSE;
    }
    
    return FALSE;
}

REDIT( redit_owner )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  owner [owner]\n\r", ch );
	send_to_char( "         owner none\n\r", ch );
	return FALSE;
    }

    free_string( pRoom->owner );
    if (!str_cmp(argument, "none"))
    	pRoom->owner = str_dup("");
    else
	pRoom->owner = str_dup( argument );

    send_to_char( "Owner set.\n\r", ch );
    return TRUE;
}

MEDIT ( medit_addmprog )
{
  int value;
  MOB_INDEX_DATA *pMob;
  PROG_LIST *list;
  PROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_MOB(ch, pMob);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        send_to_char("Syntax:   addmprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value (mprog_flags, trigger) ) == NO_FLAG)
  {
        send_to_char("Valid flags are:\n\r",ch);
        show_help( ch, "mprog");
        return FALSE;
  }

  if ( ( code =get_prog_index (atoi(num), PRG_MPROG ) ) == NULL)
  {
        send_to_char("No such MOBProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_mprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pMob->mprog_flags,value);
  list->next            = pMob->mprogs;
  pMob->mprogs          = list;

  send_to_char( "Mprog Added.\n\r",ch);
  return TRUE;
}

MEDIT ( medit_delmprog )
{
    MOB_INDEX_DATA *pMob;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char mprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_MOB(ch, pMob);

    one_argument( argument, mprog );
    if (!is_number( mprog ) || mprog[0] == '\0' )
    {
       send_to_char("Syntax:  delmprog [#mprog]\n\r",ch);
       return FALSE;
    }

    value = atoi ( mprog );

    if ( value < 0 )
    {
        send_to_char("Only non-negative mprog-numbers allowed.\n\r",ch);
        return FALSE;
    }

    if ( !(list= pMob->mprogs) )
    {
        send_to_char("MEdit:  Non existant mprog.\n\r",ch);
        return FALSE;
    }

    if ( value == 0 )
    {
	REMOVE_BIT(pMob->mprog_flags, pMob->mprogs->trig_type);
        list = pMob->mprogs;
        pMob->mprogs = list->next;
        free_mprog( list );
    }
    else
    {
        while ( (list_next = list->next) && (++cnt < value ) )
                list = list_next;

        if ( list_next )
        {
		REMOVE_BIT(pMob->mprog_flags, list_next->trig_type);
                list->next = list_next->next;
                free_mprog(list_next);
        }
        else
        {
                send_to_char("No such mprog.\n\r",ch);
                return FALSE;
        }
    }

    send_to_char("Mprog removed.\n\r", ch);
    return TRUE;
}

REDIT( redit_room )
{
	ROOM_INDEX_DATA *room;
	int value;

	EDIT_ROOM(ch, room);

	if ( (value = flag_value( room_flags, argument )) == NO_FLAG )
	{
		send_to_char( "Sintaxis: room [flags]\n\r", ch );
		return FALSE;
	}

        TOGGLE_BIT(room->room_flags, value);
	send_to_char( "Room flags toggled.\n\r", ch );
        return TRUE;
}

REDIT( redit_sector )
{
	ROOM_INDEX_DATA *room;
	int value;

	EDIT_ROOM(ch, room);

	if ( (value = flag_value( sector_flags, argument )) == NO_FLAG )
	{
		send_to_char( "Syntax: sector [type]\n\r", ch );
		return FALSE;
	}

	room->sector_type = value;
	send_to_char( "Sector type set.\n\r", ch );

	return TRUE;
}

 /* Help Editor - kermit 1/98 */
 HEDIT (hedit_make)
 {
     HELP_DATA *pHelp;
     
     if (argument[0] == '\0')
     {
         send_to_char("Syntax: mpedit make [keyword(s)]\n\r",ch);
         return FALSE;
     }
 
     pHelp                        = new_help();
     pHelp->keyword = str_dup(argument);
     pHelp->next                  = help_first;
     help_first                    = pHelp;
     ch->desc->pEdit               = (void *)pHelp;
 
     send_to_char("New Help Entry Created.\n\r",ch);
     return TRUE;
 }

HEDIT( hedit_show)
{
    HELP_DATA *pHelp;
    char buf[MAX_STRING_LENGTH];

    EDIT_HELP(ch,pHelp);

    if(pHelp->delete)  {
        send_to_char("\n\nTHIS HELP IS MARKED FOR DELETION!\n\r",ch);
        return FALSE;
    }
    
    sprintf(buf, "Level:       [%d]\n\r"
                "Keywords: %s\n\r"
                "\n\r%s\n\r",
            pHelp->level, pHelp->keyword, pHelp->text);
    send_to_char(buf,ch);
    
    return FALSE;
}

HEDIT( hedit_desc)
{
    HELP_DATA *pHelp;
    EDIT_HELP(ch, pHelp);

    if (argument[0] =='\0')
    {
       string_append(ch, &pHelp->text);
       return TRUE;
    }

    send_to_char(" Syntax: desc\n\r",ch);
    return FALSE;
}

HEDIT( hedit_keywords)
{
    HELP_DATA *pHelp;
    EDIT_HELP(ch, pHelp);

    if(argument[0] == '\0')
    {
        send_to_char(" Syntax: keywords [keywords]\n\r",ch);
        return FALSE;
    }

    pHelp->keyword = str_dup(argument);
    send_to_char( "Keyword(s) Set.\n\r", ch);
    return TRUE;
}

HEDIT(hedit_level)
{
    HELP_DATA *pHelp;

    EDIT_HELP(ch, pHelp);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }

    pHelp->level = atoi( argument );

    send_to_char( "Level set.\n\r", ch);
    return TRUE;
}

HEDIT( hedit_delete)
{
    HELP_DATA *pHelp;

    EDIT_HELP(ch,pHelp);

    if(!pHelp->delete) {
        pHelp->delete = TRUE;
        send_to_char("YOU HAVE MARKED THIS HELP DELETION!\n\r",ch);
        return TRUE;
    }

    pHelp->delete = FALSE;
    send_to_char("YOU HAVE UNMARKED THIS HELP FOR DELETION!\n\r",ch);
    return TRUE;
}

/** Guild Editor Functions.
 */

/** Function: gedit_flags
  * Descr   : Sets the various flags associated with guilds.
  * Returns : True/False if changed.
  * Syntax  : flags flag_id...
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_flags )
{
  CLAN_DATA *pClan;
  long value = 0;
  
  if ( argument[0] != '\0' )
  {
    EDIT_GUILD( ch, pClan );

    if ( ( value = flag_value( guild_flags, argument ) ) != NO_FLAG )
    {
      pClan->flags ^= value;
      send_to_char( "Guild flag(s) toggled.\n\r", ch);
      return TRUE;
    }
  }

  send_to_char( "Syntax: flag [flag ID]\n\r"
                "Type '? guild' for a list of valid flags.\n\r", ch );
  return FALSE;
}

/** Function: gedit_rank
  * Descr   : Sets the guild rank name
  * Returns : True/False if changed.
  * Syntax  : rank rank_id rank_name
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_rank )
{
  CLAN_DATA *pClan;
  char arg1[4];
 
  
  EDIT_GUILD(ch, pClan);
  
  argument = one_argument(argument, arg1);

  if (is_number(arg1) && atoi(arg1) <= MAX_RANK)
  {
    int value;
    
    value = atoi(arg1) -1;
      
    if (argument[0] != '\0')
    {
      free_string(pClan->rank[value].rankname);
      pClan->rank[value].rankname = str_dup( argument );
      send_to_char("Rank name changed.\n\r", ch);
      return TRUE;
    }
  
  }
  
  send_to_char("Syntax: rank rank# newname\n\r", ch);  
  return FALSE;
}

/** Function: gedit_skill
  * Descr   : Sets the skills associated with the various ranks.
  * Returns : True/False if changed & notice if skill dosent exist.
  * Syntax  : skill rank_# skill_name
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_skill )
{
  CLAN_DATA *pClan;
  char arg1[4];
 
  
  EDIT_GUILD(ch, pClan);
  
  argument = one_argument(argument, arg1);

  if (is_number(arg1) && atoi(arg1) <= MAX_RANK)
  {
    int value;
    
    value = atoi(arg1) -1;
      
    if (argument[0] != '\0')
    {
      free_string(pClan->rank[value].skillname);
      pClan->rank[value].skillname = str_dup( argument );
      if (skill_lookup(argument) == -1)
        send_to_char("Notice: That skill does not exist.\n\r", ch);
      send_to_char("Skill changed.\n\r", ch);
      return TRUE;
    }
  
  }
  
  send_to_char("Syntax: skill rank# newskill\n\r", ch);  
  return FALSE;
}

 
/** Function: gedit_create
  * Descr   : Creates a new, empty guild if room is available in the array.
  * Returns : True/False if created
  * Syntax  : (n/a)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_create )
{
  int i = 0;
  
  for (i=1; i < MAX_CLAN; i++) /* just loop through and find an open slot */
  {
    if (clan_lookup(clan_table[i].name) == 0)
    break;
  }

  if (i <= MAX_CLAN) /* open slot */
  {
    CLAN_DATA *pClan;
    int x;
  
    clan_table[i].name = str_dup("New Guild");
    clan_table[i].who_name = str_dup("New Guild");

    for (x = 0; x < MAX_RANK; x++)
    {
      clan_table[i].rank[x].rankname = str_dup("Empty");
      clan_table[i].rank[x].skillname = str_dup("");
    }

    pClan = &clan_table[i]; /* return new clan data */
    ch->desc->pEdit = pClan;
    send_to_char("Guild created.\n\r", ch);
    return TRUE;
  }  

  send_to_char("No room to create a new guild. Increase MAX_CLAN\n\r", ch);  
  return FALSE;
} 


/** Function: gedit_list
  * Descr   : List's all of the current guilds, including those not yet
  *         : saved to the data file, and those marked for deletion.
  * Returns : list of guilds.
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_list )
{
  char buf[MIL];
  BUFFER *buffer;
  int i;
  
  buffer = new_buf();
  
  sprintf(buf, "Num  Guild Name  Flags\n\r-----------------------------------------------\n\r");
  add_buf(buffer, buf);
  
  for (i=1; i <= MAX_CLAN; i++)
  {
    if (clan_table[i].name != NULL && clan_table[i].name[0] != '\0')
    {
      sprintf(buf,"[%2d]  %s\t Flags:\t[%s]\n\r", 
                  i, 
                  clan_table[i].name,
                  flag_string( guild_flags, clan_table[i].flags ) );
      add_buf(buffer, buf);
    }
  }

  page_to_char( buf_string(buffer), ch );
  free_buf(buffer);
  return FALSE;
}      
  
 
/** Function: gedit_show
  * Descr   : Displays currently selected guild data to the screen.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_show )
{
	CLAN_DATA *pClan;
  
	char buf[MIL];
	BUFFER *buffer;
	int i;
	
	EDIT_GUILD(ch, pClan);
	
	buffer = new_buf();
	
	sprintf(buf, "{YName     : %s %s %s{x\n\r{YWho Name : %-10s{x\n\r",
		pClan->name, 
		IS_SET(pClan->flags, GUILD_CHANGED) ? "{D[{B*{D]{x" : "",
		IS_SET(pClan->flags, GUILD_DELETED) ? "{RMarked for Deletion!{x" : "",
		pClan->who_name);
	stc(buf,ch);

	sprintf(buf, "{YRecall Room {D[{W%-6d{D]  {YMorgue Room {D[{W%-6d{D]  {YClan Temple {D[{W%-6d{D]{x\n\r",
		pClan->room[0], pClan->room[1], pClan->room[2]);
	stc(buf,ch);

	sprintf(buf, "{YThreshold: {D[{W%-6d{D]{x {YPrep Room {D[{W%-6d{D]{x\n\r",
		pClan->threshold,
		pClan->prep_room);
	stc(buf,ch);

	sprintf(buf, "{YMaze {D[{WBegin-Mid-End{D]{Y: {D[{W%d-%d-%d{D]{x\n\r",
		pClan->maze_begin,
		pClan->maze_mid,
		pClan->maze_end);
	stc(buf,ch);

	sprintf(buf, "{YFlags:      {D[{W%s{D]{x\n\r", flag_string( guild_flags, pClan->flags ) );
	stc(buf,ch);

	sprintf(buf, "{YFilename:   {D[{W%s{D]{x\n\r", pClan->file_name);
	add_buf(buffer,buf);
	
	sprintf(buf, "{YCP's: {D[{W%d{d]{x\n\r",pClan->clan_points);
	stc(buf,ch);

	sprintf(buf, "{C#    {YRank                      \n\r{G--------------------------------------------------------------{x\n\r");
	stc(buf,ch);
	for (i=0; i < MAX_RANK; i++)
	{
		sprintf(buf,"{D[{C%2d{D] {Y%-25s{x\n\r",i+1, 
			(pClan->rank[i].rankname  != '\0') ? pClan->rank[i].rankname : "None");  
		stc(buf,ch);
	}
	
	sprintf(buf, "{YMortal Leader Rights\n\r{G--------------------\n\r");
	stc(buf,ch);
	
	sprintf(buf,"{YCan Guild   : {W%s{x\n\r{YCan Deguild : {W%s{x\n\r{YCan Promote : {W%s{x\n\r{YCan Demote  : {W%s{x\n\r",
		(pClan->ml[0]==1) ? "True" : "False",
		(pClan->ml[1]==1) ? "True" : "False",
		(pClan->ml[2]==1) ? "True" : "False",
		(pClan->ml[3]==1) ? "True" : "False"); 
	stc(buf,ch);
	return FALSE;
}


/** Function: gedit_name
  * Descr   : Changes the name of the currently selected guild.
  * Returns : True/False if modified
  * Syntax  : name new_name
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_name )
{
  CLAN_DATA *pClan;
  
  EDIT_GUILD(ch, pClan);
  
  if ( argument[0] == '\0' )
  {
    send_to_char( "Syntax:  name [name]\n\r", ch );
    return FALSE;
  }
  
  if (clan_lookup(argument) != 0)
  {
    send_to_char("That guild allready exists.\n\r", ch);
    return FALSE;
  }

  if (pClan->name[0] != '\0')                          
    free_string( pClan->name );
 
  pClan->name = str_dup( argument );
                                    
  send_to_char( "Name set.\n\r", ch );
  return TRUE;
}


/** Function: gedit_whoname
  * Descr   : Changes the who_name of the currently selected guild.
  * Returns : True/False if modified.
  * Syntax  : whoname new_who_name
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_whoname )
{
  CLAN_DATA *pClan;
  
  EDIT_GUILD(ch, pClan);
  
  if ( argument[0] == '\0' )
  {
    send_to_char( "Syntax:  whoname [name]\n\r", ch );
    return FALSE;
  }
  
  if (pClan->who_name[0] != '\0')                          
    free_string( pClan->who_name );
  
  pClan->who_name = str_dup( argument );
                                    
  send_to_char( "Who Name set.\n\r", ch );
  return TRUE;
}


/** Function: gedit_rooms
  * Descr   : Changes the vnum of the room(s) selected.
  * Returns : True/False if modified.
  * Syntax  : rooms [hall new#|morgue new#|temple new#]
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_rooms )
{
  CLAN_DATA *pClan;
  char arg1[10], arg2[10], arg3[10], arg4[10], arg5[10], arg6[10];
  bool set = FALSE;
  
  EDIT_GUILD(ch, pClan);
  
  if ( argument[0] == '\0' )
  {
    send_to_char( "Syntax:  rooms [hall #|morgue #|temple #|threshold #]\n\r", ch );
    return FALSE;
  }

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);
  argument = one_argument(argument, arg4);
  argument = one_argument(argument, arg5);
  argument = one_argument(argument, arg6);

  /* kinda of convoluted, I know, but this seemed the easiest way
     to allow for any combination of commands. ie:
       hall 1 morgue 2 temple 3 
       morgue 2 hall 3 temple 1
         or even
       hall 1
       morgue 2 hall 1
  */
	/*arg1*/
	if (!str_cmp(arg1,"hall") && is_number(arg2) )
	{
		pClan->room[0] = atoi(arg2);
		set = TRUE;
	}
	else if (!str_cmp(arg1,"morgue") && is_number(arg2) )
	{
		pClan->room[1] = atoi(arg2);
		set = TRUE;
	}
	else if (!str_cmp(arg1,"temple") && is_number(arg2) )
	{ 
		pClan->room[2] = atoi(arg2);
		set = TRUE; 
	}
	else if (!str_cmp(arg1,"threshold") && is_number(arg2) )
	{
		pClan->room[3] = atoi(arg2);
		set = TRUE; 
	}

	/*arg2*/
	if (!str_cmp(arg3,"hall") && is_number(arg4) )
	{
		pClan->room[0] = atoi(arg4);
		set = TRUE;
	}
	else if (!str_cmp(arg3,"morgue") && is_number(arg4) )
	{
		pClan->room[1] = atoi(arg4);
		set = TRUE;
	}
	else if (!str_cmp(arg3,"temple") && is_number(arg4) )
	{
		pClan->room[2] = atoi(arg4);
		set = TRUE;
	}
	else if (!str_cmp(arg3,"threshold") && is_number(arg4) )
	{
		pClan->room[3] = atoi(arg4);
		set = TRUE;
	}

	/*arg3*/
	if (!str_cmp(arg5,"hall") && is_number(arg6) )
	{
		pClan->room[0] = atoi(arg6);
		set = TRUE;
	}
	else if (!str_cmp(arg5,"morgue") && is_number(arg6) )
	{
		pClan->room[1] = atoi(arg6);
		set = TRUE; 
	}
	else if (!str_cmp(arg5,"temple") && is_number(arg6) )
	{
		pClan->room[2] = atoi(arg6);
		set = TRUE;
	}
	else if (!str_cmp(arg5,"threshold") && is_number(arg6) )
	{
		pClan->room[3] = atoi(arg6);
		set = TRUE;
	}
    
	if (set)
	{
		send_to_char("Room(s) set.\n\r", ch);
		return TRUE;
	}
      
	return FALSE;
}


/** Function: gedit_ml
  * Descr   : Changes the mortal leader settings.
  * Returns : True/False if modified
  * Syntax  : ml [true/false|true/false|true/false|true/false]
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
GEDIT( gedit_ml )
{
  CLAN_DATA *pClan;
  char arg1[6], arg2[6], arg3[6], arg4[6];
  bool set = FALSE;
  
  EDIT_GUILD(ch, pClan);
  
  if ( argument[0] == '\0' )
  {
    send_to_char( "Syntax:  ml TRUE|FALSE TRUE|FALSE TRUE|FALSE TRUE|FALSE\n\r", ch );
    return FALSE;
  }

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);
  argument = one_argument(argument, arg4);

  if (!str_prefix(arg1,"true") )
    { pClan->ml[0] = TRUE; set = TRUE; }
  else
  if (!str_prefix(arg1, "false") )
    { pClan->ml[0] = FALSE; set = TRUE; }

  if (!str_prefix(arg2,"true") )
    { pClan->ml[1] = TRUE; set = TRUE; }
  else
  if (!str_prefix(arg2, "false") )
    { pClan->ml[1] = FALSE; set = TRUE; }

  if (!str_prefix(arg3,"true") )
    { pClan->ml[2] = TRUE; set = TRUE; }
  else
  if (!str_prefix(arg3, "false") )
    { pClan->ml[2] = FALSE; set = TRUE; }
   
  if (!str_prefix(arg4,"true") )
    { pClan->ml[3] = TRUE; set = TRUE; }
  else
  if (!str_prefix(arg4, "false") )
    { pClan->ml[3] = FALSE; set = TRUE; }

  if (set)
  {
    send_to_char("Mortal Leader traits set.\n\r", ch);
    return TRUE;
  }

  return FALSE;
}

OEDIT ( oedit_addoprog )
{
  int value;
  OBJ_INDEX_DATA *pObj;
  PROG_LIST *list;
  PROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_OBJ(ch, pObj);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        send_to_char("Syntax:   addoprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value (oprog_flags, trigger) ) == NO_FLAG)
  {
        send_to_char("Valid flags are:\n\r",ch);
        show_help( ch, "oprog");
        return FALSE;
  }

  if ( ( code =get_prog_index (atoi(num), PRG_OPROG ) ) == NULL)
  {
        send_to_char("No such OBJProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_oprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pObj->oprog_flags,value);
  list->next            = pObj->oprogs;
  pObj->oprogs          = list;

  send_to_char( "Oprog Added.\n\r",ch);
  return TRUE;
}

OEDIT ( oedit_deloprog )
{
    OBJ_INDEX_DATA *pObj;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char oprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument( argument, oprog );
    if (!is_number( oprog ) || oprog[0] == '\0' )
    {
       send_to_char("Syntax:  deloprog [#oprog]\n\r",ch);
       return FALSE;
    }

    value = atoi ( oprog );

    if ( value < 0 )
    {
        send_to_char("Only non-negative oprog-numbers allowed.\n\r",ch);
        return FALSE;
    }

    if ( !(list= pObj->oprogs) )
    {
        send_to_char("OEdit:  Non existant oprog.\n\r",ch);
        return FALSE;
    }

    if ( value == 0 )
    {
	REMOVE_BIT(pObj->oprog_flags, pObj->oprogs->trig_type);
        list = pObj->oprogs;
        pObj->oprogs = list->next;
        free_oprog( list );
    }
    else
    {
        while ( (list_next = list->next) && (++cnt < value ) )
                list = list_next;

        if ( list_next )
        {
		REMOVE_BIT(pObj->oprog_flags, list_next->trig_type);
                list->next = list_next->next;
                free_oprog(list_next);
        }
        else
        {
                send_to_char("No such oprog.\n\r",ch);
                return FALSE;
        }
    }

    send_to_char("Oprog removed.\n\r", ch);
    return TRUE;
}

REDIT ( redit_addrprog )
{
  int value;
  ROOM_INDEX_DATA *pRoom;
  PROG_LIST *list;
  PROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_ROOM(ch, pRoom);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        send_to_char("Syntax:   addrprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value (rprog_flags, trigger) ) == NO_FLAG)
  {
        send_to_char("Valid flags are:\n\r",ch);
        show_help( ch, "rprog");
        return FALSE;
  }

  if ( ( code =get_prog_index (atoi(num), PRG_RPROG ) ) == NULL)
  {
        send_to_char("No such ROOMProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_rprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pRoom->rprog_flags,value);
  list->next            = pRoom->rprogs;
  pRoom->rprogs          = list;

  send_to_char( "Rprog Added.\n\r",ch);
  return TRUE;
}

REDIT ( redit_delrprog )
{
    ROOM_INDEX_DATA *pRoom;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char rprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_ROOM(ch, pRoom);

    one_argument( argument, rprog );
    if (!is_number( rprog ) || rprog[0] == '\0' )
    {
       send_to_char("Syntax:  delrprog [#rprog]\n\r",ch);
       return FALSE;
    }

    value = atoi ( rprog );

    if ( value < 0 )
    {
        send_to_char("Only non-negative rprog-numbers allowed.\n\r",ch);
        return FALSE;
    }

    if ( !(list= pRoom->rprogs) )
    {
        send_to_char("REdit:  Non existant rprog.\n\r",ch);
        return FALSE;
    }

    if ( value == 0 )
    {
	REMOVE_BIT(pRoom->rprog_flags, pRoom->rprogs->trig_type);
        list = pRoom->rprogs;
        pRoom->rprogs = list->next;
        free_rprog( list );
    }
    else
    {
        while ( (list_next = list->next) && (++cnt < value ) )
                list = list_next;

        if ( list_next )
        {
		REMOVE_BIT(pRoom->rprog_flags, list_next->trig_type);
                list->next = list_next->next;
                free_rprog(list_next);
        }
        else
        {
                send_to_char("No such rprog.\n\r",ch);
                return FALSE;
        }
    }

    send_to_char("Rprog removed.\n\r", ch);
    return TRUE;
}

GEDIT( gedit_file )
{
    CLAN_DATA *pClan;
    char file[MAX_STRING_LENGTH];
    int i, length;

    EDIT_GUILD(ch, pClan);

    one_argument( argument, file );	/* Forces Lowercase */

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  filename [$file]\n\r", ch );
	return FALSE;
    }

    /*
     * Simple Syntax Check.
     */
    length = STRLEN( argument );
    if ( length > 8 )
    {
	send_to_char( "No more than eight characters allowed.\n\r", ch );
	return FALSE;
    }
    
    /*
     * Allow only letters and numbers.
     */
    for ( i = 0; i < length; i++ )
    {
	if ( !isalnum( file[i] ) )
	{
	    send_to_char( "Only letters and numbers are valid.\n\r", ch );
	    return FALSE;
	}
    }    

    free_string( pClan->file_name );
    strcat( file, ".cln" );
    pClan->file_name = str_dup( file );

    send_to_char( "Filename set.\n\r", ch );
    return TRUE;
}

/** Function: gedit_threshold
  * Descr   : Changes the threshold of the currently selected guild.
  * Returns : True/False if modified.
  * Syntax  : gedit threshold <number>
  * Written : March 2002
  * Author  : Eric Goetschalckx
  */
GEDIT( gedit_threshold )
{
  CLAN_DATA *pClan;
  int value;
  
  EDIT_GUILD(ch, pClan);
  
  if ( argument[0] == '\0' )
  {
    send_to_char( "Syntax:  threshold <number>\n\r", ch );
    return FALSE;
  }
  value = atoi(argument);

  if(!is_number(argument))
  {
	  stc("That value is not a number",ch);
	  return FALSE;
  }
  
  if (pClan->who_name[0] != '\0')                          
  {
	  pClan->threshold = value;
	  send_to_char( "Threshold set.\n\r", ch );
	  return TRUE;
  }
  return FALSE;
}

/** Function: gedit_prep
  * Descr   : Changes the prep_room of the currently selected guild.
  * Returns : True/False if modified.
  * Syntax  : prep <number>
  * Written : March 2002
  * Author  : Eric Goetschalckx
  */
GEDIT( gedit_prep )
{
  CLAN_DATA *pClan;
  int value;
  
  EDIT_GUILD(ch, pClan);
  
  if ( argument[0] == '\0' )
  {
    send_to_char( "Syntax:  prep <number>\n\r", ch );
    return FALSE;
  }
  value = atoi(argument);

  if(!is_number(argument))
  {
	  stc("That value is not a number",ch);
	  return FALSE;
  }
  
  if (pClan->who_name[0] != '\0')                          
  {
	  pClan->prep_room = value;
	  send_to_char( "Prep_room set.\n\r", ch );
	  return TRUE;
  }
  return FALSE;
}

/** Function: gedit_threshold
  * Descr   : Changes the threshold of the currently selected guild.
  * Returns : True/False if modified.
  * Syntax  : gedit threshold <number>
  * Written : March 2002
  * Author  : Eric Goetschalckx
  */
GEDIT( gedit_maze )
{
	CLAN_DATA *pClan;
	char arg1[MSL];
	char arg2[MSL];
	char arg3[MSL];
	int value1;
	int value2;
	int value3;
  
	EDIT_GUILD(ch, pClan);

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Syntax:  maze <number> <number> <number>\n\r", ch );
		return FALSE;
	}
	value1 = atoi(arg1);
	value2 = atoi(arg2);
	value3 = atoi(arg3);

	if(!is_number(arg1) || !is_number(arg2) || !is_number(arg3) )
	{
		stc("That value is not a number",ch);
		return FALSE;
	}
  
	if (pClan->who_name[0] != '\0')                          
	{
		pClan->maze_begin = value1;
		pClan->maze_mid = value2;
		pClan->maze_end = value3;
		send_to_char( "Maze rooms set set.\n\r", ch );
		return TRUE;
	}
	return FALSE;
}

OEDIT( oedit_weapon_spell )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  weapon_spell <spell>\n\r", ch );
	return FALSE;
    }

	pObj->weapon_spell = skill_lookup( argument );
	send_to_char( "Weapon Spell Set.\n\r\n\r", ch );
    return TRUE;
}
OEDIT( oedit_delete )
{
	OBJ_DATA *obj, *obj_next;
	OBJ_INDEX_DATA *pObj;
	RESET_DATA *pReset, *wReset;
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

	ptc( ch, "Removed room vnum ^C%d^x, %d resets, %d extra "
		"descriptions and %d exits.\n\r", index, rcount, edcount, ecount );
	ptc( ch, "^C%d^x objects and ^C%d^x mobiles were extracted "
		"from the room.\n\r", ocount, mcount );

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


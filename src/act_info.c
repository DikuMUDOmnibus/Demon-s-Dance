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


char *	const	where_name	[] =
{
    "{D<{wused as {Wlight{D>{x     ",	/* 0 */
    "{D<{wworn on {Wfinger{D>{x    ",
    "{D<{wworn on {Wfinger{D>{x    ",
    "{D<{wworn around {Wneck{D>{x  ",
    "{D<{wworn around {Wneck{D>{x  ",
    "{D<{wworn on {Wtorso{D>{x     ",	/* 5 */
    "{D<{wworn on {Whead{D>{x      ",
    "{D<{wworn on {Wlegs{D>{x      ",
    "{D<{wworn on {Wfeet{D>{x      ",
    "{D<{wworn on {Whands{D>{x     ",
    "{D<{wworn on {Warms{D>{x      ",	/* 10 */
    "{D<{wworn as {Wshield{D>{x    ",
    "{D<{wworn about {Wbody{D>{x   ",
    "{D<{wworn about {Wwaist{D>{x  ",
    "{D<{wworn around {Wwrist{D>{x ",
    "{D<{wworn around {Wwrist{D>{x ",	/* 15 */
    "{D<{Wwielded{D>{x           ", 
    "{D<{Wsecondary weapon{D>{x  ",  
	"{D<{Wheld{D>{x              ",
    "{D<{Wfloating nearby{D>{x   ",
	"{D<{wworn on {Wface{D>{x	    ",	/* 20 */
	"{W({Rlodged in a leg{W){x  ", 
	"{W({Rlodged in an arm{W){x ",
	"{W({Rlodged in a rib{W){x  ", 
};


/* for  keeping track of the player count */
int max_on = 0;

/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
void whowas(CHAR_DATA *ch, FILE *fp);
extern void 	AppendSlayMsg		(SLAY_MESSAGE *SMptr); /* 010501 */
extern void	save_slaym		(); /* 010501 */



char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
    ||  (obj->description == NULL || obj->description[0] == '\0'))
	return buf;

    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )	strcat( buf, "{D[{BI");else strcat( buf, "{D[{B.");
    if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && IS_OBJ_STAT(obj, ITEM_EVIL)   )	strcat( buf, "{rE"  );
    else if (IS_AFFECTED(ch, AFF_DETECT_GOOD)
		 &&  IS_OBJ_STAT(obj,ITEM_BLESS))	strcat(buf,"{yG");
	else									strcat(buf, "{w.");
	if (IS_AFFECTED(ch, AFF_DETECT_MAGIC)
    &&  IS_OBJ_STAT(obj,ITEM_MAGIC))		strcat(buf,"{MM{D]{x");else strcat(buf, "{M.{D]{x");
	if(IS_OBJ_STAT(obj,ITEM_HIDDEN)) strcat(buf,"{D[{rH{wi{Ddd{we{rn{D]{x");

    if ( fShort )
    {
	if ( obj->short_descr != NULL )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description != NULL)
	    strcat( buf, obj->description );
    }

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if ( ch->desc == NULL )
	return;

    /*
     * Alloc space for output lines.
     */
    output = new_buf();

    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    { 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj )) 
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );

	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if (prgpstrShow[iShow][0] == '\0')
	{
	    free_string(prgpstrShow[iShow]);
	    continue;
	}

	if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		add_buf(output,buf);
	    }
	    else
	    {
		add_buf(output,"     ");
	    }
	}
	add_buf(output,prgpstrShow[iShow]);
	add_buf(output,"\n\r");
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
    }
    page_to_char(buf_string(output),ch);

    /*
     * Clean up.
     */
    free_buf(output);
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH],message[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ( IS_SET(victim->act,PLR_PROKILLER )   ) strcat( buf, "{R*{x"	     );
	if ( IS_SET(victim->comm,COMM_AFK	  )   ) strcat( buf, "{D[{RAFK{D]{x"	     );
    if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "{D[{BI{bn{Dv{bi{Bs{D]{x"      );
    if ( victim->invis_level >= LEVEL_HERO    ) strcat( buf, "{D[{mW{wiz{mi{D]{x"	     );
    if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "{D[{rH{Did{re{D]{x"       );
    if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "{D[{CCh{Wa{Yr{Wm{Ced{D]{x"    );
    if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "{D[{WTr{wan{Dsl{wuc{Wen{wt{D]{x");
    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "{D[{MPi{mn{rk A{mu{Mra{D]{x"  );
    if ( IS_EVIL(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "{D[{RRe{rd {DA{ru{Rra{D]{x"   );
    if ( IS_GOOD(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_GOOD)     ) strcat( buf, "{D[{YG{yo{Wl{wd{We{yn {YA{yu{Wr{wa{D]{x");
    if ( IS_AFFECTED(victim, AFF_SANCTUARY)   ) strcat( buf, "{D[{WWhite Aura{D]{x" );
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF  ) )
						strcat( buf, "{D[{rT{DH{rI{DE{rF{D]{x"      );
	if ( IS_AFFECTED2(victim, AFF_FIRESHIELD)  ) strcat( buf, "{D[{RF{rir{Re{D]{x" );
	if ( IS_AFFECTED2(victim, AFF_SHADOW_SLIP) ) strcat( buf, "{D[{wShadowSlip{D]{x" );
	if ( IS_AFFECTED2(victim, AFF_HOLY_ARMOR)  ) strcat( buf, "{D[{DA{Wr{Ym{Wo{Dr{D]{x" );
    if ( victim->position == victim->start_pos  
		&& victim->long_descr[0] != '\0' )
    {
        strcat( buf, victim->long_descr );
        send_to_char( buf, ch );
        return;
    }
    if (IS_NPC(victim) &&ch->questmob > 0 && victim->pIndexData->vnum == ch->questmob)
        strcat( buf, "{D[{cTARGET{D]{x ");
	{
	strcat( buf, victim->long_descr );
	send_to_char( buf, ch );
	return;
    }

    strcat( buf, PERS( victim, ch ) );
    if ( !IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF) 
    &&   victim->position == POS_STANDING && ch->on == NULL )
	strcat( buf, victim->pcdata->title );

    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
    case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
    case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
    case POS_SLEEPING:
	{
		if (victim->on != NULL)
		{
			if (IS_SET(victim->on->value[2],SLEEP_AT))
			{
				sprintf(message," is sleeping at %s.",
					victim->on->short_descr);
				strcat(buf,message);
			}
			else if (IS_SET(victim->on->value[2],SLEEP_ON))
			{
				sprintf(message," is sleeping on %s.",
					victim->on->short_descr); 
				strcat(buf,message);
			}
			else
			{
				sprintf(message, " is sleeping in %s.",
					victim->on->short_descr);
				strcat(buf,message);
			}
		}
		else 
			strcat(buf," is sleeping here.");
		break;
	}
	case POS_RESTING:  
	{
		if (victim->on != NULL)
		{
			if (IS_SET(victim->on->value[2],REST_AT))
			{
				sprintf(message," is resting at %s.",
					victim->on->short_descr);
				strcat(buf,message);
			}
			else if (IS_SET(victim->on->value[2],REST_ON))
			{
				sprintf(message," is resting on %s.",
					victim->on->short_descr);
				strcat(buf,message);
			}
			else 
			{
				sprintf(message, " is resting in %s.",
					victim->on->short_descr);
				strcat(buf,message);
			}
		}
		else
			strcat( buf, " is resting here." );       
		break;
	}
	case POS_SITTING:  
	{
		if (victim->on != NULL)
		{
			if (IS_SET(victim->on->value[2],SIT_AT))
			{
				sprintf(message," is sitting at %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],SIT_ON))
            {
                sprintf(message," is sitting on %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, " is sitting in %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
        }
        else
			strcat(buf, " is sitting here.");
		break;
	}
    case POS_STANDING: 
	{
		if (victim->on != NULL)
		{
			if (IS_SET(victim->on->value[2],STAND_AT))
			{
				sprintf(message," is standing at %s.",
					victim->on->short_descr);
				strcat(buf,message);
			}
			else if (IS_SET(victim->on->value[2],STAND_ON))
			{
				sprintf(message," is standing on %s.",
					victim->on->short_descr);
				strcat(buf,message);
			}
			else
			{
				sprintf(message," is standing in %s.",
					victim->on->short_descr);
				strcat(buf,message);
			}
		}
		else
			strcat( buf, " is here." );               
		break;
	}
    case POS_FIGHTING:
	{
		strcat( buf, " is here, fighting " );
		if ( victim->fighting == NULL )
			strcat( buf, "thin air??" );
		else if ( victim->fighting == ch )
			strcat( buf, "YOU!" );
		else if ( victim->in_room == victim->fighting->in_room )
		{
			strcat( buf, PERS( victim->fighting, ch ) );
			strcat( buf, "." );
		}
		else
			strcat( buf, "someone who left??" );
		break;
	}
	}
    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
	char name_buf[MSL];
    OBJ_DATA *obj;
    int iWear;
    long percent;
    bool found;

    if ( can_see( victim, ch ) )
    {
	if (ch == victim)
	    act( "$n looks at $mself.",ch,NULL,NULL,TO_ROOM);
	else
	{
	    act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	    act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
	}
    }

    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

	sprintf( name_buf, "%s\n\r", PERS(victim, ch));
	strcpy( buf, name_buf );

    if (percent >= 100) 	
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.6{Y.7{c.8{C.9{g.{G10{D]{x \n\r");
    else if (percent >= 95) 
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.6{Y.7{c.8{C.9{g.  {D]{x \n\r");	
	else if (percent >= 90) 
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.6{Y.7{c.8{C.9   {D]{x \n\r");
	else if (percent >= 85)
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.6{Y.7{c.8{C.    {D]{x \n\r");
	else if (percent >= 80)		
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.6{Y.7{c.8     {D]{x \n\r");	
	else if (percent >= 75) 
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.6{Y.7{c.      {D]{x \n\r");
    else if (percent >= 70)
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.6{Y.7       {D]{x \n\r");
	else if (percent >= 65)
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.6{Y.        {D]{x \n\r");
	else if (percent >= 60)
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.6         {D]{x \n\r");
	else if (percent >= 55)
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5{y.          {D]{x \n\r");
	else if (percent >= 50) 
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.5           {D]{x \n\r");
    else if (percent >= 45)
	strcat( buf, "{D[{r.1{R.2{M.3{m.4{W.            {D]{x \n\r");
	else if (percent >= 40)
	strcat( buf, "{D[{r.1{R.2{M.3{m.4             {D]{x \n\r");
	else if (percent >= 35)
	strcat( buf, "{D[{r.1{R.2{M.3{m.              {D]{x \n\r");
	else if (percent >= 30)
	strcat( buf, "{D[{r.1{R.2{M.3               {D]{x \n\r");
    else if (percent >= 25)
	strcat( buf, "{D[{r.1{R.2{M.                {D]{x \n\r");
	else if (percent >= 20)
	strcat( buf, "{D[{r.1{R.2                 {D]{x \n\r");
	else if (percent >= 15)
	strcat( buf, "{D[{r.1{R.                  {D]{x \n\r");
    else if (percent >= 10)
	strcat( buf, "{D[{r.1                   {D]{x \n\r");
	else if (percent >= 5)
	strcat( buf, "{D[{r.                    {D]{x \n\r");
	else if (percent >= 0 )
	strcat( buf, "{D[                     {D]{x \n\r");
    else
	strcat( buf, "{D[                     {D]{x \n\r");

    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char( where_name[iWear], ch );
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    if ( victim != ch
    &&   !IS_NPC(ch)
    &&   number_percent( ) < get_skill(ch,gsn_peek))
    {
	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
	check_improve(ch,gsn_peek,TRUE,4);
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( get_trust(ch) < rch->invis_level)
	    continue;

	if ( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
	}
	else if ( room_is_dark( ch->in_room )
	&&        IS_AFFECTED(rch, AFF_INFRARED ) )
	{
	    send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
	}
    }

    return;
} 



bool check_blind( CHAR_DATA *ch )
{

    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT))
	return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
    { 
	send_to_char( "You can't see a thing!\n\r", ch ); 
	return FALSE; 
    }

    return TRUE;
}

/* changes your scroll */
void do_scroll(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument,arg);
    
    if (arg[0] == '\0')
    {
	if (ch->lines == 0)
	    send_to_char("You do not page long messages.\n\r",ch);
	else
	{
	    sprintf(buf,"You currently display %d lines per page.\n\r",
		    ch->lines + 2);
	    send_to_char(buf,ch);
	}
	return;
    }

    if (!is_number(arg))
    {
	send_to_char("You must provide a number.\n\r",ch);
	return;
    }

    lines = atoi(arg);

    if (lines == 0)
    {
        send_to_char("Paging disabled.\n\r",ch);
        ch->lines = 0;
        return;
    }

    if (lines < 10 || lines > 100)
    {
	send_to_char("You must provide a reasonable number.\n\r",ch);
	return;
    }

    sprintf(buf,"Scroll set to %d lines.\n\r",lines);
    send_to_char(buf,ch);
    ch->lines = lines - 2;
}

/* RT does socials */
void do_socials(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;
     
    col = 0;
   
    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
	sprintf(buf,"%-12s",social_table[iSocial].name);
	send_to_char(buf,ch);
	if (++col % 6 == 0)
	    send_to_char("\n\r",ch);
    }

    if ( col % 6 != 0)
	send_to_char("\n\r",ch);
    return;
}


 
/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "motd");
}

void do_imotd(CHAR_DATA *ch, char *argument)
{  
    do_function(ch, &do_help, "imotd");
}

void do_rules(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "rules");
}

void do_story(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "story");
}

void do_wizlist(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "wizlist");
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void do_autolist(CHAR_DATA *ch, char *argument)
{
    /* lists most player flags */
    if (IS_NPC(ch))
      return;

    send_to_char("   action     status\n\r",ch);
    send_to_char("---------------------\n\r",ch);
 
    send_to_char("autogolem     ",ch);
    if (IS_SET(ch->act,PLR_AUTOGOLEM))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch); 

    send_to_char("autotitle     ",ch);
    if (!IS_SET(ch->plr2,PLR2_NO_TITLE))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch); 

    send_to_char("autoassist     ",ch);
    if (IS_SET(ch->act,PLR_AUTOASSIST))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch); 

    send_to_char("autoexit       ",ch);
    if (IS_SET(ch->act,PLR_AUTOEXIT))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("autogold       ",ch);
    if (IS_SET(ch->act,PLR_AUTOGOLD))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("autoloot       ",ch);
    if (IS_SET(ch->act,PLR_AUTOLOOT))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("autosac        ",ch);
    if (IS_SET(ch->act,PLR_AUTOSAC))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("autosplit      ",ch);
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("compact mode   ",ch);
    if (IS_SET(ch->comm,COMM_COMPACT))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("prompt         ",ch);
    if (IS_SET(ch->comm,COMM_PROMPT))
	send_to_char("ON\n\r",ch);
    else
	send_to_char("OFF\n\r",ch);

    send_to_char("combine items  ",ch);
    if (IS_SET(ch->comm,COMM_COMBINE))
	send_to_char("ON\n\r",ch);
    else
	send_to_char("OFF\n\r",ch);

    if (!IS_SET(ch->act,PLR_CANLOOT))
	send_to_char("Your corpse is safe from thieves.\n\r",ch);
    else 
        send_to_char("Your corpse may be looted.\n\r",ch);

    if (IS_SET(ch->act,PLR_NOSUMMON))
	send_to_char("You cannot be summoned.\n\r",ch);
    else
	send_to_char("You can be summoned.\n\r",ch);
   
    if (IS_SET(ch->act,PLR_NOFOLLOW))
	send_to_char("You do not welcome followers.\n\r",ch);
    else
	send_to_char("You accept followers.\n\r",ch);
}

void do_autoassist(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
    
    if (IS_SET(ch->act,PLR_AUTOASSIST))
    {
      send_to_char("Autoassist {Rremoved{x.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOASSIST);
    }
    else
    {
      send_to_char("Autossist {Gset{x.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOASSIST);
    }
}

void do_autogolem(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
    
    if (IS_SET(ch->act,PLR_AUTOGOLEM))
    {
      send_to_char("Autogolem {Rremoved{x.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOGOLEM);
    }
    else
    {
      send_to_char("Autogolem {Gset{x.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOGOLEM);
    }
}

void do_autoexit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOEXIT))
    {
      send_to_char("Autoexit {Rremoved{x.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOEXIT);
    }
    else
    {
      send_to_char("Autoexit {Gset{x.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOEXIT);
    }
}

void do_autogold(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOGOLD))
    {
      send_to_char("Autogold {Rremoved{x.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOGOLD);
    }
    else
    {
      send_to_char("Autogold {Gset{x.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOGOLD);
    }
}

void do_autoloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOLOOT))
    {
      send_to_char("Autolooting {Rremoved{x.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOLOOT);
    }
    else
    {
      send_to_char("Autolooting {Gset{x.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOLOOT);
    }
}

void do_autosac(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOSAC))
    {
      send_to_char("Autosacrificing {Rremoved{x.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOSAC);
    }
    else
    {
      send_to_char("Autosacrificing {Gset{x.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOSAC);
    }
}

void do_autosplit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
    {
      send_to_char("Autosplitting {Rremoved{x.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOSPLIT);
    }
    else
    {
      send_to_char("Autosplitting {Gset.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOSPLIT);
    }
}

void do_brief(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_BRIEF))
    {
      send_to_char("{GFull descriptions activated{x.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_BRIEF);
    }
    else
    {
      send_to_char("{RShort descriptions activated{x.\n\r",ch);
      SET_BIT(ch->comm,COMM_BRIEF);
    }
}

void do_compact(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMPACT))
    {
      send_to_char("Compact mode {Rremoved{x.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_COMPACT);
    }
    else
    {
      send_to_char("Compact mode {Gset{x.\n\r",ch);
      SET_BIT(ch->comm,COMM_COMPACT);
    }
}

void do_show(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
    {
      send_to_char("Affects will {Rno longer {xbe shown in score.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
    else
    {
      send_to_char("Affects will {Gnow {xbe shown in score.\n\r",ch);
      SET_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
}

void do_prompt(CHAR_DATA *ch, char *argument)
{
	char buf[MSL];
 
   if ( argument[0] == '\0' )
   {
	if (IS_SET(ch->comm,COMM_PROMPT))
   	{
      	    send_to_char("You will {Rno longer see {xprompts.\n\r",ch);
      	    REMOVE_BIT(ch->comm,COMM_PROMPT);
    	}
    	else
    	{
      	    send_to_char("You will {Gnow see {xprompts.\n\r",ch);
      	    SET_BIT(ch->comm,COMM_PROMPT);
    	}
       return;
   }
	
    if ((argument[0] == 'c' ) &&
		(argument[1] == 'o' ) &&
		(argument[2] == 'l' ) &&
		(argument[3] == 'o' ) &&
		(argument[4] == 'r' ))
	{
		if (IS_SET(ch->comm,COMM_CPROMPT))
		{
		REMOVE_BIT( ch->comm, COMM_CPROMPT);
		}

		else
		{
		SET_BIT( ch->comm, COMM_CPROMPT);
		}
		return;
	}
	else
	{
		if ( strlen(argument) > 50 )
		{
			argument[50] = '\0';
		}
		strcpy( buf, argument );
		smash_tilde( buf );
		if (str_suffix("%c",buf))
		{
			strcat(buf," ");
		}
	}
	REMOVE_BIT( ch->comm, COMM_CPROMPT);
	free_string( ch->prompt );
	ch->prompt = str_dup( buf );
	sprintf(buf,"Prompt set to %s\n\r",ch->prompt );
	send_to_char(buf,ch);
	return;
}

void do_combine(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMBINE))
    {
      send_to_char("{RLong {xinventory selected.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_COMBINE);
    }
    else
    {
      send_to_char("{GCombined {xinventory selected.\n\r",ch);
      SET_BIT(ch->comm,COMM_COMBINE);
    }
}

void do_noloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_CANLOOT))
    {
      send_to_char("{GYour corpse is now safe from thieves{x.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_CANLOOT);
    }
    else
    {
      send_to_char("{M**{RYour corpse may now be looted.{M**{x\n\r",ch);
      SET_BIT(ch->act,PLR_CANLOOT);
    }
}

void do_nofollow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_NOFOLLOW))
    {
      send_to_char("{GYou now accept followers.{x\n\r",ch);
      REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    }
    else
    {
      send_to_char("{RYou no longer accept followers.{x\n\r",ch);
      SET_BIT(ch->act,PLR_NOFOLLOW);
      die_follower( ch );
    }
}

void do_nosummon(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
      if (IS_SET(ch->imm_flags,IMM_SUMMON))
      {
	send_to_char("You are {Gno longer immune {xto summon.\n\r",ch);
	REMOVE_BIT(ch->imm_flags,IMM_SUMMON);
      }
      else
      {
	send_to_char("You are {Rnow immune {xto summoning.\n\r",ch);
	SET_BIT(ch->imm_flags,IMM_SUMMON);
      }
    }
    else
    {
      if (IS_SET(ch->act,PLR_NOSUMMON))
      {
        send_to_char("You are {Gno longer immune {xto summon.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_NOSUMMON);
      }
      else
      {
        send_to_char("You are {Rnow immune {xto summoning.\n\r",ch);
        SET_BIT(ch->act,PLR_NOSUMMON);
      }
    }
}

/*
 *
 *  CHANGES MADE BELOW TO ADD COLOR TO ROM FROM WITHIN THE CODE.
 *
 */
void do_look( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number,count;

    if ( ch->desc == NULL )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   room_is_dark( ch->in_room ) )
    {
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    number = number_argument(arg1,arg3);
    count = 0;

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {

/* the line below adds a white room title surrounded by blue (). (Temple Of Mota)*/
	sprintf( buf, "{g%s{x", ch->in_room->name );
	send_to_char( buf, ch );

	if ( (IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_SET(ch->act,PLR_HOLYLIGHT)))
	||   IS_BUILDER(ch, ch->in_room->area) )
	{
/* Same as above but adds color for the room vnums for Imp's/Imm's (). (Temple Of Mota)*/
	    sprintf(buf," ||{gRoom %d{x",ch->in_room->vnum);
	    send_to_char(buf,ch);
	}
	send_to_char( "{x\n\r", ch );

	if ( arg1[0] == '\0' || ( !IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF) ) )
	{
		if (ch->desc && !ch->desc->run_buf)
		{
	    sprintf( buf, "{w  %s{x", ch->in_room->description );
	    send_to_char( buf, ch );
		}
	}
	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
	{
	    send_to_char("\n\r",ch);
            do_function(ch, &do_exits, "auto" );
	}
	send_to_char("{g",ch);
	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	send_to_char("{x",ch);
	send_to_char("{g",ch);
	show_char_to_char( ch->in_room->people,   ch );
	send_to_char("{x",ch);
	return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp(arg1, "in")  || !str_cmp(arg1,"on"))
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, NULL, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
		break;
	    }

	    sprintf( buf, "It's %sfilled with  a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than half-" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half-"     : "more than half-",
		liq_table[obj->value[2]].liq_color
		);

	    send_to_char( buf, ch );
	    break;
	
	case ITEM_QUIVER:
		if ( obj->value[0] <= 0 )
		{
		send_to_char( "{WThe quiver is out of arrows.{x\n\r", ch );
		break;
		}
		
		if (obj->value[0] == 1 )
		{
		send_to_char( "{WThe quiver has 1 arrow remaining in it.{x\n\r", ch );
		break;
		}
		
		if (obj->value[0] > 1 )
		{
		sprintf( buf, "{WThe quiver has %d arrows in it.{x\n\r", obj->value[0]);
		}
		send_to_char( buf, ch);
		break;

	case ITEM_CONTAINER:
	case ITEM_GOLEM_BAG:
	case ITEM_SHEATH:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	    act( "$p holds:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }


    if ( ( victim = get_char_room( ch,NULL, arg1 ) ) != NULL )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{  /* player can see object */
	    pdesc = get_extra_descr( arg3, obj->extra_descr );
	    if ( pdesc != NULL )
		{
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    return;
	    	}
	    	else continue;
		}
 	    pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
 	    if ( pdesc != NULL )
		{
 	    	if (++count == number)
 	    	{	
		    send_to_char( pdesc, ch );
		    return;
	     	}
			else continue;
		}

	    if ( is_name( arg3, obj->name ) )
	    	if (++count == number)
	    	{
	    	    send_to_char( obj->description, ch );
	    	    send_to_char( "\n\r",ch);
		    return;
		  }
	  }
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg3, obj->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    return;
	    	}

	    pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    return;
	    	}

	    if ( is_name( arg3, obj->name ) )
		if (++count == number)
		{
		    send_to_char( obj->description, ch );
		    send_to_char("\n\r",ch);
		    return;
		}
	}
    }

    pdesc = get_extra_descr(arg3,ch->in_room->extra_descr);
    if (pdesc != NULL)
    {
	if (++count == number)
	{
	    send_to_char(pdesc,ch);
	    return;
	}
    }
    
    if (count > 0 && count != number)
    {
    	if (count == 1)
    	    sprintf(buf,"You only see one %s here.\n\r",arg3);
    	else
    	    sprintf(buf,"You only see %d of those here.\n\r",count);
    	
    	send_to_char(buf,ch);
    	return;
    }

         if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	}
    }

    return;
}

/* RT added back for the hell of it */
void do_read (CHAR_DATA *ch, char *argument )
{
    do_function(ch, &do_look, argument);
}

void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    do_function(ch, &do_look, arg );

    if ( ( obj = get_obj_here( ch, NULL, arg ) ) != NULL )
    {
	switch ( obj->item_type )
	{
	default:
	    break;
	
	case ITEM_JUKEBOX:
	    do_function(ch, &do_play, "list");
	    break;

	case ITEM_MONEY:
	    if (obj->value[0] == 0)
	    {
	        if (obj->value[1] == 0)
		    sprintf(buf,"Odd...there's no coins in the pile.\n\r");
		else if (obj->value[1] == 1)
		    sprintf(buf,"Wow. One gold coin.\n\r");
		else
		    sprintf(buf,"There are %d gold coins in the pile.\n\r",
			obj->value[1]);
	    }
	    else if (obj->value[1] == 0)
	    {
		if (obj->value[0] == 1)
		    sprintf(buf,"Wow. One silver coin.\n\r");
		else
		    sprintf(buf,"There are %d silver coins in the pile.\n\r",
			obj->value[0]);
	    }
	    else
		sprintf(buf,
		    "There are %d gold and %d silver coins in the pile.\n\r",
		    obj->value[1],obj->value[0]);
	    send_to_char(buf,ch);
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_GOLEM_BAG:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    sprintf(buf,"in %s",argument);
	    do_function(ch, &do_look, buf );
	}
    }

    return;
}



/*
* Thanks to Zrin for auto-exit part.
*/
void do_exits( CHAR_DATA *ch, char *argument )
{
	//    extern char * const dir_name[];
	const char *dir_name[] = {"north","east","south","west","up","down"};
	char buf[MAX_STRING_LENGTH];
	EXIT_DATA *pexit;
	bool found;
	bool fAuto;
	int door;

	fAuto  = !str_cmp( argument, "auto" );

	if ( !check_blind( ch ) )
		return;

	if (fAuto)
		sprintf(buf,"{D[{DExits:{x");
	else if (IS_IMMORTAL(ch))
		sprintf(buf,"Obvious exits from room %d:\n\r",ch->in_room->vnum);
	else
		sprintf(buf,"Obvious exits:\n\r");

	found = FALSE;
	for ( door = 0; door <= 5; door++ )
	{
		if ( ( pexit = ch->in_room->exit[door] ) != NULL
			&&   pexit->u1.to_room != NULL
			&&   can_see_room(ch,pexit->u1.to_room) )
		{
			if(IS_SET(pexit->exit_info, EX_CLOSED))
			{
				found = TRUE;
				if ( fAuto )
				{
					strcat( buf, " {y<{x" );
					strcat( buf, dir_name[door] );
					strcat( buf, "{y>{x" );
				}
				else
				{
					sprintf( buf + strlen(buf), "{D[{x%-5s{D]{x - %s",
						capitalize( dir_name[door] ),
						room_is_dark( pexit->u1.to_room )
						?  "Too dark to tell"
						: pexit->u1.to_room->name
						);
					if (IS_IMMORTAL(ch))
						sprintf(buf + strlen(buf), 
						" (room %d)\n\r",pexit->u1.to_room->vnum);
					else
						sprintf(buf + strlen(buf), "\n\r");
				}
			}
			else
			{
				found = TRUE;
				if ( fAuto )
				{
					strcat( buf, " " );
					strcat( buf, dir_name[door] );
				}
				else
				{
					sprintf( buf + strlen(buf), " %-5s  - %s",
						capitalize( dir_name[door] ),
						room_is_dark( pexit->u1.to_room )
						?  "Too dark to tell"
						: pexit->u1.to_room->name
						);
					if (IS_IMMORTAL(ch))
						sprintf(buf + strlen(buf), 
						" (room %d)\n\r",pexit->u1.to_room->vnum);
					else
						sprintf(buf + strlen(buf), "\n\r");
				}
			}
		}
	}

    if ( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	strcat( buf, "{D]{x\n\r" );

    send_to_char( buf, ch );
    return;
}

void do_worth( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
	sprintf(buf,"You have %ld gold and %ld silver.\n\r",
	    ch->gold,ch->silver);
	send_to_char(buf,ch);
	return;
    }

    sprintf(buf, 
    "You have %ld gold, %ld silver, and %d experience (%d exp to level).\n\r",
	ch->gold, ch->silver,ch->exp,
	(ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);

    send_to_char(buf,ch);

    return;
}


void do_score( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];

	send_to_char("{b---------------------------------------------------------------------------------{x\n\r", ch);
	sprintf( buf,
		"{xName: {W%s{x			{xLevel: {W%d{x			{xAge: {W%d (%d hours){x.\n\r",
		ch->name,
		ch->level, get_age(ch),
		( ch->played + (int) (current_time - ch->logon) ) / 3600);
	send_to_char( buf, ch );

	if ( is_clan(ch) )
	{
		sprintf( buf, "{xClan: {W%s			{xRank: {W%s%s\n\r",
			player_clan(ch), player_rank(ch),
			IS_SET(ch->act,PLR_MORTAL_LEADER)? "{D({WLeader{D){x" : "");
		send_to_char(buf, ch);
	}

	if ( get_trust( ch ) != ch->level )
	{
		sprintf( buf, "You are trusted at level %d.\n\r",
			get_trust( ch ) );
		send_to_char( buf, ch );
	}

	sprintf(buf,
		"{xRace: {W%s			{xSex: {W%s			{xClass: {W%s{x\n\r",
		race_table[ch->race].name,
		ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
		IS_NPC(ch) ? "mobile" : class_table[ch->class].name);
	send_to_char(buf,ch);


	sprintf( buf,
		"{xHit Points: {G%ld{W/{G%ld		{xMana: {G%d{W/{G%d		{xMoves: {G%d{W/{G%d{x\n\r",
		ch->hit,  ch->max_hit,
		ch->mana, ch->max_mana,
		ch->move, ch->max_move);
	send_to_char( buf, ch );

	sprintf( buf,
		"{xPracs: {W%d			{xTrains: {W%d			{xSkill Points: {W%ld{x\n\r",
		ch->practice, ch->train,ch->pcdata->sp);
	send_to_char( buf, ch );

	sprintf( buf,
		"{xItems: {W%d/%d			{xWeight: {W%ld/%d{x\n\r",
		ch->carry_number, can_carry_n(ch),
		get_carry_weight(ch) / 10, can_carry_w(ch) /10 );
	send_to_char( buf, ch );

	sprintf( buf,
		"{xStr: {W%d{D({W%d{D)	{xInt: {W%d{D({W%d{D)	{xWis: {W%d{D({W%d{D)	{xDex: {W%d{D({W%d{D)	{xCon: {W%d{D({W%d{D){x\n\r",
		ch->perm_stat[STAT_STR],
		get_curr_stat(ch,STAT_STR),
		ch->perm_stat[STAT_INT],
		get_curr_stat(ch,STAT_INT),
		ch->perm_stat[STAT_WIS],
		get_curr_stat(ch,STAT_WIS),
		ch->perm_stat[STAT_DEX],
		get_curr_stat(ch,STAT_DEX),
		ch->perm_stat[STAT_CON],
		get_curr_stat(ch,STAT_CON) );
	send_to_char( buf, ch );

	sprintf( buf,
		"{xExp: {W%d	{yGold: {W%ld		{wSilver: {W%ld	{xBank: {W%ld{x\n\r",
		ch->exp,  ch->gold, ch->silver, ch->balance );
	send_to_char( buf, ch );
	if (ch->level == LEVEL_HERO && !IS_NPC(ch))
	{
		sprintf( buf,"{MHero's Exp: {W%ld{x\n\r",ch->pcdata->hero_exp);
		send_to_char(buf,ch);
	}

	if (!IS_NPC(ch))
	{
		if (ch->countdown > 0)
		{
			sprintf( buf, "{xQuest Points: {W%d\t\t{RQuest Time Left: {W%d{x\n\r", ch->questpoints, ch->countdown);
			send_to_char(buf,ch);
		}

		else
		{
			sprintf( buf, "{xQuest Points: {W%d\t\t{xTill Next Quest: {W%d{x\n\r", ch->questpoints, ch->nextquest);
			send_to_char(buf,ch);
		}
	}
	/* RT shows exp to level */
	if (!IS_NPC(ch) && ch->level < LEVEL_HERO)
	{
		sprintf (buf, 
			"{xExp to level:{W %d{x\n\r",
			((ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp));
		send_to_char( buf, ch );
	}

	sprintf( buf, "Wimpy set to {W%d{x hit points.\n\r", ch->wimpy );
	send_to_char( buf, ch );

	if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
		send_to_char( "You are drunk.\n\r",   ch );
	if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
		send_to_char( "You are thirsty.\n\r", ch );
	if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER]   ==  0 )
		send_to_char( "You are hungry.\n\r",  ch );

	switch ( ch->position )
	{
	case POS_DEAD:     
		send_to_char( "You are DEAD!!\n\r",		ch );
		break;
	case POS_MORTAL:
		send_to_char( "You are mortally wounded.\n\r",	ch );
		break;
	case POS_INCAP:
		send_to_char( "You are incapacitated.\n\r",	ch );
		break;
	case POS_STUNNED:
		send_to_char( "You are stunned.\n\r",		ch );
		break;
	case POS_SLEEPING:
		send_to_char( "You are sleeping.\n\r",		ch );
		break;
	case POS_RESTING:
		send_to_char( "You are resting.\n\r",		ch );
		break;
	case POS_SITTING:
		send_to_char( "You are sitting.\n\r",		ch );
		break;
	case POS_STANDING:
		send_to_char( "You are standing.\n\r",		ch );
		break;
	case POS_FIGHTING:
		send_to_char( "You are fighting.\n\r",		ch );
		break;
	}


	/* print AC values */
	if (ch->level >= 25)
	{	
		sprintf( buf,"{x(AC) {xPierce: {W%ld  {xBash: {W%ld  {xSlash: {W%ld  {xExotic: {W%ld{x\n\r",
			GET_AC(ch,AC_PIERCE),
			GET_AC(ch,AC_BASH),
			GET_AC(ch,AC_SLASH),
			GET_AC(ch,AC_EXOTIC));
		send_to_char(buf,ch);
	}




	/* RT wizinvis and holy light */
	if ( IS_IMMORTAL(ch))
	{
		send_to_char("Holy Light: ",ch);
		if (IS_SET(ch->act,PLR_HOLYLIGHT))
			send_to_char("{Won{x",ch);
		else
			send_to_char("{Woff{x",ch);

		if (ch->invis_level)
		{
			sprintf( buf, "  Invisible: level {W%d{x",ch->invis_level);
			send_to_char(buf,ch);
		}

		if (ch->incog_level)
		{
			sprintf(buf,"  Incognito: level {W%d{x",ch->incog_level);
			send_to_char(buf,ch);
		}
		send_to_char("\n\r",ch);
	}

	{
		sprintf( buf, "{xHitroll: {W%ld  {xDamroll: {W%ld{x.\n\r",
			GET_HITROLL(ch), GET_DAMROLL(ch) );
		send_to_char( buf, ch );
	}

	if (!IS_NPC(ch))
	{
		sprintf( buf, "{xArena Record: {W%d-%d\t\t{xHonor Circle Record {W%d-%d{x\n\r",
			ch->pcdata->arena_wins,ch->pcdata->arena_losses,
			ch->pcdata->hc_wins,ch->pcdata->hc_losses);
		send_to_char( buf, ch );
	}
	/*begin vres printing*/
	stc("{w+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++{x\n\r", ch);
	stc("{WResistances:{x\n\r", ch);
	ptc(ch,"{CCold\t{W%2d\t{WHoly\t{W%2d\t{bBash\t{W%2d\t{CMagic\t{W%2d\t{cSound\t{W%2d{x\n\r",
		ch->vres_cold, ch->vres_holy, ch->vres_bash, ch->vres_magic, ch->vres_sound);
	ptc(ch,"{RFire\t{W%2d\t{YLtning\t{W%2d\t{gPoison\t{W%2d\t{RWeapon\t{W%2d{x\n\r",
		ch->vres_fire, ch->vres_lightning, ch->vres_poison, ch->vres_weapon);
	ptc(ch,"{GAcid\t{W%2d\t{mPierce\t{W%2d\t{rEnergy\t{W%2d\t{MMental\t{W%2d{x\n\r",
		ch->vres_acid, ch->vres_pierce, ch->vres_energy, ch->vres_mental);
	ptc(ch,"{DNeg\t{W%2d\t{cSlash\t{W%2d\t{yDisease\t{W%2d\t{YLight\t{W%2d{x\n\r",
		ch->vres_negative, ch->vres_slash, ch->vres_disease, ch->vres_light);	
	stc("{w+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++{x\n\r", ch);
	/*end vres printing*/
	{
		sprintf( buf, "Alignment: {W%d{x.  ", ch->alignment );
		send_to_char( buf, ch );
	}

	send_to_char( "You are ", ch );
	if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
	else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
	else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
	else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
	else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
	else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
	else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
	else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
	else                             send_to_char( "satanic.\n\r", ch );

	if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
		do_function(ch, &do_affects, "");
	send_to_char("{b---------------------------------------------------------------------------------{x\n\r", ch);
}

void do_affects(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf, *paf_last = NULL;
    char buf[MAX_STRING_LENGTH];
    
    if ( ch->affected != NULL )
    {
	send_to_char( "You are affected by the following spells:\n\r", ch );
	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
	    if (paf_last != NULL && paf->type == paf_last->type)
		if (ch->level >= 20)
		    sprintf( buf, "                      ");
		else
		    continue;
	    else
	    	sprintf( buf, "{YSpell: {g%-15s", skill_table[paf->type].name );

	    send_to_char( buf, ch );

	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
		    "{W: {wmodifies {W%s {gby {W%d{x ",
		    affect_loc_name( paf->location ),
		    paf->modifier);
		send_to_char( buf, ch );
		if ( paf->duration == -1 )
		    sprintf( buf, "permanently" );
		else
		    sprintf( buf, "{gfor {W%d hours{x", paf->duration );
		send_to_char( buf, ch );
	    }

	    send_to_char( "\n\r", ch );
	    paf_last = paf;
	}
    }
    else 
	send_to_char("You are not affected by any spells.\n\r",ch);

    return;
}



char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	"It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r",
	(time_info.hour % 12 == 0) ? 12 : time_info.hour %12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month]);
    send_to_char(buf,ch);
    sprintf(buf,"ROM started up at %s\n\rThe system time is %s.\n\r",
	str_boot_time,
	(char *) ctime( &current_time )
	);

    send_to_char( buf, ch );
    return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    static char * const sky_look[4] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
    }

    sprintf( buf, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
    send_to_char( buf, ch );
    return;
}

void do_help( CHAR_DATA *ch, char *argument )
{
	HELP_DATA *pHelp;
	bool found = FALSE;
	char argall[MIL],argone[MIL],output[MSL];
	int level;

	if ( argument[0] == '\0' )
		argument = "summary";

	/* this parts handles help a b so that it returns help 'a b' */
	argall[0] = '\0';
	while (argument[0] != '\0' )
	{
		argument = one_argument(argument,argone);
		if (argall[0] != '\0')
			strcat(argall," ");
		strcat(argall,argone);
	}

//	sprintf(output,"");
	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
	{
		level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

		if (level > get_trust( ch ) )
			continue;

		if ( is_name( argall, pHelp->keyword ) )
		{
			/* add seperator if found */
			if (found)
				strcat(output,"\n\r============================================================\n\r\n\r");
			if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
			{
				strcat(output,pHelp->keyword);
				strcat(output,"\n\r");
			}

			/*
			* Strip leading '.' to allow initial blanks.
			*/
			if ( pHelp->text[0] == '.' )
				strcat(output,pHelp->text+1);
			else
				strcat(output,pHelp->text);
			found = TRUE;
			/* small hack :) */
			if (ch->desc != NULL 
				&& ch->desc->connected != CON_PLAYING 
				&& ch->desc->connected != CON_GEN_GROUPS)
				break;
		}
	}

	if (!found)
		send_to_char( "No help on that word.\n\r", ch );
	else
		stc(output,ch);
}


/* whois command */
void do_whois (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    BUFFER *output;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    bool found = FALSE;
	char buf3[MSL];

    one_argument(argument,arg);
  
    if (arg[0] == '\0')
    {
	send_to_char("You must provide a name.\n\r",ch);
	return;
    }

    output = new_buf();

    for (d = descriptor_list; d != NULL; d = d->next)
    {
	CHAR_DATA *wch;
	char const *class;

 	if (d->connected != CON_PLAYING || !can_see(ch,d->character))
	    continue;
	
	wch = ( d->original != NULL ) ? d->original : d->character;

 	if (!can_see(ch,wch))
	    continue;

	if (!str_prefix(arg,wch->name))
	{
	    found = TRUE;
	    
	    /* work out the printing */
	    class = class_table[wch->class].who_name;
	    switch(wch->level)
	    {
		case MAX_LEVEL - 0 : class = "IMP"; 	break;
		case MAX_LEVEL - 1 : class = "CRE";	break;
		case MAX_LEVEL - 2 : class = "SUP";	break;
		case MAX_LEVEL - 3 : class = "DEI";	break;
		case MAX_LEVEL - 4 : class = "GOD";	break;
		case MAX_LEVEL - 5 : class = "IMM";	break;
		case MAX_LEVEL - 6 : class = "DEM";	break;
		case MAX_LEVEL - 7 : class = "ANG";	break;
		case MAX_LEVEL - 8 : class = "AVA";	break;
	    }
	
	if (wch->level > MAX_LEVEL - 8 && wch->immtitle != NULL)
	sprintf(buf3, "{D[  %s  {D]{x  ", wch->immtitle);

	else if ((wch->level == 101) && (wch->pcdata->incarnations == 0))
		sprintf(buf3, "{D[  {GD{CE{GM{CI{GH{CE{GR{CO   {D]{x  ");

	else if ((wch->level == 101))
		sprintf(buf3, "{D[     {RH{WE{BR{RO    {D]{x  ");

	else
    sprintf( buf3, "{D[{Y%3d {B%4s {R%s{D]{x  ", wch->level,
             wch->race < MAX_PC_RACE ?
             pc_race_table[wch->race].who_name : "        ", class);

    
		send_to_char("\n\r",ch);
		sprintf( buf, "{g%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s{x\n\r", buf3 , 
        IS_SET(wch->act, PLR_PROKILLER) ? "{R*{x" : "",
		wch->incog_level >= LEVEL_HERO ? "{D[{bI{cn{bc{co{bg{D]{x" : "",
	    wch->invis_level >= LEVEL_HERO ? "{D[{mW{wiz{mi{D]{x" : "",
	    (is_clan(wch) || IS_SET(clan_table[wch->clan].flags,GUILD_INDEPENDENT)) ?
			clan_table[wch->clan].who_name : "",
	    IS_SET(wch->comm, COMM_AFK) ? "{D[{RAFK{D]{x" : "",
        IS_SET(wch->act, PLR_THIEF)  ? "{D[{GTHIEF{D]{x"  : "",
		IS_SET(wch->plr2, PLR2_ARENA)	 ? "{D[{YA{GR{BE{MN{CA{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_RED_TEAM)	 ? "{D[{RR{Re{Rd{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_BLUE_TEAM)	 ? "{D[{BB{bl{Bu{be{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_RED_LEADER)	 ? "{D[{RLdr{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_BLUE_LEADER)	 ? "{D[{BLdr{D]{x"	:	"",
		IS_SET(wch->act, PLR_CODING)	 ? "{D[{CC{cO{WDI{cN{CG{D]{x"	:	"",
		IS_SET(wch->act, PLR_QUESTOR)	 ? "{D[{cQUESTOR{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_CHALLENGER)	? "{D[{YChallenger{D]{x"	: "",
		IS_SET(wch->plr2, PLR2_CHALLENGED)	? "{D[{YChallenged{D]{x"	: "",
	    wch->short_descr,
	    IS_NPC(wch) ? "" : wch->pcdata->title );
	stc(buf,ch);
	sprintf(buf, "{xLevel:{G %d{x\n\r", wch->level);
	stc(buf, ch);

	sprintf(buf, "{xRace:{B %s{x Class:{R %s{x\n\r",
		pc_race_table[wch->race].name,
		class_table[wch->class].name );
	send_to_char(buf, ch);

	sprintf( buf, "{YA{GR{BE{MN{CA	{GWins:	{D[{G%d{D]		{YHonor Circle	{GWins:	{D[{G%d{D]{x\n\r",
		wch->pcdata->arena_wins,
		wch->pcdata->hc_wins);
	stc( buf, ch );
	sprintf( buf, "	{RLosses:	{D[{R%d{D]				{RLosses:	{D[{R%d{D]{x.\n\r",
		wch->pcdata->arena_losses,
		wch->pcdata->hc_losses);
	stc( buf, ch );
    }
	}

    if (!found)
    {
	send_to_char("No one of that name is playing.\n\r",ch);
	return;
    }

    page_to_char(buf_string(output),ch);
    free_buf(output);
}


/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    int wlevel;
	int iClass;
    int iRace;
    int iClan;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_PC_RACE];
    bool rgfClan[MAX_CLAN];
    bool fClassRestrict = FALSE;
    bool fClanRestrict = FALSE;
    bool fClan = FALSE;
    bool fRaceRestrict = FALSE;
    bool fImmortalOnly = FALSE;
	char buf3[MAX_STRING_LENGTH];

    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        rgfClass[iClass] = FALSE;
    for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
        rgfRace[iRace] = FALSE;
    for (iClan = 0; iClan < MAX_CLAN; iClan++)
	rgfClan[iClan] = FALSE;
 
    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
        char arg[MAX_STRING_LENGTH];
 
        argument = one_argument( argument, arg );
        if ( arg[0] == '\0' )
            break;
 
        if ( is_number( arg ) )
        {
            switch ( ++nNumber )
            {
            case 1: iLevelLower = atoi( arg ); break;
            case 2: iLevelUpper = atoi( arg ); break;
            default:
                send_to_char( "Only two level numbers allowed.\n\r", ch );
                return;
            }
        }
        else
        {
 
            /*
             * Look for classes to turn on.
             */
            if (!str_prefix(arg,"immortals"))
            {
                fImmortalOnly = TRUE;
            }
            else
            {
                iClass = class_lookup(arg);
                if (iClass == -1)
                {
                    iRace = race_lookup(arg);
 
                    if (iRace == 0 || iRace >= MAX_PC_RACE)
		    {
			if (!str_prefix(arg,"clan"))
			    fClan = TRUE;
			else
		        {
			    iClan = clan_lookup(arg);
			    if (iClan)
			    {
				fClanRestrict = TRUE;
			   	rgfClan[iClan] = TRUE;
			    }
			    else
			    {
                        	send_to_char(
                            	"That's not a valid race, class, or clan.\n\r",
				   ch);
                            	return;
			    }
                        }
		    }
                    else
                    {
                        fRaceRestrict = TRUE;
                        rgfRace[iRace] = TRUE;
                    }
                }
                else
                {
                    fClassRestrict = TRUE;
                    rgfClass[iClass] = TRUE;
                }
            }
        }
    }
 
    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    output = new_buf();
    
	/* Thunder -- Beginning for-loop for sorted who */
	for( wlevel=MAX_LEVEL; wlevel>0; wlevel-- )
	{
    for ( d = descriptor_list; d != NULL; d = d->next )

    {
        CHAR_DATA *wch;
        char const *class;
 
        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
            continue;
 
        wch   = ( d->original != NULL ) ? d->original : d->character;

	if( wch->level != wlevel )
		continue;
	if (!can_see(ch,wch))
	    continue;

        if ( wch->level < iLevelLower
        ||   wch->level > iLevelUpper
        || ( fImmortalOnly  && wch->level < LEVEL_IMMORTAL )
        || ( fClassRestrict && !rgfClass[wch->class] )
        || ( fRaceRestrict && !rgfRace[wch->race])
 	|| ( fClan && !is_clan(wch))
	|| ( fClanRestrict && !rgfClan[wch->clan]))
            continue;
 
        nMatch++;
 
        /*
         * Figure out what to print for class.
	 */

	class = class_table[wch->class].who_name;
	switch ( wch->level )
	{
	default: break;
            {
                case MAX_LEVEL - 0 : class = "IMP";     break;
                case MAX_LEVEL - 1 : class = "CRE";     break;
                case MAX_LEVEL - 2 : class = "SUP";     break;
                case MAX_LEVEL - 3 : class = "DEI";     break;
                case MAX_LEVEL - 4 : class = "GOD";     break;
                case MAX_LEVEL - 5 : class = "IMM";     break;
                case MAX_LEVEL - 6 : class = "DEM";     break;
                case MAX_LEVEL - 7 : class = "ANG";     break;
                case MAX_LEVEL - 8 : class = "AVA";     break;
            }
	}

	if (wch->level > MAX_LEVEL - 8 && wch->immtitle != NULL)
	sprintf(buf3, "{D[  %s  {D]{x  ", wch->immtitle);

	else if ((wch->level == 101) && (wch->pcdata->incarnations == 0))
		sprintf(buf3, "{D[  {GD{CE{GM{CI{GH{CE{GR{CO   {D]{x  ");

	else if ((wch->level == 101))
		sprintf(buf3, "{D[     {RH{WE{BR{RO    {D]{x  ");

	else
    sprintf( buf3, "{D[{Y%3d {B%4s {R%s{D]{x  ", wch->level,
             wch->race < MAX_PC_RACE ?
             pc_race_table[wch->race].who_name : "        ", class);

	/*
	 * Format it up.
	 */
	sprintf( buf, "{g%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s{x\n\r", buf3 , 
        IS_SET(wch->act, PLR_PROKILLER) ? "{R*{x" : "",
		wch->incog_level >= LEVEL_HERO ? "{D[{bI{cn{bc{co{bg{D]{x" : "",
	    wch->invis_level >= LEVEL_HERO ? "{D[{mW{wiz{mi{D]{x" : "",
	    (is_clan(wch) || IS_SET(clan_table[wch->clan].flags,GUILD_INDEPENDENT)) ?
			clan_table[wch->clan].who_name : "",
		IS_SET(wch->act,PLR_MORTAL_LEADER) ? "{D[{WLdr{D]{x" : "",
	    IS_SET(wch->comm, COMM_AFK) ? "{D[{RAFK{D]{x" : "",
        IS_SET(wch->act, PLR_THIEF)  ? "{D[{GTHIEF{D]{x"  : "",
		IS_SET(wch->plr2, PLR2_ARENA)	 ? "{D[{YA{GR{BE{MN{CA{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_RAIDER)	? "{D[{RR{rA{DID{rE{RR{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_RAIDED)	? "{D[{RR{DA{rID{DE{RD{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_RED_TEAM)	 ? "{D[{RR{Re{Rd{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_BLUE_TEAM)	 ? "{D[{BB{bl{Bu{be{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_RED_LEADER)	 ? "{D[{RLdr{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_BLUE_LEADER)	 ? "{D[{BLdr{D]{x"	:	"",
		IS_SET(wch->act, PLR_CODING)	 ? "{D[{CC{cO{WDI{cN{CG{D]{x"	:	"",
		IS_SET(wch->act, PLR_QUESTOR)	 ? "{D[{cQUESTOR{D]{x"	:	"",
		IS_SET(wch->plr2, PLR2_CHALLENGER)	? "{D[{YChallenger{D]{x"	: "",
		IS_SET(wch->plr2, PLR2_CHALLENGED)	? "{D[{YChallenged{D]{x"	: "",
	    wch->short_descr,
	    IS_NPC(wch) ? "" : wch->pcdata->title );
	add_buf(output,buf);
    }
	} /* End of wlevel for-loop for sorted who */
    
	sprintf( buf2, "\n\rPlayers found: %d\n\r", nMatch );
    add_buf(output,buf2);
    page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}

void do_count ( CHAR_DATA *ch, char *argument )
{
    int count, max;
    DESCRIPTOR_DATA *d;
	FILE *fp;
	char buf[MSL];

    count = 0;

    if ( IS_NPC(ch) || ch->desc == NULL )
    	return;

    for ( d = descriptor_list; d != NULL; d = d->next )
	{
        if ( d->connected == CON_PLAYING && can_see( ch, d->character ) )
		{
			count++;
		}
	}

    max_on = UMAX(count,max_on);

	if ( ( fp = fopen( MAX_WHO_FILE,"r" ) ) == NULL )
	{
	    log_string("Error reading from maxwho.txt");
	    return;
	}
	max = fread_number( fp );
	fclose(fp);

	if ( max_on > max )
	{
		if ( ( fp = fopen( MAX_WHO_FILE,"w" ) ) == NULL )
		{
		    log_string("Error writing to maxwho.txt");
		    return;
		}
		fprintf( fp, "%d\n", max_on );
		fclose(fp);
	}

	sprintf(buf,"The largest number of active players today was %d.\n\r", max_on );
	send_to_char(buf,ch);
	sprintf(buf,"The largest number of active players ever was %d.\n\r", max );
	stc(buf,ch);
	sprintf(buf,"You can see %d characters\n\r", count );
	stc(buf,ch);
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	    continue;

	send_to_char( where_name[iWear], ch );
	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	{
	    send_to_char( "something.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
	send_to_char( "Nothing.\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if (arg2[0] == '\0')
    {
	for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
	{
	    if (obj2->wear_loc != WEAR_NONE
	    &&  can_see_obj(ch,obj2)
	    &&  obj1->item_type == obj2->item_type
	    &&  (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if (obj2 == NULL)
	{
	    send_to_char("You aren't wearing anything comparable.\n\r",ch);
	    return;
	}
    } 

    else if ( (obj2 = get_obj_carry(ch,arg2,ch) ) == NULL )
    {
	send_to_char("You do not have that item.\n\r",ch);
	return;
    }

    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
	    break;

	case ITEM_WEAPON:
	    if (obj1->pIndexData->new_format)
		value1 = (1 + obj1->value[2]) * obj1->value[1];
	    else
	    	value1 = obj1->value[1] + obj1->value[2];

	    if (obj2->pIndexData->new_format)
		value2 = (1 + obj2->value[2]) * obj2->value[1];
	    else
	    	value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_function(ch, &do_help, "diku" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Players near you:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room != NULL
	    &&   !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
 	    &&   (is_room_owner(ch,victim->in_room) 
	    ||    !room_is_private(victim->in_room))
	    &&   victim->in_room->area == ch->in_room->area
	    &&   can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    victim->name, victim->in_room->name );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
	    if ( victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   !IS_AFFECTED(victim, AFF_HIDE)
	    &&   !IS_AFFECTED(victim, AFF_SNEAK)
		&&   !IS_AFFECTED2(victim, AFF_SHADOW_SLIP)
	    &&   can_see( ch, victim )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    PERS(victim, ch), victim->in_room->name );
		send_to_char( buf, ch );
		break;
	    }
	}
	if ( !found )
	    act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim))
    {
	send_to_char("Don't even think about it.\n\r",ch);
	return;
    }

    diff = victim->level - ch->level;

         if ( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -5 ) msg = "$N is no match for you.";
    else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if ( diff <=   1 ) msg = "The perfect match!";
    else if ( diff <=   4 ) msg = "$N says 'Do you feel lucky, punk?'.";
    else if ( diff <=   9 ) msg = "$N laughs at you mercilessly.";
    else                    msg = "Death will thank you for your gift.";

    act( msg, ch, NULL, victim, TO_CHAR );
    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}

void set_long_descr( CHAR_DATA *ch, char *long_descr )
{
    char buf[MAX_STRING_LENGTH];

    {
	strcpy( buf, long_descr );
    }

    free_string( ch->long_descr );
    ch->long_descr = str_dup( buf );
    return;
}


void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

	if(!IS_IMMORTAL(ch))
		strip_mortal_color( argument, FALSE );

    if ( strlen(argument) > 45 )
	argument[45] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );

    	if (argument[0] == '-')
    	{
            size_t len;
            bool found = FALSE;
 
            if (ch->description == NULL || ch->description[0] == '\0')
            {
                send_to_char("No lines left to remove.\n\r",ch);
                return;
            }
	
  	    strcpy(buf,ch->description);
 
            for (len = STRLEN(buf); len > 0; len--)
            {
                if (buf[len] == '\r')
                {
                    if (!found)  /* back it up */
                    {
                        if (len > 0)
                            len--;
                        found = TRUE;
                    }
                    else /* found the second one */
                    {
                        buf[len + 1] = '\0';
			free_string(ch->description);
			ch->description = str_dup(buf);
			send_to_char( "Your description is:\n\r", ch );
			send_to_char( ch->description ? ch->description : 
			    "(None).\n\r", ch );
                        return;
                    }
                }
            }
            buf[0] = '\0';
	    free_string(ch->description);
	    ch->description = str_dup(buf);
	    send_to_char("Description cleared.\n\r",ch);
	    return;
        }
	if ( argument[0] == '+' )
	{
	    if ( ch->description != NULL )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}

        if ( strlen(buf) >= 1024)
	{
	    send_to_char( "Description too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    send_to_char( "Your description is:\n\r", ch );
    send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
    return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    sprintf( buf,
	"{WYou {wsay '{YI have {G%ld{W/{c%ld {Yhp {G%d{W/{c%d {Ymana {G%d{C{W/{c%d {Ymv {W%d {Yxp.{w'{x\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    send_to_char( buf, ch );

    sprintf( buf, "{W$n {wsays '{YI have {G%ld{W/{c%ld {Yhp {G%d{W/{c%d {Ymana {G%d{C{W/{c%d {Ymv {W%d {Yxp.{w'{x\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    act( buf, ch, NULL, NULL, TO_ROOM );

    return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;
	int rating;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	int col;

	col    = 0;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    if ( ch->level < skill_table[sn].skill_level[ch->class] 
	      || ch->pcdata->learned[sn] < 1 /* skill is not known */)
		continue;

	    sprintf( buf, "%-18s %3d%%  ",
		skill_table[sn].name, ch->pcdata->learned[sn] );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 )
		send_to_char( "\n\r", ch );
	}
	if ( col % 3 != 0 )
	    send_to_char( "\n\r", ch );

	sprintf( buf, "You have %d practice sessions left.\n\r",
	    ch->practice );
	send_to_char( buf, ch );
    }
    else
    {
	CHAR_DATA *mob;
	int adept;

	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}

	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
	{
	    if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
		break;
	}

	if ( mob == NULL )
	{
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    send_to_char( "You have no practice sessions left.\n\r", ch );
	    return;
	}

	if ( ( sn = find_spell( ch,argument ) ) < 0)
	{
		stc("You dont know that skill or spell or form\n\r",ch);
		return;
	}
	
		if (!IS_NPC(ch))
		{
			if (ch->level < skill_table[sn].skill_level[ch->class] )
			{
				stc("You're not high enough level to practice that skill\n\r",ch);
				return;
			}
			if (ch->pcdata->learned[sn] < 1)/* skill is not known */
			{
				stc("You dont even have the slightest clue where to begin\n\r",ch);
				return;
			}
			if ((skill_table[sn].rating[ch->class] == 0)
				&& skill_table[sn].skill_level[ch->class] != 101)
			{
				stc("Your class cannot perform that skill or cast that spell\n\r",ch);
				return;
			}
			if (!check_teach(ch,argument,skill_table[sn].teach_required, mob))
			{
				stc("{WThis teacher is unable to teach you that skill.{x\n\r",ch);
				stc("{WSearch for one more knowledged in what you seek to learn{x\n\r",ch);
				return;
			}
		}
		adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;

		if ( ch->pcdata->learned[sn] >= adept )
		{
			sprintf( buf, "You are already learned at %s.\n\r",
			skill_table[sn].name );
			send_to_char( buf, ch );
		}
		else
		{
			if (skill_table[sn].rating[ch->class] == 0)
			{
				rating = 1;
			}
			else
			{
				rating = skill_table[sn].rating[ch->class];
			}
			ch->practice--;
			ch->pcdata->learned[sn] += 
			int_app[get_curr_stat(ch,STAT_INT)].learn / rating;
			if ( ch->pcdata->learned[sn] < adept )
			{
				sprintf(buf,"You practice $T.  {D[{W%d{D]{x",
					ch->pcdata->learned[sn]);
				act( buf,ch, NULL, skill_table[sn].name, TO_CHAR );
				act( "$n practices $T.",
					ch, NULL, skill_table[sn].name, TO_ROOM );
			}
			else
			{
			ch->pcdata->learned[sn] = adept;
			act( "{WYou are now learned at $T.{x",
				ch, NULL, skill_table[sn].name, TO_CHAR );
			act( "{W$n is now learned at $T.{x",
				ch, NULL, skill_table[sn].name, TO_ROOM );
			}
		}
	}
    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit/2 )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
    }

    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}

void whowas(CHAR_DATA *ch, FILE *fp)
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    char *name,
         *race,
         *date,
         *host,
	 *title;
	int	clan = 0;
    int classnum, level, sex, arena_wins, arena_losses, hc_wins, hc_losses;

/* Initialize variables to Error checking states. */

    name = NULL;
    race = NULL;
    date = NULL;
    host = NULL;
    title = NULL;
    arena_wins = -1;
    arena_losses = -1;
    hc_wins = -1;
    hc_losses = -1;
    classnum = -1;
    level    = -1;
    sex      = -1;
          
    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;
	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
            if ( !str_cmp(word,"ArenaWins"))
            {
				arena_wins = fread_number( fp );
   				fMatch = TRUE;
				break;
			}
			if ( !str_cmp(word,"ArenaLosses"))
			{
				arena_losses = fread_number( fp );
   				fMatch = TRUE;
				break;
			}
	        break;

	case 'C':
            if ( !str_cmp( word, "Class" ) )
            {
		classnum = fread_number(fp);
                fMatch = TRUE;
                break;
            }
			if ( !str_cmp( word, "Clan" ) )
            {
				clan = clan_lookup(fread_string(fp));
				fMatch = TRUE;
                break;
            }
            break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
                if ( (name    == NULL)
                   ||(race    == NULL)
				   ||(classnum == -1)
                   ||(sex     == -1 )
                   ||(level   == -1 ) )
                {
                   send_to_char("Information not available.\n\r",ch);
                   return;
                }
	/*
	 *Now we have our name, race, level, class, 
	 * and hopefully the host of the char. 
	 */
	        sprintf(buf, "{xName: %s%s %s{x\n\r",clan_table[clan].who_name,name,title );
            send_to_char(buf, ch);
	
			sprintf(buf, "{xLevel:{G %d{x\n\r", level);
	        send_to_char(buf, ch);

	        sprintf(buf, "{xRace:{B %s{x Class:{R %s{x\n\r",
	               race, class_table[classnum].name );
	        send_to_char(buf, ch);

			sprintf( buf, "{YA{GR{BE{MN{CA	{GWins:	{D[{G%d{D]		{YHonor Circle	{GWins:	{D[{G%d{D]{x\n\r",
			arena_wins,hc_wins);
			send_to_char( buf, ch );
			sprintf( buf, "	{RLosses:	{D[{R%d{D]				{RLosses:	{D[{R%d{D]{x.\n\r",
		    arena_losses,hc_losses);
			send_to_char( buf, ch );

			free_string(name);
            free_string(race);
			free_string(date);
			free_string(host);
		return;
	    }
	    break;

	case 'H':

            if ( !str_cmp( word, "Hst" ) )
            {
		host = str_dup(fread_string( fp ));
		fMatch = TRUE;
		break;
            }
            if ( !str_cmp(word,"HcWins"))
            {
				hc_wins = fread_number( fp );
   				fMatch = TRUE;
				break;
			}
			if ( !str_cmp(word,"HcLosses"))
			{
				hc_losses = fread_number( fp );
   				fMatch = TRUE;
				break;
			}
	        break;
            break;

	case 'L':
            if ( !str_cmp(word,"Levl")
               ||!str_cmp(word,"Lev")
               ||!str_cmp(word,"Level") )
            {
		level = fread_number( fp );
   		fMatch = TRUE;
	        break;
            } 
            if ( !str_cmp(word, "LLog") )
            {
		date = str_dup(fread_string( fp ));
		fMatch = TRUE;
		break;
           }

	    break;
            

	case 'N':
            if (!str_cmp(word,"Name") )
 	    { 
		name = str_dup(fread_string( fp ));
		fMatch = TRUE;
		break;
            }
	    break;

	case 'R':

 	    if ( !str_cmp( word, "Race") )
        {
		race = str_dup(fread_string( fp ));
		fMatch = TRUE;
		break;
 	    }
	    break;

	case 'S':
	    if ( !str_cmp( word, "Sex") )
 	    {
	 	sex = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;
	case 'T':
	    if ( !str_cmp( word, "Titl") )
 	    {
	 	title = str_dup(fread_string( fp ));
		fMatch = TRUE;
		break;
	    }
	    break;
        }

	if ( !fMatch )
	{
	    fread_to_eol( fp );
	}
    }
}

/* 
 * Tyric 6/6/97 This function requires that the char passed in to the 
 * function is not logged in.  If the char is, return a message that 
 * the char is on.  Otherwise, show date and time last logged on, and
 * also if the char using whowas is immortal, show the site logged in from.
 */
void do_whowas(CHAR_DATA *ch, char *argument)
{
    FILE *fp;
    char arg[MAX_INPUT_LENGTH];
    char strsave[MAX_INPUT_LENGTH];
    char *name;
    DESCRIPTOR_DATA *d;
    bool found = FALSE;
    char letter;
    char *word;
	CHAR_DATA *wch;

    one_argument(argument,arg);

    if (arg[0] == '\0')
    {
        send_to_char("You must provide a name.\n\r",ch);
        return;
    }
    name = arg;
    for (d = descriptor_list; d; d = d->next)
    {

        if (d->connected != CON_PLAYING || !can_see(ch,d->character))
            continue;

        wch = ( d->original != NULL ) ? d->original : d->character;
  	
	if ( !can_see(ch,wch) )
 	   continue;

        if (!str_prefix(arg,wch->name))
            found = TRUE;
    }

    if (found)
    {
        send_to_char("That char is playing right now.\n\r",ch);
        return;
    }
    else
    /* begin real whowas stuff */
    {
       /* open file */
       sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( arg ) );
       if ( ( fp = fopen( strsave, "r" ) ) != NULL ) /* open file */
       {
          letter = fread_letter( fp );
          if ( letter == '*' )
             fread_to_eol( fp );
          if ( letter != '#' )
          {
             bug( "Do_whowas: # not found.", 0 );
             return;
          }
          word = fread_word( fp );
/* if not equal to PLAYER, barf cause we are not reading a player
 * file 
 */
          if ( str_cmp( word, "PLAYER" ) ) 
          {
	     bug("Bug in whowas: File opened is not Player file.",0);
             fclose( fp );
    	     return;
 	  }
/* Is a playerfile--- BINGO! */
           whowas(ch,fp);
           fclose(fp);
       } /* end if open file */
       else /* file did not open */
       {
	  send_to_char("There is no player by that name.\n\r",ch);
	  return;
       }
    } /* end else begin real whowas stuff */    
}


void do_pk(CHAR_DATA *ch, char *argument)
{
   char buf2[MAX_STRING_LENGTH];
                 
   if (IS_NPC(ch))
   {
	   act("{RNO, THIS CAN CRASH THE MUD{x",ch,NULL,NULL,TO_ROOM);
	   return;
   }
   if (ch->pcdata->confirm_pk)
   {
        if (argument[0] != '\0')
        {
            send_to_char("PK readiness status removed.\n\r",ch);
            ch->pcdata->confirm_pk = FALSE;
            return;
        }
        else
        {
            if (IS_SET(ch->act,PLR_PROKILLER)) return;
            SET_BIT(ch->act,PLR_PROKILLER);
   
            act("{R$n glows briefly with a red aura, you get the feeling you should keep your distance.{x",ch,NULL,NULL,TO_ROOM);
            send_to_char("{RYou are now a Player Killer, good luck, you'll need it.\n\r{x", ch);
            sprintf(buf2, "%s has become a player killer!", ch->name);
			wiznet(buf2,ch,NULL,0,0,0);
            return;
        }
    }

    if (argument[0] != '\0')
    {
        send_to_char("Just type pk. No argument.\n\r",ch);
        return;
    }
            
    send_to_char("Type pk again to confirm this command.\n\r",ch);
    send_to_char("WARNING: this command is virtually irreversible.\n\r",ch);
    send_to_char("If you don't know what pk is for read help pk, DON'T type this command again.\n\r",ch);
    send_to_char("Typing pk with an argument will undo pk readiness status.\n\r",ch);
    ch->pcdata->confirm_pk = TRUE;
}

/*************************************************************
  File Name:  act_info.c
  Name:       count_color_string
  Purpose:    Returns the string length minus color codes.
  Written By: Snafu Life.
 *************************************************************/
int	count_color_string( char *string )
{
   int i,count=0;
   char arg[MSL];  /* I have MSL defined as MAX_STRING_LENGTH
                     * I am, as many others are, LAZY!!!
                     */

   /* limit the length of string */
   strcpy(arg,string);

   /* Begin the loop through the string */
   for (i=0;i<=MIL;i++) {

	/* An out as soon as the end of the string 
         * is reached, so we don't parse through 
         * the entire MSL, unnessisarily.
         */
	if (arg[i] == '\0') {
		break;
	}

        /* If string[i] isn't '{', then add to count & continue.
         * Placed here to save time as most chars won't be '{'
         */
	else if(string[i] != '{' ) {
		count++;
		continue;
	}

        /* If arg[i] and arg[i+1] is '{', add only one
         * to the count and skip the second '{'
         */
        if (arg[i] == '{' && arg[i+1] == '{' ) {
                i++;
	       	count++;
		continue;
	}

        /* If arg[i] is '{' and arg[i+1] isn't '{' or '\0'
         * Don't add to the count but skip the next char.
         */
        else if(arg[i] == '{' && arg[i+1] != '{' && arg[i+1] != '\0') {
		i += 1;
		continue;
	}
	
	/* If arg[i] is '{' and arg[i+1] is '\0'
	 * stop the procession and don't count the char.
         * 
         */
	else if(arg[i] == '{' && arg[i+1] != '{' && arg[i+1] == '\0') {
		break;
	}
	/* isn't need, but makes me feel better ;) */
	else {
		count++;
		 continue;
	}
   }

   return count;
}


/*
 * NOTE: You might want to add bool check_color_string
 *
 * If you pre-color define your gossips,who, etc. By typing { at 
 * the end you "litter"ally bypass the color.
 * 
 * Example:  gos test
 * Result:   {CYou gossip '{Rtest{C'{x
 * As Shown: You gossip 'test'
 *
 * Example:  gos test{
 * Result:   {CYou gossip '{Rtest{{C'{x
 * As Shown: You gossip 'test{C'
 *
 * Bool check_color returns false if '{' is at the end.
 */

/*************************************************************
  File Name:  act_info.c
  Name:       check_color_string
  Purpose:    Returns true if end of string is '{'
  Written By: Snafu Life.
 *************************************************************/
bool check_color_string(char *string )
{
   int i;

   /* Sanity check for empty strings */
   if(string[0] == '\0') {
	return FALSE;
   }


   /* Start looping through *string */
   for(i=0;i<strlen(string);i++) {
      
       /* When the end is reached, immediately quit
          and return false */
	if(string[i] == '\0') {
		return FALSE;
	}
        /* If string[i] isn't '{', then continue.
         * Placed here to save time as most chars won't be
         */
	else if(string[i] != '{' ) {
		continue;
	}

	/* If string[i] is '{' and string[i+1] isn't '\0'
	 * Skip the next char to save time
	 */
	else if(string[i] == '{' && string[i+1] != '\0') {
	        i++;
		continue;
	}

        /* If string[i] is '{' and string[i+1] is '\0'
         * Whoa, wrong, return true.
         */
	else if(string[i] == '{' && string[i+1] == '\0' ) {
		return TRUE;
	}
	
	/* Isn't needed, but makes me feel better ;)
         */
	else { continue; }
   }

   return FALSE;
}

/*
 * (c) 2001 Taka
 * add/modify a custom slay message
 * 010501
 */
void do_edit_slay(CHAR_DATA *ch, char *argument)
{
    char arg1[MIL];
    char arg2[MIL];
    char arg3[MIL];
 	char buf[MSL];
	char const *sName;
	SLAY_MESSAGE *smIndex;

    argument = one_argument( argument, arg1);
    argument = one_argument( argument, arg2);

	/*
	 * validate command
	 */

    if ( arg1[0] == '\0' )
    {
		send_to_char("{RSyntax: {Geditslay  <slay message name>  <type>  <text>{x\n\r", ch);
		send_to_char("{M        Types = Name, Gerneral, victim, char, room{x\n\r", ch);
		send_to_char("        Name    = Name of the Slay (place the word new in text for a new slay message)\n\r", ch);
		send_to_char("        General = 1=user owned, 0=available all\n\r", ch);
		send_to_char("        Victim  = Victim display message\n\r", ch);
		send_to_char("        Char    = Character diplay message\n\r", ch);
		send_to_char("        Room    = Room display message\n\r", ch);
		return;
	}

	/*
	 * do not allow a mob to use this command
	 * not that it will crash the mud but hey for good measure
	 */
	if ( IS_NPC(ch) )
	{
		send_to_char("Mobs have no need for slay messages!\n\r", ch);
		return;
	}

	/* set the name */
	if(arg2[0] != '\0' && !str_prefix(arg2, "name"))
	{
	    argument = one_argument( argument, arg3);
	
		if(arg3[0] != '\0' && !str_prefix(arg3, "new"))
		{
			/* Ensure slay message does not already exist */
			for(smIndex=SMHead; smIndex != NULL; smIndex=smIndex->SMnext)
			{
				if(!str_cmp( smIndex->Slay_Name, arg1))
				{
					sprintf(buf,"Slay named %s already exists!\n\r", smIndex->Slay_Name);
					stc(buf,ch);
					return;
				}
			}

			/* 
			   Slay message does not exist
			   1) allocate the needed memory for the message
			   2) set the linked list to blanks except the message name
			 */
			smIndex = malloc(sizeof(struct slay_message));

			smIndex->Slay_Number = 0;
			smIndex->Slay_Name   = str_dup(arg1);
			smIndex->Char_Name   = str_dup("");
			smIndex->Show_Vict   = str_dup("");
			smIndex->Show_Char   = str_dup("");
			smIndex->Show_Room   = str_dup("");

			AppendSlayMsg( smIndex );

			sprintf(buf, "New slay named %s has been added.\n\r", 
				smIndex->Slay_Name);
			stc(buf,ch);

			save_slaym();
			free(smIndex);
			return;
		}
		
		/* validate message and rename the slay message */
		for(smIndex=SMHead; smIndex != NULL; smIndex=smIndex->SMnext)
		{
			if(!str_cmp( smIndex->Slay_Name, arg1))
			{
				sprintf(buf,"Slay named %s has been renamed to %s.\n\r", 
					smIndex->Slay_Name, arg1);
				stc(buf,ch);
				smIndex->Slay_Name = str_dup(arg1);
				save_slaym();
				return;
			}
		}

		/* error has occured on the name prompt */
		send_to_char("That is not a slay name!\n\r", ch);
		send_to_char("To make a new one use this.\n\r", ch);
		sprintf(buf,"editslay %s %s new", arg1, arg2);
		stc(buf,ch);
		return;

	}

	/*
	 * general flag is used for user specific slays
	 */
	if(arg2[0] != '\0' && !str_prefix(arg2, "general"))
	{
		int value;
	    argument = one_argument( argument, arg3);

		if ( arg3[0] == '\0' || !is_number(arg3) )
		{
			send_to_char("{RYou must enter a number also!{x\n\r", ch);
			return;
		}

		value = atoi(arg3);

		/* validate message and set general flag */
		for(smIndex=SMHead; smIndex != NULL; smIndex=smIndex->SMnext)
		{
			if(!str_cmp( smIndex->Slay_Name, arg1))
			{
				sprintf(buf,"Slay named %s set as general %d.\n\r", 
					smIndex->Slay_Name, value);
				stc(buf,ch);
				if(!value)
					smIndex->Char_Name = str_dup("");
				else
					smIndex->Char_Name = str_dup(ch->name);
				save_slaym();
				return;
			}
		}

		send_to_char("That is not a slay name!\n\r", ch);
		send_to_char("To make a new one use this.\n\r", ch);
		sprintf(buf,"editslay %s %s new", arg1, arg2);
		stc(buf,ch);
		return;

	}

	/*
	 * check and set show messages for slay commands
	 */
	if((arg2[0] != '\0' && !str_prefix(arg2, "victim"))
		|| (arg2[0] != '\0' && !str_prefix(arg2, "char"))
		|| (arg2[0] != '\0' && !str_prefix(arg2, "room")))
	{
		/* check show value */
		if ( argument[0] == '\0' )
		{
			send_to_char("{RYou must enter a text associated to be seen!{x\n\r", ch);
			return;
		}

		/* find the message in question */
		for(smIndex=SMHead; smIndex != NULL; smIndex=smIndex->SMnext)
		{
			if(!str_cmp( smIndex->Slay_Name, arg1))
			{
				/* victim sees */
				if (!str_cmp(arg2, "victim"))
				{
					sprintf(buf,"victim message set to: %s\n\r", argument);
					stc(buf,ch);
					smIndex->Show_Vict = str_dup(argument);
				}
				/* slayor sees */
				else if (!str_cmp(arg2, "char"))
				{
					sprintf(buf,"char message set to: %s\n\r", argument);
					stc(buf,ch);
					smIndex->Show_Char = str_dup(argument);
				}
				/* room sees */
				else if (!str_cmp(arg2, "room"))
				{
					sprintf(buf,"room message set to: %s\n\r", argument);
					stc(buf,ch);
					smIndex->Show_Room = str_dup(argument);
				}
				else
				{
					/* error */
					send_to_char("Try again!\n\r victim, char or room\n\r", ch);
					return;
				}
				save_slaym();
				return;
			}
		}

		/* error slay name not found */
		send_to_char("That is not a slay name!\n\r", ch);
		send_to_char("To make a new one use this.\n\r", ch);
		sprintf(buf,"editslay %s %s new", arg1, arg2);
		stc(buf,ch);
		return;

	}

	/* list all slay names */
	if(!str_cmp(arg1, "list"))
	{
		for(smIndex=SMHead; smIndex != NULL; smIndex=smIndex->SMnext)
		{
			sName = str_dup(smIndex->Slay_Name);
			strcpy (buf, sName);

			sprintf(buf,"Slay Number: %3d Name: %s Owner: %s\n\r", 
				smIndex->Slay_Number, buf, smIndex->Char_Name);
			stc(buf,ch);
			sprintf(buf,"Vict: %s\n\r", smIndex->Show_Vict);
			stc(buf,ch);
			sprintf(buf,"Char: %s\n\r", smIndex->Show_Char);
			stc(buf,ch);
			sprintf(buf,"Room: %s\n\r", smIndex->Show_Room);
			stc(buf,ch);
			send_to_char("\n\r", ch);
		}
		return;
	}

	/* 
	 * Output a generic error message
	 */
	send_to_char("An error has occured!\n\r", ch);
	return;
}

void do_autotitle(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
    
    if (!IS_SET(ch->plr2,PLR2_NO_TITLE))
    {
      send_to_char("Autotitle {Rremoved{x.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOASSIST);
    }
    else
    {
      send_to_char("Autottitle {Gset{x.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOASSIST);
    }
}

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
#include <winsock.h>
#include <io.h>
#include <direct.h>
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
//#include <process.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "arena.h"
#include "skill_tree.h"

#if defined(unix) || defined(WIN32)
void	game_loop_unix		args( ( int control ) );
int	init_socket		args( ( int port ) );
void	init_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif


void do_copyover (CHAR_DATA *ch, char * argument)
{
	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	char buf [16], buf2[16];
	bool copy = FALSE;

	fp = fopen (COPYOVER_FILE, "w");

	if (!fp)
	{
		send_to_char ("Copyover file not writeable, aborted.\n\r",ch);
		sprintf(buf,"Could not write to copyover file: %s", COPYOVER_FILE);
		bug(buf, 0);
		perror ("do_copyover:fopen");
		return;
	}

	sprintf(buf,"[0;37mThe winds of change howl on, scourging, devouring, and improving.[0m\n\r");
	debug(ch,"do_copyover: EXE_FILE: %s",EXE_FILE);
	if(ch != NULL);
	{
		do_asave (NULL,"world"); /*- autosave changed areas*/
	}


	/* For each playing descriptor, save its state */
	for (d = descriptor_list; d ; d = d_next)
	{
		CHAR_DATA *och = d->character;
		d_next = d->next; /* We delete from the list , so need to save this */

		if (!d->character || d->connected > CON_PLAYING) /* drop those logging on */
		{
			write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
			close_socket (d); /* throw'em out */
		}
		else
		{
			fprintf (fp, "%d %s %s\n", d->descriptor, och->name, d->host);
			save_char_obj(och);
			write_to_descriptor (d->descriptor, buf, 0);
		}
	}
	fprintf (fp, "-1\n");
	fclose (fp);

	/* Close reserve and other always-open files and release other resources */

	fclose (fpReserve);

	/* exec - descriptors are inherited */	

	sprintf (buf, "%d", port);
	sprintf (buf2, "%d", control);

	execl (EXE_FILE, "rom24", buf, "copyover", buf2, (char *) NULL);

	/* Failed - sucessful exec will not return */
	perror ("do_copyover: execl");
	send_to_char ("Copyover FAILED!\n\r",ch);

	/* Here you might want to reopen fpReserve */
	fpReserve = fopen (NULL_FILE, "r");
}

/* Recover from a copyover - load players */
void copyover_recover ()
{
	DESCRIPTOR_DATA *d = NULL;
	FILE *fp;
	char name [128],host[128];
	int desc;
	bool fOld;

	log_string("Copyover recovery initiated");

	fp = fopen (COPYOVER_FILE, "r");

	if (!fp) /* there are some descriptors open which will hang forever then ? */
	{
		perror ("copyover_recover:fopen");
		bug("Copyover file not found. Exitting.\n\r",0);
		exit (1);
	}

	unlink (COPYOVER_FILE); /* In case something crashes - doesn't prevent reading	*/
	control = init_socket(port);
	for (;;)
	{
		fscanf (fp, "%d %s %s\n", &desc, name, host);
		if (desc == -1)
			break;

		d = new_descriptor();
		d->descriptor = desc;

		d->host = str_dup (host);
		d->next = descriptor_list;
		descriptor_list = d;
		d->connected = CON_COPYOVER_RECOVER; /* -15, so close_socket frees the char */


		/* Now, find the pfile */	
		fOld = load_char_obj(d, name);

		stc("{wThe icy wind begins to abate, bringing a hint of spring in the air.{x\n\r",d->character);

		if (!fOld) /* Player file not found?! */
		{
			write_to_descriptor (desc, "\n\rSomehow, your character was lost in the copyover. Sorry.\n\r", 0);
			close_socket (d);			
		}
		else /* ok! */
		{
			stc("\n\r{wSuddenly everything is still, change has come and gone, and worked its will upon the land.{x\n\r",d->character);
			/* Just In Case */
			if (!d->character->in_room)
				d->character->in_room = get_room_index (ROOM_VNUM_TEMPLE);
			/* Insert in the char_list */
			d->character->next = char_list;
			char_list = d->character;
			char_to_room (d->character, d->character->in_room);
			update_bonus(d->character,combat());
			update_bonus(d->character,covert());
			update_bonus(d->character,sorcery());
			update_bonus(d->character,arcane());
			update_bonus(d->character,worship());
			do_look (d->character, "");
			act ("$n materializes!", d->character, NULL, NULL, TO_ROOM);
			d->connected = CON_PLAYING;
		}

	}
	fclose (fp);
}

void do_auto_shutdown()
{
	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	char buf [100], buf2[100];
	extern int port,control;

	fp = fopen (COPYOVER_FILE, "w");   

	if (!fp)
	{
		for (d = descriptor_list;d != NULL;d = d_next)
		{
			if(d->character)
			{
				do_function( d->character, &do_save, "" );
				stc("{wReality wrenches sickeningly around you, as you keel over in nausea.{x\n\r",d->character);
			}

			d_next=d->next;
			close_socket(d);
		}

		exit(1);
	}

	do_function( NULL, &do_asave, "changed" );

	sprintf(buf,"[0;37mJust as darkness begins to win out, a sweet breeze gently blows the evil away.[0m\n\r");

/*	for (d = descriptor_list; d ; d = d_next)
	{
		CHAR_DATA * och = d->character;
		d_next = d->next;

		if (!d->character || d->connected > CON_PLAYING) 
		{
			write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
			close_socket (d);
		}

		else
		{
			fprintf (fp, "%d %s %s\n", d->descriptor, och->name, d->host);
			save_char_obj (och);
			write_to_descriptor (d->descriptor, buf, 0);
		}
	}

	fprintf (fp, "-1\n");
	fclose(fp);
	fclose (fpReserve);
	sprintf (buf, "%d", port);
	sprintf (buf2, "%d", control);
	execl (EXE_FILE, "rom", buf, "copyover", buf2, (char *) NULL);*/
	/* no return from execl */
	exit(1);
}


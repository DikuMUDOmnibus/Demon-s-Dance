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

int compare_mdeaths(const void *v1, const void *v2)
{
    return (*(MOB_INDEX_DATA * *)v2)->killed - (*(MOB_INDEX_DATA * *)v1)->killed;
}

void do_mobdeaths(CHAR_DATA * ch, char *argument)
{
    extern int top_mob_index;
    BUFFER *buffer;
    MOB_INDEX_DATA *pMobIndex;
    MOB_INDEX_DATA *mobs[MAX_KEY_HASH];
 	char buf[MSL];
	int nMatch,count,i;
	bool found;
	long vnum;

	/*copied from quest*/
	MOB_INDEX_DATA *vsearch;
	long mcounter;
	/*end hacknslash*/

    found = FALSE;
    nMatch = 0;
    count = 0;
    i = 0;
    buffer = new_buf();

    add_buf(buffer, "{R                 Demon Dance's Most Popular Mobs                       {x\n\r");
    add_buf(buffer, "{GNum  Mob Name                  Level  Area Name            Deaths{x\n\r");
    add_buf(buffer, "{m---  ------------------------- -----  -------------------- ------{x\n\r");


	for (mcounter = 0; mcounter < 99999; mcounter ++)
	{
		if ( (vsearch = get_mob_index(mcounter) ) != NULL )
		{
			if ((vsearch->killed > 2))
			{
				sprintf(buf,"%3d) %-38s %-5d  %-20s {R%6d{x\n\r", i + 1,
					vsearch->short_descr,
					vsearch->level,
					vsearch->area->name,
					vsearch->killed);
				add_buf(buffer,buf);
				found = TRUE;
				i ++;
			}
		}
	}
	for (vnum = 0; nMatch < MAX_KEY_HASH; vnum++)
    {
		if ((pMobIndex = get_mob_index(vnum)) != NULL)
		{
			nMatch++;

			if (pMobIndex->killed > 2)
			{
			mobs[count] = pMobIndex;
			count++;
			found = TRUE;
			}
		}
    }

    qsort(mobs, count, sizeof(pMobIndex), compare_mdeaths);
	
    for (i = 0; i < count; i++)
    {
	if (i == 49)
	    break;
    }

    if (!found)
	add_buf(buffer, "\n\rNo Mobs listed yet.\n\r");

    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);

    return;
}

int compare_mkills(const void *v1, const void *v2)
{
    return (*(MOB_INDEX_DATA * *)v2)->kills - (*(MOB_INDEX_DATA * *)v1)->kills;
}

void do_mobkills(CHAR_DATA * ch, char *argument)
{
    extern int top_mob_index;
    BUFFER *buffer;
    MOB_INDEX_DATA *pMobIndex;
    MOB_INDEX_DATA *mobs[MAX_KEY_HASH];
 	char buf[MSL];
	int nMatch,count,i;
	bool found;
	long vnum;

	/*copied from quest*/
	MOB_INDEX_DATA *vsearch;
	long mcounter;
	/*end hacknslash*/

    found = FALSE;
    nMatch = 0;
    count = 0;
    i = 0;
    buffer = new_buf();

    add_buf(buffer, "{R                 Demon Dance's Most Dangerous Monsters                 {x\n\r");
    add_buf(buffer, "{GNum  Mob Name                  Level  Area Name            Kills {x\n\r");
    add_buf(buffer, "{m---  ------------------------- -----  -------------------- ------{x\n\r");

		for (mcounter = 0; mcounter < 99999; mcounter ++)
		{
			if ( (vsearch = get_mob_index(mcounter) ) != NULL )
			{
				if (vsearch->kills > 2)
				{
					sprintf(buf,"%3d) %-38.40s %-5d  %-20s {R%6d{x\n\r", i + 1,
						vsearch->short_descr,
						vsearch->level,
						vsearch->area->name,
						vsearch->killed);
					add_buf(buffer,buf);
					found = TRUE;
					i ++;
				}
			}
		}

	for (vnum = 0; nMatch < MAX_KEY_HASH; vnum++)
    {
		if ((pMobIndex = get_mob_index(vnum)) != NULL)
		{
			nMatch++;

			if (pMobIndex->kills > 2)
			{
			mobs[count] = pMobIndex;
			count++;
			found = TRUE;
			}
		}
    }

    qsort(mobs, count, sizeof(pMobIndex), compare_mkills);


    if (!found)
	add_buf(buffer, "\n\rNo Mobs listed yet.\n\r");

    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);

    return;
}


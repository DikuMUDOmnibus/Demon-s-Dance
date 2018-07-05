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

/*Well, I figured I might as well through this up for open source.
 *What it is, is a channel snippet that replaces all globle channels,
 *Unless you have some odd modifications, but those can be easily worked
 *around. This Channel System was writen by Davvol, and myself Davion.
 *it supports emotes, and socials over the channels. It is also simple
 *to add another, just add a the required parts to the table and so on.
 *anyways, have fun with it :)
*/

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

void do_gossip( CHAR_DATA *ch, char *argument)	    
{
	do_newchan( ch, argument, CHAN_GOSSIP );
	return;
}
void do_quote( CHAR_DATA *ch, char *argument)	    
{
	do_newchan( ch, argument, CHAN_QUOTE );
	return;
}
void do_grats( CHAR_DATA *ch, char *argument)	    
{
	do_newchan( ch, argument, CHAN_GRATS );
	return;
}
void do_question( CHAR_DATA *ch, char *argument)	    
{
	do_newchan( ch, argument, CHAN_QA );
	return;
}
void do_music( CHAR_DATA *ch, char *argument)	    
{
	do_newchan( ch, argument, CHAN_MUSIC );
	return;
}
void do_immtalk( CHAR_DATA *ch, char *argument)	    
{
	do_newchan( ch, argument, CHAN_IMM );
	return;
}
void do_clanchat( CHAR_DATA *ch, char *argument)	    
{
	do_newchan( ch, argument, CHAN_CLAN );
	return;
}


void do_newchan( CHAR_DATA *ch, char *argument, int channel)
{
	char arg[MSL];
	char arg2[MSL];
	char arg3[MSL];
	char talk[2*MSL];
	char emote[2*MSL];
	char prefix[MSL];
	char social[MSL];
	char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
	emote[0] = '\0';
	talk[0] = '\0';
	social[0] = '\0';
	if (channel == CHAN_CLAN)
	{
		if (!is_clan(ch))
		{
		send_to_char("You aren't in a clan.\n\r",ch);
		return;
		}
	}

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,chan_table[channel].bit))
      {
		sprintf( buf, "The %s {xchannel is now ON.\n\r", chan_table[channel].chan_pre );
		send_to_char( buf, ch );
		REMOVE_BIT(ch->comm,chan_table[channel].bit);
		return;
      }
      else
      {
		sprintf( buf, "The %s {xchannel is now OFF.\n\r", chan_table[channel].chan_pre );
		send_to_char( buf, ch );
		SET_BIT(ch->comm,chan_table[channel].bit);
		return;
      }
    }
    else  /* message sent, toggle channel on/off */
	{
		if (IS_SET(ch->comm,COMM_QUIET))
		{
		  send_to_char("You must turn off quiet mode first.\n\r",ch);
		  return;
		}

		if (IS_SET(ch->comm,COMM_NOCHANNELS))
		{
		send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
		return;
		}

		if (IS_SET(ch->comm,chan_table[channel].bit))
		{
		send_to_char("Try turning the channel on?\n\r",ch);
		return;
		}
    }
    arg2[0] = '\0';
    strcat(talk, argument);
    one_argument(argument, arg);
    argument = one_argument(argument,arg);
    strcat(emote, argument);
	strcat(social,argument);
    argument = one_argument(argument,arg2);
	argument = one_argument(argument,arg3);

	if((!strcmp(arg, ":"))
		&& (check_chansocial(ch,arg2,arg3,channel)))
	{
		return;
	}
	if (channel == CHAN_CLAN)
	{
		sprintf(prefix,"%s",clan_table[ch->clan].who_name);
	}
	else
	{
		sprintf(prefix,"%s",chan_table[channel].chan_pre);
	}
    if(!strcmp(arg, "emote"))
    {
		argument = one_argument(argument,arg);

		sprintf( buf, "%s {W$n {%s%s{x", 
			prefix,
			chan_table[channel].chan_name,
			emote);  
			act_new(buf,ch,argument,NULL,TO_CHAR,POS_DEAD);

		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->connected == CON_PLAYING 
				&& d->character != ch
				&& !IS_SET(d->character->comm,chan_table[channel].bit)
				&& !IS_SET(d->character->comm,COMM_QUIET) )
				{
					if ((channel == CHAN_IMM) && (d->character->level <= 101))
					{
						continue;
					}
					if (channel == CHAN_CLAN && !is_same_clan(ch,d->character))
					{
						continue;
					}
				act_new(buf, ch, argument, d->character, TO_VICT, POS_DEAD);
				}	
		}
		return;
	}   

	sprintf( buf, "%s {WYou {%ssay {W'{%s%s{%W'{x\n\r", 
		prefix,
		chan_table[channel].chan_name,
		chan_table[channel].chan_say,
		talk,
		chan_table[channel].chan_name);
    send_to_char( buf, ch );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
		if ( d->connected == CON_PLAYING &&
			 d->character != ch &&
			 !IS_SET(d->character->comm,chan_table[channel].bit) &&
			 !IS_SET(d->character->comm,COMM_QUIET) )
		{
			if ((channel == CHAN_IMM) && (d->character->level <= 101))
			{
				continue;
			}
			if (channel == CHAN_CLAN && !is_same_clan(ch,d->character))
			{
				continue;
			}
			sprintf( buf, "%s {W$n {%ssays '{%s%s{%s'{x", 
				prefix,
				chan_table[channel].chan_name,
				chan_table[channel].chan_say,
				talk,
				chan_table[channel].chan_name);
			act_new(buf, ch, argument, d->character, TO_VICT, POS_DEAD);
 		}
    }
}

bool check_chansocial( CHAR_DATA *ch, char *command, char *argument, int channel )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;
    char buf[MSL];
    int counter;
    int count;
    char buf2[MSL];        
    char buf3[MSL];
	char prefix[MSL];
    DESCRIPTOR_DATA *d;
    buf[0] = '\0';
    buf2[0] = '\0';
    buf3[0] = '\0';
    found  = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == social_table[cmd].name[0]
	&&   !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
	return FALSE;

    one_argument( argument, arg );
    victim = NULL;

	if (channel == CHAN_CLAN)
	{
		sprintf(prefix,"%s",clan_table[ch->clan].who_name);
	}
	else
	{
		sprintf(prefix,"%s",chan_table[channel].chan_pre);
	}
    if ( arg[0] == '\0' )
    {
      sprintf( buf, "%s {%s{%s%s{%s{x",
		prefix,
		chan_table[channel].chan_name,
		chan_table[channel].chan_say,
		social_table[cmd].others_no_arg,
		chan_table[channel].chan_name);
	for (d = descriptor_list; d != NULL; d = d->next)
	{
			CHAR_DATA *vch;
		vch = d->original ? d->original : d->character;
		if (d->connected == CON_PLAYING 
			&& d->character != ch
			&& !IS_SET(vch->comm,chan_table[channel].bit)
			&& !IS_SET(vch->comm,COMM_QUIET))
	    {
			if ((channel == CHAN_IMM) && (d->character->level <= 101))
			{
				continue;
			}
			if (channel == CHAN_CLAN && !is_same_clan(ch,d->character))
			{
				continue;
			}
			act_new(buf,ch,NULL,vch,TO_VICT,POS_DEAD);
		}
	}

	sprintf( buf2, "%s {%s{%s%s{%s{x\n\r",
		prefix,
		chan_table[channel].chan_name,
		chan_table[channel].chan_say,
		social_table[cmd].char_no_arg,
		chan_table[channel].chan_name);
	send_to_char(buf2,ch);	
    }
    else if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
		return FALSE;
    }	
    else if(IS_SET(victim->comm,chan_table[channel].bit))
	{
		send_to_char("They cannot see this channel, therefor you cannot do that to them.\n\r",ch);
		return TRUE;
	}
	else if ( victim == ch )
	{
		sprintf( buf, "%s {%s{%s%s{%s{x",
			prefix,
			chan_table[channel].chan_name,
			chan_table[channel].chan_say,
			social_table[cmd].others_auto,
			chan_table[channel].chan_name);

		sprintf( buf2, "%s {%s{%s%s{%s{x\n\r",
			prefix,
			chan_table[channel].chan_name,
			chan_table[channel].chan_say,
			social_table[cmd].char_auto,
			chan_table[channel].chan_name);	
		send_to_char(buf2,ch);

		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			CHAR_DATA *vch;
			vch = d->original ? d->original : d->character;
 
			if ( d->connected == CON_PLAYING
				&& d->character != ch 
				&& victim->name != arg
				&& !IS_SET(vch->comm,chan_table[channel].bit)
				&& !IS_SET(vch->comm,COMM_QUIET) )
			{
				if ((channel == CHAN_IMM) && (d->character->level <= 101))
				{
					continue;
				}
				if (channel == CHAN_CLAN && !is_same_clan(ch,d->character))
				{
					continue;
				}
				act_new(buf,ch,NULL,vch,TO_VICT,POS_DEAD);
			}
		}
    }
    else
    {
      sprintf( buf, "%s {%s{%s%s{%s{x",
		prefix,
		chan_table[channel].chan_name,
		chan_table[channel].chan_say,
		social_table[cmd].others_found,
		chan_table[channel].chan_name);

      sprintf( buf2, "%s {%s{%s%s{%s{x",
		prefix,
		chan_table[channel].chan_name,
		chan_table[channel].chan_say,
		social_table[cmd].char_found,
		chan_table[channel].chan_name);
	  act_new(buf2,ch,NULL,victim,TO_CHAR,POS_DEAD);

      sprintf( buf3, "%s {%s{%s%s{%s{x",
		prefix,
		chan_table[channel].chan_name,
		chan_table[channel].chan_say,
		social_table[cmd].vict_found,
		chan_table[channel].chan_name);
	  act_new(buf3,ch,NULL,victim,TO_VICT,POS_DEAD);

        for (counter = 0; buf[counter+1] != '\0'; counter++)
        {
            if (buf[counter] == '$' && buf[counter + 1] == 'N')
            {
                strcpy(buf2,buf);
                buf2[counter] = '\0';
                strcat(buf2,victim->name);
                for (count = 0; buf[count] != '\0'; count++)
                {
                    buf[count] = buf[count+counter+2];
                }
                strcat(buf2,buf);
                strcpy(buf,buf2);

            }
            else if (buf[counter] == '$' && buf[counter + 1] == 'E')
            {
                switch (victim->sex)
                {
                default:
                    strcpy(buf2,buf);
                    buf2[counter] = '\0';
                    strcat(buf2,"it");
                    for (count = 0; buf[count] != '\0'; count ++)
                    {
                        buf[count] = buf[count+counter+2];
                    }
                    strcat(buf2,buf);
                    strcpy(buf,buf2);
                    break;
                case 1:
                    strcpy(buf2,buf);
                    buf2[counter] = '\0';
                    strcat(buf2,"it");
                    for (count = 0; buf[count] != '\0'; count++)
                    {
                        buf[count] = buf[count+counter+2];
                    }
                    strcat(buf2,buf);
                    strcpy(buf,buf2);
                    break;
                case 2:
                    strcpy(buf2,buf);
                    buf2[counter] = '\0';
                    strcat(buf2,"it");
                    for (count = 0; buf[count] != '\0'; count++)
                    {
                        buf[count] = buf[count+counter+2];
                    }
                    strcat(buf2,buf);
                    strcpy(buf,buf2);
                    break;
                }
            }
            else if (buf[counter] == '$' && buf[counter + 1] == 'M')
            {
                buf[counter] = '%';
                buf[counter + 1] = 's';
                switch (victim->sex)
                {
                default:
                    strcpy(buf2,buf);
                    buf2[counter] = '\0';
                    strcat(buf2,"it");
                    for (count = 0; buf[count] != '\0'; count++)
                    {
                        buf[count] = buf[count+counter+2];
                    }
                    strcat(buf2,buf);
                    strcpy(buf,buf2);
                    break;
                case 1:
                    strcpy(buf2,buf);
                    buf2[counter] = '\0';
                    strcat(buf2,"him");
                    for (count = 0; buf[count] != '\0'; count++)
                    {
                        buf[count] = buf[count+counter+2];
                    }
                    strcat(buf2,buf);
                    strcpy(buf,buf2);
                    break;
                case 2:
                    strcpy(buf2,buf);
                    buf2[counter] = '\0';
                    strcat(buf2,"her");
                    for (count = 0; buf[count] != '\0'; count++);
                    {
                        buf[count] = buf[count+counter+2];
                    }
                    strcat(buf2,buf);
                    strcpy(buf,buf2);
                    break;
                }
            }
            else if (buf[counter] == '$' && buf[counter + 1] == 'S')
            {
                switch (victim->sex)
                {
                default:
                strcpy(buf2,buf);
                buf2[counter] = '\0';
                strcat(buf2,"its");
                for (count = 0;buf[count] != '\0'; count++)
                {
                    buf[count] = buf[count+counter+2];
                }
                strcat(buf2,buf);
                strcpy(buf,buf2);
                break;
                case 1:
                    strcpy(buf2,buf);
                    buf2[counter] = '\0';
                    strcat(buf2,"his");
                    for (count = 0; buf[count] != '\0'; count++)
                    {
                        buf[count] = buf[count+counter+2];
                    }
                    strcat(buf2,buf);
                    strcpy(buf,buf2);
                    break;
                case 2:
                    strcpy(buf2,buf);
                    buf2[counter] = '\0';
                    strcat(buf2,"hers");
                    for (count = 0; buf[count] != '\0'; count++)
                    {
                        buf[count] = buf[count+counter+2];
                    }
                    strcat(buf2,buf);
                    strcpy(buf,buf2);
                    break;
                }
            }

        }

      for ( d = descriptor_list; d != NULL; d = d->next )
      {
	  CHAR_DATA *vch;
        vch = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
			  victim != vch &&
             !IS_SET(vch->comm,chan_table[channel].bit) &&
             !IS_SET(vch->comm,COMM_QUIET) )
        {
			if ((channel == CHAN_IMM) && (d->character->level <= 101))
			{
				continue;
			}
			if (channel == CHAN_CLAN && !is_same_clan(ch,d->character))
			{
				continue;
			}
			act_new(buf,ch,NULL,vch,TO_VICT,POS_DEAD);
        }
      }	
    }

    return TRUE;
}

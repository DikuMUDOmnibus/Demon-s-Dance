/***************************************************************************
 * Keyring [C] 2002 Petr [Dingo] Dvorak <dingo@texoma.net>                 *
 *                                                                         *
 * Written for Anime Planet MUD [animeplanet.genesismuds.com:3333]         *
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
#include "olc.h"
#include "slay.h"
#include "execute.h"

/*some state definitions so we can handle all the states with one function
instead repeating the same stuff over and over.*/
#define KR_OK				0
#define KR_NO_KEYRING		1
#define KR_NO_KEY			2
#define KR_NO_REMKEY		3
#define KR_INVALID_KEYRING	4
#define KR_INVALID_KEY		5
#define KR_INVALID_NAME		6
#define KR_KEY_DUP			7

DECLARE_DO_FUN( do_examine );

/*
 This snippet adds keyring command and ability into the mud, the syntax is 
 simple:
 
 keyring 		- examine the keyring and lists all keys.
 keyring add <key>	- add key to the keyring.
 keyring remove <key>	- remove key from keyring.

 when player adds a key on keyring, the physical key is extracted, when
 player removes key from keyring, the physical key is created and inserted
 in their inventory. There is not that much to this snippet so write your
 own help for it.
 
 I didn't want to add another object type just for something simple like
 this, so the code looks for name prefix 'keyring' in the pObjIndex->name.
*/

/*allocates memory for new key on the keyring*/
KEY_DATA * new_key( void )
{
	KEY_DATA * key;

	/*allocate memory for the key, calloc 0's the allocated memory so we don't
	have to initialize the variables.*/
	key = alloc_perm( sizeof(*key) );
	VALIDATE(key);

	return key;
}

/*frees memory allocated by new_key*/
void free_key( KEY_DATA * key )
{
	if (!key || !IS_VALID(key))	
		return;

	key->next  = NULL;
	key->index = NULL;
	INVALIDATE(key);

	return;
}

/* adds key to the keyring*/
int add_key( OBJ_DATA * keyring, OBJ_DATA * key)
{
	KEY_DATA * nkey;

	if (!keyring)		return KR_NO_KEYRING;
	if (!key)			return KR_NO_KEY;
	if (!IS_VALID(keyring))	return KR_INVALID_KEYRING;
	if (!IS_VALID(key))		return KR_INVALID_KEY;

	for ( nkey = keyring->keys; nkey != NULL; nkey = nkey->next )
		if (nkey->index == key->pIndexData)
			return KR_KEY_DUP;

	nkey = new_key();

	nkey->index = key->pIndexData;
	nkey->next = keyring->keys;
	keyring->keys = nkey;
	obj_from_char(key);
	extract_obj(key);

	return KR_OK;
}

/*removes key from the keyring*/
int remove_key( CHAR_DATA * ch, OBJ_DATA * keyring, char * name )
{
	KEY_DATA	* key;
	KEY_DATA	* key_next;

	if (!keyring)		return KR_NO_KEYRING;
	if (!keyring->keys)		return KR_NO_KEY;
	if (!name)			return KR_INVALID_NAME;
	if (name[0] == '\0')	return KR_INVALID_NAME;

	for ( key = keyring->keys; key != NULL; key = key_next )
	{
		key_next = key->next;

		if (key && key->index->name && !str_prefix(name, key->index->name))
		{
			OBJ_DATA *phys_key;

			phys_key = create_object(key->index,key->index->level);
			obj_to_char(phys_key, ch);

			if ( keyring->keys == key )	keyring->keys = key->next;
			else
			{
				KEY_DATA *prev;

				for ( prev = keyring->keys; prev != NULL; prev = prev->next )
				{
					if ( prev->next == key )
					{
						prev->next = key->next;
						break;
					}
				}
			}
			free_key(key);
			return KR_OK;
		}
	}	
	return KR_NO_REMKEY;
}

void save_keyring( FILE * fp, OBJ_DATA * obj)
{
	KEY_DATA *  keys;

	if (!obj || !obj->keys)
		return;

	fprintf(fp, "Keyring  ");

	for (keys = obj->keys; keys != NULL; keys = keys->next)
	{
		if ( keys->index->vnum )
			fprintf(fp, "%d ", keys->index->vnum);
	}

	fprintf(fp,"-1\n\r");

	return;
}

/*load keys from the pfile*/
void load_keyring( FILE * fp, OBJ_DATA * obj)
{
	int 	vnum = 0;
	KEY_DATA *  nkey;

	do
	{
		vnum = fread_number(fp);
		if (vnum > 0)
		{
			nkey = new_key();
			nkey->index = get_obj_index(vnum);
			nkey->next = obj->keys;
			obj->keys = nkey;
		}
	} 
	while (vnum > 0);

	return;
}

/*lists the keys on the keyring*/
void list_keys( CHAR_DATA * ch, OBJ_DATA * keyring )
{
	KEY_DATA	* key;
	char 	  buf[MSL];
	char buf2[MSL];

	if ( !keyring->keys )
	{
		send_to_char("The keyring is empty.\n\r",ch);
		return;
	}

	send_to_char("\n\rThe keyring contains the following keys:\n\r\n\r",ch);

	for ( key = keyring->keys; key != NULL; key = key->next)
	{
		buf[0] = '\0';

		if (IS_IMMORTAL(ch)) sprintf(buf,"{D[{C%5.5d{D]{x",key->index->vnum);

		sprintf(buf2,"    %s %s from %s.\n\r", 
			buf,
			key->index->short_descr ? key->index->short_descr : "Unknown",
			key->index->area->name ? key->index->area->name : "Unknown place"
			);
		stc(buf2,ch);

	}

	return;
}

/*searches players inventory for keyring*/
OBJ_DATA * find_keyring( CHAR_DATA * ch )
{
	int iWear;
	OBJ_DATA *obj,*obj_next;

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
		return obj;
	}
	for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
	{
		if ( ( obj = get_eq_char( ch, iWear ) ) != NULL )
		{
			return obj;
		}
	}
	return NULL;
}

void keyring_message(int message, CHAR_DATA * ch, OBJ_DATA * key)
{
	char buf[MSL];
	switch (message)
	{
	case KR_OK:
		return;
		break;
	case KR_NO_KEYRING:
		send_to_char("You don't have a keyring, go buy one.\n\r",ch);
		break;
	case KR_NO_KEY:
		send_to_char("You don't have that key.\n\r",ch);
		break;
	case KR_NO_REMKEY:
		send_to_char("You don't have that key on your keyring.\n\r",ch);
		break;
	case KR_KEY_DUP:
		sprintf(buf, "You already have %s on your keyring.\n\r",key->short_descr);
		stc(buf,ch);
		break;
	case KR_INVALID_NAME:
	case KR_INVALID_KEYRING:
	case KR_INVALID_KEY:
	default:
		send_to_char("Something gone horribly wrong, please tell the nearest immortal.\n\r",ch);
		break;
	}

	return;
}

/*the command function*/
void do_keyring( CHAR_DATA * ch, char *argument )
{
	OBJ_DATA *  keyring = NULL;
	OBJ_DATA *  key = NULL;
	char 	arg1[MIL];
	char 	arg2[MIL];
	int		message = KR_OK;

	keyring = find_keyring(ch);

	if (!keyring)
	{
		keyring_message(KR_NO_KEYRING,ch,NULL);
		return;
	}

	if (argument[0] == '\0')
	{
		do_examine(ch, keyring->name);
		return;
	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if (arg2[0] == '\0' )
	{
		send_to_char("You must specify the name of the key you wish to add or remove.\n\r", ch);
		return;
	}

	if (is_name( arg1, "add") )
	{
		key = get_obj_carry( ch, arg2, ch );

		if (!key)
		{
			send_to_char("You don't have that key.\n\r", ch);
			return;
		}

		if (key->item_type != ITEM_KEY)
		{
			send_to_char("That is not a key!\n\r", ch);
			return;
		}

		message = add_key(keyring, key);

		if (!message)
		{
			act("$n adds $p on $s keyring.", ch, key, NULL, TO_ROOM);
			act("You added $p on your keyring.", ch, key, NULL, TO_CHAR);
		} else
		{
			keyring_message(message,ch,key);
		}
	}
	else if (is_name( arg1, "remove" ) )
	{
		message = remove_key(ch, keyring, arg2);
		if (!message)
		{
			act("$n removes a key from $s keyring.", ch, NULL, NULL, TO_ROOM);
			act("You removed a key from your keyring.", ch, NULL, NULL, TO_CHAR);
		} else
		{
			keyring_message(message,ch,NULL);
		}
	}
	else
	{
		send_to_char("Invalid argument, see 'help keyring' for info\n\r", ch);
		return;
	}

	return;
}

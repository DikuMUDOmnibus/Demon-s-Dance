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

/*
 * Class table.
 */
struct	class_type	class_table	[MAX_CLASS] =
{
    {
	"mage", "Mag",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3018, 9618 },  75,  20, 6,  6,  8, TRUE, 8, 8,
	"mage basics", "mage default", FALSE,
	{3,	2,	6,	7,	4}
    },

    {
	"cleric", "Cle",  STAT_WIS,  OBJ_VNUM_SCHOOL_MACE,
	{ 3003, 9619 },  75,  20, 2,  7, 10, TRUE,8, 8,
	"cleric basics", "cleric default", FALSE,
	{4,	2,	4,	5,	7}
    },

    {
	"thief", "Thi",  STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3028, 9639 },  75,  20,  -4,  8, 13, FALSE,8, 8,
	"thief basics", "thief default", FALSE,
	{4,	7,	3,	2,	2}
    },

    {
	"warrior", "War",  STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
	{ 3022, 9633 },  75,  20,  -10,  11, 15, FALSE,8, 8,
	"warrior basics", "warrior default", FALSE,
	{8,	3,	3,	2,	2}
    },

	{
	"legionare", "Leg",	STAT_STR, OBJ_VNUM_SCHOOL_SWORD,
	{ 3022, 9633 }, 80, 60, 10, 11, 15, TRUE,8, 8,
	"legionare basics", "legionare default", TRUE,
	{10,	3,	4,	3,	4}
	},

	{
	"draconian", "Drc",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3018, 9618 },  80,  50, 10,  11,  15, TRUE,8, 8,
	"draconian basics", "draconian default", TRUE,
	{6,	2,	10,	8,	5}
    },

	{
	"sensei", "Sen",  STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
	{ 3022, 9633 },  80,  50, 10,  11,  15, TRUE,8, 8,
	"sensei basics", "sensei default", TRUE,
	{7,	6,	3,	3,	3}
    },
	
	{
	"zealot", "Zlt",  STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
	{ 3022, 9633 },  80,  50, 10,  11,  15, TRUE,8, 8,
	"zealot basics", "zealot default", TRUE,
	{6,	2,	6,	4,	10}
    },

	{
	"taomaster", "Tao",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3018, 9618 },  80,  50, 10,  11,  15, TRUE,8, 8,
	"tao basics", "tao default", TRUE,
	{5,	3,	8,	10,	6}
    },
	
	{
	"dagashi", "Dag",  STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3028, 9639 },  80,  50,  10,  11, 15, TRUE,8, 8,
	"dagashi basics", "dagashi default", TRUE,
	{7,	10,	3,	3,	2}
    },

    {
	"esper", "Esp",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3018, 9618 },  75,  20, 6,  6,  8, TRUE,8, 8,
	"esper basics", "esper default", FALSE,
	{3,	2,	7,	6,	4}
    },

};



/*
 * Titles.
 */
const	struct	title_type	title_table	[MAX_CLASS * 12]	=
{
	/*class_num,level,
	"title_male","title_female"*/
	{ 0, 1,		"Apprentice of Magic",		"Apprentice of Magic"		},
	{ 0, 10,	"Scholar of Magic",			"Scholar of Magic"			},
	{ 0, 20,	"Enchanter",				"Enchantress"				},
	{ 0, 30,	"Magician",					"Witch"						},
	{ 0, 40,	"Warlock",					"War Witch"					},
	{ 0, 50,	"Sorcerer",					"Sorceress"					},
	{ 0, 60,	"Great Sorcerer",			"Great Sorceress"			},
	{ 0, 70,	"Golem Maker",				"Golem Maker"				},
	{ 0, 80,	"Demon Summoner",			"Demon Summoner"			},
	{ 0, 90,	"Dragon Charmer",			"Dragon Charmer"			},
	{ 0, 100,	"Mage Hero",				"Mage Heroine"				},
	{ 0, 101,	"Avatar of Magic",			"Avatar of Magic"			},

	{ 1, 1,		"Man",						"Woman"						},
	{ 1, 10,	"Acolyte",					"Acolyte"					},
	{ 1, 20,	"Deacon",					"Deaconess"					},
	{ 1, 30,	"Priest",					"Priestess"					},
	{ 1, 40,	"Expositor",				"Expositress"				},
	{ 1, 50,	"Bishop",					"Bishop"					},
	{ 1, 60,	"Arch Bishop",				"Arch Lady of the Church"	},
	{ 1, 70,	"Patriarch",				"Matriarch"					},
	{ 1, 80,	"Great Patriarch",			"Great Matriarch"			},
	{ 1, 90,	"Demon Killer",				"Demon Killer"				},
	{ 1, 100,	"Holy Hero",				"Holy Heroine"				},
	{ 1, 101,	"Holy Avatar",				"Holy Avatar"				},

	{ 2, 1,		"Man",						"Woman"						},
	{ 2, 10,	"Footpad",					"Footpad"					},
	{ 2, 20,	"Filcher",					"Filcheress"				},
	{ 2, 30,	"Robber",					"Robber"					},
	{ 2, 40,	"Killer",					"Murderess"					},
	{ 2, 50,	"Cut-Throat",				"Cut-Throat"				},
	{ 2, 60,	"Master Spy",				"Master Spy"				},
	{ 2, 70,	"Assassin",					"Assassin"					},
	{ 2, 80,	"Crime Lord",				"Crime Mistress"			},
	{ 2, 90,	"Godfather",				"Godmother"					},
	{ 2, 100,	"Assassin Hero",			"Assassin Heroine"			},
	{ 2, 101,	"Angel of Death",			"Angel of Death"			},

	{ 3, 1,		"Man",						"Woman"						},
	{ 3, 10,	"Swordpupil",				"Swordpupil"				},
	{ 3, 20,	"Soldier",					"Soldier"					},
	{ 3, 30,	"Veteran",					"Veteran"					},
	{ 3, 40,	"Hero",						"Heroine"					},
	{ 3, 50,	"Dragoon",					"Lady Dragoon"				},
	{ 3, 60,	"Cavalier",					"Lady Cavalier"				},
	{ 3, 70,	"Paladin",					"Paladin"					},
	{ 3, 80,	"Demon Slayer",				"Demon Slayer"				},
	{ 3, 90,	"Dragon Slayer",			"Dragon Slayer"				},
	{ 3, 100,	"Knight Hero",				"Knight Heroine"			},
	{ 3, 101,	"Angel of War",				"Angel of War"				},

	{ 4, 1,		"Man",						"Woman"						},
	{ 4, 10,	"Private",					"Private"					},/*Enzo*/
	{ 4, 20,	"Decurion",					"Decurion"					},/*Enzo*/
	{ 4, 30,	"Centurion",				"Centurion"					},/*Enzo*/
	{ 4, 40,	"Tribune",					"Tribune"					},/*Enzo*/
	{ 4, 50,	"Primus",					"Primus"					},/*Enzo*/
	{ 4, 60,	"Consul",					"Consul"					},/*Enzo*/
	{ 4, 70,	"Praetorian",				"Praetorian"				},/*Stareye*/
	{ 4, 80,	"Legate",					"Legate"					},/*Enzo*/
	{ 4, 90,	"General",					"General"					},/*Enzo*/
	{ 4, 100,	"Imperator",				"Imperator"					},/*RumRat*/
	{ 4, 101,	"Emperor",					"Emperess"					},/*Enzo*/

	{ 5, 1,		"Man",						"Woman"						},
	{ 5, 10,	"Lizard",					"Lizard"					},/*Silza*/
	{ 5, 20,	"Basilisk",					"Basilisk"					},/*Silza*/
	{ 5, 30,	"Fledgling Dragon",			"Fledgling Dragon"			},/*CladoR*/
	{ 5, 40,	"Adept of the Blue Dragon",	"Adept of the Blue Dragon"	},/*Raithias*/
	{ 5, 50,	"Adept of the Green Dragon","Adept of the Green Dragon"	},/*Raithias*/
	{ 5, 60,	"Adept of the Yellow Dragon","Adept of the Yellow Dragon"},/*Raithias*/
	{ 5, 70,	"Adept of the Red Dragon",	"Adept of the Red Dragon"	},/*Raithias*/
	{ 5, 80,	"Adept of the Black Dragon","Adept of the Black Dragon"	},/*Raithias*/
	{ 5, 90,	"Keeper of the Flame",		"Keeper of the Flame"		},/*Enzo*/
	{ 5, 100,	"Dragon Lord",				"Dragon Lady"				},/*Raithais*/
	{ 5, 101,	"Dragon Master",			"Dragon Mistress"			},/*CladoR*/

	{ 6, 1,		"Man",						"Woman"						},
	{ 6, 10,	"Grasshopper",				"Grasshopper"				},
	{ 6, 20,	"White Belt",				"White Belt"				},
	{ 6, 30,	"Yellow Belt",				"Yellow Belt"				},
	{ 6, 40,	"Green Belt",				"Green Belt"				},
	{ 6, 50,	"Blue Belt",				"Blue Belt"					},
	{ 6, 60,	"Brown Belt",				"Brown Belt"				},
	{ 6, 70,	"Red Belt",					"Red Belt"					},
	{ 6, 70,	"Black Belt",				"Black Belt"				},
	{ 6, 90,	"Samurai",					"Samurai"					},/*Enzo*/
	{ 6, 100,	"Shogun",					"Shogun"					},
	{ 6, 101,	"Kung Fu Master",			"Kung Fu Mistress"			},


	{ 7, 1,		"Man",						"Woman"						},
	{ 7, 10,	"Fanatic",					"Fanatic"					},
	{ 7, 20,	"Maniac",					"Maniac"					},
	{ 7, 30,	"Lunatic",					"Lunatic"					},
	{ 7, 40,	"Devotee",					"Devotee"					},
	{ 7, 50,	"Votary",					"Votary"					},
	{ 7, 60,	"Visionary",				"Visionary"					},
	{ 7, 70,	"Prophet",					"Prophet"					},
	{ 7, 80,	"Self-Appointed Messenger",	"Self-Appointed Messenger"	},
	{ 7, 90,	"Diviner",					"Diviner"					},
	{ 7, 100,	"Archangel",				"Archangel"					},/*Stareye*/
	{ 7, 101,	"Messiah",					"Messiah"					},

	{ 8, 1,		"Man",						"Woman"						},
	{ 8, 10,	"Neophyte",					"Neophyte"					},/*Stareye*/
	{ 8, 20,	"Sycophant",				"Sycophant"					},
	{ 8, 30,	"Apprentice Magi",			"Apprentice Magi"			},
	{ 8, 40,	"Apprentice Spellweaver",	"Apprentice Spellweaver"	},
	{ 8, 50,	"Spellweaver",				"Spellweaver"				},
	{ 8, 60,	"Magi",						"Magi"						},
	{ 8, 70,	"Arbiter of the Elements",	"Arbiter of the Elements"	},/*Stareye*/
	{ 8, 80,	"Mindmaster",				"Mindmistress"				},
	{ 8, 90,	"Mindmelder",				"Mindmelder"				},
	{ 8, 100,	"Mindflayer",				"Mindflayer"				},
	{ 8, 101,	"Psychic Apocolypse",		"Psychic Apocolypse"		},		

	{ 9, 1,		"Man",						"Woman"						},
	{ 9, 10,	"Shadow Walker",			"Shadow Walker"				},/*Raithias*/
	{ 9, 20,	"Shadowdancer",				"Shadowdancer"				},/*Raina*/
	{ 9, 30,	"Soul Piercer",				"Soul Piercer"				},/*Raithias*/
	{ 9, 40,	"Torn Soul",				"Torn Soul"					},/*Raithias*/
	{ 9, 50,	"Slayer",					"Slayeress"					},/*Stareye*/
	{ 9, 60,	"Shadowblade",				"Shadowblade"				},/*Stareye*/
	{ 9, 70,	"Herald of Death",			"Herald of Death",			},/*Raithias*/
	{ 9, 80,	"Ripper",					"Ripper"					},
	{ 9, 90,	"Blademaster",				"Blademaster"				},
	{ 9, 100,	"Hidden Death",				"Hidden Death"				},
	{ 9, 101,	"Invisible One",			"Invisible One"				},

	{ 10, 1,	"Man",						"Woman"						},/*Silza*/
	{ 10, 10,	"Ghost",					"Ghost"						},/*Silza*/
	{ 10, 20,	"Specter",					"Specter"					},/*Silza*/
	{ 10, 30,	"Phantasm",					"Phantasm"					},/*Silza*/
	{ 10, 40,	"Wraith",					"Wraith"					},/*Silza*/
	{ 10, 50,	"Banshee",					"Banshee"					},/*Silza*/
	{ 10, 60,	"Revenant",					"Revenant"					},/*Silza*/
	{ 10, 70,	"Poltergeist",				"Poltergeist"				},
	{ 10, 80,	"Telekenetic",				"Telekenetic"				},
	{ 10, 90,	"Siren",					"Siren"						},
	{ 10, 100,	"Mind Scourge",				"Mind Scourge"				},
	{ 10, 101,	"Mind Leech",				"Mind Leech"				}
};


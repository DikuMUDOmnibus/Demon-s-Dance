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
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"
#include "execute.h"


/* item type list */
const struct item_type		item_table	[]	=
{
    {	ITEM_LIGHT,	"light"		},
    {	ITEM_SCROLL,	"scroll"	},
    {	ITEM_WAND,	"wand"		},
    {   ITEM_STAFF,	"staff"		},
    {   ITEM_WEAPON,	"weapon"	},
    {   ITEM_TREASURE,	"treasure"	},
    {   ITEM_ARMOR,	"armor"		},
    {	ITEM_POTION,	"potion"	},
    {	ITEM_CLOTHING,	"clothing"	},
    {   ITEM_FURNITURE,	"furniture"	},
    {	ITEM_TRASH,	"trash"		},
    {	ITEM_CONTAINER,	"container"	},
    {	ITEM_DRINK_CON, "drink"		},
    {	ITEM_KEY,	"key"		},
    {	ITEM_FOOD,	"food"		},
    {	ITEM_MONEY,	"money"		},
    {	ITEM_BOAT,	"boat"		},
    {	ITEM_CORPSE_NPC,"npc_corpse"	},
    {	ITEM_CORPSE_PC,	"pc_corpse"	},
    {   ITEM_FOUNTAIN,	"fountain"	},
    {	ITEM_PILL,	"pill"		},
    {	ITEM_PROTECT,	"protect"	},
    {	ITEM_MAP,	"map"		},
    {	ITEM_PORTAL,	"portal"	},
    {	ITEM_WARP_STONE,"warp_stone"	},
    {	ITEM_ROOM_KEY,	"room_key"	},
    {	ITEM_GEM,	"gem"		},
    {	ITEM_JEWELRY,	"jewelry"	},
    {   ITEM_JUKEBOX,	"jukebox"	},
	{	ITEM_QUIVER,	"quiver"	},
	{	ITEM_ARROW,		"arrow",	},
	{	ITEM_FORT,		"fort",		},
	{	ITEM_TOKEN,		"token",	},
	{	ITEM_GOLEM_BAG,	"golem_bag"	},
	{	ITEM_GOLEM_PART,"golem_part"},
	{	ITEM_SMITH_FURNACE,	"smith_furnace"	},
	{	ITEM_SMITH_HAMMER,	"smith_hammer"	},
	{	ITEM_SMITH_BARREL,	"smith_barrel"	},
	{	ITEM_SMITH_GRINDER,	"smith_grinder"	},
	{	ITEM_SMITH_ANVIL,	"smith_anvil"	},
	{	ITEM_SMITH_ORE,		"smith_ore"	},
	{	ITEM_SHEATH,		"sheath"	},

    {   0,		NULL		}
};


/* weapon selection table */
const	struct	weapon_type	weapon_table	[]	=
{
   { "sword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_SWORD,	&gsn_sword	},
   { "mace",	OBJ_VNUM_SCHOOL_MACE,	WEAPON_MACE,	&gsn_mace 	},
   { "dagger",	OBJ_VNUM_SCHOOL_DAGGER,	WEAPON_DAGGER,	&gsn_dagger	},
   { "axe",	OBJ_VNUM_SCHOOL_AXE,	WEAPON_AXE,	&gsn_axe	},
   { "staff",	OBJ_VNUM_SCHOOL_STAFF,	WEAPON_SPEAR,	&gsn_spear	},
   { "flail",	OBJ_VNUM_SCHOOL_FLAIL,	WEAPON_FLAIL,	&gsn_flail	},
   { "whip",	OBJ_VNUM_SCHOOL_WHIP,	WEAPON_WHIP,	&gsn_whip	},
   { "polearm",	OBJ_VNUM_SCHOOL_POLEARM,WEAPON_POLEARM,	&gsn_polearm	},
   { "bow",		OBJ_VNUM_SCHOOL_SWORD,	WEAPON_BOW,		&gsn_bow		},
   { NULL,	0,				0,	NULL		}
};

const	struct	golem_type	golem_table		[]	=
{
   { "heart",	GOLEM_VNUM_HEART,	GOLEM_HEART	},
   { "head",	GOLEM_VNUM_HEAD,	GOLEM_HEAD	},
   { "leg",		GOLEM_VNUM_LEG,		GOLEM_LEG	},
   { "arm",		GOLEM_VNUM_ARM,		GOLEM_ARM	},
};


 
/* wiznet table and prototype for future flag setting */
const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,         IM },
   {    "prefix",	WIZ_PREFIX,	IM },
   {    "ticks",        WIZ_TICKS,      IM },
   {    "logins",       WIZ_LOGINS,     IM },
   {    "sites",        WIZ_SITES,      L4 },
   {    "links",        WIZ_LINKS,      L7 },
   {	"newbies",	WIZ_NEWBIE,	IM },
   {	"spam",		WIZ_SPAM,	L5 },
   {    "deaths",       WIZ_DEATHS,     IM },
   {    "resets",       WIZ_RESETS,     L4 },
   {    "mobdeaths",    WIZ_MOBDEATHS,  L4 },
   {    "flags",	WIZ_FLAGS,	L5 },
   {	"penalties",	WIZ_PENALTIES,	L5 },
   {	"saccing",	WIZ_SACCING,	L5 },
   {	"levels",	WIZ_LEVELS,	IM },
   {	"load",		WIZ_LOAD,	L2 },
   {	"restore",	WIZ_RESTORE,	L2 },
   {	"snoops",	WIZ_SNOOPS,	L2 },
   {	"switches",	WIZ_SWITCHES,	L2 },
   {	"secure",	WIZ_SECURE,	L1 },
   {	NULL,		0,		0  }
};

/* attack table  -- not very organized :( */
const 	struct attack_type	attack_table	[MAX_DAMAGE_MESSAGE]	=
{
    { 	"none",		"hit",		-1		},  /*  0 */
    {	"slice",	"slice", 	DAM_SLASH	},	
    {   "stab",		"stab",		DAM_PIERCE	},
    {	"slash",	"slash",	DAM_SLASH	},
    {	"whip",		"whip",		DAM_SLASH	},
    {   "claw",		"claw",		DAM_SLASH	},  /*  5 */
    {	"blast",	"blast",	DAM_BASH	},
    {   "pound",	"pound",	DAM_BASH	},
    {	"crush",	"crush",	DAM_BASH	},
    {   "grep",		"grep",		DAM_SLASH	},
    {	"bite",		"bite",		DAM_PIERCE	},  /* 10 */
    {   "pierce",	"pierce",	DAM_PIERCE	},
    {   "suction",	"suction",	DAM_BASH	},
    {	"beating",	"beating",	DAM_BASH	},
    {   "digestion",	"digestion",	DAM_ACID	},
    {	"charge",	"charge",	DAM_BASH	},  /* 15 */
    { 	"slap",		"slap",		DAM_BASH	},
    {	"punch",	"punch",	DAM_BASH	},
    {	"wrath",	"wrath",	DAM_ENERGY	},
    {	"magic",	"magic",	DAM_ENERGY	},
    {   "divine",	"divine power",	DAM_HOLY	},  /* 20 */
    {	"cleave",	"cleave",	DAM_SLASH	},
    {	"scratch",	"scratch",	DAM_PIERCE	},
    {   "peck",		"peck",		DAM_PIERCE	},
    {   "peckb",	"peck",		DAM_BASH	},
    {   "chop",		"chop",		DAM_SLASH	},  /* 25 */
    {   "sting",	"sting",	DAM_PIERCE	},
    {   "smash",	 "smash",	DAM_BASH	},
    {   "shbite",	"shocking bite",DAM_LIGHTNING	},
    {	"flbite",	"flaming bite", DAM_FIRE	},
    {	"frbite",	"freezing bite", DAM_COLD	},  /* 30 */
    {	"acbite",	"acidic bite", 	DAM_ACID	},
    {	"chomp",	"chomp",	DAM_PIERCE	},
    {  	"drain",	"life drain",	DAM_NEGATIVE	},
    {   "thrust",	"thrust",	DAM_PIERCE	}, 
    {   "slime",	"slime",	DAM_ACID	}, /*35*/
    {	"shock",	"shock",	DAM_LIGHTNING	},
    {   "thwack",	"thwack",	DAM_BASH	},
    {   "flame",	"flame",	DAM_FIRE	},
    {   "chill",	"chill",	DAM_COLD	},	
	{	"warp",		"{Mw{Gar{Mp{x",	DAM_ENERGY	}, /*40*/
	{	"wail",		"banshee wail",	DAM_MENTAL	},
	{	"blaze",	"blaze",		DAM_FIRE	},
	{	"icicle",	"icicle",		DAM_COLD	},
	{	"lunge",	"lunge",		DAM_PIERCE	},
	{	"scythe",	"wicked scythe",	DAM_SLASH	},
    {   NULL,		NULL,		0		}
};





/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[MAX_STAT_VALUE]		=
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 130, 12 },
    {  0,  0, 130, 13 }, /* 13  */
    {  0,  1, 140, 14 },
    {  1,  1, 150, 15 }, /* 15  */
    {  1,  2, 165, 16 },
    {  2,  3, 180, 22 },
    {  2,  3, 200, 25 }, /* 18  */
    {  3,  4, 225, 30 },
    {  3,  5, 250, 35 }, /* 20  */
    {  4,  6, 300, 40 },
    {  4,  6, 350, 45 },
    {  5,  7, 400, 50 },
    {  5,  8, 450, 55 },
    {  6,  9, 500, 60 },  /* 25   */
    {  6,  10, 550, 65 },
    {  7,  11, 600, 70 },
    {  7,  12, 650, 75 },
    {  8,  13, 700, 80 },
    {  8,  14, 750, 85 }, /* 30 */
    {  9,  15, 800, 90 },
    {  9,  16, 850, 95 },
    {  10,  17, 900, 100 },
	{  10,  18, 950, 105 },
    {  11,  19, 1000, 110 }, /* 35 */
    {  12,  20, 1050, 115 },
    {  12,  21, 1100, 120 },
    {  13,  22, 1150, 125 },
    {  13,  23, 1200, 130 },
    {  14,  24, 1250, 135 }, /* 40 */
	{  15,  25, 1300, 140 },
	{  16,  26, 1350, 145 },
	{  17,  27, 1400, 150 },
	{  18,  28, 1450, 155 },
	{  19,  29, 1500, 160 },/* 50 */
	{  20,  30, 1550, 165 },
	{  21,  31, 1600, 170 },
	{  22,  32, 1650, 175 },
	{  23,  33, 1700, 180 },
	{  24,  34, 1750, 185 } /* 55 */
};



const	struct	int_app_type	int_app		[MAX_STAT_VALUE]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 80 },
    { 80 },	/* 25 */
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 }, /* 30 */
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 }, /* 35 */
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 }, /* 40 */
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 },/* 45 */
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 },
	{ 80 }/* 50 */


};



const	struct	wis_app_type	wis_app		[MAX_STAT_VALUE]		=
{
    { 0 },	
	{ 1 },	{ 1 },	{ 1 },	{ 1 },	{ 1 },	/* 5  */
	{ 2 },	{ 2 },	{ 2 },	{ 2 },	{ 2 },	/* 10 */
	{ 3 },	{ 3 },	{ 3 },	{ 3 },	{ 3 },	/* 15 */
    { 4 },	{ 4 },	{ 4 },	{ 4 },	{ 4 },	/* 20 */
	{ 5 },	{ 5 },	{ 5 },	{ 5 },	{ 5 },	/* 25 */
	{ 6 },	{ 6 },	{ 6 },	{ 6 },	{ 7	},	/* 30 */
	{ 8 },  { 8 },	{ 8 },	{ 8 },	{ 8 },	/* 35 */
    { 9 },	{ 9 },	{ 9 },	{ 9 },	{ 9 },	/* 40 */
	{ 10 },	{ 10 },	{ 10 },	{ 10 },	{ 10 },	/* 45 */
	{ 11 },	{ 11 },	{ 11 },	{ 11 },	{ 11 }	/* 50 */
};



const	struct	dex_app_type	dex_app		[MAX_STAT_VALUE]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -125 },    /* 25 */
    { -150 },
    { -175 },
    { -200 },
    { -225 }, /* 30 */
    { -250 },
    { -275 },
    { -300 },
    { -325 },
    { -350 }, /* 35 */
    { -375 },
    { -400 },
    { -425 },
    { -450 },
    { -475 },
    { -500 } /* 40 */
};


const	struct	con_app_type	con_app		[MAX_STAT_VALUE]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 },    /* 25 */
    {  9, 99 },
    {  10, 99 },
    {  11, 99 },
    {  12, 99 },
    {  13, 99 }, /* 30 */
    {  14, 99 },
    {  15, 99 },
    {  16, 99 },
    {  17, 99 },
    {  18, 99 }, /* 35 */
    {  19, 99 },
    {  20, 99 },
    {  21, 99 },
    {  22, 99 },
    {  23, 99 } /* 40 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[]	=
{
/*    name			color	proof, full, thirst, food, ssize */
    { "water",			"clear",	{   0, 1, 10, 0, 16 }	},
    { "beer",			"amber",	{  12, 1,  8, 1, 12 }	},
    { "red wine",		"burgundy",	{  30, 1,  8, 1,  5 }	},
    { "ale",			"brown",	{  15, 1,  8, 1, 12 }	},
    { "dark ale",		"dark",		{  16, 1,  8, 1, 12 }	},

    { "whisky",			"golden",	{ 120, 1,  5, 0,  2 }	},
    { "lemonade",		"pink",		{   0, 1,  9, 2, 12 }	},
    { "firebreather",		"boiling",	{ 190, 0,  4, 0,  2 }	},
    { "local specialty",	"clear",	{ 151, 1,  3, 0,  2 }	},
    { "slime mold juice",	"green",	{   0, 2, -8, 1,  2 }	},

    { "milk",			"white",	{   0, 2,  9, 3, 12 }	},
    { "tea",			"tan",		{   0, 1,  8, 0,  6 }	},
    { "coffee",			"black",	{   0, 1,  8, 0,  6 }	},
    { "blood",			"red",		{   0, 2, -1, 2,  6 }	},
    { "salt water",		"clear",	{   0, 1, -2, 0,  1 }	},

    { "coke",			"brown",	{   0, 2,  9, 2, 12 }	}, 
    { "root beer",		"brown",	{   0, 2,  9, 2, 12 }   },
    { "elvish wine",		"green",	{  35, 2,  8, 1,  5 }   },
    { "white wine",		"golden",	{  28, 1,  8, 1,  5 }   },
    { "champagne",		"golden",	{  32, 1,  8, 1,  5 }   },

    { "mead",			"honey-colored",{  34, 2,  8, 2, 12 }   },
    { "rose wine",		"pink",		{  26, 1,  8, 1,  5 }	},
    { "benedictine wine",	"burgundy",	{  40, 1,  8, 1,  5 }   },
    { "vodka",			"clear",	{ 130, 1,  5, 0,  2 }   },
    { "cranberry juice",	"red",		{   0, 1,  9, 2, 12 }	},

    { "orange juice",		"orange",	{   0, 2,  9, 3, 12 }   }, 
    { "absinthe",		"green",	{ 200, 1,  4, 0,  2 }	},
    { "brandy",			"golden",	{  80, 1,  5, 0,  4 }	},
    { "aquavit",		"clear",	{ 140, 1,  5, 0,  2 }	},
    { "schnapps",		"clear",	{  90, 1,  5, 0,  2 }   },

    { "icewine",		"purple",	{  50, 2,  6, 1,  5 }	},
    { "amontillado",		"burgundy",	{  35, 2,  8, 1,  5 }	},
    { "sherry",			"red",		{  38, 2,  7, 1,  5 }   },	
    { "framboise",		"red",		{  50, 1,  7, 1,  5 }   },
    { "rum",			"amber",	{ 151, 1,  4, 0,  2 }	},

    { "cordial",		"clear",	{ 100, 1,  5, 0,  2 }   },
    { NULL,			NULL,		{   0, 0,  0, 0,  0 }	}
};

/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

    {
	"reserved",
	{0},{0},
	0,	0,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT( 0),	 0,	 0,
	"",			"",		"", ""
    },

    {
	"acid blast",
	{0}, {0},
	spell_acid_blast, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(70),	35,	12,
	"acid blast",		"!Acid Blast!", "", "TEACH_NONE"
    },

    {
	"armor",
	{0}, {0},
	spell_armor, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT( 1),	 5,	12,
	"",			"You feel less armored.",	"", "TEACH_NONE"
    },

    {
	"bless",
	{0}, {0},
	spell_bless, form_null,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT( 3),	 5,	12,
	"",			"You feel less righteous.", 
	"$p's holy aura fades.", "TEACH_NONE"
    },

    {
	"blindness",
	{0}, {0},
	spell_blindness, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,		SLOT( 4),	 5,	12,
	"",			"You can see again.",	"", "TEACH_NONE"
    },

    {
	"burning hands",
	{0}, {0},
	spell_burning_hands, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 5),	20,	12,
	"burning hands",	"!Burning Hands!", 	"", "TEACH_NONE"
    },

    {
	"call lightning",
	{0}, {0},
	spell_call_lightning, form_null,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT( 6),	15,	12,
	"lightning bolt",	"!Call Lightning!",	"", "TEACH_NONE"
    },

    {   
	"calm",	
	{0}, {0},
	spell_calm, form_null,		TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(509),	30,	12,
	"",			"You have lost your peace of mind.",	"", "TEACH_NONE"
    },

    {
	"cancellation",
	{0}, {0},
	spell_cancellation, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(507),	20,	12,
	"",			"!cancellation!",	"", "TEACH_NONE"
    },

    {
	"cause critical",
	{0}, {0},
	spell_cause_critical, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(63),	20,	12,
	"spell",		"!Cause Critical!",	"", "TEACH_NONE"
    },

    {
	"cause light",
	{0}, {0},
	spell_cause_light, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(62),	5,	12,
	"spell",		"!Cause Light!",	"", "TEACH_NONE"
    },

    {
	"cause serious",
	{0}, {0},
	spell_cause_serious, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(64),	10,	12,
	"spell",		"!Cause Serious!",	"", "TEACH_NONE"
    },

    {   
	"chain lightning",
	{0}, {0},
	spell_chain_lightning, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(500),	25,	12,
	"lightning",		"!Chain Lightning!",	"", "TEACH_NONE"
    }, 

    {
	"change sex",
	{0}, {0},
	spell_change_sex, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(82),	15,	12,
	"",			"Your body feels familiar again.",	"", "TEACH_NONE"
    },

    {
	"charm person",
	{0}, {0},
	spell_charm_person, form_null,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	SLOT( 7),	 5,	12,
	"",			"You feel more self-confident.",	"", "TEACH_NONE"
    },

    {
	"chill touch",
	{0}, {0},
	spell_chill_touch, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 8),	7,	12,
	"chilling touch",	"You feel less cold.",	"", "TEACH_NONE"
    },

    {
	"colour spray",
	{0}, {0},
	spell_colour_spray, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(10),	25,	12,
	"colour spray",		"!Colour Spray!",	"", "TEACH_NONE"
    },

    {
	"continual light",
	{0}, {0},
	spell_continual_light, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(57),	 7,	12,
	"",			"!Continual Light!",	"", "TEACH_NONE"
    },

    {
	"control weather",
	{0}, {0},
	spell_control_weather, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(11),	25,	12,
	"",			"!Control Weather!",	"", "TEACH_NONE"
    },

    {
	"create food",
	{0}, {0},
	spell_create_food, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(12),	 5,	12,
	"",			"!Create Food!",	"", "TEACH_NONE"
    },

    {
	"create rose",
	{0}, {0},
	spell_create_rose, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(511),	30, 	12,
	"",			"!Create Rose!",	"", "TEACH_NONE"
    },  

    {
	"create spring",
	{0}, {0},
	spell_create_spring, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(80),	20,	12,
	"",			"!Create Spring!",	"", "TEACH_NONE"
    },

    {
	"create water",	
	{0}, {0},
	spell_create_water, form_null,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(13),	 5,	12,
	"",			"!Create Water!",	"", "TEACH_NONE"
    },

    {
	"cure blindness",
	{0}, {0},
	spell_cure_blindness, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(14),	 5,	12,
	"",			"!Cure Blindness!",	"", "TEACH_NONE"
    },

    {
	"cure critical",
	{0}, {0},
	spell_cure_critical, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(15),	20,	12,
	"",			"!Cure Critical!",	"", "TEACH_NONE"
    },

    {
	"cure disease",
	{0}, {0},
	spell_cure_disease, form_null,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(501),	20,	12,
	"",			"!Cure Disease!",	"", "TEACH_NONE"
    },

    {
	"cure light",
	{0}, {0},
	spell_cure_light, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(16),	8,	12,
	"",			"!Cure Light!",		"", "TEACH_NONE"
    },

    {
	"cure poison",
	{0}, {0},
	spell_cure_poison, form_null,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(43),	 5,	12,
	"",			"!Cure Poison!",	"", "TEACH_NONE"
    },

    {
	"cure serious",
	{0}, {0},
	spell_cure_serious, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(61),	15,	12,
	"",			"!Cure Serious!",	"", "TEACH_NONE"
    },

    {
	"curse",
	{0}, {0},
	spell_curse, form_null,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_curse,		SLOT(17),	20,	12,
	"curse",		"The curse wears off.", 
	"$p is no longer impure.", "TEACH_NONE"
    },

    {
	"demonfire",
	{0}, {0},
	spell_demonfire, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(505),	20,	12,
	"torments",		"!Demonfire!",		"", "TEACH_NONE"
    },	

    {
	"detect evil",
	{0}, {0},
	spell_detect_evil, form_null,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(18),	 5,	12,
	"",			"The red in your vision disappears.",	"", "TEACH_NONE"
    },

    {
    "detect good",
	{0}, {0},
    spell_detect_good, form_null,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   SLOT(513),        5,     12,
    "",                     "The gold in your vision disappears.",	"", "TEACH_NONE"
    },

    {
	"detect hidden",
	{0}, {0},
	spell_detect_hidden, form_null,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(44),	 5,	12,
	"",			"You feel less aware of your surroundings.",	
	"", "TEACH_NONE"
    },

    {
	"detect invis",
	{0}, {0},
	spell_detect_invis, form_null,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(19),	 5,	12,
	"",			"You no longer see invisible objects.",
	"", "TEACH_NONE"
    },

    {
	"detect magic",
	{0}, {0},
	spell_detect_magic, form_null,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(20),	 5,	12,
	"",			"The detect magic wears off.",	"", "TEACH_NONE"
    },

    {
	"detect poison",
	{0}, {0},
	spell_detect_poison, form_null,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(21),	 5,	12,
	"",			"!Detect Poison!",	"", "TEACH_NONE"
    },

	{
	"discern weakness",
	{0}, {0},
	spell_discern_weakness, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(606),	 9,	12,
	"",			"!Discern Weakness!",	"", "TEACH_NONE"
    },

    {
	"dispel evil",
	{0}, {0},
	spell_dispel_evil, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(22),	25,	12,
	"dispel evil",		"!Dispel Evil!",	"", "TEACH_NONE"
    },

    {
	"dispel good",
	{0}, {0},
    spell_dispel_good, form_null,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   SLOT(512),      25,     12,
    "dispel good",          "!Dispel Good!",	"", "TEACH_NONE"
    },

    {
	"dispel magic",
	{0}, {0},
	spell_dispel_magic, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(59),	15,	12,
	"",			"!Dispel Magic!",	"", "TEACH_NONE"
    },

	{
	"divine right",
	{0}, {0},
	spell_divine_right, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(612),	100,	12,
	"{Dd{Ri{yvi{Rn{De {Dr{Ri{yg{Rh{Dt{x",			"Your divine right to combat has left you",
	"", "TEACH_ZEALOT_1"
    },

    {
	"dragonfury",
	{0}, {0},
	spell_dragonfury, form_null,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT( 614),	 100,	12,
	"",			"You lose the fury of your dragon heritage", 
	"$p loses the fury accorded him by his dragon heritage", "TEACH_DRACONIAN_1"
    },

	{
	"elemental shield",
	{0}, {0},
	spell_elemental_shield, form_null,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		SLOT(611),	75,	12,
	"",			"Your shield from the elements has departed",
	"", "TEACH_TAO_2"
    },

    {
	"earthquake",
	{0}, {0},
	spell_earthquake, form_null,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(23),	15,	12,
	"earthquake",		"!Earthquake!",		"", "TEACH_NONE"
    },

    {
	"enchant armor",
	{0}, {0},
	spell_enchant_armor, form_null,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(510),	100,	24,
	"",			"!Enchant Armor!",	"", "TEACH_NONE"
    },

    {
	"enchant weapon",
	{0}, {0},
	spell_enchant_weapon, form_null,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(24),	100,	24,
	"",			"!Enchant Weapon!",	"", "TEACH_NONE"
    },

    {
	"energy drain",	
	{0}, {0},
	spell_energy_drain, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(25),	25,	12,
	"energy drain",		"!Energy Drain!",	"", "TEACH_NONE"
    },

	{
	"erecover",
	{0}, {0},
	spell_energy_recover, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(602),	20,	12,
	"",			"!Energy Recovery!",	"", "TEACH_NONE"
    },

	{
	"exodus",
	{0}, {0},
	spell_exodus, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 609),	 250,	30,
	"{REXODUS{x","!EXODUS!",	"", "TEACH_DRAC_1"
    },

    {
	"faerie fire",	
	{0}, {0},
	spell_faerie_fire, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(72),	 5,	12,
	"faerie fire",		"The pink aura around you fades away.",
	"", "TEACH_NONE"
    },

    {
	"faerie fog",
	{0}, {0},
	spell_faerie_fog, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(73),	12,	12,
	"faerie fog",		"!Faerie Fog!",		"", "TEACH_NONE"
    },

    {
	"farsight",
	{0}, {0},
	spell_farsight, form_null,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(521),	36,	20,
	"farsight",		"!Farsight!",		"", "TEACH_NONE"
    },
	
    {
	"fanatics flame",
	{0}, {0},
	spell_fanatic_flame, form_null,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT( 606),	 5,	12,
	"",			"!Fanatic's Flame!",	"$fanatic's flame$", "TEACH_NONE"
    },

    {
	"fireball",
	{0}, {0},
	spell_fireball, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(26),	25,	12,
	"fireball",		"!Fireball!",		"", "TEACH_NONE"
    },

	{
	"fireshield",
	{0}, {0},
	spell_fireshield, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(603),	12,	18,
	"{Rfi{Yre{ys{Wh{yi{Ye{Rld{x","Your {Rfi{Yre{ys{Wh{yi{Ye{Rld{x dissipates.",
	"", "TEACH_NONE"
    },
  
    {
	"fireproof",
	{0}, {0},
	spell_fireproof, form_null,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(523),	10,	12,
	"",			"",	"$p's protective aura fades.", "TEACH_NONE"
    },

    {
	"flamestrike",
	{0}, {0},
	spell_flamestrike, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(65),	30,	12,
	"flamestrike",		"!Flamestrike!",		"", "TEACH_NONE"
    },

	{
	"judgement",
	{0}, {0},
	spell_judgement, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 616),	 85,	20,
	"judgement", "!Judgement!",	"", "TEACH_NONE"
    },

    {
	"fly",
	{0}, {0},
	spell_fly, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(56),	10,	18,
	"",			"You slowly float to the ground.",	"", "TEACH_NONE"
    },

    {
	"floating disc",
	{0}, {0},
	spell_floating_disc, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(522),	40,	24,
	"",			"!Floating disc!",	"", "TEACH_NONE"
    },

    {
	"frenzy",
	{0}, {0},
    spell_frenzy, form_null,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   SLOT(504),      30,     24,
    "",                     "Your rage ebbs.",	"", "TEACH_NONE"
    },

    {
	"gate",
	{0}, {0},
	spell_gate, form_null,		TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(83),	80,	12,
	"",			"!Gate!",		"", "TEACH_NONE"
    },

    {
	"giant strength",
	{0}, {0},
	spell_giant_strength, form_null,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(39),	20,	12,
	"",			"You feel weaker.",	"", "TEACH_NONE"
    },

	{
	"giga blast",
	{0}, {0},
	spell_giga_blast, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 609),	 250,	30,
	"{rg{Ri{rg{Ra {yb{bl{ya{bs{yt{x","!Giga Blaster!",	"", "TEACH_TAO_1"
    },

	{
	"gouge",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_gouge,		SLOT( 0),	0,	24,
	"{Dg{wo{Du{wg{De{x",		"Your vision returns.",	"", "TEACH_NONE"
    },

    {
	"greater healing",
	{0}, {0},
	spell_greater_healing, form_null,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(605),	50,	45,
	"",			"!Greater Healing!",		"", "TEACH_ZEALOT_2"
    },

    {
	"harm",	
	{0}, {0},
	spell_harm, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(27),	35,	12,
	"harm spell",		"!Harm!",		"", "TEACH_NONE"
    },
  
    {
	"haste",
	{0}, {0},
	spell_haste, form_null,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(502),	30,	12,
	"",			"You feel yourself slow down.",	"", "TEACH_NONE"
    },

    {
	"heal",	
	{0}, {0},
	spell_heal, form_null,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(28),	50,	12,
	"",			"!Heal!",		"", "TEACH_NONE"
    },
  
    {
	"heat metal",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(516), 	0,	0,
	"spell",		"!Heat Metal!",		"", "TEACH_NONE"
    },

    {
	"heavenly justice",
	{0}, {0},
	spell_heavenly_justice, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 618),	 250,	25,
	"heavenly justice",			"!Heavenly Justice!",	"", "TEACH_ZEALOT_1"
    },

	{
	"hold person",
	{0}, {0},
	spell_hold_person, form_null,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	NULL,			SLOT( 608),	 5,	12,
	"",			"You have control of your legs again",	"", "TEACH_NONE"
    },

    {
	"holy armor",
	{0}, {0},
	spell_holy_armor, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(603),	95,	18,
	"",			"Your {Dh{wo{Wl{Yy ar{Wm{wo{Dr{x dissipates.",
	"", "TEACH_ZEALOT_1"
    },

    {
	"holy word",
	{0}, {0},
	spell_holy_word, form_null,	TAR_IGNORE,	POS_FIGHTING,
	NULL,			SLOT(506), 	80,	24,
	"holy word",		"!Holy Word!",		"", "TEACH_NONE"
    },

    {
	"identify",	
	{0}, {0},
	spell_identify, form_null,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(53),	12,	24,
	"",			"!Identify!",		"", "TEACH_NONE"
    },

    {
	"infravision",
	{0}, {0},
	spell_infravision, form_null,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(77),	 5,	18,
	"",			"You no longer see in the dark.",	"", "TEACH_NONE"
    },

    {
	"invisibility",
	{0}, {0},
	spell_invis, form_null,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	&gsn_invis,		SLOT(29),	 5,	12,
	"",			"You are no longer invisible.",		
	"$p fades into view.", "TEACH_NONE"
    },

    {
	"know alignment",
	{0}, {0},
	spell_know_alignment, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(58),	 9,	12,
	"",			"!Know Alignment!",	"", "TEACH_NONE"
    },

    {
	"lightning bolt",
	{0}, {0},
	spell_lightning_bolt, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(30),	25,	12,
	"lightning bolt",	"!Lightning Bolt!",	"", "TEACH_NONE"
    },

    {
	"locate object",
	{0}, {0},
	spell_locate_object, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(31),	20,	18,
	"",			"!Locate Object!",	"", "TEACH_NONE"
    },

    {
	"magic missile",
	{0}, {0},
	spell_magic_missile, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(32),	5,	12,
	"magic missile",	"!Magic Missile!",	"", "TEACH_NONE"
    },

    {
	"mass healing",	
	{0}, {0},
	spell_mass_healing, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(508),	100,	36,
	"",			"!Mass Healing!",	"", "TEACH_NONE"
    },

    {
	"mass invis",
	{0}, {0},
	spell_mass_invis, form_null,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	SLOT(69),	20,	24,
	"",			"You are no longer invisible.",		"", "TEACH_NONE"
    },

	{
	"minsc power",
	{0}, {0},
	spell_minsc_power, form_null,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SLOT(607),      30,     24,
	"",                     "Boo does not like you anymore",	"", "TEACH_NONE"
	},

	{
	"mind thrust",
	{0}, {0},
	spell_mind_thrust, form_null,TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SLOT(616),      30,     9,
	"{Bm{Ci{Bn{Cd {yt{Dh{wr{Du{ys{Dt{x", "!Mind Thrust!","", "TEACH_NONE"
	},

	{
	"mind blast",
	{0}, {0},
	spell_mind_blast, form_null, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SLOT(617),      80,     15,
	"{Bm{Ci{Bn{Cd {Rb{Gl{Ca{Gs{Rt{x", "!Mind Blast",	"", "TEACH_NONE"
	},

	{
	"mind crush",
	{0}, {0},
	spell_mind_crush, form_null,TAR_CHAR_OFFENSIVE,    POS_FIGHTING,
	NULL,                   SLOT(618),      150,     20,
	"{Bm{Ci{Bn{Cd {Yc{Dr{ru{Ds{Yh{x", "!Mind Crush!",	"", "TEACH_NONE"
	},


	{
	"mrecover",
	{0}, {0},
	spell_mental_recover, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(601),	20,	12,
	"",			"!Mental Recovery!",	"", "TEACH_NONE"
    },

	{
	"mystic sight",
	{0}, {0},
	spell_mystic_sight, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(615),	100,	12,
	"",			"Your mystic sight fades and the shadows no longer give up their secrets","",
	"TEACH_TAO_1"
	},

    {
    "nexus",
	{0}, {0},
    spell_nexus, form_null,            TAR_IGNORE,             POS_STANDING,
    NULL,                   SLOT(520),       150,   36,
    "",                     "!Nexus!",		"", "TEACH_NONE"
    },

    {
	"pass door",
	{0}, {0},
	spell_pass_door, form_null,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(74),	20,	12,
	"",			"You feel solid again.",	"", "TEACH_NONE"
    },

    {
	"plague",
	{0}, {0},
	spell_plague, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_plague,		SLOT(503),	20,	12,
	"sickness",		"Your sores vanish.",	"", "TEACH_NONE"
    },

    {
	"poison",
	{0}, {0},
	spell_poison, form_null,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_poison,		SLOT(33),	10,	12,
	"poison",		"You feel less sick.",	
	"The poison on $p dries up.", "TEACH_NONE"
    },

    {
	"portal",
	{0}, {0},
	spell_portal, form_null,           TAR_IGNORE,             POS_STANDING,
	NULL,                   SLOT(519),       100,     24,
	"",                     "!Portal!",		"", "TEACH_NONE"
    },

	{
	"precover",
	{0}, {0},
	spell_physical_recover, form_null,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(600),	20,	12,
	"",			"!Physical Recovery!",	"", "TEACH_NONE"
    },

    {
	"protection evil",
	{0}, {0},
	spell_protection_evil, form_null,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(34), 	5,	12,
	"",			"You feel less protected.",	"", "TEACH_NONE"
    },

    {
	"protection good",
	{0}, {0},
	spell_protection_good, form_null,  TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SLOT(514),       5,     12,
	"",                     "You feel less protected.",	"", "TEACH_NONE"
    },

    {
	"ray of truth",
	{0}, {0},
    spell_ray_of_truth, form_null,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   SLOT(518),      20,     12,
    "{yra{Yy {wof {Ytru{yth{x",         "!Ray of Truth!",	"", "TEACH_NONE"
    },

    {
	"recharge",
	{0}, {0},
	spell_recharge, form_null,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(517),	60,	24,
	"",			"!Recharge!",		"", "TEACH_NONE"
    },

    {
	"refresh",
	{0}, {0},
	spell_refresh, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(81),	12,	18,
	"refresh",		"!Refresh!",		"", "TEACH_NONE"
    },

    {
	"remove curse",
	{0}, {0},
	spell_remove_curse, form_null,	TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT(35),	 5,	12,
	"",			"!Remove Curse!",	"", "TEACH_NONE"
    },

	{
	"repulsar",
	{0}, {0},
	spell_repulsar, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(67),	75,	18,
	"",			"Your repulsion wave sputters out of existance",
	"", "TEACH_TAO_2"
    },

	{
	"runic scribing",
	{0}, {0},
	spell_runic_scribing, form_null,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT(81),	150,	18,
	"",		"",		"The runes wear off of your weapon", "TEACH_NONE"
    },

		{
	"rune of warding",
	{0}, {0},
	spell_rune_warding, form_null,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,			SLOT(81),	120,	18,
	"",		"",		"The runes wear off your armor", "TEACH_NONE"
    },

    {
	"sanctuary",
	{0}, {0},
	spell_sanctuary, form_null,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_sanctuary,		SLOT(36),	75,	12,
	"",			"The white aura around your body fades.",
	"", "TEACH_NONE"
    },

    {
	"shield",
	{0}, {0},
	spell_shield, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(67),	12,	18,
	"",			"Your force shield shimmers then fades away.",
	"", "TEACH_NONE"
    },

    {
	"shocking grasp",
	{0}, {0},
	spell_shocking_grasp, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(53),	25,	12,
	"shocking grasp",	"!Shocking Grasp!",	"", "TEACH_NONE"
    },

	{
	"skeleton dance",
	{0}, {0},
	spell_skeleton_dance, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(620),	500,	24,
	"",	"",	"", "TEACH_NONE"
    },

    {
	"sleep",
	{0}, {0},
	spell_sleep, form_null,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,		SLOT(38),	15,	12,
	"",			"You feel less tired.",	"", "TEACH_NONE"
    },

    {
	"slow",
	{0}, {0},
	spell_slow, form_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SLOT(515),      30,     12,
	"",                     "You feel yourself speed up.",	"", "TEACH_NONE"
    },

    {
	"stone skin",
	{0}, {0},
	spell_stone_skin, form_null,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(66),	12,	18,
	"",			"Your skin feels soft again.",	"", "TEACH_NONE"
    },

    {
	"summon",
	{0}, {0},
	spell_summon, form_null,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(40),	50,	12,
	"",			"!Summon!",		"", "TEACH_NONE"
    },

	{
	"supernova",
	{0}, {0},
	spell_supernova, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 610),	 350,	30,
	"{Bs{ru{Bp{re{Br{Wn{Go{Wv{Ga{x","!Supernova!",	"", "TEACH_TAO_1"
    },

    {
	"teleport",
	{0}, {0},
	spell_teleport, form_null,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 		SLOT( 2),	35,	12,
	"",			"!Teleport!",		"", "TEACH_NONE"
    },

	{
	"thermonuclear blast",
	{0}, {0},
	spell_thermonuclear, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(613),	9000,	17,
	"{YN{GU{YC{GL{YE{GA{YR {RDE{rTO{YNA{rTI{RON{x",	"!Thermonuclear Blast!",	"", "TEACH_NONE"
    },

	{
	"venom lance",
	{0}, {0},
	spell_venom_lance, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(609),	200,	35,
	"venom lance",	"!Venom Lance!",	"", "TEACH_DRACONIAN_1"
    },

    {
	"ventriloquate",
	{0}, {0},
	spell_ventriloquate, form_null,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(41),	 5,	12,
	"",			"!Ventriloquate!",	"", "TEACH_NONE"
    },

    {
	"vise grip",
	{0}, {0},
	spell_vise_grip, form_null,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT( 619),	 5,	12,
	"",			"Your vise-like grip on your weapon wears off",	"", "TEACH_NONE"
    },

    {
	"weaken",
	{0}, {0},
	spell_weaken, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(68),	20,	12,
	"spell",		"You feel stronger.",	"", "TEACH_NONE"
    },

	    {
	"spectral hand",
	{0}, {0},
	spell_spectral_hand, form_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(68),	35,	12,
	"",		"",	"", "TEACH_NONE"
    },

/*
 * Dragon breath
 */
    {
	"acid breath",
	{0}, {0},
	spell_acid_breath, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(200),	100,	24,
	"blast of acid",	"!Acid Breath!",	"", "TEACH_NONE"
    },

    {
	"fire breath",
	{0}, {0},
	spell_fire_breath, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(201),	200,	24,
	"blast of flame",	"The smoke leaves your eyes.",	"", "TEACH_NONE"
    },

    {
	"frost breath",
	{0}, {0},
	spell_frost_breath, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(202),	125,	24,
	"blast of frost",	"!Frost Breath!",	"", "TEACH_NONE"
    },

    {
	"gas breath",
	{0}, {0},
	spell_gas_breath, form_null,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(203),	175,	24,
	"blast of gas",		"!Gas Breath!",		"", "TEACH_NONE"
    },

    {
	"lightning breath",
	{0}, {0},
	spell_lightning_breath, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(204),	150,	24,
	"{Bb{Wlas{wt of {Dli{wgh{Wtn{yin{Yg{x",	"!Lightning Breath!",	"", "TEACH_NONE"
    },
/*
 *Wrath of the Elements spell and dam messages
 */

    {
	"elemental wrath",
	{0}, {0},
	spell_elemental_wrath, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(620),	400,	24,
	"",	"!Elemental Wrath!",	"", "TEACH_NONE"
    },

    {
	"wrath fire",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(0),	0,	0,
	"{RF{yl{Ya{Rm{Yi{yn{Rg {DW{wr{Ra{wt{Dh{x",	"!Wrath Fire!",	"", "TEACH_NONE"
    },
    {
	"wrath cold",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(0),	0,	0,
	"{BF{cr{Be{cez{Bi{cn{Bg {DW{wr{Ba{wt{Dh{x",	"!Wrath Cold!",	"", "TEACH_NONE"
    },
    {
	"wrath acid",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(0),	0,	0,
	"{gA{cc{Gid{ci{gc {DW{wr{Ga{wt{Dh{x",	"!Wrath Acid!",	"", "TEACH_NONE"
    },
    {
	"wrath lightning",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(0),	0,	0,
	"{DE{yl{Ye{Dc{yt{Yr{Di{yc{x {DW{wr{Ya{wt{Dh{x",	"!Wrath Lightning!",	"", "TEACH_NONE"
    },
    {
	"wrath negative",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(0),	0,	0,
	"{rS{wa{Dt{ra{Dn{wi{rc {DW{wr{ra{wt{Dh{x",	"!Wrath Negative!",	"", "TEACH_NONE"
    },
    {
	"wrath holy",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(0),	0,	0,
	"{WD{wi{yvi{wn{We{x {DW{wr{ya{wt{Dh{x",	"!Wrath Holy!",	"", "TEACH_NONE"
    },

/*
 *Skeletal Dance skills
 */
	{
	"skeletal mage",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(53),	0,	0,
	"{Ds{Wk{De{Wl{De{Wt{Da{Wl {Dm{Wa{Dg{We{x throws back it's head, cackles insanely, and",
	"!Skeletal Mage!",	"", "TEACH_NONE"
    },

	{
	"skeletal archer",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(53),	0,	0,
	"{Ds{Yk{De{Yl{De{Yt{Da{Yl {Da{Yr{Dc{Yh{De{Yr{x raises a bow, pulls back an arrow, and",
	"!Skeletal Archer!",	"", "TEACH_NONE"
    },

	{
	"skeletal warrior",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(53),	0,	0,
	"{Ds{Rk{De{Rl{De{Rt{Da{Rl {Dw{Ra{Dr{Rr{Di{Ro{Dr{x raises a sword, laughs madly, and",
	"!Skeletal Warrior!",	"", "TEACH_NONE"
    },

	{
	"phantasmal wraith",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(53),	0,	0,
	"{Dp{wh{Da{wn{Dt{wa{Ds{wm{Da{wl {Dw{wr{Da{wi{Dt{wh{x fades into existence, points a bony finger, and",
	"!phantasmal wraith!",	"", "TEACH_NONE"
    },

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
        "general purpose",
		{0}, {0},
        spell_general_purpose, form_null,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(401),      0,      12,
        "general purpose ammo", "!General Purpose Ammo!",	"", "TEACH_NONE"
    },
 
    {
        "high explosive",
		{0}, {0},
        spell_high_explosive, form_null,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SLOT(402),      0,      12,
        "high explosive ammo",  "!High Explosive Ammo!",	"", "TEACH_NONE"
    },


/* combat and weapons skills */


    {
	"axe",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_axe,            	SLOT( 0),       0,      0,
	"",                     "!Axe!",		"", "TEACH_NONE"
    },

	{
	"backstab", 
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_backstab,          SLOT( 0),        0,     24,
	"backstab",             "!Backstab!",		"", "TEACH_NONE"
    },

    {
	"bash",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_bash,            	SLOT( 0),       0,      24,
	"bash",                 "!Bash!",		"", "TEACH_NONE"
    },

    {
	"berserk",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_berserk,        	SLOT( 0),       0,      24,
	"",                     "You feel your pulse slow down.",	"", "TEACH_NONE"
    },

	{
	"bow", 
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_bow,     			SLOT( 0),       0,      0,
	"arrow",                     "!Bow!",		"", "TEACH_NONE"
    },

	{
	"burst of speed",
	{0}, {0},
	spell_null, form_null,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_burst_of_speed,			SLOT(0),	30,	12,
	"","The adreneline runs out of your system as you slow to normal speed",	"", "TEACH_DAGASHI_2"
    },

	{
	"circle",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_circle,          SLOT( 0),        0,     24,
	"circle",             "!Circle!",		"", "TEACH_DAGASHI_1"
    },

    {
	"coat",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,	  	POS_RESTING,
	&gsn_coat,		SLOT(0),	0,	36,
	"",			"!Coat!",		"", "TEACH_DAGASHI_1"
    },

    {
	"counter",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_counter,           SLOT( 0),       0,      0,
	"counterattack",        "!Counter!",   "", "TEACH_NONE"
    },

	{
	"create golem",
	{0}, {0},
	spell_null, form_null,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_create_golem,		SLOT(0),	200,	12,
	"",			"!Create Golem!","", "TEACH_NONE"
    },

    {
	"dagashi poison",
	{0}, {0},
	spell_null, form_null,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_dagashi_poison,		SLOT(0),	10,	12,
	"{BD{ra{Bg{ra{Bs{rh{Bi{x poisoning","Somehow, you have survived the {BD{ra{Bg{ra{Bs{rh{Bi{x poisons","The poison on $p dries up.",
	"TEACH_DAGASHI_2"
    },

    {
	"dagger",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_dagger,            SLOT( 0),       0,      0,
	"",                     "!Dagger!",		"", "TEACH_NONE"
    },

	{
	"dirt kicking",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dirt,		SLOT( 0),	0,	24,
	"kicked dirt",		"You rub the dirt out of your eyes.",	"", "TEACH_NONE"
    },

    {
	"disarm",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_disarm,            SLOT( 0),        0,     24,
	"",                     "!Disarm!",		"", "TEACH_NONE"
    },
 
    {
	"dodge",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_dodge,             SLOT( 0),        0,     0,
	"",                     "!Dodge!",		"", "TEACH_NONE"
    },
 
    {
	"enhanced damage",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_enhanced_damage,   SLOT( 0),        0,     0,
	"",                     "!Enhanced Damage!",	"", "TEACH_NONE"
    },

    {
	"envenom",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,	  	POS_RESTING,
	&gsn_envenom,		SLOT(0),	0,	36,
	"",			"!Envenom!",		"", "TEACH_NONE"
    },

    { 
	"fast healing",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_fast_healing,	SLOT( 0),	0,	0,
	"",			"!Fast Healing!",	"", "TEACH_NONE"
    },

	{
	"fatality",
	{0}, {0},
	spell_null, form_null, TAR_IGNORE,		POS_FIGHTING,
	&gsn_fatality,			SLOT(0),	0,	60,
	"Fatality",	"!Fatality!", "", "TEACH_SENSEI_2"
	},

	{
	"fifth attack",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_fifth_attack,      SLOT( 0),        0,     0,
	"",                     "!Fifth Attack!",	"", "TEACH_MULTI_HIT"
    },

	{
    "find dragon",
	{0}, {0},
    spell_null, form_null,	TAR_IGNORE,		POS_RESTING,
    &gsn_find_dragon,	SLOT(0),	0,	0,
    "",		"!Find Dragon",	"", "TEACH_NONE"
	},

    {
	"flail",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_flail,            	SLOT( 0),       0,      0,
    "",                     "!Flail!",		"", "TEACH_NONE"
    },

	{
	"forging",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_forging,		SLOT( 0),	0,	0,
	"",			"!Forging!",		"", "TEACH_NONE"
    },

    {
	"fortify",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_fortify,          SLOT( 0),        0,     80,
	"fortify",             "Your fortifications crumble around you","", "TEACH_LEGIONARE_1"
    },

	{
	"fourth attack",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_fourth_attack,      SLOT( 0),        0,     0,
	"",                     "!Fourth Attack!",	"", "TEACH_MULTI_HIT"
    },

	{
	"gouge",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_gouge,		SLOT( 0),	0,	24,
	"{Dg{wo{Du{wg{De{x",		"Your vision returns.",	"", "TEACH_DAGASHI_2"
    },

	{
	"haduken",
	{0}, {0},
	spell_null, form_null, TAR_IGNORE,		POS_FIGHTING,
	&gsn_haduken,			SLOT(0),	0,	30,
	"{YH{Ga{cd{Wu{ck{Ge{Yn{x",	"!Haduken!", "", "TEACH_SENSEI_1"
	},

	{
	"haggle",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_haggle,		SLOT( 0),	0,	0,
	"",			"!Haggle!",		"", "TEACH_NONE"
    },

    {
	"hand to hand",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_hand_to_hand,	SLOT( 0),	0,	0,
	"",			"!Hand to Hand!",	"", "TEACH_NONE"
    },

    {
	"hide",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		SLOT( 0),	 0,	12,
	"",			"!Hide!",		"", "TEACH_NONE"
    },

    {
	"kick",
	{0}, {0},
	spell_null, form_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_kick,              SLOT( 0),        0,     12,
	"kick",                 "!Kick!",		"", "TEACH_NONE"
    },

    {
	"lore",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_lore,		SLOT( 0),	0,	36,
	"",			"!Lore!",		"", "TEACH_NONE"
    },

    {
	"mace",	
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_mace,            	SLOT( 0),       0,      0,
	"",                     "!Mace!",		"", "TEACH_NONE"
    },

    {
	"meditation",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_meditation,	SLOT( 0),	0,	0,
	"",			"Meditation",		"", "TEACH_NONE"
    },

    {
	"parry",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_parry,             SLOT( 0),        0,     0,
	"",                     "!Parry!",		"", "TEACH_NONE"
    },

    {
	"peek",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		SLOT( 0),	 0,	 0,
	"",			"!Peek!",		"", "TEACH_NONE"
    },

    {
	"phalanx",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_phalanx,        	SLOT( 0),       0,      10,
	"", "Your discipline breaks, and the formation crumbles",	"","TEACH_LEGIONARE_2"
    },

	{
    "phase",
	{0}, {0},
    spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_phase,             SLOT( 0),       0,      0,
    "",                     "!Phase!",   "", "TEACH_NONE"
    },

    {
	"pick lock",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		SLOT( 0),	 0,	12,
	"",			"!Pick!",		"", "TEACH_NONE"
    },
	
    {
	"polearm",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_polearm,           SLOT( 0),       0,      0,
	"",                     "!Polearm!",		"", "TEACH_NONE"
    },

	{
	"precision",
	{0}, {0},
	spell_null, form_null,				TAR_IGNORE,				POS_FIGHTING,
	&gsn_precision,			SLOT( 0),		0,		0,
	"",						"!Precision!",		"", "TEACH_LEGIONARE_2"
	},

    {
	"rescue",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_rescue,            SLOT( 0),        0,     12,
	"",                     "!Rescue!",		"", "TEACH_NONE"
    },

    {
	"scrolls",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scrolls,		SLOT( 0),	0,	24,
	"",			"!Scrolls!",		"", "TEACH_NONE"
    },

	{
	"second attack",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_second_attack,     SLOT( 0),        0,     0,
	"",                     "!Second Attack!",	"", "TEACH_NONE"
    },

	{
	"second cast",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_second_cast,     SLOT( 0),        0,     0,
	"",                     "!Second Cast!",	"", "TEACH_NONE"
	},

	{
	"sever",
	{0}, {0},
	spell_null, form_null, TAR_IGNORE,		POS_FIGHTING,
	&gsn_sever,			SLOT(0),	0,	60,
	"{Rs{re{Rv{Der{Ri{rn{Rg{x",	"!Severing!", "", "TEACH_DRACONIAN_2"
	},

    {
	"shadowslip",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_shadow_slip,		SLOT( 0),	 0,	12,
	"",			"You slide out of the shadows.",	"", "TEACH_DAGASHI_2"
    },

    {
	"shield block",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_shield_block,	SLOT(0),	0,	0,
	"",			"!Shield!",		"", "TEACH_NONE"
    },

    {
	"shield wall",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_shield_wall,          SLOT( 0),        0,     10,
	"shield wall","You are unable to hide behind your shield for the moment","", "TEACH_LEGIONARE_1"
    },

    {
	"smite",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_smite,          SLOT( 0),        0,     60,
	"{BS{YM{WI{YT{BE{x",             "!Smite!",		"", "TEACH_ZEALOT_2"
    },

    {
	"sneak",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		SLOT( 0),	 0,	12,
	"",			"You no longer feel stealthy.",	"", "TEACH_NONE"
    },

    {
	"spear",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_spear,            	SLOT( 0),       0,      0,
	"",                     "!Spear!",		"", "TEACH_NONE"
    },

    {
	"staves",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_staves,		SLOT( 0),	0,	12,
	"",			"!Staves!",		"", "TEACH_NONE"
    },

    {
	"steal",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		SLOT( 0),	 0,	24,
	"",			"!Steal!",		"", "TEACH_NONE"
    },

    {
	"strike", 
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_strike,          SLOT( 0),        0,     60,
	"{ys{Rt{Wri{Rk{ye{x",             "!Strike!",		"", "TEACH_LEGIONARE_2"
    },

    {
	"sword",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_sword,            	SLOT( 0),       0,      0,
	"",                     "!sword!",		"", "TEACH_NONE"
    },

    {
	"third attack",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_third_attack,      SLOT( 0),        0,     0,
	"",                     "!Third Attack!",	"", "TEACH_MULTI_HIT"
    },

	{
	"third cast",
	{0}, {0},
	spell_null, form_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_third_cast,     SLOT( 0),        0,     0,
	"",                     "!Third Cast!",	"", "TEACH_NONE"
	},

    {
	"trip",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_trip,		SLOT( 0),	0,	24,
	"trip",			"!Trip!",		"", "TEACH_NONE"
    },

	{
	"triple kick",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_triple_kick,		SLOT (0),		0,	30,
	"Triple Kick",		"!Triple Kick!", "", "TEACH_SENSEI_1"
    },

	{
	"wands",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_wands,		SLOT( 0),	0,	12,
	"",			"!Wands!",		"", "TEACH_NONE"
    },

    {
	"whip",	
	{0}, {0},
    spell_null, form_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_whip,            	SLOT( 0),       0,      0,
    "",                     "!Whip!",	"", "TEACH_NONE"
    },
    
	{
	"whirlwind",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_whirlwind,		SLOT (0),		0,	30,
	"{BW{Ch{Wi{Cr{Bl{CW{Wi{Cn{Bd{x",		"!Whirlwind!", "", "TEACH_NONE"
    },
	
    {
	"recall",
	{0}, {0},
	spell_null, form_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_recall,		SLOT( 0),	0,	12,
	"",			"!Recall!",		"", "TEACH_NONE"
    },

	/*forms*/
	{
	"smiling death",
	{0},{0},
	spell_null,
	form_smiling_death,
	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,	SLOT( 0),	20,	12,
	"smiling death","!SMILING DEATH!","","TEACH_NONE"
    },

    {
	"mantis dance",
	{0},{0},
	spell_null,
	form_mantis_dance,
	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	NULL,	SLOT( 0),200,	20,
	"{Gm{ga{Dnt{gi{Gs {Dd{wa{Gn{wc{De{x","!MANTIS DANCE!","","TEACH_DAGASHI_2"
    },

    {
	"deadly fingertips",
	{0},{0},
	spell_null,
	form_deadly_fingertips,
	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,	SLOT( 0),200,	20,
	"Deadly Fingertips","!DEADLY FINGERTIPS!","","TEACH_SENSEI_2"
    }
};

const   struct  group_type      group_table     [MAX_GROUP]     =
{

    {
	"rom basics",		{ 0, 0, 0, 0, 0 },
	{ "scrolls", "staves", "wands", "recall" }
    },

    {
	"mage basics",		{ 0, -1, -1, -1 },
	{ "dagger" }
    },

    {
	"esper basics",		{ -1, -1, -1, -1,-1,-1,-1,-1,-1,0 },
	{ "whip" }
    },

    {
	"cleric basics",	{ -1, 0, -1, -1 },
	{ "mace" }
    },

    {
	"thief basics",		{ -1, -1, 0, -1},
	{ "dagger", "steal" }
    },

    {
	"warrior basics",	{ -1, -1, -1, 0},
	{ "sword", "second attack" }
    },

	{
	"legionare basics",	{ -1, -1, -1, -1, 0, -1, -1 },
	{ "sword", "spear", "dirt kicking", "second attack" }
    },

	{
	"draconian basics",		{ -1, -1, -1, -1, -1, 0, -1 },
	{ "dagger", "sword", "acid blast" }
    },

	{
	"sensei basics",	{ -1, -1, -1, -1, -1, -1 , 0},
	{ "sword", "spear", "dirt kicking", "second attack" }
    },

	{
	"zealot basics",	{ -1, -1, -1, -1, -1, -1 , -1, 0},
	{ "mace", "spear", "dirt kicking", "second attack" }
    },

	{
	"tao basics",	{ -1, -1, -1, -1, -1, -1 , -1, -1, 0},
	{ "dagger", "spear", "second cast", "third cast" }
    },

	{
	"dagashi basics",	{ -1, -1, -1, -1, -1, -1 , -1, -1, -1, 0},
	{ "dagger", "backstab", "second attack", "dodge", "parry", "gouge" }
    },

    {
	"mage default",		{ 40, -1, -1, -1, -1 , -1, -1},
	{ "beguiling", "combat", "detection", "enhancement", "illusion",
	  "maladictions", "protective", "transportation", "weather" }
    },

    {
		"esper default",		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,40},
	{ "beguiling", "combat", "detection", "illusion",
	  "maladictions", "protective", "transportation", "second cast" }
    },

    {
	"cleric default",	{ -1, 40, -1, -1, -1, -1, -1},
	{ "flail", "attack", "curative",  "benedictions",
	  "detection", "healing", "maladictions", "protective", "shield block",
	  "transportation", "weather", "harmful" }
    },

    {
	"thief default",	{ -1, -1, 40, -1, -1, -1, -1},
	{ "mace", "sword", "backstab", "disarm", "dodge", "second attack",
	  "trip", "hide", "peek", "pick lock", "sneak" }
    },

    {
	"warrior default",	{ -1, -1, -1, 40, -1, -1, -1},
	{ "shield block", "bash", "disarm", "enhanced damage",
	  "parry", "rescue", "third attack" }
    },

    {
	"legionare default",	{ -1, -1, -1, -1, 40, -1, -1},
	{ "weaponsmaster", "shield block", "bash", "disarm", "enhanced damage",
	  "parry", "rescue", "third attack", "fourth attack", "strike", "whirlwind" }
    },

	{
	"draconian default",		{ -1, -1, -1, -1, -1, 40, -1},
	{ "beguiling", "combat", "detection", "enhancement", "illusion",
	  "maladictions", "protective", "transportation", "weather","elemental wrath"}
    },

    {
	"sensei default",	{ -1, -1, -1, -1, -1, -1, 40},
	{ "shield block", "bash", "disarm", "enhanced damage",
	  "parry", "rescue", "third attack", "fourth attack" }
    },

	{
	"zealot default",	{ -1, -1, -1, -1, -1, -1, -1, 40},
	{ "flail", "attack","curative",  "benedictions",
	  "detection", "healing", "maladictions", "protective", "shield block",
	  "transportation", "weather" }
    },

	{
	"tao default",		{ -1, -1, -1, -1, -1, -1, -1, -1, 40},
	{ "combat", "detection", "enhancement", "protective",
	"transportation", "weather" }
    },

	{
	"dagashi default",		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, 40},
	{ "detection","transportation","third attack","enhanced damage","sword",
	"whip", "shadowslip", "gouge"}
    },

{
	"attack",		{ -1, 5, -1, -1, -1, -1, -1, 4, -1, -1, -1 },
	{ "demonfire", "dispel evil", "dispel good", "earthquake",
	  "flamestrike", "heat metal", "ray of truth", "exacation" }
    },

	{
	"beguiling",		{ 4, -1, -1, -1, -1, 4, -1, 6, -1, -1, 3},
	{ "calm", "charm person", "sleep" }
    },

    {
	"benedictions",		{ 8, 5, -1, -1, -1, 7, -1, 4, -1, -1, 8},
	{ "bless", "calm", "frenzy", "holy word", "remove curse" }
    },

    {
	"breath",		{ 6, -1, -1, -1, -1, 3, -1, -1, -1 , -1, 8},
	{ "acid breath", "fire breath", "frost breath", "gas breath",
	  "lightning breath"  }
    },

    {
	"combat",		{ 6, -1, -1, -1, -1, 5, -1, -1, 5, -1, 6},
	{ "acid blast", "burning hands", "chain lightning", "chill touch",
	  "colour spray", "fireball", "lightning bolt", "magic missile",
	  "shocking grasp"  }
    },

    {
	"creation",		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
	{ "continual light", "create food", "create spring", "create water",
	  "create rose", "floating disc" }
    },

    {
	"curative",		{ -1, 4, -1, -1, -1, 5, 6, 3, 4, 6, -1 },
	{ "cure blindness", "cure disease", "cure poison" }
    },

    {
	"detection",		{ 6, 5, 4, -1, -1, 5, 3, 5, 5, 3, 5},
	{ "detect evil", "detect good", "detect hidden", "detect invis","identify",
	  "know alignment", "locate object" }
    },

	{
	"dragon lore",		{ -1, -1, -1, -1, -1, 4, -1, -1 },
	{ "hold person", "vise grip", "venom lance", "dragonfury","mystic sight",
	"exodus","skeletal dance","rune warding","runic scribing"}
    },

    {
	"enchantment",		{ 6, -1, -1, -1, -1, 4, -1, 6, 6, -1, 5 },
	{ "enchant armor", "enchant weapon", "fireproof" }
    },

    {
	"enhancement",		{ 5, 4, 6, -1, 8, 4, 5, 3, 4, 5, 5},
	{ "giant strength", "haste", "infravision", "refresh" }
    },

	{
	"fanatic",		{ -1, -1, -1, -1, -1, -1, -1, 5 },
	{ "greater healing", "holy armor", "fanatics flame","heavenly justice",
	  "divine right"}
    },

    {
	"harmful",		{ -1, 5, -1, -1, -1, -1, -1, 4 },
	{ "cause critical", "cause light", "cause serious", "harm" }
    },

    {
	"healing",		{ -1, 4, -1, -1, -1, -1, -1 , 3},
	{ "cure critical", "cure light", "cure serious", "heal", "refresh" }
    },

    {
	"illusion",		{ 4, -1, 4, -1, -1, 3, 4, 5, 5, 4, 4 },
	{ "invis", "ventriloquate" }
    },

    {
	"maladictions",		{ 5, 6, 8, -1, -1, 4, 6, 5, 5, 6, 6},
	{ "blindness", "curse", "energy drain", "plague",
	  "poison", "slow", "weaken" }
    },

    {
	"protective",		{ 6, 5, -1, -1, -1, 5, -1, 4, 4, -1, 6 },
	{ "armor", "cancellation", "dispel magic", "fireproof",
	  "protection evil", "protection good", "sanctuary", "shield",
	  "stone skin","fireshield" }
    },

	{
	"tao magic",		{ -1, -1, -1, -1, -1, -1, -1, -1, 5 },
	{ "giga blast","supernova","hold person", "vise grip",
	  "elemental shield", "repulsar" }
    },

    {
	"transportation",	{ 4, 4, 6, -1, 8, 3, 5, 3, 3, 5, 3},
	{ "fly", "gate", "nexus", "pass door", "portal", "summon", "teleport" }
    },

    {
	"weaponsmaster",	{ -1, -1, -1, 40, 15, 30, 20, -1, -1, 20, -1},
	{ "axe", "dagger", "flail", "mace", "polearm", "spear", "sword","whip" }
    },

    {
	"weather",		{ 5, -1, -1, -1, -1, 4, -1, -1, 3, -1, 4 },
	{ "faerie fire", "faerie fog", "control weather", "call lightning"
	  "lightning bolt" }
    },

	{
	"mental",	{ -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, 5 },
	{ "mind thrust","mind blast","mind crush" }
	},
};

const	struct	multi_cast_type	multi_cast_table	[MAX_MULTI_CAST]	=
{
	/*{"spell_name"}*/
	{"gate"},
	{"portal"},
	{"nexus"},
	{"summon"},
	{"thermonuclear blast"},
	{"dragonfury"},
	{"fanatics flame"},
	{"shield"},
	{"armor"},
	{"frenzy"},
	{"bless"},
	{"pass door"},
	{"haste"},
	{"sanctuary"},
	{"venom lance"},
	{"enchant armor"},
	{"enchant weapon"},
	{"charm person"},
	{"holy armor"},
	{"repulsar"},
	{"elemental shield"},
	{"stone skin"},
	{"divine right"},
	{"holy word"},
	{"mind crush"},
	{"mind thrust"},
	{"mind blast"},
	{"fly"},
	{"fireshield"}
};

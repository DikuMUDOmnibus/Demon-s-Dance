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

/* race table */
const 	struct	race_type	race_table	[]		=
{
/*
    {
	name,		pc_race?,
	act bits,	aff_by bits,	off bits,
	imm,		res,		vuln,
	form,		parts,		remort_race?,
	regen_affect
    },
*/
    { "unique",		FALSE, 0, 0, 0, 0, 0, 0, 0, 0 },

    { 
	"human",		TRUE, 
	0,		0, 		0,
	0, 		0,		0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Z,  FALSE,
	1
    },

    {
	"elf",			TRUE,
	0,		AFF_INFRARED,	0,
	0,		RES_CHARM,	VULN_IRON,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Z,  FALSE,
	.6666
    },

    {
	"dwarf",		TRUE,
	0,		AFF_INFRARED,	0,
	0,		RES_POISON|RES_DISEASE, VULN_DROWNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Z,  FALSE,
	1.5
    },

    {
	"giant",		TRUE,
	0,		0,		0,
	0,		RES_FIRE|RES_COLD,	VULN_MENTAL|VULN_LIGHTNING,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Z,  FALSE,
	2
    },

	/*remort races*/

	{
	"gargoyle",		TRUE,
	0,		AFF_DETECT_MAGIC|AFF_FLYING,	0,
	IMM_POISON,		RES_DISEASE|RES_BASH|RES_COLD,	VULN_ACID|VULN_PIERCE,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y|Z,  TRUE,
	2
    },

	{
	"vampire",		TRUE,
	0,		AFF_DETECT_EVIL|AFF_INFRARED|AFF_DETECT_GOOD,	0,
	IMM_NEGATIVE,		RES_SOUND|RES_PIERCE|RES_COLD,	VULN_FIRE|VULN_BASH|VULN_HOLY,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y|Z,  TRUE,
	2
    },

	{
	"ogre",		TRUE,
	0,		AFF_REGENERATION|AFF_INFRARED,	0,
	IMM_MENTAL,		RES_BASH|RES_POISON|RES_FIRE,	VULN_SLASH|VULN_COLD,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y|Z,  TRUE,
	3
    },

	{
	"gnome",		TRUE,
	0,		AFF_DETECT_MAGIC|AFF_HASTE,	0,
	0,		RES_MENTAL|RES_BASH|RES_ACID,	VULN_DISEASE|VULN_POISON|VULN_SLASH,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y|Z,  TRUE,
	1.5
    },

	{
	"mordhel",		TRUE,
	0,		AFF_DETECT_EVIL|AFF_DETECT_MAGIC|AFF_HASTE,	0,
	0,		RES_SLASH|RES_BASH|RES_COLD,	VULN_PIERCE|VULN_FIRE|VULN_SOUND,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y|Z,  TRUE,
	1.5
    },

	{
	"angel",		TRUE,
	0,		AFF_DETECT_MAGIC|AFF_FLYING|AFF_DETECT_EVIL,	0,
	IMM_HOLY,		RES_LIGHTNING|RES_COLD,	VULN_FIRE|VULN_NEGATIVE,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y|Z,  TRUE,
	2
    },

	{
	"leprechaun",		TRUE,
	0,		AFF_DETECT_HIDDEN|AFF_DETECT_INVIS|AFF_HASTE,	0,
	0,				RES_PIERCE|RES_NEGATIVE|RES_HOLY,		VULN_SLASH|VULN_LIGHTNING,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y|Z,  TRUE,
	2
    },

	{
	"alarihel",		TRUE,
	0,		AFF_DETECT_HIDDEN|AFF_HASTE,	0,
	IMM_DISEASE,		RES_MENTAL|RES_ACID,	VULN_PIERCE|VULN_POISON,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y|Z,  TRUE,
	1.5
    },

	{
	"demogorgon",	TRUE,
	0,		AFF_DETECT_GOOD|AFF_FLYING|AFF_DETECT_INVIS,		0,
	IMM_FIRE,			RES_SLASH|RES_BASH|RES_LIGHTNING,		VULN_COLD|VULN_ACID|VULN_PIERCE,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Y|Z,  TRUE,
	1.5
	},
	/*end remort races*/

    {
	"bat",			FALSE,
	0,		AFF_FLYING|AFF_DARK_VISION,	OFF_DODGE|OFF_FAST,
	0,		0,		VULN_LIGHT,
	A|G|V,		A|C|D|E|F|H|J|K|P|Z,  FALSE,
	1
    },

    {
	"bear",			FALSE,
	0,		0,		OFF_CRUSH|OFF_DISARM|OFF_BERSERK,
	0,		RES_BASH|RES_COLD,	0,
	A|G|V,		A|B|C|D|E|F|H|J|K|U|V|Z,  FALSE,
	1
    },

    {
	"cat",			FALSE,
	0,		AFF_DARK_VISION,	OFF_FAST|OFF_DODGE,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|U|V|Z,  FALSE,
	1
    },

    {
	"centipede",		FALSE,
	0,		AFF_DARK_VISION,	0,
	0,		RES_PIERCE|RES_COLD,	VULN_BASH,
 	A|B|G|O,		A|C|K|Z,  FALSE,
	1
    },

    {
	"dog",			FALSE,
	0,		0,		OFF_FAST,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|U|V|Z,  FALSE,
	1
    },

    {
	"doll",			FALSE,
	0,		0,		0,
	IMM_COLD|IMM_POISON|IMM_HOLY|IMM_NEGATIVE|IMM_MENTAL|IMM_DISEASE
	|IMM_DROWNING,	RES_BASH|RES_LIGHT,
	VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
	E|J|M|cc,	A|B|C|G|H|K|Z,  FALSE,
	1
    },

    { 	"dragon", 		FALSE, 
	0, 			AFF_INFRARED|AFF_FLYING,	0,
	0,			RES_FIRE|RES_BASH|RES_CHARM, 
	VULN_PIERCE|VULN_COLD,
	A|H|Z,		A|C|D|E|F|G|H|I|J|K|P|Q|U|V|X|Z,  FALSE,
	1
    },

    {
	"fido",			FALSE,
	0,		0,		OFF_DODGE|ASSIST_RACE,
	0,		0,			VULN_MAGIC,
	A|B|G|V,	A|C|D|E|F|H|J|K|Q|V|Z,  FALSE,
	1
    },		
   
    {
	"fox",			FALSE,
	0,		AFF_DARK_VISION,	OFF_FAST|OFF_DODGE,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|V|Z,  FALSE,
	1
    },

    {
	"goblin",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_DISEASE,	VULN_MAGIC,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Z,  FALSE,
	1
    },

    {
	"hobgoblin",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_DISEASE|RES_POISON,	0,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y|Z,  FALSE,
	1
    },

    {
	"kobold",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_POISON,	VULN_MAGIC,
	A|B|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q|Z,  FALSE,
	1
    },

    {
	"lizard",		FALSE,
	0,		0,		0,
	0,		RES_POISON,	VULN_COLD,
	A|G|X|cc,	A|C|D|E|F|H|K|Q|V|Z,  FALSE,
	1
    },

    {
	"modron",		FALSE,
	0,		AFF_INFRARED,		ASSIST_RACE|ASSIST_ALIGN,
	IMM_CHARM|IMM_DISEASE|IMM_MENTAL|IMM_HOLY|IMM_NEGATIVE,
			RES_FIRE|RES_COLD|RES_ACID,	0,
	H,		A|B|C|G|H|J|K|Z,  FALSE,
	1
    },

    {
	"orc",			FALSE,
	0,		AFF_INFRARED,	0,
	0,		RES_DISEASE,	VULN_LIGHT,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Z,  FALSE,
	1
    },

    {
	"pig",			FALSE,
	0,		0,		0,
	0,		0,		0,
	A|G|V,	 	A|C|D|E|F|H|J|K|Z,  FALSE,
	1
    },	

    {
	"rabbit",		FALSE,
	0,		0,		OFF_DODGE|OFF_FAST,
	0,		0,		0,
	A|G|V,		A|C|D|E|F|H|J|K|Z,  FALSE,
	1
    },
    
    {
	"school monster",	FALSE,
	ACT_NOALIGN,		0,		0,
	IMM_CHARM|IMM_SUMMON,	0,		VULN_MAGIC,
	A|M|V,		A|B|C|D|E|F|H|J|K|Q|U|Z,  FALSE,
	1
    },	

    {
	"snake",		FALSE,
	0,		0,		0,
	0,		RES_POISON,	VULN_COLD,
	A|G|X|Y|cc,	A|D|E|F|K|L|Q|V|X|Z,  FALSE,
	1
    },
 
    {
	"song bird",		FALSE,
	0,		AFF_FLYING,		OFF_FAST|OFF_DODGE,
	0,		0,		0,
	A|G|W,		A|C|D|E|F|H|K|P|Z,  FALSE,
	1
    },

    {
	"troll",		FALSE,
	0,		AFF_REGENERATION|AFF_INFRARED|AFF_DETECT_HIDDEN,
	OFF_BERSERK,
 	0,	RES_CHARM|RES_BASH,	VULN_FIRE|VULN_ACID,
	A|B|H|M|V,		A|B|C|D|E|F|G|H|I|J|K|U|V|Z,  FALSE,
	1
    },

    {
	"water fowl",		FALSE,
	0,		AFF_SWIM|AFF_FLYING,	0,
	0,		RES_DROWNING,		0,
	A|G|W,		A|C|D|E|F|H|K|P|Z,  FALSE,
	1
    },		
  
    {
	"wolf",			FALSE,
	0,		AFF_DARK_VISION,	OFF_FAST|OFF_DODGE,
	0,		0,		0,	
	A|G|V,		A|C|D|E|F|J|K|Q|V|Z,  FALSE,
	1
    },

    {
	"wyvern",		FALSE,
	0,		AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN,
	OFF_BASH|OFF_FAST|OFF_DODGE,
	IMM_POISON,	0,	VULN_LIGHT,
	A|B|G|Z,		A|C|D|E|F|H|J|K|Q|V|X|Z,  FALSE,
	1
    },

    {
	"plant",		FALSE,
	0,		AFF_INFRARED,	0,
	0,		0,	VULN_FIRE,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Z,  FALSE,
	1
    },

    
   
	{
	"unique",		FALSE,
	0,		0,		0,
	0,		0,		0,		
	0,		0
    },


    {
	NULL, 0, 0, 0, 0, 0, 0
    }
};

const	struct	pc_race_type	pc_race_table	[]	=
{
    { "null race", "", 0, { 100, 100, 100, 100 },
      { "" }, { 13, 13, 13, 13, 13 }, { 18, 18, 18, 18, 18 }, 0 },
 
/*
    {
	"race name", 	short name, 	points,	{ class multipliers },
	{ bonus skills },
	{ base stats },		{ max stats },		size, "notes on race abilities",
	{com_bonus,cov_bonus,sor_bonus,arc_bonus,wor_bonus}
    },
*/
    {
	"human",	"Human",	0,	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ "" },
	{ 13, 13, 13, 13, 13 },	{ 40, 40, 40, 40, 40 },	SIZE_MEDIUM,
	"None", "None	", "None",
	{5,	5,	5,	5,	5}
    },

    { 	
	"elf",		" Elf ",	5,	{ 100, 125,  100, 120, 100, 100, 100, 100, 100, 100, 100 }, 
	{ "sneak", "hide" },
	{ 12, 14, 13, 15, 11 },	{ 40, 40, 40, 40, 40 }, SIZE_SMALL,
	"None ", "{mCharm	", "None",
	{3,	5,	5,	5,	5}
    },

    {
	"dwarf",	"Dwarf",	8,	{ 150, 100, 125, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ "berserk" },
	{ 14, 12, 14, 10, 15 },	{ 40, 40, 40, 40, 40 }, SIZE_MEDIUM,
	"None","{GPoison {yDisease", "{cDrowning",
	{6,	5,	5,	5,	5}
    },

    {
	"giant",	"Giant",	6,	{ 200, 150, 150, 105, 100, 100, 100, 100, 100, 100, 100 },
	{ "bash", "fast healing" },
	{ 16, 11, 13, 11, 14 },	{ 40, 40, 40, 40, 40 }, SIZE_LARGE,
	"None","{RFire {BCold","{MMental {YLightning",
	{8,	5,	5,	5,	5}
    },

	{
	"gargoyle",	"Grgle",	9,	{ 175, 90, 200, 100, 100, 175, 100, 90, 175, 200, 100 },
	{ "stone skin"},
	{ 19, 17, 22, 15, 18 },	{ 45, 45, 45, 45, 45 },	SIZE_LARGE,
	"{GPoison{W","{yDisease {DBash {BCold","{gAcid {DPierce",
	{5,	5,	5,	5,	5}
    },

	{
	"vampire",	"Vampe",	11,	{ 90, 200, 175, 125, 125, 90, 125, 200, 90, 100, 100 },
	{ "" },
	{ 19, 20, 15, 18, 14 },	{ 45, 45, 45, 45, 45 },	SIZE_MEDIUM,
	"{wNegative","{cSound {DPierce {BCold","{RFire {DBash {YHOLY",
	{5,	5,	5,	5,	5}
    },

	{
	"ogre",	"Ogre ",	10,	{ 200, 200, 200, 50, 75, 200, 100, 200, 200, 200, 100 },
	{ "berserk", "fast healing" },
	{ 22, 15, 15, 16, 22 },	{ 45, 45, 45, 45, 45 },	SIZE_LARGE,
	"{MMental","{GPoison {RFire","{DSlash {BCold",
	{10,	5,	5,	5,	5}
    },

	{
	"gnome",	" Gnome",	8,	{ 70, 125, 80, 200, 200, 80, 200, 125, 80, 100, 100 },
	{ "berserk", "fast healing" },
	{ 16, 22, 19, 17, 15 },	{ 45, 45, 45, 45, 45 },	SIZE_SMALL,
	"None","{MMental {DBash {gAcid","{yDisease {GPoison {DSlash",
	{2,	5,	5,	5,	5}
    },

	{
	"mordhel",	"Mrdhl",	10,	{ 50, 200, 100, 100, 100, 75, 100, 200, 75, 125, 100 },
	{ "energy drain" },
	{ 17, 22, 16, 18, 14 },	{ 45, 45, 45, 45, 45 },	SIZE_MEDIUM,
	"None","{DSlash {DBash {BCold","{DPierce {RFire {cSound",
	{7,	5,	5,	5,	5}
    },
	
	{
	"angel",	"Angel",	11,	{ 100, 50, 200, 175, 175, 100, 175, 50, 100, 150, 100 },
	{ "protection evil" },
	{ 15, 16, 22, 16, 15 },	{ 45, 45, 45, 45, 45 },	SIZE_MEDIUM,
	"{YHoly","{YLightning {BCold","{RFire {wNegative",
	{5,	5,	5,	5,	5}
    },

	{
	"leprechaun",	"Lprcn",	8,	{ 150, 175, 50, 100, 200, 175, 80, 175, 175, 75, 100 },
	{ "sneak", "hide" },
	{ 19, 17, 17, 23, 18 },	{ 45, 45, 45, 45, 45 },	SIZE_MEDIUM,
	"None","{DPierce {wNegative {YHoly","{DSlash {YLightning",
	{3,	5,	5,	5,	5}
    },

	{
	"alarihel",	"Alrhl",	10,	{ 150, 175, 50, 100, 75, 200, 75, 175, 200, 75, 100 },
	{ "frenzy", "sneak" },
	{ 19, 16, 15, 20, 16 },	{ 45, 45, 45, 45, 45 },	SIZE_MEDIUM,
	"{yDisease","{MMental {gAcid","{DPierce {GPoison",
	{5,	5,	5,	5,	5}
    },

	{
	"demogorgon",	"Dmgrg",	10,	{ 200, 200, 200, 50, 75, 200, 100, 200, 200, 200, 100 },
	{ "berserk", "fast healing" },
	{ 22, 15, 15, 16, 22 },	{ 45, 45, 45, 45, 45 },	SIZE_LARGE,
	"{RFire{x","{DSlash Bash {YLightning{x","{BCold {gAcid {DPierce{x",
	{8,	5,	5,	5,	5}
    },

};


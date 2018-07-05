/****************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,			*
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.	*
 *																			*
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael			*
 *  Chastain, Michael Quan, and Mitchell Tse.								*
 *																			*
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
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "arena.h"
#include "interp.h"
#include "tables.h"
#include "olc.h"
#include "recycle.h"
#include "lookup.h"
#include "skill_tree.h"


struct	node*	combat()
{
	struct node* temp;

	temp = empty();
	temp->name =	"combat";
	temp->tn	=	"combat";
	temp->x1	=	offense();
	temp->x2	=	defense();
	temp->x3	=	special();
	temp->level	=	1;
	set_prefix(temp);
	return temp;
}
struct	node*	offense()
{
	struct node* temp;

	temp = empty();
	temp->name =	"offense";
	temp->tn	=	"combat.offense";
	temp->x1	=	slashing();
	temp->x2	=	bashing();
	temp->x3	=	piercing();
	temp->x4	=	unarmed();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}
struct	node*	defense()
{
	struct node* temp;

	temp = empty();
	temp->name =	"defense";
	temp->tn	=	"combat.defense";
	temp->x1	=	dodging();
	temp->x2	=	parrying();
	temp->x3	=	countering();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}
struct	node*	special()
{
	struct node* temp;

	temp = empty();
	temp->name =	"special";
	temp->tn	=	"combat.special";
	temp->x1	=	berserk();
	temp->x2	=	mastery();
	temp->x3	=	multihit();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}
struct	node*	slashing()
{
	struct node* temp;

	temp = empty();
	temp->name =	"slashing";
	temp->tn	=	"combat.offense.slashing";
	temp->x1	=	swords();
	temp->x2	=	axes();
	temp->x3	=	polearms();
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	bashing()
{
	struct node* temp;

	temp = empty();
	temp->name =	"bashing";
	temp->tn	=	"combat.offense.bashing";
	temp->x1	=	maces();
	temp->x2	=	flails();
	temp->x3	=	staves();
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	piercing()
{
	struct node* temp;

	temp = empty();
	temp->name =	"piercing";
	temp->tn	=	"combat.offense.piercing";
	temp->x1	=	daggers();
	temp->x2	=	spears();
	temp->x3	=	missiles();
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	unarmed()
{
	struct node* temp;

	temp = empty();
	temp->name =	"unarmed";
	temp->tn	=	"combat.offense.unarmed";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	dodging()
{
	struct node* temp;

	temp = empty();
	temp->name =	"dodging";
	temp->tn	=	"combat.defense.dodging";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	parrying()
{
	struct node* temp;

	temp = empty();
	temp->name =	"parrying";
	temp->tn	=	"combat.defense.parrying";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	countering()
{
	struct node* temp;

	temp = empty();
	temp->name =	"countering";
	temp->tn	=	"combat.defense.countering";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct node*	shield()
{
	struct node* temp;
	temp		= empty();
	temp->name	= "shield";
	temp->tn	= "combat.defnese.shield";
	temp->level	= 3;
	set_prefix(temp);
	return temp;
}
struct	node*	swords()
{
	struct node* temp;

	temp = empty();
	temp->name =	"swords";
	temp->tn	=	"combat.offense.slashing.swords";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	axes()
{
	struct node* temp;

	temp = empty();
	temp->name =	"axes";
	temp->tn	=	"combat.offense.slashing.axes";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	polearms()
{
	struct node* temp;

	temp = empty();
	temp->name =	"polearms";
	temp->tn	=	"combat.offense.slashing.polearms";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	maces()
{
	struct node* temp;

	temp = empty();
	temp->name =	"maces";
	temp->tn	=	"combat.offense.bashing.maces";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	flails()
{
	struct node* temp;

	temp = empty();
	temp->name =	"flails";
	temp->tn	=	"combat.offense.bashing.flails";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	staves()
{
	struct node* temp;

	temp = empty();
	temp->name =	"staves";
	temp->tn	=	"combat.offense.bashing.staves";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	daggers()
{
	struct node* temp;

	temp = empty();
	temp->name =	"daggers";
	temp->tn	=	"combat.offense.piercing.daggers";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	spears()
{
	struct node* temp;

	temp = empty();
	temp->name =	"spears";
	temp->tn	=	"combat.offense.piercing.spears";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	missiles()
{
	struct node* temp;

	temp = empty();
	temp->name =	"missiles";
	temp->tn	=	"combat.offense.piercing.missiles";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	multihit()
{
	struct node* temp;

	temp = empty();
	temp->name =	"multihit";
	temp->tn	=	"combat.special.multihit";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	mastery()
{
	struct node* temp;

	temp = empty();
	temp->name =	"mastery";
	temp->tn	=	"combat.special.mastery";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	berserk()
{
	struct node* temp;

	temp = empty();
	temp->name =	"berserk";
	temp->tn	=	"combat.special.berserk";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}


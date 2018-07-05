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

struct	node*	covert()
{
	struct node* temp;

	temp = empty();
	temp->name =	"covert";
	temp->tn	=	"covert";
	temp->x1	=	stealth();
	temp->x2	=	manipulation();
	temp->level	=	1;
	set_prefix(temp);
	return temp;
}
struct	node*	stealth()
{
	struct node* temp;

	temp = empty();
	temp->name =	"stealth";
	temp->tn	=	"covert.stealth";
	temp->x1	=	hiding();
	temp->x2	=	sneaking();
	temp->x3	=	stalking();
	temp->x5	=	agility();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}
struct	node*	manipulation()
{
	struct node* temp;

	temp = empty();
	temp->name =	"manipulation";
	temp->tn	=	"covert.manipulation";
	temp->x1	=	stealing();
	temp->x2	=	lockpicking();
	temp->x3	=	poisons();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}
struct	node*	stealing()
{
	struct node* temp;

	temp = empty();
	temp->name =	"stealing";
	temp->tn	=	"covert.manipulation.stealing";
	temp->x1	=	currency();
	temp->x2	=	inventory();
	temp->x3	=	equipped();
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	lockpicking()
{
	struct node* temp;

	temp = empty();
	temp->name =	"lockpicking";
	temp->tn	=	"covert.manipulation.lockpicking";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	poisons()
{
	struct node* temp;

	temp = empty();
	temp->name =	"poisons";
	temp->tn	=	"covert.manipulation.poisons";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	sneaking()
{
	struct node* temp;

	temp = empty();
	temp->name =	"sneaking";
	temp->tn	=	"covert.stealth.sneaking";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	hiding()
{
	struct node* temp;

	temp = empty();
	temp->name =	"hiding";
	temp->tn	=	"covert.stealth.hiding";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	stalking()
{
	struct node* temp;

	temp = empty();
	temp->name =	"stalking";
	temp->tn	=	"covert.stealth.stalking";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	agility()
{
	struct node* temp;

	temp = empty();
	temp->name =	"agility";
	temp->tn	=	"covert.stealth.agility";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	equipped()
{
	struct node* temp;

	temp = empty();
	temp->name =	"equipped";
	temp->tn	=	"covert.manipulation.stealing.equipped";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	inventory()
{
	struct node* temp;

	temp = empty();
	temp->name =	"inventory";
	temp->tn	=	"covert.manipulation.stealing.inventory";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	currency()
{
	struct node* temp;

	temp = empty();
	temp->name =	"currency";
	temp->tn	=	"covert.manipulation.stealing.currency";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}


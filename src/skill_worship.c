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

struct	node*	worship()
{
	struct node* temp;

	temp = empty();
	temp->name =	"worship";
	temp->tn	=	"worship";
	temp->x1	=	benediction();
	temp->x2	=	maladiction();
	temp->level	=	1;
	set_prefix(temp);
	return temp;
}

struct	node*	benediction()
{
	struct node* temp;

	temp = empty();
	temp->name =	"benediction";
	temp->tn	=	"worship.benediction";
	temp->x1	=	protection();
	temp->x2	=	healing();
	temp->x3	=	enhancement();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}

struct	node*	protection()
{
	struct node* temp;

	temp = empty();
	temp->name =	"protection";
	temp->tn	=	"worship.benediction.protection";
	temp->x1	=	w_b_p_physical();
	temp->x2	=	w_b_p_magical();
	temp->x3	=	w_b_p_spiritual();
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	healing()
{
	struct node* temp;

	temp = empty();
	temp->name =	"healing";
	temp->tn	=	"worship.benediction.healing";
	temp->x1	=	restoration();
	temp->x2	=	curative();
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	enhancement()
{
	struct node* temp;

	temp = empty();
	temp->name =	"enhancement";
	temp->tn	=	"worship.benediction.enhancement";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	maladiction()
{
	struct node* temp;

	temp = empty();
	temp->name =	"maladiction";
	temp->tn	=	"worship.maladiction";
	temp->x1	=	affliction();
	temp->x2	=	offensive();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}

struct	node*	affliction()
{
	struct node* temp;

	temp = empty();
	temp->name =	"affliction";
	temp->tn	=	"worship.maladiction.affliction";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	offensive()
{
	struct node* temp;

	temp = empty();
	temp->name =	"offensive";
	temp->tn	=	"worship.maladiction.offensive";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	w_b_p_spiritual()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"spiritual";
	temp->tn	=	"worship.benediction.protection.spiritual";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	w_b_p_magical()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"magical";
	temp->tn	=	"worship.benediction.protection.magical";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	w_b_p_physical()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"spirtual";
	temp->tn	=	"worship.benediction.protection.physical";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	curative()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"curative";
	temp->tn	=	"worship.benediction.healing.curative";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	restoration()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"restoration";
	temp->tn	=	"worship.benediction.healing.restoration";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	mind()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"mind";
	temp->tn	=	"worship.benediction.enhancement.mind";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}
struct	node*	body()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"body";
	temp->tn	=	"worship.benediction.enhancement.body";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}


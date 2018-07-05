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


struct	node*	arcane()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"arcane";
	temp->tn	=	"arcane";
	temp->x1	=	necromancy();
	temp->x2	=	greater_elemental();
	temp->x3	=	a_alteration();
	temp->x4	=	a_protection();
	temp->level	=	1;
	set_prefix(temp);
	return temp;
}
struct	node*	necromancy()
{
	struct node* temp;

	temp = empty();
	temp->name =	"necromancy";
	temp->tn	=	"arcane.necromancy";
	temp->x1	=	summoning();
	temp->x2	=	reanimation();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}
struct	node*	greater_elemental()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"greater elemental";
	temp->tn	=	"arcane.greater_elemental";
	temp->x1	=	a_g_fire();
	temp->x2	=	a_g_earth();
	temp->x3	=	a_g_wind();
	temp->x4	=	a_g_water();
	temp->x5	=	ether();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}
struct	node*	a_alteration()
{
	struct node* temp;

	temp = empty();
	temp->name =	"alteration";
	temp->tn	=	"arcane.alteration";
	temp->x1	=	a_a_object();
	temp->x2	=	a_a_person();
	temp->x3	=	a_a_immaterial();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}
struct	node*	a_protection()
{
	struct node* temp;

	temp = empty();
	temp->name =	"protection";
	temp->tn	=	"arcane.protection";
	temp->x1	=	a_p_physical();
	temp->x2	=	a_p_magical();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}
struct	node*	summoning()
{
	struct node* temp;

	temp = empty();
	temp->name =	"summoning";
	temp->tn	=	"arcane.necromancy.summoning";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	reanimation()
{
	struct node* temp;

	temp = empty();
	temp->name =	"reanimation";
	temp->tn	=	"arcane.necromancy.reanimation";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	a_g_fire()
{
	struct node* temp;

	temp = empty();
	temp->name =	"fire";
	temp->tn	=	"arcane.greater_elemental.fire";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	a_g_earth()
{
	struct node* temp;

	temp = empty();
	temp->name =	"earth";
	temp->tn	=	"arcane.greater_elemental.earth";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	a_g_water()
{
	struct node* temp;

	temp = empty();
	temp->name =	"water";
	temp->tn	=	"arcane.greater_elemental.water";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	a_g_wind()
{
	struct node* temp;

	temp = empty();
	temp->name =	"wind";
	temp->tn	=	"arcane.greater_elemental.wind";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	ether()
{
	struct node* temp;

	temp = empty();
	temp->name =	"ether";
	temp->tn	=	"arcane.greater_elemental.ether";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	a_a_object()
{
	struct node* temp;

	temp = empty();
	temp->name =	"object";
	temp->tn	=	"arcane.alteration.object";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	a_a_person()
{
	struct node* temp;

	temp = empty();
	temp->name =	"person";
	temp->tn	=	"arcane.alteration.person";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	a_a_immaterial()
{
	struct node* temp;

	temp = empty();
	temp->name =	"immaterial";
	temp->tn	=	"arcane.alteration.immaterial";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	a_p_physical()
{
	struct node* temp;

	temp = empty();
	temp->name =	"physical";
	temp->tn	=	"arcane.protection.physical";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}
struct	node*	a_p_magical()
{
	struct node* temp;

	temp = empty();
	temp->name =	"magical";
	temp->tn	=	"arcane.protection.magical";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

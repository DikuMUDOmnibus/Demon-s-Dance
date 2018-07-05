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


struct	node*	sorcery()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"sorcery";
	temp->tn	=	"sorcery";
	temp->x1	=	heraldic();
	temp->x2	=	wizardry();
	temp->level	=	1;
	set_prefix(temp);
	return temp;
}

struct	node*	heraldic()
{
	struct node* temp;

	temp = empty();
	temp->name	=	"heraldic";
	temp->tn	=	str_dup("sorcery.heraldic");
	temp->x1	=	detection();
	temp->x2	=	illusion();
	temp->x3	=	conjuration();
	temp->x4	=	transportation();
	temp->x5	=	enchantment();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}

struct	node*	detection()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.heraldic.detection";
	temp->name	=	"detection";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	illusion()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.heraldic.illusion";
	temp->name	=	"illusion";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	conjuration()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.heraldic.conjuration";
	temp->name	=	"conjuration";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	transportation()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.heraldic.transportation";
	temp->name	=	"transportation";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	enchantment()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.heraldic.enchantment";
	temp->name	=	"enchantment";
	temp->x1	=	s_i_e_person();
	temp->x2	=	s_i_e_object();
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	s_i_e_person()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.heraldic.enchantment.person";
	temp->name	=	"person";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}

struct	node*	s_i_e_object()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.heraldic.enchantment.object";
	temp->name	=	"object";
	temp->level	=	4;
	set_prefix(temp);
	return temp;
}

struct	node*	wizardry()
{
	struct node* temp;

	temp = empty();
	temp->tn	=	"sorcery.wizardry";
	temp->name	=	"wizardry";
	temp->x1	=	weather();
	temp->x2	=	elemental();
	temp->x3	=	psionic();
	temp->level	=	2;
	set_prefix(temp);
	return temp;
}

struct	node*	weather()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.wizardry.weather";
	temp->name	=	"weather";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	elemental()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.wizardry.elemental";
	temp->name	=	"elemental";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

struct	node*	psionic()
{
	struct node* temp;

	temp = empty();
	temp->tn =	"sorcery.wizardry.psionic";
	temp->name	=	"psionic";
	temp->level	=	3;
	set_prefix(temp);
	return temp;
}

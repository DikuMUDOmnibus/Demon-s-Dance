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
#include "olc.h"

#define DIF(a,b) (~((~a)|(b)))
/*stuff to set up key*/
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

/* provided to free strings */
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    free_string(field);			\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}
/*end stuff for key*/

/*****************************************************************************
 Name:		fwrite_rooms
 Purpose:	Save #ROOMS section of an area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void fwrite_rooms( FILE *fp, AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoomIndex;
    EXTRA_DESCR_DATA *pEd;
    EXIT_DATA *pExit;
    int iHash;
    int door;
	PROG_LIST *pRprog;

    fprintf( fp, "#ROOMSNEW\n" );
    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next )
        {
            if ( pRoomIndex->area == pArea )
            {
				fprintf( fp, "#ROOM\n" );
                fprintf( fp, "Vnum %d\n",		pRoomIndex->vnum );
                fprintf( fp, "Name %s~\n",		pRoomIndex->name );
                fprintf( fp, "Desc %s~\n",		fix_string( pRoomIndex->description ) );
                fprintf( fp, "Room_flags %d\n",		pRoomIndex->room_flags );
                fprintf( fp, "Sector_type %d\n",		pRoomIndex->sector_type );

                for ( pEd = pRoomIndex->extra_descr; pEd;
                      pEd = pEd->next )
                {
					fprintf( fp, "ExtraDesc\n");
					fprintf( fp, "%s~\n",pEd->keyword );
                    fprintf( fp, "%s~\n", fix_string( pEd->description ) );
                }
                for( door = 0; door < MAX_DIR; door++ )	/* I hate this! */
                {
                    if ( ( pExit = pRoomIndex->exit[door] )
                          && pExit->u1.to_room )
                    {
						int locks = 0;
						if ( IS_SET( pExit->rs_flags, EX_ISDOOR ) 
						&& ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) ) 
		    				&& ( !IS_SET( pExit->rs_flags, EX_NOPASS ) ) )
							locks = 1;
						if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
						&& ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
							&& ( !IS_SET( pExit->rs_flags, EX_NOPASS ) ) )
							locks = 2;
						if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
						&& ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
							&& ( IS_SET( pExit->rs_flags, EX_NOPASS ) ) )
							locks = 3;
						if ( IS_SET( pExit->rs_flags, EX_ISDOOR )
						&& ( IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
							&& ( IS_SET( pExit->rs_flags, EX_NOPASS ) ) )
							locks = 4;
                        fprintf( fp, "Exit\n");
						fprintf( fp, "%d\n",	pExit->orig_door );
                        fprintf( fp, "%s~\n",	fix_string( pExit->description ) );
                        fprintf( fp, "%s~\n",	pExit->keyword );
                        fprintf( fp, "%d\n",	locks);
						fprintf( fp, "%d\n",	pExit->key);
                        fprintf( fp, "%d\n",	pExit->u1.to_room->vnum );
						fprintf( fp, "%s~\n",	pExit->exit_string );
                    }
                }
				if (pRoomIndex->mana_rate != 100 || pRoomIndex->heal_rate != 100)
				 fprintf ( fp, "ManaRate %d\n",pRoomIndex->mana_rate);
				 fprintf ( fp, "HealRate %d\n", pRoomIndex->heal_rate);
				if (pRoomIndex->clan > 0)
				 fprintf ( fp, "Clan %s~\n" , clan_table[pRoomIndex->clan].name );
		 						 
				if (!IS_NULLSTR(pRoomIndex->owner))
				 fprintf ( fp, "Owner %s~\n" , pRoomIndex->owner );

				for (pRprog = pRoomIndex->rprogs; pRprog; pRprog = pRprog->next)
				{
					 fprintf(fp, "R %s %d %s~\n",
					 prog_type_to_name(pRprog->trig_type), pRprog->vnum,
					 pRprog->trig_phrase);
				}

				fprintf( fp, "EndRoom\n\n" );
            }
        }
    }
    fprintf( fp, "#END\n\n\n\n" );
    return;
}

/*****************************************************************************
 Name:		fwrite_objects
 Purpose:	Save one object to file.
                new ROM format saving -- Hugin
 Called by:	save_objects (below).
 ****************************************************************************/
void fwrite_objects( FILE *fp, AREA_DATA *pArea )
{
    char letter;
    AFFECT_DATA *pAf;
    EXTRA_DESCR_DATA *pEd;
    char buf[MAX_STRING_LENGTH];
	PROG_LIST *pOprog;
	OBJ_INDEX_DATA *pObjIndex;
	int i;

    fprintf( fp, "#OBJECTSNEW\n" );

    for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
    {
		if ( (pObjIndex = get_obj_index( i )) )
		{
			fprintf( fp, "#OBJ\n" );
			fprintf( fp, "Vnum %d\n",    pObjIndex->vnum );
			fprintf( fp, "Name %s~\n",		pObjIndex->name );
			fprintf( fp, "Short %s~\n",    pObjIndex->short_descr );
			fprintf( fp, "Orig_short %s~\n",	 pObjIndex->orig_short	);
			fprintf( fp, "Desc %s~\n",    fix_string( pObjIndex->description ) );
			fprintf( fp, "Material %s~\n",    pObjIndex->material );
			fprintf( fp, "Item_type %s\n",      item_name(pObjIndex->item_type));
			fprintf( fp, "Extra %s\n",      fwrite_flag( pObjIndex->extra_flags, buf ) );
			fprintf( fp, "Wear %s\n",     fwrite_flag( pObjIndex->wear_flags,  buf ) );

			/*
			 *  Using fwrite_flag to write mostValues gives a strange
			 *  looking area file, consider making a case for each
			 *  item type later.
			 */

			switch ( pObjIndex->item_type )
			{
				default:
				fprintf( fp, "Values %s ",  fwrite_flag( pObjIndex->value[0], buf ) );
				fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[1], buf ) );
				fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[2], buf ) );
				fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[3], buf ) );
				fprintf( fp, "%s\n", fwrite_flag( pObjIndex->value[4], buf ) );
				break;

				case ITEM_DRINK_CON:
				case ITEM_FOUNTAIN:
					fprintf( fp, "Values %d %d '%s' %d %d\n",
							 pObjIndex->value[0],
							 pObjIndex->value[1],
							 liq_table[pObjIndex->value[2]].liq_name,
					 pObjIndex->value[3],
					 pObjIndex->value[4]);
					break;

				case ITEM_SHEATH:
					fprintf( fp, "Values %d\n",pObjIndex->value[0] );
					break;

				case ITEM_CONTAINER:
				case ITEM_GOLEM_BAG:
					fprintf( fp, "Values %d %s %d %d %d\n",
							 pObjIndex->value[0],
							 fwrite_flag( pObjIndex->value[1], buf ),
							 pObjIndex->value[2],
							 pObjIndex->value[3],
							 pObjIndex->value[4]);
					break;

				 case ITEM_GOLEM_PART:
					fprintf( fp, "Values %s\n",
						golem_name(pObjIndex->value[0]) );
					break;

				case ITEM_SMITH_ORE:
					fprintf( fp, "Values %d\n",
						pObjIndex->value[0] );
					break;

				case ITEM_WEAPON:
					 if(IS_WEAPON_STAT(pObjIndex,WEAPON_TWO_HANDS))
					 {
						 fprintf( fp, "Values %s %d %d %s %s\n",
							 weapon_name(pObjIndex->value[0]),
							 pObjIndex->value[1],
							 pObjIndex->value[2],
							 attack_table[pObjIndex->value[3]].name,
							 fwrite_flag( pObjIndex->value[4], buf ) );
					 }
					 else
					 {
						 fprintf( fp, "Values %s %d %d %s %s\n",
							 weapon_name(pObjIndex->value[0]),
							 pObjIndex->value[1],
							 pObjIndex->value[2],
							 attack_table[pObjIndex->value[3]].name,
							 fwrite_flag( pObjIndex->value[4], buf ) );
					 }
					 if ( pObjIndex->weapon_spell != -1
						 && pObjIndex->weapon_spell != 0)
					 {					
						 fprintf( fp, "Weapon_spell %s\n",
							 pObjIndex->weapon_spell != -1 ?
							 skill_table[pObjIndex->weapon_spell].name
							 : "");
					 }
					 break;
            
				case ITEM_PILL:
				case ITEM_POTION:
				case ITEM_SCROLL:
				fprintf( fp, "Values %d '%s' '%s' '%s' '%s'\n",
					 pObjIndex->value[0] > 0 ? /* no negative numbers */
					 pObjIndex->value[0]
					 : 0,
					 pObjIndex->value[1] != -1 ?
					 skill_table[pObjIndex->value[1]].name
					 : "",
					 pObjIndex->value[2] != -1 ?
					 skill_table[pObjIndex->value[2]].name
					 : "",
					 pObjIndex->value[3] != -1 ?
					 skill_table[pObjIndex->value[3]].name
					 : "",
					 pObjIndex->value[4] != -1 ?
					 skill_table[pObjIndex->value[4]].name
					 : "");
				break;

				case ITEM_STAFF:
				case ITEM_WAND:
				fprintf( fp, "Values %d %d %d '%s' %d\n",
	    					pObjIndex->value[0],
	    					pObjIndex->value[1],
	    					pObjIndex->value[2],
	    					pObjIndex->value[3] != -1 ?
	    						skill_table[pObjIndex->value[3]].name :
	    						"",
	    					pObjIndex->value[4] );
				break;
				
				case ITEM_QUIVER:
					fprintf( fp, "Values %d ", pObjIndex->value[0] );
					fprintf( fp, "%d ", pObjIndex->value[1] );
					fprintf( fp, "%d 0 0\n", pObjIndex->value[2]);
					break;

	  			case ITEM_ARROW:
					fprintf( fp, "Values 0 %d ", pObjIndex->value[1] );
					fprintf( fp, "%d 0 0\n", pObjIndex->value[2]);
				break;

			}

			fprintf( fp, "Level %d\n", pObjIndex->level );
			fprintf( fp, "Weight %d\n", pObjIndex->weight );
			fprintf( fp, "Cost %d\n", pObjIndex->cost );

				 if ( pObjIndex->condition >  90 ) letter = 'P';
			else if ( pObjIndex->condition >  75 ) letter = 'G';
			else if ( pObjIndex->condition >  50 ) letter = 'A';
			else if ( pObjIndex->condition >  25 ) letter = 'W';
			else if ( pObjIndex->condition >  10 ) letter = 'D';
			else if ( pObjIndex->condition >   0 ) letter = 'B';
			else                                   letter = 'R';

			fprintf( fp, "Condition %c\n", letter );

			for( pAf = pObjIndex->affected; pAf; pAf = pAf->next )
			{
			if (pAf->where == TO_OBJECT || pAf->bitvector == 0)
					fprintf( fp, "AffectObj\n%d %d\n",  pAf->location, pAf->modifier );
			else
			{
				fprintf( fp, "F\n" );

				switch(pAf->where)
				{
					case TO_AFFECTS:
						fprintf( fp, "Faff " );
						break;
					case TO_IMMUNE:
						fprintf( fp, "Fimm " );
						break;
					case TO_RESIST:
						fprintf( fp, "Fres " );
						break;
					case TO_VULN:
						fprintf( fp, "Fvul " );
						break;
					default:
						bug( "olc_save: Invalid Affect->where", 0);
						break;
				}
				
				fprintf( fp, "%d %d %s\n", pAf->location, pAf->modifier,
						fwrite_flag( pAf->bitvector, buf ) );
			}
			}

			for( pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next )
			{
				fprintf( fp, "ExtraDesc\n%s~\n%s~\n", pEd->keyword,
				 fix_string( pEd->description ) );
			}
			for (pOprog = pObjIndex->oprogs; pOprog; pOprog = pOprog->next)
			{
				fprintf(fp, "O %s %d %s~\n",
				prog_type_to_name(pOprog->trig_type), pOprog->vnum,
						pOprog->trig_phrase);
			}
			fprintf( fp, "EndObj\n\n" );
}
}
	fprintf( fp, "#END\n\n\n\n" );
    return;
}
 
/*****************************************************************************
 Name:		fwrite_mobiles
 Purpose:	Save #MOBILES secion of an area file.
 Called by:	save_area(olc_save.c).
 Notes:         Changed for ROM OLC.
 ****************************************************************************/
void fwrite_mobiles( FILE *fp, AREA_DATA *pArea )
{
    int i;
    MOB_INDEX_DATA *pMobIndex;
    PROG_LIST *pMprog;
    char buf[MAX_STRING_LENGTH];
    long temp;

    fprintf( fp, "#MOBILESNEW\n" );

    for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
    {
		if ( (pMobIndex = get_mob_index( i )) )
		{
			sh_int race = pMobIndex->race;
			fprintf( fp, "#MOB\n" );
			fprintf( fp, "Vnum %d\n",	pMobIndex->vnum );
			fprintf( fp, "Name %s~\n",	pMobIndex->player_name );
			fprintf( fp, "Short %s~\n",	pMobIndex->short_descr );
			fprintf( fp, "Long %s~\n",	fix_string( pMobIndex->long_descr ) );
			fprintf( fp, "Description %s~\n",	fix_string( pMobIndex->description) );
			fprintf( fp, "Race %s~\n",	race_table[race].name );
			fprintf( fp, "Act_flags %s\n",		fwrite_flag( pMobIndex->act,		buf ) );
			fprintf( fp, "Aff %s\n",		fwrite_flag( pMobIndex->affected_by,	buf ) );
			fprintf( fp, "Aff2 %s\n",		fwrite_flag( pMobIndex->affected2_by,	buf ) );
			fprintf( fp, "Align %d\n",	pMobIndex->alignment);
			fprintf( fp, "Group %d\n", pMobIndex->group);
			fprintf( fp, "Level %d\n",		pMobIndex->level );
			fprintf( fp, "Hitroll %d\n",		pMobIndex->hitroll );
			fprintf( fp, "Hit_dice %dd%d+%ld\n",	pMobIndex->hit[DICE_NUMBER], 
						pMobIndex->hit[DICE_TYPE], 
						pMobIndex->hit[DICE_BONUS] );
			fprintf( fp, "Mana_dice %dd%d+%d\n",	pMobIndex->mana[DICE_NUMBER], 
						pMobIndex->mana[DICE_TYPE], 
						pMobIndex->mana[DICE_BONUS] );
			fprintf( fp, "Dam_dice %dd%d+%d\n",	pMobIndex->damage[DICE_NUMBER], 
						pMobIndex->damage[DICE_TYPE], 
						pMobIndex->damage[DICE_BONUS] );
			fprintf( fp, "Dam_type %s\n",	attack_table[pMobIndex->dam_type].name );
			fprintf( fp, "Armor %d %d %d %d\n",
						pMobIndex->ac[AC_PIERCE] / 10, 
						pMobIndex->ac[AC_BASH]   / 10, 
						pMobIndex->ac[AC_SLASH]  / 10, 
						pMobIndex->ac[AC_EXOTIC] / 10 );
			fprintf( fp, "Off %s\n",		fwrite_flag( pMobIndex->off_flags,  buf ) );
			if ((!pMobIndex->teach_flags))
			{
				fprintf( fp, "Teach A\n",buf );
			}
			else
			{
				fprintf( fp, "Teach %s\n",		fwrite_flag( pMobIndex->teach_flags,  buf ) );		
			}
			fprintf( fp, "Imm %s\n",		fwrite_flag( pMobIndex->imm_flags,  buf ) );
			fprintf( fp, "Res %s\n",		fwrite_flag( pMobIndex->res_flags,  buf ) );
			fprintf( fp, "Vuln %s\n",	fwrite_flag( pMobIndex->vuln_flags, buf ) );
			fprintf( fp, "Start_pos %s\n",	position_table[pMobIndex->start_pos].short_name);
			fprintf( fp, "Default_pos %s\n", position_table[pMobIndex->default_pos].short_name);
			fprintf( fp, "Sex %s\n",sex_table[pMobIndex->sex].name);
			fprintf( fp, "Wealth %ld\n",pMobIndex->wealth );
			fprintf( fp, "Form %s\n",		fwrite_flag( pMobIndex->form,  buf ) );
			fprintf( fp, "Parts %s\n",		fwrite_flag( pMobIndex->parts, buf ) );

			fprintf( fp, "Size %s\n",		size_table[pMobIndex->size].name );
			fprintf( fp, "Material %s\n",	IS_NULLSTR(pMobIndex->material) ? pMobIndex->material : "unknown" );

			
			
			if (pMobIndex->clan)  /* Feydrex - For clan hall guards */
				fprintf( fp, "C %s~\n", clan_table[pMobIndex->clan].name );

			if ((temp = DIF(race_table[race].act,pMobIndex->act)))
     			fprintf( fp, "Fact %s\n", fwrite_flag(temp, buf) );

			if ((temp = DIF(race_table[race].aff,pMobIndex->affected_by)))
     			fprintf( fp, "Faff1 %s\n", fwrite_flag(temp, buf) );

			if ((temp = DIF(race_table[race].off,pMobIndex->off_flags)))
     			fprintf( fp, "Foff %s\n", fwrite_flag(temp, buf) );

			if ((temp = DIF(race_table[race].imm,pMobIndex->imm_flags)))
     			fprintf( fp, "Fimm %s\n", fwrite_flag(temp, buf) );

			if ((temp = DIF(race_table[race].res,pMobIndex->res_flags)))
     			fprintf( fp, "Fres %s\n", fwrite_flag(temp, buf) );

			if ((temp = DIF(race_table[race].vuln,pMobIndex->vuln_flags)))
     			fprintf( fp, "Fvul %s\n", fwrite_flag(temp, buf) );

			if ((temp = DIF(race_table[race].form,pMobIndex->form)))
     			fprintf( fp, "Ffor %s\n", fwrite_flag(temp, buf) );

			if ((temp = DIF(race_table[race].parts,pMobIndex->parts)))
    			fprintf( fp, "Fpar %s\n", fwrite_flag(temp, buf) );

			for (pMprog = pMobIndex->mprogs; pMprog; pMprog = pMprog->next)
			{
				fprintf(fp, "MobTrig %s %d %s~\n",
				prog_type_to_name(pMprog->trig_type), pMprog->vnum,
						pMprog->trig_phrase);
			}
			fprintf( fp, "EndMob\n\n" );
		}
	}

    fprintf( fp, "#END\n\n\n\n" );
    return;
}

/*****************************************************************************
 Name:		fwrite_area
 Purpose:	Save an area, note that this format is new.
 Called by:	do_asave(olc_save.c).
 ****************************************************************************/
void fwrite_area( AREA_DATA *pArea )
{
    FILE *fp;
	char buf[MSL];

	fclose( fpReserve );
    sprintf(buf, "%s%s", NEW_AREA_DIR,pArea->file_name);
	wiznet(buf,NULL,NULL,0,0,0);
	if ( !( fp = fopen( buf, "w" ) ) )
    {
		bug( "fwrite_area: fopen", 0 );
		perror( pArea->file_name );
		return;
    }

    fprintf( fp, "#AREADATA\n" );
    fprintf( fp, "Name %s~\n",        pArea->name );
    fprintf( fp, "Builders %s~\n",        fix_string( pArea->builders ) );
    fprintf( fp, "VNUMs %d %d\n",      pArea->min_vnum, pArea->max_vnum );
    fprintf( fp, "Credits %s~\n",	 pArea->credits );
    fprintf( fp, "Security %d\n",         pArea->security );
    fprintf( fp, "End\n\n\n\n" );

    fwrite_mobiles( fp, pArea );
    fwrite_objects( fp, pArea );
    fwrite_rooms( fp, pArea );
    save_specials( fp, pArea );
    save_resets( fp, pArea );
    save_shops( fp, pArea );
    save_mobprogs( fp, pArea );
    save_objprogs( fp, pArea );
    save_roomprogs( fp, pArea );

    if ( pArea->helps && pArea->helps->first )
	save_helps( );

    fprintf( fp, "#$\n" );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


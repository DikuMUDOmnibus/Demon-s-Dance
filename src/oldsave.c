void fread_objects( FILE *fp )
{
    OBJ_INDEX_DATA *pObjIndex;
    char *word;
    bool fMatch;
	char buf[MSL];
 
    if ( !area_last )   /* OLC */
    {
        bug( "Load_objects: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        int vnum;
        char letter;
        int iHash;
 
        letter                          = fread_letter( fp );
        if ( letter != '#' && letter != '{' )
        {
            bug( "Load_objects: #/{ not found.", 0 );
            exit( 1 );
        }
 
        vnum                            = fread_number( fp );
        if ( vnum == 0 )
            break;
 
        fBootDb = FALSE;
        if ( get_obj_index( vnum ) != NULL )
        {
            bug( "Load_objects: vnum %d duplicated.", vnum );
            exit( 1 );
        }
        fBootDb = TRUE;
 
        pObjIndex                       = alloc_perm( sizeof(*pObjIndex) );
        pObjIndex->vnum                 = vnum;
        pObjIndex->area                 = area_last;            /* OLC */
        pObjIndex->new_format           = TRUE;
		pObjIndex->reset_num		= 0;
		newobjs++;
	for ( ; ; )
    {
		word   = feof( fp ) ? "End" : fread_word( fp );
		fMatch = FALSE;

		switch ( UPPER(word[0]) )
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol( fp );
			break;
		case '}':
			break;
		case 'A':
		case 'C':
			KEY( "Cost",		pObjIndex->cost,			fread_number( fp ) );
			if (!str_cmp( word, "Condition"))
			{
				letter 				= fread_letter( fp );
				switch (letter)
 				{
					case ('P') :		pObjIndex->condition = 100; break;
					case ('G') :		pObjIndex->condition =  90; break;
					case ('A') :		pObjIndex->condition =  75; break;
					case ('W') :		pObjIndex->condition =  50; break;
					case ('D') :		pObjIndex->condition =  25; break;
					case ('B') :		pObjIndex->condition =  10; break;
 					case ('R') :		pObjIndex->condition =   0; break;
					default:			pObjIndex->condition = 100; break;
				}
			}
			break;
		case 'D':
			KEY( "Desc",		pObjIndex->description,		fread_string( fp ) );
			break;
		case 'E':
			KEY( "Extra",		pObjIndex->extra_flags,		fread_flag( fp ) );
			break;
		case 'I':
			if (!str_cmp( word, "Item_type"))
			{
				CHECK_POS(pObjIndex->item_type, item_lookup(fread_word( fp )), "item_type" );
			}
			break;
		case 'L':
			KEY( "Level",		pObjIndex->level,			fread_number( fp ) );
			break;
		case 'M':
			KEY( "Material",	pObjIndex->material,		fread_string( fp ) );
			break;
		case 'N':
			KEY( "Name",		pObjIndex->name,			fread_string( fp ) );
			break;
		case 'O':
			KEY( "Orig_short",	pObjIndex->orig_short,		fread_string( fp ) );
			break;
		case 'S':
			KEY( "Short",		pObjIndex->short_descr,		fread_string( fp ) );
			break;
		case 'W':
			KEY( "Wear",		pObjIndex->wear_flags,		fread_flag( fp ) );
			break;
			KEY( "Weight",		pObjIndex->weight,			fread_number( fp ) );
			break;
		case 'V':
			if (!str_cmp( word, "Value"))
			{
				switch(pObjIndex->item_type)
				{
				case ITEM_WEAPON:
					pObjIndex->value[0]		= weapon_type(fread_word(fp));
					pObjIndex->value[1]		= fread_number(fp);
					pObjIndex->value[2]		= fread_number(fp);
					pObjIndex->value[3]		= attack_lookup(fread_word(fp));
					pObjIndex->value[4]		= fread_flag(fp);
					break;

				case ITEM_GOLEM_PART:
					pObjIndex->value[0]		= golem_type(fread_word(fp));
					break;

				case ITEM_SMITH_ORE:
					pObjIndex->value[0]		= fread_number(fp);
					break;

				case ITEM_SHEATH:
					pObjIndex->value[0]		= fread_number(fp);
					break;

				case ITEM_CONTAINER:
				case ITEM_GOLEM_BAG:
					pObjIndex->value[0]		= fread_number(fp);
					pObjIndex->value[1]		= fread_flag(fp);
					pObjIndex->value[2]		= fread_number(fp);
					pObjIndex->value[3]		= fread_number(fp);
					pObjIndex->value[4]		= fread_number(fp);
					break;

				case ITEM_DRINK_CON:
				case ITEM_FOUNTAIN:
						pObjIndex->value[0]         = fread_number(fp);
						pObjIndex->value[1]         = fread_number(fp);
					CHECK_POS(pObjIndex->value[2], liq_lookup(fread_word(fp)), "liq_lookup" );
						pObjIndex->value[3]         = fread_number(fp);
						pObjIndex->value[4]         = fread_number(fp);
						break;
				case ITEM_WAND:
				case ITEM_STAFF:
					pObjIndex->value[0]		= fread_number(fp);
					pObjIndex->value[1]		= fread_number(fp);
					pObjIndex->value[2]		= fread_number(fp);
					pObjIndex->value[3]		= skill_lookup(fread_word(fp));
					pObjIndex->value[4]		= fread_number(fp);
					break;
				case ITEM_POTION:
				case ITEM_PILL:
				case ITEM_SCROLL:
 					pObjIndex->value[0]		= fread_number(fp);
					pObjIndex->value[1]		= skill_lookup(fread_word(fp));
					pObjIndex->value[2]		= skill_lookup(fread_word(fp));
					pObjIndex->value[3]		= skill_lookup(fread_word(fp));
					pObjIndex->value[4]		= skill_lookup(fread_word(fp));
					break;
				
				case ITEM_QUIVER:
				case ITEM_ARROW:
						pObjIndex->value[0]		= fread_number(fp);
						pObjIndex->value[1]		= fread_number(fp);
						pObjIndex->value[2]		= fread_number(fp);
						pObjIndex->value[3]		= fread_number(fp);
						pObjIndex->value[4]		= fread_number(fp);
						break;

				default:
						pObjIndex->value[0]             = fread_flag( fp );
						pObjIndex->value[1]             = fread_flag( fp );
						pObjIndex->value[2]             = fread_flag( fp );
						pObjIndex->value[3]             = fread_flag( fp );
					pObjIndex->value[4]		    = fread_flag( fp );
					break;
				}
			}
			break;


		for ( ; ; )
        {
            char letter;
 
            letter = fread_letter( fp );
 
            if ( letter == 'A' )
            {
				fread_affects( fp, pObjIndex );
            }

			else if (letter == 'F')
            {
				fread_affects( fp, pObjIndex );
			} 
            else if ( letter == 'E' )
            {
				fread_extra_desc( fp, pObjIndex );
            }
			else if ( letter == 'O' )
			{
				PROG_LIST *pOprog;
				char *word;
				int trigger = 0;

				pOprog			= alloc_perm(sizeof(*pOprog));
				word			= fread_word( fp );
				if ( !(trigger = flag_lookup( word, oprog_flags )) )
				{
					bug( "OBJprogs: invalid trigger.",0);
					exit(1);
				}
				SET_BIT( pObjIndex->oprog_flags, trigger );
				pOprog->trig_type	= trigger;
				pOprog->vnum	 	= fread_number( fp );
				pOprog->trig_phrase	= fread_string( fp );
				pOprog->next		= pObjIndex->oprogs;
				pObjIndex->oprogs	= pOprog;
			}
            else
            {
                ungetc( letter, fp );
                break;
            }
        }
        iHash                   = vnum % MAX_KEY_HASH;
        pObjIndex->next         = obj_index_hash[iHash];
        obj_index_hash[iHash]   = pObjIndex;
        top_obj_index++;
        top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;   /* OLC */
        assign_area_vnum( vnum );                                   /* OLC */
    }
	}
	}
    return;
}
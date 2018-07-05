/****************************************************************************
*	Demon's Dance MUD, and source code are property of Eric Goetschalckx	*
*	By compiling this code, you agree to include the following in your		*
*	login screen:															*
*	    Derivative of Demon's Dance, by Enzo/Stan							*
***************************************************************************/

void	log_clan	args( ( const char *str ) );
extern	const	struct	flag_type	clan_flags		[ ];
extern	const	struct	flag_type	rank_flags		[ ];
extern		CLAN_DATA	  *	clan_first;

/*Rank Definitions*/
#define CLAN_PLAIN                    0
#define CLAN_NOKILL                   1
#define CLAN_ORDER                    2
#define CLAN_GUILD                    3

#define	RANK_EXILED					1		/* Exiled from a clan   */
#define	RANK_CLANSMAN				2		/* Foot soldier         */
#define	RANK_CLANHERO				3		/* Knight               */
#define	RANK_SUBCHIEF				4		/* Knight Lord          */
#define	RANK_CHIEFTAIN				5		/* Second in command    */
#define	RANK_OVERLORD				6		/* Head clan leader     */

struct  clan_data
{
    CLAN_DATA *	next;           /* next clan in list                    */
    char *	filename;       /* Clan filename                        */
    char *	who_name;       /* Clan who name                        */
    char *	name;           /* Clan name                            */
    char *	motto;          /* Clan motto                           */
    char *	description;    /* A brief description of the clan      */
    char *	overlord;       /* Head clan leader                     */
    char *	chieftain;      /* Second in command                    */
    int		pkills;         /* Number of pkills on behalf of clan   */
    int		pdeaths;        /* Number of pkills against clan        */
    int		mkills;         /* Number of mkills on behalf of clan   */
    int		mdeaths;        /* Number of clan deaths due to mobs    */
    int		illegal_pk;     /* Number of illegal pk's by clan       */
    int		score;          /* Overall score                        */
    int		clan_type;      /* See clan type defines                */
    int		subchiefs;      /* Number of subchiefs                  */
    int		clanheros;      /* Number of clanheros                  */
    int		members;        /* Number of clan members               */
    int		clanobj1;       /* Vnum of first clan obj (ring)        */
    int		clanobj2;       /* Vnum of second clan obj (shield)     */
    int		clanobj3;       /* Vnum of third clan obj (weapon)      */
    int		recall;         /* Vnum of clan's recall room           */
    int		donation;       /* Vnum of clan's donation pit          */
};
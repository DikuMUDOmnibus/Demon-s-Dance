/*******************************************************************************
 *         _               | File Name:   custom_slay.txt
 *        / \      _-'     | Description: A new customizable Slay
 *      _/|  \-''- _ /     |              It allows an immortal with the slay command
 * __-' |          \       |              to set hisown/herown slay messages. It allows
 *     /              \    |              for multiple generic and user specific slay
 *     /       "o.  |o |   |              messages.
 *     |            \ ;    |              
 *                   ',    |
 *        \_         __\   | (c) 2000-2001 TAKA 
 *          ''-_    \.//   | (c) 2000-2001 The GhostMud Project Team
 *            / '-____'    | 
 *           /             | You may use this code under ROM license restriction 
 *         _'  The Wolf    | 1) This header block remains in the code.          
 *       _-'   strikes!    | 2) You email me at a_ghost_dancer@excite.com
 *_________________________|    letting me know you are using this code
 *                              please incluse your name, your mud name
 * All rights reserved          your mud address, your email and this file
 * GhostMud is copyrighted      name.                                         
 * by TAKA                   3) In your help files mention me where appropriate 
 *                              IE: help snippets.
 *********************************************************************************/
/****************************************************************************
*	Demon's Dance MUD, and source code are property of Eric Goetschalckx	*
*	By compiling this code, you agree to include the following in your		*
*	login screen:															*
*	    Derivative of Demon's Dance, by Enzo/Stan							*
***************************************************************************/

/*
 * 010501
 * (c) TAKA 2001
 * slay messages get saved here in this list file
 *
 * Most of these flags for your mud are not even used but my release V2.1 uses
 * all of them and will be at some point available to the general public. However
 * at this time i have not released that code.
 *
 * You should include them if you wish but becareful a few are used in my code
 * and the SLAY_MESSAGE_LIST should be used always.
 */

#define SLAY_MESSAGE_LIST	"../text/slay.txt" 	/* List of petitions to clans */
#define ALLOW_MULTI_SLAY	1 /* Allow Multi Slay */
#define ALLOW_CUSTOM_SLAY	1 /* Allow immortals to make their own custom slays */
#define CUSTOM_SLAY_COUNT	1 /* Number of custom slays per person */
#define CUSTOM_SLAY_LEVEL	102 /* Level to allow custom slays */
#define ALLOW_SHARE_CUSTOM	0 /* Allow other immortals to use someone elses custom slay */


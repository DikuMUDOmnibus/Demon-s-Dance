typedef	struct	node*	NODE	args( (void) );

/* skill_tree.c */
void		display_node							args( ( struct node* skill,CHAR_DATA *ch, CHAR_DATA *victim ) );
void		set_prefix								args( ( struct node* skill) );
char		*pull_skill								args( ( char *argument, char *arg_first ) );
void		print_node								args( ( struct node* skill ) );
void		set_skill_levels						args( ( CHAR_DATA *ch, int level) );
int			return_skill_sn							args( ( CHAR_DATA *ch, int sn) );
int			return_skill_str						args( ( CHAR_DATA *ch, char* skill_name) );
int			return_bonus_sn							args( ( CHAR_DATA *ch, int sn) );
int			return_bonus_str						args( ( CHAR_DATA *ch, char* skill_name) );
int			talent_level							args( ( CHAR_DATA *ch, char **tokens) );
int			talent_bonus							args( ( CHAR_DATA *ch, char **tokens) );
void		set_node_tn								args( (struct node* node,char * tn) );
void		boost_skill								args( ( CHAR_DATA *ch, char *skill_name,int ammount) );
int			calc_skill_cost							args( (CHAR_DATA *ch,char *skill_name,int level) );
void		set_bonus								args( (CHAR_DATA *ch,char *skill_name) );
void		update_bonus							args( (CHAR_DATA *ch,struct node *node) );
int			calc_skill_cost							args( (CHAR_DATA *ch,char *skill_name,int level) );
int			match_skill								args( ( const char *name, char * called_by ) );
void		set_parent_avg							args( (CHAR_DATA *ch,struct node *node) );
void		convert_char							args( (CHAR_DATA *ch));
int			weapon_talent							args( (int v0) );
int			sn_talent								args( (int sn) );
struct		node*		check_skill_tree			args( (char *first,char *arg,CHAR_DATA *ch,struct node *node) );
void		gain_sp									args( ( CHAR_DATA *ch, CHAR_DATA *victim) );

/* skill_table.c */
extern		struct				tree_skill_type		tree_skill_table	[MAX_TREE_SKILLS];
extern		const	struct		skill_stat_table	check_skill_group	[SKILL_GROUPS];

struct node
{
	char *	name;
	struct	node *x1;
	struct	node *x2;
	struct	node *x3;
	struct	node *x4;
	struct	node *x5;
	char *	prefix;
	char *	tn;	/*name which is used in fucntions, such as upgrade*/
	short	level;
};

struct node_type
{
    char *	name;
};

struct skill_stat_table
{
    char *	name;
	int		skill;
	int		stat;
};

struct	tree_skill_type
{
	char * name;
};

NODE(talloc);
NODE(empty);
NODE(uber);
NODE(combat);
	NODE(offense);
		NODE(slashing);
			NODE(swords);
			NODE(axes);
			NODE(polearms);
		NODE(bashing);
			NODE(maces);
			NODE(flails);
			NODE(staves);
		NODE(piercing);
			NODE(daggers);
			NODE(spears);
			NODE(missiles);
		NODE(unarmed);
	NODE(defense);
		NODE(dodging);
		NODE(parrying);
		NODE(countering);
		NODE(shield);
	NODE(special);
		NODE(berserk);
		NODE(mastery);
		NODE(multihit);

NODE(covert);
	NODE(stealth);
		NODE(stalking);
		NODE(hiding);
		NODE(sneaking);
		NODE(agility);
	NODE(manipulation);
		NODE(stealing);
			NODE(currency);
			NODE(inventory);
			NODE(equipped);
		NODE(lockpicking);
		NODE(poisons);

NODE(arcane);
	NODE(necromancy);
		NODE(summoning);
		NODE(reanimation);
	NODE(greater_elemental);
		NODE(a_g_fire);
		NODE(a_g_water);
		NODE(a_g_wind);
		NODE(a_g_earth);
		NODE(ether);
	NODE(a_alteration);
		NODE(a_a_object);
		NODE(a_a_person);
		NODE(a_a_immaterial);
	NODE(a_protection);
		NODE(a_p_magical);
		NODE(a_p_physical);

NODE(worship);
	NODE(benediction);
		NODE(protection);
			NODE(w_b_p_physical);
			NODE(w_b_p_magical);
			NODE(w_b_p_spiritual);
		NODE(healing);
			NODE(restoration);
			NODE(curative);
		NODE(enhancement);
			NODE(mind);
			NODE(body);
	NODE(maladiction);
		NODE(affliction);
		NODE(offensive);

NODE(sorcery);
     NODE(heraldic);
		NODE(detection);
		NODE(illusion);
		NODE(conjuration);
		NODE(transportation);
		NODE(enchantment);
			NODE(s_i_e_person);
			NODE(s_i_e_object);
	NODE(wizardry);
		NODE(elemental);
		NODE(weather);
		NODE(psionic);

/*
 * Enumeration of all the skills for indexes
 * within talent arrays (pts,bonus,and cost)
 */
typedef enum
{
	/*combat talents*/
	com,		com_o,		com_o_s,
	com_o_s_s,	com_o_s_a,	com_o_s_p,
	com_o_b,	com_o_b_m,	com_o_b_f,
	com_o_b_s,	com_o_p,	com_o_p_d,
	com_o_p_s,	com_o_p_m,	com_o_u,
	com_d,		com_d_p,	com_d_d,
	com_d_c,	com_d_s,	com_s,		
	com_s_mu,	com_s_b,	com_s_ma,	

	/*covert talents*/
	cov,		cov_s,		cov_s_st,
	cov_s_h,	cov_s_sn,	cov_s_a,
	cov_m,		cov_m_s,	cov_m_s_c,
	cov_m_s_i,	cov_m_s_e,	cov_m_l,
	cov_m_p,

	/*arcane talents*/
	arc,		arc_n,		arc_n_s,
	arc_n_r,	arc_g,		arc_g_f,
	arc_g_wi,	arc_g_wa,	arc_g_ea,
	arc_g_et,	arc_a,		arc_a_o,
	arc_a_p,	arc_a_im,	arc_p,
	arc_p_m,	arc_p_p,

	/*worship talents*/
	wor,		wor_b,		wor_b_p,
	wor_b_p_p,	wor_b_p_m,	wor_b_p_s,
	wor_b_h,	wor_b_h_r,	wor_b_h_c,
	wor_b_e,	wor_b_e_m,	wor_b_e_b,
	wor_b_m,	wor_b_b,	wor_m,
	wor_m_a,	wor_m_o,

	/*sorcer talents*/
	sor,		sor_h,		sor_h_d,
	sor_h_i,	sor_h_c,	sor_h_t,
	sor_h_e,	sor_h_e_p,	sor_h_e_o,
	sor_w,		sor_w_e,	sor_w_w,
	sor_w_p
}talent;


CC      = gcc
PROF    = -O -g
NOCRYPT =
C_FLAGS =  -Wall $(PROF) $(NOCRYPT)
L_FLAGS =  $(PROF)
LIBS	= -lcrypt -lm
	  
O_FILES =	act_channel.c act_comm.c act_enter.c act_info.c act_move.c act_obj.c \
			act_wiz.c alias.c arena.c auction.c ban.c bank2.c bit.c check.c classes.c \
			comm.c const.c copyover.c dagashi.c db2.c db.c draconian.c effects.c \
			esper.c execute.c fight.c flags.c forms.c golem.c guild.c handler.c healer.c \
			heroexp.c honor.c interp.c keyring.c legionare.c locker.c lookup.c magic2.c \
			magic.c mem.c mob_cmds.c mobdeaths.c mob_prog.c music.c note.c olc_act.c olc.c \
			olc_load_mobs.c olc_load_new.c olc_load_rooms.c olc_mpcode.c olc_save.c \
			olc_save_new.c pload.c qsort.c quest.c race.c raiding.c \
			recycle.c reload.c remort.c restring.c save.c  sensei.c sheath.c skill_arcane.c \
			skill_combat.c skill_covert.c skills.c skill_sorcery.c skill_table.c \
			skill_tree.c skill_worship.c slay.c smithing.c special.c stat.c string.c tables.c \
			taomaster.c teach.c trophy.c update.c weapon_spells.c zealot.c
			
rom: $(O_FILES)
	rm -f rom
	$(CC) $(L_FLAGS) -o rom $(O_FILES) $(LIBS)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

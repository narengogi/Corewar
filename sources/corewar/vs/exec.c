/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vbrazhni <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/23 19:49:18 by vbrazhni          #+#    #+#             */
/*   Updated: 2019/01/03 02:00:35 by vbrazhni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "corewar.h"
#include "corewar_vs.h"
#include <stdio.h>

static void	handle_buttons(t_vm *vm)
{
	if (vm->vs->button == SPACE)
		vm->vs->is_running = !(vm->vs->is_running);
	else if (vm->vs->button == MUSIC)
		vm->vs->sounds = !(vm->vs->sounds);
	else if (vm->vs->button == DISPLAY_HELP)
		vm->vs->display_help = !(vm->vs->display_help);
	else if (vm->vs->button == SPEED_UP_1)
		vm->vs->speed++;
	else if (vm->vs->button == SPEED_UP_10)
		vm->vs->speed += 10;
	else if (vm->vs->button == SPEED_UP_100)
		vm->vs->speed += 100;
	else if (vm->vs->button == SPEED_DOWN_1)
		vm->vs->speed--;
	else if (vm->vs->button == SPEED_DOWN_10)
		vm->vs->speed -= 10;
	else if (vm->vs->button == SPEED_DOWN_100)
		vm->vs->speed -= 100;
	else if (vm->vs->button == SPEED_DEFAULT)
		vm->vs->speed = DEFAULT_SPEED;
	vm->vs->is_running = (!vm->cursors_num) ? false : vm->vs->is_running;
	vm->vs->speed = (vm->vs->speed < 1) ? 1 : vm->vs->speed;
	vm->vs->speed = (vm->vs->speed > 1000) ? 1000 : vm->vs->speed;
}

static void	exec_cycle_vs(t_vm *vm)
{
	size_t cursors_num;

	if (vm->cursors_num)
	{
		exec_cycle(vm);
		if (vm->cycles_to_die == vm->cycles_after_check
			|| vm->cycles_to_die <= 0)
		{
			cursors_num = vm->cursors_num;
			cycles_to_die_check(vm);
			if (cursors_num != vm->cursors_num && vm->cursors_num)
				play_death_sound(vm);
		}
		if (!vm->cursors_num)
		{
			play_victory_sound(vm);
			vm->vs->is_running = false;
		}
	}
}

static void	print_result_on_exit(t_vm *vm)
{
	t_player *last_alive;

	last_alive = vm->last_alive;
	if (!last_alive)
		return ;
	printf("\n=== %s ===\n",
		(!vm->cursors_num) ? "Match finished" : "Match stopped by user (ESC)");
	printf("Player %d: \"%s\"\n",
		FT_ABS(last_alive->id),
		last_alive->name);
	printf("Cycle: %lld | Last live: %lld\n",
		(long long)vm->cycles,
		(long long)last_alive->last_live);
	printf("Lives (current/previous period): %llu/%llu\n",
		(unsigned long long)last_alive->current_lives_num,
		(unsigned long long)last_alive->previous_lives_num);
	if (!vm->cursors_num)
		printf("Result: has won!\n");
	else
		printf("Result: leading when stopped.\n");
	fflush(stdout);
}

void		exec_vs(t_vm *vm)
{
	int32_t i;

	i = 0;
	while (i < vm->players_num)
		clear_name(vm->players[i++]->name);
	configure_vs(vm);
	while ((vm->vs->button = getch()) != ESC)
	{
		handle_buttons(vm);
		if (vm->vs->button == PASS_ONE_CYCLE)
			exec_cycle_vs(vm);
		else if (vm->vs->is_running && (clock() >= calc_time_delay(vm)))
		{
			vm->vs->time = clock();
			exec_cycle_vs(vm);
		}
		draw(vm);
	}
	free_vs(&(vm->vs));
	print_result_on_exit(vm);
}

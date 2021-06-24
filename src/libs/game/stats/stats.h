/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */

struct player_stats_t
{
	long long games, wins, draws, losses, moves, playtime, bot_games, bot_wins, bot_draws, bot_losses, bot_moves, bot_playtime; // +W/L Ratio
};

struct stats_t
{
	struct player_stats_t player[10];
	long long total_games, total_wl, total_draws, total_moves, total_playtime; // + Average moves, Average Playtime
};

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

struct stats_t* init_stats(char* fileName);
void define_stats(struct stats_t* stats);
bool read_stats_file(struct stats_t* stats, char* fileName); /// TODO
bool write_stats_file(struct stats_t* stats, char* fileName); /// TODO

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

struct stats_t* init_stats(char* fileName)
{
	struct stats_t* stats = malloc(sizeof(struct stats_t));
    char editedFileName[strlen("../res/bin/") + strlen(fileName)];
    strcpy(editedFileName, strcat("../res/bin/", fileName));
    printf("%s\n", editedFileName);

    if (!read_stats_file(stats, editedFileName))
	{
		printf("Failed to open stats file!\n");
		define_stats(stats);
		if (!write_stats_file(stats, editedFileName))
		{
			printf("Failed to create stats file!\n");
		}
	}

    return stats;
}

void define_stats(struct stats_t* stats)
{
	stats->total_games = stats->total_wl = stats->total_draws = stats->total_moves = stats->total_playtime = 0;
	for (int x = 0; x < 10; x++)
	{
		stats->player[x].games = stats->player[x].wins = stats->player[x].draws = stats->player[x].losses = stats->player[x].moves = stats->player[x].playtime = stats->player[x].bot_games = stats->player[x].bot_wins = stats->player[x].bot_draws = stats->player[x].bot_losses = stats->player[x].bot_moves = stats->player[x].bot_playtime = 0;
	}

	return;
}

bool read_stats_file(struct stats_t* stats, char* fileName)
{
	/// TODO
	return true;
}

bool write_stats_file(struct stats_t* stats, char* fileName)
{
	/// TODO
	return true;
}

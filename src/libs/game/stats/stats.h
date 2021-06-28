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
    sprintf(editedFileName, "../res/bin/%s", fileName);

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
	FILE* statsFile;

    if((statsFile = fopen(fileName, "rb")))
    {
    	for (short x = 0; x < 10; x++)
		{
			fread(&stats->player[x].games, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].wins, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].draws, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].losses, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].moves, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].playtime, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].bot_games, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].bot_wins, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].bot_draws, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].bot_losses, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].bot_moves, sizeof(long long), 1, statsFile);
			fread(&stats->player[x].bot_playtime, sizeof(long long), 1, statsFile);
		}
		fread(&stats->total_games, sizeof(long long), 1, statsFile);
		fread(&stats->total_wl, sizeof(long long), 1, statsFile);
		fread(&stats->total_draws, sizeof(long long), 1, statsFile);
		fread(&stats->total_moves, sizeof(long long), 1, statsFile);
		fread(&stats->total_playtime, sizeof(long long), 1, statsFile);

    	fclose(statsFile);
    }
    else
	{
        printf("Failed to open stats file!\n");
        return false;
    }

	return true;
}

bool write_stats_file(struct stats_t* stats, char* fileName)
{
	FILE* statsFile;

    if((statsFile = fopen(fileName, "wb+")))
    {
    	for (short x = 0; x < 10; x++)
		{
			// long long games, wins, draws, losses, moves, playtime, bot_games, bot_wins, bot_draws, bot_losses, bot_moves, bot_playtime; // +W/L Ratio
			fwrite(&stats->player[x].games, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].wins, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].draws, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].losses, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].moves, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].playtime, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].bot_games, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].bot_wins, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].bot_draws, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].bot_losses, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].bot_moves, sizeof(long long), 1, statsFile);
			fwrite(&stats->player[x].bot_playtime, sizeof(long long), 1, statsFile);
		}
		// long long total_games, total_wl, total_draws, total_moves, total_playtime; // + Average moves, Average Playtime
		fwrite(&stats->total_games, sizeof(long long), 1, statsFile);
		fwrite(&stats->total_wl, sizeof(long long), 1, statsFile);
		fwrite(&stats->total_draws, sizeof(long long), 1, statsFile);
		fwrite(&stats->total_moves, sizeof(long long), 1, statsFile);
		fwrite(&stats->total_playtime, sizeof(long long), 1, statsFile);

    	fclose(statsFile);
    }
    else
	{
        printf("Failed to create or open stats file!\n");
        return false;
    }

	return true;
}

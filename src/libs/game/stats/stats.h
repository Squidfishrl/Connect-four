/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../board/board.h"

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
void display_stats_menu(struct stats_t* stats, struct dict_t* colourDict);
void free_stats(struct stats_t* stats);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void free_stats(struct stats_t* stats){
	free(stats);
}

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

void display_stats_menu(struct stats_t* stats, struct dict_t* colourDict){

    log_stderr(0, 1, "Displaying stats menu");

    // colour vars
    log_stderr(0, 0, "Fetching colours from colour dict");
    const char *flash, *white, *def, *yellow, *heavy;
    flash = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    def = binary_search_dict('D', colourDict);
    heavy = binary_search_dict('H', colourDict);
    yellow = binary_search_dict('Y', colourDict);

    system("clear");

    printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
    printf("%s (stats) %s\n\n", yellow, def);

    printf("%s%s Global stats %s\n", heavy, white, def);
    printf("%s  -%s %sGames: %lld%s\n", flash, def, white, stats->total_games, def);
    printf("%s  -%s %sWins: %lld%s\n", flash, def, white, stats->total_wl, def);
    printf("%s  -%s %sDraws: %lld%s\n", flash, def, white, stats->total_draws, def);
    printf("%s  -%s %sMoves: %lld%s\n", flash, def, white, stats->total_moves, def);
    printf("%s  -%s %sAverage moves per game: %lld%s\n", flash, def, white, stats->total_moves/stats->total_games, def);
    printf("%s  -%s %sPlaytime: %lld%s\n", flash, def, white, stats->total_playtime, def);
    printf("%s  -%s %sAverage playtime per game: %lld%s\n", flash, def, white, stats->total_playtime/stats->total_games, def);

    for(short i = 0; i<10; i++){
        printf("%s%s \nPlayer %hd stats %s\n", heavy, white, i+1, def);

        printf("%s  -%s %sGames: %lld%s\n", flash, def, white, stats->player[i].games, def);
        printf("%s  -%s %sWins: %lld%s\n", flash, def, white, stats->player[i].wins, def);
        printf("%s  -%s %sDraws: %lld%s\n", flash, def, white, stats->player[i].draws, def);
        printf("%s  -%s %sLosses: %lld%s\n", flash, def, white, stats->player[i].losses, def);
        printf("%s  -%s %sWin/Lose ratio: %lf%s\n", flash, def, white, (double)stats->player[i].wins/stats->player[i].losses, def);
        printf("%s  -%s %sMoves: %lld%s\n", flash, def, white, stats->player[i].moves, def);
        printf("%s  -%s %sPlaytime: %lld%s\n", flash, def, white, stats->player[i].playtime, def);
        printf("%s  -%s %sBot games: %lld%s\n", flash, def, white, stats->player[i].bot_games, def);
        printf("%s  -%s %sBot draws: %lld%s\n", flash, def, white, stats->player[i].bot_draws, def);
        printf("%s  -%s %sBot losses: %lld%s\n", flash, def, white, stats->player[i].bot_losses, def);
        printf("%s  -%s %sBot Win/Lose ratio: %lf%s\n", flash, def, white, (double)stats->player[i].bot_wins/stats->player[i].bot_losses, def);
        printf("%s  -%s %sBot moves: %lld%s\n", flash, def, white, stats->player[i].bot_moves, def);
        printf("%s  -%s %sBot playtime: %lld%s\n", flash, def, white, stats->player[i].bot_playtime, def);
    }

    getchar();

}

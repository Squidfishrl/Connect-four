/* IMPORTED LIBRARIES */

#include <stdio.h>
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

void display_stats_menu(struct stats_t* stats, struct dict_t* colourDict);

/* -------------------------------------------------------------------------- */


/* MAIN */



/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

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

/* -------------------------------------------------------------------------- */

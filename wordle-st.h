#pragma once

#define RENDER_OFF 0
#define RENDER_ON 1

enum error_list{
GOOD,
ERR_IMPROPER_WORD,
ERR_TRY_EXCEEDED,
};

//init_game returns the initializing status
int init_game(const char *file_path);//Return 0, if initialization successed

int guess(const char *answer, char *result);

void start_game(int render_option);
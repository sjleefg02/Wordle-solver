#pragma once

#define WORD_LEN_MAX 5
#define MAX_TRY 6

enum status_list{
CORRECT,
WRONG,
TRY_EXCEEDED,
};

typedef struct _result{
  float time;
  int try_cnt;//99 means they failed
}result_t;

//-----------------------------------------------
void init_game_ta(const char *file_path);

int check_proper_ans(const char *answer);

void set_question();
//-----------------------------------------------

void init_round();

void check_correct(const char* result);

void check_answer(const char *answer, char *result);

void store_result(result_t *dest, float time);

int guess(const char *answer, char *result);
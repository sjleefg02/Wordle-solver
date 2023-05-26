#include  "wordle-st.h"
#include  "wordle-ta.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static char question[6];
static int try_cnt;
static int status;//round status

//-----------------------------------------------
void init_game_ta(const char *file_path){
  //initialize project
  srand(time(NULL));
}

int check_proper_ans(const char *answer){
  //Check the answer is exist in word library
  return 1;
}

void set_question(){
  /*for random generation
  for(int i = 0; i < WORD_LEN_MAX; i++)
    question[i] = rand()%26 + 97;
  question[WORD_LEN_MAX] = '\0';
  */
  
  stpcpy(question, "abeam");
}
//-----------------------------------------------

void init_round(){
  try_cnt = 0;
  status = WRONG;
  set_question();
  system("clear");
  printf("Answer: %s\n", question);
}

void check_correct(const char* result){
  for(int i = 0; i < 5; i++){
    if(result[i] != 'g')
      return;
  }
  status = CORRECT;
}

void check_answer(const char *answer, char *result) {
  int checked[5] = {
      0,
  };

  // fill black
  for (int i = 0; i < 5; i++)
    result[i] = 'b';

  // check green
  for (int i = 0; i < 5; i++) {
    if (answer[i] == question[i]) {
      result[i] = 'g';
      checked[i] = 1;
    }
  }

  // check yellow
  for (int i = 0; i < 5; i++) { // for iterating answer
    if (result[i] == 'b') {
      for (int j = 0; j < 5; j++) { // for iterating checked
        if (!checked[j] && answer[i] == question[j]) {
          result[i] = 'y';
          checked[j] = 1;
          break;
        }
      }
    }
  }
}

void store_result(result_t *dest, float time){
  dest->time = time;
  if(status == CORRECT)
    dest->try_cnt = try_cnt;
  else
    dest->try_cnt = 99;
}

int guess(const char *answer, char *result){  
  if(!check_proper_ans(answer))
    return ERR_IMPROPER_WORD;

  try_cnt++;

  if(try_cnt > MAX_TRY){
    status = TRY_EXCEEDED;
    return ERR_TRY_EXCEEDED;
  }
  
  check_answer(answer, result);

  check_correct(result);

  return GOOD;
}


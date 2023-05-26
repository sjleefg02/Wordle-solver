#include "wordle-st.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  //for sleep

#define WORD_LEN_MAX 5
#define TRY_MAX 6
#define MAX_WORD 30000
#define ALPHABET_CNT 26

struct word_status{
  char answer[WORD_LEN_MAX + 1];
  char result[WORD_LEN_MAX + 1];
};
struct word_status results[TRY_MAX];

double point[ALPHABET_CNT] = {8.2,  1.5,  2.8, 4.3,  13,  2.2,  2,   6.1,   7,
                              0.15, 0.77, 4,   2.4,  6.7, 7.5,  1.9, 0.095, 6,
                              6.3,  9.1,  2.8, 0.98, 2.4, 0.15, 2,   0.074};
char word_list_data[MAX_WORD][WORD_LEN_MAX+1];
int list_len;

struct word_data {
  char word[WORD_LEN_MAX+1];
  double score;
  int unused_letter_cnt;
  int common_letter_cnt;
  struct word_data *next;
};
typedef struct word_data data;
data *head;

//각 data 라는 struct내부의 변수들을 계산하기 위한 함수 : unused_letter_count,common_letter_count,score_count//
int unused_letter_count(char* word)
{
  int word_letter_cnt = 0;
  // B, J, K, Q, X, Z 의 개수를 세는 프로그램
    char used[6] = {0,};
    for (int i = 0; i < WORD_LEN_MAX; i++) {
      if (word[i] == 'b') {used[0] = 1;}
      if (word[i] == 'j') {used[1] = 1;}
      if (word[i] == 'k') {used[2] = 1;}
      if (word[i] == 'q') {used[3] = 1;}
      if (word[i] == 'x') {used[4] = 1;}
      if (word[i] == 'z') {used[5] = 1;}
    }
    for (int i = 0; i < 6; i++)
      if (used[i] == 1) {word_letter_cnt += 1;}

  return word_letter_cnt;
}

int common_letter_count(char* word)
{
  int word_letter_cnt = 0;
  // T, N, H, S, A, E, I, O, U 의 개수를 세는 프로그램
    char used[9] = {0,};
    for (int i = 0; i < WORD_LEN_MAX; i++) {
      if (word[i] == 't') {used[0] = 1;}
      if (word[i] == 'n') {used[1] = 1;}
      if (word[i] == 'h') {used[2] = 1;}
      if (word[i] == 's') {used[3] = 1;}
      if (word[i] == 'a') {used[4] = 1;}
      if (word[i] == 'e') {used[5] = 1;}
      if (word[i] == 'i') {used[6] = 1;}
      if (word[i] == 'o') {used[7] = 1;}
      if (word[i] == 'u') {used[8] = 1;}
    }
    for (int i = 0; i < 9; i++)
      if (used[i] == 1) {word_letter_cnt += 1;}

  return word_letter_cnt;
}

double score_count(char* word){
  double word_score = 0;
  for (int i = 0; i < WORD_LEN_MAX; i++)
      word_score += point[word[i] - 'a'];
  
  return word_score;
}

//list 구성을 위한 함수 : make_list, linked_list_construct, linked_list_delete//
//node 추가를 위한 함수
void linked_list_construct(data *prev_data, char input_word[])
{
  data *new_list_data = malloc(sizeof(data)); 
  new_list_data->next = NULL;
  for(int i = 0; i < WORD_LEN_MAX; i++){// 0 1 2 3 4
    new_list_data->word[i] = input_word[i];    
  } 
  new_list_data->word[WORD_LEN_MAX] = '\0'; // null character
  new_list_data->unused_letter_cnt = unused_letter_count(input_word);
  new_list_data->common_letter_cnt = common_letter_count(input_word);
  new_list_data->score = score_count(input_word);
  prev_data->next = new_list_data;  
}

//node 삭제를 위한 함수
void linked_list_delete(data *prev_node, data *delete_node){
  if(prev_node->next != delete_node){return;}
  prev_node->next = delete_node->next;
  free(delete_node);
}

//가장 처음에 list를 만들기 위해서 사용
void make_list(data *head, int list_len)
{
  data *temp = malloc(sizeof(data));
  temp = head;
  int cnt_num = 0;
  char inputword[WORD_LEN_MAX+1];
  while (cnt_num!=list_len){
    for(int i=0; i<WORD_LEN_MAX; i++){
      inputword[i] = word_list_data[cnt_num][i];
    }
    inputword[WORD_LEN_MAX] = '\0';
    cnt_num++; 
    linked_list_construct(temp, inputword);
    temp = temp->next;
  }
  data *print_node;
  print_node = head->next;
  while (print_node!=NULL){
    print_node = print_node->next;
  }
}
//작동을 확인하기 위한 print 함수
void list_print(data *current_node){
  printf("%s / %d / %lf\n", current_node->word,current_node->unused_letter_cnt, current_node->score);
  if(current_node->next!=NULL){
    list_print(current_node->next);
  }
}

//guess를 진행함에 따라 이전 결과에 의해 list를 수정하는 함수
void list_modify(int trial_num){
  data *curr_data= malloc(sizeof(data)); 
  data *prev_data =malloc(sizeof(data)); ;
  prev_data = head;
  curr_data = head->next;

  while(prev_data->next != NULL){
    int node_delete=0;
    int yellow_in=-1;
    int black_in= -1;
    int black_check=-1;
    int same_word=1;

    for (int i=0; i<WORD_LEN_MAX; i++){
      if(results[trial_num].answer[i]!=curr_data->word[i]){same_word = 0;}
    }if(same_word == 1){node_delete = 1;}
    
    for (int i=0; i<WORD_LEN_MAX; i++){
      if(results[trial_num].result[i]=='g'){
        if(results[trial_num].answer[i] != curr_data->word[i]){node_delete=1;}
      }
    }//절대참.
    
    for(int i=0; i<WORD_LEN_MAX; i++){
      if(results[trial_num].result[i]=='y'){
        //printf("%s / %d \n", curr_data->word,yellow_in);
        yellow_in=0;
        for(int j=0; j<WORD_LEN_MAX; j++){
          if(results[trial_num].answer[i] == curr_data->word[j]){yellow_in=1;}
        }
      }
    }if(yellow_in==0){node_delete=1;}

    for(int i=0; i<WORD_LEN_MAX; i++){
      if(results[trial_num].result[i]=='b'){
        black_check=1;
        for(int j=0; j<WORD_LEN_MAX; j++){
          if(results[trial_num].answer[i] == results[trial_num].answer[j]){
            if(results[trial_num].result[j]!='b'){black_check =0;}
          }
        }
        if(black_check==1){
          for(int j=0; j<WORD_LEN_MAX; j++){
            if(results[trial_num].answer[i]==curr_data->word[j]){
              black_in=1;
            }
          }
        }
        black_check=-1;
      }
    }if(black_in==1){node_delete=1;}
    
    if(node_delete==1){
      linked_list_delete(prev_data, curr_data);
      curr_data = prev_data->next;
    }else{
      prev_data = prev_data->next;
      curr_data = curr_data->next;
    }
  }
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//Initializing part
int build_word_lib(const char *file_path, char (*word_list_data)[WORD_LEN_MAX+1])
{ 
  memset(word_list_data, 0, sizeof(char) * (MAX_WORD) * (WORD_LEN_MAX + 1));
  int word_num;
  int c;
  freopen(file_path, "r",stdin);

  if(!file_path){return -1;}
  // the number of words
  scanf("%d", &word_num);

  // change to lowercase then print it on console.
  for (int i = 0; i < word_num; i++) {
    scanf(" %s", word_list_data[i]);
    for (int j = 0; j < WORD_LEN_MAX; j++) { 
      //only considering lowercase
      word_list_data[i][j] = tolower(word_list_data[i][j]);
    }
    word_list_data[i][WORD_LEN_MAX] ='\0';
    //printf("%s\n", word_list_data[i]);
  }  
  fclose(stdin);
  return word_num;
}

int init_game(const char *file_path)
{
  head = malloc(sizeof(data));
  head->next = NULL;
  list_len = build_word_lib(file_path,word_list_data);
  if(list_len<0){return -1;}
  //make_list(head,list_len);
  srand(time(NULL));

  return 0;  
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//Progressing the game part
//첫번째 시도를 위한 함수 
void make_answer_strategy1(int try_cnt)
{
  data *current_node = malloc(sizeof(data)); 
  current_node= head->next; 
  data stored_node;
  stored_node.score = 0.0;
  int cnt = 0;

  while(current_node != NULL){
    if(current_node->unused_letter_cnt > cnt)
      cnt = current_node->unused_letter_cnt;
    current_node = current_node->next;
  }
  
  current_node = head->next; 
  while(current_node != NULL){
    if(current_node->unused_letter_cnt == cnt){
      if(stored_node.score < current_node->score){
        for(int i = 0; i < WORD_LEN_MAX; i++)
          stored_node.word[i] = current_node->word[i];
        stored_node.word[WORD_LEN_MAX] = '\0';
        stored_node.score = current_node->score;
      }
    }
    current_node = current_node->next;
  }
 
  for(int i = 0; i < WORD_LEN_MAX; i++)
      results[try_cnt].answer[i] = stored_node.word[i];
  results[try_cnt].answer[WORD_LEN_MAX] = '\0';

  return;
}

//3,4,5,6번째와 2-1의 실행을 위한 함수
void make_answer_strategy2(int try_cnt)
{
  data *curr_node= malloc(sizeof(data)); ;
  curr_node = head->next;
  data *result_node= malloc(sizeof(data)); ;
  int common_letter_max = 0;
  
  while (curr_node != NULL){
    if((curr_node->common_letter_cnt)>common_letter_max){
      common_letter_max = curr_node->common_letter_cnt;
      result_node = curr_node;
    }
    curr_node = curr_node->next;
  }
  curr_node = head->next;
  while(curr_node != NULL){
    if(curr_node->common_letter_cnt==common_letter_max){
      if(curr_node->score>result_node->score){
        result_node = curr_node;
      }
    }curr_node = curr_node->next;
  }

  for(int i = 0; i < WORD_LEN_MAX; i++){
    results[try_cnt].answer[i] = result_node->word[i];
  }
  results[try_cnt].answer[WORD_LEN_MAX] = '\0';
return;
}

//두번째 시도를 위한 함수
void make_answer_word_2(int try_cnt)
{
  int case_check=0;
  //BJKQZX가 정답에 포함된 경우
  char unused[6] = {'b','j','k','q','z','x'};
  
  for(int i = 0; i < WORD_LEN_MAX; i++)
  {
    if((results[0].result[i]=='g')||(results[0].result[i]=='y')){
      for(int j=0; j<6; j++){
        if(results[0].answer[i] == unused[j]){
          case_check = 1;
          break;
        }
      }
    }
  }
  if(case_check==0)
    make_answer_strategy1(try_cnt);
  else
    make_answer_strategy2(try_cnt);
}

int is_correct(const char *result)
{
  for(int i = 0; i < WORD_LEN_MAX; i++)
    if(result[i] != 'g') return 0;
  return 1;
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//Rendering part
void render_start_scene(){
  //rendering
  printf("\n-----[Wordle game]-----\n");
}

void render_progress_scene(int try_cnt){
  //rendering
  system("clear");
  printf("\033[0;31m┏━━━┳━━━┳━━━┳━━━┳━━━┓\n");
  for(int i = 0 ; i < TRY_MAX; i++){
    printf("\033[0;31m┃");
    for(int j = 0; j < WORD_LEN_MAX; j++){
      if(i <= try_cnt){
        if(results[i].result[j] == 'b')
          printf("\033[0;100m %c", results[i].answer[j]);
        if(results[i].result[j] == 'y')
          printf("\033[0;43m %c", results[i].answer[j]);
        if(results[i].result[j] == 'g')
          printf("\033[0;42m %c", results[i].answer[j]);
        printf(" \033[0;31m┃");
      }
      else{
        printf("   \033[0;31m┃");
      }
    }
    printf(" \n");
    if(i < TRY_MAX-1)
      printf("\033[0;31m┣━━━╋━━━╋━━━╋━━━╋━━━┫\n");
  }
  printf("\033[0;31m┗━━━┻━━━┻━━━┻━━━┻━━━┛\n");
}

void render_end_scene(int try_cnt){

  sleep(1);
  system("clear");
  
  int not_green = 0;
  for(int i = 0; i < WORD_LEN_MAX; i++){
    if(results[try_cnt].result[i] != 'g')
      not_green++;
  }
  if(not_green != 0){
    printf("\033[0;00m-----[Statistics]-----\n\n");
    printf("* Guess Failed\n");
  }
    
  else{
  for(int k = 0; k < 4; k++){
    if(k % 2 == 0){
        printf("\033[0;00m-----[Statistics]-----\n\n");
        printf("* Guess Succeed\n");
        printf("* Guess count: %d\n", try_cnt+1);
        printf("* Answer: %s\n", results[try_cnt].answer);
        sleep(1);
        system("clear");
      }
    
    else{
        printf("\033[0;30m-----[Statistics]-----\n\n");
        printf("* Guess Succeed\n");
        printf("* Guess count: %d\n", try_cnt+1);
        printf("* Answer: %s\n", results[try_cnt].answer);
        sleep(1);
        system("clear");
        }
    }

    printf("\033[0;00m-----[Statistics]-----\n\n");
    printf("* Guess Succeed\n");
    printf("* Guess count: %d\n", try_cnt+1);
    printf("* Answer: %s\n", results[try_cnt].answer);
    sleep(1);
  }
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


void start_game(int render_option)
{
  int try_cnt;

  if(render_option == RENDER_ON){
    sleep(1);//delay
    render_start_scene();
  }
  make_list(head,list_len);
  
  for(try_cnt = 0; try_cnt < TRY_MAX; try_cnt++){
    // sleep(1);
    if (try_cnt == 0)
      make_answer_strategy1(0);
    else if (try_cnt == 1)
      make_answer_word_2(try_cnt);
      //make_answer_strategy1(1);
    else
      make_answer_strategy2(try_cnt);
      //make_answer_strategy1(try_cnt);
    
    guess(results[try_cnt].answer, results[try_cnt].result);
    
    if(render_option == RENDER_ON)
      render_progress_scene(try_cnt);
    
    if(is_correct(results[try_cnt].result))
      goto finish;    

    list_modify(try_cnt);
  }
  
finish:
  //printf("trial = %d\n", try_cnt);
  if(render_option == RENDER_ON)
    render_end_scene(try_cnt);
}
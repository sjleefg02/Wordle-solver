#include  "wordle-st.h"
#include  "wordle-ta.h"

#include <stdio.h>
#include <time.h>

int main(void) {
  int rounds =1;
  int error_code;
  char* file_path = "./valid-wordle-words.txt"; 
  //variables for the time checking
  time_t start, end;
  result_t test_result[rounds];
  
  init_game_ta(file_path);
  
  error_code = init_game(file_path); //우리함수
  if(error_code){
    printf("Initializing Failed: %d\n", error_code);
  }

  for(int i = 0; i < rounds; i++){
    init_round();

    start = clock();
    start_game(RENDER_ON); //우리함수
    end = clock();
    
    store_result(&test_result[i], (float)(end-start)/CLOCKS_PER_SEC);
  }

  //print result or store result(maybe file I/O)
  
  return 0;
}
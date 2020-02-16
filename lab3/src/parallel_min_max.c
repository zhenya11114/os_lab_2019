#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int** pipes_max = malloc(sizeof(int*) * pnum);
  int** pipes_min = malloc(sizeof(int*) * pnum);
  int* proc_max_arr = malloc(sizeof(int) * pnum);
  int* proc_min_arr = malloc(sizeof(int) * pnum);

  int part_size = array_size/pnum;
  int pointer = 0;

  for (int i = 0; i < pnum; i++) {
      //init pipe
      int p_max[2];
      int p_min[2];
      //check if pipe fail
      if (pipe(p_max) == -1){
        printf("Pipe Failed");
        return 1;
      }
      if (pipe(p_min) == -1){
        printf("Pipe Failed");
        return 1;
      }
      pipes_max[i] = p_max;
      pipes_min[i] = p_min;
  }

  for (int i = 0; i < pnum; i++) {
    //pid is > 0 for main process and 0 for child
    pid_t child_pid = fork();

    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process - parallel code here
        
        int max = INT_MAX;
        int min = INT_MIN;

        close(pipes_max[i][0]);
        close(pipes_min[i][0]);

        //find min/max
        //write code there
        //use step: 1,5,9; 2,6,10 etc for every process
        //step is arr_size/procs_num
        //if arr have odd size give rest of it to procs_num-1 proc
        //pipe pass all variables by link, so use different min/max
        //good luck!


        //write min/max
        write(pipes_min[i][1], &max, sizeof(int));
        write(pipes_max[i][1], &min, sizeof(int));

        close(pipes_max[i][1]);
        close(pipes_min[i][1]);
        if (with_files) {
          // use files here
        } else {
          // use pipe here
        }
        return 0;
      }
    }
    else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  int count = -1;

  //this block should wait for all process to be done and sync them?
  while (active_child_processes > 0) {
    count++;
    int pipe_num = count;
    close(pipes_max[pipe_num][1]);
    close(pipes_min[pipe_num][1]);

    printf("\npipe_num: %d\n", pipe_num);

    int read_num = 0;

    read(pipes_max[pipe_num][0],&read_num,sizeof(int));
    proc_max_arr[pipe_num] = read_num;
    printf("read max: %d\n", proc_max_arr[pipe_num]);

    read(pipes_min[pipe_num][0],&read_num,sizeof(int));
    proc_min_arr[pipe_num] = read_num;
    printf("read min: %d\n", read_num);

    //close(pipes_max[pipe_num][0]);
    //close(pipes_min[pipe_num][0]);
    active_child_processes -= 1;
  }

  //output of programm
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    //there we set our min/max nums
    if (with_files) {
      // read from files
    } else {
      // read from pipes
    }

    //there we fill the struture of min/max nums
    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}

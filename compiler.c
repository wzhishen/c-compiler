/* Zhishen Wen
 * Dec. 10, 2012
 * MCIT, Penn CIS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// so you can call this function from here
extern char* trim(char*);

main(int argc, char *argv[]) {

  // make sure the filename is specified
  if (argc < 2) {
    printf("Please specify the name of the file to compile\n");
    return;
  }
  char *name  = argv[1];

  // create the symbol table
  if (create_symbol_table(name) == 0) {
    exit(0);
  }

  // create input stream
  FILE *input = fopen(name, "r");
  if (input == NULL) exit(0);
  char line[110];
  fgets(line, 110, input);

  // create output stream
  char *filename = malloc(sizeof(char) * (strlen(name)));
  strncpy(filename, name, strlen(name) - 2);
  strcat(filename, ".lc3");
  FILE *output = fopen(filename, "w");

  printf("Converting C code to LC3 assembly code...\n");
  int line_number = 1;
  if (strlen(trim(line)) == 0)
    fprintf(output, "\n\t;; LINE %d: (BLANK LINE)\n", line_number++);
  else {
    fprintf(output, "\t;; LINE %d: %s\n", line_number++, trim(line));

    // ignore globals
    while (has_global_var(trim(line))) {
      fgets(line, 110, input);

      if (strlen(trim(line)) == 0)
        fprintf(output, "\t;; LINE %d: (BLANK LINE)\n", line_number++);
      else {
        fprintf(output, "\t;; LINE %d: %s\n", line_number++, trim(line));
      }
    }
    generate_prologue(output, trim(line));
  }
  
  while (fgets(line, 110, input) != NULL) {

    if (strlen(trim(line)) == 0)
      fprintf(output, "\n\t;; LINE %d: (BLANK LINE)\n", line_number++);
    else {
      fprintf(output, "\n\t;; LINE %d: %s\n", line_number++, trim(line));
      compile_line(output, trim(line));
    }
  }

  generate_epilogue(output);
  printf("Conversion complete!\n");
}

/*
 * Generate the prologue for the function and write it
 * to the output file.
 * Use the function header to determine the name of the
 * function, and use it as the label of the first
 * instruction of the prologue.
 */
int generate_prologue(FILE *output, char *header) {

  if (header == NULL) return 0;

  char func_name[4];

  int i = 4;
  while (header[i] != ' ') {
    func_name[i - 4] = header[i];
    i++;
  }

  fprintf(output, "\t;; prologue for %s\n", func_name);
  fprintf(output, "%s\tADD SP, SP, -1 ; push return value\n", func_name);
  fprintf(output, "\tADD SP, SP, -1 ; push return address\n");
  fprintf(output, "\tSTR R7, SP, 0\n");
  fprintf(output, "\tADD SP, SP, -1 ; push FP\n");
  fprintf(output, "\tSTR FP, SP, 0\n");

  int local_var_number = get_local_var_number();
  if (local_var_number == 0)
    fprintf(output, "\tADD FP, SP, -1 ; set FP\n");
  else{
    fprintf(output, "\tADD SP, SP, -1 ; push local var\n");
    fprintf(output, "\tADD FP, SP, 0 ; set FP\n");
    int i;
    for (i = 0; i < local_var_number - 1; i++)
    fprintf(output, "\tADD SP, SP, -1 ; push local var\n");
  }
  fprintf(output, "\t;; prologue ends\n");

}

/*
 * Convert this line of code to LC-3 and write to
 * the output file.
 */
int compile_line(FILE *output, char *line) {

  if (line == NULL) return 0;

  char s0[10];
  char s1[10];
  char s2[10];
  char s3[10];
  char s4[10];
  char s5[10];
  char s6[10];
  char s7[10];
  char s8[10];
  char s9[10];
  char end[10];
  char *strings[] = {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, end};
  
  int numWords = tokenize(line, strings);

  if (strcmp(strings[0], "}") != 0) {

    if (strcmp(strings[0], "int") != 0 && strcmp(strings[0], "return") != 0) {
      process_case1(output, strings, line); // CASE 1 simple assignment
    }

    if (strcmp(strings[0], "int") == 0 && !has_no_assignment(line)) {
      process_case2(output, strings); // CASE 2 declaration and initialization
    }

    if (strcmp(strings[0], "return") == 0) {
      process_case3(output, strings); // CASE 3 return statement
    }

    // CASE 4 simple declaration: do NOTHING
  }

}

/*
 * Generate the epilogue for this function.
 */
int generate_epilogue(FILE *output) {

  fprintf(output, "\t;; epilogue starts\n");

  int local_var_number = get_local_var_number();
  if (local_var_number > 0) {
    int i;
    for (i = 0; i < local_var_number; i++)
    fprintf(output, "\tADD SP, SP, 1 ; pop local var\n");
  }
  
  fprintf(output, "\tLDR FP, SP, 0 ; restore old FP\n");
  fprintf(output, "\tADD SP, SP, 1\n");
  fprintf(output, "\tLDR R7, SP, 0 ; restore old PC\n");
  fprintf(output, "\tADD SP, SP, 1\n");
  fprintf(output, "\tADD SP, SP, 1 ; pop return value\n");

  int param_number = get_param_number();
  if (param_number > 0) {
    int i;
    for (i = 0; i < param_number; i++)
    fprintf(output, "\tADD SP, SP, 1 ; pop param\n");
  }

  fprintf(output, "\tRET\n");
  fprintf(output, "\t;; epilogue ends\n");

}

/************************* process each case *****************************/
// CASE 1 simple assignment
process_case1(FILE *output, char *strings[], char *line) {

  int offset = 0;
  int i = 2;

  if (!has_function_call(line)) {
    while (strlen(strings[i]) != 0) {

      // if current token is letter
      if (is_valid_word(strings[i])) {
        process_letter_token(output, strings, offset, i);
      }

      // if current token is number
      if (is_valid_number(strings[i])) {
        process_number_token(output, strings, i);
      }

      i += 2;
    }

    if (get_global_status(strings[0]) == 0) {
      get_offset(strings[0], &offset);
      fprintf(output, "\tSTR R0, FP, %d ; write %s\n", offset, strings[0]);
    }
    else {
      get_offset(strings[0], &offset);
      fprintf(output, "\tSTR R0, GP, %d ; write global %s\n", offset, strings[0]);
    }
  }
  else {

  /****************************** handles function call ******************************/
    // CASE 1 simple function call
    if (has_no_assignment(line)) {
      // CASE 1.1 if the function has no params
      if (strcmp(strings[1], "(") == 0) {
        fprintf(output, "\tJSR %s ; jump to subroutine '%s'\n", strings[0], strings[0]);
        fprintf(output, "\tLDR R0, SP, -3 ; read return value\n");
      }
      // CASE 1.2 if the function has params
      else {
        for (i = 9; i >= 1; i--) {
          if (strlen(strings[i]) != 0) {

              if (i == 1) {
                memmove(strings[i], strings[i]+1, strlen(strings[i]));
                if (is_valid_word(strings[i])) {
                  process_func_call_word(output, strings, offset, i);
                }

                if (is_valid_number(strings[i])) {
                  process_func_call_num(output, strings, i);
                }
              }
              else {
                if (is_valid_word(strings[i])) {
                  process_func_call_word(output, strings, offset, i);
                }

                if (is_valid_number(strings[i])) {
                  process_func_call_num(output, strings, i);
                }
              }
            }
          }
        fprintf(output, "\tJSR %s ; jump to subroutine '%s'\n", strings[0], strings[0]);
        fprintf(output, "\tLDR R0, SP, -3 ; read return value\n");
      }
    }
    // CASE 2 function call with assignment to some var
    else {
      // CAES 2.1 if the function has no params
      if (strcmp(strings[3], "(") == 0) {
        fprintf(output, "\tJSR %s ; jump to subroutine '%s'\n", strings[2], strings[2]);
        fprintf(output, "\tLDR R0, SP, -3 ; read return value\n");
      }
      // CASE 2.2 if the function has params
      else {
        for (i = 9; i >= 3; i--) {
          if (strlen(strings[i]) != 0) {

              if (i == 3) {
                memmove(strings[i], strings[i]+1, strlen(strings[i]));
                if (is_valid_word(strings[i])) {
                  process_func_call_word(output, strings, offset, i);
                }

                if (is_valid_number(strings[i])) {
                  process_func_call_num(output, strings, i);
                }
              }
              else {
                if (is_valid_word(strings[i])) {
                  process_func_call_word(output, strings, offset, i);
                }

                if (is_valid_number(strings[i])) {
                  process_func_call_num(output, strings, i);
                }
              }
            }
          }
        fprintf(output, "\tJSR %s ; jump to subroutine '%s'\n", strings[2], strings[2]);
        fprintf(output, "\tLDR R0, SP, -3 ; read return value\n");
      }

      if (get_global_status(strings[0]) == 0) {
        get_offset(strings[0], &offset);
        fprintf(output, "\tSTR R0, FP, %d ; write return value to %s\n", offset, strings[0]);
      }
      else {
        get_offset(strings[0], &offset);
        fprintf(output, "\tSTR R0, GP, %d ; write return value to global %s\n", offset, strings[0]);
      }
    }
    
  }

}

// CASE 2 declaration and initialization
process_case2(FILE *output, char *strings[]) {

  int offset = 0;
  int end_of_line = 0;
  int start_index = 1;
  int index_var_asgn;

  while (end_of_line == 0) {
    int i = find_first_var_initialized(strings, start_index);
    if (i == -1) break;
    index_var_asgn = i - 2;
    
    while (1) {

      // if current token is letter
      if (is_valid_word(strings[i])) {      
        process_letter_token(output, strings, offset, i);
      }

      // if current token is number
      if (is_valid_number(strings[i])) {
        process_number_token(output, strings, i);
      }

      if (strcmp(strings[i + 1], "+") == 0 || strcmp(strings[i + 1], "-") == 0) {
        i += 2;
        continue;
      }
      else if (strings[i + 1] == NULL) end_of_line = 1;
      else start_index = i + 1;
      break;
    }

    if (get_global_status(strings[index_var_asgn]) == 0) {
      get_offset(strings[index_var_asgn], &offset);
      fprintf(output, "\tSTR R0, FP, %d ; write %s\n", offset, strings[index_var_asgn]);
    }
    else {
      get_offset(strings[index_var_asgn], &offset);
      fprintf(output, "\tSTR R0, GP, %d ; write global %s\n", offset, strings[index_var_asgn]);
    }

  }
}

// CASE 3 return statement
process_case3(FILE *output, char *strings[]) {
  
  int offset = 0;
  int i = 1;
  while (strlen(strings[i]) != 0) {

    // if current token is letter
    if (is_valid_word(strings[i])) {
      process_letter_token(output, strings, offset, i);
    }

    // if current token is number
    if (is_valid_number(strings[i])) {
      process_number_token(output, strings, i);
    }

    i += 2;
  }

  fprintf(output, "\tSTR R0, FP, 3 ; write return value\n");
}

/********************************* helpers *********************************/

process_letter_token(FILE *output, char *strings[], int offset, int i) {

    if (strcmp(strings[i - 1], "=") == 0 || 
        strcmp(strings[i - 1], "return") == 0) {

      if (get_global_status(strings[i]) == 0) {
        get_offset(strings[i], &offset);
        fprintf(output, "\tLDR R0, FP, %d ; read %s\n", offset, strings[i]);
      }
      else {
        get_offset(strings[i], &offset);
        fprintf(output, "\tLDR R0, GP, %d ; read global %s\n", offset, strings[i]);
      }

    }

    if(strcmp(strings[i - 1], "+") == 0) {

      if (get_global_status(strings[i]) == 0) {
        get_offset(strings[i], &offset);
        fprintf(output, "\tLDR R1, FP, %d ; read %s\n", offset, strings[i]);
      }
      else {
        get_offset(strings[i], &offset);
        fprintf(output, "\tLDR R1, GP, %d ; read global %s\n", offset, strings[i]);
      }
      fprintf(output, "\tADD R0, R0, R1\n");

    }

    if(strcmp(strings[i - 1], "-") == 0) {

      if (get_global_status(strings[i]) == 0) {
        get_offset(strings[i], &offset);
        fprintf(output, "\tLDR R1, FP, %d ; read %s\n", offset, strings[i]);
      }
      else {
        get_offset(strings[i], &offset);
        fprintf(output, "\tLDR R1, GP, %d ; read global %s\n", offset, strings[i]);
      }
      fprintf(output, "\tNOT R1, R1\n");
      fprintf(output, "\tADD R1, R1, 1\n");
      fprintf(output, "\tADD R0, R0, R1\n");
    }
}

process_number_token(FILE *output, char *strings[], int i) {

    if (strcmp(strings[i - 1], "=") == 0 || 
        strcmp(strings[i - 1], "return") == 0) {
      fprintf(output, "\tAND R0, R0, 0\n");
      int value = atoi(strings[i]);

      // if immediate value is larger than +15
      if (value >= 0) {
        if (value <= 15)
          fprintf(output, "\tADD R0, R0, %d\n", value);
        else {
          while (value > 15) {
            fprintf(output, "\tADD R0, R0, %d\n", 15);
            value -= 15;
          }
          fprintf(output, "\tADD R0, R0, %d\n", value);
        }
      }

      if (value < 0) {
        // if immediate value is smaller than -16
        if (value >= -16 && value <= 0)
          fprintf(output, "\tADD R0, R0, %d\n", value);
        else {
          while (value < -16) {
            fprintf(output, "\tADD R0, R0, %d\n", -16);
            value += 16;
          }
          fprintf(output, "\tADD R0, R0, %d\n", value);
        }
      }

    }

    if(strcmp(strings[i - 1], "+") == 0) {
      fprintf(output, "\tAND R1, R1, 0\n");
      int value = atoi(strings[i]);

      // if immediate value is larger than +15
      if (value <= 15 && value >= 0)
        fprintf(output, "\tADD R1, R1, %d\n", value);
      else {
        while (value > 15) {
          fprintf(output, "\tADD R1, R1, %d\n", 15);
          value -= 15;
        }
        fprintf(output, "\tADD R1, R1, %d\n", value);
      }
      // add last addend to the running sum
      fprintf(output, "\tADD R0, R0, R1\n");
    }

    if(strcmp(strings[i - 1], "-") == 0) {
      fprintf(output, "\tAND R1, R1, 0\n");
      int value = atoi(strings[i]);

      // if immediate value is smaller than -16
      if (value <= 16 && value >= 0)
        fprintf(output, "\tADD R1, R1, %d\n", -value);
      else {
        while (value > 16) {
          fprintf(output, "\tADD R1, R1, %d\n", -16);
          value -= 16;
        }
        fprintf(output, "\tADD R1, R1, %d\n", -value);
      }
      // add last addend to the running sum
      fprintf(output, "\tADD R0, R0, R1\n");
    }
    
}

// returns 1 if this line has no assignment statement;
// 0 otherwise
int has_no_assignment(char *line) {
  int i = 0;
  while (line[i] != 0) {
    if (line[i] == '=') return 0;
    i++;
  }
  return 1;
}

// returns the index of first var being assigned to another var
// beginning at specific index; -1 if no such var found
int find_first_var_initialized(char *strings[], int i) {
  while (strings[i] != 0) {
    if (strcmp(strings[i - 1], "=") == 0) return i;
    i++;
  }
  return -1;
}

/*********************** helpers for handling function call ***********************/

process_func_call_word(FILE *output, char *strings[], int offset, int i) {
    if (strings[i][0] == '(') {
      get_offset(strings[i], &offset);
      fprintf(output, "\tLDR R0, FP, %d ; read %s\n", offset, strings[i]);
    }
    else {
      get_offset(strings[i], &offset);
      fprintf(output, "\tLDR R0, FP, %d ; read %s\n", offset, strings[i]);
    }
    fprintf(output, "\tADD SP, SP, -1 ; push variable argument\n");
    fprintf(output, "\tSTR R0, SP, 0\n");
}

process_func_call_num(FILE *output, char *strings[], int i) {
    fprintf(output, "\tAND R0, R0, 0\n");
    int value = atoi(strings[i]);

    // if immediate value is larger than +15
    if (value >= 0) {
      if (value <= 15)
        fprintf(output, "\tADD R0, R0, %d\n", value);
      else {
        while (value > 15) {
          fprintf(output, "\tADD R0, R0, %d\n", 15);
          value -= 15;
        }
        fprintf(output, "\tADD R0, R0, %d\n", value);
      }
    }

    if (value < 0) {
      // if immediate value is smaller than -16
      if (value >= -16 && value <= 0)
        fprintf(output, "\tADD R0, R0, %d\n", value);
      else {
        while (value < -16) {
          fprintf(output, "\tADD R0, R0, %d\n", -16);
          value += 16;
        }
        fprintf(output, "\tADD R0, R0, %d\n", value);
      }
    }
    fprintf(output, "\tADD SP, SP, -1 ; push integer literal argument\n");
    fprintf(output, "\tSTR R0, SP, 0\n");
}

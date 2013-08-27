/* Zhishen Wen
 * Dec. 10, 2012
 * MCIT, Penn CIS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int offset_body = 0; // global var to count offsets in parse_line
int offset_global = 0; // global var to count offsets in parse_global_var

int parse_global_var(char *line) 
{
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
  char *strings[] = {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9};
  
  int numWords = tokenize(line, strings);
  int i;

  if (strcmp(strings[0], "int") != 0) return 0;
  else {
    for (i = 0; i < 10; i++) {
      if (strlen(strings[i]) != 0 && is_valid_word(strings[i])){
        add_node(strings[i], offset_global, 1);
        offset_global++;
      }
    }
  }

  return 1;
}

int parse_function_header(char *header)
{
  if (header == NULL) return 0;

  // these are the 10 strings that you will populate
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
  char *strings[] = {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9};
  
  int numWords = tokenize(header, strings);

  //----------------------------------------------------------------------
  // Check whether any params declared multiple times. If not, populate
  // the symbol table; otherwise, display an error message and return 0.

  if (strcmp(strings[2], "()") == 0) return 1;
  int i, offset_head = 4;
  for (i = 3; i < 10; i++) {
    if (strcmp(strings[i], "int") != 0 && strcmp(strings[i], "{") != 0)
      if (strlen(strings[i]) != 0) {
        if (has_name(strings[i])) {
          printf("ERROR:\nDuplicate declaration for var \'%s\' ", strings[i]);
          return 0;
        }
        add_node(strings[i], offset_head, 0);
        offset_head++;
      }
  }
  //----------------------------------------------------------------------

  return 1;
}



int parse_line(char *line)
{
  if (line == NULL) return 0;

  // these are the 10 strings that you will populate
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

  //----------------------------------------------------------------------
  // Check whether any undeclared vars are being used. If not, populate
  // the symbol table; otherwise, display an error message and return 0.

  if (!has_function_call(line)) {
    int i;
    // parse a line that does not start with string 'int'.
    if(strcmp(strings[0], "int") != 0) {
      for (i = 0; i < 10; i++)
        if (strcmp(strings[i], "=") != 0 && strcmp(strings[i], "+") != 0 && strcmp(strings[i], "-") != 0 &&
          strcmp(strings[i], "}") != 0 && strcmp(strings[i], "return") != 0 &&
          !is_valid_number(strings[i]))
          if (strlen(strings[i]) != 0 && strings[i][0] != '-') {
            if (!has_name(strings[i])) {
              printf("===========================================\n");
              printf("ERROR:\nUndeclared var \'%s\' being used ", strings[i]);
              return 0;
            }
          }
    }

    // parse a line that starts with string 'int'.
    if(strcmp(strings[0], "int") == 0) {
      
      for (i = 1; i < 10; i++) {
        
        if (strcmp(strings[i], "=") != 0 && strcmp(strings[i], "+") != 0 && strcmp(strings[i], "-") != 0 &&
          !is_valid_number(strings[i]) && 
          (strcmp(strings[i - 1], "=") == 0 || 
          ((strcmp(strings[i - 1], "+") == 0 || strcmp(strings[i + 1], "+") == 0) ||
          (strcmp(strings[i - 1], "-") == 0 || strcmp(strings[i + 1], "-") == 0))))
          if (strlen(strings[i]) != 0 && strings[i][0] != '-')
            if (!has_name(strings[i])) {
              printf("===========================================\n");
              printf("ERROR:\nUndeclared var \'%s\' being used ", strings[i]);
              return 0;
            }
        
        // Check whether any vars declared multiple times. If not, populate
        // the symbol table; otherwise, display an error message and return -1.

        if ((strcmp(strings[i],
          (strcmp(strings[i - 1], "+") != 0 && strcmp(strings[i + 1], "+") != 0) &&
          (strcmp(strings[i - 1], "-") != 0 && strcmp(strings[i + 1], "-") != 0)) || 
          strcmp(strings[i + 1], "=") == 0))
          if (strlen(strings[i]) != 0) {
            if (has_name(strings[i])) {
              printf("===========================================\n");
              printf("ERROR:\nDuplicate declaration for var \'%s\' ", strings[i]);
              return -1;
            }
            add_node(strings[i], offset_body, 0);
            offset_body--;
          }
      }
    }

  }
  
  //----------------------------------------------------------------------

  return 1;
}

// Checks whether a string purely contains digits, commas, and semicolons;
// returns 0 if not; 1 otherwise.
int is_valid_number(char *s) {
  int i = 0;
  while (s[i] != 0) {
    if (!isdigit(s[i]) && s[i] != ',' && s[i] != ';' && s[i] != '-') return 0;
    i++;
  }
  return 1;
}

// Checks whether a string purely contains letters;
// returns 0 if not; 1 otherwise.
int is_valid_word(char *s) {
  int i = 0;
  while (s[i] != 0) {
    if (!is_letter(s[i])) return 0;
    i++;
  }
  return 1;
}

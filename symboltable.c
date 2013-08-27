/* Zhishen Wen
 * Dec. 10, 2012
 * MCIT, Penn CIS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function declarations */
char *trim(char *str);
FILE *open_file(char *name);

/*
 * This function takes in the name of a file as its argument
 * and then reads the file one line at a time.
 * The first line should be passed to parse_function_header.
 * The rest should be passed to parse_line.
 */ 
int create_symbol_table(char *name) {

  // open the file
  FILE *infile = open_file(name);
  if (infile == NULL) return 0;

  // 100-element character array to hold the line we're reading
  char line[110];

  // read the first line
  fgets(line, 110, infile);

  //-----------------------------------------------------
  int line_number = 1; // var to count line number

  // parse whether this line contains globals
  while (has_global_var(line)) {
    parse_global_var(trim(line));
    fgets(line, 110, infile);
  }

  // parse function header
  // if return value is 0, duplicate declaration error occurs
  if (parse_function_header(trim(line)) == 0) {
    printf("at line %d\n", line_number);
    fclose(infile);
    return 0;
  }

  // parse function body
  while (fgets(line, 110, infile) != NULL) {
    line_number++;
    //printf("READ: %s\n", line);

    /* 
    * Use a flag to indicate different kinds of errors;
    * flag is -1 when duplicate declaration error occurs;
    * flag is 0 when undeclared var error occurs.
    */
    if (!has_function_call(trim(line))) {
      int flag = parse_line(trim(line));
      if (flag == 0) {
        printf("at line %d\n", line_number);
        fclose(infile);
        return 0;
      }
      if (flag == -1) {
        printf("at line %d\n", line_number);
        fclose(infile);
        return 0;
      }
    }
  }
  fclose(infile); // close the file after using it
  //-----------------------------------------------------
  
  return 1;
 
}

/* This is a function to open a file. 
 * DO NOT CHANGE THIS CODE!!
 */
FILE *open_file(char *name) {
  
  if (name == NULL) {
    printf("Error! File name is null!\n");
    return NULL;
  }

  // open the file for reading
  FILE *infile = fopen(name, "r");

  if (infile == NULL) {
    printf("Error! Could not open file \"%s\" for reading\n", name);
    return NULL;
  }

  return infile;
}

/*
 * This function takes in a char pointer (presumably referring to a mutable
 * string) and removes any leading or trailing whitespace.
 * http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
 *
 * DO NOT CHANGE THIS CODE!!
 */
char *trim(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

/* trims first char of the input string */
char *trim_first(char *str)
{
  char *end;
  str++;
  if(*str == 0)  
    return str;
  end = str + strlen(str) - 1;
  *(end+1) = 0;
  return str;
}

/* returns 1 if this line contains global var;
 * 0 otherwise
 */
int has_global_var(char *line) {
  int i = 0;
  while (line[i] != 0) {
    if (line[i] == '{') return 0;
    i++;
  }
  return 1;
}

/* returns 1 if this line has function call command;
 * 0 otherwise
 */
int has_function_call(char *line) {
  int i = 0;
  while (line[i] != 0) {
    if (line[i] == '(') return 1;
    i++;
  }
  return 0;
}

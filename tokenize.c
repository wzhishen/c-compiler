/* Zhishen Wen
 * Dec. 10, 2012
 * MCIT, Penn CIS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_letter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');

}

int tokenize(char *s, char *strings[]) {
  if (s == NULL || strcmp(s, "") == 0 || strings == NULL) {
    int j;
    for (j = 0; j < 10; j++) {
      strings[j][0] = 0;
    }
    return 0;
  }

  int count = 0;

  int i = 0; // index into string s
  int word_index = 0; // index into word we're constructing
  int starts_with_letter = 0; 
  while (s[i] != 0) {

    // see if it starts with a letter
    if (word_index == 0 && is_letter(s[i])) {
      starts_with_letter = 1;
    }

    if (s[i] == ' ') {
      strings[count][word_index] = 0; // null-terminate
      word_index = 0;
      count++;
      starts_with_letter = 0;
    }
    else {
      if ((!starts_with_letter && s[i] != ',' && s[i] != ';' && s[i] != ')') || 
        (starts_with_letter && (is_letter(s[i]) || s[i] == '_' || '0' < s[i] && s[i] < '9'))) {
      	strings[count][word_index] = s[i];
      	word_index++;
      }
    }

    i++;

  }
  strings[count][word_index] = 0;

  int retVal = count+1;

  count++;

  for (; count < 10; count++)
    strings[count][0] = 0;

  return retVal;
}


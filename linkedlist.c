/* Zhishen Wen
 * Dec. 10, 2012
 * MCIT, Penn CIS
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * DO NOT CHANGE THIS DEFINITION!
 */
typedef struct Node node;

struct Node {
  char name[10];
  int offset;
  int is_global; // determines whether this node is 'global'
  node *next;
};

// global pointer to first Node of Linked List
struct Node *head;


/*
  Adds a new node to the linked list, with name set to parameter "s" and offset set to parameter "v".
  Returns 1 if successful, 0 if an error occurred.
*/
int add_node(char *s, int v, int g)
{
	node *new = malloc(sizeof(node));
  if (new == NULL) return 0;
  strcpy(new->name, s);
  new->offset = v;
  new->is_global = g;
  new->next = head;
  head = new;
  return 1;
}

/*
  Finds the offset of the node whose name is equal to the first argument.
  If the node is found, its offset value is put into the "v" parameter, and the function returns 1.
  If an error occurs (e.g. there is no node with that name), the function returns 0.
*/
int get_offset(char *s, int *v)
{
	node *n = head;
  while(n != NULL) {
    if(strcmp(s, n->name) == 0) {
      *v = n->offset;
      return 1;
    }
    n = n->next;
  }
  return 0;
}

/* determines whether node whose name is equal to the argument is 'global';
 * returns 1 if it is;
 * returns 0 if not;
 * returns -1 if no such node found
 */
int get_global_status(char *s)
{
  node *n = head;
  while(n != NULL) {
    if(strcmp(s, n->name) == 0) {
      return n->is_global;
    }
    n = n->next;
  }
  return -1;
}

// Checks whether the linked list already has the name 's';
// returns 0 if not; 1 otherwise.
int has_name(char *s)
{
  node *n = head;
  while (n != NULL) {
    if(strcmp(s, n->name) == 0) 
      return 1;
    n = n->next;
  }
  return 0;
}

/* gets the total number of nodes that
 * contains non-global var in the linked list
 */
int get_list_length() 
{
  node *n = head;
  int cnt = 0;
  while (n != NULL) {
    if (n->is_global == 0)
      cnt++;
    n = n->next;
  }
  return cnt;
}

/* gets the number of nodes that contains
 * local var in the linked list
 */
int get_local_var_number() 
{
  node *n = head;
  int cnt = 0; 
  while (n != NULL) {
    if (n->is_global == 0) {
      cnt++;
      if (n->offset == 0) return cnt;
    }
    n = n->next;
  }
  return 0;
}

/* gets the number of nodes that contains
 * param in the linked list
 */
int get_param_number() 
{
  return get_list_length() - get_local_var_number();
}

/*
 Print out (to the screen) the name and offset value for each node
 in the linked list.
*/
void iterate() 
{
  node *n = head;
  while (n != NULL) {
    printf("Name: %s\t\tOffset: %d\n", n->name, n->offset);
    n = n->next;
  }
  
  printf("%d\n", get_list_length());
  printf("%d\n", get_param_number());
  printf("%d\n", get_local_var_number());
}

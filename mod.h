#ifndef MOD_H
#define MOD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/wait.h>
#define N 10
#define u_ampersand 1
#define d_ampersand 2
#define u_pipe 3
#define d_pipe 4
#define consecutive 5

typedef struct cmd_inf* tree;
struct cmd_inf {
    char **argv;
    char *infile;
    char *outfile;
    int backgrnd;
    tree psubcmd;
    tree pipe;
    tree next;
    int append;
    char next_type;
	// 1 - '&'
	// 2 - '&&'
	// 3 - '|'
	// 4 - '||'
	// 5 - ';'
};

typedef struct node* dict;
struct node {
  char *elem;
  dict next;
};

void save_io(void);
void load_io(void);
dict buildlist(void);
dict add(dict l, char *s);
int size(dict l);
void write_list(dict l);
void sort(dict *l);
void del(dict l);
void wait_zomby(void);
void error(void *p,dict l);

void clear_tree(tree proc);
tree build_tree(dict lst);
void p_tree(tree proc);
int exec_tree( tree proc );
void change(dict l);
tree whotoexec(tree t,int st);
void push_pipe(pid_t pid);
void wait_pipe(void);
int check(dict l);
#endif

#include "mod.h"
extern int fd_save_io__[2];
extern pid_t bckgrndpids[100];
extern int last_of_us ;
extern pid_t pipepids[100];
int main(int argc, char ** argv) {
  tree cmnd = NULL;
  dict l = NULL;
  int i = 0;
  for ( i = 0; i < 100; bckgrndpids[i ++] = -1);
  for ( i = 0; i < 100; pipepids[i  ++]=-1);
  save_io();

  while (1) {
    wait_pipe();
    del(l);
    clear_tree(cmnd);
    wait_zomby();
    fprintf(stderr,"==> ");
    l = buildlist();
    if (!check(l))
    {
    change(l);
    if ( l == NULL ) break;
 //   printf("writelist\n");
 //   write_list(l);
    cmnd = build_tree(l);
  //  printf("\nptree\n");
   // p_tree(cmnd);
   // printf("\nexec\n");
    exec_tree(cmnd);
    load_io();
    }
  }
  printf("Shell stopped working...\n");
  return 0;
}

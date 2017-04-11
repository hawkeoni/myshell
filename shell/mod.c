#include "mod.h"
int fd_save_io__[2] = {-1,-1};
pid_t bckgrndpids[100];
pid_t pipepids[100];
int last_of_us = 0 ;
dict nothing;
dict buildlist(void)
{
    //static int flag=0;
    dict l;
    l=NULL;
    char *s ;   // we use it to contain litter`s string
    int i,k=1,j=0;
    char buf[N+1];
    char quotes_flag = 0;
       
    static dict nothing;
    nothing = calloc(1,sizeof(struct node));
    nothing->elem=calloc(1,sizeof(char));
    nothing->elem[0]='\0';
    nothing->next=NULL;
    while ((fgets(buf,N,stdin))!=NULL)
    {
      if (buf[0]=='\n'&&l==NULL&&j==0) {return nothing;}
        for (i=0;buf[i]!='\0';i++)
        {   
            if( quotes_flag && buf[i] != '\"') { 
		if ( j == 0 ) {
		    s = calloc ( ( k = 1 ) + 1, 1 );
		    s [ 0 ] = buf[ i ];
		    j = 1;
		}
		else { 
		    s = ( char * ) realloc( s, ++k + 1 );
		    s [ j ] = buf[ i ];
		    s [ j + 1 ] = '\0';
		    j ++;
	        }
                continue;
            }
            switch (buf[i])
            {
                case '\n':
		    if ( j != 0 ) 
  		    l = add( l , s );
                    del(nothing);
		    return l;
                    break;

                case '\t':
		case ' ' :
                    if ( j != 0) {
			l = add( l , s );	
			j = 0;
		    }
		    break;

                case ';':
		case '<':  
		case '(': 
		case ')':
                    if ( j != 0 ) {
			l = add( l, s );
			j = 0 ;
		    }
		    s = ( char * ) calloc( ( k = 1 ) + 1, 1 );
		    s[ 0 ] = buf[ i ];
		    l = add( l, s );
		    j = 0;
		    break;

                case '|': 
		case '&': 
		case '>': 
		    if ( j == 0 ) {
		    	s = ( char * ) calloc( ( k = 1 ) + 2, 1 ) ;
			s [ 0 ] = buf[ i ];
			j = -1;
			break;
		    }
                    if ( j < 0 ) {
			if( s [ 0 ] == buf [ i ] ) {
			    s [ 1 ] = buf[ i ];
                            l = add( l, s );
			    j = 0;
			    break;
			}
                        else {
			    l = add( l, s ); 
			    s = ( char * ) calloc( ( k = 1 ) + 2, 1 );
                            s [ 0 ] = buf[ i ];
		    	    j = -1;
			    break;
		    	}
		    }
                    if ( j > 0 ) {
			l = add( l, s );
			j = -1;
			s = ( char * ) calloc( ( k = 1 ) + 2, 1 );
                        s [ 0 ] = buf[ i ];
			break;
		    }
		case '\"': 
                    quotes_flag = !quotes_flag;
                    if( j != 0 ) {
			l = add( l , s );
			j = 0;
                    }
                    break;
                default:
                    if ( j < 0 ) {
			l = add( l , s );
			j = 0;
		    }
		    if ( j == 0 ) {
			s = calloc ( ( k = 1 ) + 1, 1 );
			s [ 0 ] = buf[ i ];
			j = 1;
		    }
		    else { 
		    	s = ( char * ) realloc( s, ++k + 1 );
		    	s [ j ] = buf[ i ];
			s [ j + 1 ] = '\0';
		    	j ++;
		    }
            }
        }
    }
    del(nothing);
    return l;
}

void wait_zomby(void) {
    int i = 0;
    for( i = 0; i < 100; i ++) {
		if ( bckgrndpids[i] > 0 ) 
	   		if( waitpid( bckgrndpids[i], NULL,WNOHANG))
        		bckgrndpids[i] = -1;
    }
}

void push_zomby(pid_t pid_) {
    int i = 0;
    for( i = 0; i < 100; i ++) 
		if ( bckgrndpids[i] == -1 ) { 
            bckgrndpids[i] = pid_;
	    	break;
    	}
}
void push_pipe(pid_t pid)
{
  int i=0;
  for (i=0;i<100;i++)
   if (pipepids[i]==-1) {pipepids[i]=pid; break;}
 // printf("%d %d here are pids\n",i,pipepids[i]);
}
void wait_pipe(void)
{
  int i=0;
  for (i=0;i<100;i++){
   // printf("%d\n",pipepids[i]);
    if (pipepids[i]>0) {
    waitpid(pipepids[i],NULL,0);
   // printf("waited for conveyyer\n");
    pipepids[i]=-1;
    }
    }
    }
dict add(dict l,char *s) //adding the next element to a dictionary
{
  if (l==NULL)
  {
    l=calloc(1,sizeof(struct node));
    if (l==NULL) {
	printf("Error, can not allocate memory\n");
	exit(0);
    } 
    l->elem=s;
    l->next=NULL;
  }
  else 
    l->next=add(l->next,s);
  return l;
}

void write_list(dict l)  //writing the words
{  
  if (l!=NULL)
  {
    printf("%s ",l->elem);
    write_list(l->next);
  }
return;
}

	//dictionary removal
void del(dict l) {
dict p;
  while (l!=NULL)
  {
    p=l;
    l=l->next;
    free(p->elem);
    free(p);
  }
}

void clear_tree( tree proc) {
    if(proc == NULL)
		return;
    free(proc -> argv);
    clear_tree(proc -> psubcmd);
    clear_tree(proc -> pipe);
    clear_tree(proc -> next);
    free(proc);
}

void save_io(void) {
    if( fd_save_io__[0] == -1 )
	fd_save_io__[0]  = dup(0);
    else
        dup2(0 , fd_save_io__[0]);

    if( fd_save_io__[1] == -1 )
	fd_save_io__[1]  = dup(1);
    else
        dup2(1 , fd_save_io__[1]);
}

void load_io(void) {
    dup2(fd_save_io__[0], 0);
    dup2(fd_save_io__[1], 1);
}

tree build_tree(dict lst) {
    if (lst == NULL)
	return NULL;
    if ( lst -> elem[0] == '\0' )
	return NULL;
    tree proc = calloc(1, sizeof( struct cmd_inf ) );
    int i = 0; // use it for argv
    for(i = 0; lst != NULL;) {
        switch ( lst -> elem[0] ) {
	    case '>':
	    	if( lst -> elem[1] == '>' )
		    proc -> append = 1;
	        else
		    proc -> append = 0;

	        if(lst -> next == NULL) {
		    printf("expected file after '>'\n");
		    clear_tree(proc);
		    return NULL;
	        }
	        proc -> outfile = lst -> next -> elem;
	        lst = lst -> next -> next; 
	        break;
	
	    case '<':
	        if(lst -> next == NULL) {
	    	    printf("expected file after '<'\n");
		    clear_tree(proc);
		    return NULL;
	        }
	        proc -> infile = lst -> next -> elem;
	        lst = lst -> next -> next; 
	        break;

	    case '&':
		if( lst -> elem[1] == '&' ) {
		    proc -> next_type = d_ampersand ;
		    proc -> next = build_tree( lst -> next );
		    return proc;
		}
		proc -> next_type = u_ampersand;
		proc -> backgrnd = 1;
		proc -> next = build_tree( lst -> next ); 
		return proc;
		break;
	    
	    case '|':
		if( lst -> elem[1] == '|') {
		    proc -> next_type = d_pipe;
		    proc -> next = build_tree( lst -> next );
		    return proc;
		}
		proc -> next_type = u_pipe;
		proc -> pipe = build_tree( lst -> next );
		return proc;
		break;

	    case ';':
		proc -> next_type = consecutive;
		proc -> next = build_tree( lst -> next );
		return proc;
		break;

	    case '(':
		free(proc -> argv);
		proc -> argv = NULL;
		proc -> psubcmd = build_tree ( lst -> next );
		int br_num = 1;
		lst = lst -> next;
		while ( br_num ) {
		    if( lst -> elem[0] == '(')
			br_num ++;
		    if( lst -> elem[0] == ')')
			br_num --;
		    lst = lst -> next;
		}
		break;

	    case ')':
		return proc;
		break;		

	    default:
		proc -> argv = realloc( proc -> argv, (i + 2) * sizeof(char*) );
		proc -> argv[ i ] = lst -> elem;
		proc -> argv[i+1] = NULL;
		i ++; 
		lst = lst -> next;
		break;
	}
    }
    return proc;
}

void p_next_type( char type_ ) {
    switch ( type_ ) {
	    case u_ampersand:	    
		printf("& ");
		break;

	    case d_ampersand:	    
		printf("&& ");
		break;

	    case u_pipe:	    
		printf("| ");
		break;

	    case d_pipe:	    
		printf("|| ");
		break;

	    case consecutive:	    
		printf("; ");
		break;
	    default:
		break;
	}
}

void p_tree(tree proc) {
    if( proc == NULL)
	return;
    if( proc -> argv != NULL ) {
    	int i = 0;
    	for(i = 0; proc->argv[i] != NULL; i ++) 
	    printf("%s ", proc->argv[i]);
    }

    if(proc->infile != NULL)
	printf("< %s ", proc->infile);
    if(proc->outfile != NULL) {
	if(proc->append)
	    printf(">> %s ", proc->outfile);
	else
	    printf( "> %s ", proc->outfile);
    }

    if(proc->psubcmd != NULL) {
	printf("( ");
    	p_tree(proc->psubcmd);
	printf(") ");
    }
    if(	proc->pipe != NULL) {
	printf("| ");
	p_tree(proc->pipe);
    }
    if( proc->next != NULL) {
	p_next_type( proc -> next_type );
    	p_tree(proc->next);
    }
    else 
	if( ( proc -> next_type != u_pipe) ||
	    ( proc -> pipe == NULL &&  proc -> next_type == u_pipe ) )
	    p_next_type( proc -> next_type );
}

int load_io_from_parameters(tree proc) {
    int fd_in, fd_out;
    if( proc -> infile != NULL ) {
	fd_in = open( proc -> infile , O_RDONLY );
	if( fd_in == -1 ) { 
	    printf("wrong input file name \n");
	    return -1;
	}
	dup2( fd_in, 0);
    }

    if( proc -> outfile != NULL ) {
    	if( proc -> append ) 
    		fd_out = open( proc -> outfile , O_WRONLY | O_CREAT | O_APPEND, 0666 );
    	else
  			fd_out = open( proc -> outfile , O_WRONLY | O_TRUNC | O_CREAT, 0666 );
  	if( fd_out == -1 ) { 
	    printf("wrong output file name \n");
	    return -1;
	}
        dup2(fd_out, 1);
    }
    return 0;
}

	// it would be recursive fnction
	// we will create new "shell" processes only when we see, that psubcmd != NULL
	// every pro will execute in new process
	// get new step of recursion e
int exec_tree( tree proc ) { 
    int des;
    if (proc==NULL) return 0;
    if (proc->argv!=NULL)
    if (!strcmp(proc->argv[0],"cd"))
		  {
              if (proc->argv[1]==NULL) {chdir("/users/hawke");}
              else (chdir(proc->argv[1]));
              load_io();
              if (proc->pipe!=NULL) {des=open("/dev/null",O_RDONLY);dup2(des,0); close(des); return exec_tree(proc->pipe);}
              else { return exec_tree(proc->next);}
          }
    int fdp[2]; // we use it as pipe
    pid_t son1;
    int status[1];
    status[0] = 0;

    switch ( proc -> next_type ) {

	case u_ampersand:
	    if ( ( son1 = fork() ) == -1 )
		return -1;
	    if ( !son1 ) {
		signal(SIGINT,SIG_IGN);
		if( load_io_from_parameters( proc ) == -1 )
		    exit(1);
		if( proc -> argv == NULL) {
                    save_io();
		    exit( exec_tree( proc -> psubcmd ) );
                }
		else
		    execvp( proc -> argv[0], proc -> argv );
	        printf("I shouldn't be here!\n");
		perror("execvp");
		exit(1);
	    }
	    push_zomby(son1);
	    return exec_tree( proc -> next ) ;
	    break;

	case d_ampersand:
	    if ( ( son1 = fork() ) == -1 )
		return -1;
	    if ( !son1 ) {
		if( load_io_from_parameters( proc ) == -1 )
		    exit(1);
		if( proc -> argv == NULL) {
                    save_io();
		    exit( exec_tree( proc -> psubcmd ) );
                }
		else
		    execvp( proc -> argv[0], proc -> argv );
		perror("execvp");
		exit(1);
	    }

	    waitpid( son1, status, 0 );
	    load_io();
            return (exec_tree(whotoexec(proc,*status)));
/*	    if( WIFEXITED(*status) && !WEXITSTATUS(*status) ) 
	    	return exec_tree( proc -> next );
	    else
	    	return 0; */
	    break;

	case u_pipe:
	    if( pipe(fdp) == -1 )
		return -1;
	    
	    if ( ( son1 = fork() ) == -1 )
		return -1;
	    if ( !son1 ) {
		signal(SIGINT,SIG_DFL);

		dup2( fdp[1], 1 );
		close( fdp[1] );
		close( fdp[0] );
		if( proc -> argv == NULL) 
                    save_io();
		if( load_io_from_parameters( proc ) == -1 )
		    exit(1);
 
		if( proc -> argv == NULL) {
		    exit( exec_tree( proc -> psubcmd ) );
                }
		else
		    execvp( proc -> argv[0], proc -> argv );
		perror("execvp");
		exit(1);
	    }
	    push_pipe(son1);

	    dup2( fdp[0], 0 );
	    close( fdp[0] );
	    close( fdp[1] );
	    return exec_tree( proc -> pipe );
	    break;

	case d_pipe:
	    if ( ( son1 = fork() ) == -1 )
		return -1;
	    if ( !son1 ) {
		if( load_io_from_parameters( proc ) == -1 )
		    exit(1);
		if( proc -> argv == NULL) {
                    save_io();
		    exit( exec_tree( proc -> psubcmd ) );
                }
		else
		    execvp( proc -> argv[0], proc -> argv );
		perror("execvp");
		exit(1);
	    }

	    waitpid( son1, status, 0 );
	    load_io();
            return (exec_tree(whotoexec(proc,*status)));
/*	    if(!WIFEXITED(*status) || WEXITSTATUS(*status)) 
	    	return exec_tree( proc -> next );
	    else
	    	return 0;  */
	    break;

	case consecutive:
	    if ( ( son1 = fork() ) == -1 )
		return -1;
	    if ( !son1 ) {
		if( load_io_from_parameters( proc ) == -1 )
		    exit(1);

		if( proc -> argv == NULL) {
                    save_io();
		    exit( exec_tree( proc -> psubcmd ) );
                }
		else
		    execvp( proc -> argv[0], proc -> argv );
		perror("execvp");
		exit(1);
	    }

	    waitpid( son1, status, 0 );
	    if(!WIFEXITED(*status)) {
  	    	printf("a)smth was bad on program lvl \n");
		return -1;
	    }
	    if(WEXITSTATUS(*status)) {
  	    	printf("smth was bad on program lvl \n");
		return -1;
	    }

	    load_io();
	    return exec_tree( proc -> next );
 	    break;

	default:
	    if( proc -> next == NULL &&
		proc -> psubcmd == NULL &&
		proc -> pipe == NULL ) {
	        
		if( proc -> argv == NULL) { 
	    	    printf("\nhome: smth were wrong\n");
	    	    return -1;
		}
		else {
		    
	    	    if ( ( son1 = fork() ) == -1 )
			return -1;
	    	    if ( !son1 ) {
			if( load_io_from_parameters( proc ) == -1 )
		    	    exit(1);
		    	execvp( proc -> argv[0], proc -> argv );
			perror("execvp");
			exit(1);
	    	    }
		}

	    	waitpid (son1, status, 0); // we need to check for correct or incorrect execution
	    	if(!WIFEXITED(*status)) { 	
  	    	    printf("a)smth was bad on program lvl \n");
		    return -1;
	    	}
	    	if(WEXITSTATUS(*status)) {
  	    	    printf("a)smth was bad on program lvl \n");
		    return -1;
	    	}

		load_io();
		return 0;

	    }
	    else {
		if( proc -> psubcmd != NULL) {

	    	    if ( ( son1 = fork() ) == -1 )
		    	return -1;
	    	    if ( !son1 ) {
			if( load_io_from_parameters( proc ) == -1 )
		    	    exit(1);
                        save_io();
		        exit( exec_tree( proc -> psubcmd ) );
		    }
			
	    	    waitpid (son1, status, 0); // we need to check for correct or incorrect execution
	    	    if(!WIFEXITED(*status)) {
  	    	    	printf("a)smth was bad on program lvl \n");
    		    	return -1;
	    	    }
	    	    if(WEXITSTATUS(*status)) {
  	    		printf("a)smth was bad on program lvl \n");
			return -1;
	    	    }

		    load_io();
		    return 0;
		}
		else
		    return -1;
	    }
	    break;
    }
    

    load_io();
    return 0;
}


tree whotoexec(tree t,int st)
{
    int l;
    if (t==NULL) return NULL;
    l=WIFEXITED(st)&&!WEXITSTATUS(st);
    if (t->next_type==u_ampersand)
        return (t->next);
//    if (t->next_type==0) return NULL;
    if (t->next_type==consecutive || t->next_type==0)
    {
    //    printf(";returning next\n");
     //   if (t->next==NULL) printf("it is null btw\n");
        return (t->next);
    }
    if (t->next_type==u_pipe)
        return whotoexec(t->pipe,st);
    if (t->next_type==d_ampersand)
    {
        if (!l) {return whotoexec(t->next,st);}
        else {return (t->next);}
    }
    if ( t->next_type==d_pipe)
    {
        if (l) {return whotoexec(t->next,st);}
        else {return (t->next); }
    }
    printf("men were never meant to be here\n");
}

void change(dict l)
{
    char *name;
    char *buf;
    buf=calloc(1024,sizeof(char));
    name=getwd(buf);
    struct passwd *p;
    struct stat sv;
    lstat("/users/hawke/desktop/shell/mod.h",&sv);
    p=getpwuid(sv.st_uid);
    while (l!=NULL)
    {
        if (!strcmp("$PATH",l->elem)) {free(l->elem);l->elem=calloc(45,sizeof(char));memcpy(l->elem,"/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin",45);}
        if (!strcmp("$USER",l->elem)) {free(l->elem);l->elem=calloc(45,sizeof(char));memcpy(l->elem,p->pw_name,strlen(p->pw_name));}
        if (!strcmp("$HOME",l->elem)) {free(l->elem);l->elem=calloc(45,sizeof(char));memcpy(l->elem,p->pw_dir,strlen(p->pw_dir));}
//      if (!strcmp("$PWD",l->elem)) {free(l->elem);l->elem=name;}
        if (!strcmp("$SHELL",l->elem)) {free(l->elem);l->elem=calloc(45,sizeof(char));memcpy(l->elem,p->pw_shell,strlen(p->pw_shell));}
        if (!strcmp("$EUID",l->elem)) {free(l->elem);l->elem=calloc(45,sizeof(char));sprintf(l->elem,"%d",p->pw_uid);}
        l=l->next;
    }
    free(buf);
}

int check(dict l)
{
  if (l==NULL) return 0;
  int br=0;
  while (l!=NULL)
  {
    if (l->elem[0]==')') br-=1;
    if (l->elem[0]=='(') br+=1;
    if (br<0) {printf("incorrect brackets\n");return 1; }
    l=l->next;
  }
  if (br!=0) {printf("incorrect brackets\n"); return 1; }
  return 0;
}

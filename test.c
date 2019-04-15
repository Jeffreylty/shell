    char *argv[MAXARGS];
    int bg;
    pid_t pid;
    sigset_t mask;


    bg = parseline(cmdline,argv);
    if(argv[0] == NULL)
        return ;
    if(!builtin_cmd(argv)){
        sigemptyset(&mask);
        sigaddset(&mask,SIGCHLD);
        sigaddset(&mask,SIGINT);
        sigaddset(&mask,SIGTSTP);
        sigprocmask(SIG_BLOCK,&mask,NULL);
        /* child proc */
        if((pid = Fork()) == 0){
            setpgid(0, 0);
            if(verbose){
                pid = getpid();
                printf("Child proc started with pid %d\n",(int)pid);
            }
            sigprocmask(SIG_UNBLOCK,&mask,NULL);
            if(execve(argv[0],argv,environ)<0){
                printf("%s: Command not found.\n",argv[0]);
                exit(0);
            }
        }
        /* parent proc */
        else{
            addjob(jobs,pid,bg?BG:FG,cmdline);
            sigprocmask(SIG_UNBLOCK,&mask,NULL);
            if(!bg){
                /* Use waitfg to wait until proc(pid) is no longer a frontgroud proc. */
                waitfg(pid);
            }
            else{
                printf("[%d] (%d) %s",pid2jid(pid),pid,cmdline);
            }
        }
    }
    return ;

void eval(char *cmdline) 
{
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;
    sigset_t mask;
    
    //copy and parse inpit
    strcpy(buf, cmdline);
    bg = parseline(buf, argv);

    if (argv[0] == NULL){//if empty line, return
        return;
    }
    
    if (!builtin_cmd(argv)){//if input is not built in cmd, fork()

        //block SIGCHLD
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, 0);
    
        if ((pid = fork()) < 0){//if unsuccess fork()
            printf("fork(): forking error\n");
            return;
        }
    
        if (pid == 0){
            setpgid(0,0);//set child group ID
            sigprocmask(SIG_UNBLOCK, &mask, 0);//unblock SIGCHLD

            if (execve(argv[0], argv, environ) < 0){
                printf("%s: Command not found. \n", argv[0]);
                exit(0);
            }
        }else{
            if (bg){//if bg
                addjob(jobs, pid, BG, cmdline);//add job to job list as bg
                sigprocmask(SIG_UNBLOCK, &mask, 0);//unblocks SIGCHLD
                printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);//print bg info
            }else{//if !bg
                addjob(jobs, pid, FG, cmdline);//add job to joblist as fg
                sigprocmask(SIG_UNBLOCK, &mask, 0);//unblocks SIGCHLD
                waitfg(pid);//parent waits for fg job to terminate
            }    
        }
    }
    return;
}

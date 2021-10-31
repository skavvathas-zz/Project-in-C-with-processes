#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc, char *argv[]){
	int p1, p2, pipefd[2], p3;
	int fp2_args, fp, fp2_in, olderr, oldout2, oldin2, oldin3;
	int status1, status2, status3, i = 0, j = 0, k = 0, f =0, compilation = 0, memory_access = 0, output = 0, total, plithos = 0, nm, read_ret, size_of_arg = 0;
	long int size_err = 0, size_args = 0, size;
	char *progname, *help, *prognameerr, *buffer, *path_prog, buf;
	struct stat sb;
	
	//argv[1] = <progname>.c
	//argv[2] = <progname>.args
	//argv[3] = <progname>.in
	//argv[4] = <progname>.out
	
	if(argc != 5){
		printf("\nThe number of argument is wrong\n");
		return(2);
	}
	else{
		size = strlen(argv[1]);
		help = (char *)malloc(size+1);//sizeof(argv[1]));
		memset(help,'\0',size*sizeof(char));
		strcpy(help,argv[1]);
		progname = (char *)malloc(size+1);
		memset(progname,'\0',size*sizeof(char));
		do{
			if(help[i] != '.'){
				progname[j++] = help[i];
			}
		}while(help[i++] != '.');
	}
	//progname = <progname>
	prognameerr = (char *)malloc(size + 3);
	memset(prognameerr,'\0',(size+3)*sizeof(char));
	
	path_prog = (char *)malloc(size+1);
	memset(path_prog,'\0',(size+1)*sizeof(char));
	
	strcpy(prognameerr,progname);
	strcpy(path_prog,"./");//./<progname>
	strcat(path_prog,progname);
	strcat(prognameerr,".err");
	
	fp = open(prognameerr,O_RDWR|O_CREAT,S_IRWXU);
	if(fp == -1){
		perror("\nopen");
	}
	
	p1 = fork();
	if(p1 == -1){
		perror("\nperror");
	}
	if(p1 == 0){
	
		olderr = dup(STDERR_FILENO);
		if(olderr == -1){
			perror("\ndup");
		}
		if(dup2(fp,STDERR_FILENO) == -1){
			perror("\ndup2");
		}
		nm = execl("/usr/bin/gcc","gcc","-Wall",argv[1],"-o",progname,NULL);
		if(nm == -1){//gcc -Wall <progname>.c -o <progname>
			perror("\nexecl 59");
		}
		if(dup2(olderr,STDERR_FILENO) == -1){
			perror("\ndup2");
		}
		exit(1);
	}
	
	if(waitpid(p1,&status1,0) == -1){
		perror("\nwaitpid");
	}
	
	if(stat(prognameerr,&sb) != -1){
		size_err = sb.st_size;
	}
	buffer = (char *)malloc(size_err+1);
	memset(buffer,'\0',(size_err+1)*sizeof(char));
	lseek(fp,0,SEEK_SET);
	read_ret = read(fp,buffer,size_err);
	if(read_ret == -1){
		perror("\nread");
	}
	if(strstr(buffer,"error:") != NULL){
		compilation -= 100;
	}
	if(strstr(buffer,"warning:") != NULL){
		if(compilation != -100){
			compilation -= 10;
		}		
	}
	
	if(compilation != -100){
			
		fp2_args = open(argv[2],O_RDONLY,S_IRWXU);//open the <progname>.args to save the arguments in an array
		if(fp2_args == -1){
			perror("\nopen");
		}
		if(stat(argv[2],&sb) != -1){
			size_args = sb.st_size;//the size of <progname>.args
		}
		do{
			f++;
			if(f < size_args){
				read(fp2_args,&buf,1);
				if(buf == ' '){
					plithos++;
				}
				if((buf == ' ') && (f == size_args - 1)){
					plithos -= 2;
				}
			}
		}while(f < size_args);
		
		if(size_args == 0){
			size_of_arg = 2;
		}
		else if((size_args > 0) && (plithos == 0)){
			size_of_arg = 3;
		}
		else{
			size_of_arg = plithos + 3;
		}
			
		char *arg[size_of_arg];
		char help1[size_of_arg][size_args];
			
		for(i = 0; i < size_of_arg; i++){
			for(j = 0; j < size_args; j++){
				help1[i][j] = '\0';
			}
		}
			
		if(size_args == 0){
			arg[0] = progname;
			arg[1] = NULL;
		}
		else{
			arg[0] = progname;
			i = 1;
			j = 0;
			f = 0;
			lseek(fp2_args,0,SEEK_SET);
			do{
				f++;
				if(f == size_args){
					arg[i] = help1[j];
				}
				else if(f < size_args){
					if(read(fp2_args,&buf,1) != -1){
						if(buf != ' '){
							if(j < size_of_arg){
								help1[j][k++] = buf;
							}
						}
						else if(buf == ' '){
							if(i < size_of_arg){
								arg[i] = help1[j];
								i++;
								j++;
								k = 0;
							}
						}
					}
				}
			}while(f < size_args);
			arg[size_of_arg - 1] = NULL;
		}
			
		if(pipe(pipefd) == -1){
			perror("\nperror");
		}

		p2 = fork();
		if(p2 == -1){
			perror("\nfork");
		}
		
		
		if(p2 == 0){
			oldout2 = dup(STDOUT_FILENO);
			if(oldout2 == -1){
				perror("\ndup");
			}
			if(dup2(pipefd[1],STDOUT_FILENO) == -1){
				perror("\ndup2");
			}
			close(pipefd[0]);
			close(pipefd[1]);
			fp2_in = open(argv[3],O_RDONLY,S_IRWXU);//open <progname>.in
			if(fp2_in == -1){
				perror("\nopen");
			}
			oldin2 = dup(STDIN_FILENO);
			if(oldin2 == -1){
				perror("\ndup");
			}
			if(dup2(fp2_in,STDIN_FILENO) == -1){
				perror("\ndup2");
			}
			if(execv(path_prog,arg) == -1){//run <progname>//WORKS
				perror("\nexecv 158");
			}
			
			exit(1);
		}

		p3 = fork();
		if(p3 == -1){
			perror("\nfork");
		}
		
		if(p3 == 0){
			oldin3 = dup(STDIN_FILENO);
			if(oldin3 == -1){
				perror("\ndup");
			}
			
			if(dup2(pipefd[0],STDIN_FILENO) == -1){
				perror("\ndup2");
			}
			close(pipefd[1]);
			close(pipefd[0]);
			if(execl("./p4diff","p4diff",argv[4],NULL) == -1){//run p4diff
				perror("\nexecl 179");
			}
			exit(1);
		}
		close(pipefd[0]);
		close(pipefd[1]);	
		if(waitpid(p2,&status2,0) == -1){
			perror("\nwaitpid");
		}
		if(WIFSIGNALED(status2)){
			if((WTERMSIG(status2) == SIGSEGV) || (WTERMSIG(status2) == SIGABRT) || (WTERMSIG(status2) == SIGBUS)){
				memory_access -= 15;
			}
		}
		if(waitpid(p3,&status3,0) == -1){
			perror("\nwaitpid");
		}
		if(WIFEXITED(status3)){
			output = WEXITSTATUS(status3);
		}
			
	}
	total = compilation + output + memory_access;
	if(total < 0){
		total = 0;
	}
	printf("Compilation: %d\nOutput: %d\nMemory access: %d\nTotal: %d\n", compilation, output, memory_access, total);
	
	free(prognameerr);
	free(buffer);
	free(help);
	free(progname);
	close(fp);
	
	return(0);
}

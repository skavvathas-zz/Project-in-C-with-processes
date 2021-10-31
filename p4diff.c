#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc, char *argv[]){
	int fp_out, i = 0, bytes_of_in = 0, mult_of_same_bytes = 0, bytes_of_pr_out = 0, max_bytes = 1, read_in, read_out, bytes_to_read = 64, x = 64, pos = 0, old_read_in; //,pos1 = 0, bytes_to_read_o = 0, old_read_out;
	char buffer[64] = {"\0"}, buff[64] = {"\0"};
	long int  result;
	struct stat sb;
	
	if(argc != 2){
		printf("\nWrong number of arguments in p4diff.c");
	}
	
	if(stat(argv[1],&sb) != -1){
		bytes_of_pr_out = sb.st_size;//how many bytes is the <progname>.out
	}
	fp_out = open(argv[1],O_RDONLY,S_IRWXU);
	if(fp_out == -1){
		perror("\nopen");
	}
	
	while(1){
		i = 0;
		read_out = read(fp_out,buffer,64);
		
		if(read_out == -1){//reads 64 bytes from <progname>.out
			perror("\nread");
		}

		read_in = read(STDIN_FILENO,buff,64);//read from the stdin
		
		bytes_of_in = bytes_of_in + read_in;
	
		if(read_in == 0){
			break;
		}
		else if(read_in == -1){
			perror("\nread");
		}
		else if(read_in != 64){//if p4diff doesnt read 64 bytes (the bytes i want)
			old_read_in = read_in;
			
			do{
				bytes_to_read -= read_in;
				pos += read_in;
				read_in = read(STDIN_FILENO,&buff[pos],bytes_to_read);
				bytes_of_in += read_in;
			
				if(read_in == -1){
					perror("\nread");
				}
				
				if(read_in == 0){
					read_in = read(STDIN_FILENO,&buff[pos],bytes_to_read);
					if(read_in == 0){
						break;
					}
					else{
						bytes_of_in += read_in;
					}
				}
				if(read_in == bytes_to_read){
					break;
				}
			}while(1);
			
			read_in = old_read_in + pos;
			
			if(read_out != 0){
			
				do{
					if(buffer[i] == '\0'){
						break;
					}
					if(buff[i] == '\0'){
						break;
					}
					if(buffer[i] == buff[i]){
						mult_of_same_bytes++;
					}
					i++;
				}while(i < 64);
			}
			
		}
		else{
			if(read_out != 0){
				if(read_out < read_in){
					x = read_out;
				}
				else if(read_out > read_in){
					x = read_in;
				}
				else{
					x = read_in;
				}
				do{
					if(buffer[i] == buff[i]){
						mult_of_same_bytes++;
					}
					i++;
				}while(i < x);
			}
		}
		bytes_to_read = 64;
		pos = 0;
		memset(buffer,'\0',64*sizeof(char));
		memset(buff,'\0',64*sizeof(char));
		bytes_to_read = 64;
	}
	
	if(bytes_of_pr_out > bytes_of_in){
		max_bytes = bytes_of_pr_out;
	}
	else if(bytes_of_in > bytes_of_pr_out){
		max_bytes = bytes_of_in;
	}
	else{
		if(bytes_of_in > max_bytes ){
			max_bytes = bytes_of_in;
		}
	}
	if(max_bytes == 1){
		if(mult_of_same_bytes == 0){
			mult_of_same_bytes = 1;
		}
	}
	
	result = (mult_of_same_bytes*100)/max_bytes;
	
	return(result);
}

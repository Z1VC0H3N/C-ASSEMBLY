#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void* info(Elf32_Phdr * program_headers,int i){
        write(1,"Program header number: ",23);
        char number[32];
        sprintf(number,"%d",i);
        write(1,number,strlen(number));
        write(1," ",1);
        char address[32];
        int len = snprintf(address,sizeof(address)+1,"Program header address %p\n",(void*) &program_headers[i]);
        write(1,address,len-1);
        write(1,"\n",1);
}
/*
map_start: The address in virtual memory the executable is mapped to.
func: the function which will be applied to each Phdr.
arg: an additional argument to be passed to func, for later use (not used in this task).
*/
int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg){
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *) map_start;
    Elf32_Phdr *phdr_table = (Elf32_Phdr *) (map_start + ehdr->e_phoff);
/*e_phoff = This member holds the program header table’s file offset in bytes. If the file has no
program header table, this member holds zero*/
    for (int i = 0; i < ehdr->e_phnum; i++) {
        func(phdr_table, i);
    }

    return 0;
}
int main (int argc ,char** argv){
    write(1,"Please enter file's name of a 32bit ELF formatted executable:\n",50);
    write(1,"\n",1);
    char* s1 = (char*) malloc(100);
	read(0,s1,100);
	s1[strlen(s1)-1] = '\0';
	int fd = open(s1, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 0;
    }
    int size = lseek(fd, 0, SEEK_END) + 1 ;
	lseek(fd,0,SEEK_SET);

    void *map_start = mmap(NULL,size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("Failed to mmap file");
        return 0;
    }

    int ret = foreach_phdr(map_start,info, 0);

    munmap(map_start, size);
    close(fd);
    free(s1);
    return ret;
}





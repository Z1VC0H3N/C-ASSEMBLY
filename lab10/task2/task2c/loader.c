#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
char* retType(int type)
{
    char* typr_str = "Unknown Type";
    switch (type)
    {
        case PT_NULL:
            typr_str = "NULL";
            break;
        case PT_LOAD:
            typr_str = "LOAD";
            break;
        case PT_DYNAMIC:
            typr_str = "DYNAMIC";
            break;
        case PT_INTERP:
            typr_str = "INTERP";
            break;
        case PT_NOTE:
            typr_str = "NOTE";
            break;
        case PT_SHLIB:
            typr_str = "SHLIB";
            break;
        case PT_PHDR:
            typr_str = "PHDR";
            break;
        case PT_LOPROC:
            typr_str = "LOPROC";
            break;
        case PT_HIPROC:
            typr_str = "HIPROC";
            break;
        case PT_GNU_STACK:
            typr_str = "GNU_STACK";
            break;
        default:
            typr_str = "Unknown Type";
            break;
    }
    return typr_str;
}
char* retFlag(int flag){
    switch (flag)
    {
    case 1:
        return "X";
    case 2:
        return "W";
    case 3:
        return "W X";
    case 4:
        return "R";
    case 5:
        return "R X";
    case 6:
        return "R W";
    case 7:
        return "R W X";
    default:
        break;
    }
}

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
void* realInfo(Elf32_Phdr * program_headers,int i){
    write(1,"\n",1);
    write(1,"Program header number: ",23);
    char number[32];
    sprintf(number,"%d",i);
    write(1,number,strlen(number));
    write(1," ",1);
    write(1,"\n",1);
    char type[40];
    char offset[40];
    char virtAddr[40];
    char physAddr[40];
    char fileSiz[40];
    char memSiz[40];
    char flg [40] ;
    char Align[40];
    int lenType = snprintf(type,sizeof(type),"%u\n",(void*) program_headers[i].p_type);
    int lenOffset = snprintf(offset,sizeof(type),"0x%06x\n",(void*) program_headers[i].p_offset);
    int lenVirtAddr = snprintf(virtAddr,sizeof(type),"0x%06x\n",(void*) program_headers[i].p_vaddr);
    int lenphysAddr = snprintf(physAddr,sizeof(type),"0x%06x\n",(void*) program_headers[i].p_paddr);
    int lenFileSiz = snprintf(fileSiz,sizeof(type),"0x%06d\n",(void*) program_headers[i].p_filesz);
    int lenMemSiz = snprintf(memSiz,sizeof(type),"0x%06d\n",(void*) program_headers[i].p_memsz);
    int lenFlg = snprintf(flg,sizeof(type),"%d\n",(void*) program_headers[i].p_flags);
    int lenAlign =  snprintf(Align,sizeof(type),"0x%x\n",(void*) program_headers[i].p_align);
    char* t = retType(program_headers[i].p_type);
    write(1,"Type: ",7);
    write(1,t,strlen(t));
    write(1,"\n",1);
    write(1,"offset: ",9);
    write(1,offset,lenOffset);
    write(1,"virtAddr: ",11);
    write(1,virtAddr,lenVirtAddr);
    write(1,"physAddr: ",11);
    write(1,physAddr,lenphysAddr);
    write(1,"fileSiz: ",10);
    write(1,fileSiz,lenFileSiz);
    write(1,"memSiz: ",9);
    write(1,memSiz,lenMemSiz);
    char* f = retFlag(program_headers[i].p_flags);
    write(1,"flags: ",8);
    write(1,f,strlen(f));
    write(1,"\n",1);
    write(1,"align: ",8);
    write(1,Align,lenAlign);


        char p[40];
        int prot = 0;
        if (program_headers[i].p_flags & PF_R) prot |= PROT_READ;
        if (program_headers[i].p_flags & PF_W) prot |= PROT_WRITE;
        if (program_headers[i].p_flags & PF_X) prot |= PROT_EXEC;
        int protLen = snprintf(p,sizeof(p),"0%x\n",prot);
        write(1,"Protection flags for mmap:",27);
        write(1,p,strlen(p));

        // Print the mapping flags that should be passed to mmap
        char flgArr[40];
        int flags = 0;
        if (program_headers[i].p_flags & PF_R) flags |= MAP_PRIVATE;
        if (program_headers[i].p_flags & PF_W) flags |= MAP_PRIVATE;
        if (program_headers[i].p_flags & PF_X) flags |= MAP_PRIVATE;
        int flgLen = snprintf(flgArr,sizeof(flgArr),"0%x\n",flags);
        write(1,"Mapping flags for mmap: ",25);
        write(1,flgArr,strlen(flgArr));
   }
void load_phdr(Elf32_Phdr *phdr, int fd){
    int off;
    int pad;
    void* map;
    void* vaddr;
    for (int i = 0; i < phdr->p_memsz; i++) {
        vaddr = phdr[i].p_vaddr & 0xfffff000;
        off = phdr[i].p_offset & 0xfffff000;
        pad = phdr[i].p_paddr & 0xfff;
        if(phdr[i].p_type ==PT_LOAD){
            map = mmap(vaddr,phdr[i].p_memsz+pad,PROT_READ,MAP_PRIVATE|MAP_FIXED,fd,off);
            realInfo(phdr,i);
        }
    }
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg){
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *) map_start;
    Elf32_Phdr *phdr_table = (Elf32_Phdr *) (map_start + ehdr->e_phoff);
    write(1,"Type   Offset    VirtAddr    PhysAddr   FileSiz   MemSiz   Flg Align",69);
    for (int i = 0; i < ehdr->e_phnum; i++) {
        func(phdr_table, i);
    }
    return 0;
}
int main (int argc ,char** argv){
   if (argc < 2)
    {
        char *errorMessage = "no file name detected\n";
        write(1, errorMessage, strlen(errorMessage));
        return -1;
    }
    int fd;
    char * fileName=argv[1];
    fd = open(fileName, O_RDONLY);
    int size = lseek(fd, 0, SEEK_END) + 1 ;
	lseek(fd,0,SEEK_SET);

    void *map_start = mmap(NULL,size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("Failed to mmap file");
        return 0;
    }
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *) map_start;
    Elf32_Phdr *phdr_table = (Elf32_Phdr *) (map_start + ehdr->e_phoff);
    
    //foreach_phdr(map_start, load_phdr, 0);
    load_phdr(phdr_table,fd); //task2b

    // Get the entry point of the executable
    //void (*start)() = ((void *)(ehdr->e_entry));

    // Pass control to the loaded program
    int ret = startup(argc-1, argv+1, (void *)(ehdr->e_entry));

    // Return the exit code of the loaded program
    close(fd);
    return ret;
    
    //return 0;
   
}

#include <elf.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

#include <sys/mman.h>
#define ABS 65521
/*#define ELF32_ST_BIND(i)((i)>>4)
#define ELF32_ST_TYPE(i)((i)&0xf)
#define ELF32_ST_INFO(b,t)(((b)<<4)+((t)&0xf) */

int fd = -1;
void* mappedFile = 0; 

typedef struct state {
  char debug_mode;
  char* file_name;
  int file_size;
} state;

struct Menu {
	char *name;
	void (*fun)(state* s);
};
void quit(state* s){
	munmap(mappedFile, s->file_size);

	free(s);
	exit(0);
}
void mapFile(state* s){
	char* s1 = (char*) malloc(100);
	fgets(s1,100,stdin);
	s1[strlen(s1)-1] = '\0';
	fd = open(s1, O_RDONLY);
	if (fd < 0) {
    	perror("Error opening file");
    	quit(s);
  	}
	s->file_size = lseek(fd, 0, SEEK_END) + 1 ;
	lseek(fd,0,SEEK_SET);
	mappedFile = mmap(NULL, s->file_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
} 

void ToggleDebugMode(state* s){
	if(s->debug_mode){
		fprintf(stderr,"Debug flag now off\n");
		s->debug_mode = 0;
	}
	else{
		fprintf(stderr,"Debug flag now on\n");
		s->debug_mode = 1;
	}
} 
char* getFileData(int d){
	switch (d) 
	{
		case ELFDATANONE:
		return "Unknown data format";
		break;
		case ELFDATA2LSB:
		return "littel endian";
		break;
		case ELFDATA2MSB:
		return "big endian";
		break;
	}
	return "";
}
void ExamineELFFile(state* s){
	mapFile(s);
	char* temp = mappedFile;
	if(temp[0]!=0x7f || temp[1]!= 0x45 || temp[2]!= 0x4c){
		printf("this is not elf file\n");
		mappedFile = 0;
		return;
	}
	printf("\n\n1.printing magic numbers:\n");
	printf("first byte: %x\n",temp[0]);
	printf("second byte: %x\n",temp[1]);
	printf("thired byte: %x\n\n",temp[2]);
	Elf32_Ehdr* fileHead = (Elf32_Ehdr *) mappedFile;
	printf("2.the encoded data: %s\n\n",getFileData(fileHead->e_ident[EI_DATA]));
	printf("3.printing entry point\n");
	printf("EntryPoint: %x\n\n",fileHead->e_entry);
	printf("4.offset of section hader table: %d\n\n",fileHead->e_shoff);
	printf("5.number of section heders: %d\n\n",fileHead->e_shnum);
	printf("6.Size of each section header entry: %d\n\n", fileHead->e_shentsize);
	printf("7.offset of program hader table: %d\n\n",fileHead->e_phoff);
	printf("8.number of program heders: %d\n\n",fileHead->e_phnum);
	printf("9.Size of each program header entry: %d\n\n", fileHead->e_phentsize);
}
void printSectionNames(state* s){
	if(mappedFile == 0){
		printf("did not entered file\n");
		return;
	}
	Elf32_Ehdr *fileHead = (Elf32_Ehdr *) mappedFile;
	Elf32_Shdr *sectionHeaders = (Elf32_Shdr*)(mappedFile + fileHead->e_shoff);
	char *sectionNames = (char *)(mappedFile + sectionHeaders[fileHead->e_shstrndx].sh_offset);
	if(s->debug_mode){
		printf("Debug: shstrndx = %d, names offset = %d",fileHead->e_shstrndx,sectionHeaders[fileHead->e_shstrndx].sh_offset);
	}
	printf("[Nr] name             type             addr     off    size  \n");
	for (int i = 0; i < fileHead->e_shnum; i++) {
    	printf("[%2d] %-16s",i, sectionNames + sectionHeaders[i].sh_name);
		printf("%-16d ",sectionHeaders[i].sh_type);
		printf("%08x ",sectionHeaders[i].sh_addr);
		printf("%06x ",sectionHeaders[i].sh_offset);
		printf("%06x\n",sectionHeaders[i].sh_size);
  	}
	printf("\n\n");
}




void PrintSymbols(state* s){
	if(mappedFile == 0){
		printf("did not entered file\n");
		return;
	}
	Elf32_Ehdr *fileHead = (Elf32_Ehdr *) mappedFile;
	Elf32_Shdr *sectionHeaders = (Elf32_Shdr*)(mappedFile + fileHead->e_shoff);
	char *sectionNames = (char *)(mappedFile + sectionHeaders[fileHead->e_shstrndx].sh_offset);
	int symTableIndex = -1;
	int i=0;
	for(;i<fileHead->e_shnum;i++){
		if(sectionHeaders[i].sh_type == SHT_SYMTAB){
			symTableIndex = i;
			break;
		}
	}
	if(symTableIndex == -1){
		fprintf(stderr,"no symbol table");
		return;
	}
	Elf32_Sym* symbolTable = (Elf32_Sym*)(mappedFile+ sectionHeaders[symTableIndex].sh_offset);
	int linkedTable = sectionHeaders[symTableIndex].sh_link;
	char* symbolsName = (char*)(mappedFile + sectionHeaders[linkedTable].sh_offset);
	printf("[Nr] value    Sindex Secname          Syname          \n");
	for(i=0;i<sectionHeaders[symTableIndex].sh_size / sizeof(Elf32_Sym);i++){
		printf("[%2d] %08x ",i,symbolTable[i].st_value);
		printf("%06d ",symbolTable[i].st_shndx);
		if(symbolTable[i].st_shndx != ABS){
			printf("%-16s ",sectionNames + sectionHeaders[symbolTable[i].st_shndx].sh_name);
		}
		else{
			printf("%-16s ","");
		}
		printf("%-16s\n", symbolsName + symbolTable[i].st_name);
	}
	
}





void RelocationTables(state* s){
	if(mappedFile == 0){
		printf("did not entered file\n");
		return;
	}
	Elf32_Ehdr *fileHead = (Elf32_Ehdr *) mappedFile;
	Elf32_Shdr *sectionHeaders = (Elf32_Shdr*)(mappedFile + fileHead->e_shoff);
	char *sectionNames = (char *)(mappedFile + sectionHeaders[fileHead->e_shstrndx].sh_offset);
	int i=0;
	int j=0;
	for(;i<fileHead->e_shnum;i++){
		if(sectionHeaders[i].sh_type == SHT_REL){
			int linkedTable = sectionHeaders[i].sh_link;
			Elf32_Sym* symbolTable = (Elf32_Sym*)(mappedFile + sectionHeaders[linkedTable].sh_offset);

			int symbolNameTable = sectionHeaders[linkedTable].sh_link;
			char* symbolsName = (char*)(mappedFile + sectionHeaders[symbolNameTable].sh_offset);

			printf("section header: %s in offset: %x\n",sectionNames + sectionHeaders[i].sh_name,sectionHeaders[i].sh_offset);
			Elf32_Rel *relocationTable =(Elf32_Rel *)(mappedFile + sectionHeaders[i].sh_offset);
			printf("offset    info     Type   symValue   symName\n");
			for(j=0;j<sectionHeaders[i].sh_size/sizeof(Elf32_Rel);j++){
				int index = ELF32_R_SYM(relocationTable[j].r_info);
				printf("%08x  ",relocationTable[j].r_offset);
				printf("%08x ",relocationTable[j].r_info);
				printf("%04x   ",ELF32_R_TYPE(relocationTable[j].r_info));
				printf("%08x   ",symbolTable[index].st_value);
				printf("%-16s\n",symbolsName + symbolTable[index].st_name);
				
				/*printf("offset: %x. info: %x. Type: %x\n",relocationTable[j].r_offset,relocationTable[j].r_info,ELF32_R_TYPE(relocationTable[j].r_info));
				printf("value: %x\n",symbolTable[index].st_value);
				printf("name: %s\n",symbolsName + symbolTable[index].st_name);*/
			}
			printf("\n\n");
		}
	}

}



int main(int argc,char** argv){
	state* s = (state*)malloc(sizeof(state)); 
	s->debug_mode = 0;
	struct Menu menu[] = {{"Toggle Debug Mode",ToggleDebugMode},{"Examine ELF File",ExamineELFFile},{"Print Section Names",printSectionNames},{"Print Symbols",PrintSymbols},{"Relocation Tables",RelocationTables},{"quit",quit},{NULL,NULL}};
	int i = 0;
	int func = 0;
	while(1){
		for(i=0;menu[i].name!=NULL;i++){
			printf("%d. %s\n",i,menu[i].name);
		}
		printf("enter a function by the name\n");
		scanf("%d",&func);
		fgetc(stdin);
		if(func>=0&&func<i){
			printf("Within bounds\n");
			menu[func].fun(s);

		}
		else{
			printf("not Within bounds\n");
			quit(s);
		}
		
	}
}

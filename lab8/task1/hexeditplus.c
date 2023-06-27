#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXFILENAME 100

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  int display_flag;//decimal = 0, hex = 1
} state;

typedef struct {
  char *name;
  void (*fun)(state*);
}fun_desc;



void displayHelper(void* buff, int lentgh, int unitSize,state* s)
{
    unsigned char* unit = malloc(unitSize);
    int num;
    int k;
    if (s->display_flag)
    {
        printf("Hexadecimal\n =============\n");
    }
    else 
    {
        printf("Decimal\n =============\n");

    }
   for(int i=0;i< lentgh;i++){
        memcpy(unit,buff,unitSize);
        num=unit[0];
        k=8;
        for(int j=i;j<unitSize;j++){
            num = num +(unit[j] << k);
            k=k+8;
        }
        if(s->display_flag){
            printf("%x\n", num);
        }
        else{
            printf("%d\n",num);
        }
        buff=buff+unitSize;
        
    }
   
}

void toggleDebug(state* s)
{
    if (s->debug_mode == 0)
    {
        s->debug_mode = 1;
        printf("Debug flag now on\n");
    }
    else{
        s->debug_mode = 0;
        printf("Debug flag now off\n");
    }
}

void debugPrinter(state* s)
{
    printf("unit_size: %d\nfile_name: %s\nmem_count: %d\n", s->unit_size, s->file_name, s->mem_count);
}

void setFileName(state* s)
{
    char fileName[MAXFILENAME];
    printf("Please enter the file's name:\n");
    scanf("%s", fileName);
    strcpy(s->file_name, fileName);
    if (s->debug_mode){
        printf("Debug: file name set to %s\n", s->file_name);
        debugPrinter(s);
    }
}

void setUnitSize(state* s)
{
   int num;
    printf("Please enter new size:\n");
    scanf("%d", &num);
    if (num == 1 || num == 2 || num == 4)
    {
        s->unit_size = num;
        if (s->debug_mode){
            printf("Debug: set size to %d\n", num);
            debugPrinter(s);
            }
        return;
    }
    printf("Size isn't valid\n");
}

void quit(state* s)
{
    if (s->debug_mode) {
        printf("quitting...\n");}
    exit(0);
}

void loadInMem(state* s)
{
    if(strcmp(s->file_name, "") == 0)
    {
        printf("File's name is empty\n");
        return;
    }
    FILE* inputFile = fopen(s->file_name, "r");
    if(inputFile == NULL){
        printf("Opening file failed.\n");
        return;
    }
    printf("Please enter <location> <length> :\n");
    int loc;
    int len;
    scanf("%x %d", &loc, &len);
    //scanf("%d", &len);
    if(s->debug_mode){
        debugPrinter(s);
    }
    fseek(inputFile, loc, SEEK_SET); //find location in the file
    if(fread(s->mem_buf, s->unit_size, len, inputFile) != -1) { //success in reading into membuf
        printf("Loaded-> %d units into memory\n", len);
        s->mem_count = len * s->unit_size;
    }
    fclose(inputFile);
}

void toggleDisplay(state* s)
{
    s->display_flag = 1 - s->display_flag;
    if(s->display_flag) {
        printf("Display flag now on, hexadecimal representation.\n");
        return;
    }
    printf("Display flag now off, decimal representation.\n");
}

void memoryDisplay(state* s)
{
     int lentgh;
    int addr;
    printf("Enter address and length\n");
    scanf("%X %d", &addr, &lentgh);
    if (!addr) 
    {
        displayHelper(&(s->mem_buf), lentgh, s->unit_size,s);
    }
    else 
    {
        displayHelper(&addr, lentgh, s->unit_size,s);
    }

}

void saveInFile(state* s)
{
     printf("Please enter <source-address> <target-location> <length>\n");
    int addr;
    int loc;
    int len;
    scanf("%X %X %d", &addr, &loc, &len);
    FILE* inFile = fopen(s->file_name,"r+");
    if (inFile == NULL)
    {
        printf("Error while opening the file\n");
        return;
    }
    fseek(inFile, 0, SEEK_END);
    if (ftell(inFile) < loc)
    {
        printf("location isnt valid you stupid fuck face\n");

    }
    fseek(inFile, 0, SEEK_SET);
    fseek(inFile, loc, SEEK_SET);
    if (addr)
    {
        fwrite(&addr, s->unit_size, len, inFile);
    }
    else 
    {
        fwrite(&(s->mem_buf), s->unit_size, len, inFile);
    }
    fclose(inFile);
}

void memModify(state* s)
{
    int loc;
    int tmp;
    unsigned char* val = malloc(s->unit_size);
    printf("Please enter <location> <val>\n");
    scanf("%x %s", &loc, val);
    if (s->debug_mode){
        debugPrinter(s);
    }
    if (s->mem_count < loc) //maybe should be only loc
    {
        printf("location is bigger the memory buf\n");
        return;
    } 
    tmp = strtol(val,NULL,16);
    memcpy(s->mem_buf+loc,&tmp,s->unit_size);
}


int main(int argc, char* argv[])
{
    int user_choise;
    fun_desc menu[] = {{"Toggle Debug Mode", toggleDebug}, {"Set File Name", setFileName}, {"Set Unit Size", setUnitSize}, 
    {"Load Into Memory", loadInMem}, {"Toggle Display Mode", toggleDisplay}, {"Memory Display", memoryDisplay}, {"Save Into File", saveInFile}, {"Memory Modify", memModify}, 
    {"Quit", quit},  {NULL, NULL}};
    state* s = calloc(1, sizeof(state));
    while (1)
    {
      printf("Please choose a function: \n");
	  for(int i=0;i<9;i++){
		printf(" %i", i);
		printf(")     ");
		printf(menu[i].name);
		printf("%C",'\n');
		}
		 int fail =1;
        int x=0;
		while (fail){
			scanf("%d", &x);
			if(x<0 || x>8){
 				printf("Not within bounds");
				printf("%C",'\n');
				printf("u typed wrong input, try again");
			}
			else{
				fail = 0;
			}
		}
	      menu[x].fun(s);
    }

}

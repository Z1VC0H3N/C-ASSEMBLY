#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
	unsigned short SigSize;
	char virusName[16];
	unsigned char* sig;
} virus;
typedef struct link {
	struct link *nextVirus;
	virus *vir;
} link;
typedef struct fun_desc {
	char *name;
	void (*fun)(link*);
} fun_desc;

void quit(link* l){
 list_free(l);
 exit(0);	
}
char* filedet ;
virus* readVirus(FILE* file){
	 short sigSize;
	 char* sig1 ;
	 virus* v = (virus*)malloc(sizeof(virus));
	 fread(&sigSize,2,1,file);
	 sig1 =  (char*)malloc(sigSize);
	 fread(v->virusName,16,1,file);
	 fread(sig1,sigSize,1,file);
	 v->SigSize = sigSize;
	 v->sig = sig1;
	 return v;

 }

 
 void printVirus(virus* virus, FILE* out){
	 if(virus){
    fprintf(out,"%s\n",virus->virusName);
    fprintf(out,"%d\n",virus->SigSize);
    for( int i=0;i<virus->SigSize ;i++){
   	 fprintf(out,"%x ",virus->sig[i]);
    }
    fprintf(out," ");
	fprintf(out,"\n");
 }
 }

 void list_print(link* virus_list,FILE* file){
 	while(virus_list->vir != NULL ){
 		virus* v = virus_list->vir;
 		printVirus(v,file);
 		virus_list = virus_list->nextVirus;
		 if(virus_list->nextVirus==NULL){
		   break;
		 }
 	}
 
 }
 
 
link* list_append(link* virus_list,virus* data){
 	if(virus_list->vir == NULL){
 		virus_list->nextVirus = NULL;
 		virus_list->vir = data;
 		return virus_list;
 	}
 	if(virus_list->nextVirus==NULL)
 	{
 		link* l = (link*)malloc(sizeof(link));
 		l ->nextVirus = NULL;
 		l->vir = data;
 		virus_list->nextVirus = l;
 	}
	else{
 		list_append(virus_list->nextVirus,data);
	}
	return virus_list;
 	}
void list_free(link* virus_list){
   if(virus_list==NULL){
         return;	
	}
   if(virus_list->vir!=NULL){
     		free(virus_list->vir);
     		link *next =  virus_list->nextVirus;
     		list_free(next);
     		free(virus_list->nextVirus);
     	}
}



 void load_sig(link* link){
  printf("Please enter a file name: \n");
  char* m = (char*) malloc(4);
  char fileName[100] ;
  FILE* in;
  scanf("%s",fileName);
  in = fopen(fileName,"rb");
  if(in==NULL){
	fprintf(stderr, "%s", "No such file");
 }
   fread(m,4,1,in);
  while(!feof(in)){
   link = list_append(link,readVirus(in));
  }
  fclose(in);
 }
 
 
 
void print_sig(link* link){
  printf("Please enter a file name: \n");
  char* m = (char*) malloc(4);
  char fileName[100] ;
   FILE* out;
   scanf("%s",fileName);
   out = fopen(fileName,"w");
    if(out==NULL){
	 	fprintf(stderr, "%s", "No such file");
	 	}
   list_print(link,out);
   fclose(out);
 }

void detect(link* l){
  printf("Please enter a file name: \n");
  int x;
  FILE* in;
  in = fopen(filedet,"rb");
  if(in==NULL){
	fprintf(stderr, "%s", "No such file");
 }
 else{
	fseek(in, 0, SEEK_END);
  
    // calculating the size of the file
   long int res = ftell(in);
   fseek(in,0,SEEK_SET);

   char* content = (char*)malloc(res);
   fread(content,res,1,in);
   printf("\n");
   while(l->nextVirus!=NULL){
	   for(int i=0;i<res;i+=1){
        x =  memcmp(l->vir->sig,content+i,l->vir->SigSize);
	    if(x==0){
			printf("\n");
			fprintf(stdout,"%d",i);
			printf("\n");
			fprintf(stdout, "%s", l->vir->virusName );
			printf("\n");
                       fprintf(stdout,"%d",l->vir->SigSize);	       
			printf("\n");
	 }
	}
	 l=l->nextVirus;
   }
   free(content);
   }
   fclose(in);

}
void kill_virus(char* fileName,int offset,int size){
	FILE* in;
    in = fopen(filedet,"w");
	char* toWrite ='90';
    if(in==NULL){
	fprintf(stderr, "%s", "No such file");
 }
	fseek(filedet,offset,SEEK_SET);
	fwrite(toWrite,size,1,in);
	fclose(in);
}
void fix_file(link* l){
  int start;
  int sig;
  printf("Please enter a starting byte: \n");
  scanf("%x",start);
  printf("Please enter a size of virus signature: \n");
  scanf("%d",sig);
  kill_virus(filedet,start,sig);
}
 
 int main (int argc , char** argv){
 	struct fun_desc menu[] ={{"Load signatures",load_sig},{"Print signatures",print_sig},{"Detect viruses",detect},{"Fix file",fix_file},{"Quit",quit},{NULL,NULL}};
 int x;
 link* l;
 l =(link*)malloc(sizeof(link));
 if(argc>1){
	filedet= (char*)malloc(100);
	filedet = argv[1];
 }
 while(1){
	printf("Please choose a function: \n");
	  for(int i=1;i<6;i++){
		printf(" %i", i);
		printf(")  ");
		printf(menu[i-1].name);
		printf("%C",'\n');
	  }
	   scanf("%d",&x);
	   printf("Option : %i\n" ,x);
     	  if(x<1 || x>5){
		printf("Not within bounds");
		printf("%C",'\n');
		printf("u typed wrong input, try again");
		printf("%C",'\n');
		}
	menu[x-1].fun(l);
 }
 
 }

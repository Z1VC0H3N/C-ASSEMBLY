#include <stdlib.h>
#include <stdio.h>
#include <string.h>
struct fun_desc {
	char *name;
	char (*fun)(char);

};
char glob[5] = {};
char *quit(){
	exit(0);
}
void censor(){
for(int i=0; i<5;i++){
	if(glob[i]=='!'){
		glob[i]='.';
}
}
}

char *my_get(char c){
	char x = fgetc(stdin);
	strncat(glob,&x,1);
	return x;

}

void *cprt(){
for(int i=0; i<5;i++){
	if(glob[i]>=32 && glob[i]<=126){
		fputc(glob[i],stdout);
		fputc('\n',stdout);
	}
	else{
		fputc('.',stdout);
		fputc('\n',stdout);
	}
}

}
void *encrypt(){
for(int i=0; i<5;i++){
	if(glob[i]>=32 && glob[i]<=126){
		glob[i] = glob[i]+3;
	}
	else{
		// do nothing
	}

}
}













void *decrypt(char c){
	for(int i=0; i<5;i++){
	if(glob[i]>=32 && glob[i]<=126){
		glob[i] = glob[i]-3;
	}
	else{
		//do nothing
	}
}

}
void *xprt(){
	for(int i=0; i<5;i++){
 		printf(glob[i]);
	}

}
int main(int argc,char **argv){
		struct fun_desc menu[] ={{"my_get",my_get},{"xprt",xprt},{"cprt",cprt},{"censor",censor},{"encrypt",encrypt},{"decrypt",decrypt},{"quit",quit},{NULL,NULL}};
int x;
int fail;
	while(1){
	printf("Please choose a function: \n");
	  for(int i=0;i<7;i++){
		printf(" %i", i);
		//printf(i);
		printf(")     ");
		printf(menu[i].name);
		printf("%C",'\n');
		}
		 fail =1;
		while (fail){
			char c =fgetc(stdin);
			x= atoi(c);
			if(x<0 || x>6){
 				printf("Not within bounds");
				printf("%C",'\n');
				printf("u typed wrong input, try again");
			}
			else{
				fail = 0;
			}
		}
		
	      printf("Option :" ,x);
	      printf("Within bounds");
	      menu[x].fun;
	      printf("DONE.");
	  }
	}


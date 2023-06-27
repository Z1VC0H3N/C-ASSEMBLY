#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char censor(char c){
	if(c=='!')
		return '.';
	else 
		return c;
}

char* map(char *array,int array_length,char (*f) (char)){
//i dont now if i need to cheak the input
	char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
	for(int i=0;i<array_length;i++){
		mapped_array[i] = f(array[i]);
	}
	return mapped_array;
	}
	
	char quit(char c){
	if(c=='q'){
		exit(0);
	}
	else{
		return c;
	}
}
	
	//Task2b
char my_get(char c){
	char x = fgetc(stdin);
	quit((char)x);
	return x;

}

char cprt(char c){
	if(c>=32 && c<=126){
		//fprintf(stdout, "\n",c);
		fputc(c,stdout);
		fputc('\n',stdout);
	}
	else{
		fputc('.',stdout);
		fputc('\n',stdout);
	}
	return c;

}
char encrypt(char c){

	if(c>=32 && c<=126){
		return c+3;
	}
	else{
		return c;
	}

}
char decrypt(char c){
	if(c>=32 && c<=126){
		return c-3;
	}
	else{
		return c;
	}

}
char xprt(char c){
	fprintf(stdout,"%X \n",c);
return c;

}


int main(int argc,char **argv){
	int base_len = 5;
	char arr1[base_len];
	char* arr2 = map(arr1, base_len, my_get);
	char* arr3 = map(arr2, base_len, cprt);
	char* arr4 = map(arr3, base_len, xprt);
	char* arr5 = map(arr4, base_len, encrypt);
	char* arr6 = map(arr5, base_len, decrypt);
	free(arr2);
	free(arr3);
	free(arr4);
	free(arr5);
	free(arr6); 
}

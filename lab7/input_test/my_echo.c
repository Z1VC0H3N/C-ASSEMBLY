#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
	char c;
		c = fgetc(stdin);
		while(c!=-1){
		printf("%c",c);
		c = fgetc(stdin);
		}
}

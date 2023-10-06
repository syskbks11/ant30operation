/*!
\file srvPowerDataReader.cpp
\date 2009.08.10
\author NAGAI
*/
//#ifdef WIN32
//#include <windows.h>
//#include <wincon.h>
//#endif

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	int ret;
	double value;

	if(argc != 2){
		fprintf(stderr, "Please specify a file to read.\n");
		return 1;
	}
	FILE* fp = fopen(argv[1], "r");
	if(fp == NULL){
		fprintf(stderr, "File %s open failed.\n", argv[1]);
		return 1;
	}
	int index = 0;
	while(1){
		ret = fread(&value, sizeof(double), 1, fp);
		if(ret != 1){
			break;
		}
		printf("%d %lf\n", index, value);
		index++;
	}
	fclose(fp);

	return 0;
}


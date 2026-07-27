#ifndef QUICK_CSV
#define QUICK_CSV
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
typedef unsigned char  u8 ;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;


#define dbg(x) printf("%s : %d\n",#x,x);


static unsigned long qcsv_GetFileSize(FILE* fl){
	if(fl==NULL){
		printf("err in file\n");
		return -1;
	}
	long prev=ftell(fl);
	if(fseek(fl, 0L, SEEK_END)==-1){
		printf("files to fseek");
		return -1;
	}
	unsigned long fileSize = ftell(fl);
	fseek(fl, prev, SEEK_SET);
	return fileSize;

}


static void* qcsv_readFile(const char* fileName , unsigned long * sizeOUT){
	FILE* file = fopen(fileName, "r");
	if(file==NULL){
		printf("err in opening file\n");
		return NULL;
	}
	unsigned long fileSz = qcsv_GetFileSize(file);
	*sizeOUT=fileSz;
	
	if(fileSz!=-1){
		char* data = (char*)malloc(fileSz);
		fread(data, fileSz, 1, file);
		
		
		fclose(file);
		return data;
	}

	fclose(file);
	return NULL;
}





typedef struct{
	u8* data;
	unsigned long datasize;
	u32 cellCount;
	int *lines;
	unsigned int linesCount;
} QCSVContext;



static QCSVContext QCSVInit(const char* csvfn){
	QCSVContext ret;
	ret.data = (char*)qcsv_readFile(csvfn, &ret.datasize);

	ret.lines=(int*)malloc(1024*sizeof(int));
	ret.linesCount=0;
	
	int linesize=1024*4;
	bool textbrakets = false;
	
	for(int i = 0 ; i < ret.datasize ; i++){
		if(ret.data[i]=='\n' || i +1 ==ret.datasize){
			if(ret.linesCount<linesize){
				ret.lines=(int*)realloc(ret.lines, linesize+=1024*4);
			}
			ret.lines[ret.linesCount]=i;
			ret.linesCount++;
		}
		else if(ret.data[i]=='"'){
			textbrakets=!textbrakets;
			continue;
		}
		else if(ret.data[i]==','&&!textbrakets){
			ret.cellCount++;
		}
	}
		
	return ret;
}



static char* QCSVGetCell(int x , int y , QCSVContext* ctx){
	dbg(ctx->linesCount);
	dbg(ctx->lines[y?y-1:0]);
	dbg(ctx->lines[y]);


	dbg(ctx->lines[y]- ctx->lines[y?y-1:0]);

	if(y>=ctx->linesCount )
	{
		printf("[ERORR] Cell %d, %d is off frome the sheet.\n",x,y);
		return NULL;
	}
		

	static char cell[1025];
	memset(cell, 0, 1025);
	u32  cellptr=0;
	
	u32 xcell=0;
	bool textbrakets = false;
	for(int i = y?ctx->lines[y-1]:0 ;  i < (ctx->linesCount>1?ctx->lines[y]:ctx->cellCount) ; i++ )
	{
		if(ctx->data[i]==',' && !textbrakets){xcell++;continue;}
		if(ctx->data[i]=='\n'){continue;}
		
		if(ctx->data[i]=='"'){
			textbrakets=!textbrakets;
			continue;
		}
		if(xcell==x){
			if(cellptr<1024){
				cell[cellptr]=ctx->data[i];
				cellptr++;
			}else{
				printf("[WARNING] cell %d,%d over 1024 byte, will not been read all",x,y);
				break;
			}
		}

		if(xcell>x)break;
	}
	if(x>xcell)
	{
		printf("[ERORR] Cell %d, %d is off frome the sheet.\n",x,y);
		return NULL;
	}

	return cell;
}



#endif

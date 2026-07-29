#ifndef QUICK_CSV
#define QUICK_CSV
#include <complex.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
typedef unsigned char  u8 ;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;


#define dbg(x) printf("\n[DEBUG DATA] %s : %d",#x,x);
#define info(x,...) printf("\n[INFO] ");printf(x,__VA_ARGS__);
#define iinfo(x) printf("\n[INFO] %s",x);

/**
 * Look at this mistrasity!
 * */

/****************************************** Utilitys *******************************************/


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






static void*qcsv_PushBuffer(void* val , u32 sizeofstr ,size_t pos,  size_t* dataSize , size_t* usedData , void* data){
	
	if(((*usedData)+sizeofstr) >= *dataSize){
		size_t newDataSize = *usedData+sizeofstr+0x1000;
		data=realloc(data,newDataSize);
		*dataSize=newDataSize;
	}
	if(pos>=*dataSize){
		size_t newDataSize = pos+sizeofstr+0x1000;
		data=realloc(data, newDataSize);
		*dataSize=newDataSize;
	}
	for(u32 i = *usedData+sizeofstr; i>pos+sizeofstr; i--){
		((u8*)data)[i-1] = ((u8*)data)[i-sizeofstr-1];
	}
	
	for(u32 i = 0 ; i < sizeofstr ; i++){
		((u8*)data)[i+pos]=((u8*)val)[i];
	}
	*usedData+=sizeofstr;
	
	///*	
	usleep(100000);
	system("clear");
	for(int i = 0 , linenum = 0 ; i < *usedData ; i++){
		if(((u8*)data)[i]=='\n'){
			linenum++;
			printf("\n%d :",linenum);
			continue;
		}
		if(i==pos)
		{
			printf("[%c]",*(u8*)(data+i));
		}
		else{
			printf("%c",*(u8*)(data+i));
		}

	}
	//*/
	fflush(stdout);
	

	return data;
}
static void*qcsv_PushChar   (char val  ,size_t pos,  size_t* dataSize , size_t* usedData , void* data){
	void *ret= qcsv_PushBuffer(&val , (u32)sizeof(typeof(val)) , pos*sizeof(typeof(val)),  dataSize , usedData , data);
	return ret;
}
static void*qcsv_PushShort  (short val  ,size_t pos,  size_t* dataSize , size_t* usedData , void* data){
	return  qcsv_PushBuffer(&val , (u32)sizeof(typeof(val)) , pos*sizeof(typeof(val)),  dataSize , usedData , data); //intristing stuff
}
static void*qcsv_PushInteger(int val  ,size_t pos,  size_t* dataSize , size_t* usedData , void* data){
	return  qcsv_PushBuffer(&val , (u32)sizeof(typeof(val)) , pos*sizeof(typeof(val)),  dataSize , usedData , data);
}
static void*qcsv_PushFloat  (float val  ,size_t pos,  size_t* dataSize , size_t* usedData , void* data){
	return  qcsv_PushBuffer(&val , (u32)sizeof(typeof(val)) , pos*sizeof(typeof(val)),  dataSize , usedData , data);
}
static void*qcsv_PushLong   (long val  ,size_t pos,  size_t* dataSize , size_t* usedData , void* data){
	return  qcsv_PushBuffer(&val , (u32)sizeof(typeof(val)) , pos*sizeof(typeof(val)),  dataSize , usedData , data);
}
static void*qcsv_PushDouble (double val  ,size_t pos,  size_t* dataSize , size_t* usedData , void* data){
	return  qcsv_PushBuffer(&val , (u32)sizeof(typeof(val)) , pos*sizeof(typeof(val)),  dataSize , usedData , data);
}
static void*qcsv_PushCountInteger(int val  ,size_t pos,  size_t* dataSize , size_t* usedData , void* data){//this is primary used in lines to preven doplication... brrrrrrr
	unsigned long intmemesize = *usedData*4;
	void* ret = qcsv_PushBuffer(&val , (u32)sizeof(typeof(val)) , pos*sizeof(typeof(val)),  dataSize , &intmemesize , data);
	*usedData=intmemesize/4;
	return ret;
}

static void qcsv_PopBuffer(size_t pos, u32 size,  size_t* dataSize ,void* data){
	if(size>*dataSize ){
		printf("[**ERORR**] poped size biger then the buffer size");
		return;
	}
	if(pos+size>*dataSize ){
		printf("[**ERORR**] poped pos over buffer size");
		return;
	}
	for(size_t i = pos ; i+size<*dataSize ; i++){
		*(u8*)((size_t)data+i) = *(u8*)((size_t)data+i+size);
	}
	for(u32 i = 0 ; i < size ; i++){
		((u8*)data)[*dataSize-i]=0;
	}
	*dataSize-=size;
	return ;
}


static char qcsv_PopChar   (size_t pos,  size_t* dataSize ,  void* data){
	char ret = *(char*)((size_t)data+pos*sizeof(char));
	qcsv_PopBuffer(pos , (u32)sizeof(char) ,  dataSize ,  data);
	return ret;
}
static short qcsv_PopShort  (size_t pos,  size_t* dataSize ,  void* data){
	short ret = *(short*)((size_t)data+pos*sizeof(short));
	qcsv_PopBuffer(pos , (u32)sizeof(short) ,   dataSize ,  data); //intristing stuff
	return ret;
}
static int qcsv_PopInteger(size_t pos,  size_t* dataSize ,  void* data){
	int ret = *(int*)((size_t)data+pos*sizeof(int));
	qcsv_PopBuffer(pos , (u32)sizeof(int) ,  dataSize ,  data);
	return ret;
}
static float qcsv_PopFloat  (size_t pos,  size_t* dataSize ,  void* data){
	float ret = *(float*)((size_t)data+pos*sizeof(float));
	qcsv_PopBuffer(pos , (u32)sizeof(float) ,   dataSize ,  data);
	return ret;
}
static long qcsv_PopLong   (size_t pos,  size_t* dataSize ,  void* data){
	long ret = *(long*)((size_t)data+pos*sizeof(long));
	qcsv_PopBuffer(pos , (u32)sizeof(long) ,   dataSize ,  data);
	return ret;
}
static double qcsv_PopDouble (size_t pos,  size_t* dataSize ,  void* data){
	double ret = *(double*)((size_t)data+pos*sizeof(double));
	qcsv_PopBuffer(pos , (u32)sizeof(double) , dataSize ,  data);
	return ret;
}


static u64 qcsvmax(u64 v1 , u64 v2)
{
	return v1>v2?v1:v2;
}

static u64 qcsvmin(u64 v1 , u64 v2)
{
	return v1>v2?v2:v1;
}




/*******************************************************************************************************/





typedef struct{
	u8* data;
	unsigned long datasize;
	unsigned long datamemsize;// size that data took frome memory, tepcly is more than the axtiol size to minimize mallocs

	u32 cellCount; 		  // the all cell count in the file
	u32 maxrowscount; 	  // the max cell in row size in the file
	int *lines;		  // line positions, this is an array of lines pos
	unsigned long linesCount;
	
	unsigned long linememsize; //size 
} QCSVContext;



static QCSVContext QCSVInit(const char* csvfn){
	QCSVContext ret;
	ret.data = (char*)qcsv_readFile(csvfn, &ret.datasize);
	ret.datamemsize=ret.datasize;
	ret.lines=(int*)malloc(1024*sizeof(int));
	ret.linesCount=0;
	ret.maxrowscount=0;
	int linesize=1024*4;
	bool textbrakets = false;
	
	u32 cellperrow=0;
	for(int i = 0 ; i < ret.datasize ; i++){
		if(ret.data[i]=='\n' || i +1 ==ret.datasize){
			if(ret.linesCount<linesize){
				ret.lines=(int*)realloc(ret.lines, linesize+=1024*4);
			}
			ret.lines[ret.linesCount]=i;
			ret.linesCount++;
			ret.cellCount++;
			if(cellperrow>ret.maxrowscount){
				ret.maxrowscount=cellperrow;
			}
			cellperrow=0;
		}
		else if(ret.data[i]=='"'){
			textbrakets=!textbrakets;
			continue;
		}
		else if(ret.data[i]==','&&!textbrakets){
			ret.cellCount++;
			cellperrow++;
		}
	}
	return ret;
}

void qcsvrebaselines(QCSVContext* ctx, u32 pos, u32 off){
	for(int i = pos; i < ctx->linesCount-1 ; i++)
	{
		ctx->lines[i]+=off;
	}
}


void QCSVSave(const char* file,QCSVContext* ctx){
	FILE* fl = fopen(file,"w");

	if(fl==NULL){
		printf("[ERORR] can-not open file \"%s\".\n",file);
		return;
	}
	fwrite(ctx->data, ctx->datasize,1 , fl);
	fflush(fl);
	return;

}


static char* QCSVGetCell(int x , int y , QCSVContext* ctx){
	/*dbg(ctx->linesCount);
	dbg(ctx->lines[y?y-1:0]);
	dbg(ctx->lines[y]);


	dbg(ctx->lines[y]- ctx->lines[y?y-1:0]);*/

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



static void* QCSVSetCell(const char* data,int x , int y , QCSVContext* ctx){

	if(y>=ctx->linesCount){
		for(int i = ctx->linesCount-1 ; i<y ; i++){
			ctx->lines=(int*)qcsv_PushCountInteger(ctx->datasize-1, ctx->linesCount, &ctx->linememsize, &ctx->linesCount, ctx->lines);
			ctx->data=(u8*)qcsv_PushChar('\n', ctx->datasize-1, &ctx->datamemsize, &ctx->datasize, ctx->data);
			ctx->cellCount++;

			for(int ii = 0 ; ii < ctx->maxrowscount ; ii++){
				ctx->data=(u8*)qcsv_PushChar(',', ctx->datasize-1, &ctx->datamemsize, &ctx->datasize, ctx->data);
				qcsvrebaselines(ctx, y, 1);
				ctx->cellCount++;
			}
			
		}
	}
	



	int  rowcell = 0;
	bool textbrakets=false;

	for(int i = y?ctx->lines[y-1]:0;i< (y<ctx->linesCount-1)?ctx->lines[y]:ctx->datasize;i++){
		
		
				
		if(i+1>=(y<ctx->linesCount-1)?ctx->lines[y]:ctx->datasize && rowcell<x){
			dbg(x-rowcell);
			for(int ii = 0; ii < x-rowcell; ii++){
				ctx->data=(u8*)qcsv_PushChar(',', i, &ctx->datamemsize, &ctx->datasize, ctx->data);
				qcsvrebaselines(ctx, y, 1);
				ctx->cellCount++;
			}
		}

		if(rowcell==x){
			u32 len = qcsvmin(strlen(data),1024);
			for(int c = 0 ; c < len ; c++){
				if((data[c]=='"' || data[c]==',')&&c&&data[0]!='"'){
					ctx->data=(u8*)qcsv_PushChar('"', i, &ctx->datamemsize, &ctx->datasize, ctx->data);
				
				}
				if(data[c]=='"'){
					ctx->data=(u8*)qcsv_PushBuffer((void*)"”",strlen("”"), i+c, &ctx->datamemsize, &ctx->datasize, ctx->data);//same same , but defrent
					c+=strlen("”")-1;
				}else {
					ctx->data=(u8*)qcsv_PushChar(data[c], i+c, &ctx->datamemsize, &ctx->datasize, ctx->data);
				}
				if(ctx->data[i]=='"' && c+1>=len){
					ctx->data=(u8*)qcsv_PushChar('"', i+c, &ctx->datamemsize, &ctx->datasize, ctx->data);

				}
				
				dbg(i);
				dbg(rowcell);
				printf("\n");
			}

			qcsvrebaselines(ctx, y, len);
			i+=rowcell;
		}
		dbg(rowcell);
		dbg(i);	
		if(ctx->data[i]==',' && !textbrakets){rowcell++;/*continue;*/}
		if(ctx->data[i]=='\n'){rowcell=0;/*continue;*/}
		
		if(rowcell>x)break;
		if(ctx->data[i]=='"'){
			textbrakets=!textbrakets;
		}
		
	}
	 

}

#endif

#include <stdio.h>
#include "../quickcsv.h"

int main(){

	QCSVContext qcvctx = QCSVInit("boha?.csv");
	u32 x , y;
	printf("x: ");
	scanf("%d", &x);
	printf("y: ");
	scanf("%d", &y);

	char* bohacell = QCSVGetCell(x, y, &qcvctx);
	printf("\nx:%d, y:%d :  %s\n",x,y,bohacell);

}

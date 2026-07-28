#include <stdio.h>
#include "../quickcsv.h"

int main(){

	QCSVContext qcvctx = QCSVInit("test.csv");
	u32 x , y;
	printf("\nx: ");
	scanf("%d", &x);
	printf("\ny: ");
	scanf("%d", &y);

	char* bohacell = QCSVGetCell(x, y, &qcvctx);
	
	QCSVSetCell("LAMA LAMA LAMA LAMA LAMA", 40,40, &qcvctx);
	QCSVSave("test.csv", &qcvctx);
	printf("\nx:%d, y:%d :  %s\n",x,y,bohacell);

	
}

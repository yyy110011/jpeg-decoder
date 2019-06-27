#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <string.h>
#include <cstring>
#include <iostream>

#define SOF 0xc0
#define DQT 0xdb
#define DHT 0xc4
#define SOS 0xda
#define EOI 0xd9




unsigned char *sof;
unsigned char *dqt;
unsigned char *dqt2;
unsigned char *DChuf;
unsigned char *DChuf2;
unsigned char *AChuf;
unsigned char *AChuf2;
unsigned char *Scan;
unsigned char *data;

int hieght = 0;
int width = 0;
int hh = 0;
int vv = 0;

int datacounter = 0;
int hvinfo[3][2];
int hvcount[3] = {0,0,0};
int compindex[3] = {0,0,0};
int hiblk = 0;
int wiblk = 0;

int zztable[64][2] = {{0, 0},{0, 1}, {1, 0},{2, 0}, {1, 1}, {0, 2}, {0, 3}, {1, 2}, {2, 1}, {3, 0}, {4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4}, {0, 5}, {1, 4}, {2, 3}, {3, 2}, {4, 1}, {5, 0}, {6, 0}, {5, 1}, {4, 2}, {3, 3}, {2, 4}, {1, 5}, {0, 6}, {0, 7}, {1, 6}, {2, 5}, {3, 4}, {4, 3}, {5, 2}, {6, 1}, {7, 0}, {7, 1}, {6, 2}, {5, 3}, {4, 4}, {3, 5}, {2, 6}, {1, 7}, {2, 7}, {3, 6}, {4, 5}, {5, 4}, {6, 3}, {7, 2}, {7, 3}, {6, 4}, {5, 5}, {4, 6}, {3, 7}, {4, 7}, {5, 6}, {6, 5}, {7, 4}, {7, 5}, {6, 6}, {5, 7}, {6, 7}, {7, 6}, {7, 7} };
double C[8][8] = {{0.35355339, 0.35355339, 0.35355339, 0.35355339, 0.35355339, 0.35355339, 0.35355339, 0.35355339}, 
				  {0.49039264, 0.41573481, 0.27778512, 0.09754516, -0.09754516, -0.27778512, -0.41573481, -0.49039264},
				  {0.46193977, 0.19134172, -0.19134172, -0.46193977, -0.46193977, -0.19134172, 0.19134172, 0.46193977},
				  {0.41573481, -0.09754516, -0.49039264, -0.27778512, 0.27778512, 0.49039264, 0.09754516, -0.41573481}, 
				  {0.35355339, -0.35355339, -0.35355339, 0.35355339, 0.35355339, -0.35355339, -0.35355339, 0.35355339},
				  {0.27778512, -0.49039264, 0.09754516, 0.41573481, -0.41573481, -0.09754516, 0.49039264, -0.27778512},
				  {0.19134172, -0.46193977, 0.46193977, -0.19134172, -0.19134172, 0.46193977, -0.46193977, 0.19134172},
				  {0.09754516, -0.27778512, 0.41573481, -0.49039264, 0.49039264, -0.41573481, 0.27778512, -0.09754516}};

double Ct[8][8] = { {0.35355339, 0.49039264, 0.46193977, 0.41573481, 0.35355339, 0.27778512, 0.19134172, 0.09754516, },
					{0.35355339, 0.41573481, 0.19134172, -0.09754516, -0.35355339, -0.49039264, -0.46193977, -0.27778512, },
					{0.35355339, 0.27778512, -0.19134172, -0.49039264, -0.35355339, 0.09754516, 0.46193977, 0.41573481, },
					{0.35355339, 0.09754516, -0.46193977, -0.27778512, 0.35355339, 0.41573481, -0.19134172, -0.49039264, },
					{0.35355339, -0.09754516, -0.46193977, 0.27778512, 0.35355339, -0.41573481, -0.19134172, 0.49039264, },
					{0.35355339, -0.27778512, -0.19134172, 0.49039264, -0.35355339, -0.09754516, 0.46193977, -0.41573481, },
					{0.35355339, -0.41573481, 0.19134172, 0.09754516, -0.35355339, 0.49039264, -0.46193977, 0.27778512, },
					{0.35355339, -0.49039264, 0.46193977, -0.41573481, 0.35355339, -0.27778512, 0.19134172, -0.09754516, }};


struct hufftable
{
	int tablesize;
	int *size;
	unsigned int *codeword;
	unsigned char *symbol;
};

int **Y;
int **U;
int **V;
int **image[] = { Y,U,V};



struct hufftable DChuftable;
struct hufftable DChuftable2;
struct hufftable AChuftable;
struct hufftable AChuftable2;
struct hufftable DcHuff[2] = {DChuftable,DChuftable2};
struct hufftable AcHuff[2] = {AChuftable,AChuftable2};


void WriteBMP24(const char* szBmpFileName, int Width, int Height, unsigned char*** RGB);
void post(int block[8][8],int component);
void recover(int h,int v);

void IDCT(double input[8][8], int output[8][8]);
int getBit(unsigned char input, int index)
{
		return (input >> (7-index)) % 2;

}
int DIFFANDACCOEFFICIENT(int t,int word)
{
	if (t==0) return 0;

	if (word >= powf(2,t-1)) return word;
	else return word - powf(2,t)+1;
}
hufftable huffconstruct(unsigned char *source,int length,FILE *afptr);
void decode(FILE *afptr);
void scan(long Lsize,FILE *afptr,unsigned char *buffer);


FILE *debugFptr ;

int main(){

	FILE *fptr;
	unsigned char *buffer;
	fptr = fopen("C:\\Users\\YYY\\Desktop\\phw_jpeg\\teatime.jpg","rb");
	if (fptr == NULL) {printf("ERROR\n"); return 0;}
	fseek(fptr,0,SEEK_END);
	long Lsize = ftell(fptr);
	rewind(fptr);
	buffer = (unsigned char*)malloc(sizeof(unsigned char)*Lsize);
	fread(buffer,1,Lsize,fptr);
	long i = 0;
	int ffcount = 0;
	FILE *afptr ;
	afptr = fopen("C:\\Users\\YYY\\Desktop\\print.txt","w");


	
	debugFptr = fopen("C:\\Users\\YYY\\Desktop\\print2.txt","w");



	scan(Lsize,afptr,buffer);
	decode(afptr);
	recover(hh,vv);


	for(int cc = 0 ;cc < 448;cc++)
	{
		for(int dd = 0; dd<288;dd++)
		{
			fprintf(afptr,"%15d",image[0][cc][dd]);	
		}
		fprintf(afptr,"\n");
	}



	system("pause");
	return 0;
}

hufftable huffconstruct(unsigned char *source,int length,FILE *afptr)
{
	hufftable huffman;
	int Li[16];
//	int *Vi;
	int vicount = length-16;
	huffman.size = (int*)malloc(sizeof(int)*vicount);
	huffman.tablesize = vicount;
	huffman.codeword = (unsigned int*)malloc(sizeof(unsigned int)*vicount);
	huffman.codeword[0] = 0;
	huffman.symbol = (unsigned char*)malloc(sizeof(unsigned char)*vicount);
//	fprintf(afptr,"%d",vicount);
	unsigned int temp = 0;
	for(int i = 0;i < length;i++)
	{
		if (i < 16)
		{
			Li[i] = source[i];
			temp += Li[i];
			if(Li[i] != 0)
			{
				for(int h =0; h < Li[i];h++)
				{
					huffman.size[temp - Li[i]+h] = i+1;
					if (temp - Li[i]+h == 0)
					{ 
						//fprintf(afptr,"\n%d %d  %d\n",huffman.size[0],huffman.codeword[0],temp - Li[i]+h);
						continue;
					}
					if (h == 0) huffman.codeword[temp - Li[i]+h] = powl(2,huffman.size[temp - Li[i]+h] - huffman.size[temp - Li[i]+h-1])*(huffman.codeword[temp - Li[i]+h-1]+1);
					else huffman.codeword[temp - Li[i]+h] = huffman.codeword[temp - Li[i]+h-1]+1;
					//fprintf(afptr,"\n%d %d  %d\n",huffman.size[a],huffman.codeword[a],a);
				}
			}
		}
		else
		{
			huffman.symbol[i-16] = source[i];
			//fprintf(afptr,"%2d",Vi[i-16]);
		}
	}
	for (int c = 0 ;c < vicount;c++)
	{
		fprintf(afptr,"%3d %3x %3x\n",huffman.size[c],huffman.codeword[c],huffman.symbol[c]);
	}
	return huffman;
}

void decode(FILE *afptr)
{
	int tabletype[3][2];
	for(int a = 0;a < 3;a++)
	{
		tabletype[a][0] = Scan[2+2*a]/16;
		tabletype[a][1] = Scan[2+2*a]%16;
			fprintf(afptr,"%d %d %d  %d  %d\n",a,tabletype[a][0],tabletype[a][1],hvinfo[a][0],hvinfo[a][1]);
	}
	fprintf(afptr,"\n\n\n\n");
	
	//fprintf(afptr,"%x\n",data[0]);
	unsigned char aa = data[0];
	unsigned int codebuffer = 0;
	unsigned int byteCount = 0;
	unsigned int bitCount = 0; //01234567
	int blkelecnt = 0;
	int componentc = 0;
	int bit_in_buffer = 0;

	int sy[64];
	int scb[64];
	int scr[64];
	int *SCOMP[3] = {sy,scb,scr};


	int a[8][8];
	int **b=(int** ) a;

	double yblock[8][8];
	double cbblock[8][8];
	double crblock[8][8];
	double** BLOCKED[3] = {(double **) yblock,(double **) cbblock,(double **) crblock};
	



	int idcty[8][8];
	int idctcb[8][8];
	int idctcr[8][8];
	int IDCTED[3] ={**idcty,**idctcb,**idctcr};


	bool mode = false;
	int dc_all[3]={0,0,0}; //dc累積值
	int modecnt = 0;

	while (byteCount < datacounter)
	{
				
		if (byteCount > 0 && data[byteCount-1] == 0xff && data[byteCount] == 0x00) {byteCount++;continue;}
		
		
		codebuffer = codebuffer << 1 | getBit(data[byteCount],bitCount);
		/*if (codebuffer >= 65535) 141,0 00 00 
		{
			fprintf(debugFptr,"\nError !\n ");
			fclose(debugFptr);

			codebuffer = codebuffer ;
		}
*/
		if (byteCount==141 && bitCount==2) 
		{
			byteCount +=0;
		}

		if (bit_in_buffer < 8*sizeof(unsigned int))bit_in_buffer++;
		//fprintf(afptr,"%2x %d\n",codebuffer,bit_in_buffer); 

		// move next bit
		bitCount++;
		if (bitCount ==8)
		{
			bitCount=0;
			byteCount++;
		}

		//做判斷
		//if(blkelecnt == 64 ) {blkelecnt = 0;break;}

		if (blkelecnt == 0)//解DC
		{
		//選第0張HUFFMAN
				for(int i = 0;i < DcHuff[tabletype[componentc][0]].tablesize ; i++)
				{
					if (codebuffer == DcHuff[tabletype[componentc][0]].codeword[i] && bit_in_buffer == DcHuff[tabletype[componentc][0]].size[i])//找到對的codeword
					{
						fprintf(afptr,"code is %x\nsize:%d\n ",DcHuff[tabletype[componentc][0]].codeword[i],DcHuff[tabletype[componentc][0]].size[i]);
						
						fprintf(debugFptr,"----------\n DC Match %d bits  = %x\n",bit_in_buffer,codebuffer); 
						bit_in_buffer =0;
						codebuffer =0;
						
						for(int j =0; j < DcHuff[tabletype[componentc][0]].symbol[i];j++)//往後取跟symbol一樣多的bit
						{
							//if (byteCount > 0 && data[byteCount-1] == 0xff && data[byteCount] == 0x00) {byteCount++;}
							codebuffer = codebuffer << 1 | getBit(data[byteCount],bitCount);
							if (bit_in_buffer < 8*sizeof(unsigned int))bit_in_buffer++;
							fprintf(afptr,"%2x %d %d\n",codebuffer,bit_in_buffer,j); 
							// move next bit
							bitCount++;
							if (bitCount ==8)
							{
								bitCount=0;
								byteCount++;
							}
						}
						fprintf(afptr,"----------\nGet%d bits  = %x\n",bit_in_buffer,codebuffer); 
						int value = DIFFANDACCOEFFICIENT(DcHuff[tabletype[componentc][0]].symbol[i],codebuffer);
						fprintf(afptr,"value: %d\n",value);

						dc_all[componentc] += value;
						if(componentc == 0)	sy[blkelecnt] = dc_all[componentc];
						else if(componentc == 1) scb[blkelecnt] = dc_all[componentc];
						else scr[blkelecnt] = dc_all[componentc];


						bit_in_buffer =0;
						fprintf(debugFptr,"----------\nGet%d bits  = %x\n",bit_in_buffer,codebuffer); 
						codebuffer =0;
						blkelecnt++;

						break;
					}
				}
						
		}
		else if (blkelecnt < 64)//解AC
		{
			//fprintf(afptr,"<%d  %d>",componentc,tabletype[componentc][1]);
			for(int i = 0;i < AcHuff[tabletype[componentc][1]].tablesize ; i++)
			{
				if ((codebuffer == AcHuff[tabletype[componentc][1]].codeword[i] && bit_in_buffer == AcHuff[tabletype[componentc][1]].size[i]))//找到對的codeword
				{
					//if (blkelecnt >= 64)
					//{

					//	if (componentc == 0)
					//	{
					//		for(int cc = blkelecnt;cc < 64; cc++)
					//		{
					//			sy[cc] = 0;							
					//		}

					//		for(int kk = 0 ;kk<64 ; kk++)
					//		{
					//			if(kk%8 == 0)fprintf(afptr,"\n");
					//			sy[kk] = sy[kk]*dqt[kk];  //框太
					//			yblock[zztable[kk][0]][zztable[kk][1]] = sy[kk];
					//			//fprintf(afptr,"%3d",yblock[zztable[kk][0]][zztable[kk][1]]);
					//		}
					//		IDCT(yblock,idcty);
					//		post(idcty,componentc);
					//		for(int y = 0 ;y < 8;y++)
					//		{
					//			for(int x = 0 ;x < 8;x++)
					//			{

					//				fprintf(afptr,"%3d",idcty[y][x]);
					//			}
					//			fprintf(afptr,"\n");
					//		}
					//	}
					//	else if(componentc ==1)
					//	{
					//		for(int cc = blkelecnt;cc < 64; cc++)
					//		{
					//			scb[cc] = 0;							
					//		}
					//		for(int kk = 0 ;kk<64 ; kk++)
					//		{
					//			if(kk%8 == 0)fprintf(afptr,"\n");
					//			scb[kk] = scb[kk]*dqt2[kk];  //框太
					//			cbblock[zztable[kk][0]][zztable[kk][1]] = scb[kk];

					//			//fprintf(afptr,"%3d",cbblock[zztable[kk][0]][zztable[kk][1]]);
					//		}
					//		IDCT(cbblock,idctcb);
					//		post(idctcb,componentc);
					//		for(int y = 0 ;y < 8;y++)
					//		{
					//			for(int x = 0 ;x < 8;x++)
					//			{
					//				fprintf(afptr,"%3d",idctcb[y][x]);
					//			}
					//			fprintf(afptr,"\n");
					//		}
					//	}
					//	else
					//	{

					//		for(int cc = blkelecnt;cc < 64; cc++)
					//		{
					//			scr[cc] = 0;							
					//		}
					//		for(int kk = 0 ;kk<64 ; kk++)
					//		{
					//			if(kk%8 == 0)fprintf(afptr,"\n");
					//			scr[kk] = scr[kk]*dqt2[kk];  //框太
					//			crblock[zztable[kk][0]][zztable[kk][1]] = scr[kk];

					//			//fprintf(afptr,"%3d",crblock[zztable[kk][0]][zztable[kk][1]]);
					//		}



					//		IDCT(crblock,idctcr);
					//		post(idctcr,componentc);

					//		for(int y = 0 ;y < 8;y++)
					//		{
					//			for(int x = 0 ;x < 8;x++)
					//			{
					//				fprintf(afptr,"%3d",idctcr[y][x]);
					//			}
					//			fprintf(afptr,"\n");
					//		}
					//	}

					//	fprintf(afptr,"EOB\n");
					//	blkelecnt = 0;
					//	modecnt++;

					//	if(modecnt == hvinfo[0][0]*hvinfo[0][1]) componentc = 1;
					//	else if (modecnt == hvinfo[0][0]*hvinfo[0][1]+hvinfo[1][0]*hvinfo[1][1]) componentc = 2;
					//	else if (modecnt == hvinfo[0][0]*hvinfo[0][1]+hvinfo[1][0]*hvinfo[1][1] + hvinfo[2][0]*hvinfo[2][1]) 
					//	{componentc = 0;modecnt = 0;}


					//	break;
					//}










					fprintf(debugFptr,"----------\n AC Match %d bits  = %x at %d,%d \n",bit_in_buffer,codebuffer,byteCount ,bitCount ); 
					fprintf(afptr,"code is %x\nsize:%d\n ",AcHuff[tabletype[componentc][1]].codeword[i],AcHuff[tabletype[componentc][1]].size[i]);
					bit_in_buffer =0;
					codebuffer =0;

					//往後取跟symbol一樣多的bit
					for(int j =0; j < AcHuff[tabletype[componentc][1]].symbol[i]%16;j++)
					{
						//if (byteCount > 0 && data[byteCount-1] == 0xff && data[byteCount] == 0x00) {byteCount++;}
							
						codebuffer = codebuffer << 1 | getBit(data[byteCount],bitCount);
						if (bit_in_buffer < 8*sizeof(unsigned int))bit_in_buffer++;

						fprintf(afptr,"%2x %d\n",codebuffer,bit_in_buffer);
						bitCount++;
						if (bitCount ==8)
						{
							bitCount=0;
							byteCount++;
						}
					}
					//fprintf(afptr,"----------\nGet%d bits  = %x\n---------------\n",bit_in_buffer,codebuffer); 

					/////////////////////////////////////////////////
					
					if ((AcHuff[tabletype[componentc][1]].symbol[i] != 0) )
					{
						
						int value = DIFFANDACCOEFFICIENT(AcHuff[tabletype[componentc][1]].symbol[i]%16,codebuffer);
						fprintf(afptr,"RS: 0x %x RRRR: %d  SSSS: %d   EX: %d\n",AcHuff[tabletype[componentc][1]].symbol[i],AcHuff[tabletype[componentc][1]].symbol[i]/16,AcHuff[tabletype[componentc][1]].symbol[i]%16,value);
						//填陣列
						//sy[blkelecnt] = value;
						if(componentc == 0)
						{
							int bb = blkelecnt; 
							while(bb < blkelecnt + AcHuff[tabletype[componentc][1]].symbol[i]/16)
							{
								sy[bb] = 0;
								fprintf(afptr,"%d",sy[bb]);
								bb++;
							}
							blkelecnt = blkelecnt + AcHuff[tabletype[componentc][1]].symbol[i]/16; 
							fprintf(afptr,"\n");
							sy[bb] = value;

						}
						else if(componentc == 1)
						{
							int bb = blkelecnt; 
							while(bb < blkelecnt + AcHuff[tabletype[componentc][1]].symbol[i]/16)
							{
								scb[bb] = 0;
								fprintf(afptr,"%d",scb[bb]);
								bb++;
							}
							blkelecnt = blkelecnt + AcHuff[tabletype[componentc][1]].symbol[i]/16; 
							fprintf(afptr,"\n");
							scb[bb] = value;
						}
						else
						{
							int bb = blkelecnt; 
							while(bb < blkelecnt + AcHuff[tabletype[componentc][1]].symbol[i]/16)
							{
								scr[bb] = 0;
								fprintf(afptr,"%d",scr[bb]);
								bb++;
							}
							blkelecnt = blkelecnt + AcHuff[tabletype[componentc][1]].symbol[i]/16; 
							fprintf(afptr,"\n");
							scr[bb] = value;
						} 						
					
						fprintf(debugFptr,"----------\nGet%d bits  = %x\n",bit_in_buffer,codebuffer); 
						bit_in_buffer =0;
						codebuffer =0;
					
  						blkelecnt++;

					}
					
					if ((AcHuff[tabletype[componentc][1]].symbol[i] == 0) || blkelecnt >= 64)
					{

						if (componentc == 0)
						{
							for(int cc = blkelecnt;cc < 64; cc++)
							{
								sy[cc] = 0;							
							}

							for(int kk = 0 ;kk<64 ; kk++)
							{
								if(kk%8 == 0)fprintf(afptr,"\n");
								sy[kk] = sy[kk]*dqt[kk];  //框太
								yblock[zztable[kk][0]][zztable[kk][1]] = sy[kk];
								//fprintf(afptr,"%3d",yblock[zztable[kk][0]][zztable[kk][1]]);
							}
							IDCT(yblock,idcty);
							post(idcty,componentc);
							for(int y = 0 ;y < 8;y++)
							{
								for(int x = 0 ;x < 8;x++)
								{

									fprintf(afptr,"%3d",idcty[y][x]);
								}
								fprintf(afptr,"\n");
							}
						}
						else if(componentc ==1)
						{
							for(int cc = blkelecnt;cc < 64; cc++)
							{
								scb[cc] = 0;							
							}
							for(int kk = 0 ;kk<64 ; kk++)
							{
								if(kk%8 == 0)fprintf(afptr,"\n");
								scb[kk] = scb[kk]*dqt2[kk];  //框太
								cbblock[zztable[kk][0]][zztable[kk][1]] = scb[kk];
								//fprintf(afptr,"%3d",cbblock[zztable[kk][0]][zztable[kk][1]]);
							}
							IDCT(cbblock,idctcb);
							post(idctcb,componentc);
							for(int y = 0 ;y < 8;y++)
							{
								for(int x = 0 ;x < 8;x++)
								{
									fprintf(afptr,"%3d",idctcb[y][x]);
								}
								fprintf(afptr,"\n");
							}
						}
						else
						{

							for(int cc = blkelecnt;cc < 64; cc++)
							{
								scr[cc] = 0;							
							}
							for(int kk = 0 ;kk<64 ; kk++)
							{
								if(kk%8 == 0)fprintf(afptr,"\n");
								scr[kk] = scr[kk]*dqt2[kk];  //框太
								crblock[zztable[kk][0]][zztable[kk][1]] = scr[kk];

								//fprintf(afptr,"%3d",crblock[zztable[kk][0]][zztable[kk][1]]);
							}



							IDCT(crblock,idctcr);
							post(idctcr,componentc);

							for(int y = 0 ;y < 8;y++)
							{
								for(int x = 0 ;x < 8;x++)
								{
									fprintf(afptr,"%3d",idctcr[y][x]);
								}
								fprintf(afptr,"\n");
							}
						}

						fprintf(afptr,"EOB\n");
						blkelecnt = 0;
						modecnt++;

						if(modecnt == hvinfo[0][0]*hvinfo[0][1]) componentc = 1;
						else if (modecnt == hvinfo[0][0]*hvinfo[0][1]+hvinfo[1][0]*hvinfo[1][1]) componentc = 2;
						else if (modecnt == hvinfo[0][0]*hvinfo[0][1]+hvinfo[1][0]*hvinfo[1][1] + hvinfo[2][0]*hvinfo[2][1]) 
						{componentc = 0;modecnt = 0;}


						break;
					}

					

				}
				
			}
		
		


		}


	}
	


}

void IDCT(double input[8][8], int output[8][8])
{
	double temp[8][8];
	double temp1;
	int i, j, k, N = 8;
	/*  MatrixMultiply( temp, input, C ); */
	for ( i=0;i<N;i++ ){
		for ( j=0;j<N;j++ ){
			temp[i][j] = 0.0;
			for ( k=0;k<N;k++ )
				temp[i][j] += input[i][k] * C[k][j];
		}
	}
	/*  MatrixMultiply( output, Ct, temp ); */
	for ( i=0;i<N;i++ ){
		for ( j=0;j<N;j++ ){
			temp1 = 0.0;
			for (k=0;k<N;k++ )
				temp1 += Ct[i][k] * temp[k][j];
			/*
			if ( temp1<0 )
				output[i][j] = 0;
			else if ( temp1>255 )
				output[i][j] = 255;
			else*/
			output[i][j] =  (temp < 0?(int)(temp1-0.5):(int)(temp1+0.5))+127;
		}
	}
}

void post(int block[8][8],int component)
{
	for(int j = 0;j < 8;j++)
	{
		for(int i = 0;i < 8 ; i++)
		{
			image[component]
			[          8*(hvcount[component]/hvinfo[component][0])     + j + 8*hvinfo[0][1]*(compindex[component]/wiblk)]
			[          8*(hvcount[component]%hvinfo[component][0])     + i + 8*hvinfo[0][0]*(compindex[component]%wiblk)] = block[j][i];
		}

	}

	hvcount[component]++;
	if (hvcount[component] == hvinfo[component][0]*hvinfo[component][1])
	{
		hvcount[component]=0;
		compindex[component]++;
	}
}

void recover(int h,int v)
{
	unsigned char ***photo = new unsigned char**[v];
	for (int i=0; i<v; i++)
	{
		photo[i] = new unsigned char*[h];
		for (int j=0; j<h; j++) photo[i][j] = new unsigned char[3];
	}

	int  twofivefiveyesorno = 0;

	for(int i = 0 ; i < v ;i++)
	{
		for(int j = 0; j < h ; j++)
		{
			//x = (i%(8*hvinfo[0][1]))/2 + (8*hvinfo[0][1])*(i/(8*hvinfo[0][1]))
			//y = (j%(8*hvinfo[0][1]))/2 + (8*hvinfo[0][1])*(j/(8*hvinfo[0][1]))
			//(hvinfo[0][0]*hvinfo[0][1])
			twofivefiveyesorno = image[0][i][j] + 1.772*  (image[1][(i%(8*hvinfo[0][1]))/2 + (8*hvinfo[0][1])*(i/(8*hvinfo[0][1]))]/* */[(j%(8*hvinfo[0][1]))/2+ (8*hvinfo[0][1])*(j/(8*hvinfo[0][1]))]-128);
			if (twofivefiveyesorno > 255) photo[i][j][0] = 255;
			else if (twofivefiveyesorno < 0) photo[i][j][0] = 0;
			else photo[i][j][0] = twofivefiveyesorno;

			twofivefiveyesorno = image[0][i][j] - 0.34414*(image[1][(i%(8*hvinfo[0][1]))/2 + (8*hvinfo[0][1])*(i/(8*hvinfo[0][1]))][(j%(8*hvinfo[0][1]))/2 + (8*hvinfo[0][1])*(j/(8*hvinfo[0][1]))]-128)- 0.71414*(image[2][(i%(8*hvinfo[0][1]))/2 + (8*hvinfo[0][1])*(i/(8*hvinfo[0][1]))][(j%(8*hvinfo[0][1]))/2+ (8*hvinfo[0][1])*(j/(8*hvinfo[0][1]))] - 128);
			if (twofivefiveyesorno > 255) photo[i][j][1] = 255;
			else if (twofivefiveyesorno < 0) photo[i][j][1] = 0;
			else photo[i][j][1] = twofivefiveyesorno;

			twofivefiveyesorno = image[0][i][j] + 1.402*  (image[2][(i%(8*hvinfo[0][1]))/2 + (8*hvinfo[0][1])*(i/(8*hvinfo[0][1]))][(j%(8*hvinfo[0][1]))/2 + (8*hvinfo[0][1])*(j/(8*hvinfo[0][1]))] - 128);
			if (twofivefiveyesorno > 255) photo[i][j][2] = 255;
			else if (twofivefiveyesorno < 0) photo[i][j][2] = 0;
			else photo[i][j][2] = twofivefiveyesorno;
 
			//printf("%d %d %d \n",photo[i][j][0],photo[i][j][1],photo[i][j][2]);
		}
	}		




		
		
		WriteBMP24("C:\\Users\\YYY\\Desktop\\phw_jpeg\\test.bmp",width,hieght,photo);







}

void WriteBMP24(const char* szBmpFileName, int Width, int Height, unsigned char*** RGB)
{
    #pragma pack(1)
    struct stBMFH // BitmapFileHeader & BitmapInfoHeader
    {
        // BitmapFileHeader
        char         bmtype[2];     // 2 bytes - 'B' 'M'
        unsigned int iFileSize;     // 4 bytes
        short int    reserved1;     // 2 bytes
        short int    reserved2;     // 2 bytes
        unsigned int iOffsetBits;   // 4 bytes
        // End of stBMFH structure - size of 14 bytes
        // BitmapInfoHeader
        unsigned int iSizeHeader;    // 4 bytes - 40
        unsigned int iWidth;         // 4 bytes
        unsigned int iHeight;        // 4 bytes
        short int    iPlanes;        // 2 bytes
        short int    iBitCount;      // 2 bytes
        unsigned int Compression;    // 4 bytes
        unsigned int iSizeImage;     // 4 bytes
        unsigned int iXPelsPerMeter; // 4 bytes
        unsigned int iYPelsPerMeter; // 4 bytes
        unsigned int iClrUsed;       // 4 bytes
        unsigned int iClrImportant;  // 4 bytes
        // End of stBMIF structure - size 40 bytes
        // Total size - 54 bytes
    };
    #pragma pack()

    // Round up the width to the nearest DWORD boundary
    int iNumPaddedBytes = 4 - (Width*3 ) % 4;
    iNumPaddedBytes = iNumPaddedBytes % 4;

    stBMFH bh;
    memset(&bh, 0, sizeof(bh));
    bh.bmtype[0]='B';
    bh.bmtype[1]='M';
    bh.iFileSize = (Width*Height*3) + (Height*iNumPaddedBytes) + sizeof(bh);
    bh.iOffsetBits = sizeof(stBMFH);
    bh.iSizeHeader = 40;
    bh.iPlanes = 1;
    bh.iWidth = Width;
    bh.iHeight = Height;
    bh.iBitCount = 24;


    char temp[1024]={0};
    sprintf(temp, "%s", szBmpFileName);
    FILE* fp = fopen(temp, "wb");
    fwrite(&bh, sizeof(bh), 1, fp);
    for (int y=Height-1; y>=0; y--)
    {
        for (int x=0; x<Width; x++)
        {
            //int i = (x + (Width)*y) * 3;
            unsigned int rgbpix = (RGB[y][x][2]<<16)|(RGB[y][x][1]<<8)|(RGB[y][x][0]<<0);
            fwrite(&rgbpix, 3, 1, fp);
        }
        if (iNumPaddedBytes>0)
        {
            unsigned char pad = 0;
            fwrite(&pad, iNumPaddedBytes, 1, fp);
        }
    }
    fclose(fp);
}

void scan(long Lsize,FILE *afptr,unsigned char *buffer)
{
	int i = 0;
	int ffcount = 0;
	while(i<Lsize)
	{
			if (buffer[i] == 0xff)
			{
				ffcount++;
				switch (buffer[i+1])
				{
					//STAR OF IMAGE
					case 0xd8:
						{i+=2;
						break;}

					case 0xe0:
					{
						int dqts = 0;
						dqts = buffer[i+2]<<8|buffer[i+3];
						i = i+dqts+2;
						break;
					}
					case 0xfe:
					{
						int dqts = 0;
						dqts = buffer[i+2]<<8|buffer[i+3];
						i = i+dqts+2;
						break;
					}
					//ŪSOF
					case SOF:
					{
						int dqts = 0;
						dqts = buffer[i+2]<<8 | buffer[i+3];
						sof = (unsigned char*)malloc(sizeof(unsigned char)*(dqts-2));
						fprintf(afptr,"\n\nSOF: element:%d\n",dqts);					
						for(int c = 0; c < dqts-2 ; c++)
						{
							sof[c] = buffer[i+c+4];
							fprintf(afptr,"%x ",sof[c]);
						}
						hieght = buffer[i+5]<<8|buffer[i+6];
						width = buffer[i+7]<<8|buffer[i+8];





						int h = 4*(buffer[i+11]/16 + buffer[i+14]/16 +buffer[i+17]/16);
						int v = 4*(buffer[i+11]%16 + buffer[i+14]%16 +buffer[i+17]%16);
						for (int con = 0 ;con < 3;con++)
						{
							hvinfo[con][0] = buffer[i+11+3*con]/16;
							hvinfo[con][1] = buffer[i+11+3*con]%16;
						}



						h = (width/h + (width%h?1:0))*h;
						v = (hieght/v + (hieght%v?1:0))*v;
	//宣告圖片			
						hh = h;
						vv = v;
						fprintf(afptr,"\n\n%d  %d \n\n!!!!",h,v);
						hiblk = v/(8*hvinfo[0][1]);
						wiblk = h/(8*hvinfo[0][0]);
						image[0] = (int **)malloc(v*sizeof(int *) + v*h*sizeof(int*));
						image[1] = (int **)malloc(v*sizeof(int *) + v*h*sizeof(int*));
						image[2] = (int **)malloc(v*sizeof(int *) + v*h*sizeof(int*));
						int *iData = (int*)(image[0] + v);
						int *pData = (int*)(image[1] + v);
						int *qData = (int*)(image[2] + v);
						for (int loc = 0;loc < v;loc++,iData += h) image[0][loc] = iData;
						for (int loc = 0;loc < v;loc++,pData += h) image[1][loc] = pData;
						for (int loc = 0;loc < v;loc++,qData += h) image[2][loc] = qData;



						//fprintf(afptr,"Y:%d X:%d  %d  %d %d %d",hieght,width,v,h,v*(buffer[i+17]%16)/2,(h*(buffer[i+17]/16)/2));
						i = i+dqts+2;
						break;
					}
					// DC and AC 's Quantization table
					case DQT:
					{
						if(buffer[i+4] ==0x00)
						{
							int dqts = 0;
							dqts = buffer[i+2]<<8|buffer[i+3];
							if(( i+69) != 0xff)
							{
									dqt = (unsigned char*)malloc(sizeof(unsigned char)*64);
									dqt2 = (unsigned char*)malloc(sizeof(unsigned char)*64);
									fprintf(afptr,"\n\nDCDQT: element:%d\n",dqts);					
									for(int c = 0; c < 64 ; c++)
									{
										int temp = 64;
										temp = sqrtl(temp);
										if((c%temp) == 0) fprintf(afptr,"\n");

										dqt[c] = buffer[i+c+5];
										dqt2[c] = buffer[i+c+70];
										fprintf(afptr,"%4d ",dqt2[c]);
									}
									i = i+dqts+2;

							}
							else
							{
								dqt = (unsigned char*)malloc(sizeof(unsigned char)*(dqts-2));
								fprintf(afptr,"\n\nDCDQT: element:%d\n",dqts);					
								for(int c = 0; c < dqts-2 ; c++)
								{
									int temp = dqts-3;
									temp = sqrtl(temp);
									if((c%temp) == 0) fprintf(afptr,"\n");
									dqt[c] = buffer[i+c+5];
									fprintf(afptr,"%4d ",dqt[c]);
								}
								i = i+dqts+2;
							}
						}
						else if(buffer[i+4] ==0x01)
						{
							int dqts = 0;
							dqts = buffer[i+2]<<8|buffer[i+3];
							dqt2 = (unsigned char*)malloc(sizeof(unsigned char)*(dqts-2));
							fprintf(afptr,"\n\nACDQT: element:%d\n",dqts);
							for(int c = 0; c < dqts-2 ; c++)
							{
								int temp = dqts-3;
								temp = sqrtl(temp);
								if((c%temp) == 0) fprintf(afptr,"\n");
								dqt2[c] = buffer[i+c+5];
								fprintf(afptr,"%4d ",dqt2[c]);
							}
							i = i+dqts+2;
						}
						break;
					}
					case DHT:
					{
						if(buffer[i+4] ==0x00 )
						{
							int dqts = 0;
							dqts = buffer[i+2]<<8|buffer[i+3];
							if((dqts +i +2) != 0xff)
							{
								dqts = 16;
								int vc = 0;
								for (int ccc = 0 ;ccc < 16; ccc++)
								{
									vc += buffer[i+ + ccc + 4];
								}
								dqts = dqts + vc +1;
								DChuf = (unsigned char*)malloc(sizeof(unsigned char)*(dqts));
								for(int c = 0; c < dqts ; c++)
								{
									DChuf[c] = buffer[i+c +5];
									//fprintf(afptr,"%x ",DChuf[c]);
								}
								DcHuff[buffer[i+4]]= huffconstruct(DChuf,dqts-1,afptr);
								i = i+dqts+4;
							}
							else
							{
								DChuf = (unsigned char*)malloc(sizeof(unsigned char)*(dqts-2));
								fprintf(afptr,"\n\nDChuff: element:%d\n",dqts);					
								for(int c = 0; c < dqts-2 ; c++)
								{
									DChuf[c] = buffer[i+c+5];
									//fprintf(afptr,"%x ",DChuf[c]);
								}
								DcHuff[buffer[i+4]]= huffconstruct(DChuf,dqts-3,afptr);
								i = i+dqts+2;

							}
							while (buffer[i] != 0xff)
							{
								int dqts = 16;
								int vc = 0;
								for (int ccc = 0 ;ccc < 16; ccc++)
								{
									vc += buffer[i+ccc + 1];
								}
								dqts = dqts + vc +1;
								unsigned char *temphuff;
								temphuff = (unsigned char*)malloc(sizeof(unsigned char)*(dqts));
								fprintf(afptr,"\n\nDChuff: element:%d\n",dqts);					
								for(int c = 0; c < dqts ; c++)
								{
									temphuff[c] = buffer[i+c+1];
									//fprintf(afptr,"%x ",DChuf[c]);
								}

								if ((buffer[i]/16) == 0 )
								{
									DcHuff[buffer[i]%16]= huffconstruct(temphuff,dqts-1,afptr);
								}
								else
								{
									AcHuff[buffer[i]%16] = huffconstruct(temphuff,dqts-1,afptr);
								}
								i = i+dqts;	
								free(temphuff);						
							}
						}
						else if(buffer[i+4] == 0x01)
						{
							int dqts = 0;
							dqts = buffer[i+2]<<8|buffer[i+3];
						
							DChuf2 = (unsigned char*)malloc(sizeof(unsigned char)*(dqts-2));
							fprintf(afptr,"\n\nDChuff2: element:%d\n",dqts);
							for(int c = 0; c < dqts-2 ; c++)
							{
								DChuf2[c] = buffer[i+c+5];
								//fprintf(afptr,"%x ",DChuf2[c]);
							}
							DcHuff[buffer[i+4]] = huffconstruct(DChuf2,dqts-3,afptr);
							i = i+dqts+2;
						}
						else if(buffer[i+4] ==0x10)
						{
							int dqts = 0;
							dqts = buffer[i+2]<<8|buffer[i+3];
						
							AChuf = (unsigned char*)malloc(sizeof(unsigned char)*(dqts-2));
							fprintf(afptr,"\n\nAChuff: element:%d\n",dqts);
							for(int c = 0; c < dqts-2 ; c++)
							{
								AChuf[c] = buffer[i+c+5];
								//fprintf(afptr,"%x ",AChuf[c]);
							}
							AcHuff[buffer[i+4]%16] = huffconstruct(AChuf,dqts-3,afptr);
							i = i+dqts+2;	
						}
						else if(buffer[i+4] == 0x11)
						{
							int dqts = 0;
							dqts = buffer[i+2]<<8|buffer[i+3];
						
							AChuf2 = (unsigned char*)malloc(sizeof(unsigned char)*(dqts-2));
						//	fprintf(afptr,"\n\n\n%x %x %x %x %d\n\n\n",buffer[i],buffer[i+1],buffer[i+2],buffer[i+3],dqts);
							fprintf(afptr,"\n\nAChuff2: element:%d\n",dqts);
							for(int c = 0; c < dqts-2 ; c++)
							{
								AChuf2[c] = buffer[i+c+5];
								//fprintf(afptr,"%x ",AChuf2[c]);
							}
							AcHuff[buffer[i+4]%16] = huffconstruct(AChuf2,dqts-3,afptr);
							i = i+dqts+2;
						}
						break;
					}
					case SOS:
					{
						int dqts = 0;
						dqts = buffer[i+2]<<8|buffer[i+3];					
						Scan = (unsigned char*)malloc(sizeof(unsigned char)*(dqts-2));
						fprintf(afptr,"\n\n\n%x %x %x %x %d\n\n\n",buffer[i],buffer[i+1],buffer[i+2],buffer[i+3],dqts);
						fprintf(afptr,"\n\nSTARTSCAN: element:%d\n",dqts);
						for(int c = 0; c < dqts-2 ; c++)
						{
							Scan[c] = buffer[i+c+4];
							fprintf(afptr,"%x ",Scan[c]);
						}
						fprintf(afptr,"\n\n");
						i = i+dqts+2;
					//讀取scan data
						int dc = 0;
						data = (unsigned char*)malloc(sizeof(unsigned char)*(Lsize-i-2));
						while(i < Lsize-2)
						{
							data[dc] = buffer[i];
							//if (i%16 == 0)fprintf(afptr,"%x ",data[dc]);
							i++;
							dc++;
						}
						datacounter = dc;
						break;
					}
					case 0xdd:
						break;
					case EOI:
						i = i+1;
						break;
				default:
					break;
				}
			}
			else break;
			//fprintf(afptr,"%1x " ,buffer[i]);
		}	
}
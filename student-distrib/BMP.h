/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/

#ifndef _BMP_H
#define _BMP_H




typedef struct __attribute__((packed)) BMP_header_t
{
	unsigned short Signature;
	unsigned int FileSize;
	unsigned int reserved;
	unsigned int DataOffset;
	

}BMP_header_t;

typedef struct __attribute__((packed)) colorTable_t
{
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
	unsigned char reserved;

}colorTable_t;


typedef struct __attribute__((packed)) InfoHeader_t
{

	unsigned int size;
	unsigned int width;
	unsigned int height;
	unsigned short plan;
	unsigned short BitCount;
	unsigned int compression;
	unsigned int inmageSize;
	unsigned int xPPM;
	unsigned int yPPM;
	unsigned int colorUsed;
	unsigned int colorImp;
	


}InfoHeader_t;

typedef struct __attribute__((packed)) BMP256_t
{
	BMP_header_t BMP_header;
	InfoHeader_t InfoHeader;
	colorTable_t colorTable[256];


}BMP256_t;


extern int handle_bmp(BMP256_t * bmp);
extern int handle_bmpK(BMP256_t * bmp);
extern void start_modex();
extern void end_modex();




#endif // !_BMP_H

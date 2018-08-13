#ifndef GENERATE_PICTURE_H
#define GENERATE_PICTURE_H

#include <stdio.h>
#include <tchar.h>
#include "stdafx.h"
#include "BinZipAlg.h"

#define ENCODE_TYPE_NORMAL 1
#define ENCODE_TYPE_ZIP 2

typedef struct tagPictureNormalCode
{
	uint8_t nEncodeType;
	uint8_t nPictureID[4];
	uint8_t nPictureLen[2];
}PictureNormalCode;

typedef struct tagPictureZipCode
{
	uint8_t nEncodeType;
	uint8_t nPictureID[4];
	uint8_t nPictureLen[2];
	uint8_t nUnZipDictoryLen;   //��ѹ�ֵ����
	uint8_t nUnZipDictoryCode[MAX_DICT_CHAR_NUM];  //��ѹ�ֵ䳤��
}PictureZipCode;

int GeneratePicContent(const uint8_t* pPicContent, int nPicContentLen, int nPictureID, char* pOutputString);

int GeneratePicZipContent(const uint8_t* pPicContent, int nPicContentLen, int nPictureID, char* pOutputString);

#endif
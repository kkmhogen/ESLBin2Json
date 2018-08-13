#include "stdafx.h"
#include "GeneratePicture.h"
#include "JsonEncode.h"
#include "BinZipAlg.h"
#include "ToolsBytes.h"

uint32_t H2NL(uint32_t hostValue)
{
	uint32_t networkValue = 0;
	uint8_t* pByte = (uint8_t*)&networkValue;
	pByte[0] = hostValue >> 24; 
	pByte[1] = (uint8_t)((hostValue >> 16) & 0xFF);
	pByte[2] = (uint8_t)((hostValue >> 8) & 0xFF);
	pByte[3] = (uint8_t)(hostValue & 0xFF);

	return networkValue;
}

uint16_t H2NS(uint16_t hostValue)
{
	uint16_t networkValue = 0;
	uint8_t* pByte = (uint8_t*)&networkValue;
	pByte[0] = (uint8_t)(hostValue >> 8);
	pByte[1] = (uint8_t)(hostValue & 0xFF);

	return networkValue;
}

int GeneratePicContent(const uint8_t* pPicContent, int nPicContentLen, int nPictureID, char* pOutputString)
{
	//write tag
	PictureNormalCode picContentHead;
	int nDecodeLen = 0;

	picContentHead.nEncodeType = ENCODE_TYPE_NORMAL; //hex picture type

	*(uint32_t*)picContentHead.nPictureID = H2NL(nPictureID);  //picture id

	//picture id
	*(uint16_t*)picContentHead.nPictureLen = H2NS(nPicContentLen);

	//head to hex string
	tools_byteToHexStrs((uint8_t*)&picContentHead, pOutputString, sizeof(picContentHead));
	nDecodeLen += sizeof(picContentHead) * 2;
	pOutputString += nDecodeLen;

	// to hex string
	int nJsonDataLen = nPicContentLen * 2;
	tools_byteToHexStrs(pPicContent, pOutputString, nPicContentLen);
	nDecodeLen += nJsonDataLen;

	return nDecodeLen;
}

int GeneratePicZipContent(const uint8_t* pPicContent, int nPicContentLen, int nPictureID, char* pOutputString)
{
	ZipStru zipDictory[MAX_ARRAY_SIZE];   //Ñ¹Ëõ×Öµä

	//write tag
	PictureZipCode picContentHead;
	int nDecodeLen = 0;

	picContentHead.nEncodeType = ENCODE_TYPE_ZIP; //hex picture type

	//picture id
	*(uint32_t*)picContentHead.nPictureID = H2NL(nPictureID);  //picture id

	//picture len
	*(uint16_t*)picContentHead.nPictureLen = H2NS(nPicContentLen);


	//Éú³ÉÑ¹ËõºÍ½âÑ¹×Öµä
	int nZipDictoryLen = GenerateZipDictory(pPicContent, nPicContentLen, zipDictory, picContentHead.nUnZipDictoryCode);
	picContentHead.nUnZipDictoryLen = nZipDictoryLen;
	int nValidHeadLen = sizeof(picContentHead) - (MAX_DICT_CHAR_NUM - picContentHead.nUnZipDictoryLen);

	//head to hex string
	tools_byteToHexStrs((uint8_t*)&picContentHead, pOutputString, nValidHeadLen);
	nDecodeLen += nValidHeadLen * 2;
	pOutputString += nDecodeLen;

	//Ñ¹ËõÎÄ¼þ
	uint16_t nZipFileLen = 0;
	nZipFileLen = ZipBytes2String(zipDictory, nZipDictoryLen, pPicContent, nPicContentLen, pOutputString);
	nDecodeLen += nZipFileLen;

	return nDecodeLen;
}
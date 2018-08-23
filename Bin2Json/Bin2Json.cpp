// Bin2Json.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sys/types.h>
#include <time.h>
#include "JsonEncode.h"
#include "GeneratePicture.h"

typedef unsigned char uint8_t;
using namespace std;

#define MAX_FILE_NAME_LEN 100
#define BIN_FILE_LENGTH 10000
#define MAX_INPUT_FILE_SIZE 10000
#define MAX_OUTPUT_FILE_LENGTH 20000

char gapOutPutFile[MAX_OUTPUT_FILE_LENGTH];
uint8_t gapBinInputBytes[MAX_INPUT_FILE_SIZE];

char JSON_TAIL_STR[] = ",\"data\":\"";

int _tmain(int argc, _TCHAR* argv[])
{
	char fileName[MAX_FILE_NAME_LEN];
	char cPassword[30];
	char cMacAddress[30];
	
	int nPictureID;

	cout<<"Input BIN format picture file name:";
	cin>>fileName;

	cout<<"Please input password:";
	cin>>cPassword;
	if (strlen(cPassword) != 8)
	{
		cerr<<"password must be 8 characteristic"<<endl;
		return 1;
	}


	cout<<"Please input device mac address:";
	cin>>cMacAddress;
	if (strlen(cMacAddress) != 12)
	{
		cerr<<"mac address length must be 12 characteristic"<<endl;
		return 1;
	}

	cout<<"Please input picture number:";
	cin>>nPictureID;

	char nChar;
	bool bUsingZip = false;
	cout<<"Using Zip format? Y or N:";
	cin>>nChar;
	if (nChar == 'Y' || nChar == 'y')
	{
		bUsingZip = true;
	}

	//read bin picture file
	FILE *fPicBinFile;
	int nErr = fopen_s(&fPicBinFile, fileName,"rb");
	if (fPicBinFile == NULL || nErr != 0)
	{
		cerr<<"Open input file failed";
		return 1;
	}

	fseek(fPicBinFile,0L,SEEK_END);
    int size = ftell(fPicBinFile);
	if (size > MAX_INPUT_FILE_SIZE)
	{
		cerr<<"read file length error";
		fclose(fPicBinFile);
		return 1;
	}

	//read bin file
	fseek(fPicBinFile,0L,SEEK_SET);
	int ret = fread(gapBinInputBytes,size,1,fPicBinFile);
	if (ret == 0)
	{
		cerr<<"read file error";
		fclose(fPicBinFile);
		return 1;
	}
	fclose(fPicBinFile);

	//encode message
	uint16_t nBufferIdx = 0;
	nBufferIdx = JSON_startJsonMsg(gapOutPutFile, "msg", "dData");
	nBufferIdx = JSON_middleStrField(gapOutPutFile, nBufferIdx, "mac", cMacAddress);

	srand((int)time(NULL));
	int32_t nRandom = rand();
	nBufferIdx = JSON_middleIntField(gapOutPutFile, nBufferIdx, "seq", nRandom);
	
	nBufferIdx = JSON_middleStrField(gapOutPutFile, nBufferIdx, "auth1", cPassword);
	if (bUsingZip)
	{
		nBufferIdx = JSON_middleStrField(gapOutPutFile, nBufferIdx, "dType", "ascii");
	}
	else
	{
		nBufferIdx = JSON_middleStrField(gapOutPutFile, nBufferIdx, "dType", "hex");
	}
	memcpy(&gapOutPutFile[nBufferIdx], JSON_TAIL_STR,  sizeof(JSON_TAIL_STR)-1);
	nBufferIdx+= sizeof(JSON_TAIL_STR)-1;

	//encode picture file
	if (bUsingZip)
	{
		int nEncodeLen = GeneratePicZipContent(gapBinInputBytes, size, nPictureID, &gapOutPutFile[nBufferIdx]);
		nBufferIdx += nEncodeLen;
	}
	else
	{
		int nEncodeLen = GeneratePicContent(gapBinInputBytes, size, nPictureID, &gapOutPutFile[nBufferIdx]);
		nBufferIdx += nEncodeLen;
	}

	//add json tail
	memcpy(&gapOutPutFile[nBufferIdx], "\"}", 2);
	nBufferIdx += 2;

	//write json file to output file
	char outputFileName[MAX_FILE_NAME_LEN] = "";
	strcpy_s(outputFileName, MAX_FILE_NAME_LEN, fileName);
	if (bUsingZip)
	{
		strcat_s(outputFileName, MAX_FILE_NAME_LEN, "z.json");
	}
	else
	{
		strcat_s(outputFileName, MAX_FILE_NAME_LEN, ".json");
	}
	FILE* jsonOutputFile;
	nErr = fopen_s(&jsonOutputFile, outputFileName,"wb");
	if (jsonOutputFile == NULL || nErr != 0)
	{
		cerr<<"Open  output file failed"<<endl;
		fclose(jsonOutputFile);
		return 1;
	}

	if (1 != fwrite(gapOutPutFile, nBufferIdx, 1, jsonOutputFile))
	{
		cerr<<"write JSON file failed"<<endl;
		return 1;
	}

	fclose(jsonOutputFile);
	cout<<"Write JSON file successs"<<endl;

	int nTemp;
	cin>>nTemp;


	return 0;
}

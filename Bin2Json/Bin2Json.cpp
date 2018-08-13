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
#define BIN_FILE_LENGTH 4736
#define MAX_INPUT_FILE_SIZE 4736
#define MAX_OUTPUT_FILE_LENGTH 10000

char gapOutPutFile[MAX_OUTPUT_FILE_LENGTH];

char JSON_TAIL_STR[] = ",\"data\":\"";

int _tmain(int argc, _TCHAR* argv[])
{
	char fileName[MAX_FILE_NAME_LEN];
	char cPassword[30];
	char cMacAddress[30];
	uint8_t cBinInputBytes[MAX_INPUT_FILE_SIZE];
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
	FILE *picBinFile;
	int nErr = fopen_s(&picBinFile, fileName,"rb");
	if (picBinFile == NULL || nErr != 0)
	{
		cerr<<"Open input file failed";
		return 1;
	}
	//read bin file
	int ret = fread(cBinInputBytes,MAX_INPUT_FILE_SIZE,1,picBinFile);
	if (ret == 0)
	{
		cerr<<"read file error";
		fclose(picBinFile);
		return 1;
	}
	fclose(picBinFile);

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
	if (nChar == 'Y' || nChar == 'y')
	{
		int nEncodeLen = GeneratePicZipContent(cBinInputBytes, MAX_INPUT_FILE_SIZE, nPictureID, &gapOutPutFile[nBufferIdx]);
		nBufferIdx += nEncodeLen;
	}
	else
	{
		int nEncodeLen = GeneratePicContent(cBinInputBytes, MAX_INPUT_FILE_SIZE, nPictureID, &gapOutPutFile[nBufferIdx]);
		nBufferIdx += nEncodeLen;
	}

	//add json tail
	memcpy(&gapOutPutFile[nBufferIdx], "\"}", 2);
	nBufferIdx += 2;

	//write json file to output file
	char outputFileName[MAX_FILE_NAME_LEN] = "";
	strcpy_s(outputFileName, MAX_FILE_NAME_LEN, fileName);
	strcat_s(outputFileName, MAX_FILE_NAME_LEN, ".json");
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

#include "stdafx.h"
#include "DMReader.h"
#include "stdlib.h"
#include "iostream"
#include "fstream"
#include <algorithm> 

using namespace std;

const char *tagImageList ="ImageList";
const char *tagImageData ="ImageData";
const char *tagData ="Data";
const char *tagDimensions ="Dimensions";
const char *tagScale ="Scale";

// Constructrue
DMReader::DMReader()
{
    init();
}

// Initialize
void DMReader::init()
{
	row = 0;
	col = 0;
	rowPrew = 0;
	colPrew = 0;
	pos = 0;
	level = 0;
	dataPos = 0;
	for (int i = 0; i < 5; i++)
		tagFlag[i] = false;
	dataSize = 0;
}

/*
 * Read data according to data type, always return a double type for high precision,
 * caller could convert result to other types.
 */
double DMReader::getDataPerType(char *data, DMDataType type)
{
	double result = 0;
	switch (type) {
	case DataTypeShort:
		result = (double)getShort(data);
		break;
	case DataTypeInt:
		result = (double)getInt(data);
		break;
	case DataTypeUshort:
		result = (double)getUShort(data);
		break;
	case DataTypeUint:
		result = (double)getUInt(data);
		break;
	case DataTypeFloat:
		result = (double)getFloat(data);
		break;
	case DataTypeDouble:
		result = (double)getDouble(data);
		break;
	case DataTypeBool:
		result = (double)getBool(data);
		break;
	case DataTypeChar:
		result = (double)getChar(data);
		break;
	case DataTypeOctet:
		result = (double)getOctet(data);
		break;
	case DataTypeInt64:
		result = (double)getLong(data);
		break;
	case DataTypeUint64:
		result = (double)getULong(data);
		break;
	default:
		break;
	}
	return result;
}


// Get image and store to OpenCV Mat, make sure the Mat has been initialized with required size and depth.
void DMReader::getImageData(cv::Mat &img)
{
    char * memblock;
    int i,j;
    std::streampos size;
    std::ifstream File(filePath,std::ios::in|std::ios::binary|std::ios::ate);
    pos=0;
    if(File.is_open())
    {
        size = File.tellg();
        memblock = new char [size];
		//move the file read pointer to the start of file.
        File.seekg (0, std::ios::beg);
		//read the file into the buffer, however this might not appliable to large files (more than GiataByte)
        File.read (memblock, size);
        File.close();
        pos+=dataPos;
		if (img.depth() == CV_32F)
			for (i = 0; i<row; i++)
			{
				float* src = (float*)(img.data + img.step[0] * i);
				for (j = 0; j<col;j++)
					src[j] = max((float)getDataPerType(memblock, dataType),(float)0.0);
			}
		else if (img.depth() == CV_64F)
			for (i = 0; i<row; i++)
			{
				double* src = (double*)(img.data + img.step[0] * i);
				for (j = 0; j<col; j++)
					src[j] = max((double)getDataPerType(memblock, dataType),0.0);
			}
		else
		{
			// ouput error
		}
        delete[] memblock;
    }
}

// Get image demension
void DMReader::getImageDimension(cv::Size &imgSize)
{
    imgSize.width=col;
    imgSize.height=row;
}

// Get Preview Image Dimension, not work for now
void DMReader::getPreviewDimension(cv::Size &imgSize)
{
    imgSize.width=colPrew;
    imgSize.height=rowPrew;
}

// Read dm3 or dm4 file
void DMReader::readDMfile(std::string fileName)
{
    char * memblock;
    std::streampos size;
    std::ifstream File(fileName,std::ios::in|std::ios::binary|std::ios::ate);
    init();
    if(File.is_open())
    {
        filePath=fileName;
        size = File.tellg();
        memblock = new char [size];
        File.seekg (0, std::ios::beg);
        File.read (memblock, size);
        File.close();
        version=getIntBigEndian(memblock);
        fileSize= getLongPerVersion(memblock)/pow(2.0f,20.0f);
        //on x86 structure, the default is little endian. For DM file, this would be different.
        if(!getIntBigEndian(memblock))
        {
            std::cout<< "machine error"<<std::endl;
            return;
        }
		//locating the information for each tag.
		//maybe we need other tags.
        getTagGroup(memblock);
        delete[] memblock;
    }
    File.close();
}

// Get all children nodes and traverse them
void DMReader::getTagGroup(char *data)
{
    pos+=2;
    long TagNum= getLongPerVersion(data);
    for(int i=0;i<TagNum;i++)
    {
        enterTag(data);
    }
}

// Traverse one tag
void DMReader::enterTag(char *data)
{
	static bool readRow = false;
    level++;
    char isData=data[pos];
    pos+=1;
    unsigned short labelSize= getShortBigEndian(data);
    char* labelString=new char[labelSize+1];
    getString(data,labelString,labelSize);
    checkTag(labelString,labelSize);
	if(version == 4)
		long totalBytes = getLongPerVersion(data); // Exist in DM4, not used.
    if(isData==21)
    {
        int dum=getIntBigEndian(data);
        long long deflen= getLongPerVersion(data);
        long long  encType= getLongPerVersion(data);
        if(tagFlag[3]) // Dimensions
        {
            //std::cout<<"image dimension type:"<<encType<<std::endl;
            if(readRow)
                row=getUInt(data);	
            else
                col=getUInt(data);
			readRow = !readRow;
        }
        else if(tagFlag[4])  // Scale
        {
            //std::cout<<"scale type:"<<encType<<std::endl;
            if(readRow)
                scaleRow=getFloat(data);
            else
                scaleCol=getFloat(data);
			readRow = !readRow;
        }
        else
        {
            ReadData(encType,data);
        }
    }
    else if(isData==20)
    {
        getTagGroup(data);
    }
    else
    {
        std::cout<<"error"<<std::endl;
    }
    level--;
}

// Mark that we've entered one tag in {ImageList, ImageData, Data, Dimensions, Scale}
void DMReader::checkTag(char *tag, int length)
{
    if(level==1&&length>=9)
    {
		tagFlag[0] = !strcmp(tag, tagImageList);
    }
    if(tagFlag[0]&&level==3&&length>=9)
    {
		tagFlag[1] = !strcmp(tag, tagImageData);
    }
    if(tagFlag[0]&& tagFlag[1]&&level==4&&length>=4)
    {
		tagFlag[2] = !strcmp(tag, tagData);
    }
    if(tagFlag[0]&& tagFlag[1]&&level==4&&length>=10)
    {
		tagFlag[3] = !strcmp(tag, tagDimensions);
    }
    if(tagFlag[0]&& tagFlag[1]&&level==7&&length>=5)
    {
		tagFlag[4] = !strcmp(tag, tagScale);
    }
}

// Read data
void DMReader::ReadData(int type,char* data)
{
    int i,j;
    int structNameLength;
    int fieldNum;
    int * FieldNameLength;
    int * FieldType;
    char* structName;
    char* fieldName;
    int stringLength;
    char*stringProfile;
    int arrayType;
    int arrayLength;
    switch (type) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
		getDataPerType(data, (DMDataType)type); // value is not used, just for moving pos.
        break;
    case 15:
        structNameLength= getLongPerVersion(data);
        fieldNum= getLongPerVersion(data);
        FieldNameLength=new int[fieldNum];
        FieldType=new int[fieldNum];
        structName=new char[structNameLength+1];
        for(i=0;i<fieldNum;i++)
        {
            FieldNameLength[i]= getLongPerVersion(data);
            FieldType[i]= getLongPerVersion(data);
        }
        getString(data,structName,structNameLength);
        for(i=0;i<fieldNum;i++)
        {
            fieldName=new char[FieldNameLength[i]+1];
            getString(data,fieldName,FieldNameLength[i]);
            ReadData(FieldType[i],data);
        }
        break;
    case 18:    // String
        stringLength=getIntBigEndian(data);
        stringProfile=new char[stringLength+1];
        getString(data,stringProfile,stringLength);
        break;
    case 20:  // Array
        arrayType= getLongPerVersion(data);
        if(arrayType==15) //It is a struct array
        {
            structNameLength= getLongPerVersion(data);
            fieldNum= getLongPerVersion(data);
            FieldNameLength=new int[fieldNum];
            FieldType=new int[fieldNum];
            for(i=0;i<fieldNum;i++)
            {
                FieldNameLength[i]= getLongPerVersion(data);
                FieldType[i]= getLongPerVersion(data);
            }
            arrayLength= getLongPerVersion(data);
            for(i=0;i<arrayLength;i++)
            {
                for(j=0;j<fieldNum;j++)
                {
                    fieldName=new char[FieldNameLength[j]+1];
                    getString(data,fieldName,FieldNameLength[j]);
                    ReadData(FieldType[j],data);
                }
            }
        }
        else // It is not a struct array
        {
            arrayLength= getLongPerVersion(data);
            if(tagFlag[2])   // From tag "Data", record data type.
            {
                if(arrayType == 2 || arrayType == 3 || arrayType == 4 || arrayType == 5 ||
				   arrayType == 6 || arrayType == 7)
                {
                    dataPos=pos;
                    dataSize=arrayLength;
                    dataType= (DMDataType)arrayType;
                }
            }
            for(i=0;i<arrayLength;i++)
            {
                ReadData(arrayType,data);
            }
        }
        break;
    default:
        break;
    }
}


// Read a string with a specified length
void DMReader::getString(char* data,char* stringValue,int length)
{
    int i;
    if(length<1)
    {
        return;
    }
    for(i=0;i<length;i++)
    {
        stringValue[i]=data[pos+i];
    }
    stringValue[i]='\0';
    pos+=length;
}


int DMReader::getInt(char* data)
{
    int value;
    memcpy(&value,data+pos,4);
    pos+=4;
    return value;
}
short DMReader::getShort(char* data)
{
    short value;
    memcpy(&value,data+pos,2);
    pos+=2;
    return value;
}
long DMReader::getLong(char* data)
{
    long long value;
    memcpy(&value,data+pos,8);
    pos+=8;
    return value;
}

bool DMReader::getBool(char* data)
{
	BYTE value;
	memcpy(&value, data + pos, 1);
	pos += 1;
	return (bool)value;
}

char DMReader::getChar(char* data)
{
	BYTE value;
	memcpy(&value, data + pos, 1);
	pos += 1;
	return (char)value;
}

BYTE DMReader::getOctet(char* data)
{
	BYTE value;
	memcpy(&value, data + pos, 1);
	pos += 1;
	return value;
}

float DMReader::getFloat(char *data)
{
    float value;
    memcpy(&value,data+pos,4);
    pos+=4;
    return value;
}
double DMReader::getDouble(char *data)
{
    double value;
    memcpy(&value,data+pos,8);
    pos+=8;
    return value;
}
unsigned int DMReader::getUInt(char* data)
{
    unsigned int value;
    memcpy(&value,data+pos,4);
    pos+=4;
    return value;
}
ushort DMReader::getUShort(char* data)
{
    ushort value=0;
    memcpy(&value,data+pos,2);
    pos+=2;
    return value;
}
unsigned long DMReader::getULong(char* data)
{
    unsigned long value;
    memcpy(&value,data+pos,8);
    pos+=8;
    return value;
}
int DMReader::getIntBigEndian(char* data)
{
    int value;
    char data_Copy[4];
    for(int i=0;i<4;i++)
    {
        data_Copy[i]=data[pos+3-i];
    }
    memcpy(&value,data_Copy,4);
    pos+=4;
    return value;
}
unsigned short DMReader::getShortBigEndian(char* data)
{
    unsigned short value;
    char data_Copy[2];
    for(int i=0;i<2;i++)
    {
        data_Copy[i]=data[pos+1-i];
    }
    memcpy(&value,data_Copy,2);
    pos+=2;
    return value;
}
long DMReader::getLongBigEndian(char* data)
{
    long long value;
    char data_Copy[8];
    for(int i=0;i<8;i++)
    {
        data_Copy[i]=data[pos+7-i];
    }
    memcpy(&value,data_Copy,8);
    pos+=8;
    return value;
}

// Some data is 32bit integer in DM3 but 64bit integer in DM4
long DMReader::getLongPerVersion(char* data)
{
	long result;
	if (version == 3)
		result = (long)getIntBigEndian(data);
	else 
		result = getLongBigEndian(data);
	
	return result;
}  


float DMReader::getScaleRow()
{
	return scaleRow;
}
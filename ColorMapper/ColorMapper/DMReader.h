#ifndef DMREADER_H
#define DMREADER_H
#include "stdlib.h"
#include "iostream"
#include "opencv2/core/core.hpp"

using namespace std;

enum DMDataType
{
	DataTypeShort = 2,
	DataTypeInt = 3,
	DataTypeUshort = 4,
	DataTypeUint = 5,
	DataTypeFloat = 6,
	DataTypeDouble = 7,
	DataTypeBool = 8,
	DataTypeChar = 9,
	DataTypeOctet = 10,
	DataTypeInt64 = 11,
	DataTypeUint64 = 12,
	DataTypeStruct = 15,
};

class DMReader
{
public:
    DMReader();
    void readDMfile(std::string fileName);
    void getImageData(cv::Mat &img);
    void getImageDimension(cv::Size &imgSize);
    void getPreviewDimension(cv::Size &imgSize);
	float getScaleRow();
protected:
    void            getTagGroup(char* data);
    void            enterTag(char* data);
    void            ReadData(int type,char* data);
    void            getString(char* data, char* stringValue,int length);
    int             getInt(char* data);
    unsigned int    getUInt(char* data);
    int             getIntBigEndian(char* data);
    short           getShort(char* data);
    ushort          getUShort(char* data);
	unsigned short           getShortBigEndian(char* data);
    long            getLong(char* data);
	long	        getLongPerVersion(char* data);
    unsigned long   getULong(char* data);
    long            getLongBigEndian(char* data);
    float           getFloat(char*data);
    double          getDouble(char*data);
	bool		    getBool(char*data);
	char		    getChar(char*data);
	BYTE		    getOctet(char*data);
    void            init();
    void            checkTag(char* tag,int length);
	double			getDataPerType(char *data, DMDataType type);
protected:
    int row;
    int col;
    int rowPrew;
    int colPrew;
    int version;
    float fileSize;

    int  level;
    long pos;
	bool tagFlag[5]; // Flags indicate whether we are in a specified tag.
    long dataPos;
    long dataSize;
	DMDataType  dataType;
    ushort* uint16Data;
    short* int16Data;
    std::string filePath;
    float scaleRow;
    float scaleCol;
};

#endif // DMREADER_H

//=======================================================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// ======================================================================================
#include <stdint.h>
// This is the data structure for Windows 3.x Bitmap File header 
#pragma pack(push,1)
typedef struct {
    char         filetype[2];  //Indentifies it as a .bmp file. Has filetype[0]='B' and filetype[1]='M' 
    unsigned int filesize;
    short        reserved1;
    short        reserved2;
    unsigned int dataoffset;   //offset in bytes to starting address where the bitmap data starts 
} file_header;

// This is the data structure for Windows 3.x Full header which includes the file header too 
typedef struct {
    file_header  fileheader;
    unsigned int headersize;
    int          width; //Width for the image in terms of number of pixels
    int          height; //Height for the image in terms of number of pixels
    short        planes;
    short        bitsperpixel;  //This field indentifies if the image is in RGB format (value 24) or in RGBA format (value 32) 
    unsigned int compression;  //This parameter mentions the type of compression involved. In our case we are not concerned about the compression but rather working with the plain image without compression 
    unsigned int bitmapsize; //The total size of the image data in bytes
    int          horizontalres;
    int          verticalres;
    unsigned int numcolors;
    unsigned int importantcolors;
} bitmap_header;

//This is the data structure which is going to represent one pixel value in RGB format 
/*typedef struct {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
} rgb;
*/
template<typename ComponentT>
class rgb {
public: 
	ComponentT blue;
    ComponentT green;
    ComponentT red;

public: 
	rgb() {blue = 0; green = 0; red = 0;}
    rgb(const rgb &iOther) 
		: blue(iOther.blue), green(iOther.green), red(iOther.red)
    {}

	template<typename OtherComponentT>
	explicit rgb(const rgb<OtherComponentT> &iOther)
		: blue(static_cast<ComponentT>(iOther.blue))
		, green(static_cast<ComponentT>(iOther.green))
		, red(static_cast<ComponentT>(iOther.red))
	{}


    rgb & operator =(const rgb &iOther)
    {
        blue = iOther.blue;
        green = iOther.green;
        red = iOther.red;
        return *this;
    }

    rgb operator + (const rgb &iOther) const
    {
        rgb sum;
        sum.blue = blue + iOther.blue;
        sum.green = green + iOther.green;
        sum.red = red + iOther.red;
        return sum;
    }

	template<typename OtherComponentT>	// C++11 Required for -> typename = std::enable_if<sizeof(OtherComponentT) < sizeof(ComponentT)>::type>
	rgb operator + (const rgb<OtherComponentT> &iOther) const
	{
		rgb sum;
		sum.blue = blue + iOther.blue;
		sum.green = green + iOther.green;
		sum.red = red + iOther.red;
		return sum;
	}

	rgb operator / (ComponentT  iScalar) const
	{
		rgb scaledColor;
		scaledColor.blue = blue / iScalar;
		scaledColor.green = green / iScalar;
		scaledColor.red = red / iScalar;
		return scaledColor;
	}
};

typedef rgb<uint8_t> rgb8;
typedef rgb<int16_t> rgb16;
typedef rgb<int32_t> rgb32;

#pragma pack(pop)

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
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include"timer.h"
#include"AveragingFilter.h"

#ifdef _WIN32
#include<sdlt\sdlt.h>
#define ALIGN __declspec(align(ALIGNMENT))
#else
#include<sdlt/sdlt.h>
#define ALIGN __attribute__((aligned(ALIGNMENT)))
#endif

#include<string.h>
#include<xmmintrin.h>
#define ALIGNMENT 32 //Set to 16 bytes for SSE architectures and 32 bytes for Intel(R) AVX architectures
using namespace std;
// Variable to track the average time taken across 5 iterations
long long average = 0;
CUtilTimer t1;
/* This API provides Averaging Filter implementation on normal data structures
   indatatset -> Input image data (Input Parameter)
   outdataset -> Output image data (Output Parameter)
   w -> width of the image in pixels
   h -> height of the image in pixels
*/
ALIGN 
void process_image_serial(rgb8 *indataset, rgb8 *outdataset, int w, int h) 
{
#ifdef __INTEL_COMPILER
	__assume_aligned(indataset, ALIGNMENT);
	__assume_aligned(outdataset, ALIGNMENT);
#endif
	int reduced_width = w - 2;
	t1.start();
	for(int i = 1; i < (h-1); i++)
	{
		int x = (w * i) + 1;
		for (int j = x; j < (x + reduced_width); j++)
		{
			const rgb16 p00(indataset[j - w - 1]);
			const rgb16 p01(indataset[j - w]);
			const rgb16 p02(indataset[j - w + 1]);
			const rgb16 p10(indataset[j - 1]);
			const rgb16 p11(indataset[j]);
			const rgb16 p12(indataset[j + 1]);
			const rgb16 p20(indataset[j + w - 1]);
			const rgb16 p21(indataset[j + w]);
			const rgb16 p22(indataset[j + w + 1]);
			rgb16 sum = p00 + p01 + p02 + p10 + p11 + p12 + p10 + p11 + p12;
			const rgb16 sum1 = sum / 9;
			outdataset[j] = rgb8(sum1);
		}
	}
	t1.stop();
	average += t1.get_ticks();
    return;
}

/* Since C++ runtime doesn't support reflection, the only way SDLT can understand the structure of data is by explicitly stating using SDLT_PRIMITIVE macro. This macro 
   does the real magic of providing the Array of Structure (AOS) interface but store the data as Structure of Arrays (SOA).
   First parameter for SDLT_PRIMITIVE is the name of the structure and the subsequent parameters are the individual components of the structure 

   soa1d_container is the structure of array container provided by SDLT library. Container is an instance of soa1d_container with template specification parameter as rgb8.
*/
SDLT_PRIMITIVE(
        rgb8,
        blue,
        green,
        red
)
typedef sdlt::soa1d_container<rgb8> Container;
//typedef sdlt::aos1d_container<rgb8, sdlt::access_by_struct> Container;

/* This API provides Averaging Filter implementation using SDLT library
   indatatset -> Input image data (Input Parameter)
   outdataset -> Output image data (Output Parameter)
   w -> width of the image in pixels
   h -> height of the image in pixels
*/
SDLT_NOINLINE ALIGN  void process_image_sdlt(Container::accessor<> indataset, Container::accessor<> outdataset, int w, int h)
{
	int reduced_width = w - 2;
	t1.start();

	SDLT_INLINE_BLOCK
	{
		for (int i = 1; i < (h - 1); i++)
		{
			int x = ((w * i) + 1);
			#pragma omp simd
			for (int j = x; j < (x + reduced_width); j++)
			{
				const rgb16 p00(unproxy(indataset[j - w - 1]));
				const rgb16 p01(unproxy(indataset[j - w]));
				const rgb16 p02(unproxy(indataset[j - w + 1]));
				const rgb16 p10(unproxy(indataset[j - 1]));
				const rgb16 p11(unproxy(indataset[j]));
				const rgb16 p12(unproxy(indataset[j + 1]));
				const rgb16 p20(unproxy(indataset[j + w - 1]));
				const rgb16 p21(unproxy(indataset[j + w]));
				const rgb16 p22(unproxy(indataset[j + w + 1]));
				rgb16 sum = p00 + p01 + p02 + p10 + p11 + p12 + p10 + p11 + p12;
				const rgb16 sum1 = sum / 9;
				outdataset[j] = rgb8(sum1);
			}
		}
	}
	t1.stop();
	average += t1.get_ticks();
	return;
}


//This API does the reading and writing from/to the .bmp file. Also invokes the image processing API from here
ALIGN int read_process_write(char* input, char *output, int choice) 
{

    FILE *fp,*out;
    bitmap_header* hp;
    int n;
    CUtilTimer t;
	long long avg_ticks = 0;

    //Instantiating a file handle to open a input BMP file in binary mode
#ifdef _WIN32
	fopen_s(&fp, input, "rb");
#else
	fp = fopen(input, "rb");
#endif
    if(fp==NULL){
        cout<<"The file could not be opened. Program will be exiting\n";
	return 0;
    }


    //Allocating memory for storing the bitmap header information which will be retrieved from input image file
    hp=(bitmap_header*)malloc(sizeof(bitmap_header));
    if(hp==NULL)
    {
	cout<<"Unable to allocate the memory for bitmap header\n";
        return 0;
    }

    //Reading from input file the bitmap header information which is in turn stored in memory allocated in the previous step
    n=fread(hp, sizeof(bitmap_header), 1, fp);
        if(n<1){
            cout<<"Read error from the file. No bytes were read from the file. Program exiting \n";
            return 0;        
        }

    if(hp->bitsperpixel != 24){
        cout<<"This is not a RGB image\n";
        return 0;
    }

    long long dim1 = hp->width;
    long long dim2 = hp->height;
    long long total = dim1 * dim2;

    // Setting the File descriptor to the starting point in the input file where the bitmap data(payload) starts
    fseek(fp,sizeof(char)*hp->fileheader.dataoffset,SEEK_SET);

    // Opening an output file to which the processed result will be written
#ifdef _WIN32
	fopen_s(&out, output, "wb");
#else
	out = fopen(output, "wb");
#endif
    if(out==NULL){
        cout<<"The file could not be opened. Program will be exiting\n";
        return 0;
    }

    // Writing the bitmap header which we copied from the input file to the output file. We need not make any changes because we haven't made any change to the image size or compression type.
    n=fwrite(hp,sizeof(char),sizeof(bitmap_header),out);
    if(n<1){
        cout<<"Write error to the file. No bytes were wrtten to the file. Program exiting \n";
        return 0;
    }

    //Setting the file descriptor to point to the location where the bitmap data is to be written
    fseek(out,sizeof(char)*hp->fileheader.dataoffset,SEEK_SET);

    //Creating input and output containers
     Container inputContainer(total);
     Container outputContainer(total);
	 rgb8 black;
	 black.red = black.green = black.blue = 0;
	 std::fill(outputContainer.begin(), outputContainer.end(), black);


	 ALIGN rgb8 *indata, *outdata;
	 indata = (rgb8 *)_mm_malloc((sizeof(rgb8)*(total)), ALIGNMENT);
	 if (indata == NULL){
		 cout << "Unable to allocate the memory for bitmap date\n";
		 return 0;
	 }
	 //Allocate memory for storing the bitmap data of the processed image
	 outdata = (rgb8 *)_mm_malloc((sizeof(rgb8)*(total)), ALIGNMENT);
	 if (outdata == NULL){
		 cout << "Unable to allocate the memory for bitmap date\n";
		 return 0;
	 }
	 std::fill(outdata, outdata+total, black);


    // Involing the image processing API which does some manipulation on the bitmap data read from the input .bmp file
#ifdef PERF_NUM
for(int i = 0; i < 5; i++)
{
#endif
	unsigned char buffer[3];
	switch (choice){
	case 1:
	{	
		// Setting the File descriptor to the starting point in the input file where the bitmap data(payload) starts
		fseek(fp, sizeof(char)*hp->fileheader.dataoffset, SEEK_SET);
		//Setting the file descriptor to point to the location where the bitmap data is to be written
		fseek(out, sizeof(char)*hp->fileheader.dataoffset, SEEK_SET);
		t.start();
		for (int i = 0; i < hp->height; i++)
		{
			long long index = (i*dim1);
			for (int j = 0; j < hp->width; j++)
			{
				long long index1 = index + j;
				n = fread(buffer, sizeof(unsigned char), 3, fp);
				if (n < 1){
					cout << "Read error from the file. No bytes were read from the file. Program exiting \n";
					return 0;
				}
				rgb8 color;
				color.blue = buffer[0];
				color.green = buffer[1];
				color.red = buffer[2];
				indata[index1] = color;
			}
		}
		process_image_serial(indata, outdata, hp->width, hp->height);
		for (int i = 0; i < hp->height; i++)
		{
			long long index = (i*dim1);
			for (int j = 0; j < hp->width; j++)
			{
				rgb8 * src = (i == 0 || i == (hp->height - 1) || j == 0 || j == (hp->width - 1)) ? indata : outdata;

				long long index1 = index + j;
				rgb8 color = src[index1];
				buffer[0] = color.blue;
				buffer[1] = color.green;
				buffer[2] = color.red;
				n = fwrite(buffer, sizeof(unsigned char), 3, out);
				if (n < 1){
					cout << "Write error to the file. No bytes were wrtten to the file. Program exiting \n";
					return 0;
				}
			}
		}
		t.stop();
		break;
	}
	case 2:
	{
		auto input = inputContainer.access();
		auto output = outputContainer.access();
		// Setting the File descriptor to the starting point in the input file where the bitmap data(payload) starts
		fseek(fp, sizeof(char)*hp->fileheader.dataoffset, SEEK_SET);
		//Setting the file descriptor to point to the location where the bitmap data is to be written
		fseek(out, sizeof(char)*hp->fileheader.dataoffset, SEEK_SET);
		t.start();
		for (int i = 0; i < hp->height; i++)
		{
			long long index = (i*dim1);
			for (int j = 0; j < hp->width; j++)
			{
				long long index1 = index + j;
				n = fread(buffer, sizeof(unsigned char), 3, fp);
				if (n < 1){
					cout << "Read error from the file. No bytes were read from the file. Program exiting \n";
					return 0;
				}
				rgb8 color;
				color.blue = buffer[0];
				color.green = buffer[1];
				color.red = buffer[2];
				input[index1] = color;
			}
		}
		process_image_sdlt(input, output, hp->width, hp->height);
		for (int i = 0; i < hp->height; i++)
		{
			long long index = (i*dim1);
			for (int j = 0; j < hp->width; j++)
			{
				Container::const_accessor<> src = (i == 0 || i == (hp->height - 1) || j == 0 || j == (hp->width - 1)) ? input : output;

				long long index1 = index + j;
				rgb8 color = src[index1];
				buffer[0] = color.blue;
				buffer[1] = color.green;
				buffer[2] = color.red;
				n = fwrite(buffer, sizeof(unsigned char), 3, out);
				if (n < 1){
					cout << "Write error to the file. No bytes were written to the file. Program exiting \n";
					return 0;
				}
			}
		}
		t.stop();
		break;
	}
	default:
	{ 
		cout << "Wrong choice\n";
		break;
	}
	}
	avg_ticks += t.get_ticks();
#ifdef PERF_NUM
	}
avg_ticks /= 5;
average /= 5;
#endif

	cout<<"The time taken in number of ticks is "<<avg_ticks<<"\n";
	cout<<"The time taken just for filter operation is "<<average<<"\n";

    // Closing all file handles and also freeing all the dynamically allocated memory
	_mm_free(indata);
	_mm_free(outdata);

    fclose(fp);
    fclose(out);
    free(hp);
    return 0;
}
int main(int argc, char *argv[]){
        if(argc < 3){
                cout<<"Program usage is <modified_program> <inputfile.bmp> <outputfile.bmp>\n";
                return 0;
        }
		int choice;
		cout<<"Please enter the version you want to execute:\n";
		cout<<"1) Serial version\n";
		cout<<"2) SDLT\n";
		cin>>choice;
        read_process_write(argv[1], argv[2], choice);
        return 0;
}
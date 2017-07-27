#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <aio.h>

//direct writing to file required alligned memory buffer
//so be careful when change this values
#define BUFFER_ALIGN 4096
#define RECORDS_PER_BUFFER 1024
#define RECORD_SIZE 16
#define RECORD_COUNT 1024*1024*32

char*  buffers[2] = {NULL, NULL};
int    fd = 0;
char*  fileName = NULL;
int    flag = -1;
int    mode = 0;
size_t recordsPerBuffer = 1;
size_t bufferSize = RECORD_SIZE;

void clear();
bool readOptions(int argc, char **argv);
void processData(char *buffer);

int main(int argc, char **argv)
{
    char*       buffer = NULL;

    if(!readOptions(argc, argv))
    {
        return -1;
    }

    struct aiocb req[2];
    struct aiocb *aio_list[2][1] = {{&req[0]}, {&req[1]}};

    flag |= O_WRONLY | O_CREAT | O_TRUNC;
    fd = open(fileName,flag,0644);
    if (-1 == fd)
    {
        fprintf(stderr,"failed to create file %s\n",fileName);
        return -1;
    }

    if(mode == 3)
    {
        for(size_t i = 0; i < 2; i++)
        {
            buffers[i] = (char*)memalign(BUFFER_ALIGN, bufferSize);
            if (!buffers[i])
            {
                fprintf(stderr,"could not allocate buffer\n");
                clear();
                return -1;
            }
            memset(&req[i], 0, sizeof(struct aiocb64));
            req[i].aio_fildes = fd;
            req[i].aio_nbytes = bufferSize;
            req[i].aio_buf = NULL;
        }
    }
    else
    {
        buffers[0] = (char*)memalign(BUFFER_ALIGN, bufferSize);
        if (!buffers[0])
        {
            fprintf(stderr,"could not allocate buffer\n");
            clear();
            return -1;
        }
        buffer = buffers[0];
    }

    int idx;
    for(size_t i = 0; i < RECORD_COUNT/recordsPerBuffer; i ++)
    {
        if(mode == 3)
        {
            //change buffer and wait previous operation
            idx = i % 2;
            buffer = buffers[idx];
            if(req[idx].aio_buf == buffer)
            {
                aio_suspend(aio_list[idx], 1, 0);
            }
            req[idx].aio_buf = (void*) buffer;
            req[idx].aio_offset = i * bufferSize;
        }

        processData(buffer);

        if(mode == 3)
        {
            aio_write(&req[idx]);
        }
        else
        {
            if ( bufferSize != (write(fd, buffer, bufferSize)) )
            {
                fprintf(stderr,"failed write record to file\n");
                break;
            }
        }
    }
    if(mode == 3)
    {
        aio_suspend(aio_list[idx], 1, 0);
    }

    if ( !(flag & O_DIRECT) )
    {
        fdatasync(fd);
    }
    clear();
    return 0;
}

void processData(char *buffer)
{
    for (size_t j = 0; j < recordsPerBuffer; j++)
    {
        for (size_t k1 = 0; k1 < 25; k1++)
        {
            for (size_t k = 0; k < RECORD_SIZE; k++)
            {
                buffer[k] = (buffer[k] + k1* j * k) % 256;
            }
        }
    }
}

void clear()
{
    for(size_t i= 0; i < 2; i++)
    {
        if(buffers[i])
        {
           free(buffers[i]);
        }
    }
    if(fd)
    {
        close(fd);
    }
    if(fileName)
    {
        remove(fileName);
    }
}

bool readOptions(int argc, char **argv)
{
    int c;

    while ( -1 != (c = getopt(argc,argv,"f:m:")) )
    {
        switch (c)
        {
        case 'f': /* -o outfile */
        {
            fileName = optarg;
            break;
        }
        case 'm': /* -m mode */
        {
            switch (*optarg)
            {
            case 'c' : /*system file cache*/
                flag = 0;
                mode = 0;
                recordsPerBuffer = 1;
                break;
            case 'b' : /*user buffer passed throught system file cache*/
                flag = 0;
                recordsPerBuffer = RECORDS_PER_BUFFER;
                mode = 1;
                break;
            case 's' : /*user buffer with sync dirrect writing*/
                flag = O_DIRECT;
                recordsPerBuffer = RECORDS_PER_BUFFER;
                mode = 2;
                break;
            case 'a': /*2 user buffers with async dirrect writing*/
                flag = O_DIRECT;
                recordsPerBuffer = RECORDS_PER_BUFFER;
                mode = 3;
                break;
            default:
                fprintf(stderr,"please define system buffer mode 's' or user buffer mode 'u' or both 'b'\n");
                return false;
            }
            break;
        }
        default:
            return false;
        }
    }
    bufferSize = recordsPerBuffer * RECORD_SIZE;
    if(fileName == NULL || flag == -1)
    {
         fprintf(stderr,"usage: exe -f <outputfile> -m <c|b|s|a>\n");
         return false;
    }
    return true;
}

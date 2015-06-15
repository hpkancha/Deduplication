/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2005  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <malloc.h>

#pragma pack(1)

typedef struct 
{
	char path[300];
	char funcCalled[15];
	char from[50];
	char to[50];
	int mask;
	int buffSize;
	struct stat stbuf;
	size_t size;
	off_t offset;
	mode_t mode;
	dev_t rdev;	
	int result;
	struct fuse_file_info fi;
	//struct statvfs stbvfs;
} msgContents;

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

static int hello_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if(strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if(strcmp(path, hello_path) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_str);
    }
    else
        res = -ENOENT;

    return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t
filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if(strcmp(path, "/") != 0)
    return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, hello_path + 1, NULL, 0);

    return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;

    if((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t
offset,
                      struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;

    len = strlen(hello_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, hello_str + offset, size);
    } else
        size = 0;

    return size;
}


int hello_write(char * path, char *buffer,size_t size, off_t offset,  struct fuse_file_info *fi){

    (void) fi;
     
    msgContents mc1,mc2,mc3,mc4;
    memset(&mc1,0,sizeof(msgContents));
    memset(&mc2,0,sizeof(msgContents));
    memset(&mc3,0,sizeof(msgContents));
    strcpy (mc1.funcCalled,"write");
    strcpy (mc1.path,path);
    mc1.offset = offset;
    mc1.size=size;
     
    //sendHead( &mc1 ) ;
    //printf ("sending %s",buf);
//    sendBuff((char *)buf,size );
    //printf ("size is %ld , buffer size is %ld",size,strlen(buf));
  //  recvHead( &mc2 ) ;
    //printf ("*****The result is %d******",mc2.result);
    //return mc2.result;

    //msgContents mc3;
    memset(&mc3,0,sizeof(msgContents));
     
    int fd;
    int res;
    int buffWindow = mc2->size; // +1 for \0
    char writeBuff[buffWindow];
    strcpy (writeBuff,"");
     
    recvBuff(writeBuff,mc2->size);
    //printf ("Size  sent by client fuse: %ld,strlen(writeBuff): %ld\n",mc2->size,strlen(writeBuff));
    //printf ("Recieved buffer is %s\n",writeBuff);
    fd = open(mc2->path, O_WRONLY);
    if (fd == -1)
    {
        mc3.result = -errno;
        sendHead( &mc3 );
        return;
    }
     
    res = pwrite(fd, writeBuff, mc2->size, mc2->offset);
     
    if (res == -1)
    {
        mc3.result = -errno;
        sendHead( &mc3 );
        return;
    }
    mc3.result=res;
    sendHead( &mc3 );
    close(fd);
}

static struct fuse_operations hello_oper = {
    .getattr    = hello_getattr,
    .readdir    = hello_readdir,
    .open       = hello_open,
    .read       = hello_read,
    .write	= hello_write,

};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &hello_oper);
}


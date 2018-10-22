#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdarg.h>
#include "nvram.h"
#include "nvram_lock.h"


static int readFileBin_unlock(char *path, char **data)
{
    int total;
    int fd = 0;

    if( (fd = open(path, O_RDONLY)) < 0 )
        goto _error;

    total = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, 0);

    if( (*data = malloc(total)) == NULL )
    {
        close(fd);
        goto _error;
    }

    if(read(fd, *data, total) < 0)
    {
        free(*data);
        close(fd);
        goto _error;
    }
    
    close(fd);
    return total;

_error:
    return -1;
}

static int readFileBin(char *path, char **data)
{
    int lock;
    int size;

    lock = nvram_lock(path);
    size = readFileBin_unlock(path, data);
    nvram_unlock(lock);
    return size;
}

static int writeFileBin_unlock(char *path, char *data, int len)
{
    int fd;
    int size;
    
    if( (fd = open(path, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR)) < 0 )
        return -1;

    size = write(fd, data, len);
    close(fd);

    return size;
}

static int writeFileBin(char *path, char *data, int len)
{
    int lock;
    int size;

    lock = nvram_lock(path);
    size = writeFileBin_unlock(path, data, len);
    nvram_unlock(lock);

    return size;
}

static char* nvram_get_func_unlock(const char *name,char *path)
{
	char *bufspace;
	int size;
	char *s,*sp;
	
	if((size=readFileBin_unlock(path, &bufspace))<0) 
		return NULL;

	for (s = bufspace; *s; s++) {
		if (!strncmp(s, name, strlen(name)) && *(s+strlen(name))=='=') {
			sp=malloc(strlen(s)-strlen(name));
			memcpy(sp,(s+strlen(name)+1),(strlen(s)-strlen(name)));
			free(bufspace);
			//make this function threadsafe
		    //add_nv_entry(name,sp,path);
			return sp;
		}
		while(*(++s));
	}
	free(bufspace);
	return NULL;
}

/* threadsafe version: return value MUST be free. */
static char* __nvram_get_func(const char *name,char *path)
{
    char *value;
	int lock;

	lock = nvram_lock(path);
	value = nvram_get_func_unlock(name,path);
	nvram_unlock(lock);

	return value;
}

static int nvram_set_func_unlock(const char* name,const char* value,char *path)
{
	char *bufspace, *targetspace;
	int size;
	char *sp, *s;
	int found=0;

	if((size=readFileBin_unlock(path, &bufspace))>0) {
	    targetspace=malloc(size+strlen(name)+strlen(value)+4);
	}
	else {
	    targetspace=malloc(strlen(name)+strlen(value)+4);
	}

	sp=targetspace;
	if(size > 0) {
	   for (s = bufspace; *s; s++) {
		if (!strncmp(s, name, strlen(name)) && *(s+strlen(name))=='=') {
			found=1;
  			strcpy(sp, name);
			sp+=strlen(name);
        		*(sp++) = '=';
       			strcpy(sp, value);
			sp+=strlen(value);		
			while (*(++s));
		}
		while(*s) *(sp++)=*(s++);
	        *(sp++)=END_SYMBOL;
	    }
	
		free(bufspace);
	}
	if(!found){
		strcpy(sp, name);
		sp+=strlen(name);
        	*(sp++) = '=';
	        strcpy(sp, value);
		sp+=strlen(value);
	        *(sp++) = END_SYMBOL;
	}

	*(sp) = END_SYMBOL;

	writeFileBin_unlock(path, targetspace, (sp-targetspace)+1);
	free(targetspace);

	return NVRAM_SUCCESS;
}

int nvram_set_func(const char* name,const char* value,char *path)
{
	int lock;
	int err;
	
	lock = nvram_lock(path);
	err = nvram_set_func_unlock(name, value, path);
	nvram_unlock(lock);
	
	return err;
}

int nvram_set(const char* category, const char* key, const char* value)
{
	char path[256];

	snprintf( path, 256, "%s%s", NVRAM_BASE_PATH, category);

	return nvram_set_func( key, value, path);
}

int nvram_set_idx(const char* category, const char* key, int key_idx, const char* value)
{
	char path[256];
	char name[256] = {0x00};

	snprintf( name, sizeof(name), "%s%d", key, key_idx);
	snprintf( path, 256, "%s%s", NVRAM_BASE_PATH, category);

	return nvram_set_func( name, value, path);
}

int nvram_set_int(const char* category, const char* key, int value)
{
	char value_str[256];
	
	snprintf( value_str, 256, "%d", value);

	return nvram_set(category, key, value_str);
}

int nvram_set_idx_int(const char* category, const char* key, int key_idx, int value)
{
	char value_str[256];
	
	snprintf( value_str, 256, "%d", value);

	return nvram_set_idx(category, key, key_idx, value_str);
}

int nvram_unset(const char *category)
{
	char path[256];
	int lock;

	snprintf(path, sizeof(path), "%s%s", NVRAM_BASE_PATH, category);
	lock = nvram_lock(path);
	unlink(path);
	nvram_unlock(lock);
	return 0;
}

char* nvram_get(const char* category, const char* key, char *outbuf)
{
    char *pt;
	char path[256];

	*outbuf = '\0';

	snprintf( path, 256, "%s%s", NVRAM_BASE_PATH, category);

	if((pt=__nvram_get_func(key, path))==NULL)
		goto _end;

	if ( pt && !strncmp( pt, "*DEL*", 5 ) ) // check if this is the deleted var
	{
		free(pt);
		pt = NULL;
	}

	if (pt) {
		strcpy(outbuf, pt);
		free(pt);
		pt = NULL;
	}
_end:
	return outbuf;
}

char* nvram_get_idx(const char* category, const char* key, int key_idx, char *outbuf)
{
    char *pt;
	char name[256] = {0x00};
	char path[256];

	*outbuf = '\0';
	snprintf( name, sizeof(name), "%s%d", key, key_idx);
	snprintf( path, 256, "%s%s", NVRAM_BASE_PATH, category);

	if((pt=__nvram_get_func(name, path))==NULL)
		goto _end;

	if ( pt && !strncmp( pt, "*DEL*", 5 ) ) // check if this is the deleted var
	{
		free(pt);
		pt = NULL;
	}

	if (pt) {
		strcpy(outbuf, pt);
		free(pt);
	}
_end:
	return outbuf;
}

int nvram_get_int(const char* category, const char* key)
{
	char* pt;	
	int ret=0;
	char outbuf[256] = "";

	pt = nvram_get(category, key, outbuf);

	if(pt)
	{
		ret = atoi(pt);
	}

	return ret;
}

int nvram_get_idx_int(const char* category, const char* key, int key_idx)
{
	char* pt;	
	int ret=0;
	char outbuf[256] = "";

	pt = nvram_get_idx(category, key, key_idx, outbuf);

	if(pt)
	{
		ret = atoi(pt);
	}

	return ret;
}


#ifndef _NVRAM_H
#define _NVRAM_H


#define NVRAM_BASE_PATH  "./"


#define END_SYMBOL	    0x00		  	
#define DIVISION_SYMBOL	    0x01





/* errorno */
#define NVRAM_SUCCESS       0
#define NVRAM_FLASH_ERR     1 
#define NVRAM_MAGIC_ERR	    2
#define NVRAM_LEN_ERR	    3
#define NVRAM_CRC_ERR	    4
#define NVRAM_SHADOW_ERR    5

int nvram_set(const char* category, const char* key, const char* value);
int nvram_set_idx(const char* category, const char* key, int key_idx, const char* value);
int nvram_set_int(const char* category, const char* key, int value);
int nvram_set_idx_int(const char* category, const char* key, int key_idx, int value);

int nvram_unset(const char *category);

char* nvram_get(const char* category, const char* key, char *outbuf);
char* nvram_get_idx(const char* category, const char* key, int key_idx, char *outbuf);
int nvram_get_int(const char* category, const char* key);
int nvram_get_idx_int(const char* category, const char* key, int key_idx);



#endif


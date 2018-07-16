#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/limits.h>
#include "nvram_lock.h"


/*
 *==================================================================
 * Description: Add lock to avoid operator the same nvram file
 *        in the same time. Locker path: /var/lock/(path).lock
 * Input:
 *     path      --- the url of nvram file
 * Output:
 *     fd        --- descriptor of locker file
 *==================================================================
 */
int nvram_lock(const char *path)
{
	char lock_path[PATH_MAX];
	char *p=NULL;
	int fd;
	
	sprintf(lock_path, "%s%s.lock", LOCK_PATH_BASE, path);
	
	// change character '/' to '_'
	p = lock_path + sizeof(LOCK_PATH_BASE);
	while(*p)
	{
		if( *p == '/' )
			*p = '_';
		p++;
	}
	p = NULL;
	
	fd = open( lock_path, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
	if( fd < 0 )
		return -1;
	if( lockf(fd, F_LOCK, 0) < 0 )
	{
		close(fd);
		return -1;
	}
	
	return fd;
}

/*
 *==================================================================
 * Description: Unlock nvram file.
 * Input:
 *     fd        --- descriptor of locker file
 * Output:
 *     Null
 *==================================================================
 */
 void nvram_unlock(int fd)
 {
	 if( fd >= 0 )
	 {
		 lockf(fd, F_ULOCK, 0);
		 close(fd);
	 }
 }
 
 
 
 
 

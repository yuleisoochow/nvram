#ifndef _NVRAM_LOCK_H
#define _NVRAM_LOCK_H

#define LOCK_PATH_BASE "/var/lock/"

int nvram_lock(const char *path);
void nvram_unlock(int fd);

#endif

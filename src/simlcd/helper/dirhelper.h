#ifndef _DIRHELPER_H
#define _DIRHELPER_H

#include <stdbool.h>

#include <basedir_fs.h>

void create_dir(char* dir);
char* create_user_dir(char* dirtype, char* programpath);
char* gethome();
char* str2md5(const char* str, int length);
bool does_directory_exist(char* path, char* dirname);
void restrict_folders_to_cache(char* path, int cachesize);
void delete_dir(char* path);
char* get_config_file(const char* confpath, xdgHandle* xdg);
char* get_dir_with_default(const char* dirpath, char* defaultpath);

#endif

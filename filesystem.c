#define _XOPEN_SOURCE 700

#include "filesystem.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

int path_exists(const char *path){
  struct stat st;

  if (stat(path, &st) != 0)
    return 0;

  return S_ISDIR(st.st_mode);
}

int file_exists(const char *path){
  struct stat st;

  if (stat(path, &st) != 0)
    return 0;

  return S_ISREG(st.st_mode);
}

int absolute_path(const char *path, char *buffer, size_t size){
  char *resolved = realpath(path, NULL);

  if (resolved == NULL)
    return 1;

  if (strlen(resolved) >= size) {
    free(resolved);
    return 1;
  }

  strcpy(buffer, resolved);

  free(resolved);
  return 0;
}

char ** Percorre_Diretoria(const char *dir,int *count){
  DIR* dirent;
  char** listanomes=(char**)malloc(sizeof(char*)*10);
  struct dirent *data;
  int tamanhomaximo=10;
  if((dirent=opendir(dir))!=NULL){
    while ((data=readdir(dirent))!=NULL){
      if(tamanhomaximo-*count<2){
        tamanhomaximo+=10*sizeof(char*);
        listanomes=realloc(listanomes,tamanhomaximo);
      }
      int tamanhonome=strlen(data->d_name);
      char* apanhaconf=data->d_name+tamanhonome*sizeof(char)-5;
      if (tamanhonome>=5){
        if(strncmp(apanhaconf,".conf",5*sizeof(char))==0){
          listanomes[*count]=strdup(data->d_name);
          count++;
        }
    }

    }
    closedir(dirent);
    return listanomes;
  }
  else{
    perror("erro a abrir diretoria");
    return NULL;  }   
}
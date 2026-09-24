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
  /*passamos endereço do count(vai contar os ficheiros na dir) como parametro
    pois assim vamos poder,alem de ter a lista de nomes,ter o numero de ficheiros*/
  DIR* dirent; /*inicializamos o dirent que vai ligar as diversas fases(open,read e close)*/
  char** listanomes=(char**)malloc(sizeof(char*)*10); /*damos malloc de uma lista de ponteiros(strings)
                                                        que tem tamanho inicial de 10 strings*/
  struct dirent *data;   /*com struct *data vamos ter acesso á info proveniente das funções open,read e close*/
  int tamanhomaximo=10;  /*tamanho inicial da lista*/
  if((dirent=opendir(dir))!=NULL){ /*garantimos que o open dir corre com sucesso*/
    while ((data=readdir(dirent))!=NULL){ /*enquanto houver ficheiros,continuamos a correr o read*/
      if(tamanhomaximo-*count<2){  /*se tivermos a aproximar do limite da lista,realocamos espaço para mais 10 strings*/
        tamanhomaximo+=10*sizeof(char*);
        listanomes=realloc(listanomes,tamanhomaximo);
      }
      int tamanhonome=strlen(data->d_name);
      char* apanhaconf=data->d_name+tamanhonome*sizeof(char)-5; /*vamos avançar o ponteiro do nome
                                                                  para até ao fim e depois retrocedemos 5 para ficarmos
                                                                  com os bytes onde é possivel estar o .conf(5 bytes) */
      if (tamanhonome>=5){ /*se o nome for maior ou igual a 5,então pode ser um .conf pois .conf ocupa 5 bytes*/
        if(strncmp(apanhaconf,".conf",5*sizeof(char))==0){ /*se os ultimos 5 bytes do nome original forem .conf
                                                            então usamos o strdup para criar um ponteiro
                                                            com tamanho igual ao nome e inserimos na nossa lista 
                                                            no indice do count,*/
          listanomes[*count]=strdup(data->d_name);
          count++; /*count=ficheiros .conf lidos com sucesso*/
        }
    }

    }
    closedir(dirent);  /*fechamos o repositorio*/
    return listanomes; /*retornamos a lista de nomes*/
  }
  else{
    perror("erro a abrir diretoria"); /*caso o opendir falhou,então exibimos mensagem de erro e retornamos NULL*/
    return NULL;  }   
}
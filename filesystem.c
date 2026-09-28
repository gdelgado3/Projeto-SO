#define _XOPEN_SOURCE 700
#define COPY_BUFFER_SIZE 4096

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

int compara(const void* a, const void *b){  /*função que basicamente apenas chama o strcmp para comparar os 
                                                      nomes dos ficheiros .conf para o qsort ordenar alfabeticamente*/
                                                      
  char **elemento_a=(char **)a;
  char **elemento_b=(char **)b;
  int valor= strcmp(*elemento_a,*elemento_b);
  return valor;
}


char ** Percorre_Diretoria(const char *dir,size_t *count){
  /*passamos endereço do count(vai contar os ficheiros na dir) como parametro
    pois assim vamos poder,alem de ter a lista de nomes,ter o numero de ficheiros*/
  DIR* dirent; /*inicializamos o dirent que vai ligar as diversas fases(open,read e close)*/
  char** listanomes=(char**)malloc(sizeof(char*)*10); /*damos malloc de uma lista de ponteiros(strings)
                                                        que tem tamanho inicial de 10 strings*/
  struct dirent *data;   /*com struct *data vamos ter acesso á info proveniente das funções open,read e close*/
  size_t tamanhomaximo=10;  /*tamanho inicial da lista*/
  if((dirent=opendir(dir))!=NULL){ /*garantimos que o open dir corre com sucesso*/
    while ((data=readdir(dirent))!=NULL){ /*enquanto houver ficheiros,continuamos a correr o read*/
      if(tamanhomaximo-*count<2){  /*se tivermos a aproximar do limite da lista,realocamos espaço para mais 10 strings*/
        tamanhomaximo+=10;
        listanomes=realloc(listanomes,tamanhomaximo * sizeof(char*));
      }
      size_t tamanhonome=strlen(data->d_name);
       /*vamos avançar o ponteiro do nome
                                                                  para até ao fim e depois retrocedemos 5 para ficarmos
                                                                  com os bytes onde é possivel estar o .conf(5 bytes) */
      if (tamanhonome>=5){ /*se o nome for maior ou igual a 5,então pode ser um .conf pois .conf ocupa 5 bytes*/
        char* apanhaconf=data->d_name+tamanhonome -5;
        if(strncmp(apanhaconf,".conf",5)==0){ /*se os ultimos 5 bytes do nome original forem .conf
                                                            então usamos o strdup para criar um ponteiro
                                                            com tamanho igual ao nome e inserimos na nossa lista 
                                                            no indice do count,*/
          listanomes[*count]=strdup(data->d_name);
          (*count)++; /*count=ficheiros .conf lidos com sucesso*/
        }
    }

    }
    closedir(dirent);  /*fechamos o repositorio*/

    qsort(listanomes,*count,sizeof(char *),compara);
    return listanomes; /*retornamos a lista de nomes*/
  }
  else{
    perror("erro a abrir diretoria"); /*caso o opendir falhou,então exibimos mensagem de erro e retornamos NULL*/
    free(listanomes);
    return NULL;  }   
}


int copia_ficheiro(const char *inicio,const char *fim){
  int fd_inicio=open(inicio,O_RDONLY);

  if(fd_inicio<0){
    perror("open error");
    return 1;   /*deu erro na abertura*/
  }

  int fd_fim=open(fim,O_WRONLY|O_CREAT|O_TRUNC,0644);
  if(fd_fim<0){

    perror("open error");
    close(fd_inicio);
    return 1;   /*deu erro na abertura*/
  }

  char buffer_copia[COPY_BUFFER_SIZE];

  while(1){
    ssize_t bytes_lidos= read(fd_inicio,buffer_copia,sizeof(buffer_copia));

    if(bytes_lidos<0){
      perror("Erro na leitura");
      close(fd_inicio);
      close(fd_fim);
      
      return 1;
    }

    else if(bytes_lidos==0){
      break;
    }

    ssize_t escritos=0;
    while(escritos<bytes_lidos){
      ssize_t bytes_escritos=write(fd_fim,buffer_copia + escritos,(size_t)(bytes_lidos - escritos));

      if(bytes_escritos<0){
        perror("Erro na escrita");
        close(fd_inicio);
        close(fd_fim);
        
        return 1;
      }

      escritos+= bytes_escritos;

    }
  }

  close(fd_inicio);
  close(fd_fim);
  return 0;

}
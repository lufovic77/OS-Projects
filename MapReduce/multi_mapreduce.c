#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "mapreduce.h"
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
typedef struct db {
	int count;
	int partition_num;
	struct db* cur;
	struct db* left;
	struct db* right;	
	char key[40];

}db_t;
db_t * array[1010];
char * next(char *a, int b);
Partitioner hashing;
Reducer traversal;
Mapper map2;
/////////////
int order;
int num_thread;
int visited[10000];
int gindex=0;
int not_null=0;
db_t* hashtable[1010];
////////////////hashtable info
/////////////////
int file_num=0;
char filename[1000][100];
/////////////////file info
//int count=0;//used in getter function
void inorder(db_t* node);
void MR_Emit(char *key, char *value){
	//printf("world!");
	unsigned long hash=hashing(key, 1000);	
	db_t* temp=array[hash];
	db_t* space;
	db_t* parent=NULL;
	int n;
	int val=atoi(value);
	int right=-1;
	if(temp==NULL){	//nothing in the tree(input key is the root)
		space=(db_t*)malloc(sizeof(db_t));
		space->cur=NULL;
		space->left=NULL;
		space->right=NULL;
		space->partition_num=hash;
		strcpy(space->key,key);
		space->count=val;
		array[hash]=space;
		return ;
	}
	else{//adding nodes
		while(temp!=NULL){
			parent=temp;
			n=strcmp(temp->key, key);
			if(n==0){	//identical
				temp->count+=val;
				return ;
			}
			else if(n>0){	//temp>key
				temp=temp->left;
				right=0;
			}
			else{	//temp<key
				temp=temp->right;
				right=1;
			}
		}
	}
	space=(db_t*)malloc(sizeof(db_t));	//nothing matched
	if(right){	//if the node is right to the parent
		parent->right=space;
	}
	else{
		parent->left=space;
	}
	space->right=NULL;
	space->left=NULL;
	space->partition_num=hash;
	strcpy(space->key, key);
	space->count=val;
	//binary search

	return ;
}
void *thread_map(void *arg){
	int i;
	pthread_mutex_lock(&mutex);
	int amount=file_num/num_thread;
	if(num_thread!=order){
		for(i=0;i<amount;i++){
			map2(filename[gindex]);
			gindex++;
		}	
	}
	else{	//last thread
		while(gindex!=file_num){
			map2(filename[gindex]);
			gindex++;
		}
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
	return NULL;
}
void *thread_reduce(void *arg){
	int i;
	pthread_mutex_lock(&mutex);
	int amount=not_null/num_thread;
	if(num_thread!=order){
		for(i=0;i<amount;i++){
			inorder(hashtable[gindex]);
			gindex++;
		}
	}
	else{//last thread
		while(gindex!=not_null){
			inorder(hashtable[gindex]);
			gindex++;
		}
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
	return NULL;
}

void MR_Run(int argc, char *argv[], Mapper map, int num_mappers, Reducer reduce, int num_reducers, Partitioner partition){
///////////////////////////// thread info
	pthread_t p_thread[100];
//	int status;
	int thr_id[100];
/////////////////////////////		
	int i;
	int trash;
	int limit=atoi(argv[2]);
	db_t * node;

	for(i=0;i<limit;i++){
		array[i]=NULL;
	}
	map2=map;
	traversal=reduce;
	hashing=partition; //hashing function
	DIR *dir_info=NULL;
    struct dirent *dir_entry=NULL;

	char path[40];
	strcpy(path, argv[1]);
	dir_info = opendir(argv[1]);           
	if(dir_info==NULL){
		fprintf(stderr, "Failed");
		return ;
	}
	i=0;
	strcpy(filename[0], argv[1]);
	while((dir_entry = readdir(dir_info))!=NULL){
		if(!strcmp(dir_entry->d_name, "."))
			continue;
		if(!strcmp(dir_entry->d_name, ".."))
			continue;
		file_num++;
		strcat(filename[i], dir_entry->d_name);		
		i++;
		strcpy(filename[i], argv[1]);
	}
	closedir( dir_info);
	
	num_thread=num_mappers;
	order=0;
	if(file_num <num_mappers)
		num_thread=file_num;	
	for(i=0;i<num_thread;i++){
		thr_id[i] = pthread_create(&p_thread[i], NULL, thread_map,(void*)&i);
		order++;
	}
	for(i=0;i<num_thread;i++)
		 pthread_join(p_thread[i], NULL);
	trash=thr_id[0];
	i=trash;
	int m=0;
	order=0;
	gindex=0;
	for(i=0;i<limit;i++){
		node=array[i];
		if(node==NULL){
			continue;
		}
		else{
			hashtable[m]=node;
			m++;
			node->cur=NULL;
		}
	}
	not_null=m;
	m--;
	num_thread=num_mappers;
	if(m<num_mappers){
		num_thread=m;
	}

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	for(i=0;i<num_thread;i++){
		thr_id[i]=pthread_create(&p_thread[i],NULL,thread_reduce,(void*)&i);
		order++;
	}
	for(i=0;i<num_thread;i++)
		pthread_join(p_thread[i],NULL);

	for(i=0;i<limit;i++)
		free(array[i]);
}	

char * next(char * key, int partition_num){
	db_t* root=array[partition_num];
	int count,n=0;
	db_t*parent=NULL;
	if(root->cur==NULL){
		db_t* temp=root;
		while(temp!=NULL){
		parent=temp;
		n=strcmp(temp->key, key);
			if(n==0){	//identical
				break;
			}
			else if(n>0){	//temp>key
				temp=temp->left;
			}
			else{	//temp<key
				temp=temp->right;
			}
		}
		if(n==0)
			root->cur=temp;
		else
		root->cur=parent;

	}
	else{
		root->cur->count-=1;
		count=root->cur->count;
		if(count==0){
			root->cur=NULL;
			return NULL;
		}
	}
	return "1";
	
}
void inorder(db_t * node){
	if(node == NULL){
		return;
	}
    inorder(node->left);
	  	traversal(node->key, next,node->partition_num); 
    inorder(node->right);
	return ;
}

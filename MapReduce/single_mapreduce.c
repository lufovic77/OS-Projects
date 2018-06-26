#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mapreduce.h"
typedef struct db {
	int count;
	struct db* left;
	struct db* right;	
	char key[60];

}db_t;
db_t * array[1010];
Partitioner hashing;
Reducer traversal;
int count=0;//used in getter function
void inorder(db_t* node);
void MR_Emit(char *key, char *value){
	unsigned long hash=hashing(key, 1000);	
	db_t* temp=array[hash];
	db_t* space;
	db_t* parent=NULL;
	int n;
	int val=atoi(value);
	int right=-1;
	if(temp==NULL){	//nothing in the tree(input key is the root)
		space=(db_t*)malloc(sizeof(db_t));
		space->left=NULL;
		space->right=NULL;
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
	strcpy(space->key, key);
	space->count+=val;
	//binary search

	return ;
}

void MR_Run(int argc, char *argv[], Mapper map, int num_mappers, Reducer reduce, int num_reducers, Partitioner partition){
	int i;
	int limit=atoi(argv[2]);
	db_t * node;

	for(i=0;i<limit;i++){
		array[i]=NULL;
	}
	traversal=reduce;
	hashing=partition; //hashing function
	DIR *dir_info=NULL;
    struct dirent *dir_entry=NULL;
	char path[60];
	strcpy(path, argv[1]);
	dir_info = opendir(argv[1]);           
	if ( NULL != dir_info){
		while( (dir_entry = readdir(dir_info))!=NULL){  	
			if(!strcmp(dir_entry->d_name, "."))
				continue;
			if(!strcmp(dir_entry->d_name, ".."))
				continue;
			strcat(path, dir_entry->d_name);
			map(path);			
			strcpy(path, argv[1]);
		}
		closedir( dir_info);
	} 
	for(i=0;i<limit;i++){
		node=array[i];
		if(node==NULL){
			continue;
		}
		else{
			inorder(node);			
		}
	}
	for(i=0;i<limit;i++)
		free(array[i]);
}	

char * next(char * key, int partition_num){
	if(count==-1){
		count=partition_num;
		return key;
	}
	else{
		if(count==1)
			return NULL;
		else{
			count--;
			return key;
		}
	}
}

void inorder(db_t * node){
	if(node == NULL){
		return;
	}
    inorder(node->left);
	count=-1;
	  	traversal(node->key, next,node->count); 
    inorder(node->right);
	return ;
}

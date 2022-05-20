#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Hash *HashTable;
struct Hash{
    int Rank;
    char ChannelName[30];
    char Uploads[20];
    char Subscribers[20];
    char Views[20];
    int Flag;
};
int NumOfData = 0;

int IsPrime(int x){
    int i=2,flag=1;
    while(i<x){
        if((x%i)==0)
        return 0;
        i++;
    }
    return flag;
}

unsigned int HashFunction(char* key,int TableSize){
    unsigned int HashValue=0;
    while(*key != '\0')
        HashValue=(HashValue<<5)+ *key++;
    return (HashValue%TableSize);
}

void UpdateHash(HashTable hashtable[],int key,char Uploads[20],char Subscribers[20],char Views[20]){
    unsigned long uploads = convert(hashtable[key]->Uploads);
    unsigned long subscribers = convert(hashtable[key]->Subscribers);
    unsigned long views = convert(hashtable[key]->Views);
    unsigned long New_Uploads = convert(Uploads);
    unsigned long New_Subscribers = convert(Subscribers);
    unsigned long New_Views = convert(Views);

    New_Uploads = New_Uploads + uploads;
    New_Subscribers = New_Subscribers + subscribers;
    New_Views = New_Views + views;

    sprintf(hashtable[key]->Uploads, "%lu", New_Uploads);
    sprintf(hashtable[key]->Subscribers, "%lu", New_Subscribers);
    sprintf(hashtable[key]->Views, "%lu\n", New_Views);

    Reconvert(hashtable[key]->Uploads);
    Reconvert(hashtable[key]->Subscribers);
    Reconvert(hashtable[key]->Views);
}

int HashSearch(HashTable hashtable[],int HashSize,char ChannelName[30]){
    int key=0;
    int i=1;
    key = HashFunction(ChannelName,HashSize);
    while((strcmp(hashtable[key]->ChannelName,ChannelName)!=0)&&(i<=HashSize)){
        key=(key+(i*i))%HashSize;
        i++;
    }
    if(i>HashSize || hashtable[key]->Flag == 2)
        return -1;
    else
        return key;
}

void HashInsert(HashTable hashtable[],int HashSize ,int Rank,char ChannelName[30],char Uploads[20],char Subscribers[20],char Views[20]){
    int key=0;
    int i=1;

    int hashedkey = HashSearch(hashtable,HashSize,ChannelName);
    if(hashedkey != -1){
        UpdateHash(hashtable,hashedkey,Uploads,Subscribers,Views);
    }
    else{
    key = HashFunction(ChannelName,HashSize);
    while(hashtable[key]->Flag == 1){
        key=(key+(i*i))%HashSize;
        i++;
    }
        hashtable[key]->Rank = Rank;
        strcpy(hashtable[key]->ChannelName,ChannelName);
        strcpy(hashtable[key]->Uploads,Uploads);
        strcpy(hashtable[key]->Subscribers,Subscribers);
        strcpy(hashtable[key]->Views,Views);
        hashtable[key]->Flag = 1;
    }
}

int getNewHashSize(int n){
    int newHashSize = 2*n + 1;
    while(!IsPrime(newHashSize))
        newHashSize++;
    return newHashSize;
}

void Rehash(HashTable hashtable[], int hashsize){
    int oldhashsize = hashsize;
    int newhashsize = getNewHashSize(hashsize);
    hashsize = newhashsize;
    HashTable newhashtable[hashsize];
    for(int i=0;i<oldhashsize;i++){
        newhashtable[i] = malloc(sizeof(struct Hash));
        newhashtable[i]->Flag = 0;
        HashInsert(newhashtable,hashsize,hashtable[i]->Rank,hashtable[i]->ChannelName,hashtable[i]->Uploads,hashtable[i]->Subscribers,hashtable[i]->Views);
    }
    free(hashtable);
    hashtable = newhashtable;
}

void ReadHashFile(HashTable hashtable[],int HashSize){
    int i;
    FILE *inp;
    for(i=0;i<HashSize;i++){
        hashtable[i]=malloc(sizeof(struct Hash));
        hashtable[i]->Flag = 0;
    }
    int Rank;
    char ChannelName[30];
    char Uploads[20];
    char Subscribers[20];
    char Views[20];
    char line[1000];
    inp = fopen("Youtube+Channels.txt","r");
    while(fgets(line,1000,inp) != 0){
        if(feof(inp))
        	break;
    	else
            Rank = atoi(strtok(line,"\t"));
            strcpy(ChannelName,strtok(NULL,"\t"));
            strcpy(Uploads,strtok(NULL,"\t"));
            strcpy(Subscribers,strtok(NULL,"\t"));
            strcpy(Views,strtok(NULL,"\t"));
            NumOfData++;
            /*if(NumOfData > (HashSize/2))
                Rehash(hashtable,HashSize);
            else*/
            HashInsert(hashtable,HashSize,Rank,ChannelName,Uploads,Subscribers,Views);
    }
        fclose(inp);
}

void HashDelete(HashTable hashtable[],int HashSize,char ChannelName[]){
    int key=0;
    int i=1;
    key = HashFunction(ChannelName,HashSize);
    while(strcmp(hashtable[key]->ChannelName,ChannelName)!=0 && i<=HashSize){
        key=(key+i)%HashSize;
        i++;
    }
    if(i<HashSize || hashtable[key]->Flag == 2)
        hashtable[key]->Flag = 2;
}

void printHash(HashTable hashtable[],int HashSize){
    for(int i=0;i<HashSize;i++){
        if(hashtable[i]->Flag == 1)
            printf("At index: %d   %2d  %s  %s  %s  %s",i,hashtable[i]->Rank,hashtable[i]->ChannelName,hashtable[i]->Uploads,hashtable[i]->Subscribers,hashtable[i]->Views);
        else if(hashtable[i]->Flag == 0)
            printf("At index: %d        \n",i);
        }
}

void SaveHashData(FILE *outp, HashTable hashtable[], int HashSize){
    for(int i=0;i<HashSize;i++)
    if(hashtable[i]->Flag == 1)
        fprintf(outp,"%2d	%s	%s	%s	%s",hashtable[i]->Rank,hashtable[i]->ChannelName,hashtable[i]->Uploads,hashtable[i]->Subscribers,hashtable[i]->Views);
}

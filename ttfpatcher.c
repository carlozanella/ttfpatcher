#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

struct table_record
{
  char tag[4];
  uint32_t checksum;
  uint32_t offset;
  uint32_t length;
  
};

struct table_directory
{
  uint32_t sfntVersion;
  uint16_t numTables;
  uint16_t searchRange;
  uint16_t entrySelector;
  uint16_t rangeShift;
};

struct OS2_partial {
  uint16_t version;
  int16_t xAvgCharWidth;
  int16_t usWeightClass;
  int16_t usWidthClass;
  int16_t fsType;
};


int main(int argv, char** argc)
{
  // Parsing arguments

  if(!(argv >= 2 && argv <=3)) {
    printf("Usage: %s ttf-file [newFsType]\n\nBy default newFsType=0 which removes all restrictions.\n",argc[0]);
    exit(1);
  }
  
  int newFsType = 0;
  if(argv == 3) {
    if(strlen(argc[2]) != 1 || !isdigit(argc[2][0])) {
      printf("Invalid newFsType argument. Choose a single digit.\n");
      exit(1);
    }
    newFsType = atoi(argc[2]);
  }

  // Opening and reading file
  
  FILE *fp = fopen(argc[1],"rb");
  if(fp == NULL) {
    printf("Cannot open file for reading\n");
    exit(1);
  }

  fseek(fp, 0L, SEEK_END);
  int sz = ftell(fp);

  char *buffer = (char*) malloc(sz);

  rewind(fp);
  fread(buffer,sizeof(char),sz,fp);
  fclose(fp);
  
  // Parsing the table directory
  struct table_directory *header = (struct table_directory*) buffer;

  int num_tables = ntohs(header->numTables);
  int version = ntohl(header->sfntVersion);
  int strue = ntohl(*((int*)"true"));

  if (version == strue || version == 0x00010000)
    printf("Recognized ttf version 0x%08X version\n", version);
  else {
    printf("Unknown true type file version\n");
    exit(1);
  }
  if (sz < sizeof(struct table_directory) + sizeof(struct table_record)*num_tables) {
    printf("Invalid ttf file");
    exit(1);
  }
    
  printf("Read %d bytes and found %d tables\n", sz,num_tables);

  
  // Looking for the OS/2 table directory
  struct table_record *tr = (struct table_record*)(buffer+sizeof(struct table_directory));
  int j = -1;
  for(int i = 0 ; i < num_tables ; ++i) {
    if(memcmp(tr[i].tag,"OS/2",4) == 0) {
      printf("Found table OS/2 at offset 0x%X",tr[i].offset);
      j = i;
      break;
    }
  }

  if(j == -1) {
    printf("Did not find OS/2 table. Nothing to be done.\n");
    exit(0);
  }

  // Checking the OS/2 table
  int offset = ntohl(tr[j].offset);

  if (offset + sizeof(struct OS2_partial) > sz) {
    printf("Invalid ttf file!\n");
    exit(2);
  }
  
  struct OS2_partial *os2 = (struct OS2_partial*)(buffer + offset);


  int fsType = ntohl(os2->fsType);
  printf("OS/2 version: %d\n",ntohl(os2->version));
  printf("fsType: 0x%04X\n",fsType);
  printf("newFfsType: 0x%04X\n",newFsType);
  
  // Patching the byte and writing the buffer back to the file

  os2->fsType = htons(newFsType);

  fp = fopen("basicsanssf.ttf","wb");
  size_t ne = fwrite(buffer,sizeof(char),sz,fp);

  printf("OK: %ld bytes written\n",ne);
  
  fclose(fp);
  free(buffer);
  
  return 0;
}


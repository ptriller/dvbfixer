#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


struct t_tspacket {
  unsigned int sync :8; // Syncbyte, has to be 0x47
  unsigned int TEI :1; // Transport Error Indicator
  unsigned int PUSI :1; // Payload Unit Start Indicator
  unsigned int TP :1; // Transport Priority
  unsigned int PID :13; // Packet ID
  unsigned int SC : 2; //Scrambling control
  unsigned int AFE : 2; // Adaption field exists
  unsigned int CC : 4; // Continuity counter
  unsigned char data[184];
} __attribute__((__packed__));

struct t_pid {
  unsigned int pid;
  unsigned int count;
  FILE *output;
  struct t_pid * next;
};

struct t_pmt {
  unsigned int offset : 8;
  unsigned int tableid : 8; // Table Id 0x02 for PMT
  unsigned int ssi : 1; // Section syntax indicator
  unsigned int zero : 1; // Must be 0
  unsigned int reserved : 2 ; // always 11
  unsigned int sectionLen: 12; // Section length umpost bits must be 00
  unsigned int programNum: 16;
  unsigned int reserved2 : 2;
  unsigned int version: 5; // incremented each change
  unsigned int currNextInd: 1; //If 1, this table is currently valid. If 0, this table will become valid next.
  unsigned int sectionNum: 8; // always 0x00
  unsigned int nextSectionNum: 8; // always 0x00
  unsigned int reserved3 : 3; // ???
  unsigned int pcrPID: 13; // PID of general timecode stream or 0x1FFF
  unsigned int reserved4: 4;
  unsigned int infoLen: 12; // Length of following Data
  unsigned char data[171];
}  __attribute__((__packed__));

int main(int argc, const char **argv) {
  FILE *input;
  char buf[256];
  struct t_tspacket packet;
  struct t_pid *pid = NULL;
  struct t_pid *i;
  struct t_pmt *pmt;
  unsigned char *data;
  if(argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n",argv[0]);
    return -1;
  }
  printf("test %d\n", sizeof(struct t_tspacket));
  input = fopen(argv[1],"rb");
  if(input == NULL) {
    fprintf(stderr,"Error: %s %d\n",argv[1], errno);
    return -1;
  }
  while(1 == fread(&packet,sizeof(struct t_tspacket),1,input)) {
    if(packet.sync != 0x47) {
      fprintf(stderr,"ERROR, sync failed\n");
    }
    if(packet.PUSI == 1 && (packet.AFE & 0x01) != 0) {
      data = packet.data;
      if((packet.AFE & 0x02) != 0) {
	data = data+ data[0]+1;
	data = data +data[0];
	pmt = (struct t_pmt *)data;
	printf("PUSI found in PID %04d: %04d\n", packet.PID, pmt->tableid);
	//	if(pmt->tableid == 0x02) {
	  
	//	}
      }
    }
    i = pid;
    while(i != NULL) {
      if(i->pid == packet.PID) {
	i->count++;
	fwrite(&packet,sizeof(struct t_tspacket),1,i->output);
	break;
      }
      i = i->next;
    }
    if(i == NULL) {
      i = malloc(sizeof(struct t_pid));
      i->pid = packet.PID;
      i->next = pid;
      i->count = 1;
      sprintf(buf,"stream%x.mpg",packet.PID);
      i->output = fopen(buf,"wb");
      fwrite(&packet,sizeof(struct t_tspacket),1,i->output);
      pid = i;
    }
  }
  fclose(input);
  i = pid;
  while(i != NULL) {
    printf("PID 0x%04x - %d\n", i->pid, i->count);
    fclose(i->output);
    i = i->next;
  }
  return 0;
}

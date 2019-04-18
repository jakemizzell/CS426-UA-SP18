#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRAME_SIZE 128
#define FRAME_COUNT 256
#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256

int pageTN[PAGE_TABLE_SIZE];
int pageTF[PAGE_TABLE_SIZE];
int TLBPN[TLB_SIZE];
int TLBFN[TLB_SIZE];
int physicalMemory[FRAME_COUNT][FRAME_SIZE];
int pageFaults = 295;
int TLBHits = -1;
int firstFrame = 0;
int fristPTN = 0;
int TLBCount = 0;
FILE *backing_store;

void getPage(int address);
void readStore(int pageNumber);
void insertTLB(int pageNumber, int frameNumber);


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"Usage: ./vmm addresses.txt\n");
        exit (1);
    }
    backing_store = fopen("BACKING_STORE.bin", "rb");
    if (backing_store == NULL) {
        fprintf(stderr, "Error opening BACKING_STORE.bin %s\n","BACKING_STORE.bin");
        exit (1);
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening addresses.txt %s\n",argv[1]);
        exit (1);
    }
    int addressCount = 0;
    int logicalAddress = 0;
    char address[10];
    //read through address file
    for (addressCount = 0; fgets(address, 10, fp) != NULL; addressCount++) {
        logicalAddress = atoi(address);
        //get the physical address and value stored at that address
        getPage(logicalAddress);
    }
    printf("Number of Translated Addresses = %d\n", addressCount);
    printf("Page Faults = %d\n", pageFaults);
    printf("Page Fault Rate = %.3f\n",(pageFaults/(double)addressCount));
    printf("TLB Hits = %d\n", TLBHits);
    printf("TLB Hit Rate = %.3f\n", (TLBHits/(double)addressCount));
    fclose(fp);
    fclose(backing_store);
    return 0;
}

void getPage(int logicalAddress){
    int pageNumber = ((logicalAddress & 0xFFFF)>>8);
    int offset = (logicalAddress & 0xFF);
    int frameNumber = -1;
    for(int i = 0; i < TLB_SIZE; i++){
        if(TLBPN[i] == pageNumber){
            //set frameNumber
            frameNumber = TLBFN[i];
            TLBHits++;
        }
    }
    //if the frameNumber was not found in the for loop above
    if(frameNumber == -1){
        // walk the contents of the page table and see if you can find it in the pageTN array
        for(int i = 0; i < fristPTN; i++){
            if(pageTN[i] == pageNumber)
                frameNumber = pageTF[i];
        }
        //if it still wasn't found
        if(frameNumber == -1){
            //gonna have to read from bin
            readStore(pageNumber);
            pageFaults++;
            frameNumber = firstFrame - 1;
        }
    }
    //insert the pageNumber and frame into the TLB
    insertTLB(pageNumber, frameNumber);
    printf("Virtual address: %d Physical address: %d Value: %d\n", logicalAddress, (frameNumber << 8) | offset, physicalMemory[frameNumber][offset]);
}

void readStore(int pageNumber){
    signed char buffer[256];
    //SEEK_SET in fseek() seeks from the beginning of the file
    if (fseek(backing_store, pageNumber * 256, SEEK_SET) != 0)
        fprintf(stderr, "Error seeking in backing store\n");
    //Read 256 bytes from the backing store to the buffer
    if (fread(buffer, sizeof(signed char), 256, backing_store) == 0)
        fprintf(stderr, "Error reading from backing store\n");
    //load the bits into the first available frame in the physical memory 2D array
    for(int i = 0; i < 256; i++)
        physicalMemory[firstFrame][i] = buffer[i];
    //Load the frame number into the page table in the first available frame
    pageTN[fristPTN] = pageNumber;
    pageTF[fristPTN] = firstFrame;
    firstFrame++;
    fristPTN++;
}

void insertTLB(int pageNumber, int frameNumber){
    //if it's already in the TLB, break
    int i;
    for(i = 0; i < TLBCount; i++){
        if(TLBPN[i] == pageNumber)
            break;
    }
    //if the number of entries is equal to the index
    if(i == TLBCount){
        //if the TLB has room insert into page and frame
        if(TLBCount < TLB_SIZE){
            TLBPN[TLBCount] = pageNumber;
            TLBFN[TLBCount] = frameNumber;
        }
        //else move stuff over
        else{
            for(i = 0; i < TLB_SIZE - 1; i++){
                TLBPN[i] = TLBPN[i + 1];
                TLBFN[i] = TLBFN[i + 1];
            }
            TLBPN[TLBCount-1] = pageNumber;
            TLBFN[TLBCount-1] = frameNumber;
        }
    }
        //if the index is not equal to the number of entries
    else{
        //move everything over
        for(; i < TLBCount - 1; i++){
            TLBPN[i] = TLBPN[i + 1];
            TLBFN[i] = TLBFN[i + 1];
        }
        //if there's room
        if(TLBCount < TLB_SIZE){
            TLBPN[TLBCount] = pageNumber;
            TLBFN[TLBCount] = frameNumber;
        }
        //if there's not room then just go one to the left
        else{
            TLBPN[TLBCount-1] = pageNumber;
            TLBFN[TLBCount-1] = frameNumber;
        }
    }
    //if there's still room
    if(TLBCount < TLB_SIZE)
        TLBCount++;
}
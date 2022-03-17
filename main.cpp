#include <stdio.h>  //FILE
#include <stdlib.h> //exit
#include <unistd.h> //getopt
#include <vector>   //vector
#include <locale>   //isdigit

#include "PageTable.h"
#include "tracereader.h"
#include "output_mode_helpers.h"

#define DEFAULT_CACHE_CAP 0
#define DEFAULT_MODE 0
/*Default for # of addresses processed is ALL ADDRESSES
  N addresses if nFlag TRUE, otherwise ALL ADDRESSES*/

//modes
#define SUMMARY 0
#define BITMASKS 1
#define VIRTUAL2PHYSICAL 2
#define V2P_TLB_PT 3
#define VPN2PFN 4
#define OFFSET 5

#define MAXTOTALNUMBITS 28


void getArguments(int argc, char* argv[], int& numAddr, bool& nFlag, int& cacheCap, int& mode, std::vector<int>* levelBits, int& pathIdx);

int main(int argc, char **argv)
{
  if(argc < 2){
    printf("USAGE: %s <file>.tr", argv[0]);
    exit(EXIT_FAILURE);
  }
  int numAddr;
  int cacheCap;
  int mode;
  int pathIdx;
  bool nFlag;

  std::vector<int>* levelBits = new std::vector<int>;
  getArguments(argc, argv, numAddr, nFlag, cacheCap, mode, levelBits, pathIdx);
  printf("numAddr: %d cacheCap: %d mode: %d \n", numAddr, cacheCap, mode);
  printf("trace file: %s nFlag: %d", argv[pathIdx], nFlag);
  for(auto i : *levelBits){
    printf("%d", i);
  }
  delete(levelBits);
  return (0);
}

void getArguments(int argc, char* argv[], int& numAddr, bool& nFlag, int& cacheCap, int& mode, std::vector<int>* levelBits, int& pathIdx){
  /*flags to check presence of option and throw error for more than 1
    of same option*/
  bool oFlag = false;
  bool cFlag = false;
  nFlag = false;  //to determine ALL or n # of addresses

  /*Retrieve optional arguments
    First if statement of each case checks for multiple of same option
    -n and -c options expect integers*/
  int Option;
  while((Option = getopt(argc, argv, "o:n:c:")) != -1){
    switch(Option){
      case 'o':
        if(oFlag){
          printf("ERROR: Multiple -o options\n");
          exit(EXIT_FAILURE);
        }
        //select mode
        else if(optarg == "summary"){
          mode = SUMMARY;
        }
        else if(optarg == "bitmasks"){
          mode = BITMASKS;
        }
        else if(optarg == "virtual2physical"){
          mode = VIRTUAL2PHYSICAL;
        }
        else if(optarg == "v2p_tlb_pt"){
          mode = V2P_TLB_PT;
        }
        else if(optarg == "vpn2pfn"){
          mode = VPN2PFN;
        }
        else if(optarg == "offset"){
          mode = OFFSET;
        }
        else{
          printf("ERROR: Unknown Mode Choice\n");
          exit(EXIT_FAILURE);
        }
        oFlag = true;
        break;
      case 'n':
        if(nFlag){
          printf("ERROR: Multiple -p options\n");
          exit(EXIT_FAILURE);
        }
        else if(!isdigit(*optarg)){
          printf("ERROR: -p expects integer\n");
          exit(EXIT_FAILURE);
        }
        else if(optarg < 0){
          printf("ERROR: -o argument must be >= 0");
          exit(EXIT_FAILURE);
        }
        else{
          numAddr = atoi(optarg);
        }
        nFlag = true;
        break;
      case 'c':
        if(cFlag){
          printf("ERROR: Multiple -c options\n");
          exit(EXIT_FAILURE);
        }
        else if(!isdigit(*optarg)){
          printf("ERROR: -c expects integer\n");
          exit(EXIT_FAILURE);
        }
        else if(atoi(optarg) < 0){
          printf("ERROR: -c argument must >= 0");
          exit(EXIT_FAILURE);
        }
        else{
          cacheCap = atoi(optarg);
        }
        cFlag = true;
        break;
      case '?':
        printf("ERROR: Unknown argument");
        exit(EXIT_FAILURE);
      default:
        printf("ERROR: Default");
        exit(EXIT_FAILURE);
    }
  }

  //set defaults if necessary
  if(!oFlag){
    mode = DEFAULT_MODE;
  }
  if(!cFlag){
    cacheCap = DEFAULT_CACHE_CAP;
  }

  //Retrieve mandatory arguments
  int idx = optind;
  //need at least two mandatory arguments, file + bits for level 0
  if((idx + 1 >= argc) || (idx >= argc)){
    printf("Usage: %s <file>.tr int...\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  else{
    FILE *ifp;
    //check if valid file
    if((ifp = fopen(argv[idx], "rb")) == NULL){
      printf("Unable to open <<%s>>\n", argv[idx]);
      fclose(ifp);
      exit(EXIT_FAILURE);
    }
    fclose(ifp);
    //index for trace file in argv
    pathIdx = idx;

    //push back level bits into vector
    int levelBitsSum = 0;
    int currLevel = 0;
    for(int i = idx + 1; i < argc; i++){
      if(atoi(argv[i]) < 1){
        printf("Level %d page table must be at least 1 bit\n", currLevel);
        exit(EXIT_FAILURE);
      }
      levelBitsSum += atoi(argv[i]);
      levelBits->push_back(atoi(argv[i]));
      currLevel++;
    }
    //check if sum of bits does not exceed specified value
    if(levelBitsSum > MAXTOTALNUMBITS){
      printf("Too many bits used in page tables");
      exit(EXIT_FAILURE);
    }
  }

  

}
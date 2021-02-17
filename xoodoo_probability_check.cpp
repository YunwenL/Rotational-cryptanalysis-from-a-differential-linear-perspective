#include<iostream>
#include<cstdlib>
#include<cmath>
#include<cstdint>
#include<random>
#define ROL(x,t) ((x<<t)|x>>(32-t))
using namespace std;
const uint32_t rc[12] = {0x00000058,0x00000038,0x000003C0,0x000000D0,0x00000120,0x00000014,0x00000060,0x0000002C,0x00000380,0x000000F0,0x000001A0,0x00000012};


//random number gen
uint32_t p = 0x7fffffff;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0,p-1);

bool maskdot(uint32_t mask, uint32_t value){
  bool tmp = 0;
  for(int i=0; i<32; i++){
    if(((mask>>i)&0x1) == 1) tmp ^= ((value>>i)&0x1);
  }
  return tmp;
}

void xoodoo_permu(uint32_t input[][3], uint32_t output[][3], int nr){ //run xoodoo permutation
for(int r=0; r<nr; r++){
  //1:theta
  uint32_t column_parity[4] = {0};
  for(int x = 0; x<=3; x++){
    column_parity[x] = input[x][0] ^ input[x][1] ^ input[x][2];
  }
  uint32_t tmp[4] = {0};
  for(int x=0; x<=3; x++){
    tmp[x] = (ROL(column_parity[(x+3)%4],5) ^ ROL(column_parity[(x+3)%4],14));
    for(int y=0; y<=2; y++){
      input[x][y] ^= tmp[x];
    }
  }
  //2:rho west
    uint32_t lane1[4] = {0};
    uint32_t lane2[4] = {0};
    for(int x=0; x<=3; x++){
      lane1[x] = input[(x+3)%4][1];
      lane2[x] = ROL(input[x][2],11);
    }
    for(int x=0; x<=3; x++){
      input[x][1] = lane1[x];
      input[x][2] = lane2[x];
    }

  //3:iota constant addition
  input[0][0] ^= rc[r+12-nr];

  //4:chi
  uint32_t chi[4][3] = {0};
  for(int x=0; x<=3; x++){
    for(int y=0; y<=2; y++){
      chi[x][y] = input[x][y] ^ ((~input[x][(y+1)%3])&(input[x][(y+2)%3]));
    }
  }
  for(int x=0; x<=3; x++){
    for(int y=0; y<=2; y++){
      input[x][y] = chi[x][y];
    }
  }

  //rho east
    for(int x=0; x<=3; x++){
    output[x][1] = ROL(input[x][1],1);
    output[x][2] = ROL(input[(x+2)%4][2],8);
    output[x][0] = input[x][0];}

    for(int x=0; x<=3; x++){
    input[x][1] = output[x][1];
    input[x][2] = output[x][2];
    input[x][0] = output[x][0];}
}
}//end of function xoodoo_permu

double RXDL(uint32_t indiff[][3], uint32_t outmask[][3], int nr){//compute the correlation for given diff and mask
  uint32_t state[4][3] = {0};
  uint32_t statep[4][3] = {0};
  long counter = 0;
  uint64_t TEST_NUM = 0xffffffff;
  double cor = 0;
  for(int test=0; test<=TEST_NUM; test++){
    for(int x=0; x<=3; x++){
    for(int y=0; y<=2; y++){
      state[x][y] = (dis(gen));  //generate random 32-bit string
      statep[x][y] = (ROL(state[x][y],1) ^ indiff[x][y]);
    }}
  uint32_t output[4][3] = {0};
  uint32_t outputp[4][3] = {0};
  xoodoo_permu(state,output,nr);
  xoodoo_permu(statep,outputp,nr);
  bool linear_approx = 0;
  for(int x=0; x<=3; x++){
  for(int y=0; y<=2; y++){
      linear_approx ^= maskdot(outmask[x][y],(ROL(output[x][y],1)^outputp[x][y]));
  }}
  if(linear_approx == 0){counter++;}
  else{counter--;}
  }
  cor = log2(abs(counter))-log2(TEST_NUM+1);
  return cor;
}

void RXDL_full(uint32_t indiff[][3], int nr){//compute the correlation for given diff and all-one-bit mask
  uint32_t state[4][3] = {0};
  uint32_t statep[4][3] = {0};
  long counter[4][3][32] = {0};  //store counter for each one-bit mask
  long TEST_NUM = 0xffff;
  double cor = 0;
  for(int test=0; test<=TEST_NUM; test++){
    for(int x=0; x<=3; x++){
    for(int y=0; y<=2; y++){
      state[x][y] = (dis(gen));  //generate random 32-bit string
      statep[x][y] = (ROL(state[x][y],1)^ indiff[x][y]);
    }}
  uint32_t output[4][3] = {0};
  uint32_t outputp[4][3] = {0};
  xoodoo_permu(state,output,nr);
  xoodoo_permu(statep,outputp,nr);
  for(int x=0; x<=3; x++){
  for(int y=0; y<=2; y++){
  for(int z=0; z<=31; z++){
      bool linear_approx = 0;
      linear_approx = ((ROL(output[x][y],1)^outputp[x][y])>>z)&0x1;
      if(linear_approx == 0){counter[x][y][z]++;}
      else{counter[x][y][z]--;}
  }}}
  }
  cout << "Correlation after round  " << nr << " = 2 ^ "<<endl;
  for(int x=0; x<=3; x++){
  for(int y=0; y<=2; y++){cout << x << " " << y << "  ";
  for(int z=0; z<=31; z++){
  cout << log2(abs(counter[x][y][z]))-log2(TEST_NUM+1) << " ";
  }cout <<endl;}}
}

int main(){
int rd = 4;
int rot = 1;
int mode = 2;  //0 - diff, 1 - RX, 2 - RXDL, 3 - DL
cout << "Round = " << rd << endl;
cout << "rotation =  " << rot << endl;
cout << "Mode = " << mode << endl;

uint32_t a[4][3] = {0};  //state
uint32_t ap[4][3] = {0}; //state with a diff

uint32_t indiff[4][3] = {0};
uint32_t outdiff[4][3] = {0};
uint32_t outmask[4][3] = {0};



outmask[0][0] = 0x8040000; outmask[0][1] = 0x80000000; 
outmask[2][2] = 0x80000100;
outmask[3][2] = 0x4020000;

//4 round.
indiff[0][0] = 0x484ccc80;
indiff[0][1] = 0x484cc800;
indiff[0][2] = 0x484cc800;
indiff[1][0] = 0x3ab9821a;
indiff[1][1] = 0x3ab9821a;
indiff[1][2] = 0x3ab9821a;
indiff[2][0] = 0x37b6cde9;
indiff[2][1] = 0x37b6cde9;
indiff[2][2] = 0x37b6cde9;
indiff[3][0] = 0x45a3f0cb;
indiff[3][1] = 0x45a3f0cb;
indiff[3][2] = 0x45a3f0cb;

cout << "correlation = 2^ " << RXDL(indiff,outmask,rd) << endl;
return 0;
}

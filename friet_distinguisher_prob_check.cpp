//Verify the probability/correlation of RX/DC/RXDL
//need to modify:
//1. the input and output diff/mask
//2. change the variable "shift = ??"
//3, ./a.out [NumRD] [MODE]
//MODE = 0-RX/1-DC/2-RXDL
#include<iostream>
#include<cstdlib>
#include<cstdint>
#include<cmath>
#include<random>
using namespace std;

//num < 64
uint64_t MSB_ROL(uint64_t left, uint64_t right, int num){
  return ((left<<num)|(right>>(64-num)));
}

uint64_t LSB_ROL(uint64_t left, uint64_t right, int num){
  return ((right<<num)|(left>>(64-num)));
}

uint64_t MSB_ROR(uint64_t left, uint64_t right, int num){
  return ((left>>num)|(right<<(64-num)));
}

uint64_t LSB_ROR(uint64_t left, uint64_t right, int num){
  return ((right>>num)|(left<<(64-num)));
}

bool maskdot(uint64_t mask, uint64_t value){
  bool tmp = 0;
  for(int i=0; i<64; i++){
    if(((mask>>i)&0x1) == 1) tmp ^= ((value>>i)&0x1);
  }
  return tmp;
}

bool compute_linear(uint64_t mask0, uint64_t mask1, uint64_t mask2, uint64_t mask3, uint64_t mask4, uint64_t mask5, uint64_t value0, uint64_t value1, uint64_t value2, uint64_t value3, uint64_t value4, uint64_t value5){
  bool tmp = 0;
  tmp ^= maskdot(mask0,value0);
  tmp ^= maskdot(mask1,value1);
  tmp ^= maskdot(mask2,value2);
  tmp ^= maskdot(mask3,value3);
  tmp ^= maskdot(mask4,value4);
  tmp ^= maskdot(mask5,value5);
  return tmp;
}

uint64_t rc[24][2] = {
0x0000000000000000,0x0000000000001111,
0x0000000000000000,0x0000000011100000,
0x0000000000000000,0x0000000000001101,
0x0000000000000000,0x0000000010100000,
0x0000000000000000,0x0000000000000101,
0x0000000000000000,0x0000000010110000,
0x0000000000000000,0x0000000000000110,
0x0000000000000000,0x0000000011000000,
0x0000000000000000,0x0000000000001001,
0x0000000000000000,0x0000000000100000,
0x0000000000000000,0x0000000000000100,
0x0000000000000000,0x0000000010000000,
0x0000000000000000,0x0000000000000001,
0x0000000000000000,0x0000000000110000,
0x0000000000000000,0x0000000000000111,
0x0000000000000000,0x0000000011110000,
0x0000000000000000,0x0000000000001110,
0x0000000000000000,0x0000000011010000,
0x0000000000000000,0x0000000000001010,
0x0000000000000000,0x0000000001010000,
0x0000000000000000,0x0000000000001011,
0x0000000000000000,0x0000000001100000,
0x0000000000000000,0x0000000000001100,
0x0000000000000000,0x0000000010010000};

//initialise the input differences and output masks.
//You DON'T need change the value here, value pop-in below ...
uint64_t aout_diff[2] = {0x0000000000000000,0x0000000000000000};
uint64_t bout_diff[2] = {0x0000000000000000,0x0000000000000000};
uint64_t cout_diff[2] = {0x0000000000000000,0x0000000000000000};

uint64_t ain_diff[2] = {0x0000000000000000,0x0000000000000000};
uint64_t bin_diff[2] = {0x0000000000000000,0x0000000000000000};
uint64_t cin_diff[2] = {0x0000000000000000,0x0000000000000000};

uint64_t aout_mask[2] = {0x0000000000000000,0x0000000000000000};
uint64_t bout_mask[2] = {0x0000000000000000,0x0000000000000000};
uint64_t cout_mask[2] = {0x0000000000000000,0x0000000000000000}; // 4-round RX-truncated differential, branch = 2 --- diff_active at = 8 --- maxmum index = 54 --- max_c_overall = 0.000000. Very strange, this cannot be varified by this program, we get correlation = 2^-2.414

//////////////////////////////////////////////
////////////-----MAIN----/////////////////////
//////////////////////////////////////////////
int main(int argc, char** argv){
long long int counter = 0;
long long int counter_total_data = 0;
uint32_t p = 0x7fffffff;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0,p-1);

uint64_t a[2] = {0}; // a[0] = MSB, a[1] = LSB
uint64_t b[2] = {0};
uint64_t c[2] = {0};
uint64_t ap[2] = {0}; // a[0] = MSB, a[1] = LSB
uint64_t bp[2] = {0};
uint64_t cp[2] = {0};

//////////////////////////////////////////////
//////////////SET     PARAMETER///////////////
//////////////////////////////////////////////
int Nround = atoi(argv[1]); // number of round
int mode = atoi(argv[2]); // 0 = RX, 1 = DC, 2 = RXDL, 3 = DL
int shift = 4;   //RX shift ammount 1--127
uint64_t NUM_of_TEST = 0xffffffffff; // number of total tests.

if(mode == 0) cout << "TEST DP of characteristic: number of round = " << Nround << ", mode = RX " << endl;
else if(mode == 1) cout << "TEST DP of characteristic: number of round = " << Nround << ", mode = DC " << endl;
else if((mode == 2)|(mode==3)){
  if(mode==2){cout << "TEST bias of RXDL: number of round = " << Nround << ", mode = RXDL, " << "SHIFT = " << shift << endl;}
  else{cout << "TEST bias of DL: number of round = " << Nround << ", mode = DL, " << "SHIFT = " << shift << endl;}
//initialise the difference and mask
//COMMENT if you want to test one specific RXDL
/*int branch;       //input branch and nonzero bit position
int act_pos;      //branch 0 = a, 1 = b, 2 = c, -1 = all zero input diff. act_pos: 127, 126, ..., 1, 0
int mask_branch;
int mask_index;   //nonzero bit in the output mask
cout << "input the branch: -1/0/1/2" << endl;
cin >> branch;
cout << "active position: 127 -- 0" << endl;
cin >> act_pos;
cout << "input the mask branch: 0/1/2" << endl;
cin >> mask_branch;
cout << "mask nonzero bit at" << endl;
cin >> mask_index;
if(branch == 0){
  if(act_pos<64){ain_diff[1] = (0x1)<<act_pos;}
  else{ain_diff[0] = (0x1)<<(act_pos-64);}
}
if(branch == 1){
  if(act_pos<64){bin_diff[1] = (0x1)<<act_pos;}
  else{bin_diff[0] = (0x1)<<(act_pos-64);}
}
if(branch == 2){
  if(act_pos<64){cin_diff[1] = (0x1)<<act_pos;}
  else{cin_diff[0] = (0x1)<<(act_pos-64);}
}
if(mask_branch == 0){
  if(mask_index<64){aout_mask[1] = (0x1)<<mask_index;}
  else{aout_mask[0] = (0x1)<<(mask_index-64);}
}
if(mask_branch == 1){
  if(mask_index<64){bout_mask[1] = (0x1)<<mask_index;}
  else{bout_mask[0] = (0x1)<<(mask_index-64);}
}
if(mask_branch == 2){
  if(mask_index<64){cout_mask[1] = (0x1)<<mask_index;}
  else{cout_mask[0] = (0x1)<<(mask_index-64);}
}
cout << hex << ain_diff[0] << " " << ain_diff[1] << endl;
cout << hex << bin_diff[0] << " " << bin_diff[1] << endl;
cout << hex << cin_diff[0] << " " << cin_diff[1] << endl;
cout << hex << aout_mask[0] << " " << aout_mask[1] << endl;
cout << hex << bout_mask[0] << " " << bout_mask[1] << endl;
cout << hex << cout_mask[0] << " " << cout_mask[1] << endl;*/
//COMMENT if you want to test one specific RXDL ------ END comment
}

//test one specific RXDL, when use this chunk of code, COMMENT the above
ain_diff[0] = 0; ain_diff[1] = 0;
bin_diff[0] = 0; bin_diff[1] = 0;
cin_diff[0] = 0; cin_diff[1] = 0x10000;
aout_mask[0] = 0x0000000000000003; aout_mask[1] = 0x0000000000060000;
bout_mask[0] = 0x0000600000000001; bout_mask[1] = 0x0000000030020000;
cout_mask[0] = 0x600000000000c001; cout_mask[1] = 0x0000000000030000;

cout << hex << ain_diff[0] << " " << ain_diff[1] << endl;
cout << hex << bin_diff[0] << " " << bin_diff[1] << endl;
cout << hex << cin_diff[0] << " " << cin_diff[1] << endl;
cout << hex << aout_mask[0] << " " << aout_mask[1] << endl;
cout << hex << bout_mask[0] << " " << bout_mask[1] << endl;
cout << hex << cout_mask[0] << " " << cout_mask[1] << endl;

///////////////////////////////////////////////
/////////////////encryption////////////////////
///////////////////////////////////////////////
for(uint64_t test = 0; test <= NUM_of_TEST; test++){
counter_total_data++;
//represent the 128-bit input as two 64-bit inputs
a[0] = ((dis(gen))+(((uint64_t) dis(gen))<<32)); a[1] = ((dis(gen))+(((uint64_t) dis(gen))<<32)); // a[0] = MSB, a[1] = LSB
b[0] = ((dis(gen))+(((uint64_t) dis(gen))<<32)), b[1] = ((dis(gen))+(((uint64_t) dis(gen))<<32));
c[0] = ((dis(gen))+(((uint64_t) dis(gen))<<32)), c[1] = ((dis(gen))+(((uint64_t) dis(gen))<<32));

if(mode==0){  //mode = 0 ---> RX
ap[0] = MSB_ROL(a[0],a[1],shift) ^ ain_diff[0];
ap[1] = LSB_ROL(a[0],a[1],shift) ^ ain_diff[1];
bp[0] = MSB_ROL(b[0],b[1],shift) ^ bin_diff[0];
bp[1] = LSB_ROL(b[0],b[1],shift) ^ bin_diff[1];
cp[0] = MSB_ROL(c[0],c[1],shift) ^ cin_diff[0];
cp[1] = LSB_ROL(c[0],c[1],shift) ^ cin_diff[1];}

else if(mode==1){
ap[0] = a[0] ^ ain_diff[0];
ap[1] = a[1] ^ ain_diff[1];
bp[0] = b[0] ^ bin_diff[0];
bp[1] = b[1] ^ bin_diff[1];
cp[0] = c[0] ^ cin_diff[0];
cp[1] = c[1] ^ cin_diff[1];}

else if(mode==2){  //mode = 2 ---> RXDL
ap[0] = MSB_ROL(a[0],a[1],shift) ^ ain_diff[0];
ap[1] = LSB_ROL(a[0],a[1],shift) ^ ain_diff[1];
bp[0] = MSB_ROL(b[0],b[1],shift) ^ bin_diff[0];
bp[1] = LSB_ROL(b[0],b[1],shift) ^ bin_diff[1];
cp[0] = MSB_ROL(c[0],c[1],shift) ^ cin_diff[0];
cp[1] = LSB_ROL(c[0],c[1],shift) ^ cin_diff[1];}

else if(mode==3){  //mode == 3 --->DL
  ap[0] = a[0] ^ ain_diff[0];
  ap[1] = a[1] ^ ain_diff[1];
  bp[0] = b[0] ^ bin_diff[0];
  bp[1] = b[1] ^ bin_diff[1];
  cp[0] = c[0] ^ cin_diff[0];
  cp[1] = c[1] ^ cin_diff[1];}

else{cout << "INPUT the MODE" << endl;}
//round transformation
for(int r = 0; r < Nround; r++){
c[0] ^= rc[r][0];
c[1] ^= rc[r][1];

uint64_t e[2] = {0};
uint64_t f[2] = {0};
uint64_t g[2] = {0};

e[0] = a[0] ^ b[0] ^ c[0];
e[1] = a[1] ^ b[1] ^ c[1];
//g = a ^ c>>>48 ^ a>>>47
g[0] = a[0] ^ MSB_ROR(c[0],c[1],48) ^ MSB_ROR(a[0],a[1],47);
g[1] = a[1] ^ LSB_ROR(c[0],c[1],48) ^ LSB_ROR(a[0],a[1],47);
//f = c ^ a<<<1 ^ e ^ g
f[0] = c[0] ^ MSB_ROL(a[0],a[1],1) ^ e[0] ^ g[0];
f[1] = c[1] ^ LSB_ROL(a[0],a[1],1) ^ e[1] ^ g[1];

b[0] = f[0]; b[1] = f[1];
c[0] = g[0]; c[1] = g[1];

//a = e ^ (f<<<36)&(g>>>61)
a[0] = e[0] ^ ((MSB_ROL(f[0],f[1],36))&(MSB_ROR(g[0],g[1],61)));
a[1] = e[1] ^ ((LSB_ROL(f[0],f[1],36))&(LSB_ROR(g[0],g[1],61)));

}//end for encryption

for(int r = 0; r < Nround; r++){
cp[0] ^= rc[r][0];
cp[1] ^= rc[r][1];

uint64_t ep[2] = {0};
uint64_t fp[2] = {0};
uint64_t gp[2] = {0};

ep[0] = ap[0] ^ bp[0] ^ cp[0];
ep[1] = ap[1] ^ bp[1] ^ cp[1];
//g = a ^ c>>>48 ^ a>>>47
gp[0] = ap[0] ^ MSB_ROR(cp[0],cp[1],48) ^ MSB_ROR(ap[0],ap[1],47);
gp[1] = ap[1] ^ LSB_ROR(cp[0],cp[1],48) ^ LSB_ROR(ap[0],ap[1],47);
//f = c ^ a<<<1 ^ e ^ g
fp[0] = cp[0] ^ MSB_ROL(ap[0],ap[1],1) ^ ep[0] ^ gp[0];
fp[1] = cp[1] ^ LSB_ROL(ap[0],ap[1],1) ^ ep[1] ^ gp[1];

bp[0] = fp[0]; bp[1] = fp[1];
cp[0] = gp[0]; cp[1] = gp[1];

//a = e ^ (f<<<36)&(g>>>61)
ap[0] = ep[0] ^ ((MSB_ROL(fp[0],fp[1],36))&(MSB_ROR(gp[0],gp[1],61)));
ap[1] = ep[1] ^ ((LSB_ROL(fp[0],fp[1],36))&(LSB_ROR(gp[0],gp[1],61)));

}//end of encryption prime

uint64_t tmp0 = 0, tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0, tmp5 = 0;

if(mode==0){   //mode = 0 ---> RX
tmp0 = ap[0] ^ MSB_ROL(a[0],a[1],shift) ^ aout_diff[0];
tmp1 = ap[1] ^ LSB_ROL(a[0],a[1],shift) ^ aout_diff[1];
tmp2 = bp[0] ^ MSB_ROL(b[0],b[1],shift) ^ bout_diff[0];
tmp3 = bp[1] ^ LSB_ROL(b[0],b[1],shift) ^ bout_diff[1];
tmp4 = cp[0] ^ MSB_ROL(c[0],c[1],shift) ^ cout_diff[0];
tmp5 = cp[1] ^ LSB_ROL(c[0],c[1],shift) ^ cout_diff[1];

if((tmp0|tmp1|tmp2|tmp3|tmp4|tmp5)==0){counter++;}
}

else if(mode==1){
tmp0 = ap[0] ^ a[0] ^ aout_diff[0];
tmp1 = ap[1] ^ a[1] ^ aout_diff[1];
tmp2 = bp[0] ^ b[0] ^ bout_diff[0];
tmp3 = bp[1] ^ b[1] ^ bout_diff[1];
tmp4 = cp[0] ^ c[0] ^ cout_diff[0];
tmp5 = cp[1] ^ c[1] ^ cout_diff[1];

if((tmp0|tmp1|tmp2|tmp3|tmp4|tmp5)==0){counter++;}
}

else if(mode==2){  //check output imbalance
tmp0 = ap[0] ^ MSB_ROL(a[0],a[1],shift);
tmp1 = ap[1] ^ LSB_ROL(a[0],a[1],shift);
tmp2 = bp[0] ^ MSB_ROL(b[0],b[1],shift);
tmp3 = bp[1] ^ LSB_ROL(b[0],b[1],shift);
tmp4 = cp[0] ^ MSB_ROL(c[0],c[1],shift);
tmp5 = cp[1] ^ LSB_ROL(c[0],c[1],shift);
if(compute_linear(aout_mask[0],aout_mask[1],bout_mask[0],bout_mask[1],cout_mask[0],cout_mask[1],tmp0,tmp1,tmp2,tmp3,tmp4,tmp5)==0){counter++;}
else {counter--;}
}

else if(mode==3){  //DL
  tmp0 = ap[0] ^ a[0] ^ aout_diff[0];
  tmp1 = ap[1] ^ a[1] ^ aout_diff[1];
  tmp2 = bp[0] ^ b[0] ^ bout_diff[0];
  tmp3 = bp[1] ^ b[1] ^ bout_diff[1];
  tmp4 = cp[0] ^ c[0] ^ cout_diff[0];
  tmp5 = cp[1] ^ c[1] ^ cout_diff[1];
  if(compute_linear(aout_mask[0],aout_mask[1],bout_mask[0],bout_mask[1],cout_mask[0],cout_mask[1],tmp0,tmp1,tmp2,tmp3,tmp4,tmp5)==0){counter++;}
  else {counter--;}
}


}
cout << "Plaintext ammount:  2^ " << (log2(counter_total_data)) << endl;
cout << "correlation weight = " << -(log2(abs(counter))-log2(counter_total_data)) << endl;




return 0;
}

//alzette_rxdl function == theoretical correlation
//alzette_rxdl_exper function == experimental correlation
//left rotate by ONE bit

#include<iostream>
#include<cstdlib>
#include<cstdint>
#include<cmath>
#include<random>
#define XOR(p1,p2) (p1+p2-2*p1*p2)
#define AND(p1,p2) ((p1+p2-p1*p2)*0.5)
#define mix(p1,p2,p3) (p1*p2*p3-0.5*(p1*p2+p1*p3+p2*p3)+0.5*(p1+p2+p3))
#define ROR(x,t) ((x>>t)|(x<<(32-t)))
#define ROL(x,t) ((x<<t)|(x>>(32-t)))
using namespace std;
//////////////////////////////////////////////// random number generator
uint32_t p = 0x7fffffff;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0,p-1);
////////////////////////////////////////////////

bool maskdot(int mask, int value){
  bool tmp = 0;
  for(int i=0; i<=31; i++){
    if(((mask>>i)&0x1) == 1) tmp ^= ((value>>i)&0x1);
  }
  return tmp;
}

void modadd_rxdl(double *a, double *b, double *z, int wordsize){ //boxplus rule with RXDL left rot by 1 bit
  double c[32] = {0};
  c[0] = 0.5;
  z[0] = XOR(XOR(a[0],b[0]),c[0]);
  c[1] = 0.25;
  z[1] = XOR(XOR(a[1],b[1]),c[1]);
  for(int index = 2; index<= (wordsize-1); index++){
    c[index] = mix(a[index-1],b[index-1],c[index-1]);
    z[index] = XOR(XOR(a[index],b[index]),c[index]);
  }
}

void print(double *v){     //print correlation
	for(int j=0; j<=3; j++){
	for(int i=0; i<=7; i++){
	  cout << log2(abs(2*v[8*j+i]-1)) << "   ";
	}cout << endl;}
cout << endl;
}

void Shift_modadd_rxdl(double *a, double *b, double *z, int wordsize, int shift){  //tailored for alzette, one of the operand to modadd is right rotated by Shift.
  double b_shift[32] = {0};
  for(int i=0; i<=31; i++){
    b_shift[i] = b[(i+shift)%wordsize];    //right rotate b by shift bits
  }
  modadd_rxdl(a,b_shift,z,32);
}

void alzette_rxdl(double *a, double *b, uint32_t c){  //32-bit modadd, rxdl with left rot 1 bit, constant c.
  uint32_t c_diff = c ^ ROL(c,1);//RX-diff by constant c
  cout << "c_diff = " << hex << c_diff <<endl;
  bool flag[32] = {0};
    for(int i=0; i<=31; i++){
      if((c_diff&(0x1<<i))>1) flag[i] = 1;
    }
  double z1[32] = {0};
  double w1[32] = {0};
  Shift_modadd_rxdl(a,b,z1,32,31);
    for(int i=0; i<=31; i++){
    w1[i] = XOR(b[i],z1[(i+24)%32]);
  }
  for(int i =0; i<=31; i++){  //constant addition
     if(flag[i] == 1){
       z1[i] = 1-z1[i];
     }
  }
  for(int i =0; i<=31; i++){
       a[i] = z1[i];
       b[i] = w1[i];
  }
  double z2[32] = {0};
  double w2[32] = {0};
  Shift_modadd_rxdl(z1,w1,z2,32,17);
  for(int i=0; i<=31; i++){
    w2[i] = XOR(w1[i],z2[(i+17)%32]);
  }
   for(int i =0; i<=31; i++){  //constant addition
     if(flag[i] == 1){
       z2[i] = 1-z2[i];
     }
  }
  double z3[32] = {0};
  double w3[32] = {0};
  modadd_rxdl(z2,w2,z3,32);
  for(int i=0; i<=31; i++){
    w3[i] = XOR(w2[i],z3[(i+31)%32]);
  }
  for(int i =0; i<=31; i++){  //constant addition
     if(flag[i] == 1){
       z3[i] = 1-z3[i];
     }
  }
  Shift_modadd_rxdl(z3,w3,a,32,24);
  for(int i=0; i<=31; i++){
    b[i] = XOR(w3[i],a[(i+16)%32]);
  }
   for(int i =0; i<=31; i++){  //constant addition
     if(flag[i] == 1){
       a[i] = 1-a[i];
     }
  }
}

void alzette_rxdl_exper(uint32_t diff1, uint32_t diff2, uint32_t c){ //experiment verification RXDL with left rot 1 bit
  int counter[64] = {0};                     //initialise counter, a[0..31] = counter[0..31], b[0..31] = counter[32..63]
  for(int i = 0; i<=0x3fff; i++){
  for(int j = 0; j<=0x3fff; j++){ //PT space
    uint32_t x = (dis(gen))&0xffffffff; uint32_t y = (dis(gen))&0xffffffff;
    uint32_t xp = ROL(x,1) ^ diff1; uint32_t yp = ROL(y,1) ^ diff2;    //RX-diff
    //alzette PT
    x +=ROR(y ,31);y ^=ROR(x ,24);x ^=c ;
    x +=ROR(y ,17);y ^=ROR(x ,17);x ^=c ;
    x +=y ;y ^=ROR(x ,31);x ^=c;
    x +=ROR(y ,24);y ^=ROR(x ,16);x ^=c;
    //alzette PT
    xp +=ROR(yp ,31);yp ^=ROR(xp ,24);xp ^=c ;
    xp +=ROR(yp ,17);yp ^=ROR(xp ,17);xp ^=c ;
    xp +=yp ;yp ^=ROR(xp ,31);xp ^=c;
    xp +=ROR(yp ,24);yp ^=ROR(xp ,16);xp ^=c;

    uint32_t delta_x = ROL(x,1) ^ xp;
    uint32_t delta_y = ROL(y,1) ^ yp;   //RX-diff
    for(int index = 0; index <= 63; index++){  //for all one bit output masks
      if(index<=31){
        if(maskdot((0x00000001<<index), delta_x) == 1) counter[index]++;
        //else counter[index]--;
      }
      else{
        if(maskdot((0x00000001<<(index-32)), delta_y) == 1) counter[index]++;
        //else counter[index]--;
      }
    }
  }}
  for(int index=0; index<=63; index++){
    /*cout << "mask bit at position: ";
    if(index<=31){
      cout << "branch a + bit at " << index << endl;
      cout << "Correlation === 2^ " << (log2(abs(counter[index]))-24) << endl;
    }
    else{
      cout << "branch b + bit at " << (index-32) << endl;
      cout << "Correlation === 2^ " << (log2(abs(counter[index]))-24) << endl;
    }*/
      cout << ((double)(abs(counter[index]))/pow(2,28)) << "   ";    //output experiment probability
      //cout << (log2(abs(counter[index]))-24) << "   ";
      if((index+1)%16==0)cout << endl;
  }
}

int main(){
  uint32_t c[8] = {0xB7E15162,0xBF715880,0x38B4DA56,0x324E7738,0xBB1185EB,0x4F7C7B57,0xCFBFA1C8,0xC2B3293D};
  uint32_t diff1 = 0x7ffffffc;
  uint32_t diff2 = 0x3fffffff;
  double a[32] = {0};
  double b[32] = {0};
	cout << "a = ";
	for(int i=0; i<=31; i++){a[i] = (diff1>>i)&0x1;cout << a[i] << "   ";}cout << endl;
	cout << "b = ";
	for(int i=0; i<=31; i++){b[i] = (diff2>>i)&0x1;cout << b[i] << "   ";}cout << endl;

  alzette_rxdl(a,b,c[0]);
  /*cout << "probability by theoretical prediction in binary logrithm:" <<endl;
  cout << "a: " << endl;
  for(int i=0; i<=31; i++){cout << (log2(a[i])) << "   ";if((i+1)%16==0){cout << endl;}}cout << endl;
  cout << "b: " << endl;
  for(int i=0; i<=31; i++){cout << (log2(b[i])) << "   ";if((i+1)%16==0){cout << endl;}}cout << endl;*/
  cout << "probability by theoretical prediction:" <<endl;
  cout << "a: " << endl;
  for(int i=0; i<=31; i++){cout << ((a[i])) << "   ";if((i+1)%16==0){cout << endl;}}cout << endl;
  cout << "b: " << endl;
  for(int i=0; i<=31; i++){cout << ((b[i])) << "   ";if((i+1)%16==0){cout << endl;}}cout << endl;

  //cout << "Probability by experiment in binary logrithm:" <<endl;
  cout << "Probability by experiment:" <<endl;
  alzette_rxdl_exper(diff1,diff2,c[0]);
  return 0;
}

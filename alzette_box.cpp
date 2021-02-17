//alzette_dl function == theoretical correlation
//alzette_dl_exper function == experimental correlation

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

void modadd(double *a, double *b, double *z, int wordsize){
	double c[32] = {0};
	c[0] = 0;
	z[0] = XOR(a[0],b[0]);
		for(int i=1; i<wordsize; i++){
		c[i] = mix(a[i-1],b[i-1],c[i-1]);
		z[i] = XOR(XOR(a[i],b[i]),c[i]);
		}
}

void print(double *v){     //print correlation
	for(int j=0; j<=3; j++){
	for(int i=0; i<=7; i++){
	  cout << log2(abs(2*v[8*j+i]-1)) << "   ";
	}cout << endl;}
cout << endl;
}

void Shift_modadd(double *a, double *b, double *z, int wordsize, int shift){  //tailored for alzette, one of the operand to modadd is right rotated by Shift
  double b_shift[32] = {0};
  for(int i=0; i<=31; i++){
    b_shift[i] = b[(i+shift)%wordsize];    //right rotate b by shift bits
  }
  modadd(a,b_shift,z,32);
}

void alzette_dl(double *a, double *b){  //32-bit modadd
  double z1[32] = {0};
  double w1[32] = {0};
  Shift_modadd(a,b,z1,32,31);
    for(int i=0; i<=31; i++){
    w1[i] = XOR(b[i],z1[(i+24)%32]);
  }
  double z2[32] = {0};
  double w2[32] = {0};
  Shift_modadd(z1,w1,z2,32,17);
  for(int i=0; i<=31; i++){
    w2[i] = XOR(w1[i],z2[(i+17)%32]);
  }
  double z3[32] = {0};
  double w3[32] = {0};
  modadd(z2,w2,z3,32);
  for(int i=0; i<=31; i++){
    w3[i] = XOR(w2[i],z3[(i+31)%32]);
  }
  Shift_modadd(z3,w3,a,32,24);
  for(int i=0; i<=31; i++){
    b[i] = XOR(w3[i],a[(i+16)%32]);
  }
}

void alzette_dl_exper(uint32_t diff1, uint32_t diff2, uint32_t c){
  int counter[64] = {0};                     //initialise counter, a[0..31] = counter[0..31], b[0..31] = counter[32..63]
  for(int i = 0; i<=0xfff; i++){
  for(int j = 0; j<=0xfff; j++){ //PT space = 24-dim
    uint32_t x = (ROL((dis(gen))&0xffffffff,2) ^ ((dis(gen))&0xffffffff)); uint32_t y = (ROL((dis(gen))&0xffffffff,2) ^ ((dis(gen))&0xffffffff));  //the random number generater only has 31 bits, so we use this method to get 32-bit random numbers
    uint32_t xp = x ^ diff1; uint32_t yp = y ^ diff2;
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

    uint32_t delta_x = x ^ xp;
    uint32_t delta_y = y ^ yp;
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
  cout << "DL-Probability by experiment : " <<endl;
  for(int index=0; index<=63; index++){
      cout << (abs(counter[index])/pow(2,24)) << "   ";
            if((index+1)%16==0)cout << endl;
  }
}

int main(){
  uint32_t c[8] = {0xB7E15162,0xBF715880,0x38B4DA56,0x324E7738,0xBB1185EB,0x4F7C7B57,0xCFBFA1C8,0xC2B3293D};
  for(int diff_pos = 0; diff_pos<=31; diff_pos++){
  uint32_t diff1 = 0x0;
  uint32_t diff2 = 0x1 << diff_pos;
  double a[32] = {0};
  double b[32] = {0};
	cout << "a = ";
	for(int i=0; i<=31; i++){a[i] = (diff1>>i)&0x1;cout << a[i] << "   ";}cout << endl;
	cout << "b = ";
	for(int i=0; i<=31; i++){b[i] = (diff2>>i)&0x1;cout << b[i] << "   ";}cout << endl;

  alzette_dl(a,b);
  cout << "DL-probability by theoretical prediction:  " <<endl;
  cout << "a: " << endl;
  for(int i=0; i<=31; i++){cout << a[i] << "   ";if((i+1)%16==0){cout << endl;}}cout << endl;
  cout << "b: " << endl;
  for(int i=0; i<=31; i++){cout << b[i] << "   ";if((i+1)%16==0){cout << endl;}}cout << endl;
  cout << "Correlation (1-2*p) by theoretical prediction:  2^" <<endl;
  cout << "a: " << endl;
  for(int i=0; i<=31; i++){cout << log2(abs(1-2*a[i])) << "   ";if((i+1)%16==0){cout << endl;}}cout << endl;
  cout << "b: " << endl;
  for(int i=0; i<=31; i++){cout << log2(abs(1-2*b[i])) << "   ";if((i+1)%16==0){cout << endl;}}cout << endl;

  alzette_dl_exper(diff1,diff2,c[0]);

  }
  return 0;
}

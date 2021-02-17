// compute siphash DL probability
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
const int state_size = 64;
bool maskdot(uint64_t mask, uint64_t value){
  bool tmp = 0;
  for(int i=0; i<state_size; i++){
    if(((mask>>i)&0x1) == 1) tmp ^= ((value>>i)&0x1);
  }
  return tmp;
}

void modadd(double *a, double *b, double *z){
	double c[state_size] = {0};
	c[0] = 0;
	z[0] = XOR(a[0],b[0]);
		for(int i=1; i<state_size; i++){
		c[i] = mix(a[i-1],b[i-1],c[i-1]);
		z[i] = XOR(XOR(a[i],b[i]),c[i]);
		}
}

void Shift_modadd(double *a, double *b, double *z, int shift){  //tailored for alzette, one of the operand to modadd is right rotated by Shift
  double b_shift[state_size] = {0};
  for(int i=0; i<state_size; i++){
    b_shift[i] = b[(i+shift)%state_size];    //right rotate b by shift bits
  }
  double c[state_size] = {0};
	c[0] = 0;
	z[0] = XOR(a[0],b_shift[0]);
		for(int i=1; i<state_size; i++){
		c[i] = mix(a[i-1],b_shift[i-1],c[i-1]);
		z[i] = XOR(XOR(a[i],b_shift[i]),c[i]);
		}
}

void siphash_dl(double *a, double *b, double *c, double *d){  //siphash one round
  double z1[64] = {0};
  double w1[64] = {0};
  modadd(a,b,z1);
  for(int i=0; i<=63;i++){
    w1[i] = XOR(z1[i],a[(i+51)%64]);    //<<< 13 == >>> 51
  }
  double z2[64] = {0};
  double w2[64] = {0};
  modadd(c,d,z2);
  for(int i=0; i<=63;i++){
    w2[i] = XOR(z2[i],d[(i+48)%64]);    //<<< 16 == >>> 48
  }
  double z3[64] = {0};
  double z4[64] = {0};
  modadd(z2,w1,z3);
  Shift_modadd(w2,z1,z4,32);

  for(int i=0; i<=63;i++){
    a[i] = XOR(z3[i],w1[(i+47)%64]);  //<<<17 == >>>47
    b[i] = z4[i];
    c[i] = z3[(i+32)%64];
    d[i] = XOR(z4[i],w2[(i+43)%64]);  //<<<21 == >>> 43
  }
}

void print(double *v){
	for(int j=0; j<=3; j++){
	for(int i=0; i<=15; i++){
	  cout << log2(abs(v[16*j+i]-0.5)) << "   ";
	}cout << endl;}
cout << endl;
}

int main(){

  double a[64] = {0};
  double b[64] = {0};
  double c[64] = {0};
  double d[64] = {0};
  a[0] = 1;
  cout << "a[0] = 1 " <<endl;
  siphash_dl(a,b,c,d);
  /*cout << "a: " << endl; print(a);
  cout << "b: " << endl; print(b);
  cout << "c: " << endl; print(c);
  cout << "d: " << endl; print(d);*/
  siphash_dl(a,b,c,d);   //second round
  /*cout << "a: " << endl; print(a);
  cout << "b: " << endl; print(b);
  cout << "c: " << endl; print(c);
  cout << "d: " << endl; print(d);*/
  siphash_dl(a,b,c,d);   //third round
  //siphash_dl(a,b,c,d);   //fourth round
  cout << "The imbalance = DL_prob - 1/2 :" <<endl;
  cout << "a: " << endl; print(a);
  cout << "b: " << endl; print(b);
  cout << "c: " << endl; print(c);
  cout << "d: " << endl; print(d);
  return 0;
}

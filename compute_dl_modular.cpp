//test differential-linear probability of modadd
#include<iostream>
#include<cstdlib>
#include<cstdint>
#include<cmath>
#include<random>
#define XOR(p1,p2) (p1+p2-2*p1*p2)
#define AND(p1,p2) ((p1+p2-p1*p2)*0.5)
#define mix(p1,p2,p3) (p1*p2*p3-0.5*(p1*p2+p1*p3+p2*p3)+0.5*(p1+p2+p3)) //mix = xy^yz^xz
using namespace std;
#define WORD uint32_t

//////////////////////////////////////////////// random number generator
uint32_t p = 0x7fffffff;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0,p-1);
////////////////////////////////////////////////

const int mod_size = 32;
bool maskdot(int mask, int value){
  bool tmp = 0;
  for(int i=0; i<mod_size; i++){
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

void print_cor_thm(WORD diff1, WORD diff2){
	double a[mod_size] = {0};
	cout << "a = ";
	for(int i=0; i<mod_size; i++){
	  a[i] = (diff1>>i)&0x1;
	  cout << a[i] << "   ";
	}cout << endl;
	double b[mod_size] = {0};
	cout << "b = ";
	for(int i=0; i<mod_size; i++){
	  b[i] = (diff2>>i)&0x1;
	  cout << b[i] << "   ";
	}cout << endl;                   // if a_i = 1, then a[i] = 1

	double z[mod_size] = {0};
	modadd(a,b,z,mod_size);
	cout << "Prob by bit method" <<endl;
	for(int i=0; i<mod_size; i++){
	  cout << i << "   " << z[i] << endl;
}}

void print_cor_exper(WORD diff1, WORD diff2){
  WORD mask = 0x1;
	for(int shift = 0; shift < mod_size; shift++){
	int counter = 0;
	int counter_carry = 0;
	for(int i = 0; i<=0xff; i++){
	for(int j = 0; j<=0xff; j++){
	/*uint8_t x = i; uint8_t y = j;
	uint8_t xp = x ^ diff1; uint8_t yp = y ^ diff2;
	uint8_t z = x + y;
	uint8_t zp = xp + yp;*/

  uint32_t x = (dis(gen))&0xffffffff; uint32_t y = (dis(gen))&0xffffffff;
  uint32_t xp = x ^ diff1; uint32_t yp = y ^ diff2;
  uint32_t z = x + y;
  uint32_t zp = xp + yp;

	uint32_t temp = 0;
	temp = z ^ zp;
	if(maskdot(mask,temp)==1){
	  counter++;
	}
	}}
	cout << "mask = " << hex << (int)mask << "    ";
	//cout << "Pr[z] = 2 ^ " << (log2(abs(counter))-16) << endl;
  cout << "Pr[z] =" << ((double)counter/65536) << endl;
	mask = mask*2;
}}

int main(){
//modadd size = word bit
bool FULL_CHECK = false; //true: output comparison in THM and EXPER; false: only comput for one given in-out diff
	//NOTE: In comparison, the program outputs some unequal cases, but this might because of the precision, because the values are actually equal.
if(FULL_CHECK==true){cout << "Check formula compatible experiment." << endl;}
else{cout << "Print specific configuration. You need to control the variables diff1 and diff2." << endl;}

if(FULL_CHECK==true){
	uint8_t diff1 = 0x0;
	uint8_t diff2 = 0x0;
	for(int d1 = 0; d1<=0xff; d1++){  //for all diff, check if the formula is correct
	for(int d2 = 0; d2<=0xff; d2++){
	diff1 = d1; diff2 = d2;

	uint8_t mask = 0x1;
	double exper[8] = {0}; //experiment correlation
	double theory[8] = {0};       //theory correlation
	//Compute the probability by experiment
	for(int shift = 0; shift <= 7; shift++){
	int counter = 0;
	int counter_carry = 0;
	for(int i = 0; i<=0xff; i++){
	for(int j = 0; j<=0xff; j++){
	uint8_t x = i; uint8_t y = j;
	uint8_t xp = x ^ diff1; uint8_t yp = y ^ diff2;
	uint8_t z = x + y;
	uint8_t zp = xp + yp;

	uint8_t temp = 0;
	temp = z ^ zp;
	if(maskdot(mask,temp)==1){
	  counter++;
	}
	}}

	exper[shift] = (log2(abs(counter))-16);
	mask = mask*2;
	}

	//compute the probability by boxplus-rule
	double a[8] = {0};
	cout << "a = ";
	for(int i=0; i<=7; i++){
	  a[i] = (diff1>>i)&0x1;
	  cout << a[i] << "   ";
	}cout << "    ";
	double b[8] = {0};
	cout << "b = ";
	for(int i=0; i<=7; i++){
	  b[i] = (diff2>>i)&0x1;
	  cout << b[i] << "   ";
	}cout << endl;

	double z[8] = {0};
	modadd(a,b,z,8);
	for(int i=0; i<=7; i++){
	  theory[i] = (log2(z[i]));
	}

  }*/
  //output all probability
  for(int i=0; i<=7; i++){
    cout << exper[i] << "  ";
  }cout << endl;
  for(int i=0; i<=7; i++){
    cout << theory[i] << "  ";
  }cout << endl;
	}}
}

else{ //only print one case
	WORD diff1 = 0xffffffff;
	WORD diff2 = 0xffffffff;
cout << "Experiment dl-probability:" <<endl;
print_cor_exper(diff1,diff2);
cout << "Thm dl-probability:" <<endl;
print_cor_thm(diff1,diff2);
}
return 0;
}

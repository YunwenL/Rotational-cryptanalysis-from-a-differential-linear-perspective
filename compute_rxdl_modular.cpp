#include<iostream>
#include<cstdlib>
#include<cmath>
#include<time.h>
#define wordsize 32   //size of the modular addition
#define wordlimit 0x32    //2^wordsize -1
#define datasize 8   //size of the PT space for experiment verification
#define dataspace 0xff     //2^datasize -1
#define rol(x,r) (((x<<r)|(x>>(wordsize-r)))&wordlimit)
#define XOR(p1,p2) (p1+p2-2*p1*p2)
#define AND(p1,p2) ((p1+p2-p1*p2)*0.5)
#define mix(p1,p2,p3) (p1*p2*p3-0.5*(p1*p2+p1*p3+p2*p3)+0.5*(p1+p2+p3)) //mix = xy^yz^xz
using namespace std;

int t = 1; //rot(x) = x <<< t

//FUNCTION: Prob_by_exper  ---> compute the experimental probability of a RXDL-distinguisher.
void Prob_by_exper(int indiffleft, int indiffright, int t){  //rot(x) = x <<< t
  long counter[wordsize] = {0};
  //int RXddt[16] = {0};

  for(int i = 0; i<=dataspace; i++){
    for(int j = 0; j<=dataspace; j++){
      //int leftin = i;     //if wordsize is small, use these two lines to run through the full space
      //int rightin = j;
      int leftin = rand()&wordlimit;
      int rightin = rand()&wordlimit;
      int addsum = (leftin + rightin)&dataspace;
      int carry = addsum ^ leftin ^ rightin;
      int leftinp = rol(leftin,t) ^ indiffleft;
      int rightinp = rol(rightin,t) ^ indiffright;
      int addsump = (leftinp + rightinp)&dataspace;
      int carryp = addsump ^ leftinp ^ rightinp;
      int diff = rol(addsum,t) ^ addsump;
      int diff_carry = rol(carry,t) ^ carryp;
        for(int index = 0; index <= (wordsize-1); index++){
           int mask = 0x1 << index;
           if((diff&mask)!=0) {counter[index]++;}
        }
  }}
  //cout << hex << indiffleft << "  " << indiffright << endl;
  cout << "EXP  " ;
  for(int index=0; index<=(wordsize-1); index++){
    cout << (log2(abs(counter[index]))-2*datasize) << " ";
  }cout << endl;
  cout << "EXP bias: " ;
  for(int index=0; index<=(wordsize-1); index++){
    cout << (log2(abs((counter[index]/pow(2,2*datasize)-0.5)))) << " ";
  }cout << endl;
}

void Prob_by_thm(int indiffleft, int indiffright){
  double a[wordsize] = {0};
  double b[wordsize] = {0};
  for(int i=0; i<=(wordsize-1); i++){
    a[i] = (indiffleft>>i)&0x1;
  }
  for(int i=0; i<=(wordsize-1); i++){
    b[i] = (indiffright>>i)&0x1;
  }
  double c[wordsize] = {0};
  double z[wordsize] = {0};
  c[0] = 0.5;
  z[0] = XOR(XOR(a[0],b[0]),c[0]);
  c[1] = 0.25;
  z[1] = XOR(XOR(a[1],b[1]),c[1]);
  for(int index = 2; index<=(wordsize-1); index++){
    c[index] = mix(a[index-1],b[index-1],c[index-1]);
    z[index] = XOR(XOR(a[index],b[index]),c[index]);
  }

  //cout << "THM  " << log2(z[0]) << " " << log2(z[1]) << " " << log2(z[2]) << " " << log2(z[3]) << " " << log2(z[4])<< " " << log2(z[5])<< " " << log2(z[6])<< " " << log2(z[7]) << endl;
  cout << "THM  " ;
  for(int index=0; index<=(wordsize-1); index++){
    cout << (log2(z[index])) << " ";
  }cout << endl;
  /*cout << "THM probability " ;
  for(int index=0; index<=(wordsize-1); index++){
    cout << z[index] << " ";
  }cout << endl;*/
  cout << "THM bias" ;
  for(int index=0; index<=(wordsize-1); index++){
    cout << (log2(abs(z[index]-0.5))) << " ";
  }cout << endl;
}

void Prob_by_thm_t(int indiffleft, int indiffright, int t){
  double a[wordsize] = {0};
  double b[wordsize] = {0};
  for(int i=0; i<=(wordsize-1); i++){
    a[i] = (indiffleft>>i)&0x1;
  }
  for(int i=0; i<=(wordsize-1); i++){
    b[i] = (indiffright>>i)&0x1;
  }
  double c[wordsize] = {0};
  double z[wordsize] = {0};
  for(int i = 0; i<=wordsize-1; i++){
    if(i == 0){
      c[i] = 0.5;
      z[i] = XOR(XOR(a[i],b[i]),c[i]);
    }
    else if(i == t){
      c[i] = 0.5;
      z[i] = XOR(XOR(a[i],b[i]),c[i]);
    }
    else{
      c[i] = mix(a[i-1],b[i-1],c[i-1]);
      z[i] = XOR(XOR(a[i],b[i]),c[i]);
    }
  }
  //cout << "THM  " << log2(z[0]) << " " << log2(z[1]) << " " << log2(z[2]) << " " << log2(z[3]) << " " << log2(z[4])<< " " << log2(z[5])<< " " << log2(z[6])<< " " << log2(z[7]) << endl;
  cout << "THM  " ;
  for(int index=0; index<=(wordsize-1); index++){
    cout << (log2(z[index])) << " ";
  }cout << endl;
  /*cout << "THM probability " ;
  for(int index=0; index<=(wordsize-1); index++){
    cout << z[index] << " ";
  }cout << endl;*/
  cout << "THM bias" ;
  for(int index=0; index<=(wordsize-1); index++){
    cout << (log2(abs(z[index]-0.5))) << " ";
  }cout << endl;
}

int main(){
  srand(time(NULL));
  //experiment correlation
  /*for(int i=0; i<=dataspace; i++){
      for(int j=0; j<=dataspace; j++){
        int indiffleft = i;
        int indiffright = j;
        cout << hex << indiffleft << " " << indiffright << endl;
        Prob_by_exper(indiffleft,indiffright);
        Prob_by_thm(indiffleft,indiffright);
  }}*/
  cout << "Given the following input RX-difference, print the experimental and theoretical RL-probability" << endl;
  cout << "The rotation amount is " << t << endl;
        int indiffleft = 0x7ffffffc;
        int indiffright = 0x7ffffffe;
        cout << "input difference: " << hex << indiffleft << " " << indiffright << endl;
        Prob_by_exper(indiffleft,indiffright,t);
        if(t == 1) {Prob_by_thm(indiffleft,indiffright);}
        else {Prob_by_thm_t(indiffleft,indiffright,t);}


return 0;
}

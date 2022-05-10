//Check the theoretical and experimental correlation in Example 1
// for all input differences a and b
#include<iostream>
#include<cstdlib>
#include<cstdint>
#include<cmath>
#include<random>
#define XOR(p1,p2) (p1+p2-2*p1*p2)
#define mix(p1,p2,p3) (p1*p2*p3-0.5*(p1*p2+p1*p3+p2*p3)+0.5*(p1+p2+p3)) //mix = xy^yz^xz
using namespace std;

const int mod_size = 8;


void modadd(double *a, double *b, double *z, int wordsize){
	double c[32] = {0};
	c[0] = 0;
	z[0] = XOR(a[0],b[0]);
		for(int i=1; i<wordsize; i++){
		c[i] = mix(a[i-1],b[i-1],c[i-1]);
		z[i] = XOR(XOR(a[i],b[i]),c[i]);
		}
}
void modadd_true(int diff1, int diff2, double *z, int wordsize){
   int i_d, j_d, out;
   for(int i=0x0; i<=0xff; i++)
   {
       for(int j=0x0; j<=0xff; j++)
       {
           i_d = i^diff1;
           j_d = j^diff2;
           out = ((i_d+j_d)&0xff)^((i+j)&0xff);

           for(int index=0; index<wordsize; index++)
           {
               if((out>>index)&0x1 == 1)
               {
                   z[index]=z[index]+1;
               }
           }

       }
   }
}

void print_cor_thm(int diff1, int diff2){
	double a[mod_size] = {0};
	for(int i=0; i<mod_size; i++){
	  a[i] = (diff1>>i)&0x1;
	}
	double b[mod_size] = {0};
	for(int i=0; i<mod_size; i++){
	  b[i] = (diff2>>i)&0x1;
	}
	double z[mod_size] = {0};
	double ex_z[mod_size] = {0};
	modadd(a,b,z,mod_size);
	modadd_true(diff1, diff2, ex_z, mod_size);
	for(int i=0; i<mod_size; i++)
    {
        cout<<z[i]<<" "<<(ex_z[i]/(256.0*256.0))<<endl;
    }
}

void confirm(int diff1, int diff2){
	double a[mod_size] = {0};
	for(int i=0; i<mod_size; i++){
	  a[i] = (diff1>>i)&0x1;
	}
	double b[mod_size] = {0};
	for(int i=0; i<mod_size; i++){
	  b[i] = (diff2>>i)&0x1;
	}
	double z[mod_size] = {0};
	double ex_z[mod_size] = {0};
	modadd(a,b,z,mod_size);
	modadd_true(diff1, diff2, ex_z, mod_size);
	int mid;
	for(int i=0; i<mod_size; i++)
    {
        mid = z[i]*(256.0*256.0);
        if (ex_z[i]!=mid)
        {
            cout<<diff1<<" "<<diff2<<" "<<i<<endl;
        }
    }
}


int main()
{
  for(int i=0x0; i<=0xff; i++)
  {
       for(int j=0x0; j<=0xff; j++)
       {
          confirm(i, j);
       }
     cout<<i<<" is check out."<<endl;
}
    cout<<"check complete."<<endl;
    return 0;
}

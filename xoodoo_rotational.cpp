//find rotational-linear distinguisher in xoodoo09
#include<iostream>
#include<cstdlib>
#include<cmath>
#include<vector>

#define XOR(p1,p2) (p1+p2-2*p1*p2)
#define AND(p1,p2) ((p1+p2-p1*p2)*0.5)
#define ROL(x,r) ((x<<r)|(x>>(32-r)))
using namespace std;

int main(){
int rot = 1; //amount of left rotation
int rd = 4; //number of round
cout << "Xoodoo round " << rd << endl;
float a[4][3][32] = {0};  //initial probability, every bit equals its rotational pair bit
float p[4][32] = {0};
float e[4][32] = {0};


uint32_t rx_diff[4][3] = {0};

rx_diff[0][0] = 0x07473334;
rx_diff[0][1] = 0x07473340;
rx_diff[0][2] = 0x07473340;
rx_diff[1][0] = 0x242b6751;
rx_diff[1][1] = 0x242b6751;
rx_diff[1][2] = 0x242b6751;
rx_diff[2][0] = 0x5cb8a32e;
rx_diff[2][1] = 0x5cb8a32e;
rx_diff[2][2] = 0x5cb8a32e;
rx_diff[3][0] = 0xbfdff2e5;
rx_diff[3][1] = 0xbfdff2e5;
rx_diff[3][2] = 0xbfdff2e5;

for(int x=0; x<=3; x++){
for(int y=0; y<=2; y++){
for(int z=0; z<=31; z++){
  a[x][y][z] = (float)((rx_diff[x][y]>>z)&0x1);
}}}

//cout << "rx_diff" << hex <<  rx_diff[0][0] <<endl;


//const uint64_t rc[12] = {0x00000058,0x00000038,0x000003C0,0x000000D0,0x00000120,0x00000014,0x00000060,0x0000002C,0x00000380,0x000000F0,0x000001A0,0x00000012};
const uint64_t rc[4] = {0x0000002C,0x00000380,0x000000F0,0x000001A0};

uint64_t rc_diff[12] = {0};
vector<vector<int>> pos_rc_diff(12); //define 12-dim vector, each vector is a vector of integer that contains all the nonzero positions in rc-diff.
for(int i=0; i<=11; i++){  //12 round, store the position of the nonzero bit in rc_diff
  rc_diff[i] = rc[i] ^ ROL(rc[i],rot);
  for(int j=0; j<=31; j++){
    if((rc_diff[i]&0x1)==1){pos_rc_diff[i].push_back(j);}
    rc_diff[i] = rc_diff[i] >> 1;
  }
}

for(int i = 0; i<=3; i++){
for(int j = 0; j<=2; j++){
  cout << i << j << "    ";
for(int k = 0; k<=31; k++){
  cout << log2(abs(2*a[i][j][k]-1)) << " ";
  //cout << a[i][j][k] << " ";
}cout << endl;}}
cout << endl;

for(int round = 1; round <= rd; round++){
//theta
for(int x = 0; x<= 3; x++){
for(int z = 0; z<= 31; z++){
p[x][z] = XOR(XOR(a[x][0][z],a[x][1][z]),a[x][2][z]);
/*cout << b[j][i] << " ";
if(i==63) cout << endl;*/
}}

for(int x = 0; x<= 3; x++){
for(int z = 0; z<= 31; z++){
e[x][z] = XOR(p[(x+3)%4][(z+27)%32], p[(x+3)%4][(z+18)%32]);
}}


for(int x = 0; x<= 3; x++){
for(int y = 0; y<= 2; y++){
for(int z = 0; z<= 31; z++){
a[x][y][z] = XOR(a[x][y][z],e[x][z]);
}}}

//rho west
double tmp[4][32] = {0};
for(int z=0; z<=31; z++){
for(int x=0; x<=3; x++){
tmp[x][z] = a[(x+3)%4][1][z];
}}
for(int z=0; z<=31; z++){
for(int x=0; x<=3; x++){
a[x][1][z] = tmp[x][z];
}}
for(int z=0; z<=31; z++){
for(int x=0; x<=3; x++){
tmp[x][z] = a[x][2][(z+21)%32];   //<<<11 = >>> 21
}}
for(int z=0; z<=31; z++){
for(int x=0; x<=3; x++){
a[x][2][z] = tmp[x][z];
}}

//iota constant addition
for(std::vector<int>::iterator iter = pos_rc_diff[round-1].begin(); iter != pos_rc_diff[round-1].end(); ++iter){
  int temp = *iter;   //the nonzero position
  a[0][0][temp] = 1 - a[0][0][temp];   //only flip the probability at the nonzero rc_diff positions
}

//chi
  double temp[4][3][32] = {0};
for(int x = 0; x<= 3; x++){
for(int y = 0; y<= 2; y++){
for(int z = 0; z<= 31; z++){
  temp[x][y][z] = XOR(a[x][y][z], AND(a[x][(y+1)%3][z],a[x][(y+2)%3][z]));
}}}
for(int x = 0; x<= 3; x++){
for(int y = 0; y<= 2; y++){
for(int z = 0; z<= 31; z++){
  a[x][y][z] = temp[x][y][z];
}}}


//rho east
for(int z=0; z<=31; z++){
for(int x=0; x<=3; x++){
  temp[x][1][z] = a[x][1][(z+31)%32];   //<<<1 = >>> 31
}}
for(int z=0; z<=31; z++){
for(int x=0; x<=3; x++){
  a[x][1][z] = temp[x][1][z];   //<<<1 = >>> 31
}}
for(int z=0; z<=31; z++){
for(int x=0; x<=3; x++){
  temp[x][2][z] = a[(x+2)%4][2][(z+24)%32];   //<<<8 = >>> 24
}}
for(int z=0; z<=31; z++){
for(int x=0; x<=3; x++){
  a[x][2][z] = temp[x][2][z];
}}

cout << "round counter = " << round << endl;
cout << "Correlation = 2 ^ " << endl;
for(int i = 0; i<=3; i++){
for(int j = 0; j<=2; j++){
  cout << i << j << "    ";
for(int k = 0; k<=31; k++){
  cout << log2(abs(2*a[i][j][k]-1)) << " ";
  //cout << a[i][j][k] << " ";
}cout << endl;}}
cout << endl;
}//end of round
return 0;
}

//this program has one-bit difference input
//CONSTANT: this program uses the adjusted C-rule
#include<iostream>
#include<cstdlib>
#include<cmath>
#include<vector>
#include<iomanip>
#define XOR(p1,p2) (p1+p2-2*p1*p2)
#define AND(p1,p2) ((p1+p2-p1*p2)*0.5)
#define ROL(x,r) ((x<<r)|(x>>(64-r)))

using namespace std;

const uint64_t rc[24] = {0x1111, 0x11100000, 0x1101, 0x10100000,
                0x101, 0x10110000, 0x110, 0x11000000,
                0x1001, 0x100000, 0x100, 0x10000000,
                0x1, 0x110000, 0x111, 0x11110000,
                0x1110, 0x11010000, 0x1010, 0x1010000,
                0x1011, 0x1100000, 0x1100, 0x10010000};
uint64_t rc_diff[24] = {0};

void find_max(double *vec, double *max, int *flag){ //vector size = 128, max and flag have their address passed into the function
  double tmp = -64; //initialise
  *flag = 0; //index of the max
  for(int i=0; i<128; i++){
    double vec_log = log2(abs(2*vec[i]-1));
    if(vec_log>tmp){ tmp = vec_log; *flag = i;}
  }
  *max = tmp;
}//for finding the maximum in one round.


int main(){
bool Print_DETAIL = false; //true = print all cases; false = only print the maximum
int rot = 4; //amount of rotation, rotate to the left
int Rd = 7; // round counter

//Compute the constant difference
vector<vector<int>> pos_rc_diff(24); //define 24-dim vector, each vector is a vector of integer that contains all the nonzero positions in rc-diff.
for(int i=0; i<=23; i++){
  rc_diff[i] = rc[i] ^ ROL(rc[i],rot);
  for(int j=0; j<=63; j++){
    if((rc_diff[i]&0x1)==1){pos_rc_diff[i].push_back(j);}
    rc_diff[i] = rc_diff[i] >> 1;
  }
}
/* //print pos_rc_diff
for(int i=0; i<=23; i++){
  for(std::vector<int>::iterator iter = pos_rc_diff[i].begin(); iter != pos_rc_diff[i].end(); ++iter){
    cout << *iter << " ";
  }
  cout << endl;
}*/

//for(int shift = 1; shift <= 127; shift++){
//rot++;
//cout << "SHIFT_AMOUNT = " << rot << endl;
//DO NOT change the parameter!!!1
//a[7][128] -> 7: all intermediate steps and varaibles used in one round, 128: the wordsize
//DO NOT change 7 to the round counter!!!
int branch_a_overall = -1; //if output max is -1, it means that all prob < 2^-64
int branch_b_overall = -1;
int branch_c_overall = -1;

int act_pos_a_overall = -1; //if branc_a_overall is -1, then this variable is also -1
int act_pos_b_overall = -1;
int act_pos_c_overall = -1;

double max_a_overall = -64; //we assume that a 64-it operation system computer can only detect up to 2^-64
double max_b_overall = -64;
double max_c_overall = -64;

int flag_a_overall = 0;  //gives the position of the maximum, 0<=flag<=127
int flag_b_overall = 0;
int flag_c_overall = 0;

for(int branch = 0; branch <=2; branch ++){ //branch 0 = a, branch 1 = b, branch 2 = c
for(int act_pos = 0; act_pos <=127; act_pos++){ //127,126,...,1,0 //
double a[7][128] = {0};  //initial probability, every bit equals its rotational pair bit
double b[7][128] = {0};
double c[7][128] = {0};

  if(branch==0){
    a[0][act_pos] = 1;
  }
  else if(branch==1){
    b[0][act_pos] = 1;
  }
  else if(branch==2){
    c[0][act_pos] = 1;
  }
  else cout << "branch not found!" << endl;

if(Print_DETAIL==true){
cout << "Print the correlation of input difference: branch = " << branch << "  active position at = " << act_pos << endl;
}

for(int round=1; round<=Rd; round++){
   cout.setf(ios::fixed);
   if(Print_DETAIL==true) {cout << "round number: " << round << endl; }

        //This part outputs the full probability state
        /*cout << "a: ";
        for(int k = 0; k<= 127; k++){cout << log2(abs(2*a[0][k]-1)) << " ";}cout << endl;
        cout << "b: ";
        for(int k = 0; k<= 127; k++){cout << log2(abs(2*b[0][k]-1)) << " ";}cout << endl;
        cout << "c: ";
        for(int k = 0; k<= 127; k++){cout << log2(abs(2*c[0][k]-1)) << " ";}cout << endl;*/


        //We want to output the maximum in the whole state, and use the following
       if(Print_DETAIL==true) {
        double max_a = 0;
        double max_b = 0;
        double max_c = 0;
        int flag_a = 0;
	      int flag_b = 0;
	      int flag_c = 0;
        find_max(a[0], &max_a, &flag_a);  cout << "maxmum index = " << flag_a << " --- max_a = " << max_a <<endl;
        find_max(b[0], &max_b, &flag_b);  cout << "maxmum index = " << flag_b << " --- max_b = " << max_b <<endl;
        find_max(c[0], &max_c, &flag_c);  cout << "maxmum index = " << flag_c << " --- max_c = " << max_c <<endl;
        cout << endl;}
        //flag = output position
        //max_a = output correlation


///////////////////ROUND FUNCTION BEGIN/////////////////////
	//const addition  delta layer
	for(int k = 0; k<= 127; k++){
    a[1][k] = a[0][k];
    b[1][k] = b[0][k];
	  c[1][k] = c[0][k];
	}
  for(std::vector<int>::iterator iter = pos_rc_diff[round-1].begin(); iter != pos_rc_diff[round-1].end(); ++iter){
    int temp = *iter;   //the nonzero position
    c[1][temp] = 1 - c[0][temp];   //only flip the probability at the nonzero rc_diff positions
  }

	//twist  tau1 layer
      	for(int k = 0; k<= 127; k++){
	        a[2][k] = XOR(c[1][k],XOR(a[1][k],b[1][k]));
          b[2][k] = c[1][k];
          c[2][k] = a[1][k];
        }
  //mu1 layer
        for(int k = 0; k<= 127; k++){
	        a[3][k] = a[2][k];
          if(k>=1) {b[3][k] = XOR(b[2][k],c[2][k-1]);}
          else {b[3][k] = XOR(b[2][k],c[2][127]);}
          c[3][k] = c[2][k];
        }

   //mu2 layer
        for(int k = 0; k<= 127; k++){
	        a[4][k] = a[3][k];
          if(k>=80) {c[4][k] = XOR(c[3][k],b[3][k-80]);}
          else {c[4][k] = XOR(c[3][k],b[3][k+128-80]);}
          b[4][k] = b[3][k];
        }

  //tau2 layer
        for(int k = 0; k<= 127; k++){
	        a[5][k] = a[4][k];
          b[5][k] = XOR(c[4][k],XOR(a[4][k],b[4][k]));
	        c[5][k] = c[4][k];
        }
	//xi
        for(int k = 0; k<= 127; k++){
	        if(k>=67) {a[6][k] = XOR(a[5][k],AND(b[5][k-36],c[5][k-67]));}
          else if((k>=36)&&(k<67)) {a[6][k] = XOR(a[5][k],AND(b[5][k-36],c[5][k+128-67]));}
          else {a[6][k] = XOR(a[5][k],AND(b[5][k+128-36],c[5][k+128-67]));}
          b[6][k] = b[5][k];
	        c[6][k] = c[5][k];
        }

        for(int k = 0; k<= 127; k++){
	        a[0][k] = a[6][k];
          b[0][k] = b[6][k];
	        c[0][k] = c[6][k];
        }
}
///////////////////ROUND FUNCTION END/////////////////////

if(Print_DETAIL==false){ //Print_DETAIL = false, only output the maximum in this round
        double max_a_current = 0;
        double max_b_current = 0;
        double max_c_current = 0;
        int flag_a_current = 0;
	int flag_b_current = 0;
	int flag_c_current = 0;
  find_max(a[0], &max_a_current, &flag_a_current);
  find_max(b[0], &max_b_current, &flag_b_current);
  find_max(c[0], &max_c_current, &flag_c_current);
//Now we have branch -- act_pos -- max_a_current -- flag_a_current, we find max_a_overall
if(max_a_current>max_a_overall){max_a_overall = max_a_current; flag_a_overall = flag_a_current; branch_a_overall = branch; act_pos_a_overall = act_pos;}
if(max_b_current>max_b_overall){max_b_overall = max_b_current; flag_b_overall = flag_b_current; branch_b_overall = branch; act_pos_b_overall = act_pos;}
if(max_c_current>max_c_overall){max_c_overall = max_c_current; flag_c_overall = flag_c_current; branch_c_overall = branch; act_pos_c_overall = act_pos;}
}
}}

if(Print_DETAIL==false){
cout << "ROUND = " << Rd <<endl;
cout << "branch = " << branch_a_overall << " --- diff_active at = " << act_pos_a_overall << " --- maxmum index = " << flag_a_overall << " --- max_a_overall = " << max_a_overall <<endl;
cout << "branch = " << branch_b_overall << " --- diff_active at = " << act_pos_b_overall << " --- maxmum index = " << flag_b_overall << " --- max_b_overall = " << max_b_overall <<endl;
cout << "branch = " << branch_c_overall << " --- diff_active at = " << act_pos_c_overall << " --- maxmum index = " << flag_c_overall << " --- max_c_overall = " << max_c_overall <<endl;
}

return 0;
}

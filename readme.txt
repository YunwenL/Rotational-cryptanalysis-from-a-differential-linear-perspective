# Supplementary codes for "Rotational cryptanalysis from a differential-linear perspective"
# 32-bit-modular-gurobi-rot.py 
  + Find the optimal input difference pattern for 32-bit modular addition with gurobipy
  + Dependencies: gurobi, gurobipy
  
# alzette_box.cpp
  + test dl-probability in alzette
  + output theoretical and experimental probabilities given input difference
  
  Input differences from LSB to MSB
  a = 0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   
  b = 1   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   
  DL-probability by theoretical prediction:  from LSB to MSB
  a: 
  0.655592   0.485762   0.5   0.500016   0.499984   0.498234   0.511236   0.5   0.5   0.500159   0.5   0.5   0.499989   0.498681   0.490532         0.465059   
  0.5   0.5   0.5   0.5   0.499919   0.49806   0.487089   0.45399   0.607869   0.5   0.5   0.499999   0.499832   0.497588   0.51345   0.5   

  b: 
  0.5   0.5   0.5   0.5   0.499998   0.49979   0.496895   0.518265   0.5   0.5   0.5   0.5   0.499992   0.499612   0.504439   0.5   
  0.611633   0.5   0.5   0.5   0.499999   0.499743   0.503395   0.5   0.5   0.499881   0.5   0.5   0.499999   0.499668   0.503844   0.5 
  
  
# alzette_box_rxdl.cpp
  + test rdl-probability in alzette
  
  Input differences from LSB to MSB
  a = 0   0   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   0   
  b = 1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   0   0   
  c_diff = d823f3a7
  probability by theoretical prediction:
a: 
0.5   0.500189   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   
0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   

b: 
0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   
0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5   0.5 
  
# compute_dl_modular.cpp/compute_rxdl_modular.cpp
  + compute dl/rdl probability in modular addition

# friet-rl-find-overall-maximum.cpp
  + find the maximum rl-probability for friet with one-bit input difference
  Instruction:
  branch = 0 (a)/ 1 (b)/ 2 (c)
  diff_active at = 16  ---> difference = 0x1<<16
  maximum index = 18   ---> output mask = 0x1<<18
  max_c_overall = -5.812183   ---> output mask at branch c, correlation = 2^{-5.812183}
  
  program outputs:
ROUND = 6
branch = 2 --- diff_active at = 16 --- maxmum index = 18 --- max_a_overall = -6.710447
branch = 2 --- diff_active at = 16 --- maxmum index = 18 --- max_b_overall = -14.491025
branch = 2 --- diff_active at = 16 --- maxmum index = 18 --- max_c_overall = -5.812183
  
# friet_distinguisher_prob_check.cpp
  +experiment verification of friet rl-distinguishers
  Instruction: input difference and output mask at line 169 -- 175
  
# siphash_dl.cpp
  + compute dl-probability for siphash
  Instruction: input difference at line 92, the program outputs the imbalance.
  
  
# xoodoo_probability_check.cpp
  + experiemnt verification of xoodoo rl-distinguishers
  instruction: 

# xoodoo_rotational.cpp
   + xoodoo rl-distinguisher by theoretical evaluation
   //////////////////////////////////////////
using last 4-round constants. 
Input differences are:
/*indiff[0][0] = 0x484ccc80;
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
indiff[3][2] = 0x45a3f0cb;*/
////////////////////////////////////////
round counter = 4
Correlation = 2 ^ 
00    -6 -3.19265 -inf -inf -5 -3.41504 -inf -0.678072 -inf -3.41504 -3.67807 -inf -inf -3 -4 -inf -3 -3 -2 -3 -inf -2.67807 -inf -5 -inf -1.67807 -inf -4 -2 -1.67807 -4 -inf 
01    -inf -inf -4.41504 -inf -2 -inf -5 -inf -inf -1 -3.41504 -3.67807 -3.91254 -3 -1.67807 -inf -2.91254 -inf -inf -inf -inf -inf -inf -inf -inf -3.67807 -inf -inf -inf -inf -inf -inf 
02    -inf -inf -inf -inf -inf -inf -inf -inf -inf -inf -3 -inf -inf -inf -inf -inf -inf -inf -inf -2 -inf -inf -inf -4 -inf -inf -inf -4 -inf -2 -inf -inf 
10    -inf -inf -3 -inf -inf -3 -2 -1 -inf -inf -inf -inf -inf -inf -inf -inf 0 -inf -1 -inf -inf -inf -inf -inf -inf -2 -inf -inf -1 -inf -2 -inf 
11    -inf -6 -3 -inf -inf -5.41504 -inf -inf -inf -inf -inf -inf -inf -inf -3.41504 -4 -inf 0 -1.41504 -inf -3 -inf -2 -inf -5 -inf -1.41504 -inf -inf -1 -inf -1.41504 
12    -2.67807 -inf -3 -inf -inf -inf -inf -inf -inf -inf -inf -3 -inf -inf -inf -inf -inf -inf -inf -inf -inf -inf -1.41504 -inf -4 -inf -4 -2 -3 -inf -4 -3 
20    -1 -inf -inf -inf -4 -inf -inf -inf -4 -inf -4 -inf -inf -inf -4 -inf -inf -3 -inf -inf -inf -1.41504 -inf -5 -inf -inf -inf -inf -inf -inf -inf -inf 
21    -inf -inf -2 -inf -inf -inf -2 -inf -2 -inf -inf -inf -inf -inf -inf -inf -inf -2 -inf -2 -inf -inf -inf -inf -inf -inf -inf -inf -inf -2 -2 -inf 
22    -3.67807 -3 -inf -1.83008 -inf -3 -2.41504 -inf -inf -inf -5 -inf -1.91254 -inf -6 -inf -1 -inf -inf -5.67807 -2 -inf -inf -7 -2 -inf -1.41504 -inf -4 -3.41504 -inf -inf 
30    -inf -4 -inf -1.67807 -inf -5 -inf -4 -1.67807 -4 -4 -4 -2 -2 -inf -3 -inf -inf -inf -inf -inf -inf -inf -inf -3.41504 -inf -inf -inf -inf -inf -inf -inf 
31    -inf -2 -inf -inf -inf -4 -inf -inf -inf -3 -inf -2.41504 -inf -inf -inf -2 -inf -inf -3 -inf -inf -inf -2 -inf -2 -inf -inf -inf -inf -inf -inf -inf 
32    -inf -inf -inf -2 -inf -1 -inf -inf -inf -inf -inf -inf -2.91254 -inf -inf -1 -3 -2 -inf -inf -2 -2.67807 -inf -inf 0 -2 -1 -inf -inf -inf -inf -inf 

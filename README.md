# Rotational Cryptanalysis From a Differential-Linear Perspective

This repository contains the source code for verifying the distinguishers presented in the paper "Rotational Cryptanalysis From a Differential-Linear Perspective".

## 32-bit-modular-gurobi-rot.py 
  + Find the optimal input difference pattern for 32-bit modular addition with gurobipy
  + Dependencies: gurobi, gurobipy
  
## alzette_box.cpp
  + test dl-probability in alzette
  + output theoretical and experimental probabilities given input difference
  
  + Input differences from LSB to MSB: a = 0x0, b = 0x1  
  > a = 0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0     
  > b = 1   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0    
  + DL-probability by theoretical prediction:  from LSB to MSB
  >a:   
  >0.655592   0.485762   0.5   0.500016   0.499984   0.498234   0.511236   0.5   0.5   0.500159   0.5   0.5   0.499989   0.498681   0.490532         0.465059     
  >0.5   0.5   0.5   0.5   0.499919   0.49806   0.487089   0.45399   0.607869   0.5   0.5   0.499999   0.499832   0.497588   0.51345   0.5     

  >b:   
  >0.5   0.5   0.5   0.5   0.499998   0.49979   0.496895   0.518265   0.5   0.5   0.5   0.5   0.499992   0.499612   0.504439   0.5     
  >0.611633   0.5   0.5   0.5   0.499999   0.499743   0.503395   0.5   0.5   0.499881   0.5   0.5   0.499999   0.499668   0.503844   0.5   
  
  
## alzette_box_rxdl.cpp
  + compute and verify rdl-probability in alzette
  
  + Input differences from LSB to MSB: a = 0x7ffffffc, b = 0x3fffffff
  >a = 0   0   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   0     
  >b = 1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   0   0     

  
## compute_dl_modular.cpp/compute_rxdl_modular.cpp
  + compute dl/rdl probability in modular addition

## friet-rl-find-overall-maximum.cpp
  + find the maximum rl-probability for friet with one-bit input difference
  + Instruction:  
  + branch = 0 (a)/ 1 (b)/ 2 (c)  
  + diff_active at = 16  ---> difference = 0x1<<16  
  + maximum index = 18   ---> output mask = 0x1<<18  
  + max_c_overall = -5.812183   ---> output mask at branch c, correlation = 2^{-5.812183}  
  
  > Example of outputs:  
  > ROUND = 6  
> branch = 2 --- diff_active at = 16 --- maxmum index = 18 --- max_a_overall = -6.710447  
> branch = 2 --- diff_active at = 16 --- maxmum index = 18 --- max_b_overall = -14.491025  
> branch = 2 --- diff_active at = 16 --- maxmum index = 18 --- max_c_overall = -5.812183  
  
## friet_distinguisher_prob_check.cpp
  +experiment verification of friet rl-distinguishers
  +Instruction: input difference and output mask at line 169 -- 175
  
## siphash_dl.cpp
  + compute dl-probability for siphash
  +Instruction: input difference at line 92, the program outputs the imbalance.

## xoodoo_rotational.cpp
   + xoodoo rl-distinguisher by theoretical evaluation

+In program we're using the last 4 round constants. 
>Input differences are:
>indiff[0][0] = 0x484ccc80;
>indiff[0][1] = 0x484cc800;
>indiff[0][2] = 0x484cc800;    
>indiff[1][0] = 0x3ab9821a;
>indiff[1][1] = 0x3ab9821a;
>indiff[1][2] = 0x3ab9821a;  
>indiff[2][0] = 0x37b6cde9;
>indiff[2][1] = 0x37b6cde9;
>indiff[2][2] = 0x37b6cde9;  
>indiff[3][0] = 0x45a3f0cb;
>indiff[3][1] = 0x45a3f0cb;
>indiff[3][2] = 0x45a3f0cb;  
>round counter = 4  
+correlation for the output bits: the first two numbers "32" represent the index for row and column respectively, the following values are the binary logrithm of the correlation, from lsb to msb, for instance,
>Correlation = 2 ^   
>32    -inf -inf -inf -2 -inf -1 -inf -inf -inf -inf -inf -inf -2.91254 -inf -inf -1 -3 -2 -inf -inf -2 -2.67807 -inf -inf 0 -2 -1 -inf -inf -inf -inf -inf 

## xoodoo_probability_check.cpp
  + experiemnt verification of xoodoo rl-distinguishers obtained from xoodoo_rotational.cpp
 

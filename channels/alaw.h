/*
   Copyright (c) 1997 Kolja Waschk 

   Name: alaw.h
   Version: 1.0
   Author: Marco Budde
   Description: Converts a-law signals to signed linear signals and
                vice versa.


   This file is part of isdn2h323.              
 
   This is public domain, but if you take more from this source than 
   just the idea, it would be neat to see my name mentioned in your 
   source or readme.        

*/


#define ALAW2INT(x) alaw2int[x]
#define INT2ALAW(x) int2alaw[(x>>4)+4096]         


// table to convert unsigned a-law bytes to signed linear integers

static const int alaw2int[256] = {
  -5504,5504,-344,344,-22016,22016,-1376,1376,-2752,2752,-88,88,-11008,11008,
  -688,688,-7552,7552,-472,472,-30208,30208,-1888,1888,-3776,3776,-216,216,
  -15104,15104,-944,944,-4480,4480,-280,280,-17920,17920,-1120,1120,-2240,2240,
  -24, 24,-8960,8960,-560,560,-6528,6528,-408,408,-26112,26112,-1632,1632,
  -3264,3264,-152,152,-13056,13056,-816,816,-6016,6016,-376,376,-24064,24064,
  -1504,1504,-3008,3008,-120,120,-12032,12032,-752,752,-8064,8064,-504,504,
  -32256,32256,-2016,2016,-4032,4032,-248,248,-16128,16128,-1008,1008,-4992,
   4992,-312,312,-19968,19968,-1248,1248,-2496,2496,-56,56,-9984,9984,-624,624,
  -7040,7040,-440,440,-28160,28160,-1760,1760,-3520,3520,-184,184,-14080,14080,
  -880,880,-5248,5248,-328,328,-20992,20992,-1312,1312,-2624,2624,-72,72,
  -10496,10496,-656,656,-7296,7296,-456,456,-29184,29184,-1824,1824,-3648,3648,
  -200,200,-14592,14592,-912,912,-4224,4224,-264,264,-16896,16896,-1056,1056,
  -2112,2112, -8,  8,-8448,8448,-528,528,-6272,6272,-392,392,-25088,25088,
  -1568,1568,-3136,3136,-136,136,-12544,12544,-784,784,-5760,5760,-360,360,
  -23040,23040,-1440,1440,-2880,2880,-104,104,-11520,11520,-720,720,-7808,7808,
  -488,488,-31232,31232,-1952,1952,-3904,3904,-232,232,-15616,15616,-976,976,
  -4736,4736,-296,296,-18944,18944,-1184,1184,-2368,2368,-40,40,-9472,9472,
  -592,592,-6784,6784,-424,424,-27136,27136,-1696,1696,-3392,3392,-168,168,
  -13568,13568,-848,848
};


// table to convert linear shorts to a-law bytes
// shift the integer to be 12+1 bit first, then add 4096 to get 
// the right index 

static const unsigned char int2alaw[8192] = {
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,84,
  212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,
  212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,
  212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,
  212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,
  20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
  20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
  20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
  20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,
  148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
  148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
  148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
  148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
  116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,
  116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,
  116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,
  116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,
  244,244,244,244,244,244,244,244,244,244,244,244,244,244,244,244,
  244,244,244,244,244,244,244,244,244,244,244,244,244,244,244,244,
  244,244,244,244,244,244,244,244,244,244,244,244,244,244,244,244,
  244,244,244,244,244,244,244,244,244,244,244,244,244,244,244,244,
  52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,
  52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,
  52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,
  52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,
  180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,
  180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,
  180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,
  180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,
  68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
  68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
  68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
  68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
  196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
  196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
  196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
  196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
  4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
  4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
  4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
  4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
  132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
  132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
  132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
  132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
  100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,
  100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,
  100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,
  100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,
  228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,
  228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,
  228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,
  228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,
  36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
  36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
  36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
  36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,
  164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,
  164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,
  164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,
  164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,
  92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
  92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,
  220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,
  220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,
  28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,
  28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,
  156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,
  156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,
  124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
  124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,124,
  252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,
  252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,
  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
  188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,
  188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,
  76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,
  76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,
  204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,
  204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,
  12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
  12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
  140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
  140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
  108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
  108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
  236,236,236,236,236,236,236,236,236,236,236,236,236,236,236,236,
  236,236,236,236,236,236,236,236,236,236,236,236,236,236,236,236,
  44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
  44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
  172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,
  172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,
  80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,
  208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,
  16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
  144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,
  112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,
  240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,240,
  48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,
  176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,
  64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
  192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
  96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,
  224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,
  88,88,88,88,88,88,88,88,216,216,216,216,216,216,216,216,
  24,24,24,24,24,24,24,24,152,152,152,152,152,152,152,152,
  120,120,120,120,120,120,120,120,248,248,248,248,248,248,248,248,
  56,56,56,56,56,56,56,56,184,184,184,184,184,184,184,184,
  72,72,72,72,72,72,72,72,200,200,200,200,200,200,200,200,
  8,8,8,8,8,8,8,8,136,136,136,136,136,136,136,136,
  104,104,104,104,104,104,104,104,232,232,232,232,232,232,232,232,
  40,40,40,40,40,40,40,40,168,168,168,168,168,168,168,168,
  86,86,86,86,214,214,214,214,22,22,22,22,150,150,150,150,
  118,118,118,118,246,246,246,246,54,54,54,54,182,182,182,182,
  70,70,70,70,198,198,198,198,6,6,6,6,134,134,134,134,
  102,102,102,102,230,230,230,230,38,38,38,38,166,166,166,166,
  94,94,222,222,30,30,158,158,126,126,254,254,62,62,190,190,
  78,78,206,206,14,14,142,142,110,110,238,238,46,46,174,174,
  82,210,18,146,114,242,50,178,66,194,2,130,98,226,34,162,
  90,218,26,154,122,250,58,186,74,202,10,138,106,234,42,170,
  171,43,235,107,139,11,203,75,187,59,251,123,155,27,219,91,
  163,35,227,99,131,3,195,67,179,51,243,115,147,19,211,83,
  175,175,47,47,239,239,111,111,143,143,15,15,207,207,79,79,
  191,191,63,63,255,255,127,127,159,159,31,31,223,223,95,95,
  167,167,167,167,39,39,39,39,231,231,231,231,103,103,103,103,
  135,135,135,135,7,7,7,7,199,199,199,199,71,71,71,71,
  183,183,183,183,55,55,55,55,247,247,247,247,119,119,119,119,
  151,151,151,151,23,23,23,23,215,215,215,215,87,87,87,87,
  169,169,169,169,169,169,169,169,41,41,41,41,41,41,41,41,
  233,233,233,233,233,233,233,233,105,105,105,105,105,105,105,105,
  137,137,137,137,137,137,137,137,9,9,9,9,9,9,9,9,
  201,201,201,201,201,201,201,201,73,73,73,73,73,73,73,73,
  185,185,185,185,185,185,185,185,57,57,57,57,57,57,57,57,
  249,249,249,249,249,249,249,249,121,121,121,121,121,121,121,121,
  153,153,153,153,153,153,153,153,25,25,25,25,25,25,25,25,
  217,217,217,217,217,217,217,217,89,89,89,89,89,89,89,89,
  161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,
  33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,
  225,225,225,225,225,225,225,225,225,225,225,225,225,225,225,225,
  97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
  129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,
  65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,
  177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,
  49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,
  241,241,241,241,241,241,241,241,241,241,241,241,241,241,241,241,
  113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
  145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,
  17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
  209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,
  81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
  173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,
  173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,
  45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,
  45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,
  237,237,237,237,237,237,237,237,237,237,237,237,237,237,237,237,
  237,237,237,237,237,237,237,237,237,237,237,237,237,237,237,237,
  109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
  109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
  141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,
  141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,
  13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
  13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
  205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,
  205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,
  77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,
  77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,
  189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,
  189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,
  61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
  61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
  253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,
  253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,
  125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
  125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
  157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
  157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,
  221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,221,
  93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
  93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
  165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
  165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
  165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
  165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
  37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,
  37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,
  37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,
  37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,
  229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,
  229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,
  229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,
  229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,229,
  101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,
  101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,
  101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,
  101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,
  133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
  133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
  133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
  133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,
  197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,
  197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,
  197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,
  69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,
  69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,
  69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,
  69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,
  181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,
  181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,
  181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,
  181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,
  53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
  53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
  53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
  53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
  245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,
  245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,
  245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,
  245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,
  117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,
  117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,
  117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,
  117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,
  149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
  149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
  149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
  149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
  21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
  21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
  21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
  21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
  213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,
  213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,
  213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,
  213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,213,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,
  85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85
};

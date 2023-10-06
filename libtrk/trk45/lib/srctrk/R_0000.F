C----------------------------------------------------------------------C
C BLOCK DATA STATEMENT                                                 C
C----------------------------------------------------------------------C
      BLOCK DATA R_0000                                                 
C----------------------------------------------------------------------C
C     BLOCK DATA NAME : R_0000                                         C
C                                                                      C
C     FUNCTION        : INITIALIZE THE CONSTANTS                       C
C                                                                      C
C     AUTHOR          : FUJTISU LIMITED    1990.09.03                  C
C                                                                      C
C     -NOTE-                                                           C
C                                                                      C
C      PROPER MOTION & ANNUAL PARALLAX ARE DUMMY VARIABLES, USED FOR   C
C      THE TEST OF THE TRACKING MODULE.                                C
C                                                                      C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * DECLARE DIVITION             *
C                                       *------------------------------*
#include      "__DFTF"                                                
C                                       * COMMON AREA OF INSTRUCTION   *
#include      "XTRK00"
#include      "XIERST"                                                
#include      "XGLCST"                                                
#include      "XMATHC"                                                
#include      "XUNTCV"                                                
#include      "XOBSEI"                                                
#include      "XDUMMY"                                                
C                                                                       
C----------------------------------------------------------------------C
C CONSTANT DATA DEFINITION                                             C
C----------------------------------------------------------------------C
C                                       *------------------------------*
C                                       * ASTRONOMICAL CONSTANT (IERS) *
C                                       *------------------------------*
C DVLLGT : ∏˜¬Æ [m/sec]
C DMRLPE : ∑Ó°ø√œµÂº¡ŒÃ»Ê
C DASUNT : ≈∑ ∏√±∞Ã[Km]
C DGSHGS : ¬¿Õ€§ŒΩ≈ŒœƒÍøÙ = (¬¿Õ€º¡ŒÃ)°ﬂ(À¸Õ≠∞˙ŒœƒÍøÙ) [m**3/sec**2]
C DSLMSS : ¬¿Õ€º¡ŒÃ[Kg]
C DMRPPS : ¬¿Õ€°øœ«¿±º¡ŒÃ»Ê  øÂ¿±°¢∂‚¿±°¢√œµÂ°¢≤–¿±°¢Ã⁄¿±°¢≈⁄¿±°¢
C                            ≈∑≤¶¿±°¢≥§≤¶¿±°¢ÃΩ≤¶¿±
C DVLERT : √œµÂº´≈æ≥—¬Æ≈Ÿ[rad/sec]
C
      DATA   DVLLGT , DMRLPE , DASUNT , DGSHGS , DSLMSS ,               
     +       DMRPPS , DVLERT                                            
     +     / 299792458D0,   0.012300034D0, 1.4959787066D11,             
     +       1.32712440D20, 1.9891D30,     6023600D0,                   
     +       408523.5D0,    332946.038D0,  3098710D0, 1047.350D0,       
     +       3498.0D0,      22960D0,       19314D0,   130000000D0,      
     +       7.292115D-5 /                                              
C                                                                       
C                                       *------------------------------*
C                                       * GALACTIC STANDARD            *
C                                       *------------------------------*
C DSMTJ2 : ¬¿Õ€∏˛≈¿[rad](J2000)
C DSMTB1 : ¬¿Õ€∏˛≈¿[rad](B1950)
C
      DATA   DSMTJ2 , DSMTB1 , DGNPRA , DGNPDE , DGCTRA , DGCTDE ,      
     +       DGANGL , DGANRA , DWQINC                                   
     +     / 4.7291354145576D00,     5.236803697408D-1,             
     +       4.7207564524290D00,     5.236190892915D-1,             
     +       3.3660329342285640D00,  3.3553954869590987D00,             
     +       4.734773070448358D-1,   4.782202150464463D-1,              
     +       4.6495571273128940D00,  4.635594493296939D00,              
     +      -5.0498193024369270D-1, -5.046910420350270D-1,              
     +       5.747703990664133D-1,   5.759586531581288D-1,              
     +       4.9366637892659613D00,  4.926191813753995D00,              
     +       1.0972303230871018D00,  1.092576111748450D00 /             
C                                                                       
C                                       *------------------------------*
C                                       * MATHEMATICAL CONSTANT        *
C                                       *------------------------------*
      DATA   DPIVAL , DRCPPI , DHLFPI , DTWOPI , DTHQPI , DFVQPI        
     +     / 3.1415926535897932D00,  3.1830988618379067D-1,             
     +       1.5707963267948966D00,  6.2831853071795865D00,             
     +       4.7123889803846899D00,  7.8539816339744831D00 /            
C                                                                       
C                                       *------------------------------*
C                                       * CONSTANT RELATED TO OBSERVER *
C                                       *-10M--------------------------*
      DATA   JANTNO / 5 /                                               
C                                                                       
C                                       *------------------------------*
C                                       * CONSTANT FOR UNIT CONVERSION *
C                                       *------------------------------*
      DATA   DHSRAD , DACRAD , DRNDEG , DRADDH , DRNHMN , DRNHSC        
     +     / 7.2722052166430399D-5, 4.8481368110953599D-6,              
     +       5.7295779513082321D01, 3.8197186342054881D00,              
     +       2.2918311805232928D02, 1.3750987083139757D04 /             
C                                                                       
C                                       *------------------------------*
C                                       * PROPER MOTION & ANNUAL PAR.  *
C                                       *------------------------------*
      DATA   DPROPM , DANPRL , DRVELC                                   
     +     / 0D0 , 0D0 ,                                                
     +       0D0 , 0D0 /                                                
C                                                                       
C                                       *------------------------------*
C                                       * trk_xx •È•§•÷•È•Íª»Õ—•’•È•∞  *
C                                       *------------------------------*
      DATA   ITRF00, ITRF10, ITRF20
     +      / 0, 0, 0/
C
      END 

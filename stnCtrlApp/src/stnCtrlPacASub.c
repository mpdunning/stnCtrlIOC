 /* Collection of aSub subroutines...*/
 /*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <dbDefs.h>
#include <alarm.h>
#include <registryFunction.h>
#include "aSubRecord.h"
#include <epicsExport.h>
#include <math.h>
#include <stdlib.h>

#define SIZE(x)               (sizeof(x)/sizeof(x[0]))

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;



static long asPacRBV(aSubRecord *pr){
/*------------------------------------------------------------------------------
 * Calculate current PAC phase/amp from I & Q 
 *----------------------------------------------------------------------------*/
    #define PI 3.14159265
    double *i0 = (double*)pr->a;
    double *q0 = (double*)pr->b;
    double *phsDes0 = (double*)pr->c;
    double *phs=(double*)pr->vala;
    double *amp=(double*)pr->valb;
    double *phsRad=(double*)pr->valc;

    double phsDiff;    

    *phsRad=atan2(*q0,*i0);
  
    // Allow for -360 <= phase <= 360  
    if (*phsDes0 >= 180.0) {
        *phsRad += 2*PI;
    }
    if (*phsDes0 <= -180.0) {
        *phsRad -= 2*PI;
    }

    *phs=*phsRad*(180.0/PI);
    *amp=sqrt((*i0)*(*i0) + (*q0)*(*q0))*(100.0/32767);

    // Correct for phase flips  
    phsDiff=*phsDes0-*phs;
    if (phsDiff > 180.0) {
        *phs += 360.0;
    }
    if (phsDiff < -180.0) {
        *phs -= 360.0;
    }

//printf("phsDes=%f, phsRad=%f, phsDeg=%f, phsDiff=%f\n",*phsDes0,*phsRad,*phs,phsDiff);

    return(0);
}

static long asPacSet(aSubRecord *pr){
/*------------------------------------------------------------------------------
 * Calculate I & Q from desired phase/amp and send to PAC 
 *----------------------------------------------------------------------------*/
    #define PI 3.14159265
    double *phsDes=(double*)pr->a;
    double *ampDes=(double*)pr->b;
    double *iMax=(double*)pr->c;
    double *qMax=(double*)pr->d;
    
    int *iOut=(int*)pr->vala;
    int *qOut=(int*)pr->valb;
    
    double q1,i1;

    i1=*ampDes*(*iMax/100.0)*cos(*phsDes*(PI/180));
    q1=*ampDes*(*qMax/100.0)*sin(*phsDes*(PI/180));    

    *iOut=lround(i1);
    *qOut=lround(q1);
//printf("i1=%f, q1=%f, iout=%f, qout=%f\n",i1,q1,*iOut,*qOut);

    return(0);
}

static long asPacPhsAmpWf(aSubRecord *pr){
/*------------------------------------------------------------------------------
 * Calculate phase & amp waveforms
 *----------------------------------------------------------------------------*/
    #define DATA_LEN 2048
    double *iref=(double*)pr->a;
    double *qref=(double*)pr->b;
    double *phsOut=(double*)pr->vala;
    double *ampOut=(double*)pr->valb;
    int i;

    for (i=0; i<DATA_LEN; i++) {
        phsOut[i]=atan2(qref[i],iref[i]);
        ampOut[i]=sqrt(pow(qref[i],2) + pow(iref[i],2));
    }

//printf("i1=%f, q1=%f, iout=%f, qout=%f\n",i1,q1,*iOut,*qOut);

    return(0);
}

static long asPacPulse(aSubRecord *pr){
/*------------------------------------------------------------------------------
 * Select & tune I/Q waveforms
 *----------------------------------------------------------------------------*/
    #define DATA_LEN    2048
    #define PI          3.14159265
    #define NBINS       6
    #define CLOCKFREQ   119.0e6
    short  *pulseMode=  (short*)pr->a;
    short  *pulseWidth= (short*)pr->b;
    short  *pulseDelay= (short*)pr->c;
    short  *sledBinSep= (short*)pr->d;
    double *phsBin1=    (double*)pr->e;
    double *phsBin2=    (double*)pr->f;
    double *phsBin3=    (double*)pr->g;
    double *phsBin4=    (double*)pr->h;
    double *phsBin5=    (double*)pr->i;
    double *phsBin6=    (double*)pr->j;

    double *iOut=          (double*)pr->vala;
    double *qOut=          (double*)pr->valb;
    double *pwidOut=       (double*)pr->valc;
    double *pdelOut=       (double*)pr->vald;
    double  *sledBinSepOut=(double*)pr->vale;

    int i,j,pwidTemp=0;
    int iDefault=30000,qDefault=30000;
    double clockPeriod=1/CLOCKFREQ;
    //int sledPwidMax=floor(240/(clockPeriod*1e9));
    int sledPwidMax=*sledBinSep;
    int pwidMax=floor(3000/(clockPeriod*1e9));
    double phsBinArr[NBINS]={*phsBin1,*phsBin2,*phsBin3,*phsBin4,*phsBin5,*phsBin6};

    

    if (*pulseMode==1) {  /* SLED Waveform */
        if (*pulseWidth > sledPwidMax) pwidTemp=sledPwidMax;
        else pwidTemp=*pulseWidth;
        for (i=0; i<*pulseDelay; i++) {
            iOut[i]=0;
            qOut[i]=0;
        }
        for (j=0; j<NBINS; j++) {
            for (i=*pulseDelay + j*sledPwidMax; i<*pulseDelay + j*sledPwidMax + pwidTemp; i++) {
                iOut[i]=iDefault*cos(phsBinArr[j]*(PI/180.0));
                qOut[i]=qDefault*sin(phsBinArr[j]*(PI/180.0));
            }
            for (i=*pulseDelay + j*sledPwidMax + pwidTemp; i<*pulseDelay + (j+1)*sledPwidMax; i++) {
                iOut[i]=0;
                qOut[i]=0;
            }
        }
        for (i=*pulseDelay + (NBINS+1)*sledPwidMax + pwidTemp; i<DATA_LEN; i++) {
            iOut[i]=0;
            qOut[i]=0;
        }
    }
    else if (*pulseMode==2) {  /* Calibration Waveform */
        for (i=0; i<DATA_LEN; i++) {
            iOut[i]=0;
            qOut[i]=0;
        }
    }
    else {  /* Klystron Waveform */
        if (*pulseWidth > pwidMax) pwidTemp=pwidMax;
        else pwidTemp=*pulseWidth;
        for (i=0; i<*pulseDelay; i++) {
            iOut[i]=0;
            qOut[i]=0;
        }
        for (i=*pulseDelay; i<*pulseDelay + pwidTemp; i++) {
            iOut[i]=iDefault;
            qOut[i]=0;
        }
        for (i=*pulseDelay + pwidTemp; i<DATA_LEN; i++) {
            iOut[i]=0;
            qOut[i]=0;
        }
    }
    
    *pwidOut=(double)pwidTemp*clockPeriod*1e9;
    *pdelOut=(double)(*pulseDelay)*clockPeriod*1e9;
    *sledBinSepOut=(double)(*sledBinSep)*clockPeriod*1e9;

//printf("sled=%d, pulseWidth=%d, pwidTemp=%d, pwidOut=%f\n",sled,*pulseWidth,pwidTemp,*pwidOut);

    return(0);
}

/**************************************************************************/

epicsRegisterFunction(asPacRBV);
epicsRegisterFunction(asPacSet);
epicsRegisterFunction(asPacPhsAmpWf);
epicsRegisterFunction(asPacPulse);


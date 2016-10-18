 /* Collection of aSub subroutines...
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <dbDefs.h>
#include <alarm.h>
#include <registryFunction.h>
#include "subRecord.h"
#include "aSubRecord.h"
#include <epicsExport.h>
#include <math.h>
#include <stdlib.h>

#define SIZE(x)               (sizeof(x)/sizeof(x[0]))

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;



static long asPad(subRecord *pr){
/*------------------------------------------------------------------------------
 * Calculates phase & amplitude from I & Q 
 *----------------------------------------------------------------------------*/
    #define PI 3.14159265
    double q0=pr->a;
    double i0=pr->b;
    
    double ampMax=32767,ampMin=0;

    //pr->c=(sqrt(q0*q0 + i0*i0)-ampMin)*(100/ampMax);
    pr->c=sqrt(q0*q0 + i0*i0);
    pr->d=atan2(i0,q0);

//printf("tmpPhs=%f, offPhs=%f\n",tmpPhs,offPhs);

    return(0);
}

static long asPadStats(aSubRecord *pr){
/*------------------------------------------------------------------------------
 * Do some stats
 *----------------------------------------------------------------------------*/
    double *ampData=(double*)pr->a;
    int    ampDataLen=pr->noa;
    double *phsData=(double*)pr->b;
    int    phsDataLen=pr->nob;

    double *ampAve=(double*)pr->vala;
    double *ampStd=(double*)pr->valb;
    double *phsAve=(double*)pr->valc;
    double *phsStd=(double*)pr->vald;

    int i;
    double sum;

    sum=0.0;
    for (i=0; i<ampDataLen; i++) {
        sum+=ampData[i];
    }
    *ampAve=sum/((double)(ampDataLen));
    
    sum=0.0;
    for (i=0; i<ampDataLen; i++) {
        sum+=(ampData[i] - *ampAve)*(ampData[i] - *ampAve);
    }
    *ampStd=sqrt(sum/((double)(ampDataLen)));

    sum=0.0;
    for (i=0; i<phsDataLen; i++) {
        sum+=phsData[i];
    }
    *phsAve=sum/((double)(phsDataLen));
    
    sum=0.0;
    for (i=0; i<phsDataLen; i++) {
        sum+=(phsData[i] - *phsAve)*(phsData[i] - *phsAve);
    }
    *phsStd=sqrt(sum/((double)(phsDataLen)));

//printf("i1=%f, q1=%f, iout=%f, qout=%f\n",i1,q1,*iOut,*qOut);

    return(0);
}

static long asPadWf(aSubRecord *pr){
/*------------------------------------------------------------------------------
 * 
 *----------------------------------------------------------------------------*/
    double *wfIn=(double*)pr->a;
    int  wfLen=pr->noa;

    double *wfAve=(double*)pr->vala;
    double *wfRms=(double*)pr->valb;
    double *wfStd=(double*)pr->valc;

    int i;
    double sum,sum1,sumSq;

    sum=0.0,sumSq=0.0;
    for (i=0; i<wfLen; i++) {
        sum+=wfIn[i];
        sumSq+=(wfIn[i])*(wfIn[i]);
    }
    *wfAve=sum/((double)wfLen);
    *wfRms=sqrt(sumSq/((double)wfLen));

    sum1=0.0;
    for (i=0; i<wfLen; i++) {
        sum1+=(wfIn[i]-*wfAve)*(wfIn[i]-*wfAve);
    }
    *wfStd=sqrt(sum1/((double)wfLen));

//printf("wfLen=%d, sum=%f, sumSq=%f\n",wfLen,sum,sumSq);
//printf("wfAve=%f, wfRms=%f, wfStd=%f\n",*wfAve,*wfRms,*wfStd);

    return(0);
}


/**************************************************************************/

epicsRegisterFunction(asPad);
epicsRegisterFunction(asPadStats);
epicsRegisterFunction(asPadWf);


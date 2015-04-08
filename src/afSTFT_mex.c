/*Copyright (c) 2015 Juha Vilkamo
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */


#include <string.h>
#include "mex.h"
#include "afSTFTlib.h"

/* Variables in MEX file */
int forwardChannels=0;
int forwardChannels_new=0;
int inverseChannels=0;
int inverseChannels_new=0;
int hopSize=0;
int forwardPointer=0;
int hopSize_new=0;
int isInitialized=0;
void* afSTFT_handle;
float **forwardTD;
complexVector *forwardFD;
float **inverseTD;
complexVector *inverseFD;
int hybridMode,lowDelayMode;
char *parameter;

/* MEX-function */
void mexFunction(int nlhs, mxArray *plhs[ ],int nrhs, const mxArray *prhs[ ])
{
    int ch,sample,band,FDsample,isComplex,k;
    double *prIn, *piIn;
    double *prIn2, *piIn2;
    double *prOut, *piOut;
    double *prOut2, *piOut2;
    
    /* INPUT PARAMETER CHECKS */
    if (nrhs > 1 && nrhs != 3  && nrhs != 4  && nrhs != 5)
    {
        mexWarnMsgTxt("one input, please. No inputs = flush / free, 3-5 inputs = init(hopSize, forwardChannels, inverseChannels,'hybrid' (optional), 'low_delay' (optional))");
        return;
    }
    
    /* FREE */
    if (nrhs == 0 || (nrhs > 2 && isInitialized == 1))
    {
        if (isInitialized > 0)
        {
            afSTFTfree(afSTFT_handle);
            
            for (ch=0;ch<forwardChannels;ch++)
            {
                free(forwardTD[ch]);
                free(forwardFD[ch].re);
                free(forwardFD[ch].im);
            }
            free(forwardTD);
            free(forwardFD);
            for (ch=0;ch<inverseChannels;ch++)
            {
                free(inverseTD[ch]);
                free(inverseFD[ch].re);
                free(inverseFD[ch].im);
            }
            inverseTD=(float**)malloc(sizeof(float*)*inverseChannels);
            inverseFD=(complexVector*)malloc(sizeof(complexVector)*inverseChannels);
            isInitialized = 0;
        }
    }
    
    /* INITIALIZATION */
    if (nrhs > 2)
    {
        hopSize = *mxGetPr(prhs[0]);
        forwardChannels = *mxGetPr(prhs[1]);
        inverseChannels = *mxGetPr(prhs[2]);
        
        hybridMode=0;
        lowDelayMode=0;
        
        /* PARSE INPUT PARAMETERS */
        for (k=3;k<nrhs;k++)
        {
            parameter = mxArrayToString(prhs[k]);
            if (parameter && strcmp(parameter, "hybrid") == 0)
            {
                if (hopSize == 64 || hopSize == 128)
                {
                    hybridMode = 1;
                }
                else
                {
                    mexWarnMsgTxt("The hybrid mode is meaningful (and allowed) on hopsizes 64 or 128 only.");
                    return;
                }
            }
            else if (parameter && strcmp(parameter, "low_delay") == 0)
            {
                lowDelayMode = 1;
            } else
            {
                mexWarnMsgTxt("The optional parameters are 'hybrid' and 'low_delay' (or both as separate parameters, or neither)");
                return;
            }
            mxFree(parameter);
        }
        
        /* Allow only certain hop sizes, prototype defined until 1024 hop size */
        if (!(hopSize == 32 || hopSize == 64 || hopSize == 128 || hopSize == 256 || hopSize == 512 || hopSize == 1024))
        {
            mexWarnMsgTxt("Use pow2 hop sizes between 32--1024");
            return;
            
        }
        if (forwardChannels < 0 || forwardChannels > 2000)
        {
            mexWarnMsgTxt("Use 0-2000 forward channels (or re-compile afSTFT with new setting)");
            return;
        }
        if (inverseChannels < 0 || inverseChannels > 2000)
        {
            mexWarnMsgTxt("Use 0-2000 inverse channels (or re-compile afSTFT with new setting)");
            return;
        }

        /* Call filter bank init function */
        afSTFTinit(&afSTFT_handle,hopSize, forwardChannels, inverseChannels,lowDelayMode,hybridMode);
        
        /* Allocate MEX file buffers */
        forwardTD=(float**)malloc(sizeof(float*)*forwardChannels);
        forwardFD=(complexVector*)malloc(sizeof(complexVector)*forwardChannels);
        for (ch=0;ch<forwardChannels;ch++)
        {
            forwardTD[ch] = (float*)malloc(sizeof(float)*hopSize);
            forwardFD[ch].re = (float*)malloc(sizeof(float)*(hopSize+1+4*hybridMode));
            forwardFD[ch].im = (float*)malloc(sizeof(float)*(hopSize+1+4*hybridMode));
        }
        
        inverseTD=(float**)malloc(sizeof(float*)*inverseChannels);
        inverseFD=(complexVector*)malloc(sizeof(complexVector)*inverseChannels);
        for (ch=0;ch<inverseChannels;ch++)
        {
            inverseTD[ch] = (float*)malloc(sizeof(float)*hopSize);
            inverseFD[ch].re = (float*)malloc(sizeof(float)*(hopSize+1+4*hybridMode));
            inverseFD[ch].im = (float*)malloc(sizeof(float)*(hopSize+1+4*hybridMode));
        }
        isInitialized = 1;
        
        /* If an output variable is defined, output the normalized band frequencies */
        if (nlhs == 1)
        {
            int numBands = hopSize+1+4*hybridMode;
            plhs[0] = mxCreateDoubleMatrix(numBands, 1, mxREAL);
            double *freq = mxGetPr(plhs[0]);
            for (int band=0;band<numBands;band++)
            {
                if (hybridMode)
                {
                    if (band<1)
                    {
                        freq[band]=0.0f;
                    }
                    else if (band < 9)
                    {
                        freq[band]=(0.25f + (float)band*0.5f)/(float)(numBands-5);
                    }
                    else
                    {
                        freq[band]= (float)(band-4)/(float)(numBands-5);
                    }
                        
                }
                else
                {
                    freq[band]=(float)(band)/(float)(numBands-1);
                }
            }
        }
        
    }
    
    /* PROCESS */
    if (nrhs == 1)
    {
        if (isInitialized==0)
        {
            mexWarnMsgTxt("Init first");
        }
        else
            
        {
            size_t dims = mxGetNumberOfDimensions(prhs[0]);
            const size_t* dim = mxGetDimensions_730(prhs[0]);
            
            if (dims < 3 && dim[1] == forwardChannels && dim[0] != (hopSize+1+4*hybridMode))
            {
                /*** FORWARD PROCESSING ***/
                size_t TDsamples = dim[0];
                size_t outDim = 3;
                size_t outDims[3];
                outDims[0]=hopSize+1+4*hybridMode;
                outDims[1]=TDsamples/hopSize;
                outDims[2]=forwardChannels;
                
                if (forwardChannels == 1)
                {
                    outDim = 2;
                }
                prIn = mxGetPr(prhs[0]);
                plhs[0]=mxCreateNumericArray_730(outDim, outDims, mxDOUBLE_CLASS, mxCOMPLEX);
                prOut = mxGetPr(plhs[0]);
                piOut = mxGetPi(plhs[0]);
                FDsample=0;
                for (FDsample=0;FDsample<outDims[1];FDsample++)
                {
                    for (ch=0;ch<forwardChannels;ch++)
                    {
                        prIn2 = prIn + dim[0]*ch + FDsample*hopSize;
                        for (sample=0;sample<hopSize;sample++)
                        {
                            forwardTD[ch][sample]=*prIn2;
                            prIn2++;
                        }
                        
                    }
                    
                    afSTFTforward(afSTFT_handle, forwardTD, forwardFD);
                    
                    for (ch=0;ch<forwardChannels;ch++)
                    {
                        prOut2 = prOut + (outDims[0]*outDims[1]*ch) + outDims[0]*FDsample;
                        piOut2 = piOut + (outDims[0]*outDims[1]*ch) + outDims[0]*FDsample;
                        for (band=0;band<hopSize+1+4*hybridMode;band++)
                        {
                            *prOut2 = forwardFD[ch].re[band];
                            *piOut2 = forwardFD[ch].im[band];
                            prOut2++;
                            piOut2++;
                        }
                    }
                }
            }
            else
            {
                
                size_t channels = 1;
                size_t FDsamples = dim[1];
                if (dims == 3)
                {
                    channels = dim[2];
                }
                
                if( dim[0]==(hopSize+1+4*hybridMode) && channels == inverseChannels)
                {
                    /*** INVERSE PROCESSING ***/
                    plhs[0]=mxCreateDoubleMatrix(hopSize*FDsamples,channels,mxREAL);
                    prOut= mxGetPr(plhs[0]);
                    isComplex= mxIsComplex(prhs[0]);
                    prIn = mxGetPr(prhs[0]);
                    if (isComplex)
                    {
                        piIn = mxGetPi(prhs[0]);
                    }
                    for (FDsample=0;FDsample<FDsamples;FDsample++)
                    {
                        
                        
                        for (ch=0;ch<inverseChannels;ch++)
                        {
                            prIn2 = prIn +(dim[0]*dim[1]*ch) + dim[0]*FDsample;
                            if (isComplex)
                            {
                                piIn2 = piIn +(dim[0]*dim[1]*ch) + dim[0]*FDsample;
                                for (band=0;band<hopSize+1+4*hybridMode;band++)
                                {
                                    inverseFD[ch].re[band]=*prIn2;
                                    inverseFD[ch].im[band]=*piIn2;
                                    prIn2++;
                                    piIn2++;
                                }
                            }
                            else
                            {
                                for (band=0;band<hopSize+1+4*hybridMode;band++)
                                {
                                    inverseFD[ch].re[band]=*prIn2;
                                    inverseFD[ch].im[band]=0.0f;
                                    prIn2++;
                                }
                            }
                            inverseFD[ch].im[0]=0.0f;
                            inverseFD[ch].im[hopSize+4*hybridMode]=0.0f;
                        }
                        
                        afSTFTinverse(afSTFT_handle, inverseFD,inverseTD);
                        
                        for (ch=0;ch<inverseChannels;ch++)
                        {
                            prOut2 = prOut + (hopSize*FDsamples)*ch + FDsample*hopSize;
                            for (sample=0;sample<hopSize;sample++)
                            {
                                *prOut2 = inverseTD[ch][sample];
                                prOut2++;
                            }
                        }
                    }
                    
                }
                else{
                    mexWarnMsgTxt("Incorrect input configuration. Forward: (samples x forwardChannels), Inverse: (hopSize+1 x FD samples x inverseChannels)");
                }
            }
        }
    }
}

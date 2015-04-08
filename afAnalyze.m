function out=afAnalyze(in)
% Transforms a time-domain signal to 133 band "hybrid" resolution. 
% The resolution is otherwise the same as 129-point FFT, but first four bands
% above DC frequency are further subdivided with linear filters.
%
% This function is for single-shot processing. For continuous processing 
% apply afSTFT function as instructed in afSTFT_usage_instruction.m
%
% Juha Vilkamo
% e-mail: juha.vilkamo@aalto.fi
%

afSTFT(128,size(in,2),1,'hybrid'); % init
out=afSTFT(in); % process
afSTFT(); % free
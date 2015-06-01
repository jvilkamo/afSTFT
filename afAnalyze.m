function out=afAnalyze(in,hybMode)
% Transforms a time-domain signal to 133 band "hybrid" resolution or to 129 band non-hybrid resolution. 
% The hybrid resolution is otherwise the same as 129-point FFT, but first four bands
% above DC frequency are further subdivided with linear filters.
%
% This function is for single-shot processing. For continuous processing 
% apply afSTFT function as instructed in afSTFT_usage_instruction.m
%
% Juha Vilkamo
% e-mail: juha.vilkamo@aalto.fi
%
if nargin < 2
    hybMode = 1;
end 
if hybMode > 0
    afSTFT(128,size(in,2),1,'hybrid'); % init
else
    afSTFT(128,size(in,2),1); % init
end
out=afSTFT(in); % process
afSTFT(); % free
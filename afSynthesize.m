function out=afSynthesize(in)
% Transforms a 133 band "hybrid" or 129 band non-hybrid resolution signal to time domain. 
% The time-frequency data is as provided by the afAnalyze.m
%
% This function is for single-shot processing. For continuous processing 
% apply afSTFT function as instructed in afSTFT_usage_instruction.m
%
% Juha Vilkamo
% e-mail: juha.vilkamo@aalto.fi
%
if size(in,1) == 133
    afSTFT(128,1,size(in,3),'hybrid'); % Initialize
else
    afSTFT(128,1,size(in,3)); % Initialize
end
out=afSTFT(in); % Process
afSTFT(); % Free

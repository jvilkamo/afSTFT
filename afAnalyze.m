function out=afAnalyze(in)
afSTFT(128,size(in,2),1,'hybrid'); % init
out=afSTFT(in); % process
afSTFT(); % free
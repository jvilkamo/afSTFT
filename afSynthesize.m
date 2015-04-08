function out=afSynthesize(in)
afSTFT(128,1,size(in,3),'hybrid');
out=afSTFT(in);
afSTFT();

function compile_afSTFT_mex
try
    mex src/afSTFT_mex.c src/afSTFTlib.c src/vecTools.c src/fft4g.c -Isrc COPTIMFLAGS='-O3 -DNDEBUG' -output afSTFT
catch % Windows compiler fix (not maybe most elegant, but worked..)
    system('copy src/afSTFT_mex.c afSTFT_mex.cpp');
    system('copy src/afSTFTlib.c afSTFTlib.cpp');
    system('copy src/vecTools.c vecTools.cpp');
    system('copy src/fft4g.c fft4g.cpp');
    mex afSTFT_mex.cpp afSTFTlib.cpp vecTools.cpp fft4g.cpp -Isrc COPTIMFLAGS='-O3 -DNDEBUG' -output afSTFT
    system('del afSTFT_mex.cpp');
    system('del afSTFTlib.cpp');
    system('del vecTools.cpp');
    system('del fft4g.cpp');
end
inFrame=randn(1280,40);

afSTFT(128,40,40); % init
a=cputime;
for k=1:800
    out= afSTFT(inFrame); % keep processing same frame for a few times..
end
b=cputime;
CPU_LOAD_TEST_on_i7_this_is_about_one = (b-a)
afSTFT(); % free




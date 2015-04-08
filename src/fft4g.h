#include <math.h>

void cdft(int n, int isgn, float *a, int *ip, float *w);
void rdft(int n, int isgn, float *a, int *ip, float *w);
void ddct(int n, int isgn, float *a, int *ip, float *w);
void ddst(int n, int isgn, float *a, int *ip, float *w);
void dfct(int n, float *a, float *t, int *ip, float *w);
void dfst(int n, float *a, float *t, int *ip, float *w);
void makewt(int nw, int *ip, float *w);
void makect(int nc, int *ip, float *c);
void bitrv2(int n, int *ip, float *a);
void bitrv2conj(int n, int *ip, float *a);
void cftfsub(int n, float *a, float *w);
void cftbsub(int n, float *a, float *w);
void cft1st(int n, float *a, float *w);
void cftmdl(int n, int l, float *a, float *w);
void rftfsub(int n, float *a, int nc, float *c);
void rftbsub(int n, float *a, int nc, float *c);
void dctsub(int n, float *a, int nc, float *c);
void dstsub(int n, float *a, int nc, float *c);


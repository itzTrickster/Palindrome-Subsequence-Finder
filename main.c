#include <stdio.h>

#define MAXN 40

char strbuf[MAXN];
int n;
int lenMemo[MAXN][MAXN];
int lenDone[MAXN][MAXN];

typedef struct { int len; int pos[MAXN]; } Seq;
Seq seqMemo[MAXN][MAXN];
int seqDone[MAXN][MAXN];

char lowerc(char c){
    if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
    return c;
}

int eqIC(char a, char b){
    return lowerc(a) == lowerc(b);
}

int lps_length(int i, int j){
    if (i > j) return 0;
    if (i == j) return 1;
    if (lenDone[i][j]) return lenMemo[i][j];
    int res;
    if (eqIC(strbuf[i], strbuf[j])){
        res = 2 + lps_length(i+1, j-1);
    } else {
        int a = lps_length(i+1, j);
        int b = lps_length(i, j-1);
        res = (a > b) ? a : b;
    }
    lenDone[i][j] = 1;
    lenMemo[i][j] = res;
    return res;
}

int lexicomp(const Seq *a, const Seq *b){
    int L = a->len;
    if (L != b->len) return (a->len > b->len) ? 1 : -1;
    for (int k = 0; k < L; k++){
        if (a->pos[k] < b->pos[k]) return 1;
        if (a->pos[k] > b->pos[k]) return -1;
    }
    return 0;
}

Seq copySeq(const Seq *src){
    Seq dst;
    dst.len = src->len;
    for (int i = 0; i < src->len; i++) dst.pos[i] = src->pos[i];
    return dst;
}

Seq makeEmpty(){
    Seq s; s.len = 0; return s;
}

Seq makeSingle(int p){
    Seq s; s.len = 1; s.pos[0] = p; return s;
}

Seq makePairWithInner(int left, int right, const Seq *inner){
    Seq out;
    out.len = inner->len + 2;
    out.pos[0] = left;
    for (int k = 0; k < inner->len; k++) out.pos[k+1] = inner->pos[k];
    out.pos[out.len-1] = right;
    return out;
}

Seq build_seq(int i, int j){
    if (i > j) return makeEmpty();
    if (i == j) return makeSingle(i);
    if (seqDone[i][j]) return seqMemo[i][j];

    Seq best = makeEmpty();

    if (eqIC(strbuf[i], strbuf[j])){
        Seq inner = build_seq(i+1, j-1);
        Seq cand = makePairWithInner(i, j, &inner);
        best = cand;
    }

    Seq leftOpt = build_seq(i+1, j);
    Seq rightOpt = build_seq(i, j-1);

    Seq candidates[3];
    int ccount = 0;
    candidates[ccount++] = best;
    candidates[ccount++] = leftOpt;
    candidates[ccount++] = rightOpt;

    Seq chosen = candidates[0];
    for (int t = 1; t < ccount; t++){
        if (candidates[t].len == 0 && chosen.len == 0) { chosen = candidates[t]; continue; }
        if (candidates[t].len > chosen.len) chosen = candidates[t];
        else if (candidates[t].len == chosen.len){
            int cmp = lexicomp(&candidates[t], &chosen);
            if (cmp == 1) chosen = candidates[t];
        }
    }

    seqDone[i][j] = 1;
    seqMemo[i][j] = chosen;
    return chosen;
}

int isAllowed(char c){
    if (c >= 'A' && c <= 'Z') return 1;
    if (c >= 'a' && c <= 'z') return 1;
    if (c >= '0' && c <= '9') return 1;
    const char *ok = "' ,.~\\<>/?!@#$%^&*()_+{}[]:;|";
    for (int i = 0; ok[i]; i++) if (c == ok[i]) return 1;
    return 0;
}

int main(){
    char buf[100];
    if (!fgets(buf, sizeof(buf), stdin)){
        printf("\n\n");
        return 0;
    }
    int ln = 0;
    while (buf[ln] != '\0' && buf[ln] != '\n' && buf[ln] != '\r') ln++;
    if (ln > 35){ printf("incorrect msg\n"); return 0; }
    n = ln;
    for (int i = 0; i < n; i++){
        strbuf[i] = buf[i];
        if (!isAllowed(strbuf[i])){ printf("incorrect msg\n"); return 0; }
    }
    if (n == 0){ printf("\n\n"); return 0; }

    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++){ lenDone[i][j] = 0; seqDone[i][j] = 0; }

    lps_length(0, n-1);
    Seq ans = build_seq(0, n-1);

    int L = ans.len;
    char out[MAXN];
    for (int k = 0; k < L; k++) out[k] = strbuf[ans.pos[k]];
    out[L] = '\0';
    printf("%s\n", out);

    int used[MAXN] = {0};
    for (int k = 0; k < L; k++) used[ans.pos[k]] = 1;

    int first = 1;
    for (int p = 0; p < n; p++){
        if (!used[p]){
            if (!first) printf(", ");
            printf("%d - %c", p+1, strbuf[p]);
            first = 0;
        }
    }
    printf("\n");
    return 0;
}

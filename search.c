#include "stdio.h"
#include "string.h"

#define S 26 // alphabet size
#define M 4  // min word length
#define H 7  // unique letters

int mask_scores[1 << S];
char wordbuf[32];

// popcount of mask gives UNIQUE letter count
// if disallowed characters, return 0
unsigned get_word_mask(char s[])
{
    unsigned mask = 0;
    for (; *s != '\0'; ++s)
    {
        if (!('a' <= *s && *s <= 'z')) // lowercase letters 
            return 0;

        mask |= 1 << (*s - 'a');
    }
    return mask;
}

int word_score(int len, unsigned nunique)
{
    return (len == M) ? 1 : len + H * (nunique == H);
}

// score all mask subsets 
// require given center or use -1 to skip center
int subset_score(unsigned mask, int center)
{
    int inds[H];
    int j = 0;
    for (int i = 0; i < S; ++i)
    {
        if (mask & (1 << i))
            inds[j++] = i;
    }

    int score = 0;

    // go through all 7-bit subsets except empty
    for (int i = 0; i < 1 << H; ++i)
    {
        // skip non-center masks if center is specified
        if (!(i & (1 << center)) && center >= 0)
            continue;

        unsigned subset_mask = 0;
        for (int j = 0; j < H; ++j)
        {
            if (i & (1 << j))
                subset_mask |= 1 << inds[j];
        }

        score += mask_scores[subset_mask];
    }

    return score;
}


int main(void)
{
    FILE* file = fopen("/usr/share/dict/words", "r");

    // read word as null-terminated string in buffer
    while (fscanf(file, "%s", wordbuf) != EOF)
    {
        unsigned mask = get_word_mask(wordbuf);

        // filter on longer than 4 letters and valid mask
        int len = strlen(wordbuf);
        if (len >= M && mask != 0) 
        {
            // add word score to mask scores here
            int nunique = __builtin_popcount(mask);
            mask_scores[mask] += word_score(len, nunique);
        }
    }


    int best_score = 0;
    unsigned best_mask = 0;
    int best_center_ind = 0;

    // brute force (26 choose 7 masks)
    for (unsigned mask = 0; mask < 1 << S; ++mask)
    {
        if (__builtin_popcount(mask) != H)
            continue;

        // score mask subsets without center
        int possible_score = subset_score(mask, -1);

        // if score without center doesn't beat best_score, don't bother
        if (possible_score <= best_score)
            continue;

        // try center from 0 to H
        for (int center = 0; center < H; ++center) 
        {
            int score = subset_score(mask, center);

            if (score > best_score)
            {
                best_score = score;
                best_mask = mask;
                printf("%d ", score);
               
                // capitalize center letter for display
                int j = 0;
                for (int i = 0; i < S; ++i)
                {
                    if (mask & (1 << i))
                    {
                        if (j == center)
                            best_center_ind = i;

                        char c = (j == center) ? 'A' + i : 'a' + i;
                        printf("%c", c);
                        ++j;
                    }
                }
                printf("\n");
            }
        }
    }

    // recover words within mask
    // for simplicity didn't store words so goes through file again
    rewind(file);

    while (fscanf(file, "%s", wordbuf) != EOF)
    {
        unsigned mask = get_word_mask(wordbuf);

        if (strlen(wordbuf) >= M && (mask & (1 << best_center_ind)) 
            && (mask | best_mask) == best_mask)
        {
            if (__builtin_popcount(mask) == H)
            {
                // capitalize
                for (char* s = wordbuf; *s != '\0'; ++s)
                    *s += 'A' - 'a';
            }

            printf("%s ", wordbuf);
        }
    }
    fclose(file);
}

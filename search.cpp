// Compile with
// g++ -O3 -Wall -Wextra -std=c++20 -o spelling_bee spelling_bee.cpp

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>

// simpler and much faster than hash table at the cost of space
int mask_scores[1 << 26];

int unique_letters(const std::string& word)
{
    std::unordered_set<char> s(word.begin(), word.end());
    return s.size();
}

// defaults to C locale
bool is_lower(const std::string& word)
{
    for (unsigned char c : word)
        if (!islower(c)) return false;
    
    return true;
}

int word_score(const std::string& word)
{
    if (word.length() == 4) return 1;
    else return word.length() + 7 * (unique_letters(word) == 7);
}

int mask_subset_score(const unsigned int mask, const int center)
{
    int inds[7]; // actually faster than vector<int> and push_back
    int j = 0;
    for (int i = 0; i < 26; ++i) 
    {
        if (mask & (1 << i))
        {
            inds[j++] = i;
        }
    }

    int score = 0;

    // go through all 7-bit combos except empty
    for (int i = 0; i < 1 << 7; ++i)
    {
        if (!(i & (1 << center)))
            continue;

        unsigned int subset_mask = 0;
        for (int j = 0; j < 7; ++j)
        {
            if (i & (1 << j)) 
                subset_mask |= 1 << inds[j];
        }

        score += mask_scores[subset_mask];
    }
    return score; 
}


int main()
{
    // Read words file 

    std::ifstream infile;
    std::vector<std::string> words;

    std::string s; 
    infile.open("/usr/share/dict/words");

    while (infile >> s)
    {
        if (is_lower(s) && s.length() >= 4 && unique_letters(s) <= 7) 
        {
            words.push_back(s);
        }
    }

    // preprocess mask scores
    for (std::string& word : words)
    {
        unsigned int mask = 0;
        for (char c : word)
        {
            mask |= 1 << (c - 'a');
        }
            
        mask_scores[mask] += word_score(word);
    }

    int best_score = 0;
    unsigned int best_mask = 0; 


    // brute force (26 choose 7) masks
    for (unsigned int mask = 0; mask < 1 << 26; ++mask)
    {
        if (std::popcount(mask) != 7)
            continue;

        for (int center = 0; center < 7; ++center)
        {
            int score = mask_subset_score(mask, center);
            
            if (score > best_score)
            {
                best_score = score;
                best_mask = mask;

                std::cout << score << " ";
               
                int j = 0;
                for (int i = 0; i < 26; ++i) 
                {
                    if (mask & (1 << i))
                    {
                        unsigned char c = (j == center) ? 'A' + i : 'a' + i;

                        std::cout << c;
                        ++j;
                    }
                }

                std::cout << '\n';
            }
        }
    }
    
    // recover words within mask
    for (std::string& word : words)
    {
        unsigned int mask = 0;
        for (char c : word)
            mask |= 1 << (c - 'a');
           
        if ((mask | best_mask) == best_mask)
        {
            if (std::popcount(mask) == 7)
            {
                // capitalize
                for (char& c : word) 
                    c = std::toupper(c);
            } 
            std::cout << word << " ";
        }
    }

}

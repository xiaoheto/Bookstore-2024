#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;
int main() {
    unordered_set<string>targetWords;
    int n,m;
    cin >> n;
    for(int i = 0;i < n;++i) {
        string word;
        cin >> word;
        targetWords.insert(word);
    }
    cin >> m;
    vector<string>article(m);
    for(int i = 0;i < m;++i) {
        cin >> article[i];
    }
}
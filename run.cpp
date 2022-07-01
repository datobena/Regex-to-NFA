#include <bits/stdc++.h>
#include <iostream>
using namespace std;
map<int, map<char, vector<int>>> edges;
set<int> accepts;
string input;
int n, a, t;
void getInput(){
    cin >> n >> a >> t;
    for(int i = 0; i < a; i++){
        int acc;
        cin >> acc;
        accepts.insert(acc);
    }
    for(int i = 0; i < n; i++){
        int transition;
        cin >> transition;
        for(int j = 0; j < transition; j++){
            char c;
            int to;
            cin >> c >> to;
            edges[i][c].push_back(to);
        }
    }
}

void getOutput(){
    queue<int> q;
    q.push(0);
    for(int i = 0; i < input.length(); i++){
        bool yes = false;
        char curC = input[i];
        int size = q.size();
        bool used[n];
        memset(used, false, n * sizeof(bool));
        for(int j = 0; j < size; j++){
            int cur = q.front();
            q.pop();
            if(edges.find(cur) == edges.end() || edges[cur].find(curC) == edges[cur].end()) continue;
            auto vec = edges[cur][curC];
            for(int to : vec){
                if(used[to]) continue;
                if(accepts.find(to) != accepts.end()) yes = true;
                used[to] = true;
                q.push(to);
            }
        }
        if(yes) cout << 'Y';
        else cout << 'N';
    }
    cout << endl;
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cin >> input;
    getInput();
    getOutput();
}
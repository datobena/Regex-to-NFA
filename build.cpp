#include <bits/stdc++.h>
#include <iostream>
using namespace std;
struct edge{
    int a, b;
    char c;
    bool removed;
    edge(int i, int o, char p){
        a = i;
        b = o;
        c = p;
        removed = false;
    }
    
};
struct cmp{
    bool operator()(const edge* a, const edge* b) const {
        if(a->a != b->a) return a->a < b->a;
        if(a->b != b->b) return a->b < b->b;
        return a->c < b->c;
    }
};
struct nfa{
    set<edge*, cmp> edges;
    set<edge*, cmp> epsilonEdges;
    set<int> accepting;
    int start;
    int countOfNodes;
    nfa(int c){
        start = c;
        countOfNodes = 0;
    }
};
int counter = 0;
//Functions used to convert infix string to postfix
//operations being *, ? - concat, | - or operator, () means epsilon.
map<char, int> weights{{'*', 3}, {'?', 2}, {'|', 1}};
bool isChar(char cur){
    return ((cur >= 'a' && cur <= 'z') || (cur >= '0' && cur <= '9') || cur == '$');
}
bool isGood(char cur, char next){
    return !(cur == '(' || next == ')' ||cur == '|' || next == '|' || next == '*');
    
}
bool notBad(char c){
    return c != '|' && c != '*' && c != ')';
}
vector<char> changeStr(string str){
    vector<char> res;
    char last = 0, cur = str[0];
    res.push_back(cur);
    for(int i = 1; i < str.length(); i++){
        int idx = res.size() - 1;
        if(cur == '(' && str[i] == ')'){
            res[idx] = '$';
            cur = '$';
            continue;
        }
        if(isGood(cur, str[i])) res.push_back('?');
        cur = str[i];
        res.push_back(cur);
    }
    return res;
}
vector<char> getPostfix(){
    string input;
    cin >> input;
    auto vec = changeStr(input);
    vector<char> res;
    stack<char> st;
    char cur = '!';
    for(int i = 0; i < vec.size(); i++){
        char cur = vec[i];
        if((cur >= 'a' && cur <= 'z') ||(cur >= '0' && cur <='9') || cur == '$'){
            res.push_back(cur);
        }
        else if(cur == '(' || cur == ')'){
            if(cur == '(') st.push(cur);
            else{
                while(st.top() != '('){
                    char move = st.top();
                    st.pop();
                    res.push_back(move);
                }
                st.pop();
            }
        }
        else{
            int curWeight = weights[cur];
            while(!st.empty() && st.top() != '(' && weights[st.top()] >= curWeight){
                char move = st.top();
                st.pop();
                res.push_back(move);
            }
            st.push(cur);
        }
    }
    while(!st.empty()){
        res.push_back(st.top());
        st.pop();
    }
    return res;
}

void connectOrs(stack<nfa*> &st){
    nfa *firstNfa, *secondNfa, *newNfa;
    secondNfa = st.top();
    st.pop();
    firstNfa = st.top();
    st.pop();
    newNfa = new nfa(counter);
    counter++;
    newNfa->edges.insert(firstNfa->edges.begin(), firstNfa->edges.end());
    newNfa->epsilonEdges.insert(firstNfa->epsilonEdges.begin(), firstNfa->epsilonEdges.end());
    newNfa->accepting.insert(firstNfa->accepting.begin(), firstNfa->accepting.end());
    
    newNfa->edges.insert(secondNfa->edges.begin(), secondNfa->edges.end());
    newNfa->epsilonEdges.insert(secondNfa->epsilonEdges.begin(), secondNfa->epsilonEdges.end());
    newNfa->accepting.insert(secondNfa->accepting.begin(), secondNfa->accepting.end());
    edge* first = new edge(newNfa->start, firstNfa->start, '$'), *second = new edge(newNfa->start, secondNfa->start, '$');
    newNfa->epsilonEdges.insert(first);
    newNfa->epsilonEdges.insert(second);
    st.push(newNfa);
    free(firstNfa);
    free(secondNfa);
}

void connectConcat(stack<nfa*> &st){
    nfa *firstNfa, *secondNfa;
    secondNfa = st.top();
    st.pop();
    firstNfa = st.top();
    for(int a : firstNfa->accepting){
        edge* newEdge = new edge(a, secondNfa->start, '$');
        firstNfa->epsilonEdges.insert(newEdge);
    }
    firstNfa->accepting.clear();
    firstNfa->edges.insert(secondNfa->edges.begin(), secondNfa->edges.end());
    firstNfa->epsilonEdges.insert(secondNfa->epsilonEdges.begin(), secondNfa->epsilonEdges.end());
    firstNfa->accepting.insert(secondNfa->accepting.begin(), secondNfa->accepting.end());
    free(secondNfa);
}

void connectStar(stack<nfa*> &st){
    nfa* curNfa;
    curNfa = st.top();

    curNfa->epsilonEdges.insert(new edge(counter, curNfa->start, '$'));
    curNfa->start = counter;
    counter++;
    for(int a : curNfa->accepting){
        curNfa->epsilonEdges.insert(new edge(a, curNfa->start, '$'));
    }
    curNfa->accepting.insert(curNfa->start);
}

void connectNfas(stack<nfa*> &st, char c){
    if(c == '|')
        connectOrs(st);
    else if(c == '?')
        connectConcat(st);
    else
        connectStar(st);
}

nfa* makeNFA(vector<char> &postfix){
    stack<nfa*> st;
    for(int i = 0; i < postfix.size(); i++){
        char cur = postfix[i];
        if(isChar(cur)){
            nfa *newNfa = new nfa(counter);
            counter++;
            edge *newEdge = new edge(newNfa->start, counter, cur);
            newNfa->accepting.insert(counter);
            counter++;
            if(cur != '&')
                newNfa->edges.insert(newEdge);
            else
                newNfa->epsilonEdges.insert(newEdge);
            st.push(newNfa);
        }
        else{
            connectNfas(st, cur);

        }
    }
    return st.top();
}

void addEdgesToStructures(queue<edge*>& q, map<int, set<edge*, cmp>> &fromPointEpsilon,
map<int, set<edge*, cmp>>& fromPoint, nfa* NFA){
    for(auto a : NFA->epsilonEdges) {
        fromPointEpsilon[a->a].insert(a);
        q.push(a);
    }
    for(auto a : NFA->edges) fromPoint[a->a].insert(a);
}

void removeEpsilons(queue<edge*>& q, map<int, set<edge*, cmp>> &fromPointEpsilon,
map<int, set<edge*, cmp>>& fromPoint, nfa* NFA){
    while(!q.empty()){
        edge* cur = q.front();
        q.pop();
        if(cur->removed) continue;
        cur->removed = true;
        if(NFA->accepting.find(cur->b) != NFA->accepting.end())
            NFA->accepting.insert(cur->a);
        set<edge*, cmp>& epsilonEdges = fromPointEpsilon[cur->a];
        set<edge*, cmp>& edges = fromPoint[cur->a];
        for(auto a : fromPointEpsilon[cur->b]){
            if(a->b == a->a){a->removed = true; continue;}
            if(a->removed || cur->a == a->b) continue;
            edge* newEdge = new edge(cur->a, a->b, a->c);
            auto k = epsilonEdges.insert(newEdge);
            auto x = *(k.first);
            if(!k.second) free(newEdge);
            if(!x->removed)
                q.push(x);
        }
        for(auto a : fromPoint[cur->b]){
            edge* newEdge = new edge(cur->a, a->b, a->c);
            edges.insert(newEdge);
        }
    }
}
void removeObjects(map<int, set<edge*, cmp>>& epsilons){
    for(auto a : epsilons){
        auto cur = a.second;
        for(auto b : cur){
            free(b);
        }
    }
}
void finalize(map<int, set<edge*, cmp>>& fromPoint, nfa* NFA){
    map<int, int> index;
    set<int> used;
    queue<int> q;
    set<int> newAccepting;
    index[NFA->start] = index.size();
    q.push(NFA->start);
    used.insert(NFA->start);
    if(NFA->accepting.find(NFA->start) != NFA->accepting.end())
        newAccepting.insert(0);
    NFA->start = 0;
    while(!q.empty()){
        int cur = q.front();
        q.pop();
        int real = index[cur];
        set<edge*, cmp> &edges = fromPoint[cur];
        for(auto curE : edges){
            if(used.find(curE->b) == used.end()){
                index[curE->b] = index.size();
                used.insert(curE->b);
                q.push(curE->b);
                if(NFA->accepting.find(curE->b) != NFA->accepting.end())
                    newAccepting.insert(index[curE->b]);
            }
            edge* newE = new edge(real, index[curE->b], curE->c);
            NFA->edges.insert(newE);
        }

    }
    NFA->accepting.clear();
    NFA->accepting = newAccepting;
    NFA->countOfNodes = index.size();
}

void removeEpsilonsAndFinalize(nfa* NFA){
    queue<edge*> q;
    map<int, set<edge*, cmp>> fromPointEpsilon;
    map<int, set<edge*, cmp>> fromPoint;
    addEdgesToStructures(q, fromPointEpsilon, fromPoint, NFA);
    NFA->edges.clear();
    removeEpsilons(q, fromPointEpsilon, fromPoint, NFA);
    removeObjects(fromPointEpsilon);
    finalize(fromPoint, NFA);
}

void addEdgesToMap(nfa* NFA, map<int, set<edge*, cmp>>& edges){
    for(auto a : NFA->edges) edges[a->a].insert(a);
}

void outputAnswer(nfa* NFA){
    cout << NFA->countOfNodes << ' ' << NFA->accepting.size() << ' ' << NFA->edges.size() << endl;
    auto itr = NFA->accepting.begin();
    cout << *itr;
    itr++;
    for(; itr != NFA->accepting.end(); itr++){
        int cur = *itr;
        cout << ' ' << cur;
    }
    cout << endl;
    map<int, set<edge*, cmp>> edges;
    addEdgesToMap(NFA, edges);
    for(int i = 0; i < NFA->countOfNodes; i++){
        set<edge*, cmp>& curEdges = edges[i];
        cout << curEdges.size();
        for(auto curEdge : curEdges)
            cout << ' ' << curEdge->c << ' ' << curEdge->b;
        cout << endl;
    }
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    auto postfix = getPostfix();
    nfa* last = makeNFA(postfix);
    removeEpsilonsAndFinalize(last);
    outputAnswer(last);
}
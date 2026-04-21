#include <bits/stdc++.h>
using namespace std;

// Dinic maxflow for unweighted unit capacities
struct Dinic {
    struct Edge { int to; int cap; int rev; };
    int n;
    vector<vector<Edge>> g;
    vector<int> level, it;
    Dinic(int n=0){ reset(n); }
    void reset(int n2){ n=n2; g.assign(n+1, {}); level.assign(n+1, 0); it.assign(n+1, 0);}    
    void add_edge(int u, int v, int c){
        Edge a{v,c,(int)g[v].size()};
        Edge b{u,0,(int)g[u].size()};
        g[u].push_back(a);
        g[v].push_back(b);
    }
    bool bfs(int s, int t){
        fill(level.begin(), level.end(), -1);
        queue<int> q; level[s]=0; q.push(s);
        while(!q.empty()){
            int u=q.front(); q.pop();
            for(const auto &e: g[u]) if(e.cap>0 && level[e.to]<0){ level[e.to]=level[u]+1; q.push(e.to);}        
        }
        return level[t]>=0;
    }
    int dfs(int u, int t, int f){
        if(u==t) return f;
        for(int &i=it[u]; i<(int)g[u].size(); ++i){
            Edge &e = g[u][i];
            if(e.cap>0 && level[e.to]==level[u]+1){
                int ret = dfs(e.to, t, min(f, e.cap));
                if(ret>0){ e.cap-=ret; g[e.to][e.rev].cap += ret; return ret; }
            }
        }
        return 0;
    }
    int maxflow(int s, int t){
        int flow=0, aug;
        while(bfs(s,t)){
            fill(it.begin(), it.end(), 0);
            while((aug=dfs(s,t,INT_MAX))>0) flow+=aug;
        }
        return flow;
    }
    vector<char> mincut_reachable(int s){
        vector<char> vis(n+1,false);
        queue<int> q; q.push(s); vis[s]=true;
        while(!q.empty()){
            int u=q.front(); q.pop();
            for(const auto &e: g[u]) if(e.cap>0 && !vis[e.to]){ vis[e.to]=true; q.push(e.to);}            
        }
        return vis;
    }
};

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int n, m; if(!(cin>>n>>m)) return 0;
    vector<pair<int,int>> edges; edges.reserve(m);
    for(int i=0;i<m;++i){ int a,b; cin>>a>>b; if(a==b) continue; edges.emplace_back(a,b);}    

    // Build Gomory–Hu tree for undirected unit-capacity graph
    vector<int> parent(n+1, 0);
    vector<long long> w(n+1, 0);
    for(int i=2;i<=n;++i) parent[i]=1;

    Dinic din(n);
    auto build_graph = [&](int N){
        din.reset(N);
        for(auto &e: edges){
            int u=e.first, v=e.second;
            din.add_edge(u,v,1);
            din.add_edge(v,u,1);
        }
    };

    for(int s=2; s<=n; ++s){
        int t = parent[s];
        build_graph(n);
        int flow = din.maxflow(s, t);
        vector<char> inS = din.mincut_reachable(s);
        for(int v=1; v<=n; ++v){
            if(v!=s && parent[v]==t && inS[v]) parent[v]=s;
        }
        if(parent[t]!=0 && inS[parent[t]]){
            parent[s]=parent[t];
            parent[t]=s;
            long long tmp=w[t]; w[t]=flow; flow=tmp; // swap w[t] and flow
        }
        w[s]=flow;
    }

    // Tree edges: i -- parent[i] with weight w[i]
    vector<vector<pair<int,long long>>> tree(n+1);
    for(int i=2;i<=n;++i){
        int u=i, v=parent[i]; long long ww=w[i];
        tree[u].push_back({v, ww});
        tree[v].push_back({u, ww});
    }

    // Accumulate sum over all pairs: sum over edges w * sz * (n - sz)
    long long ans=0;
    vector<int> sz(n+1,0); vector<int> par(n+1,0);
    // Root at 1 and get order
    vector<int> st; st.reserve(n);
    st.push_back(1); par[1]=0;
    vector<int> order; order.reserve(n);
    while(!st.empty()){
        int u=st.back(); st.pop_back(); order.push_back(u);
        for(auto [v,ww]: tree[u]) if(v!=par[u]){ par[v]=u; st.push_back(v);}        
    }
    for(int i=(int)order.size()-1; i>=0; --i){
        int u=order[i]; sz[u]=1;
        for(auto [v,ww]: tree[u]) if(v!=par[u]){
            sz[u]+=sz[v]; ans += ww * 1LL * sz[v] * (n - sz[v]);
        }
    }
    cout<<ans<<
;
    return 0;
}

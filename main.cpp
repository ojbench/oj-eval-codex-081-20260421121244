#include <bits/stdc++.h>
using namespace std;

struct MF {
    struct Edge { int to, cap, rev; unsigned char orig; };
    int n;
    vector<vector<Edge>> g;
    MF(int n=0){ reset(n); }
    void reset(int n2){ n=n2; g.assign(n+1, {}); }
    void add_edge(int u,int v){
        Edge a{v,1,(int)g[v].size(),1};
        Edge b{u,0,(int)g[u].size(),0};
        g[u].push_back(a); g[v].push_back(b);
        Edge c{u,1,(int)g[u].size(),1};
        Edge d{v,0,(int)g[v].size(),0};
        g[v].push_back(c); g[u].push_back(d);
    }
    void reset_caps(){
        for(int u=1; u<=n; ++u) for(auto &e: g[u]) e.cap = e.orig ? 1 : 0;
    }
    int augment_once(int s,int t, vector<pair<int,int>>& par){
        par.assign(n+1, {-1,-1});
        queue<int> q; q.push(s); par[s] = {s,-1};
        while(!q.empty()){
            int u=q.front(); q.pop();
            for(int i=0;i<(int)g[u].size();++i){
                auto &e=g[u][i];
                if(e.cap>0 && par[e.to].first==-1){
                    par[e.to] = {u,i};
                    if(e.to==t){
                        int v=t;
                        while(v!=s){ auto [pu,idx]=par[v]; Edge &ed=g[pu][idx]; Edge &rv=g[v][ed.rev]; ed.cap-=1; rv.cap+=1; v=pu; }
                        return 1;
                    }
                    q.push(e.to);
                }
            }
        }
        return 0;
    }
    int maxflow(int s,int t){
        int flow=0; vector<pair<int,int>> par;
        for(int it=0; it<3; ++it){
            int aug = augment_once(s,t,par);
            if(!aug) break; flow += aug;
        }
        return flow;
    }
    vector<char> reachable_from(int s){
        vector<char> vis(n+1,false);
        queue<int> q; q.push(s); vis[s]=true;
        while(!q.empty()){
            int u=q.front(); q.pop();
            for(auto &e: g[u]) if(e.cap>0 && !vis[e.to]){ vis[e.to]=true; q.push(e.to);}        
        }
        return vis;
    }
};

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int n, m; if(!(cin>>n>>m)) return 0;
    vector<pair<int,int>> edges; edges.reserve(m);
    for(int i=0;i<m;++i){ int a,b; cin>>a>>b; if(a==b) continue; edges.emplace_back(a,b);}    

    vector<int> parent(n+1,0); vector<int> w(n+1,0);
    for(int i=2;i<=n;++i) parent[i]=1;

    MF mf(n);
    for(auto &e: edges) mf.add_edge(e.first,e.second);

    for(int s=2; s<=n; ++s){
        int t = parent[s];
        mf.reset_caps();
        int flow = mf.maxflow(s,t);
        auto inS = mf.reachable_from(s);
        for(int v=1; v<=n; ++v){ if(v!=s && parent[v]==t && inS[v]) parent[v]=s; }
        if(parent[t]!=0 && inS[parent[t]]){
            parent[s]=parent[t];
            parent[t]=s;
            w[s]=w[t];
            w[t]=flow;
        }else{
            w[s]=flow;
        }
    }

    struct DSU{ vector<int> p, sz; DSU(int n=0){init(n);} void init(int n){p.resize(n+1); sz.assign(n+1,1); iota(p.begin(), p.end(), 0);} int find(int x){ return p[x]==x?x:p[x]=find(p[x]); } bool unite(int a,int b,long long &ans,long long ww){ a=find(a); b=find(b); if(a==b) return false; if(sz[a]<sz[b]) swap(a,b); ans += ww * 1LL * sz[a] * sz[b]; p[b]=a; sz[a]+=sz[b]; return true; } } dsu(n);
    vector<tuple<int,int,int>> tedges; tedges.reserve(n-1);
    for(int i=2;i<=n;++i) tedges.emplace_back(w[i], i, parent[i]);
    sort(tedges.begin(), tedges.end(), [](auto &A, auto &B){ return get<0>(A) > get<0>(B); });
    long long ans=0; for(auto &e: tedges){ int ww,u,v; tie(ww,u,v)=e; dsu.unite(u,v,ans,ww); }
    cout << ans << n;
    return 0;
}

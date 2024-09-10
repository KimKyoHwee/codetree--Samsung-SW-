#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cstring>
#include <utility>
using namespace std;
int moveY[4]={-1,1,0,0}, moveX[4]={0,0,-1,1}, K,M;
vector<vector<bool>> visited(5,vector<bool>(5, false));
vector<int> refill;
int refillIndex=0;
vector<vector<int>> board(5,vector<int>(5));

struct Rotate{
    int y,x, relic, angle; //중심 행, 열, 유물수, 회전각도
};
struct Compare{
    bool operator()(const Rotate& a, const Rotate& b){
        if(a.relic!=b.relic) return a.relic<b.relic;
        else if(a.angle!=b.angle) return a.angle>b.angle;
        else if(a.x!=b.x) return a.x>b.x;
        else return a.y>b.y;
    }
};
struct Cmp{
    bool operator()(pair<int, int>& a, pair<int, int>& b){
        if(a.second!=b.second) return a.second>b.second;
        else return a.first<b.first;
    }
};

Rotate makeRotate(int y, int x, int relic, int angle){
    Rotate rotate;
    rotate.y=y;
    rotate.x=x;
    rotate.relic=relic;
    rotate.angle=angle;
    return rotate;
}
Rotate makeNullRotate(){
    Rotate rotate;
    rotate.y=-1;
    return rotate;
}

int dfs(vector<vector<int>> copy, int y, int x, int index){
    int cnt=0;
    if(copy[y][x]==index) {
        cnt++;
        visited[y][x]=true;
    }
    for(int i=0;i<4;i++){
        int nextY=y+moveY[i];
        int nextX=x+moveX[i];
        if(nextY<0||nextY>4||nextX<0||nextX>4||visited[nextY][nextX]||copy[nextY][nextX]!=index)continue;
        visited[nextY][nextX]=true;
        cnt+=dfs(copy, nextY, nextX,index);
    }
    //cout<<y<<", "<<x<<"에서 "<<cnt<<"유물\n";
    return cnt;
}
Rotate findRotate(int y, int x, int angle){ //보드 돌려서 유물갯수 찾고, Rotate만들기
    vector<vector<int>> copy(5,vector<int>(5));
    //보드 복사
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++) copy[i][j]=board[i][j];
    }

    // 3x3 배열을 따로 저장
    int subBoard[3][3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            subBoard[i][j] = board[y-1+i][x-1+j];
        }
    }

    // 회전된 3x3 배열을 copy로 복사
    if (angle == 1) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                copy[y-1+i][x-1+j] = subBoard[2-j][i]; // 시계방향 90도 회전
            }
        }
    } else if (angle == 2) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                copy[y-1+i][x-1+j] = subBoard[2-i][2-j]; // 180도 회전
            }
        }
    } else if (angle == 3) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                copy[y-1+i][x-1+j] = subBoard[j][2-i]; // 시계방향 270도 회전
            }
        }
    }
    //현재 배열의 유물 수 찾기
    int relic=0;
    //fill(visited[0].begin(), visited[4].end(), false); //방문배열 초기화 다차원벡터는 fill하면안됨
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++) visited[i][j]=false;
    }
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            if(visited[i][j]) continue;
            int nowRelic=dfs(copy,i,j,copy[i][j]);
            if(nowRelic>=3) relic+=nowRelic;
        }
    }
    Rotate rotate=makeRotate(y,x,relic,angle);
    return rotate;
    
    
}
Rotate findRotate(){ //턴 시작 직후 회전해야 하는 각도와 중심점 찾기
    priority_queue<Rotate, vector<Rotate>, Compare> pq;
    for(int i=1;i<=3;i++){ //행 (3x3배열 회전시켜야 하므로 중심점은 1,1~3,3까지 총 9개)
        for(int j=1;j<=3;j++){ //열
            for(int k=1;k<=3;k++){ //회전각도
                pq.push(findRotate(i,j,k));
            }
        }
    }
    if(pq.top().relic==0) return makeNullRotate();
    return pq.top();
}

void doRotate(int y, int x, int angle){
    int subBoard[3][3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            subBoard[i][j] = board[y-1+i][x-1+j];
        }
    }
    // 회전된 3x3 배열을 copy로 복사
    if (angle == 1) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                board[y-1+i][x-1+j] = subBoard[2-j][i]; // 시계방향 90도 회전
            }
        }
    } else if (angle == 2) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                board[y-1+i][x-1+j] = subBoard[2-i][2-j]; // 180도 회전
            }
        }
    } else if (angle == 3) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                board[y-1+i][x-1+j] = subBoard[j][2-i]; // 시계방향 270도 회전
            }
        }
    }
}
void dfs2(int index, int y, int x, vector<vector<bool>>& visited, vector<pair<int, int>>& pv) {
    // 방문 처리
    visited[y][x] = true;
    pv.push_back({y, x});  // 좌표 저장

    // 상하좌우로 이동
    for (int i = 0; i < 4; i++) {
        int nextY = y + moveY[i];
        int nextX = x + moveX[i];
        // 경계 체크와 방문 체크
        if (nextY < 0 || nextY > 4 || nextX < 0 || nextX > 4 || visited[nextY][nextX] || board[nextY][nextX] != index)
            continue;
        // 재귀적으로 탐색
        dfs2(index, nextY, nextX, visited, pv);
    }
}
void printBoard(){
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++) cout<<board[i][j]<<" ";
        cout<<"\n";
    }
}
void refillRelic(priority_queue<pair<int, int>, vector<pair<int, int>>, Cmp>& pq){
    while(!pq.empty()){
        int y=pq.top().first;
        int x=pq.top().second;
        pq.pop();
        board[y][x]=refill[refillIndex];
        refillIndex++;
    }
}
int relay(){ //보드가 회전한 상태에서, 유물들을 찾고 다시 리필하고 찾고 반복
    
    int result=0; //최종적으로 얻은 유물 수
    priority_queue<pair<int, int>, vector<pair<int, int>>, Cmp> pq; //유물을 찾은 좌표 큐, 우선순위대로 리필하면됨
    vector<vector<bool>> visited(5,vector<bool>(5, false)); 
    while(1){ //유물 찾고 리필하기
        vector<pair<int, int>> pv;
        for(int i=0;i<5;i++){
            for(int j=0;j<5;j++) visited[i][j]=false;
        }
        //pq.clear(); 우선순위큐는 clear없음
        while (!pq.empty()) pq.pop();
        //현재상황에서 유물 전부 찾고 좌표들 받기
        for(int i=0;i<5;i++){
            for(int j=0;j<5;j++) {
                if(visited[i][j]) continue;
                pv.clear();
                dfs2(board[i][j],i,j,visited, pv);
                if(pv.size()>=3){
                    //cout<<i<<", "<<j<<"에서 "<<pv.size()<<" ";
                    for(int i=0;i<pv.size();i++) pq.push(pv[i]);
                }
            }
        }
        int relicCnt=pq.size(); //찾은 유물의 수
        
        result+=relicCnt; //유물 개수 결과에 추가
        if(relicCnt==0) return result;
        refillRelic(pq);

    }
    return -1;
}


int main() {
    cin>>K>>M;
    refill.resize(M+1);
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++) cin>>board[i][j];
    }
    for(int i=0;i<M;i++) cin>>refill[i];
    while(K--){
        //cout<<K<<"턴\n";
        Rotate rotate=findRotate();
        if(rotate.y==-1) break; //유물을 찾을수없음
        //cout<<rotate.y<<", "<<rotate.x<<","<<rotate.relic<<"\n";
        doRotate(rotate.y, rotate.x, rotate.angle); //찾은 좌표와 각도로 회전
        int turnResult=relay();
        cout<<turnResult<<" ";
    }
    return 0;
}
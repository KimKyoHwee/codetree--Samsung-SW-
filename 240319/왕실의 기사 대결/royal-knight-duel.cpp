#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <queue>
using namespace std;
#define MAX_L 41

//전역변수
int L,N,Q;
struct Knight{
    int r,c,h,w,k;
    int damage=0; //받은 데미지
    int damageSum=0; //누적 데미지
    bool isMove=false;
    int knightNum;
};

int chess[MAX_L][MAX_L];
vector<Knight> knightV;
vector<int> moveY={-1, 0, 1, 0};   //상우하좌  
vector<int> moveX={0, 1, 0, -1};
vector<pair<int, int>> order;  //first : 기사 id second : 이동방향
int result=0;
//전역변수 종료

bool simulation(int knightNum, int direction, int aa){
    for(int i=1;i<=N;i++){  //기사들의 이번턴 변수 초기화
        knightV[i].damage=0;
        knightV[i].isMove=false;
    }
    Knight nowKnight=knightV[knightNum];
    knightV[nowKnight.knightNum].isMove=true;  //첫기사는 움직인거 처리 미리해줌
    nowKnight.isMove=true;
    queue<Knight> q;
    q.push(nowKnight);
    while(!q.empty()){
        nowKnight=q.front(); q.pop(); //한놈씩 뽑아서
        nowKnight.r+=moveY[direction];
        nowKnight.c+=moveX[direction];
        if(nowKnight.r<1||nowKnight.r+nowKnight.h-1>L||nowKnight.c<1||nowKnight.c+nowKnight.w-1>L)
            return false;
        //기사가 체스판 밖으로 나감
        for(int i=nowKnight.r;i<nowKnight.r+nowKnight.h;i++){  //기사의 방패 범위 돌아보기
            for(int j=nowKnight.c;j<nowKnight.c+nowKnight.w;j++){
                if(chess[i][j]==2) return false; //기사가 벽에 부딪힘
                if(chess[i][j]==1){
                    //기사가 폭탄 밟으면
                    knightV[nowKnight.knightNum].damage++;
                    //cout<<aa<<"번명령: "<<nowKnight.knightNum<<"번기사 폭탄밟음"<<i<<", "<<j<<"에서\n";
                }
            }
        }
        for(int i=1;i<=N;i++){
            if(knightV[i].r>nowKnight.r+nowKnight.h-1||knightV[i].r+knightV[i].h-1<nowKnight.r) continue;
            if(knightV[i].c>nowKnight.c+nowKnight.w-1||knightV[i].c+knightV[i].w-1<nowKnight.c) continue;
            if(knightV[i].isMove||knightV[i].k<=0) continue;            
            knightV[i].isMove=true;
            q.push(knightV[i]);
        }
    }
    return true;
}

void getOrder(int knightNum, int direction, int aa){  //첫 명령 수행 함수
    Knight knight=knightV[knightNum];
    if(knight.k<=0) return;  //현재 기사 죽었으면 수행 X
    if(simulation(knightNum, direction, aa)){  //현재 기사 포함 연결된 전체 기사 움직일 수 있으면
        //cout<<aa<<"번째 명령 수행성공\n";
        for(int i=1;i<=N;i++){  
            if(knightV[i].isMove){    //이번턴에 움직인 기사들 정산  
                if(i!=knightNum){  //첫기사 예외처리
                    knightV[i].k-=knightV[i].damage; 
                    knightV[i].damageSum+=knightV[i].damage;
                }
                knightV[i].r+=moveY[direction];
                knightV[i].c+=moveX[direction];
            }
        }
    }
}

int main() {
    cin>>L>>N>>Q;
    for(int i=1;i<=L;i++){  //체스판 초기화
        for(int j=1;j<=L;j++){
            cin>>chess[i][j];
        }
    }
    Knight trashKnight;
    knightV.push_back(trashKnight); //0번기사 넣어두기
    for(int i=1;i<=N;i++){ //기사정보 초기화
        Knight addKnight;
        cin>>addKnight.r>>addKnight.c>>addKnight.h>>addKnight.w>>addKnight.k;
        addKnight.knightNum=i;
        knightV.push_back(addKnight);
    } 
    for(int i=0;i<Q;i++) {  //명령 수행
        int a,b;
        cin>>a>>b;
        getOrder(a,b,i+1);
    }
    for(int i=1;i<=N;i++){
        if(knightV[i].k>0) {
            result+=knightV[i].damageSum;
        }
    }
    cout<<result;
    return 0;
}
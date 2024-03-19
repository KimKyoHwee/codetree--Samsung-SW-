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

bool simulation(int knightNum, int direction){
    for(int i=1;i<=N;i++){  //기사들의 이번턴 변수 초기화
        knightV[i].damage=0;
        knightV[i].isMove=false;
    }
    Knight nowKnight=knightV[knightNum];
    nowKnight.isMove=true;  //첫기사는 움직인거 처리 미리해줌
    queue<Knight> q;
    q.push(nowKnight);
    while(!q.empty()){
        nowKnight=q.front(); q.pop(); //한놈씩 뽑아서
        nowKnight.r+=moveY[direction];
        nowKnight.c+=moveX[direction];
        if(nowKnight.r<1||nowKnight.r+nowKnight.h>=L||nowKnight.c<1||nowKnight.c+nowKnight.w>=L)
            return false;
        //기사가 체스판 밖으로 나감
        for(int i=nowKnight.r;i<nowKnight.r+nowKnight.h;i++){  //기사의 방패 범위 돌아보기
            for(int j=nowKnight.c;j<nowKnight.c+nowKnight.w;j++){
                if(chess[i][j]==2) return false; //기사가 벽에 부딪힘
                if(chess[i][j]==1){
                    //기사가 폭탄 밟으면
                    knightV[nowKnight.knightNum].damage++;
                }
            }
        }
        for(int i=1;i<=N;i++){
            if(knightV[i].r<nowKnight.r||knightV[i].r>=nowKnight.r+nowKnight.h) continue;
            if(knightV[i].c<nowKnight.c||knightV[i].c>=nowKnight.c+nowKnight.w) continue;
            if(knightV[i].isMove||knightV[i].k<=0) continue;            
            knightV[i].isMove=true;
            q.push(knightV[i]);
        }
    }
    return true;
}

void getOrder(int knightNum, int direction){  //첫 명령 수행 함수
    Knight knight=knightV[knightNum];
    if(knight.k<=0) return;  //현재 기사 죽었으면 수행 X
    if(simulation(knightNum, direction)){  //현재 기사 포함 연결된 전체 기사 움직일 수 있으면
        for(int i=1;i<=N;i++){  
            if(knightV[i].isMove){    //이번턴에 움직인 기사들 정산  
                if(i!=knightNum){  //첫기사 예외처리
                    knightV[i].k-=knightV[i].damage; 
                    knightV[i].damageSum+=knightV[i].damage;
                }
                knightV[i].r+=moveY[direction];
                knightV[i].c+=moveX[direction];
                knightNum=i;
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
    Knight addKnight;
    knightV.push_back(addKnight); //0번기사 넣어두기
    for(int i=0;i<N;i++){ //기사정보 초기화
        cin>>addKnight.r>>addKnight.c>>addKnight.h>>addKnight.w>>addKnight.k;
    } 
    pair<int, int> addOrder;
    for(int i=0;i<Q;i++) {  //명령 초기화
        cin>>addOrder.first>>addOrder.second;
        order.push_back(addOrder);
    }

    for(int i=0;i<Q;i++) getOrder(order[i].first, order[i].second);
    for(int i=1;i<=N;i++){
        if(knightV[i].k>0) {
            result+=knightV[i].damageSum;
        }
    }
    cout<<result;
    return 0;
}
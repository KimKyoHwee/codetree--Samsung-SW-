#include <iostream>
#include <utility>
#include <queue>
#include <vector>
using namespace std;
//전역변수 선언
int N, M, K;
int board[11][11];
int lastAttack[11][11]; //포탑마다 가장 최근에 공격한 턴
int rowColSum[11][11];   //각 행과 열의 합 (i행j열의 값은 i+j)
int moveY[4] = { 0,1,0,-1 };  //우하좌상
int moveX[4] = { 1,0,-1,0 };
int bombY[9] = { -1,-1,-1,0,0,0,1,1,1 };
int bombX[9] = { -1,0,1,-1,0,1,-1,0,1 };
pair<int, int> attacker;
pair<int, int> target;
bool attackInTurn[11][11] = { false };  //이번턴에 공격과 관련이 있었는지
int backX[11][11]; //역추적
int backY[11][11]; //역추적
bool visited[11][11];
bool attacked[11][11]; //이번턴에 공격과 연관이 있는지

bool checkAliveMan() {
    int cnt = 0;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            if (board[i][j] > 0) cnt++;
        }
    }
    return cnt ==1;
}

void chooseAttacker(int turn) { //공격자 선택 (공격력 가장 낮은 포탑)
    int minStat = 2100000000;
    bool flag = false; //minStat 최신화 될때마다 false로 리셋, 같은 공격력을 가지는 포탑이 있는지 체크
    for (int i = 1; i <= N; i++) { //일단 최고 공격력만 체크(겹치는거 있는지랑)
        for (int j = 1; j <= M; j++) {
            if (board[i][j] == 0) continue; //죽은 포탑은 건너뛰기
            if (minStat == board[i][j]) flag = true;
            else if (minStat > board[i][j]) {
                minStat = board[i][j];
                flag = false;
                attacker.first = i; attacker.second = j;
            }
        }
    }
    if (flag) {  //공격력이 겹치는 포탑이 있는 경우
        vector<pair<int, int>> vec;  //공격력 겹치는 포탑들 좌표 벡터
        for (int i = 1; i <= N; i++) {  //공격력 겹치는 포탑들 벡터에 넣고
            for (int j = 1; j <= M; j++) {
                if (board[i][j] == 0) continue; //죽은 포탑은 건너뛰기
                if (minStat == board[i][j]) vec.push_back({ i,j });
            }
        }
        int minAttackTurn = 0;  //가장 최근에 공격한거 찾아야 하므로 공격한 턴이 가장 큰거 찾아야함
        bool flag2 = false; //maxAttackTurn 같은거 있으면 true
        for (int i = 0; i < vec.size(); i++) {
            if (lastAttack[vec[i].first][vec[i].second] == minAttackTurn) {
                flag2 = true;
            }
            else if (lastAttack[vec[i].first][vec[i].second] > minAttackTurn) {  //가장 최근에 공격한 포탑을 공격자로 선정
                minAttackTurn = lastAttack[vec[i].first][vec[i].second];
                flag2 = false;
                attacker.first = vec[i].first;
                attacker.second = vec[i].second;
            }
        }
        if (flag2) {  //가장 최근에 공격한 포탑이 겹친다면
            vector<pair<int, int>> vec2;
            for (int i = 0; i < vec.size(); i++) { //겹치는 포탑들 좌표 vec2에 넣고
                if (minAttackTurn == lastAttack[vec[i].first][vec[i].second]) vec2.push_back({ vec[i].first, vec[i].second });
            }
            vec.clear();
            int maxRowColSum = 0;
            bool flag3 = false;
            for (int i = 0; i < vec2.size(); i++) {
                if (rowColSum[vec2[i].first][vec2[i].second] == maxRowColSum) flag3 = true;
                else if (rowColSum[vec2[i].first][vec2[i].second] > maxRowColSum) {
                    maxRowColSum = rowColSum[vec2[i].first][vec2[i].second];
                    flag3 = false;
                    attacker.first = vec2[i].first;
                    attacker.second = vec2[i].second;
                }
            }
            if (flag3) { //행과 열의 값까지 겹치면
                vector<pair<int, int>> vec3;
                for (int i = 0; i < vec2.size(); i++) {
                    if (rowColSum[vec2[i].first][vec2[i].second] == maxRowColSum) vec3.push_back({ vec2[i].first, vec2[i].second });
                }
                vec2.clear();
                int maxCol = 0;
                for (int i = 0; i < vec3.size(); i++) {
                    if (maxCol < vec3[i].first) {
                        maxCol = vec3[i].first;
                        attacker.first = vec3[i].first;
                        attacker.second = vec3[i].second;
                    }
                }
            }
        }
    }
    lastAttack[attacker.first][attacker.second] = turn;  //최종 선정된 공격자의 최근 공격턴 최신화 후 종료
    board[attacker.first][attacker.second] += (N + M); //공격력 버프
}

void chooseTarget(int turn) { //공격자 선택 (공격력 가장 낮은 포탑)
    int maxStat = 0;
    bool flag = false; //minStat 최신화 될때마다 false로 리셋, 같은 공격력을 가지는 포탑이 있는지 체크
    for (int i = 1; i <= N; i++) { //일단 최고 공격력만 체크(겹치는거 있는지랑)
        for (int j = 1; j <= M; j++) {
            if (board[i][j] == 0) continue; //죽은 포탑은 건너뛰기
            if (maxStat == board[i][j]) flag = true;
            else if (maxStat < board[i][j]) {
                maxStat = board[i][j];
                flag = false;
                target.first = i; target.second = j;
            }
        }
    }
    if (flag) {  //공격력이 겹치는 포탑이 있는 경우
        vector<pair<int, int>> vec;  //공격력 겹치는 포탑들 좌표 벡터
        for (int i = 1; i <= N; i++) {  //공격력 겹치는 포탑들 벡터에 넣고
            for (int j = 1; j <= M; j++) {
                if (board[i][j] == 0) continue; //죽은 포탑은 건너뛰기
                if (maxStat == board[i][j]) vec.push_back({ i,j });
            }
        }
        int maxAttackTurn2 = 2100000000;  //공격한지 가장 오래된 포탑 구해야 하므로 공격턴 가장 작은 포탑 구하기
        bool flag2 = false; //maxAttackTurn 같은거 있으면 true
        for (int i = 0; i < vec.size(); i++) {
            if (lastAttack[vec[i].first][vec[i].second] == maxAttackTurn2) {
                flag2 = true;
            }
            else if (lastAttack[vec[i].first][vec[i].second] < maxAttackTurn2) {  //가장 최근에 공격한 포탑을 공격자로 선정
                maxAttackTurn2 = lastAttack[vec[i].first][vec[i].second];
                flag2 = false;
                target.first = vec[i].first;
                target.second = vec[i].second;
            }
        }
        if (flag2) {  //가장 최근에 공격한 포탑이 겹친다면
            vector<pair<int, int>> vec2;
            for (int i = 0; i < vec.size(); i++) { //겹치는 포탑들 좌표 vec2에 넣고
                if (maxAttackTurn2 == lastAttack[vec[i].first][vec[i].second]) vec2.push_back({ vec[i].first, vec[i].second });
            }
            vec.clear();
            int minRowColSum = 2100000000;  //가장 작은거 찾기
            bool flag3 = false;
            for (int i = 0; i < vec2.size(); i++) {
                if (rowColSum[vec2[i].first][vec2[i].second] == minRowColSum) flag3 = true;
                else if (rowColSum[vec2[i].first][vec2[i].second] < minRowColSum) {
                    minRowColSum = rowColSum[vec2[i].first][vec2[i].second];
                    flag3 = false;
                    target.first = vec2[i].first;
                    target.second = vec2[i].second;
                }
            }
            if (flag3) { //행과 열의 값까지 겹치면
                vector<pair<int, int>> vec3;
                for (int i = 0; i < vec2.size(); i++) {
                    if (rowColSum[vec2[i].first][vec2[i].second] == minRowColSum) vec3.push_back({ vec2[i].first, vec2[i].second });
                }
                vec2.clear();
                int minCol = 2100000000;
                for (int i = 0; i < vec3.size(); i++) {
                    if (minCol > vec3[i].first) {
                        minCol = vec3[i].first;
                        target.first = vec3[i].first;
                        target.second = vec3[i].second;
                    }
                }
            }
        }
    }
    //lastAttack[target.first][attacker.second] = turn;  //최종 선정된 공격자의 최근 공격턴 최신화 후 종료
    //board[attacker.first][attacker.second] += (N + M); //공격력 버프
}
bool laserAttack() {
    bool flag = false; //레이저 공격 가능한지
    queue<pair<int, int>> q;
    q.push(attacker);
    visited[attacker.first][attacker.second] = true;
    while (!q.empty()) {
        int y, x;
        pair<int, int> p = q.front(); q.pop();
        y = p.first; x = p.second;
        if (y == target.first && x == target.second) {  //목적지도착
            flag = true;
            break;
        }
 
        for (int i = 0; i < 4; i++) {
            int ny = (y + moveY[i] + N) % N;
            int nx = (x + moveX[i] + M) % M;

            if (visited[ny][nx]) continue;  //이미 방문한곳 안감
            if (board[ny][nx] == 0) continue; //죽은포탑 못지나감
            
            visited[ny][nx] = true;
            backY[ny][nx] = y;
            backX[ny][nx] = x;
            q.push({ ny, nx });
        }
    }
    if (flag) {//만약 도달했으면
        attacked[attacker.first][attacker.second] = true;
        int y, x;
        y = target.first; x = target.second;
        board[y][x] -= board[attacker.first][attacker.second];  //포탑공격
        if (board[y][x] < 0) board[y][x] = 0;
        attacked[y][x] = true;
        attacked[attacker.first][attacker.second] = true;
        while (1) {  //역추적 시작
            int ny = backY[y][x];
            int nx = backX[y][x];
            if (ny == attacker.first && nx == attacker.second) {  //역추적종료
                break;
            }
            board[ny][nx] -= (board[attacker.first][attacker.second] / 2);
            if (board[ny][nx] < 0) board[ny][nx] = 0;
            attacked[ny][nx] = true;
        }
    }
    return flag;
}
void bombAttack() {
    int y, x;
    y = target.first; x = target.second;
    attacked[attacker.first][attacker.second] = true;
    attacked[y][x] = true;
    board[y][x] -= board[attacker.first][attacker.second];
    if (board[y][x] < 0) board[y][x] = 0;
    for (int i = 0; i < 9; i++) {
        int ny = y + bombY[i]; int nx = x + bombX[i];
        if (ny == y && nx == x) continue; //공격받는 포탑 제외
        board[ny][nx] -= (board[attacker.first][attacker.second]) / 2;
        attacked[ny][nx] = true;
    }
}
void attack() {
    for (int i = 1; i <= N; i++) {  //초기화
        for (int j = 1; j <= M; j++) {
            visited[i][j] = false;
            attacked[i][j] = false;
        }
    }
    if (!laserAttack()) {
        for (int i = 1; i <= N; i++) {  //초기화
            for (int j = 1; j <= M; j++) {
                visited[i][j] = false;
                attacked[i][j] = false;
            }
        }
        bombAttack();
    }
}
void last() {
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            if (!attacked[i][j]&&board[i][j]!=0) {
                board[i][j]++;
            }
        }
    }
}
void startTurn(int turn) {
    chooseAttacker(turn);
    //cout << turn << "번째 공격자 " << attacker.first << ", " << attacker.second << "\n";
    chooseTarget(turn);
    //cout << turn << "번째 공격자 " << target.first << ", " << target.second << "\n";
    attack();
    last();
}

int findStrongMan() {
    int maxAttack = 0;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            if (maxAttack < board[i][j]) maxAttack = board[i][j];
        }
    }
    return maxAttack;
}

int main() {
    cin >> N >> M >> K;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            cin >> board[i][j];
            rowColSum[i][j] = i + j;
        }
    }
    for (int i = 1; i <= K; i++) {
        startTurn(i);
        /*
        for (int j = 1; j <= N; j++) {
            for (int k = 1; k <= M; k++) {
                cout << board[j][k] << " ";
            }
            cout << "\n";
        }
        */
        if (checkAliveMan) break;  //살아남은 포탑 1개면 종료
    }
    cout << findStrongMan();
    return 0;
}
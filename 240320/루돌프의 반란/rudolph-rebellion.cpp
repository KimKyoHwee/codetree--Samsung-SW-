#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
using namespace std;
//전역변수
int board[51][51]; //게임판
int N, M, P, C, D;
int santaY[31];
int santaX[31];
int santaScore[31]; //각 산타의 점수
int rudolfY, rudolfX;
int stun[31];  //산타가 몇번째 턴까지 스턴인지
bool dead[31] = { false }; //산타가 아웃됐는지
int dist[31]; //루돌프와 각 산타의 거리
int nearSanta;  //가장 가까운 산타 번호
int minDistance;  //현재까지 가장 가까웠던 산타와의 거리
int moveY[8] = { -1, 0, 1, 0, -1, 1, -1, 1 }; //상우하좌 대각(우상 우하 좌상 좌하)
int moveX[8] = { 0, 1, 0, -1, 1, 1, -1, -1 };
//전역변수 종료
void calculateDistance() {  //각 산타와 루돌프사이의 거리 계산
    for (int i = 1; i <= P; i++) {
        if (dead[i]) continue;
        dist[i] = pow((santaY[i] - rudolfY), 2) + pow((santaX[i] - rudolfX), 2);
    }
}
void chooseNearSanta() { //가장 가까운 산타 찾기
    minDistance = 2147483640;
    for (int i = 1; i <= P; i++) {
        if (dead[i]) continue; //죽은 산타는 배제
        if (minDistance >= dist[i]) {  //거리 최신화 가능성 생김
            if (minDistance == dist[i]) { //거리가 같을때는 Y값과 X값 큰것부터
                if (santaY[nearSanta] == santaY[i]) { //Y값같으면 X값으로 결정
                    if (santaX[nearSanta] > santaX[i]) continue;
                    else {
                        nearSanta = i;
                        continue;
                    }
                }
                else if (santaY[nearSanta] > santaY[i]) continue;
                else {
                    nearSanta = i;
                    continue;
                }
            }
            else {
                nearSanta = i;
                minDistance = dist[i];
                continue;
            }
        }
    }
}
int chooseRudolfDirection() { //루돌프가 가장 가까운 산타에게 갈 방향 정하기
    int directionY = santaY[nearSanta] - rudolfY;
    int directionX = santaX[nearSanta] - rudolfX;
    if (directionY > 0) {
        if (directionX == 0) return 2; //하
        if (directionX > 0) return 5; //우하
        else return 7; //좌하
    }
    else if (directionY < 0) {
        if (directionX == 0) return 0; //상
        if (directionX > 0) return 4; //우상
        else return 6; //좌하
    }
    else {  //Y값이 같을때
        if (directionX > 0) return 1; //우
        else return 3; //좌
    }
}
int chooseSantaDirection(int santaNum) {  //산타의 움직일 방향 정하기
            //-1 : 움직임 X 0123 : 상우하좌
    //움직일수 있는 칸 내에서 루돌프랑 가장 가까워지는 칸, 가까워질 칸 없으면 -1리턴
    int santaDirection = -1;
    int nowDistance = pow((santaY[santaNum] - rudolfY), 2) + pow((santaX[santaNum] - rudolfX), 2);
    for (int i = 0; i <= 3; i++) {
        int ny = santaY[santaNum] + moveY[i];
        int nx = santaX[santaNum] + moveX[i];
        if (board[ny][nx] == 1) continue; //가려는 곳에 산타 있으면 다음으로 패스
        int nextDistance = pow((ny - rudolfY), 2) + pow((nx - rudolfX), 2);
        if (nextDistance < nowDistance) {
            nowDistance = nextDistance;
            santaDirection = i;
        }
    }
    return santaDirection;
}
int findSantaByBoardLocation(int y, int x, int prevSanta) {
    for (int i = 1; i <= P; i++) {  //그 자리에 있던 산타 찾기
        if (i == prevSanta) continue; //밀려난 산타를 찾는게 아니라 그자리에 있던 산타를 찾는거
        if (y == santaY[i] && x == santaX[i]) return i;
    }
    return -1;  //못찾음
}
void santaBoomSanta(int prevSanta, int direction) { //산타끼리의 상호작용(prev산타가 날라와서 박음)
    //cout << "산타끼리 충돌 발생\n";
    int nextSanta = findSantaByBoardLocation(santaY[prevSanta], santaX[prevSanta], prevSanta);
    while (1) {  //계속되는 산타의 상호작용
        santaY[nextSanta] += moveY[direction];
        santaX[nextSanta] += moveX[direction]; //밀린 산타가 움직일 곳
        if (santaY[nextSanta]<1 || santaY[nextSanta]>N ||
            santaX[nextSanta]<1 || santaX[nextSanta]>N) {  //밀린 산타가 죽으면
            dead[nextSanta] = true;
            break;
        }
        else {  //밀린 산타가 안죽으면
            if (board[santaY[nextSanta]][santaX[nextSanta]] == 1) { //밀린곳에 또 산타가있으면
                nextSanta = findSantaByBoardLocation(santaY[nextSanta], santaX[nextSanta], nextSanta);
                continue;
            }
            else {
                board[santaY[nextSanta]][santaX[nextSanta]] = 1;
                break;
            }
        }
    }
    return;
}
void haveBoom(int direction, int power, int santaNum, int turn) { //충돌
    stun[santaNum] = turn + 1; //다음턴까지 스턴
    santaScore[santaNum] += power; //박은 산타 점수 추가
    if(board[santaY[santaNum]][santaX[santaNum]]!=2)board[santaY[santaNum]][santaX[santaNum]] = 0; //기존의 산타자리 비워주고
    santaY[santaNum] += (power * moveY[direction]);
    santaX[santaNum] += (power * moveX[direction]);
    //산타 날라감
    //cout << santaNum << "번째 산타 루돌프 박고 날라가서 " << santaY[santaNum] << ", " << santaX[santaNum] << "으로감\n";
    if (santaY[santaNum]<1 || santaY[santaNum]>N || santaX[santaNum] < 1 || santaX[santaNum]>N) {
        dead[santaNum] = true;
        return;
    }
    //날라간 산타가 죽음 이하 코드들은 산타가 안죽은 경우
    if (board[santaY[santaNum]][santaX[santaNum]] == 1) { //날라간 자리에 산타가 있는 경우
        santaBoomSanta(santaNum, direction);
    }
    board[santaY[santaNum]][santaX[santaNum]] = 1;
    return;
}
void santaMove(int turn) {
    for (int i = 1; i <= P; i++) { //각 산타들 움직임 시작
        if (dead[i] || (stun[i] >= turn))continue; //죽은산타나 스턴걸려있는 산타면 넘어가기  
        int santaDirection = chooseSantaDirection(i); //산타 움직이기
        if (santaDirection == -1)continue; //움직일 곳 없으면 걍 넘어가기
        board[santaY[i]][santaX[i]] = 0; //기존 산타 자리 비워주고
        santaY[i] += moveY[santaDirection];
        santaX[i] += moveX[santaDirection];
        //산타 이동
        if (board[santaY[i]][santaX[i]] == 2) {  //루돌프한테 산타가 박음
            //cout << "루돌프 산타 충돌\n";
            int boomDirection;  //날라가는 방향
            if (santaDirection <= 1) boomDirection = santaDirection + 2;
            else boomDirection = santaDirection- 2;
            haveBoom(boomDirection, D, i, turn);
        }
        else board[santaY[i]][santaX[i]] = 1;
        //산타 이동 완료
    }
}
void rudolfMove(int turn) {
    calculateDistance();
    chooseNearSanta();
    //cout << turn << "번째에서 가장 가까운 산타는 : " << nearSanta << "번째\n";
    //cout << turn << "번째 턴 시작할때 산타 사망정보 : ";
    //for (int i = 1; i <= P; i++) cout << dead[i] << ", ";
    //cout << "\n";
    int rudolfDirection = chooseRudolfDirection();
    //루돌프가 nearSatna에게 갈 rudolfDirection(방향) 정함
    board[rudolfY][rudolfX] = 0;
    rudolfY += moveY[rudolfDirection];
    rudolfX += moveX[rudolfDirection];
    board[rudolfY][rudolfX] = 2;
    //루돌프가 산타에게 다가감
    //cout << turn << "번째 턴 : " << rudolfY << ", " << rudolfX << "에 루돌프 움직임\n";
    if ((santaY[nearSanta] == rudolfY) && (santaX[nearSanta] == rudolfX)) { //산타랑 박으면
        haveBoom(rudolfDirection, C, nearSanta, turn);
    }
    santaMove(turn); //산타이동 시작
    for (int i = 1; i <= P; i++) { //살아있는 산타 전부 1점추가
        if (!dead[i]) santaScore[i]++;
    }
    /*
    cout << turn << "번째 종료시점 산타 위치 : ";
    for (int i = 1; i <= P; i++) cout << i << "번산타 : " << santaY[i] << ", " << santaX[i]<<"   ";
    cout << turn << "번턴 끝난 후 체스판 : \n";
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) cout << board[i][j] << " ";
        cout << "\n";
    }
    cout << "\n";
    */
}

int main() {
    cin >> N >> M >> P >> C >> D;
    cin >> rudolfY >> rudolfX;
    board[rudolfY][rudolfX] = 2;
    for (int i = 1; i <= P; i++) {
        int p, y, x;
        cin >> p >> y >> x;
        santaY[p] = y; santaX[p] = x;
        board[y][x] = 1;
    }
    for (int i = 1; i <= M; i++) {
        bool endGame = true;  //만약 모든 산타 죽었으면 게임 종료
        for (int i = 1; i <= P; i++) {
            if (!dead[i]) {
                endGame = false;
                break;
            }
        }
        if (endGame) break;
        rudolfMove(i);
        /*
        cout << i << "번째 턴 : ";
        for (int j = 1; j <= P; j++) {
            cout << santaScore[j] << ", ";
        }
        cout << "\n";
        */
    }
    for (int i = 1; i <= P; i++) {
        cout << santaScore[i] << " ";
    }
    return 0;
}
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;
//전역변수 
int moveY[4] = { -1,1,0,0 }; //상하좌우
int moveX[4] = { 0,0,-1,1 };
int board[11][11];  //게임판
int copyBoard[11][11];  //게임판 돌린거 임시저장 위한 카피보드
int runnerY[11];  //러너 현재 좌표
int runnerX[11];
int exitY, exitX;  //출구 좌표
int N, M, K;
int turn=0;
bool escape[11]={false}; //탈출한 러너들
int distancePerTurn[11]; //각 턴마다 측정되는 러너와 출구와의 거리
int nearRunner; //각 턴마다 출구와 가장 가까운 러너
int minDistance; //턴마다 21억으로 초기화되는 턴변수(최소거리)
int movingSum = 0; //전체 러너가 움직인 합
bool isMoved[11] = { false }; //각 턴마다 움직였는지(움직인 거리 추가할때 판단용)
int rectangleN;  //매턴마다 잡히는 가장 작은 정사각형의 변의 길이
int R,C;  //매턴마다 잡히는 가장 작은 정사각형의 시작점
bool isChanged[11] = { false };
bool checkIfAllRunnerEscape() {
	for (int i = 1; i <= M; i++)
		if (!escape[i]) return false;
	return true;
}
int calculateDistance(int ay, int ax, int by, int bx) {
	int y = ay - by;
	int x = ax - bx;
	if (y < 0) y = y * (-1);
	if (x < 0) x = x * (-1);
	return(y + x);
}
int getRectangleDistancePerTurn() {  //턴마다 정사각형의 변 길이 계산
	
	int a = runnerY[nearRunner] - exitY;
	if (a < 0) a *= (-1);
	int b = runnerX[nearRunner] - exitX;
	if (b < 0) b *= (-1);
	if (a > b) return a+1;
	else return b+1;
	
}
void checkDistancePerTurn() { //각 턴마다 모든 러너와 출구간의 거리 측정+가장 가까운놈 정하기
	minDistance = 2147483600;
	for (int i = 1; i <= M; i++) {
		if (escape[i]) continue; //탈출 성공한 러너는 제외
		distancePerTurn[i] = calculateDistance(runnerY[i], runnerX[i], exitY, exitX);
		if (minDistance >= distancePerTurn[i]) {
			if (minDistance == distancePerTurn[i]) {//같으면 y값 작은거 이것도 같으면 x값 작은거
				if (runnerY[nearRunner] > runnerY[i]) {
					nearRunner = i;
					continue;
				}
				else if (runnerY[nearRunner] == runnerY[i]) {
					if (runnerX[nearRunner] > runnerX[i]) {
						nearRunner = i;

						continue;
					}
					else continue;
				}
				else continue;
			} 
			else {  //nearRunner수정+ minDistance수정
				nearRunner = i;
				minDistance = distancePerTurn[i];
	
			}
		}
	}
}

void turnRunning() {  //턴 시작
	fill(isMoved, &isMoved[M + 1], false);
	for (int i = 1; i <= M; i++) distancePerTurn[i] = calculateDistance(runnerY[i], runnerX[i], exitY, exitX);
	//checkDistancePerTurn();  //먼저 각 러너들과 출구와의 거리 확인하고
	for (int i = 1; i <= M; i++) {  //각 러너들 이동
		int y = runnerY[i]; int x = runnerX[i];
		for (int j = 0; j < 4; j++) {
			int ny = y + moveY[j];
			int nx = x + moveX[j];
			if (board[ny][nx] >= 1 || ny<1 || ny>N || nx<1 || nx>N) continue;  //못가는 곳으로 가는 가능성은 폐기
			if (distancePerTurn[i] > calculateDistance(ny, nx, exitY, exitX)) {  //이동한 곳이 출구랑 더 가까운 곳이면 이동 (상하좌우순서)
				distancePerTurn[i] = calculateDistance(ny, nx, exitY, exitX);  //러너와 출구 거리 최신화
				runnerY[i] = ny; runnerX[i] = nx; //러너 이동
				if (!isMoved[i]) {  //한칸이라도 움직일 수 있으면 움직인거리+1
					movingSum++;
					isMoved[i] = true;
				}
			}
		}
	}
	//러너들 이동 완료
	for (int i = 1; i <= M; i++) {  //탈출 성공한 러너들 처리
		if (runnerY[i] == exitY && runnerX[i] == exitX) {
			escape[i] = true;
		}
	}
	checkDistancePerTurn();  //러너들 이동하고 한번 더 거리 확인
	rectangleN = getRectangleDistancePerTurn(); //변의 길이 구하고
	//가장 가까운 러너와 출구 중에서 더 큰 y값과 x값으로부터 변의 길이를 뺀 것이 사각형의 시작점 
	if (runnerY[nearRunner] > exitY) R = runnerY[nearRunner] - rectangleN+1;
	else R=exitY - rectangleN+1;
	if (runnerX[nearRunner] > exitX) C = runnerX[nearRunner] - rectangleN+1;
	else C = exitX - rectangleN+1;
	if (R < 1) R = 1;  
	if (C < 1) C = 1; //꼭짓점이 보드 밖으로 나가면 1,1로 보내기
	//미로 돌리기 시작
	memmove(copyBoard, board, sizeof(board));
	fill(isChanged, &isChanged[M + 1], false);
	for (int i = 1; i <= M; i++) { //정사각형 안의 러너들 위치 바꾸기
		if (isChanged[i]) continue;
		if (runnerY[i] >= R && runnerY[i] < R + rectangleN && runnerX[i] >= C && runnerX[i] < C + rectangleN) {
			int by = runnerY[i] - R + 1, bx = runnerX[i] - C + 1;  //정사각형 내에서 1~rectangle행과 열로 계산하기
			int nx = rectangleN - by + 1, ny = bx;
			int newY = R + ny - 1, newX = C + nx - 1;
			runnerY[i] = newY; runnerX[i] = newX;
			isChanged[i] = true;
	
		}
	}
	for (int i = R; i < R + rectangleN; i++) {
		for (int j = C; j < C + rectangleN; j++) {
			int newY = j - C, newX = rectangleN - 1 - (i - R); // 회전된 새 위치 계산
			copyBoard[newY + R][newX + C] = board[i][j];
			if (board[i][j] >= 1) copyBoard[newY+R][newX+C]--; //벽 1감소
			if (board[i][j] == -1) { //출구위치 저장
				exitY = newY+R;
				exitX = newX+C;
			}
		}
	}
	memmove(board, copyBoard, sizeof(board));
	
}


int main() {
	cin >> N >> M >> K;
	for (int i = 1; i <= N; i++) {  //게임판 입력
		for (int j = 1; j <= N; j++) {
			cin >> board[i][j];
		}
	}
	for (int i = 1; i <= M; i++) {  //러너 위치 입력
		cin >> runnerY[i] >> runnerX[i];
	}
	cin >> exitY >> exitX; //출구위치 입력
	board[exitY][exitX] = -1; //보드에 출구는 -1로 입력
	checkDistancePerTurn();
	for (int i = 1; i <= K; i++) {
		if (checkIfAllRunnerEscape()) break; //모든 러너 탈출시 턴 종료
		turn++; //턴 시작
		turnRunning();
	}
	cout << movingSum << "\n" << exitY << " " << exitX;
}
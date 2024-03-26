#include <iostream>
#include <vector>
#include <cstring>
using namespace std;
int N, M, K;
int board[11][11];
int runnerY[11], runnerX[11];
int exitY, exitX;
int result = 0;
bool escape[11] = { false };
int dist[11];  //현재 각 러너와 출구와의 거리
int moveY[4] = { -1, 1, 0, 0 };// 상하좌우
int moveX[4] = { 0,0,-1,1 };
int R, C, rectangleN;
//bool isMoved[11] = { false };

bool allRunnerEscape() {
	for (int i = 1; i <= M; i++) {
		if (!escape[i]) return false;
	}
	return true;
}
int calculateDistance(int y1, int x1, int y2, int x2) { //두 점의 거리계산
	int sum1 = y1 - y2;
	int sum2 = x1 - x2;
	if (sum1 < 0) sum1 *= (-1);
	if (sum2 < 0) sum2 *= (-1);
	return sum1 + sum2;
}
void calculateAllRunnersDistance() {  //지금 시점의 모든 러너와 출구의 거리 계산
	for (int i = 1; i <= M; i++) {
		dist[i] = calculateDistance(runnerY[i], runnerX[i], exitY, exitX);
	}
}
void moveAllRunners() {
	//fill(isMoved, &isMoved[M + 1], false);
	//모든러너들 상하좌우로 움직여보면서 거리 단축되면 옮기기(한번에 러너의 위치 옮겨버리면 2번 이동하는 경우 생기므로 임시값들로 저장하면서 가야됨)
	for (int i = 1; i <= M; i++) {  
		if (escape[i]) continue;
		bool flag = false;  //이번턴에 움직였나
		for (int j = 0; j < 4; j++) {
			int ny = runnerY[i] + moveY[j], nx = runnerX[i] + moveX[j];
			if (board[ny][nx] >= 1) continue;  //이동하려는곳에 벽있으면 못감
			if (ny<1 || ny>N || nx<1 || nx>N) continue; //게임판 밖으로 못감
			int nextDistance = calculateDistance(ny, nx, exitY, exitX);
			if (dist[i] > nextDistance) {  //이동
				flag = true;
				dist[i] = nextDistance;
				runnerY[i] = ny;
				runnerX[i] = nx;
				if (ny == exitY && nx == exitX) escape[i] = true;
				break;
			}

		}
		if(flag) result++;
	}
}
bool RectangleContainPoint(int y, int x, int r, int c, int n) { //(y,x)가 (r,c)에서 시작하는 nxn 정사각형 안에 들어있는가
	if (y >= r && y <= (r + n-1) && x >= c && x <= (c + n-1)) return true;
	else return false;
}
void getRectangle() {
	for (int i = 2; i <= N; i++) {  //정사각형 한 변의 길이 2~N까지
		for (int j = 1; j <= N - i + 1; j++) {
			for (int k = 1; k <= N - i + 1; k++) {
				if (RectangleContainPoint(exitY, exitX, j, k, i)) { //만들어진 사각형이 출구를 포함하고 있고
					for (int l = 1; l <= M; l++) { //러너 한명이라도 포함하고 있으면
						if (escape[l]) continue;
						if (RectangleContainPoint(runnerY[l], runnerX[l], j, k, i)) {
							R = j; C = k; rectangleN = i;
							return;
						}
					}
				}
			}
		}
	}
}
void rotate() {
	int copyBoard[11][11]={0};
	memmove(copyBoard, board, sizeof(board));
	bool isChanged[11] = { false };
	for (int i = 1; i <= M; i++) { //정사각형 안의 러너들 위치 바꾸기
		if (isChanged[i]) continue;
		if (RectangleContainPoint(runnerY[i], runnerX[i], R, C, rectangleN)) {
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
			if (board[i][j] >= 1) copyBoard[newY + R][newX + C]--; //벽 1감소
			if (board[i][j] == -1) { //출구위치 저장
				exitY = newY + R;
				exitX = newX + C;
			}
		}
	}
	memmove(board, copyBoard, sizeof(board));
}
void startTurn() {
	calculateAllRunnersDistance();
	moveAllRunners();
	getRectangle();
	rotate();
}
int main() {
	cin >> N >> M >> K;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			cin >> board[i][j];
		}
	}
	for (int i = 1; i <= M; i++) {
		cin >> runnerY[i] >> runnerX[i];
	}
	cin >> exitY >> exitX;
	board[exitY][exitX] = -1;
	for (int i = 1; i <= K; i++) {  //턴 진행
		if (allRunnerEscape()) break;
		startTurn();
	}
	cout << result<<"\n"<< exitY <<" " << exitX;
	return 0;
}
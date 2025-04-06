/*
*   삼성 SW 역량테스트 2022 하반기 오전 1번 문제 / 싸움땅 (L14)
*   Date: 2025-04-06 (일)
*   Duration: 
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <set>
#include <numeric>
#include <iomanip>
#include <algorithm>
#include <queue>

#define endl '\n'
#define INF 987654321
using namespace std;

//--------------------- 변수 선언 ---------------------
struct PLAYER {
	int r; // 행
	int c; // 열
	int d; // 방향
	int s; // 초기 능력치
	int gun; // 현재 총기 능력
	int score; // 누적 포인트
};

int N; // 격자 크기
int M; // 플레이어 수
int K; // 라운드 수
int dr[] = { -1, 0, 1, 0 }; // 상, 우, 하, 좌
int dc[] = { 0, 1, 0, -1 };
string dir_str[] = { "UP", "EAST","DOWN", "LEFT"};
 
vector<vector<priority_queue<int>> > grid;
vector<PLAYER> players;

//--------------------- 디버깅 함수 ---------------------
void traverse_2d(const vector<vector<int> >& vec) {
	cout << "[Traverse]" << endl;
	for (int i = 0; i < vec.size(); ++i) {
		for (int j = 0; j < vec[i].size(); ++j) {
			cout << vec[i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;
}

void traverse_player() {
	cout << "[Traverse players]" << endl;
	for (int i = 0; i < M; ++i) {
		cout << "Player " << i + 1 << " | ";
		cout << "pos: " << players[i].r << ' ' << players[i].c << ' ';
		cout << "| d, s, gun, score: " << dir_str[players[i].d] << ' ' << players[i].s << ' ' << players[i].gun << ' ' << players[i].score << endl;
	}
	cout << endl;
}


//--------------------- 보조 함수 ---------------------

void fill_2d(vector<vector<int> >& vec, int data) {
	for (int i = 0; i < vec.size(); ++i) {
		for (int j = 0; j < vec[i].size(); ++j) {
			vec[i][j] = data;
		}
	}
}

bool is_grid(int r, int c) {
	if (r < 0 || c < 0 || r >= N || c >= N) return false;
	return true;
}


//--------------------- 메인 함수 ---------------------
void init() {

}

void input() {
	cin >> N >> M >> K;
	int tmp;
	// 격자 입력
	grid.resize(N);
	for (int i = 0; i < N; ++i) {
		grid[i].resize(N);
		for (int j = 0; j < N; ++j) {
			cin >> tmp;
			grid[i][j].push(tmp);
		}
	}

	// 플레이어 초기화
	players.resize(M);
	for (int i = 0; i < M; ++i) {
		PLAYER& p = players[i];
		cin >> p.r >> p.c >> p.d >> p.s;
		p.r--, p.c--, p.gun = 0, p.score = 0;
	}
}

void exchange_gun(int player) {
	PLAYER& p = players[player];
	priority_queue<int> & pq = grid[p.r][p.c];
	
	if (pq.empty()) return; // 총 없는 경우 리턴
	
	if (p.gun < pq.top()) { // 놓인 총이 더 클 경우
		pq.push(p.gun);
		p.gun = pq.top();
		pq.pop();
	}
}

void fight_two(int p1, int p2) {
	PLAYER& a = players[p1], & b = players[p2];
	int winner = -1, loser = -1, score = abs((a.s + a.gun) - (b.s + b.gun));

	// 2-2-1
	if (a.s + a.gun != b.s + b.gun) {
		if (a.s + a.gun > b.s + b.gun) winner = p1, loser = p2;
		else winner = p2, loser = p1;
	}
		
	else {
		if (a.s > b.s) winner = p1, loser = p2;
		else winner = p2, loser = p1;
	}

	// cout << "loser & winner: " << loser + 1 << ' ' << winner + 1 << endl;
	PLAYER& l = players[loser], & w = players[winner];


	w.score += score;

	// 패자 처리 (2-2-2)
	grid[b.r][b.c].push(l.gun);
	l.gun = 0;

	// cout << "loser pos: " << l.r << ' ' << l.c << endl;


	for (int i = 0; i < 4; ++i) {
		int nr = l.r + dr[(l.d + i) % 4], nc = l.c + dc[(l.d + i) % 4];
		if (!is_grid(nr, nc)) continue; // 격자 밖인 경우

		bool exist_other = false;

		for (int idx = 0; idx < M; ++idx) {
			if (idx == loser) continue;
			PLAYER& op = players[idx];
			if (op.r == nr && op.c == nc) exist_other = true;
		}

		if (exist_other) continue;
		// cout << "loser pos: " << nr << ' ' << nc << endl;

		l.r = nr, l.c = nc, l.d = (l.d + i) % 4;
		break;
	}


	exchange_gun(loser);

	// 2-2-3
	exchange_gun(winner);
}

void move_player(int player) {
	PLAYER& p = players[player];
	int nr = p.r + dr[p.d], nc = p.c + dc[p.d];

	// 격자 벗어날 경우 방향 변경 + 해당 방향 1 이동
	if (!is_grid(nr, nc)) {
		p.d = (p.d + 2) % 4;
		nr = p.r + dr[p.d], nc = p.c + dc[p.d];
	}

	p.r = nr, p.c = nc;

	// 플레이어 만났는지 체크
	int other = -1;
	for (int i = 0; i < M; ++i) {
		if (i == player) continue;

		PLAYER& op = players[i];
		if (op.r == p.r && op.c == p.c) other = i;
	}

	if (other == -1) { // 이동한 곳에 플레이어 없는 경우
		exchange_gun(player);
	}

	else { // 이동 후 플레이어 만난 경우
		fight_two(player, other);
	}

	// cout << "Player: " << player + 1 << ' ';
	// traverse_player();
	
}

void run() {
	while (K--) {
		// 1. 플레이어 이동
		for(int i = 0; i < M; ++i) move_player(i);
	}

	// 각 플레이어 획득 포인트 출력
	for (auto & p : players) cout << p.score << ' ';
}


int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
	// freopen("input.txt", "r", stdin);

	init();
	input();
	run();

	return 0;
}
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <queue>
#include <algorithm>

#define endl '\n'
#define pii pair<int, int>
#define INF 987654321
using namespace std;
constexpr size_t MAX_N = 20, MAX_M = 10;
int dx[4] = { 0, 0, 1, -1 }; // E, W, S, N
int dy[4] = { 1, -1, 0, 0 };
int grid_2d[MAX_N][MAX_N], grid_3d[5][MAX_M][MAX_M];
bool visited_2d[MAX_N][MAX_N], visited_3d[5][MAX_M][MAX_M];

int grid_strange_2d[MAX_N][MAX_N];
int N, M, F;

void traverse_strange_2d() {
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++) {
			cout << grid_strange_2d[i][j] << ' ';
		}
		cout << endl;
	}
}

// 3차원 상의 위치 정보 (현재 면, row, col)
struct INFO_3D{
	int dim, r, c;
};

// 2차원 상 위치 정보 (row, col)
struct INFO_2D {
	int r, c;
};

INFO_3D start_pos, connector_3d;
INFO_2D exit_pos, left_top, right_bot, connector_2d;


// queue에 넣고 BFS 돌릴 객체 (3D)
struct NODE_3D {
	int turn;
	INFO_3D pos;
};

// queue에 넣고 BFS 돌릴 객체 (2D)
struct NODE_2D {
	int turn;
	INFO_2D pos;
};

bool is_out_2d(int x, int y) {
	if (x < 0 || y < 0 || x >= N || y >= N) return true;
	return false;
}

// 이상 현상 확장
void spread_strange(int r, int c, int d, int v) {
	int cur_turn = 0;
	grid_strange_2d[r][c] = 0;

	while (true) {
		r += dx[d], c += dy[d], cur_turn += v;
		if (is_out_2d(r, c)) break; // 격자 벗어난 경우
		if (grid_2d[r][c] == 1 || grid_2d[r][c] == 3 || grid_2d[r][c] == 4) break; // 장애물, 3차원, 탈출구 일 경우
		grid_strange_2d[r][c] = min(grid_strange_2d[r][c], cur_turn);
	}
}

// 3d 좌표에서 탈출 지점 찾기
// 3d 좌표 이동

void find_connector() {
	connector_3d = { -1, -1, -1 };;
	
	// 1. 탈출구가 큐브 동쪽
	if (connector_2d.c > right_bot.c) {
		connector_3d = { 0, M - 1, right_bot.r - connector_2d.r };
	}

	// 2. 탈출구가 큐브 서쪽
	else if (connector_2d.c < left_top.c) {
		connector_3d = { 1, M - 1, connector_2d.r - left_top.r };
	}

	// 3. 탈출구가 큐브 남쪽
	else if (connector_2d.r > right_bot.r) {
		connector_3d = { 2, M - 1, connector_2d.c - left_top.c };
	}

	// 4. 탈출구가 큐브 북쪽
	else if (connector_2d.r < left_top.r) {
		connector_3d = { 3, M - 1, right_bot.c - left_top.c };
	}

	// cout << "Con 3d: " << connector_3d.dim << ' ' << connector_3d.r << ' ' << connector_3d.c << endl;
}


INFO_3D transfer_3d(INFO_3D cur, int next_dir) { // 3차원 상의 이동 좌표 변환
	INFO_3D next_pos = {-1, -1, -1};
	
	// 1. 동에서 이동
	if (cur.dim == 0) {
		// 1. 동 -> 위
		if (next_dir == 3 && cur.r == 0) next_pos = { 4, M - 1 - cur.c, M - 1 };

		// 2. 동 -> 남
		else if (next_dir == 1 && cur.c == 0) next_pos = { 2, cur.r, M - 1 };

		// 3. 동 -> 북
		else if (next_dir == 0 && cur.c == M - 1) next_pos = { 3, cur.r, 0 };

		// 4. out of bound
		else if (next_dir == 2 && cur.r == M - 1) next_pos = { -1, -1, -1 };

		// 5. 차원 내 이동
		else next_pos = { cur.dim, cur.r + dx[next_dir], cur.c + dy[next_dir] };
	}

	// 2. 서에서 이동
	else if (cur.dim == 1) {
		// 1. 서 -> 위
		if (next_dir == 3 && cur.r == 0) next_pos = { 4, cur.c, 0 };

		// 2. 서 -> 남
		else if (next_dir == 0 && cur.c == M - 1) next_pos = { 2, cur.r, 0 };

		// 3. 서 -> 북
		else if (next_dir == 1 && cur.c == 0) next_pos = { 3, cur.r, M - 1 };

		// 4. out of bound
		else if (next_dir == 2 && cur.r == M - 1) next_pos = { -1, -1, -1 };

		// 5. 차원 내 이동
		else next_pos = { cur.dim, cur.r + dx[next_dir], cur.c + dy[next_dir] };
	}

	// 3. 남에서 이동
	else if (cur.dim == 2) {
		// 1. 남 -> 위
		if (next_dir == 3 && cur.r == 0) next_pos = { 4, M - 1, cur.c };

		// 2. 남 -> 동
		else if (next_dir == 0 && cur.c == M - 1) next_pos = { 0, cur.r, 0 };

		// 3. 남 -> 서
		else if (next_dir == 1 && cur.c == 0) next_pos = { 1, cur.r, M - 1 };

		// 4. out of bound
		else if (next_dir == 2 && cur.r == M - 1) next_pos = { -1, -1, -1 };


		// 5. 차원 내 이동
		else next_pos = { cur.dim, cur.r + dx[next_dir], cur.c + dy[next_dir] };
	}

	// 4. 북에서 이동
	else if (cur.dim == 3) {
		// 1. 북 -> 위
		if (next_dir == 3 && cur.r == 0) next_pos = { 4, 0, M - 1 - cur.c };

		// 2. 북 -> 동
		else if (next_dir == 1 && cur.c == 0) next_pos = { 0, cur.r, M - 1 };

		// 3. 북 -> 서
		else if (next_dir == 0 && cur.c == M - 1) next_pos = { 1, cur.r, 0 };

		// 4. out of bound
		else if (next_dir == 2 && cur.r == M - 1) next_pos = { -1, -1, -1 };


		// 5. 차원 내 이동
		else next_pos = { cur.dim, cur.r + dx[next_dir], cur.c + dy[next_dir] };
	}

	// 5. 위에서 이동
	else {
		// 1. 위 -> 동
		if (next_dir == 0 && cur.c == M - 1) next_pos = { 0, 0,  M - 1 - cur.r };

		// 2. 위 -> 서
		else if (next_dir == 1 && cur.c == 0) next_pos = { 1, 0, cur.r };

		// 3. 위 -> 남
		else if (next_dir == 2 && cur.r == M - 1) next_pos = { 2, 0, cur.c };

		// 3. 위 -> 북
		else if (next_dir == 3 && cur.r == 0) next_pos = { 3, 0, M - 1 - cur.c };

		// 5. 차원 내 이동
		else next_pos = { cur.dim, cur.r + dx[next_dir], cur.c + dy[next_dir] };
	}

	return next_pos;
} 


void init() {
	left_top = { -1, -1 };
	cin >> N >> M >> F;

	// 2d 평면도
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++) {
			cin >> grid_2d[i][j];

			if (grid_2d[i][j] == 3 && left_top.r == -1 && left_top.c == -1) {
				left_top = { i, j }; // 2d 좌표 상 큐브의 좌상단 좌표
				right_bot = { i + M - 1, j + M - 1};
			}

			else if (grid_2d[i][j] == 4) exit_pos = { i, j }; // 2d 좌표 상의 탈출 구
		}
	}

	// 3d 평면도
	for (int k = 0; k < 5; ++k) { // E, W, S, N, U
		for (int i = 0; i < M; i++) { 
			for (int j = 0; j < M; j++) {
				cin >> grid_3d[k][i][j];

				if (grid_3d[k][i][j] == 2) { // 타임 머신 위치 기록
					start_pos.dim = k, start_pos.r = i, start_pos.c = j;
				}
			}
		}
	}

	// cout << left_top.r << ' ' << left_top.c << endl;
	// connector (2d-3d 접합부)
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			for (int dir = 0; dir < 4; ++dir) {
				int nr = left_top.r + i + dx[dir], nc = left_top.c + j + dy[dir];
				if (is_out_2d(nr, nc)) continue;
				if (grid_2d[nr][nc] == 0) connector_2d = { nr , nc };
			}
		}
	}

	// cout << "left_top: " << left_top.r << ' ' << left_top.c << endl;
	// cout << "right_bot: " << right_bot.r << ' ' << right_bot.c << endl;
	// cout << "Connector: " << connector_2d.r << ' ' << connector_2d.c << endl;

	// 이상 현상
	int r, c, d, v;

	for (int rpt = 0; rpt < MAX_N; ++rpt) {
		fill(grid_strange_2d[rpt], grid_strange_2d[rpt] + MAX_N, INF);
	}

	for (int i = 0; i < F; i++){
		cin >> r >> c >> d >> v;
		spread_strange(r, c, d, v);
	}

	// traverse_strange_2d();

	find_connector();
}

NODE_3D run_3d() {
	queue<NODE_3D> q;
	q.push({ 0, start_pos });
	visited_3d[4][start_pos.r][start_pos.c] = true;
	
	while (!q.empty()) {
		NODE_3D top = q.front();
		q.pop();
		// cout << "top: " << top.turn << ' ' << top.pos.dim << ' ' << top.pos.r << ' ' << top.pos.c << endl;

		for (int i = 0; i < 4; ++i) {
			INFO_3D tar = transfer_3d(top.pos, i);

			// cout << tar.dim << ' ' << tar.r << ' ' << tar.c << endl;

			if (tar.dim == -1) continue; // out of grid
			if (visited_3d[tar.dim][tar.r][tar.c] || grid_3d[tar.dim][tar.r][tar.c] == 1) continue; // already visited or blocked
			
			visited_3d[tar.dim][tar.r][tar.c] = true;

			if (tar.dim == connector_3d.dim && tar.r == connector_3d.r && tar.c == connector_3d.c) return { top.turn + 1, tar };

			q.push({ top.turn + 1, tar });
		}
	}

	return { -1, {-1, -1, -1} };
}

NODE_2D run_2d(int start_turn) {
	queue<NODE_2D> q;
	q.push({start_turn, connector_2d});
	visited_2d[connector_2d.r][connector_2d.c] = true;

	while (!q.empty()) {
		NODE_2D top = q.front();
		// cout << top.turn << "| " << top.pos.r << ' ' << top.pos.c << endl;

		q.pop();

		for (int dir = 0; dir < 4; ++dir) {
			int nr = top.pos.r + dx[dir], nc = top.pos.c + dy[dir];
			if (is_out_2d(nr, nc)) continue; // 격자 벗어난 경우
			if (visited_2d[nr][nc]) continue; // 이미 방문한 경우
			if (grid_2d[nr][nc] == 1 || grid_2d[nr][nc] == 3) continue; // 장애물 있는 경우
			if (grid_strange_2d[nr][nc] <= top.turn + 1) continue; // 이상 현상 보다 늦게 or 같은 턴에 도착하는 경우
			if (exit_pos.r == nr && exit_pos.c == nc) return {top.turn + 1, top.pos};

			visited_2d[nr][nc] = true;
			q.push({ top.turn + 1, {nr, nc} });
		}
	}

	return { -1, {-1, -1} };
}


int main() {
	// freopen("input.txt", "r", stdin);
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);

	init();


	NODE_3D exit_3d = run_3d();
	if (exit_3d.turn == -1) { // 3차원 탈출 못한 경우 
		cout << -1;
		return 0;
	}


	NODE_2D exit_2d = run_2d(exit_3d.turn + 1);
	if (exit_2d.turn == -1) { // 3차원 탈출 못한 경우 
		cout << -1;
		return 0;
	}

	cout << exit_2d.turn << endl;
	return 0;
}
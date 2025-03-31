#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <set>
#define endl '\n'
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

// 격자 표시 용
#define EMPTY 0
#define EXIT 1

using namespace std;

// ---------------- 변수 선언 ----------------
int R; // 격자 행 크기
int C; // 격자 열 크기
int K; // 정령의 수
int dr[4] = { -1, 0, 1, 0 }; // 북, 동, 남, 서
int dc[4] = { 0, 1, 0, -1 };

struct GOLEM {
    int c; // 골렘이 출발하는 열
    int d; // 골렘의 출구 방향 정보
    int idx; // 골렘의 고유 번호 (grid 표시 용)
};

struct POS {
    int r; // row
    int c; // col
};

queue<GOLEM> q_golem;
vector<vector<int> > grid, exit_num, visited;

// ---------------- 보조 함수 ----------------
void fill_2d(vector<vector<int> >& vec, int data) {
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            vec[i][j] = data;
        }
    }
}

int rotate_cw(int dir) {
    return (dir + 1) % 4;
}

int rotate_ccw(int dir) {
    return (dir + 3) % 4;
}

bool is_valid(int r, int c) {
    if (r < 0 || c <= 0 || r > R + 2 || c > C) return false;
    return true;
}
// ---------------- 메인 함수 ----------------
void init() {
    while (!q_golem.empty()) q_golem.pop();
}

void input() {
    cin >> R >> C >> K;
    grid.resize(R + 3), visited.resize(R + 3), exit_num.resize(R + 3);
    for (int i = 0; i <= R + 2; ++i) grid[i].resize(C + 1), visited[i].resize(C + 1), exit_num[i].resize(C + 1);

    int c, d;
    for (int i = 2; i <= K + 1; i++) {
        cin >> c >> d;
        q_golem.push({c, d, i});
    }
}

bool south_golem(POS& p, int& dir) {
    // 움직일 수 있는지 check, 없으면 false return;
    if (!is_valid(p.r + 1, p.c - 1) || !is_valid(p.r + 1, p.c + 1) || !is_valid(p.r + 2, p.c)) return false; // 남쪽 check
    if (grid[p.r + 1][p.c - 1] != 0 || grid[p.r + 1][p.c + 1] != 0 || grid[p.r + 2][p.c] != 0) return false; 
    p.r++;
    return true;
}

bool west_golem(POS& p, int& dir) {
    // 움직일 수 있는지 check, 없으면 false return;
    if (!is_valid(p.r - 1, p.c - 1) || !is_valid(p.r, p.c - 2) || !is_valid(p.r + 1, p.c - 2)) return false; // 서쪽 check
    if (grid[p.r - 1][p.c - 1] != 0 || grid[p.r][p.c - 2] != 0 || grid[p.r + 1][p.c - 2] != 0) return false;
    if (!is_valid(p.r + 1, p.c - 1) || !is_valid(p.r + 2, p.c - 1)) return false;// 남쪽 check
    if (grid[p.r + 1][p.c - 1] != 0 || grid[p.r + 2][p.c - 1] != 0) return false;
    p.r++, p.c--, dir = rotate_ccw(dir);
    return true;
}

bool east_golem(POS& p, int& dir) {
    // 움직일 수 있는지 check, 없으면 false return;
    if (!is_valid(p.r - 1, p.c + 1) || !is_valid(p.r, p.c + 2) || !is_valid(p.r + 1, p.c + 2)) return false; // 동쪽 check
    if (grid[p.r - 1][p.c + 1] != 0 || grid[p.r][p.c + 2] != 0 || grid[p.r + 1][p.c + 2] != 0) return false;
    if (!is_valid(p.r + 1, p.c + 1) || !is_valid(p.r + 2, p.c + 1)) return false; // 남쪽 check
    if (grid[p.r + 1][p.c + 1] != 0 || grid[p.r + 2][p.c + 1] != 0) return false;
    p.r++, p.c++, dir = rotate_cw(dir);
    return true;
}

void color_golem(const POS& p, int dir, int idx) {
    grid[p.r][p.c] = idx;
    for (int i = 0; i < 4; ++i) grid[p.r + dr[i]][p.c + dc[i]] = idx;
    exit_num[p.r + dr[dir]][p.c + dc[dir]] = idx;
}

int find_max_row(POS& p) {
    queue<POS> q;
    fill_2d(visited, 0);
    int max_row = -1;
    q.push({ p.r, p.c });

    while (!q.empty()) {
        POS top = q.front(); 
        q.pop();
        max_row = max(max_row, top.r);

        for (int i = 0; i < 4; ++i) {
            int nr = top.r + dr[i], nc = top.c + dc[i];
            if (!is_valid(nr, nc) || visited[nr][nc] || grid[nr][nc] == 0) continue;
            if (exit_num[top.r][top.c] != grid[top.r][top.c] && grid[nr][nc] != grid[top.r][top.c]) continue;
            visited[nr][nc] = 1;
            q.push({ nr, nc });
        }
    }

    // traverse_2d(visited);
    return max_row;
}

int move_golem(GOLEM gol) {
    POS cur_gol = { 0, gol.c };
    int cur_dir = gol.d;

    while (true) {
        if (south_golem(cur_gol, cur_dir)) continue;
        if (west_golem(cur_gol, cur_dir)) continue;
        if (east_golem(cur_gol, cur_dir)) continue;

        // 남, 서, 동으로 못 움직이면 종료
        if (cur_gol.r < 4) return -1; // 격자 밖으로 나간 경우

        color_golem(cur_gol, cur_dir, gol.idx);
        // traverse_2d(grid);
        
        return find_max_row(cur_gol); 
    }
}

void run() {
    int acc = 0;
    while (!q_golem.empty()) {
        GOLEM top = q_golem.front();
        q_golem.pop();

        int score = move_golem(top);
        // cout << "Score: " << score - 2 << endl;
        if (score != -1) acc += (score - 2);
        else {
            fill_2d(grid, EMPTY);
            fill_2d(exit_num, EMPTY);
        }
    }
    cout << acc;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    init();
    input();
    run();
    return 0;
}
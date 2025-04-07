/*
*   삼성 SW 역량테스트 2023 하반기 오후 1번 문제 / 루돌프의 반란 (L12)
*   Date: 2025-04-07 (월)
*   Duration: 2h 2m
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <queue>
#include <numeric>
#include <iomanip>
#include <cmath>

#define endl '\n'
#define INF 987654321
#define EMPTY 0
#define WALL -1

using namespace std;

// ---------------- 변수 선언 ----------------
struct POS {
    int r, c;
};

vector<vector<int> > grid, tmp_grid, left_killer;
int N; // 격자의 크기
int M; // 박멸이 진행되는 년 수
int K; // 제초제의 확산 범위
int C; // 제초제가 남아있는 년 수
int cur_turn, acc_remove;
int dr[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
int dc[] = { 1, -1, 0, 0, 1, -1, 1, -1 };

// ---------------- 디버깅 함수 ----------------
void traverse_2d_int(const vector<vector<int> >& vec) {
    cout << "[Traverse]" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec.size(); ++j) {
cout << setw(2);
cout << vec[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

// ---------------- 보조 함수 ----------------
void fill_2d_int(vector<vector<int> >& vec, int data) {
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec.size(); ++j) {
            vec[i][j] = data;
        }
    }
}

bool is_grid(POS& p) {
    if (p.r < 0 || p.c < 0 || p.r >= N || p.c >= N) return false;
    return true;
}

void mark_tmp() {
    for (int i = 0; i < tmp_grid.size(); ++i) {
        for (int j = 0; j < tmp_grid.size(); ++j) {
            if (grid[i][j] != EMPTY) tmp_grid[i][j] = 1;
            else tmp_grid[i][j] = 0;
        }
    }
}

int count_around_trees(const vector<vector<int> >& vec, POS p) {
    int cnt = 0;
    for (int i = 0; i < 4; ++i) {
        POS np = { p.r + dr[i], p.c + dc[i] };
        if (!is_grid(np) || grid[np.r][np.c] <= 0) continue;
        cnt++;
    }
    return cnt;
}

int count_around_empty(const vector<vector<int> >& vec, POS p) {
    int cnt = 0;
    for (int i = 0; i < 4; ++i) {
        POS np = { p.r + dr[i], p.c + dc[i] };
        if (!is_grid(np) || tmp_grid[np.r][np.c] != 0 || left_killer[np.r][np.c] >= cur_turn) continue;
        cnt++;
    }
    return cnt;
}

// ---------------- 메인 함수 ----------------
void init() {
    cur_turn = 0, acc_remove = 0;
}

void input() {
    cin >> N >> M >> K >> C;
    grid.resize(N), tmp_grid.resize(N), left_killer.resize(N);
    for (int i = 0; i < N; ++i) {
        grid[i].resize(N), tmp_grid[i].resize(N), left_killer[i].resize(N);
        for (int j = 0; j < N; ++j) {
            cin >> grid[i][j];
        }
    }
}

void grow_trees() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] > 0) grid[i][j] += count_around_trees(grid, { i, j });
        }
    }

    // traverse_2d_int(grid);
}

void spread_trees() {
    mark_tmp();

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] > 0 && tmp_grid[i][j] == 1) {
                int cnt = count_around_empty(tmp_grid, { i, j }); // 주변 빈 칸 수
                if (cnt == 0) continue;
                int val = grid[i][j] / cnt;

                for (int dir = 0; dir < 4; ++dir) {
                    POS np = { i + dr[dir], j + dc[dir] };
                    if (!is_grid(np) || tmp_grid[np.r][np.c] == 1 || left_killer[np.r][np.c] >= cur_turn) continue;
                    grid[np.r][np.c] += val;
                }
            }

        }
    }

    // traverse_2d_int(grid);
}

/*
* @brief: p에 제초제 뿌릴 경우 제거 가능한 나무 개수 리턴
*
*/
int drop_killer(POS p, bool is_simulate) {
    int acc = grid[p.r][p.c];

    for (int dir = 4; dir < 8; ++dir) {
        POS cur = p;
        if(!is_simulate) left_killer[p.r][p.c] = cur_turn + C, grid[p.r][p.c] = 0;

        for (int len = 1; len <= K; ++len) {
            cur = { cur.r + dr[dir], cur.c + dc[dir] };
            if (!is_grid(cur) || grid[cur.r][cur.c] == -1) break;
            if (!is_simulate) left_killer[cur.r][cur.c] = cur_turn + C;
            if (grid[cur.r][cur.c] == 0) break;
            acc += grid[cur.r][cur.c];
            if (!is_simulate) grid[cur.r][cur.c] = 0;
        }
    }

    return acc;
}

POS find_spot() {
    POS p = { 0, 0 };
    int max_cnt = 0;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] == -1) continue;
            int cnt = drop_killer({ i, j }, true);
            // cout << i << ' ' << j << ' ' << cnt << endl;
            if (cnt > max_cnt) p = { i, j }, max_cnt = cnt;
        }
    }

    return p;
}

void remove_trees() {
    POS p = find_spot();
    acc_remove += drop_killer(p, false);

    // traverse_2d_int(left_killer);
}

void run() {
    while (M--) {
        cur_turn++;
        grow_trees();
        spread_trees();
        remove_trees();
    }

    cout << acc_remove;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    init();
    input();
    run();

    return 0;
}
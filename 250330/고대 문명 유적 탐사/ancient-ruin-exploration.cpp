#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#define endl '\n'
using namespace std;
typedef pair<int, int> pii;

//---------------- 변수 선언 ----------------
int K; // 탐사 반복 회수
int M; // 유물 조각 개수
int dr[4] = { 0, 0, 1, -1 };
int dc[4] = { 1, -1, 0 ,0 };
vector<vector<int> > grid, sub_grid, visited, rem_select; // 격자, 회전 용 격자
queue<int> next_num; // 다음 유물 번호

struct POS {
    int r, c;
};

// val, deg, r, c;
struct NODE {
    int val, deg, r, c;
};

struct cmp {
    bool operator()(const NODE& a, const NODE& b) {
        if (a.val != b.val) return a.val < b.val;
        if (a.deg != b.deg) return a.deg > b.deg;
        if (a.c != b.c) return a.c > b.c;
        return a.r > b.r;
    }
};

//---------------- 보조 함수 ----------------
void copy_2d(vector<vector<int>>& to, const vector<vector<int>>& from) {
    for (int i = 0; i < to.size(); ++i) {
        for (int j = 0; j < to[i].size(); ++j) {
            to[i][j] = from[i][j];
        }
    }
}

void fill_2d(vector<vector<int>>& vec, int data) {
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            vec[i][j] = data;
        }
    }
}

void rotate_grid(int r, int c, int deg) {
    int tar_len = 3, grid_len = grid.size();

    copy_2d(sub_grid, grid);

    if (deg == 1) { // 90'
        for (int i = 0; i < tar_len; ++i) {
            for (int j = 0; j < tar_len; ++j) {
                sub_grid[r + i][c + j] = grid[r + tar_len - 1 - j][c + i];
            }
        }
    }

    else if (deg == 2) {  // 180'
        for (int i = 0; i < tar_len; ++i) {
            for (int j = 0; j < tar_len; ++j) {
                sub_grid[r + i][c + j] = grid[r + tar_len - 1 - i][c + tar_len - 1 - j];
            }
        }
    }

    else if (deg == 3) { // 270'
        for (int i = 0; i < tar_len; ++i) {
            for (int j = 0; j < tar_len; ++j) {
                sub_grid[r + i][c + j] = grid[r + j][c + tar_len - 1 - i];
            }
        }
    }
}

bool is_valid_5x5(int r, int c) {
    if (r < 0 || c < 0 || r >= 5 || c >= 5) return false;
    return true;
}

//---------------- 메인 함수 ----------------
int get_score(const vector<vector<int>>& tar, bool do_mark) {
    fill_2d(visited, 0);
    fill_2d(rem_select, 0);
    queue<POS> bfs_q;
    set<int> mark_set;

    int acc = 0, mark = 0;
    for (int i = 0; i < tar.size(); ++i) {
        for (int j = 0; j < tar[i].size(); ++j) {
            if (visited[i][j]) continue;
            visited[i][j] = 1, rem_select[i][j] = ++mark;

            // 먼저 q 비우기
            while (!bfs_q.empty()) bfs_q.pop();
            bfs_q.push({ i, j });

            int cur_acc = 1, cur_tar = tar[i][j];
            while (!bfs_q.empty()) {
                POS top = bfs_q.front();
                bfs_q.pop();

                for (int dir = 0; dir < 4; ++dir) {
                    int nr = top.r + dr[dir], nc = top.c + dc[dir];
                    if (!is_valid_5x5(nr, nc) || visited[nr][nc] || tar[nr][nc] != cur_tar) continue;
                    visited[nr][nc] = true, rem_select[nr][nc] = mark;

                    cur_acc++;
                    bfs_q.push({ nr, nc });
                }
            }

            if (cur_acc >= 3) {
                acc += cur_acc;
                if (do_mark) mark_set.insert(mark);
            }
        }
    }

    if (do_mark){
        for (int i = 0; i < tar.size(); ++i) {
            for (int j = 0; j < tar[i].size(); ++j) {
                if (mark_set.find(rem_select[i][j]) == mark_set.end()) rem_select[i][j] = 0;
                else rem_select[i][j] = 1;
            }
        }
    }

    return acc;
}

NODE simulate(int r, int c, int deg) {
    // r, c을 좌상단으로 deg 만큼 3*3 회전 
    rotate_grid(r, c, deg);

    // 1차 가치 획득
    int acc = get_score(sub_grid, false);
    return { acc, deg, r, c };
}

bool rotate_small_grid() {
    bool flag_continue = true;
    priority_queue<NODE, vector<NODE>, cmp> pq;

    for (int i = 0; i <= 2; ++i) {
        for (int j = 0; j <= 2; ++j) {
            for (int deg = 1; deg <= 3; ++deg) {
                NODE sim = simulate(i, j, deg);
                pq.push(sim);
            }
        }
    }

    NODE top = pq.top();

    // 점수 못 얻을 경우, 리턴
    if (top.val == 0) {
        return flag_continue = false;
    }

    // pq 최상단 값 실제 grid에 반영
    rotate_grid(top.r, top.c, top.deg);
    copy_2d(grid, sub_grid);

    return flag_continue;
}

int get_score_cascaded() {
    int acc = 0;
    while (true) {
        int cur_score = get_score(grid, true);
        if (cur_score == 0) return acc;
        acc += cur_score;

        for (int c = 0; c < 5; ++c) {
            for (int r = 4; r >= 0; r--) {
                if (rem_select[r][c] == 1) {
                    grid[r][c] = next_num.front();
                    next_num.pop();
                }
            }
        }
    }
}

void input() {
    cin >> K >> M;

    grid.resize(5), sub_grid.resize(5), visited.resize(5), rem_select.resize(5);
    for (int i = 0; i < 5; i++) {
        grid[i].resize(5), sub_grid[i].resize(5), visited[i].resize(5), rem_select[i].resize(5);
        for (int j = 0; j < 5; j++) {
            cin >> grid[i][j];
        }
    }

    int tmp;
    for (int i = 0; i < M; i++) {
        cin >> tmp;
        next_num.push(tmp);
    }
}

void init() {
    while (!next_num.empty()) next_num.pop();
}

void run() {
    bool flag_continue = true;

    while (K--) {
        flag_continue = rotate_small_grid();
        if (!flag_continue) return;
        cout << get_score_cascaded() << ' ';
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    init();
    input();
    run();
    return 0;
}
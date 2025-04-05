/*
*   삼성 SW 역량테스트 2023 상반기 오후 1번 문제 / 메이즈 러너 (L13)
*   Date: 2025-04-05 (토)
*   Duration: 2h 04m
*/

#define _CRT_SECURE_NO_WARNINGS
#define endl '\n'
#define INF 987654321

#include <iostream>
#include <vector>
#include <queue>
#include <numeric>
#include <algorithm>
#include <set>

using namespace std;

// ------------------ 변수 선언 ------------------
int N; // 미로 크기 (4 ~ 10)
int M; // 참가자 수 (1 ~ 10)
int K; // 게임 시간 (1 ~ 100)
int dr[4] = { -1, 1, 0 ,0 }; // 상, 하, 좌, 우
int dc[4] = { 0, 0, 1 ,-1 }; // 상, 하, 좌, 우

struct POS {
    int r, c;
};

vector<vector<int> > grid, grid_user, visited;
vector<int> acc_move;
vector<POS> users;
POS exit_pos;
set<int> remain_users;

// ------------------ 디버깅 함수 ------------------
void traverse_2d(const vector<vector<int> >& vec) {
    cout << "[Traverse]" << endl;
    for (int i = 1; i < vec.size(); ++i) {
        for (int j = 1; j < vec[i].size(); ++j) {
            cout << vec[i][j] << ' ';
        }
        cout << endl;
    }
}

void traverse_alivepos() {
    cout << "[Traverse pos of remain users]" << endl;
    for (int u : remain_users) {
        cout << "Pos of User " << u << ": " << users[u].r << ' ' << users[u].c << endl;
    }

    cout << "Pos of exit: " << exit_pos.r << ' ' <<exit_pos.c << endl;
}

// ------------------ 보조 함수 ------------------
void fill_2d(vector<vector<int> > & vec, int data) {
    for (int i = 1; i <= N; ++i) {
        for (int j = 1; j <= N; ++j) {
            vec[i][j] = data;
        }
    }
}

int manhattan_dist(POS a, POS b) {
    return abs(a.r - b.r) + abs(a.c - b.c);
}

bool is_grid(int r, int c) {
    if (r <= 0 || c <= 0 || r > N || c > N) return false;
    return true;
}

// ------------------ 메인 함수 ------------------
void init() {
    remain_users.clear();
}

void input() {
    cin >> N >> M >> K;

    // 미로 입력
    grid.resize(N + 1), visited.resize(N + 1), grid_user.resize(N + 1);
    for (int i = 1; i <= N; ++i) {
        grid[i].resize(N + 1), visited[i].resize(N + 1), grid_user[i].resize(N + 1);
        for (int j = 1; j <= N; ++j) {
            cin >> grid[i][j];
        }
    }

    // 참가자 좌표 입력
    users.resize(M), acc_move.resize(M);
    for (int i = 0; i < M; ++i) {
        remain_users.insert(i);
        cin >> users[i].r >> users[i].c;
    }

    // 출구 좌표 입력
    cin >> exit_pos.r >> exit_pos.c;
}

void move_users() {
    set<int> escaped_users;

    for (int user : remain_users) {
        POS& u = users[user];
        POS tmp = u;
        int min_d = manhattan_dist(exit_pos, u);
        bool did_move = false;

        for (int i = 0; i < 4; ++i) {
            int nr = u.r + dr[i], nc = u.c + dc[i];
            if (!is_grid(nr, nc) || grid[nr][nc] != 0) continue; // 격자 밖이거나 장애물 있는 경우
            if (manhattan_dist(exit_pos, { nr, nc }) >= min_d) continue; // 가까워 지지 않는 경우
            tmp = { nr, nc }, min_d = manhattan_dist(exit_pos, { nr, nc }), did_move = true;
        }

        if (manhattan_dist(exit_pos, tmp) == 0) escaped_users.insert(user);
        if (did_move) {
            u = tmp;
            acc_move[user]++;
        }
    }

    for (int user : escaped_users) {
        remain_users.erase(user);
    }
}

bool check_box(int len, int r, int c) {
    bool exist_exit = false, exist_user = false;

    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (exit_pos.r == r + i && exit_pos.c == c + j) exist_exit = true;
            if (grid_user[r + i][c + j] != 0) exist_user = true;
        }
    }

    if (exist_exit && exist_user) return true;
    return false;
}

void rotate_user(int user, int len, int r, int c) {
    auto& u = users[user];
    for (int i = 1; i < len + 1; ++i) {
        for (int j = 1; j < len + 1; ++j) {
            if (u.r == r + len - j && u.c == c + i - 1) {
                u = { i + r - 1, j + c - 1};
                return;
            }
        }
    }
}

void rotate_exit(int len, int r, int c) {
    for (int i = 1; i < len + 1; ++i) {
        for (int j = 1; j < len + 1; ++j) {
            if (exit_pos.r == r + len - j && exit_pos.c == c + i - 1) {
                exit_pos = { i+ r - 1, j + c - 1};
                return;
            }
        }
    }
}

void rotate_box(int len, int r, int c) {
    vector<vector<int> > tmp;

    for (int user : remain_users) {
        rotate_user(user, len, r, c);
    }

    rotate_exit(len, r, c);

    tmp.resize(len + 1);
    for (int i = 1; i < len + 1; ++i) {
        tmp[i].resize(len + 1);
        for (int j = 1; j < len + 1; ++j) {
            tmp[i][j] = grid[r + len - j][c + i - 1];
        }
    }

    for (int i = 1; i < len + 1; ++i) {
        for (int j = 1; j < len + 1; ++j) {
            grid[r + i - 1][c + j - 1] = tmp[i][j];
        }
    }
}

void damage_wall(int len, int r, int c) {
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (grid[r + i][c + j] != 0) grid[r + i][c + j]--;
        }
    }
}

void rotate_maze() {
    // 정사각형 잡기
    fill_2d(grid_user, 0);

    for (int user : remain_users) {
        POS u = users[user];
        grid_user[u.r][u.c] = 1;
    }

    for (int len = 2; len <= N; ++len) {
        for (int i = 1; i <= N - len + 1; ++i) {
            for (int j = 1; j <= N - len + 1; ++j) {
                if (check_box(len, i, j)) {
                    rotate_box(len, i, j);    // 시계 방향 90도 회전                                       
                    damage_wall(len, i, j);    // 내구도 감소
                    return;
                }
            }
        }
    }
}

void run() {
    while (K--) {
        move_users();
        rotate_maze();
        if (remain_users.empty()) break; // 모두 탈출한다면 게임 종료
    }

    cout << accumulate(acc_move.begin(), acc_move.end(), 0) << endl;
    cout << exit_pos.r << ' ' << exit_pos.c;
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
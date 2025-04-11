/*
*   삼성 SW 역량테스트 2021 하반기 오후 1번 문제 / 팩맨 (L15)
*   Date: 2025-04-11 (목)
*   Duration: 1h 37m
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <numeric>
#include <algorithm>
#include <iomanip>

#define endl '\n'
#define INF 987654321
using namespace std;

// ----------------- 변수 -----------------
struct POS {
    int r, c;
};

struct INFO {
    int r, c, d;
    bool is_egg, is_dead;
};

int N = 4; // 격자 크기
int M; // 몬스터 수
int T; // 진행 턴 수
int cur_turn; // 현재 턴 수
int dr[] = { -1, -1, 0, 1, 1, 1, 0, -1 }; // 상, 좌상, 좌, 좌하, 하, 우하, 우, 우상
int dc[] = { 0, -1, -1, -1, 0, 1, 1, 1 };
POS pacman;
vector<INFO> monsters;
vector<vector<int> > grid, grid_dead, grid_alive;

// ----------------- 보조 함수 -----------------
void fill_2d_int(vector<vector<int> >& vec, int data) {
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            vec[i][j] = data;
        }
    }
}

bool is_grid(POS p) {
    if (p.r < 0 || p.c < 0 || p.r >= N || p.c >= N) return false;
    return true;
}

// ----------------- 메인 함수 -----------------
void init() {
    grid.resize(N), grid_dead.resize(N), grid_alive.resize(N);
    for (int i = 0; i < N; ++i) {
        grid[i].resize(N), grid_dead[i].resize(N), grid_alive[i].resize(N);
    }
}

void input() {
    cin >> M >> T;

    // 팩맨 위치
    cin >> pacman.r >> pacman.c;
    pacman.r--, pacman.c--;

    // 몬스터 위치
    monsters.resize(M);
    for (int i = 0; i < M; ++i) {
        INFO& m = monsters[i];
        cin >> m.r >> m.c >> m.d;
        m.r--, m.c--, m.d--;
        m.is_egg = false, m.is_dead = false;
    }
    cur_turn = 0;
}

void copy_monsters() {
    int size = monsters.size();
    for (int i = 0; i < size; ++i) {
        INFO& m = monsters[i];
        if (m.is_dead) continue;
        monsters.push_back({ m.r, m.c, m.d, true, false });
    }
}

void move_monsters() {
    int size = monsters.size();
    for (int i = 0; i < size; ++i) {
        INFO& m = monsters[i];
        if (m.is_egg || m.is_dead) continue;

        for (int dir = 0; dir < 8; ++dir) {
            POS np = { m.r + dr[(m.d + dir) % 8], m.c + dc[(m.d + dir) % 8] };
            if (!is_grid({ np.r, np.c }) || grid_dead[np.r][np.c] >= cur_turn) continue; // 격자 벗어나거나 시체 있는 경우
            if (np.r == pacman.r && np.c == pacman.c) continue; // 팩맨 있는 경우
            m = { np.r, np.c, (m.d + dir) % 8, m.is_egg, m.is_dead};
            break;
        }
    }
}

void move_pacman() {
    fill_2d_int(grid_alive, 0);
    int size = monsters.size();
    for (int i = 0; i < size; ++i) {
        INFO& m = monsters[i];
        if (m.is_egg || m.is_dead) continue;
        grid_alive[m.r][m.c]++;
    }

    POS p = { -1, -1 }, p1, p2;
    int max_cnt = 0;
    bool is_first = true;

    for (int dir_1 = 0; dir_1 < 8; dir_1 += 2) {
        POS p1_tmp = { pacman.r + dr[dir_1], pacman.c + dc[dir_1] };
        for (int dir_2 = 0; dir_2 < 8; dir_2 += 2) {
            POS p2_tmp = { p1_tmp.r + dr[dir_2], p1_tmp.c + dc[dir_2] };
            for (int dir_3 = 0; dir_3 < 8; dir_3 += 2) {
                POS p3_tmp = { p2_tmp.r + dr[dir_3], p2_tmp.c + dc[dir_3] };
                if (!is_grid(p1_tmp) || !is_grid(p2_tmp) || !is_grid(p3_tmp)) continue;
                if (is_first) {
                    is_first = !is_first;
                    p1 = p1_tmp, p2 = p2_tmp, p = p3_tmp;
                }

                int cnt = grid_alive[p1_tmp.r][p1_tmp.c];
                if (p2_tmp.r != pacman.r || p2_tmp.c != pacman.c) cnt += grid_alive[p2_tmp.r][p2_tmp.c];
                if (p3_tmp.r != p1_tmp.r || p3_tmp.c != p1_tmp.c) cnt += grid_alive[p3_tmp.r][p3_tmp.c];

                if (cnt > max_cnt) {
                    p1 = p1_tmp, p2 = p2_tmp, p = p3_tmp, max_cnt = cnt;
                }
            }
        }
    }
  
    pacman = p;
    if (grid_alive[p1.r][p1.c] > 0) grid_dead[p1.r][p1.c] = cur_turn + 2;
    if (grid_alive[p2.r][p2.c] > 0) grid_dead[p2.r][p2.c] = cur_turn + 2;
    if (grid_alive[p.r][p.c] > 0) grid_dead[p.r][p.c] = cur_turn + 2;

    for (int i = 0; i < size; ++i) {
        INFO& m = monsters[i];
        if (m.is_egg) continue;
        if ((m.r == p1.r && m.c == p1.c) || (m.r == p2.r && m.c == p2.c) || (m.r == p.r && m.c == p.c)) {
            m.is_dead = true;
        }
    }
}

void wakeup_eggs() {
    int size = monsters.size();
    for (int i = 0; i < size; ++i) {
        INFO& m = monsters[i];
        if (m.is_dead) continue; // 사망한 경우
        m.is_egg = false;
    }
}

void run() {
    while (T--) {
        cur_turn++;
        copy_monsters(); // 1. 몬스터 복제
        move_monsters(); // 2. 몬스터 이동
        move_pacman(); // 3. 팩맨 이동
        wakeup_eggs(); // 4. 몬스터 복제 완성
    }

    int cnt = 0;
    for (auto m : monsters) {
        if (!m.is_egg && !m.is_dead) cnt++;
    }
    cout << cnt;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    freopen("input.txt", "r", stdin);
    init();
    input();
    run();
    return 0;
}
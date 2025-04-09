/*
*   삼성 SW 역량테스트 2022 상반기 오전 1번 문제 / 술래잡기 (L15)
*   Date: 2025-04-09 (수)
*   Duration:
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <queue>
#include <numeric>
#include <algorithm>
#include <set>
#include <iomanip>

#define endl '\n'
#define INF 987654321
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
using namespace std;

// --------------------- 변수 선언 ---------------------
struct PLAYER {
    int r, c, dir;
};

struct POS {
    int r, c;
};

struct WATCHER {
    int r, c, dir, len, cnt;
    bool is_twice, is_rev;
};

int N; // 격자 크기
int M; // 도망자 수
int H; // 나무 개수
int K; // 전체 턴수
int acc_score; // 술래가 얻는 총 점수
int cur_turn; // 현재 턴
int dr[] = { -1, 0, 1, 0 }; // 북, 동, 남, 서 
int dc[] = { 0, 1, 0, -1 };
string dir_str[] = {"북", "동", "남", "서"};

POS center;
WATCHER watcher;
vector<vector<int> > grid;
vector<PLAYER> players;
set<int> alive_players;


// --------------------- 디버깅 함수 ---------------------
void traverse_2d_int(const vector<vector<int> >& vec) {
    cout << "[Traverse]" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            cout << vec[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

void traverse_players() {
    cout << "[Traverse players]" << endl;
    for (int i : alive_players) cout << "player " << i << ": " << players[i].r << ' ' << players[i].c << ' ' << dir_str[players[i].dir] << endl;
}

void traverse_watcher() {
    cout << "[Watcher Info]" << endl;
    cout << "r, c: " << watcher.r << ' ' << watcher.c << endl;
    cout << "dir, len, cnt, is_twice, is_rev: " << dir_str[watcher.dir] << ' ' << watcher.len << ' ' << watcher.cnt << ' ';
    cout << watcher.is_twice << ' ' << watcher.is_rev << endl;
}

// --------------------- 보조 함수 ---------------------
void fill_2d_int(vector<vector<int> >& vec, int data) {
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            vec[i][j] = data;
        }
    }
}

int manhattan_dist(POS a, POS b) {
    return abs(a.r - b.r) + abs(a.c - b.c);
}

bool is_grid(POS p) {
    if (p.r < 0 || p.c < 0 || p.r >= N || p.c >= N) return false;
    return true;
}

// --------------------- 메인 함수 ---------------------
void init() {
    acc_score = 0, cur_turn = 0;
    watcher.dir = 0, watcher.len = 2, watcher.cnt = 1, watcher.is_twice = false, watcher.is_rev = false;
}

void input() {
    cin >> N >> M >> H >> K;

    grid.resize(N);
    for (int i = 0; i < N; ++i) grid[i].resize(N);

    // 중앙 위치
    center = { N / 2, N / 2 };

    // 술래 위치
    watcher.r = center.r, watcher.c = center.c;

    // 도망자 위치
    players.resize(M);
    for (int i = 0; i < M; ++i) {
        alive_players.insert(i);
        cin >> players[i].r >> players[i].c >> players[i].dir;
        players[i].r--, players[i].c--;
    }

    // 나무 위치
    int r, c;
    for (int i = 0; i < H; ++i) {
        cin >> r >> c;
        grid[r - 1][c - 1] = 1;
    }
}

void move_players() {
    for (int player : alive_players) {
        PLAYER& p = players[player];        
        if (manhattan_dist({ p.r, p.c }, { watcher.r, watcher.c }) > 3) continue; // 거리 3 이하인 도망자만 이동

        POS np = { p.r + dr[p.dir], p.c + dc[p.dir] };
        // 방향 전환
        if (!is_grid(np)) {
            p.dir = (p.dir + 2) % 4;
            np = { p.r + dr[p.dir], p.c + dc[p.dir] };
        }

        // 술래 있다면 이동 X
        if (np.r == watcher.r && np.c == watcher.c) continue;
       
        // 술래 없으면 이동 (나무 있어도 ok)
        p.r = np.r, p.c = np.c;
    }
}

void move_watcher() {
    WATCHER& w = watcher;
    // 위치 이동
    w.r += dr[w.dir], w.c += dc[w.dir], w.cnt++;

    if (w.r == 0 && w.c == 0) {
        w.dir = SOUTH, w.len = N, w.cnt = 1, w.is_twice = false, w.is_rev = true;
        return;
    }

    else if (w.r == center.r && w.c == center.c) {
        w.dir = NORTH, w.len = 2, w.cnt = 1, w.is_twice = false, w.is_rev = false;
        return;
    }

    // 시야 회전
    if (w.cnt == w.len) {
        w.dir = w.is_rev ? (w.dir + 3) % 4 : (w.dir + 1) % 4;
        w.cnt = 1;
        if (w.is_twice) w.len = w.is_rev ? w.len - 1 : w.len + 1;
        w.is_twice = !w.is_twice;
    }
}

void catch_players() {
    set<int> killed_players;
    int cnt = 0;

    // 격자 크기 관계없이 시야 3칸 고정
    for (int len = 0; len <= 2; ++len) {
        POS np = { watcher.r + len * dr[watcher.dir], watcher.c + len * dc[watcher.dir] };

        if (!is_grid(np)) break;

        // 나무 가려진 경우 안 잡음
        if (grid[np.r][np.c] == 1) continue;
        
        for (int player : alive_players) {
            PLAYER pl = players[player];

            // cout << np.r << ' ' << np.c << ' ' << pl.r << ' ' << pl.c << endl;

            // 잡힌 술래는 사망
            if (pl.r == np.r && pl.c == np.c) {
                killed_players.insert(player);
                cnt++;
            }
        }
    }

    for (int player : killed_players) {
        alive_players.erase(player);
    }
        
    // 점수: 현재 턴 * 이번에 잡은 도망자 수
    acc_score += cur_turn * cnt;
}

void run() {
    while (K--) {
        cur_turn++;
        move_players(); // m명의 도망자 이동
        move_watcher(); // 술래 이동
        catch_players(); // 도망자 잡기

        // cout << endl << "Score of turn " << cur_turn << " : " << acc_score << endl;
        // traverse_players();
        // traverse_watcher();
    }

    cout << acc_score;
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
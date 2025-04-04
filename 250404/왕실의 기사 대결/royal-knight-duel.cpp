/*
*   삼성 SW 역량테스트 2023 하반기 오전 1번 문제 / 왕실의 기사 대결 (L13)
*   Date: 2025-04-01 (화)
*   Duration: 2h 15m
*/

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <string>

#define endl '\n'
#define INF 987654321

#define EMPTY 0
#define TRAP 1
#define WALL 2

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

using namespace std;

// ------------------ 변수 선언 ------------------
int L; // 체스판 크기 (3 ~ 40)
int N; // 기사 수 (1 ~ 30)
int Q; // 명령 수 (1 ~ 100)
vector<vector<int > > grid, grid_knight, visited;
int dr[4] = { -1, 0, 1, 0 }; // 위, 오른쪽, 아래, 왼쪽
int dc[4] = { 0, 1, 0, -1 };

struct KNIGHT {
    int r, c, h, w, k;
};

struct NODE {
    int r, c, idx;
};

vector<int> damage_acc;
vector<KNIGHT> knights;
set<int> alive_knights, pushed_knights;

// ------------------ 디버깅 함수 ------------------
void traverse_1d(const vector<int> & v) {
    cout << "[Traverse]" << endl;
    for (int i = 1; i < v.size(); ++i) {
        cout << v[i] << ' ';
    }
    cout << endl;
}

void traverse_2d(const vector<vector<int> > & v) {
    cout << "[Traverse]" << endl;
    for (int i = 1; i < v.size(); ++i) {
        for (int j = 1; j < v[i].size(); ++j) {
            cout << v[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

void traverse_grid() {
    cout << "[Traverse]" << endl;
    for (int i = 1; i < grid.size(); ++i) {
        for (int j = 1; j < grid[i].size(); ++j) {
            if (grid[i][j] == EMPTY) cout << "□" << ' ';
            else if (grid[i][j] == WALL) cout << "■" << ' ';
            else if (grid[i][j] == TRAP) cout << "★" << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

// ------------------ 보조 함수 ------------------
void fill_2d(vector<vector<int> > & v, int data) {
    for (int i = 1; i < v.size(); ++i) {
        for (int j = 1; j < v[i].size(); ++j) {
            v[i][j] = data;
        }
    }
}

bool is_grid(int r, int c) {
    if (r <= 0 || c <= 0 || r > L || c > L) return false;
    return true;
}

// ------------------ 메인 함수 ------------------
void init() {

}

void input() {
    cin >> L >> N >> Q;
    grid.resize(L + 1), grid_knight.resize(L + 1), damage_acc.resize(N + 1), visited.resize(L + 1);

    // 격자 정보
    for (int i = 1; i < L + 1; ++i) {
        grid[i].resize(L + 1), grid_knight[i].resize(L + 1), visited[i].resize(L + 1);

        for (int j = 1; j < L + 1; ++j) {
            cin >> grid[i][j];
        }
    }

    // 기사 정보
    knights.resize(N + 1);
    for (int i = 1; i <= N; ++i) {
        alive_knights.insert(i);
        cin >> knights[i].r >> knights[i].c >> knights[i].h >> knights[i].w >> knights[i].k;

        for (int h = 0; h < knights[i].h; h++) {
            for (int w = 0; w < knights[i].w; w++) {
                grid_knight[knights[i].r + h][knights[i].c + w] = i;
            }
        }
    }
    fill(damage_acc.begin(), damage_acc.end(), 0);
}

bool judge_move(int idx, int d) {
    pushed_knights.clear();
    fill_2d(visited, 0);

    KNIGHT k = knights[idx];
    queue<NODE> q;
    
    if (d == RIGHT || d == LEFT) {
        for (int i = 0; i < k.h; ++i) {
            q.push({ k.r + i, k.c, idx });
        }
    }

    else if (d == UP || d == DOWN) {
        for (int i = 0; i < k.w; ++i) {
            q.push({ k.r, k.c + i, idx });
        }
    }

    while (!q.empty()) {
        NODE top = q.front();
        q.pop();

        int nr = top.r + dr[d], nc = top.c + dc[d];
        if (!is_grid(nr, nc) || grid[nr][nc] == WALL) {
            pushed_knights.clear();
            return false;
        }
        if (visited[nr][nc]) continue;
        visited[nr][nc] = 1;

        int next = grid_knight[nr][nc];
        if (next == 0) continue;

        // 다른 기사 만난 경우
        if (next != top.idx) {
            pushed_knights.insert(next);

            if (d == RIGHT || d == LEFT) {
                for (int i = 0; i < knights[next].h; ++i) {
                    q.push({ knights[next].r + i, knights[next].c, next});
                }
            }

            else if (d == UP || d == DOWN) {
                for (int i = 0; i < knights[next].w; ++i) {
                    q.push({ knights[next].r, knights[next].c + i, next });
                }
            }
        }

        // 같은 기사인 경우
        else if (grid_knight[nr][nc] == top.idx) {
            q.push({ nr, nc, top.idx });
        }
    }

    return true;
}

bool move_knights() {
    int knight, dir;
    cin >> knight >> dir;

    // 사라진 기사일 경우 명령 무시
    if (alive_knights.find(knight) == alive_knights.end()) return false;

    // 해당 기사 해당 방향 이동 가능 여부 판단
    bool is_move = judge_move(knight, dir);

    // 이동 불가 시 명령 무시
    if (!is_move) return false;

    // 실제 이동
    fill_2d(grid_knight, 0);

    knights[knight].r += dr[dir], knights[knight].c += dc[dir];

    for (int i : pushed_knights) {
        KNIGHT& k = knights[i];
        k.r += dr[dir], k.c += dc[dir];
    }

    for (int i : alive_knights) {
        KNIGHT& k = knights[i];

        for (int h = 0; h < k.h; h++) {
            for (int w = 0; w < k.w; w++) {
                grid_knight[k.r + h][k.c + w] = i;

            }
        }
    }

    return true;
}

void calc_damage() {
    set<int> killed_kights;

    // 밀려난 기사들 데미지 계산
    for (int i : pushed_knights) {
        KNIGHT& k = knights[i];
        int cur_dam = 0;

        for (int h = 0; h < k.h; h++) {
            for (int w = 0; w < k.w; w++) {
                if (grid[k.r + h][k.c + w] == TRAP) cur_dam++;
            }
        }

        k.k -= cur_dam, damage_acc[i] += cur_dam;

        // 현재 체력 이상 데미지 받은 경우 사망
        if (k.k <= 0) {
            killed_kights.insert(i);

            // grid에서도 제거
            for (int h = 0; h < k.h; h++) {
                for (int w = 0; w < k.w; w++) {
                    grid_knight[k.r + h][k.c + w] = 0;
                }
            }
        }
    }

    for (int i : killed_kights) {
        alive_knights.erase(i);
    }
}

void acc_damage() {
    int acc = 0;

    // 생존한 기사들의 총 데미지 합 출력
    for (int i : alive_knights) {
        acc += damage_acc[i];
    }
    cout << acc;
}

void run() {
    while (Q--) {
        if(move_knights()) calc_damage();
    }
    acc_damage();
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
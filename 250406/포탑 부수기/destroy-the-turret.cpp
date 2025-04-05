/*
*   삼성 SW 역량테스트 2023 상반기 오전 1번 문제 / 포탑 부수기 (L15)
*   Date: 2025-04-05 (토)
*   Duration: 
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <numeric>
#include <algorithm>
#include <iomanip>

#define endl '\n'
#define INF 987654321
#define EAST 0
#define SOUTH 1
#define WEST 2
#define NORTH 3
#define SE 4
#define SW 5
#define NW 6
#define NE 7

using namespace std;

// ------------------ 변수 선언 ------------------
struct POS {
    int r, c;
};

struct NODE {
    int atk, turn, r, c;
};

int N; // 격자 세로 길이
int M; // 격자 가로 길이
int K; // 전체 턴
int cur_turn; // 현재 턴
int alive_towers; // 살아있는 포탑 수
int dr[] = { 0, 1, 0, -1, 1, 1, -1, -1 }; // 우, 하, 좌, 상, 남동, 남서, 북서, 북동
int dc[] = { 1, 0, -1, 0, 1, -1, -1, 1};

vector<vector<int> > grid, attack_history, cur_asso;
vector<vector<POS> > visited;

struct cmp_weak {
    bool operator()(const NODE& a, const NODE& b) {
        if (a.atk != b.atk) return a.atk > b.atk;
        if (a.turn != b.turn) return a.turn < b.turn;
        if (a.r + a.c != b.r + b.c) return a.r + a.c < b.r + b.c;
        return a.c < b.c;
    }
};

struct cmp_strong {
    bool operator()(const NODE& a, const NODE& b) {
        if (a.atk != b.atk) return a.atk < b.atk;
        if (a.turn != b.turn) return a.turn > b.turn;
        if (a.r + a.c != b.r + b.c) return a.r + a.c > b.r + b.c;
        return a.c > b.c;
    }
};

POS pos_s, pos_w;
priority_queue<NODE, vector<NODE>, cmp_weak> pq_w;
priority_queue<NODE, vector<NODE>, cmp_strong> pq_s;

// ------------------ 디버깅 함수 ------------------
void traverse_2d_int(const vector<vector<int> >& vec) {
    cout << "[Traverse 2d]" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            cout << setw(2);
            cout << vec[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

void traverse_2d_pos(const vector<vector<POS> >& vec) {
    cout << "[Traverse 2d]" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            cout << setw(5);
            cout << vec[i][j].r << ',' << vec[i][j].c << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// ------------------ 보조 함수 ------------------

/*
*   @brief vec를 data로 채우는 함수
    @param vec: 벡터, data: 채우고자 하는 값
*/
void fill_2d_int(vector<vector<int> >& vec, int data) {
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            vec[i][j] = data;
        }
    }
}

void fill_2d_pos(vector<vector<POS> >& vec, POS data) {
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            vec[i][j] = data;
        }
    }
}

bool is_grid(int r, int c) {
    if (r < 0 || c < 0 || r >= N || c >= M) return false;
    return true;
}

POS transformer(int r, int c, int dir) {
    POS p = { r, c };

    // 1. 우
    if (dir == EAST && c == M) p.c = 0;

    // 2. 하
    else if (dir == SOUTH && r == N) p.r = 0;

    // 3. 좌
    else if (dir == WEST && c == -1) p = { r, M - 1 };

    // 4. 상
    else if (dir == NORTH && r == -1) p = { N - 1, c };

    // 5. 남동
    else if (dir == SE) {
        if (r == N) p.r = 0;
        if (c == M) p.c = 0;
    }

    // 6. 남서
    else if (dir == SW) {
        if (r == N) p.r = 0;
        if (c == -1) p.c = M - 1;
    }

    // 7. 북서
    else if (dir == NW) {
        if (r == -1) p.r = N - 1;
        if (c == -1) p.c = M - 1;
    }

    // 8. 북동
    else if (dir == NE) {
        if (r == -1) p.r = N - 1;
        if (c == M) p.c = 0;
    }

    // 이 외에는 에러 케이스
    else {
        cout << "Error occured with transformer" << endl;
        p = { -1, -1 };
    }

    return p;
}

// ------------------ 메인 함수 ------------------
void init() {
    cur_turn = 0;
}

void input() {
    cin >> N >> M >> K;
    grid.resize(N), visited.resize(N), attack_history.resize(N), cur_asso.resize(N);

    for (int i = 0; i < N; ++i) {
        grid[i].resize(M), visited[i].resize(M), attack_history[i].resize(M), cur_asso[i].resize(M);

        for (int j = 0; j < M; ++j) {
            cin >> grid[i][j];
        }
    }
}

void select_weak() {

    while (!pq_w.empty()) pq_w.pop();

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (grid[i][j] <= 0) continue; // 부서진 경우
            pq_w.push({ grid[i][j], attack_history[i][j], i, j });
        }
    }

    pos_w = { pq_w.top().r, pq_w.top().c };
}

void select_strong(bool except_self) {
    while (!pq_s.empty()) pq_s.pop();
    // cout << pq_s.size();

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (grid[i][j] <= 0) continue; // 부서진 경우
            pq_s.push({ grid[i][j], attack_history[i][j], i, j });
        }
    }

    if (except_self) {
        while (!pq_s.empty()) {
            auto top = pq_s.top();
            pq_s.pop();
            if (top.r == pos_w.r && top.c == pos_w.c) continue;
            pos_s = { top.r, top.c };
            break;
        }
    }

    else {
        pos_s = { pq_s.top().r, pq_s.top().c };
    }
}

// 1. 공격자 선정
void select_attack() {
    select_weak();

    // 선정 후 공격력 증가
    grid[pos_w.r][pos_w.c] += (N + M);

    attack_history[pos_w.r][pos_w.c] = cur_turn;

    cur_asso[pos_w.r][pos_w.c] = 1;
}


bool attack_razer() {
    fill_2d_pos(visited, {-1, -1});
    visited[pos_w.r][pos_w.c] = { pos_w.r, pos_w.c };
    queue<POS> q;
    q.push(pos_w);

    while (!q.empty()) {
        POS top = q.front();
        q.pop();

        for (int i = 0; i < 4; ++i) {
            int nr = top.r + dr[i], nc = top.c + dc[i];
            if (!is_grid(nr, nc)) { // 격자 벗어나면 별도 처리
                POS trans_pos = transformer(nr, nc, i);
                nr = trans_pos.r, nc = trans_pos.c;
            }

            if (grid[nr][nc] <= 0) continue;
            if (visited[nr][nc].r != -1 || visited[nr][nc].c != -1) continue;
            visited[nr][nc] = {top.r, top.c};

            if (nr == pos_s.r && nc == pos_s.c) break;
        
            q.push({ nr, nc });
        }
    }

    if (visited[pos_s.r][pos_s.c].r == -1 && visited[pos_s.r][pos_s.c].c == -1) return false;

    POS cur = pos_s;
    int dam = grid[pos_w.r][pos_w.c];
    grid[cur.r][cur.c] -= dam;
    dam /= 2;

    
    while (true) {
        cur_asso[cur.r][cur.c] = 1;
        cur = visited[cur.r][cur.c];
        if (cur.r == visited[cur.r][cur.c].r && cur.c == visited[cur.r][cur.c].c) break;
        grid[cur.r][cur.c] -= dam;

    }

    return true;
}

void attack_canon() {
    int dam = grid[pos_w.r][pos_w.c];
    grid[pos_s.r][pos_s.c] -= dam, cur_asso[pos_s.r][pos_s.c] = 1;
    
    dam /= 2;

    for (int dir = 0; dir < 8; ++dir) {
        int nr = pos_s.r + dr[dir], nc = pos_s.c + dc[dir];

        if (!is_grid(nr, nc)) {
            POS t = transformer(nr, nc, dir);
            nr = t.r, nc = t.c;
        }

        if (nr == pos_w.r && nc == pos_w.c) continue;
        grid[nr][nc] -= dam, cur_asso[nr][nc] = 1;

    }
}

// 2. 공격자의 공격
void attack_strong() {
    // 가장 강한 포탑 선정
    select_strong(true);

    // 레이저 공격 시도 후 실패시 대포 공격
    if (!attack_razer()) attack_canon();
}

void destroy_tower() {
    alive_towers = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (grid[i][j] > 0) {
                alive_towers++;
            }
        }
    }
}

void repair_tower() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (grid[i][j] > 0 && cur_asso[i][j] == 0) {
                grid[i][j] += 1;
            }
        }
    }
}

void run() {
    while (K--) {
        cur_turn++;
        fill_2d_int(cur_asso, 0);

        // 1. 공격자 선정
        select_attack();

        // 2. 공격자의 공격
        attack_strong();

        // 3. 포탑 부서짐
        destroy_tower();

        // 부서지지 않은 포탑 1개 이하 될 경우 바로 종료
        if (alive_towers <= 1) break;

        // 4. 포탑 정비
        repair_tower();
    }

    select_strong(false);
    
    // 가장 강한 포탑 공격력 출력
    cout << grid[pos_s.r][pos_s.c];
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
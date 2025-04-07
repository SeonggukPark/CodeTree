/*
*   삼성 SW 역량테스트 2022 하반기 오후 1번 문제 / 코드트리 빵 (L14)
*   Date: 2025-04-06 (일)
*   Duration: 1h 40m
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
using namespace std;

// ---------------- 변수 선언 ----------------
struct POS {
    int r, c;
};

struct NODE {
    int d, r, c;
};

struct cmp {
    bool operator()(const NODE& a, const NODE& b) {
        if (a.d != b.d) return a.d > b.d;
        if (a.r != b.r) return a.r < b.r;
        return a.c < b.c;
    }
};

priority_queue<NODE, vector<NODE>, cmp> pq;

int N; // 게임판 크기 (3 ~ 50)
int M; // 게임 턴 수 (1 ~ 1000)
int P; // 산타 수 (1 ~ 30)
int C; // 루돌프 힘 (1 ~ N)
int D; // 산타 힘 (1 ~ N)
int cur_turn;
int dr[] = { -1, 0, 1, 0, -1, 1, 1, -1 }; // 상, 우, 하, 좌, 우상, 우하, 좌하, 좌상
int dc[] = { 0, 1, 0, -1, 1, 1, -1, -1 };

vector<vector<int> > grid, visited;
vector<POS> santas; // 산타 위치 관리 용
vector<int> santas_scores, santas_status; // 산타 점수 누적 용, 산타 상태 관리 용
POS rudolf;

// ---------------- 디버깅 함수 ----------------
void traverse_2d_int(const vector<vector<int> >& vec) {
    cout << "[Traverse}" << endl;
    for (int i = 1; i < vec.size(); ++i) {
        for (int j = 1; j < vec.size(); ++j) {
            cout << vec[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

// ---------------- 보조 함수 ----------------
void fill_2d_int(vector<vector<int> >& vec, int data) {
    for (int i = 1; i < vec.size(); ++i) {
        for (int j = 1; j < vec.size(); ++j) {
            vec[i][j] = data;
        }
    }
}

int euclid_dist(POS p1, POS p2) {
    return pow((p1.r - p2.r), 2) + pow((p1.c - p2.c), 2);
}

bool is_grid(POS & p) {
    if (p.r <= 0 || p.c <= 0 || p.r > N || p.c > N) return false;
    return true;
}

int is_santas_exist(int self, POS& p) {
    for (int i = 1; i <= P; ++i) {
        if (i == self || santas_status[i] == -1) continue;
        if (p.r == santas[i].r && p.c == santas[i].c) return i;
    }
    return -1;
}

int count_alive_santas() {
    int cnt = 0;
    for (int i = 1; i <= P; ++i) {
        if (santas_status[i] != -1) cnt++;
    }

    return cnt;
}

// ---------------- 메인 함수 ----------------
void init() {
    while (!pq.empty()) pq.pop();
    cur_turn = 0;
}

void input() {
    cin >> N >> M >> P >> C >> D;
    cin >> rudolf.r >> rudolf.c;

    grid.resize(N + 1);
    for (int i = 1; i <= N; ++i) {
        grid[i].resize(N + 1);
    }

    int s_id = 0;
    santas.resize(P + 1), santas_scores.resize(P + 1), santas_status.resize(P + 1);
    for (int i = 1; i <= P; ++i) {
        cin >> s_id;
        cin >> santas[s_id].r >> santas[s_id].c;
    }
}

/*
*  @param from: 누가 / to: 누구를 / dir: 어느 방향으로 이동해서 충돌 했는지
*/

void inter_act(int idx, int dir) {
    int cur = is_santas_exist(idx, santas[idx]);

    while (true) {
        santas[cur].r += dr[dir], santas[cur].c += dc[dir];

        if (!is_grid(santas[cur])) { // 격자 벗어나거나는 경우
            santas_status[cur] = -1;
            break;
        }

        else if (is_santas_exist(cur, santas[cur]) == -1) { // 연쇄 충돌 끝난 경우
            break;
        }

        cur = is_santas_exist(cur, santas[cur]);
    }
}

void collapse(int from, int to, int dir) {
    int santa, score;

    // 루돌프가 친 경우
    if (from == 0) santa = to, score = C;
   
    // 산타가 친 경우
    else santa = from, dir = (dir + 2) % 4, score = D;
   

    POS& s_pos = santas[santa];
    santas_scores[santa] += score, santas_status[santa] = cur_turn + 1;
    s_pos.r += score * dr[dir], s_pos.c += score * dc[dir];

    // cout << "After hit, santa " << santa << " pos: " << s_pos.r << ' ' << s_pos.c << endl;

    if (!is_grid(s_pos)) santas_status[santa] = -1; // 격자 밖으로 밀려난 경우 사망
    else if(is_santas_exist(santa, s_pos) != -1) inter_act(santa, dir); // 다음 칸에 다른 산타 있을 경우 연쇄 충돌
}


void move_rudolf() {
    while (!pq.empty()) pq.pop();

    for (int i = 1; i <= P; ++i) {
        if (santas_status[i] == -1) continue; // 이미 사망한 경우 pass
        pq.push({ euclid_dist(santas[i], rudolf), santas[i].r, santas[i].c });
    }

    NODE tar = pq.top();

    // cout << tar.d << ' ' << tar.r << ' ' <<tar.c << endl;
    POS tar_pos = { tar.r, tar.c };
    int min_dist = INF, min_dir = -1;

    for (int i = 0; i < 8; ++i) {
        POS np = { rudolf.r + dr[i], rudolf.c + dc[i] };
        if (!is_grid(np)) continue;
        if (euclid_dist(np, tar_pos) < min_dist) {
            min_dist = euclid_dist(np, tar_pos), min_dir = i;

            // cout << min_dist << ' ' << dir_str[min_dir] << endl;
        }
    }

    // Exception 처리
    // if (min_dir == -1) cout << "Error occured at move_rudolf()" << endl;
    rudolf = { rudolf.r + dr[min_dir], rudolf.c + dc[min_dir] };

    //cout << "Pos of rudolf: " << rudolf.r << ' ' << rudolf.c << endl;

    // 충돌 처리
    int next = is_santas_exist(0, rudolf);
    if (next != -1) {
        collapse(0, next, min_dir);
    }
}

void move_santas() {
    for (int santa = 1; santa <= P; ++santa) {
        if (santas_status[santa] == -1 || santas_status[santa] >= cur_turn) continue; // 사망 or 기절한 산타 pass

        POS & cur_pos = santas[santa];
        int min_dist = euclid_dist(rudolf, cur_pos), min_dir = -1;
        //cout << "min_dist of santa" << santa << " : " << min_dist << endl;

        for (int i = 0; i < 4; ++i) {
            POS np = { cur_pos.r + dr[i], cur_pos.c + dc[i] };
            if (!is_grid(np) || is_santas_exist(santa, np) != -1) continue; // 다른 산타 있거나 격자 밖인 경우 pass
            if (euclid_dist(rudolf, np) < min_dist) {
                min_dist = euclid_dist(rudolf, np), min_dir = i;
            }
        }

        if (min_dir == -1) continue; // 이동 불가의 경우

        cur_pos.r += dr[min_dir], cur_pos.c += dc[min_dir];
       // cout << "New pos of santa " << santa << ": " << cur_pos.r << ' ' << cur_pos.c << endl;

        // 충돌 처리
        if (cur_pos.r == rudolf.r && cur_pos.c == rudolf.c) {
            collapse(santa, 0, min_dir);
        }
    }
}

void score_round() {
    for (int i = 1; i <= P; ++i) {
        if (santas_status[i] != -1) santas_scores[i]++;
    }
}

void run() {
    while (M--) {
        cur_turn++;
        move_rudolf();
        move_santas();
        if (count_alive_santas() == 0) break;
        score_round();
    }

    for (int i = 1; i <= P; ++i) cout << santas_scores[i] << ' ';
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
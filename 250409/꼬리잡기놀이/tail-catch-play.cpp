/*
*   삼성 SW 역량테스트 2022 상반기 오후 1번 문제 / 꼬리잡기놀이 (L15)
*   Date: 2025-04-08 (화)
*   Duration: 
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>

#define endl '\n'
#define INF 987654321
using namespace std;

// ---------------- 변수 정의 ---------------- 
struct POS {
    int r, c;
};

struct TEAM {
    POS h, t; // head, tail;
};

constexpr int EMPTY = 0, HEAD = 1, OTHER = 2, TAIL = 3, ROAD = 4; // 별명 정의
int N; // 격자 크기 (3 ~ 20)
int M; // 팀 개수 (1 ~ 5)
int K; // 라운드 수 (1 ~ 1000)
int team_idx, cur_turn, acc_score;
int dr[] = { 0, -1, 0, 1 }; // 동, 북, 서, 남
int dc[] = { 1, 0, -1, 0 };

vector<vector<int> > grid, people, visited;
vector<TEAM> teams;

// ---------------- 디버깅 함수 ----------------
void traverse_2d_int(const vector<vector<int> >& vec) {
    cout << "[Traverse]" << endl;
    for (int i = 0; i < vec.size(); ++i) {

        for (int j = 0; j < vec.size(); ++j) {
            cout << vec[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

void traverse_team() {
    cout << "[Traverse team]" << endl;
    for (int i = 1; i < teams.size(); ++i) {
        cout << "Team " << i << " head & tail: " << teams[i].h.r << ' ' << teams[i].h.c << ' ';
        cout << teams[i].t.r << ' ' << teams[i].t.c << endl;
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

bool is_grid(POS p) {
    if (p.r < 0 || p.c < 0 || p.r >= N || p.c >= N) return false;
    return true;
}

// ---------------- 메인 함수 ----------------
void init() {
    team_idx = 1, cur_turn = 0, acc_score = 0;
}

void input() {
    cin >> N >> M >> K;
    grid.resize(N), people.resize(N), visited.resize(N);
    for (int i = 0; i < N; ++i) {
        grid[i].resize(N), people[i].resize(N), visited[i].resize(N);
        for (int j = 0; j < N; ++j) {
            cin >> grid[i][j];
        }
    }

    teams.resize(M + 1); // team 번호 1 ~ M
}

POS find_cluster(POS p) {
    POS head = { p.r, p.c }, tail = { p.r, p.c };
    visited[p.r][p.c] = 1, people[p.r][p.c] = team_idx;

    int cnt = 1;
    while (true) {
        for (int dir = 0; dir < 4; ++dir) {
            int nr = tail.r + dr[dir], nc = tail.c + dc[dir];
            if (!is_grid({ nr, nc }) || visited[nr][nc] == 1 || grid[nr][nc] == ROAD || grid[nr][nc] == EMPTY) continue;
            if (grid[nr][nc] == TAIL && cnt == 1) continue;

            visited[nr][nc] = 1, people[nr][nc] = team_idx, tail = { nr, nc }, cnt++;
            if (grid[nr][nc] == OTHER) break;
            if (grid[nr][nc] == TAIL) return {nr, nc};
        }
    }
}

void cluster_teams() {
    fill_2d_int(visited, 0);

    // 팀 클러스터링
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] == HEAD) {
                teams[team_idx] = { {i, j}, find_cluster({i, j}) };
                team_idx++;
            }
        }
    }

    // 격자 재 설정
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] == 1 || grid[i][j] == 2 || grid[i][j] == 3) {
                grid[i][j] = ROAD;
            }
        }
    }
}

POS find_next_pos(int idx, bool is_head) {  
    POS cur;
    if (is_head) cur = { teams[idx].h.r, teams[idx].h.c };
    else  cur = { teams[idx].t.r, teams[idx].t.c };

    for (int i = 0; i < 4; ++i) {
        int nr = cur.r + dr[i], nc = cur.c + dc[i];
        if (!is_grid({ nr, nc })) continue;
        if (!is_head && (nr == teams[idx].h.r && nc == teams[idx].h.c)) continue;

        if ((is_head && people[nr][nc] == EMPTY && grid[nr][nc] == ROAD)
            || (!is_head && people[nr][nc] != 0 && grid[nr][nc] == ROAD )
            || (is_head && nr == teams[idx].t.r && nc == teams[idx].t.c)) return {nr, nc};
    }

    return cur;
}

void move_teams() {
    // 팀 순회
    for (int team = 1; team < teams.size(); ++team) {
       // cout << "Team " << team << ": ";

        // 1. 머리, 꼬리 이동 방향 찾기
        POS nh = find_next_pos(team, true), nt = find_next_pos(team, false);

        // 머리, 꼬리 정보 갱신 + people 격자 반영
        POS& h = teams[team].h, & t = teams[team].t;

       // cout << h.r << ' ' << h.c << ' ' << t.r << ' ' << t.c << " → ";
        //cout << nh.r << ' ' << nh.c << ' ' << nt.r << ' ' << nt.c << endl;

        if (nh.r == t.r && nh.c == t.c) {
            h = nh, t = nt;
            continue;
        }

        people[t.r][t.c] = 0;
        h = nh, t = nt;
        people[h.r][h.c] = team;
    }
}

void throw_ball() {
    // 공 시작점, 방향 설정
    int dir = ((cur_turn - 1 + (4 * N) % (4 * N)) / N) % 4;
    POS cur_ball;

    if (dir == 0)       cur_ball = { (cur_turn - 1) % N, 0 };
    else if (dir == 1)  cur_ball = { N - 1, (cur_turn - 1) % N };
    else if (dir == 2)  cur_ball = { N - 1 - (cur_turn - 1) % N, N - 1 };
    else                cur_ball = { 0, N - 1 - (cur_turn - 1) % N };

    // 공 발사
    while (is_grid(cur_ball) && people[cur_ball.r][cur_ball.c] == 0) {
        cur_ball.r += dr[dir], cur_ball.c += dc[dir];

    }

    fill_2d_int(visited, 0);

    if (is_grid(cur_ball)) { // 사람 맞은 경우 그룹 내 순서 찾고 점수화
        POS cur = teams[people[cur_ball.r][cur_ball.c]].h;
        visited[cur.r][cur.c] = 1;
        int idx = 1, cnt = 1;
        while (cur.r != cur_ball.r || cur.c != cur_ball.c) {
            for (int i = 0; i < 4; ++i) {
                int nr = cur.r + dr[i], nc = cur.c + dc[i];
                if (!is_grid({ nr, nc }) || people[nr][nc] == 0 || grid[nr][nc] != ROAD || visited[nr][nc] == 1) continue;
                if (teams[people[cur_ball.r][cur_ball.c]].t.r == nr && 
                    teams[people[cur_ball.r][cur_ball.c]].t.c == nc && cnt == 1) continue;

                cur = { nr, nc }, visited[nr][nc] = 1, cnt++;
                break;
            }

            idx++;
        }
        
        // cout << "Hit at " << cur_ball.r << ' ' << cur_ball.c << " whose idx is " << idx << " of groud " << people[cur_ball.r][cur_ball.c] << endl;
        acc_score += (idx * idx);

        POS& h = teams[people[cur_ball.r][cur_ball.c]].h, &t = teams[people[cur_ball.r][cur_ball.c]].t;
        swap(h, t);
    }
}

void run() {
    cluster_teams(); // 팀 클러스터링 + 격자 재 설정

    //traverse_2d_int(grid);
    //traverse_2d_int(people);
    while (K--) {
        cur_turn++;
        move_teams(); // 1. 각 팀은 머리사람 따라 한 칸 이동
        throw_ball(); // 2. 공 투척
        // traverse_2d_int(people);

    }

    // 각 팀이 얻은 점수 총합 출력
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
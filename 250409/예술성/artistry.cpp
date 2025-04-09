/*
*   삼성 SW 역량테스트 2022 상반기 오전 2번 문제 / 예술성 (L13)
*   Date: 2025-04-09 (수)
*   Duration: 대략 1h 30m
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <queue>

#define endl '\n'
using namespace std;

// ---------------------- 변수 선언 -----------------------------------
struct POS {
    int r, c;
};

struct NODE {
    int cnt, num;
};

vector<vector<int> > grid, grid_group, grid_22, visited;
int N; // 격자 크기
int dr[] = { 0, 0, 1, -1 };
int dc[] = { 1, -1, 0, 0 };

// ---------------------- 보조 함수 -----------------------------------
void fill_2d_int(vector<vector<int> >& vec, int data) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            vec[i][j] = data;
        }
    }
}

bool is_grid(POS p) {
    if (p.r < 0 || p.c < 0 || p.r >= N || p.c >= N) return false;
    return true;
}

// ---------------------- 메인 함수 -----------------------------------
void init() {
}

void input() {
    cin >> N;
    grid.resize(N), grid_group.resize(N), grid_22.resize(N), visited.resize(N);
    for (int i = 0; i < N; ++i) {
        grid[i].resize(N), grid_group[i].resize(N), grid_22[i].resize(N), visited[i].resize(N);
        for (int j = 0; j < N; ++j) {
            cin >> grid[i][j];
        }
    }
}

int score_art() {
    fill_2d_int(grid_group, -1), fill_2d_int(visited, 0);
    int acc = 0, node_idx = 0;
    vector<NODE> nodes;

    // 좌표 돌면서 group clustering + cluster 개수 구하기 + 해당 cluster에 속한 칸수 count + 해당 cluster 숫자 값 구하기
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (visited[r][c]) continue;
            visited[r][c] = 1;

            int num = grid[r][c], cell_cnt = 0;
            queue<POS> q;
            q.push({ r, c });

            while (!q.empty()) {
                POS top = q.front();
                q.pop();
                cell_cnt++;
                grid_group[top.r][top.c] = node_idx;

                for (int dir = 0; dir < 4; ++dir) {
                    POS np = { top.r + dr[dir], top.c + dc[dir] };
                    if (!is_grid(np) || visited[np.r][np.c] || grid[np.r][np.c] != num) continue;
                    visited[np.r][np.c] = 1;
                    q.push({ np.r, np.c });
                }
            }
            nodes.push_back({ cell_cnt, num });
            node_idx++;
        }
    }

    int cnt = nodes.size(); // cluster 개수

    // cluster 개수로 2차원 벡터 생성
    vector<vector<int>> edge_count(cnt, vector<int>(cnt));

    // 모든 cluster 쌍에 대해 격자 돌면서 맞닿은 변 개수 count ( ~ 2,829,124)
    for (int i = 0; i < cnt; ++i) {
        for (int j = i + 1; j < cnt; ++j) {
            for (int r = 0; r < N; ++r) {
                for (int c = 0; c < N; ++c) {
                    if (!is_grid({ r, c }) || grid_group[r][c] != i) continue;
                    for (int dir = 0; dir < 4; ++dir) {
                        POS np = { r + dr[dir], c + dc[dir] };
                        if (!is_grid(np) || grid_group[np.r][np.c] != j) continue;
                        edge_count[i][j]++;
                    }
                }
            }
        }
    }

    // 모든 cluster 쌍에 대해 점수 구해서 누적
    for (int i = 0; i < cnt; ++i) {
        for (int j = i + 1; j < cnt; ++j) {
            acc += (nodes[i].cnt + nodes[j].cnt) * nodes[i].num * nodes[j].num * edge_count[i][j];
        }
    }
    return acc;
}

void rotate_center() {
    int l = grid.size();
    
    // 회전
    for (int i = 0; i < l; ++i) grid_22[i][l / 2] = grid[l / 2][l - i - 1];
    for (int j = 0; j < l; ++j) grid_22[l / 2][j] = grid[j][l / 2];

    // grid 반영
    for (int i = 0; i < l; ++i) grid[i][l / 2] = grid_22[i][l / 2];
    for (int j = 0; j < l; ++j) grid[l / 2][j] = grid_22[l / 2][j];
}

/*
*   @brief p를 좌상단으로 하는 (n / 2)*(n / 2) 정사각형 회전
*/
void rotate_clock(POS p) {
    fill_2d_int(grid_22, 0);
    int l = grid_22.size() / 2;

    // 회전
    for(int i = 0; i < l; ++i){
        for (int j = 0; j < l; ++j) {
            grid_22[p.r + i][p.c + j] = grid[p.r + l - 1 - j][p.c + i];
        }
    }

    // grid 반영
    for (int i = 0; i < l; ++i) {
        for (int j = 0; j < l; ++j) {
            grid[p.r + i][p.c + j] = grid_22[p.r + i][p.c + j];
        }
    }
}

void rotate_grid() {
    // 십자 반시계 회전
    rotate_center(); 

    // 4개 정사각형 시계 회전
    rotate_clock({ 0, 0 }); 
    rotate_clock({ 1 + (N / 2), 0 });
    rotate_clock({ 0, 1 + (N / 2) });
    rotate_clock({ 1 + (N / 2), 1 + (N / 2) });
}

void run(){
    int acc = 0;
    for (int round = 0; round <= 3; ++round) {
        acc += score_art();
        rotate_grid();
    }
    cout << acc;
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
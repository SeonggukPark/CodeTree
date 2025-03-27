#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <algorithm>
#include <set>
#define endl '\n'
using namespace std;

//------------------------ 선언 부 ------------------------

// 구조체 선언
struct POS {
    int r, c;
};

struct PRI {
    int dist, dir;
};

// 변수 선언
constexpr int INF = 987654321;
int N, M;
int dr[4] = { -1, 1, 0, 0 }; // 상, 하, 좌, 우 순
int dc[4] = { 0, 0, -1, 1 };
POS start_pos, exit_pos, cur_pos;
vector<vector<int> > grid, cur_sight, man_cnt, change_sight;
vector<vector<POS> > visited;
vector<POS> mans;
queue<int> q;
stack<int> stk;
set<int> alive_mans, stunned_mans;

//------------------------ 보조 함수 ------------------------

// 2차원 int vector 출력
void traverse_2d_int(vector<vector<int>>& target) {
    cout << "Traverse 2d pos" << endl;

    for (int i = 0; i < target.size(); ++i) {
        for (int j = 0; j < target[i].size(); ++j) {
            cout << target[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

// 2차원 POS vector 출력
void traverse_2d_pos(vector<vector<POS>> & target) {
    cout << "Traverse 2d pos" << endl;

    for (int i = 0; i < target.size(); ++i) {
        for (int j = 0; j < target[i].size(); ++j) {
            cout << '{' << target[i][j].r << "," << target[i][j].c << "} ";
        }
        cout << endl;
    }
    cout << endl;
}

// 좌표 유효성 검증
bool is_grid(int r, int c) {
    if (r < 0 || c < 0 || r >= N || c >= N) return false;
    return true;
 }

// 2차원 int vector fill
void fill_2d_int(vector<vector<int>>& target, int data) {
    for (int i = 0; i < target.size(); ++i) {
        fill(target[i].begin(), target[i].end(), data);
    }
}

// 2차원 좌표에서 맨해튼 거리 반환
int manhattan_dist(POS& from, POS& to) {
    return abs(from.r - to.r) + abs(from.c - to.c);
}

void copy_2d_int(vector<vector<int>>& from, vector<vector<int>>& to) {
    for (int i = 0; i < from.size(); ++i) {
        for (int j = 0; j < from[i].size(); ++j) {
            to[i][j] = from[i][j];
        }
    }
}

//------------------------ 메인 함수 ------------------------

// 메두사 최단 경로 visited에 기록
void find_shortest_path() {
    queue<POS> tmp_q;
    tmp_q.push(start_pos);
    
    while (!tmp_q.empty()) {
        POS top = tmp_q.front();
        tmp_q.pop();

        for (int i = 0; i < 4; ++i) {
            int nr = top.r + dr[i], nc = top.c + dc[i];
            if (!is_grid(nr, nc) || grid[nr][nc] == 1) continue; // 격자 벗어나거나 막힌 경우
            if (visited[nr][nc].r != -1 || visited[nr][nc].c != -1) continue; // 이미 방문
            visited[nr][nc] = { top.r, top.c }; // 이전 좌표 기록
            if (nr == exit_pos.r && nc == exit_pos.c) return;
            tmp_q.push({ nr, nc });
        }
    }
}

// visited 추적 하면서 최단 경로 stk에 복원
void trace_shortest_path() {
    POS cur = exit_pos;

    while (cur.r != start_pos.r || cur.c != start_pos.c) { // 출발지 도착하면 추적 종료
        POS prev = visited[cur.r][cur.c];

        // 1. 이전: 우 (= 직전 이동 좌)
        if (prev.c > cur.c) stk.push(2);
        // 2. 이전: 좌 (= 직전 이동 우)
        else if (prev.c < cur.c) stk.push(3);
        // 3. 이전: 하 (= 직전 이동 상)
        else if (prev.r > cur.r) stk.push(0);
        // 4. 이전: 상 (= 직전 이동 하)
        else if (prev.r < cur.r) stk.push(1);

        cur = prev;
    }
}

// 1. 메두사 이동
void move_medusa() {
    int dir = stk.top(); stk.pop();
    cur_pos = { cur_pos.r + dr[dir], cur_pos.c + dc[dir] };

    // 전사와 만날 경우 해당 전사 사망
    set<int> dead_mans;
    for (int i : alive_mans) {
        if (mans[i].r == cur_pos.r && mans[i].c == cur_pos.c) dead_mans.insert(i);
    }

    for (int i : dead_mans) {
        alive_mans.erase(i);
    }
}

// 2. 메두사 시선 처리
int scan_medusa() {
    int cnt_rock = 0;
    fill_2d_int(man_cnt, 0);

    // 2차원 평면 상에 전사 숫자 새기기
    for (int man : alive_mans) {
        man_cnt[mans[man].r][mans[man].c]++;
    }

    // 상, 하, 좌, 우 순으로 메두사 시선 색칠
    // cout << "cur pos: " << cur_pos.r << ' ' << cur_pos.c << endl;

    for (int dir = 0; dir < 4; ++dir) {
        fill_2d_int(change_sight, 0);
        POS pivot = cur_pos;
        int len = 1;

        while (true) {
            pivot.r += dr[dir], pivot.c += dc[dir];
            if (!is_grid(pivot.r, pivot.c)) break;
            
            if (dir == 2 || dir == 3) { // 좌, 우
                for (int r = max(0, pivot.r - len); r <= min(N - 1, pivot.r + len); ++r) {
                    change_sight[r][pivot.c] = 1;
                }
            }

            else if (dir == 0 || dir == 1) { // 상, 하
                for (int c = max(0, pivot.c - len); c <= min(N - 1, pivot.c + len); ++c) {
                    change_sight[pivot.r][c] = 1;
                }
            }

            len++;
        }


        // 가리는 것 제외
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                // 해당 위치에 전사 존재 + 시선 구역일 경우
                if (man_cnt[i][j] > 0 && change_sight[i][j] == 1) {
                    int man_len = 1;

                    if (dir == 0) { // 상
                        for (int r = i - 1; r >= 0; --r) {
                            if (j == cur_pos.c) {
                                change_sight[r][j] = 0;
                            }

                            else if (j < cur_pos.c) {
                                for (int c = max(0, j - man_len); c <= j; ++c) {
                                    change_sight[r][c] = 0;
                                }
                            }

                            else {
                                for (int c = j; c <= min(N - 1, j + man_len); ++c) {
                                    change_sight[r][c] = 0;
                                }
                            }

                            man_len++;
                        }
                    }

                    else if (dir == 1) { // 하
                        for (int r = i + 1; r < N; ++r) {
                            if (j == cur_pos.c) {
                                change_sight[r][j] = 0;
                            }

                            else if (j < cur_pos.c) {
                                for (int c = max(0, j - man_len); c <= j; ++c) {
                                    change_sight[r][c] = 0;
                                }
                            }

                            else {
                                for (int c = j; c <= min(N - 1, j + man_len); ++c) {
                                    change_sight[r][c] = 0;
                                }
                            }
                            man_len++;
                        }
                    }

                    else if (dir == 2) { // 좌
                        for (int c = j - 1; c >= 0; --c) {
                            if (i == cur_pos.r) {
                                change_sight[i][c] = 0;
                            }

                            else if (i < cur_pos.r) {
                                for (int r = max(0, i - man_len); r <= i; ++r) {
                                    change_sight[r][c] = 0;
                                }
                            }

                            else {
                                for (int r = i; r <= min(N - 1, i + man_len); ++r) {
                                    change_sight[r][c] = 0;
                                }
                            }
                            man_len++;
                        }
                    }
                    else if (dir == 3) { // 우
                        for (int c = j + 1; c < N; ++c) {
                            if (i == cur_pos.r) {
                                change_sight[i][c] = 0;
                            }

                            else if (i < cur_pos.r) {
                                for (int r = max(0, i - man_len); r <= i; ++r) {
                                    change_sight[r][c] = 0;
                                }
                            }

                            else {
                                for (int r = i; r <= min (N - 1, i + man_len); ++r) {
                                    change_sight[r][c] = 0;
                                }
                            }
                            man_len++;
                        }
                    }                  
                }
            }
        }

        //traverse_2d_int(change_sight);


        // 볼 수 있는 사람 수 세기 
        int cur_cnt = 0;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (man_cnt[i][j] > 0 && change_sight[i][j] == 1) {
                    cur_cnt += man_cnt[i][j];
                }
            }
        }

        // 이전 보다 많다면, 개수 및 cur_sight 갱신
        if (cur_cnt > cnt_rock) {
            cnt_rock = cur_cnt;
            copy_2d_int(change_sight, cur_sight);
        }
    }

    // 기절 시키기
    stunned_mans.clear();

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (man_cnt[i][j] > 0 && cur_sight[i][j] == 1) {

                for (int man : alive_mans) {
                    if (mans[man].r == i && mans[man].c == j) {
                        stunned_mans.insert(man);
                    }
                }
            }
        }
    }

    return cnt_rock;
}

// 3. 전사 이동
int move_mans() {
    int cnt_dist = 0;

    // Phase_1
    for (int man : alive_mans) {
        if (stunned_mans.find(man) == stunned_mans.end()) {
            PRI move_dir = { manhattan_dist(mans[man], cur_pos), -1 };
            for (int i = 0; i < 4; ++i) {
                int nr = mans[man].r + dr[i], nc = mans[man].c + dc[i];
                if (!is_grid(nr, nc) || cur_sight[nr][nc] == 1) continue;

                POS new_pos = { nr, nc };
                int dist = manhattan_dist(cur_pos, new_pos);
                if (move_dir.dist > dist) {
                    move_dir = { dist, i };
                }
            }

            if (move_dir.dir == -1) continue; // 더 가까워 질 수 없는 경우
            mans[man].r += dr[move_dir.dir], mans[man].c += dc[move_dir.dir], cnt_dist++;
        }
    }

    // Phase_2
    int new_nr[4] = { 0, 0, -1, 1 }; // 좌, 우, 상, 하 순
    int new_nc[4] = { -1, 1, 0, 0 };

    for (int man : alive_mans) {
        if (stunned_mans.find(man) == stunned_mans.end()) {
            PRI move_dir = { manhattan_dist(mans[man], cur_pos), -1 };
            for (int i = 0; i < 4; ++i) {
                int nr = mans[man].r + new_nr[i], nc = mans[man].c + new_nc[i];
                if (!is_grid(nr, nc) || cur_sight[nr][nc] == 1) continue;

                POS new_pos = { nr, nc };
                int dist = manhattan_dist(cur_pos, new_pos);
                if (move_dir.dist > dist) {
                    move_dir = { dist, i };
                }
            }

            if (move_dir.dir == -1) continue; // 더 가까워 질 수 없는 경우
            mans[man].r += new_nr[move_dir.dir], mans[man].c += new_nc[move_dir.dir], cnt_dist++;
        }
    }

    return cnt_dist;
}

// 4. 전사 공격
int attack_mans() {
    int cnt_attack = 0;
    set<int> dead_mans;

    for (int man : alive_mans) {
        // cout << mans[man].r << ' ' << mans[man].c << endl;

        if (mans[man].r == cur_pos.r && mans[man].c == cur_pos.c) { // 메두사와 같은 칸 도착한 경우
            cnt_attack++;
            dead_mans.insert(man);
        }
    }

    for (int man : dead_mans) {
        alive_mans.erase(man);
    }

    return cnt_attack;
}


// 초기화 함수
void init() {
    while (!q.empty()) q.pop();
    while (!stk.empty()) stk.pop();
    fill_2d_int(cur_sight, 0);
}

// 입력 함수
void input() {
    // 마을크기 N, 전사 수 M
    cin >> N >> M;

    // 메두사 시작 위치, 탈출 위치
    cin >> start_pos.r >> start_pos.c >> exit_pos.r >> exit_pos.c;

    // M명 전사 위치
    mans.resize(M);
    for (int i = 0; i < M; i++){
        cin >> mans[i].r >> mans[i].c;
        alive_mans.insert(i);
    }

    // 마을 위치
    grid.resize(N), visited.resize(N), cur_sight.resize(N), man_cnt.resize(N), change_sight.resize(N);
    for (int i = 0; i < N; ++i) {
        grid[i].resize(N), visited[i].resize(N), cur_sight[i].resize(N), man_cnt[i].resize(N), change_sight[i].resize(N);

        for (int j = 0; j < N; ++j) {
            cin >> grid[i][j];
        }

        // visited 초기화
        POS dummy = { -1, -1 };
        fill(visited[i].begin(), visited[i].end(), dummy);
    }

    // 메두사 현재 위치 = 시작 위치 초기화
    cur_pos = start_pos;
}

// 실행 함수
void run() {
    // 메두사 최단 경로 찾기
    find_shortest_path();

    // 경로 존재 안하면 -1 출력 후 리턴
    if (visited[exit_pos.r][exit_pos.c].r == -1 && visited[exit_pos.r][exit_pos.c].c == -1) {
        cout << -1;
        return;
    }


    // 경로 복원
    trace_shortest_path();

    int cnt_dist, cnt_rock, cnt_attack;
    while(stk.size() > 1){
        // 1. 메두사 이동
        move_medusa();

        // 2. 메두사 시선 처리
        cnt_rock = scan_medusa();

        // 3. 전사 이동
        cnt_dist = move_mans();

        // 4. 전사 공격
        cnt_attack = attack_mans();

        // 이번 턴 출력
        cout << cnt_dist << ' ' << cnt_rock << ' ' << cnt_attack << endl;
    }
    
    cout << 0; // 메두사 도착
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    init();
    input();
    run();

    return 0;
}
/*
*   삼성 SW 역량테스트 2022 하반기 오후 1번 문제 / 코드트리 빵 (L14)
*   Date: 2025-04-06 (일)
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
using namespace std;

// --------------- 변수 선언 ---------------
struct POS {
    int r, c;
};

struct NODE {
    int d, r, c, idx;
};

struct cmp {
    bool operator()(const NODE& a, const NODE& b) {
        if (a.d != b.d) return a.d > b.d;
        if (a.r != b.r) return a.r > b.r;
        return a.c > b.c;
    }
};

int N; // 격자 크기
int M; // 사람의 수
int dr[] = { -1, 0, 0, 1 }; // 상, 좌, 우, 하
int dc[] = { 0, -1, 1, 0 };
int base_idx; // 베이스 관리용 인덱스
int cur_time;  // 현재 시간

vector<vector<int> > grid, grid_locked;
vector<vector<POS> > visited;
vector<POS> convs, persons, bases;
set<int> alive_persons, alive_bases;


// --------------- 디버깅 함수 ---------------
void traverse_2d(const vector<vector<int> >& vec) {
    cout << "[Traverse]" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            cout << vec[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

void traverse_1d_pos(const vector<POS>& vec) {
    cout << "[Traverse]" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        cout << "Pos of " << i << ": " << vec[i].r << ' ' << vec[i].c << endl;
    }
    cout << endl;
}

void traverse_2d_pos(const vector<vector<POS> >& vec) {
    cout << "[Traverse]" << endl;
    for (int i = 0; i < vec.size(); ++i) {
        for (int j = 0; j < vec[i].size(); ++j) {
            cout << setw(4);
            cout << vec[i][j].r << ',' << vec[i][j].c << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

// --------------- 보조 함수 ---------------
void fill_2d_int(vector<vector<int> > & vec, int data) {
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

void fill_1d_pos(vector<POS>& vec, POS data) {
    for (int i = 0; i < vec.size(); ++i) {
        vec[i] = data;
    }
}

bool is_grid(POS p) {
    if (p.r < 0 || p.c < 0 || p.r >= N || p.c >= N) return false;
    return true;
}

// --------------- 메인 함수 ---------------
void init() {
    alive_persons.clear();
    base_idx = 0, cur_time = 0;
}

void input() {
    cin >> N >> M;

    // 격자 입력
    grid.resize(N), grid_locked.resize(N), visited.resize(N);
    for (int i = 0; i < N; ++i) {
        grid[i].resize(N), grid_locked[i].resize(N), visited[i].resize(N);
        for (int j = 0; j < N; ++j) {
            cin >> grid[i][j];
            if (grid[i][j] == 1) {
                alive_bases.insert(base_idx++);
                bases.push_back({ i, j });
            }
        }
    }

    // 가고 싶은 편의점 위치
    convs.resize(M), persons.resize(M);
    for (int i = 0; i < M; ++i) {
        cin >> convs[i].r >> convs[i].c;
        convs[i].r--, convs[i].c--;
    }

    fill_1d_pos(persons, { -1, -1 });
}

void bfs_mark(POS from, POS to) {
    fill_2d_pos(visited, { -1, -1 });

    queue<POS> q;
    q.push(from);
    visited[from.r][from.c] = { from.r, from.c };

    while (!q.empty()) {
        POS top = q.front();
        q.pop();

        for (int i = 0; i < 4; ++i) {
            POS np = { top.r + dr[i], top.c + dc[i] };
            if (!is_grid(np) || grid_locked[np.r][np.c] == 1) continue;
            if (visited[np.r][np.c].r != -1 || visited[np.r][np.c].c != -1) continue; // 이미 방문 한 경우 
            visited[np.r][np.c] = { top.r, top.c };
            q.push({ np.r, np.c });

            if (np.r == to.r && np.c == to.c) return;
        }
    }
}

/*
    @brief from에서 to까지 최단거리로 가기위해 from에서 다음으로 향해야 하는 방향 리턴
    @param from: 출발지 / to: 도착지

*/
int shortest_dir(POS from, POS to) {
    bfs_mark(from, to);

    // Tracing
    POS cur = to;
    while (true) {
        POS next = visited[cur.r][cur.c];

        if (next.r == from.r && next.c == from.c) {
            int dir = -1;

            if (cur.r < next.r && cur.c == next.c) dir = 0; // 상
            else if (cur.r == next.r && cur.c < next.c) dir = 1; // 좌
            else if (cur.r == next.r && cur.c > next.c) dir = 2; // 우
            else if (cur.r > next.r && cur.c == next.c) dir = 3; // 하
            else cout << "Error occured.." << endl;

            return dir;
        }
        
        cur = next;
    }
}

void move_persons() {
    for (int person : alive_persons) {

        POS& p = persons[person], & conv = convs[person];

        int dir = shortest_dir(p, conv);
        p = { p.r + dr[dir], p.c + dc[dir] };
    }
}

void lock_convs() {
    set<int> cleared_persons;

    for (int person : alive_persons) {
        POS& p = persons[person], &conv = convs[person];
        if (p.r == conv.r && p.c == conv.c) { // 해당 인원 편의점 도착한 경우
            cleared_persons.insert(person); 
            grid_locked[p.r][p.c] = 1; // 격자 잠그기
        }
    }

    for (int person : cleared_persons) {
        alive_persons.erase(person);
    }
}

void lock_basecamps() {
    int p = cur_time - 1;
    alive_persons.insert(p);

    priority_queue<NODE, vector<NODE>, cmp> pq;

    for (int base : alive_bases) {
        POS& b = bases[base], &conv = convs[p];
        bfs_mark(b, convs[p]);

        int dist = 0;
        POS cur = conv;

        while (true) {
            POS next = visited[cur.r][cur.c];
            dist++;
            if (next.r == b.r && next.c == b.c) break;
            cur = next;
        }

        pq.push({ dist, b.r, b.c, base });
    }

    NODE cur_base = pq.top();
    persons[p] = { cur_base.r, cur_base.c };
    grid_locked[cur_base.r][cur_base.c] = 1;
    alive_bases.erase(cur_base.idx);
}


void run() {
    while (true) {
        cur_time++;

        move_persons(); // 1. 편의점으로 이등
        lock_convs(); // 2. 편의점 도착한 인원들 처리 & 격자 잠그기

        if(cur_time <= M) lock_basecamps(); // 3. 해당 시간 베이스 캠프 투입 & 격자 잠그기

        // traverse_1d_pos(persons);

        if (alive_persons.empty()) break;
    }

    cout << cur_time;
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
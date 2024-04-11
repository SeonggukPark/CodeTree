#include <iostream>
#include <cstring>
#include <queue>
#define pii pair<int, int>
using namespace std;
constexpr size_t MAX_N = 20, MAX_M = 31;
int n, m, k;
int dx[4] = {-1, 0, 1, 0}; // 상, 우, 하, 좌
int dy[4] = {0, 1, 0, -1};
int gun[MAX_N][MAX_N];
int grid[MAX_N][MAX_N];
int points[MAX_M];

struct Player{
    int x, y, d, s, g;
};

Player players[MAX_M];

priority_queue<int> pq[MAX_N][MAX_N];

void traverse_gun(){
    cout << "traverse gun.. " << endl;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << gun[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}


void traverse_player(){
    cout << "traverse player.. " << endl;

    for (int i = 1; i <= m; ++i) {
        cout << "player " << i << ": " << players[i].x << ' ' << players[i].y << endl;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << grid[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

void init(){
    memset(gun, 0, sizeof(gun));
    memset(points, 0, sizeof(points));
    memset(grid, 0, sizeof(grid));

    for (auto & player : players) {
        player.x = player.y = player.d = player.s = player.g = 0;
    }

    for(auto & i : pq){
        for (auto & j : i) {
            j = {};
        }
    }
}

void input(){
    cin >> n >> m >> k;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> gun[i][j];
            pq[i][j].push(gun[i][j]);
        }
    }

    for (int i = 1; i <= m; ++i) {
        cin >> players[i].x >> players[i].y >> players[i].d >> players[i].s;
        players[i].x--, players[i].y--;

        grid[players[i].x][players[i].y] = i;
    }
}

void replace_gun(int x, int y){
    auto & player = players[grid[x][y]];

    // 1. player 총이 더 약할 경우 교체
    if(pq[x][y].top() > player.g){
        pq[x][y].push(player.g);
        player.g = pq[x][y].top();
        pq[x][y].pop();
    }
}

bool is_grid(int x, int y){
    if(x < 0 || y < 0 || x >= n || y >= n) return false;
    return true;
}

void phase_1(){
    for(int i = 1; i <= m; ++i){
        // traverse_player();

        auto & p = players[i];
        // cout << "player " << i << " pos:  " << p.x << ' ' << p.y <<endl;

        grid[p.x][p.y] = 0;
        int nx = p.x + dx[p.d], ny = p.y + dy[p.d];

        // 격자 벗어 나면 방향 반대 1 이동
        if(!is_grid(nx, ny)){
            p.d = (p.d + 2) % 4;
            // cout << "player " << i << " out.. new d: " << p.d << endl;
            nx = p.x + dx[p.d], ny = p.y + dy[p.d];
        }

        p.x = nx, p.y = ny;
        // cout << "px, py: " << p.x << ' ' << p.y << endl;

        // case 1. 이동한 좌표에 player X
        if(grid[p.x][p.y] == 0){
            // 이동
            grid[p.x][p.y] = i;

            // 해당 칸 총 확인
            // 총 없으면 아무 일 X
            if(pq[p.x][p.y].top() == 0) continue;

            // 플레이어 총이랑 비교 후 더 센총 고르기
            replace_gun(p.x, p.y);
        }

        // case 2. 이동한 좌표에 player O
        else{
            int idx_enemy = grid[p.x][p.y];
            auto & enemy = players[idx_enemy];

            int winner, loser, my_p = p.s + p.g, enemy_p = enemy.s + enemy.g;

            if(my_p == enemy_p) {
                if(p.s > enemy.s) winner = i, loser = idx_enemy;
                else winner = idx_enemy, loser = i;
            }

            else{
                if(my_p > enemy_p) winner = i, loser = idx_enemy;
                else winner = idx_enemy, loser = i;
            }

            // cout << "w, l : " << winner << ' ' << loser << endl;

            auto & w = players[winner];
            auto & l = players[loser];

            // 승자 점수 휙득
            points[winner] += abs(my_p - enemy_p);
            grid[nx][ny] = winner;

            // 패자 총 내려 놓기
            pq[nx][ny].push(l.g);
            l.g = 0;


            // 진 플레이어 이동
            for (int j = 0; j < 4; ++j) {
                int nnx = l.x + dx[l.d], nny = l.y + dy[l.d];
                if(!is_grid(nnx, nny) || grid[nnx][nny] != 0){
                    l.d = (l.d + 1) % 4;
                    continue;
                }

                // 이동 가능하면 패자 이동
                grid[nnx][nny] = loser;
                l.x = nnx, l.y = nny;

                // cout << "lx, ly: " << l.x << ' ' <<l.y << endl;

                // 패자 총이랑 비교 후 더 센총 고르기
                replace_gun(nnx, nny);
                break;
            }

            // 승자는 떨어진 총들 중 가장 공격력 높은 총 획득, 나머지는 내려 놓기
            // cout << "px, py: " << p.x << ' ' << p.y << endl;

            w.x = nx, w.y = ny;

            replace_gun(p.x, p.y);
        }
    }
}

void run(){
    while(k--){
        phase_1();
        //traverse_gun();
        //traverse_player();
    }

    for (int i = 1; i <= m; ++i) {
        cout << points[i] << ' ';
    }
}

int main() {
    //freopen("input.txt", "r", stdin);
    init();
    input();
    run();
    return 0;
}
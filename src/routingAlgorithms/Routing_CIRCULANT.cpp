#include "Routing_CIRCULANT.h"
#include "../GlobalParams.h"
#include <algorithm>
#include <queue>
#include <vector>
#include <cstdlib>
#include <limits>
#include <utility>

RoutingAlgorithmsRegister Routing_CIRCULANT::routingAlgorithmsRegister("CIRCULANT", getInstance());

Routing_CIRCULANT * Routing_CIRCULANT::routing_CIRCULANT = 0;

Routing_CIRCULANT * Routing_CIRCULANT::getInstance() {
	if (routing_CIRCULANT == 0)
		routing_CIRCULANT = new Routing_CIRCULANT();

	return routing_CIRCULANT;
}

static int bfs_distance_circulant(int start, int dst, int n, int s)
{
    if (start == dst)
        return 0;

    std::vector<bool> visited(n, false);
    std::queue<std::pair<int,int>> q; // {node, dist}

    visited[start] = true;
    q.push({start, 0});

    while (!q.empty()) {
        int u = q.front().first;
        int d = q.front().second;
        q.pop();

        const int plus1  = (u + 1) % n;
        const int minus1 = (u - 1 + n) % n;
        const int pluss  = (u + s) % n;
        const int minuss = (u - s + n) % n;

        const int neigh[4] = {plus1, pluss, minus1, minuss};

        for (int k = 0; k < 4; k++) {
            int v = neigh[k];
            if (!visited[v]) {
                if (v == dst)
                    return d + 1;
                visited[v] = true;
                q.push({v, d + 1});
            }
        }
    }

    return -1;
}

static void erase_direction(vector<int> &dirs, int dir)
{
    dirs.erase(std::remove(dirs.begin(), dirs.end(), dir), dirs.end());
}

static bool has_direction(const vector<int> &dirs, int dir)
{
    return std::find(dirs.begin(), dirs.end(), dir) != dirs.end();
}

static bool is_positive_direction(int dir)
{
    return (dir == DIRECTION_NORTH || dir == DIRECTION_EAST);
}

vector<int> Routing_CIRCULANT::route(Router * router, const RouteData & routeData)
{
    vector<int> directions;

    const int n = GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y;
    const int s = GlobalParams::circulant_step;

    const int cur = routeData.current_id;
    const int dst = routeData.dst_id;

    if (cur == dst) {
        directions.push_back(DIRECTION_LOCAL);
        return directions;
    }

    struct Candidate {
        int dir;
        int next;
    };

    vector<Candidate> cand;
    cand.push_back({DIRECTION_NORTH, (cur + 1) % n});
    cand.push_back({DIRECTION_EAST,  (cur + s) % n});
    cand.push_back({DIRECTION_SOUTH, (cur - 1 + n) % n});
    cand.push_back({DIRECTION_WEST,  (cur - s + n) % n});

    int best_total_dist = std::numeric_limits<int>::max();
    vector<int> best_dirs;

    for (const auto &c : cand) {
        int d = bfs_distance_circulant(c.next, dst, n, s);
        if (d < 0)
            continue;

        int total = 1 + d;

        if (total < best_total_dist) {
            best_total_dist = total;
            best_dirs.clear();
            best_dirs.push_back(c.dir);
        } else if (total == best_total_dist) {
            bool already_present = false;
            for (int dir : best_dirs) {
                if (dir == c.dir) {
                    already_present = true;
                    break;
                }
            }
            if (!already_present)
                best_dirs.push_back(c.dir);
        }
    }

    if (best_dirs.empty()) {
        directions.push_back(DIRECTION_NORTH);
        return directions;
    }

    vector<int> preferred_dirs = best_dirs;
    vector<int> long_step_dirs;
    for (int dir : best_dirs) {
        if (dir == DIRECTION_EAST || dir == DIRECTION_WEST)
            long_step_dirs.push_back(dir);
    }

    // Legacy baseline: among shortest paths, prefer long (+/-s) edges.
    if (!long_step_dirs.empty())
        preferred_dirs = long_step_dirs;

    // Preserve the proven 4x4 behavior exactly.
    if (n <= 16)
        return preferred_dirs;

    vector<int> filtered_dirs = preferred_dirs;

    // On large circulants, avoid immediate backtracking when alternatives exist.
    if (filtered_dirs.size() > 1)
        erase_direction(filtered_dirs, routeData.dir_in);

    // Avoid mixing opposite signs (+/-) in one candidate set on large networks.
    if (filtered_dirs.size() > 1) {
        int positive_count = 0;
        int negative_count = 0;
        for (int dir : filtered_dirs) {
            if (is_positive_direction(dir))
                positive_count++;
            else
                negative_count++;
        }

        if (positive_count > 0 && negative_count > 0) {
            const int cw = (dst - cur + n) % n;
            const int ccw = (cur - dst + n) % n;
            const bool prefer_positive = (cw <= ccw);

            filtered_dirs.erase(
                std::remove_if(filtered_dirs.begin(),
                               filtered_dirs.end(),
                               [prefer_positive](int dir) {
                                   return is_positive_direction(dir) != prefer_positive;
                               }),
                filtered_dirs.end());
        }
    }

    if (!filtered_dirs.empty())
    {
        if (filtered_dirs.size() == 1)
            return filtered_dirs;

        const int cw = (dst - cur + n) % n;
        const int ccw = (cur - dst + n) % n;
        const bool prefer_positive = (cw <= ccw);

        int chosen = NOT_VALID;
        if (prefer_positive) {
            // Keep the legacy long-step preference when possible.
            if (has_direction(filtered_dirs, DIRECTION_EAST))
                chosen = DIRECTION_EAST;
            else if (has_direction(filtered_dirs, DIRECTION_NORTH))
                chosen = DIRECTION_NORTH;
            else if (has_direction(filtered_dirs, DIRECTION_WEST))
                chosen = DIRECTION_WEST;
            else if (has_direction(filtered_dirs, DIRECTION_SOUTH))
                chosen = DIRECTION_SOUTH;
        } else {
            if (has_direction(filtered_dirs, DIRECTION_WEST))
                chosen = DIRECTION_WEST;
            else if (has_direction(filtered_dirs, DIRECTION_SOUTH))
                chosen = DIRECTION_SOUTH;
            else if (has_direction(filtered_dirs, DIRECTION_EAST))
                chosen = DIRECTION_EAST;
            else if (has_direction(filtered_dirs, DIRECTION_NORTH))
                chosen = DIRECTION_NORTH;
        }

        if (chosen != NOT_VALID)
            return vector<int>(1, chosen);

        return filtered_dirs;
    }

    return preferred_dirs;
}

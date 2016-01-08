/**
 * @file maze.cpp
 * Definition of a maze class.
 *
 * @author Chase Geigle
 * @author Chloe Kim
 * @date Summer 2014
 */

#include <random>

#include "tools.h"
#include "maze.h"
#include "tools_provided.h"
#include <iostream>
#include <queue>

namespace cs225
{

std::ostream& operator<<(std::ostream& os, direction dir)
{
    switch (dir)
    {
        case direction::RIGHT:
            return os << "RIGHT";
        case direction::DOWN:
            return os << "DOWN";
        case direction::LEFT:
            return os << "LEFT";
        case direction::UP:
            return os << "UP";
    }
}

maze::maze(uint64_t width, uint64_t height) : width_{width}, height_{height}
{
    std::random_device rd;
    std::mt19937_64 rng{rd()};
    // use rng for your random numbers: it's a functor that returns you
    // random numbers

	// connect edges
	for (int i=0; i<width*height; i++) {

		graph_.add_vertex();

		if (i == 0) {
			continue;
		}
		// first row add one edge
		else if (i < width) {
			graph_.edge_weight(graph_.add_edge(vertex{i}, vertex{i-1}), rng());
		}
		// first column add one edge
		else if (i % (width) == 0) {
			graph_.edge_weight(graph_.add_edge(vertex{i}, vertex{i-width}), rng());
		}
		// otherwise add two edges
		else {
			graph_.edge_weight(graph_.add_edge(vertex{i}, vertex{i-width}), rng());
			graph_.edge_weight(graph_.add_edge(vertex{i}, vertex{i-1}), rng());

		}
	}

	auto eset = tools::minimum_spanning_tree(graph_);
	
	for (auto e : graph_.edges()) {
		if (eset.find(e) == eset.end()) {
			graph_.remove_edge(e);
		}
	}
}

bool maze::can_travel(uint64_t x, uint64_t y, direction dir) const
{
	if (dir == direction::RIGHT) {
		if (x == width_-1)
			return false;
		if (!graph_.has_edge(vertex{y*(width_)+x}, vertex{y*(width_)+x+1}))
			return false;
	} else if (dir == direction::LEFT) {
		if (x == 0)
			return false;
		if (!graph_.has_edge(vertex{y*(width_)+x}, vertex{y*(width_)+x-1}))
			return false;
	} else if (dir == direction::DOWN) {
		if (y == height_-1)
			return false;
		if (!graph_.has_edge(vertex{y*(width_)+x}, vertex{(y+1)*(width_)+x}))
			return false;
	} else if (dir == direction::UP) {
		if (y == 0)
			return false;
		if (!graph_.has_edge(vertex{y*(width_)+x}, vertex{(y-1)*(width_)+x}))
			return false;
	}

    return true;
}

void maze::set_wall(uint64_t x, uint64_t y, direction dir, bool exists)
{
 	
	auto cur = y*(width_) + x;

	if (dir == direction::RIGHT && x != width_-1) {
		if (!exists) {
			if (!graph_.has_edge(vertex{cur}, vertex{cur+1}))
				graph_.add_edge(vertex{cur}, vertex{cur+1});
		} else {
			if (graph_.has_edge(vertex{cur}, vertex{cur+1}))
				graph_.remove_edge(vertex{cur}, vertex{cur+1});
		}

	} else if (dir == direction::LEFT && x != 0) {
		if (!exists) {
			if (!graph_.has_edge(vertex{cur}, vertex{cur-1}))
				graph_.add_edge(vertex{cur}, vertex{cur-1});
		} else {
			if (graph_.has_edge(vertex{cur}, vertex{cur-1}))
				graph_.remove_edge(vertex{cur}, vertex{cur-1});
		}

	} else if (dir == direction::DOWN && y != height_-1) {
		if (!exists) {
			if (!graph_.has_edge(vertex{cur}, vertex{cur+width_}))
				graph_.add_edge(vertex{cur}, vertex{cur+width_});

		} else {
			if (graph_.has_edge(vertex{cur}, vertex{cur+width_}))
				graph_.remove_edge(vertex{cur}, vertex{cur+width_});
		}

	} else if (dir == direction::UP && y != 0) {
		if (!exists) {
			if (!graph_.has_edge(vertex{cur}, vertex{cur-width_}))
				graph_.add_edge(vertex{cur}, vertex{cur-width_});
		} else {
			if (graph_.has_edge(vertex{cur}, vertex{cur-width_}))
				graph_.remove_edge(vertex{cur}, vertex{cur-width_});
		}
	}
	
}

std::vector<direction> maze::solve()
{
	std::vector<direction> dir;

	find_path(dir);

	return dir;
}

void maze::find_path(std::vector<direction>& dir)
{
	dir.resize(0);
	// depth first search until end is found
	std::queue<vertex> q;
	std::vector<vertex> vv;

	vertex_map<tools::vertex_state> vtx_mark;
	edge_map<tools::edge_state> edg_mark;

	uint64_t min = 0;

	for (const auto& v : graph_.vertices())
		vtx_mark[v] = tools::vertex_state::UNEXPLORED;

	for (const auto& e : graph_.edges())
		edg_mark[e] = tools::edge_state::UNEXPLORED;

	auto start = vertex{0};

	vtx_mark[start] = tools::vertex_state::VISITED;
	q.push(start);
	vv.push_back(start);

	while (!q.empty()) {
		auto u = q.front();
		q.pop();

		for (const auto& v : graph_.adjacent(u)) {
			auto lbl = vtx_mark[v];

			if (lbl == tools::vertex_state::UNEXPLORED) {
				vtx_mark[v] = tools::vertex_state::VISITED;
				edg_mark[{u,v}] = tools::edge_state::DISCOVERY;
				q.push(v);
				vv.push_back(v);
			}
			else if (edg_mark[{u,v}] == tools::edge_state::UNEXPLORED) {
				edg_mark[{u,v}] = tools::edge_state::BACK;
			}
		}
	}
	
	vertex t1, t2, t3;
	auto j = vv.size()-1;

	vertex_set vset;
	
	for (size_t i=width_*(height_-1); i<width_*height_; i++) {
		vset.insert(vertex{i});
	}

	while (true) {
		if (vset.find(vv[j]) != vset.end())
			break;
		j--;
	}
	
	t1 = vv[j];

	std::vector<vertex> tmp;
	tmp.push_back(t1);
	while (true) {
		for (int i=j-1; i>-1; i--) {
			if (graph_.has_edge(vv[i], t1)) {
				j = i;
				t2 = vv[i];
				tmp.push_back(t2);
				break;
			}
		}
		t1 = t2;
		if (j == 0) break;
	}

	int idx = 0;
	for (int i=tmp.size()-1; i!=0; i--) {
		dir.push_back(get_direction(tmp[i-1], idx));
	}
}

direction maze::get_direction(vertex v, int& idx)
{

	if (((idx+1) % width_ != 0) && vertex{idx + 1} == v) {
		idx = idx+1;
		return direction::RIGHT;
	} else if ((idx % width_ != 0) && vertex{idx -1} == v) {
		idx = idx-1;
		return direction::LEFT;
	} else if ((idx + width_ < width_*height_) && vertex{idx + width_} == v) {
		idx = idx + width_;
		return direction::DOWN;
	} else if ((idx > width_-1) && vertex{idx - width_} == v) {
		idx = idx - width_;
		return direction::UP;
	}

}

epng::png maze::draw() const
{
	epng::png canvas{width_*10+1, height_*10+1};
	epng::rgba_pixel black{0, 0, 0};

	for (size_t i=10; i<canvas.width(); i++)
		*canvas(i, 0) = black;
	for (size_t i=0; i<canvas.height(); i++)
		*canvas(0, i) = black;

	for (size_t i=0; i<width_; i++) {
		for (size_t j=0; j<height_; j++) {
			if (!can_travel(i, j, direction::RIGHT)) {
				for (size_t k=0; k<11; k++)
					*canvas((i+1)*10, j*10+k) = black;
			}
			if (!can_travel(i, j, direction::DOWN)) {
				for (size_t k=0; k<11; k++)
					*canvas(i*10+k, (j+1)*10) = black;
			}
		}
	}
	
	return canvas;
}

epng::png maze::draw_with_solution()
{
    auto result = draw();
	auto dir = solve();
	
	epng::rgba_pixel red{255,0,0};
	epng::rgba_pixel white{255,255,255};
	int x = 5; int y = 5;

	for (size_t i=0; i<dir.size(); i++) {
		if (dir[i] == direction::RIGHT) {
			for (size_t k=0; k<10; k++) {
				*result(x, y) = red;
				x++;
			}
		} else if (dir[i] == direction::LEFT) {
			for (size_t k=0; k<10; k++) {
				*result(x, y) = red;
				x--;
			}
		} else if (dir[i] == direction::UP) {
			for (size_t k=0; k<10; k++) {
				*result(x, y) = red;
				y--;
			}
		} else if (dir[i] == direction::DOWN) {
			for (size_t k=0; k<10; k++) {
				*result(x, y) = red;
				y++;
			}
		}

		if (i==dir.size()-1) {
			*result(x, y) = red;
			x -= 4; y += 5;
			for (size_t k=0; k<9; k++) {
				*result(x+k, y) = white;
			}
		}
	}
	
    return result;
}
}

/**
 * @file kdtree.tcc
 * Implementation of kd_tree class.
 */

#include "kdtree.h"
#include <cmath>

using namespace std;

/**
 * Determines if point a is smaller than point b in a given dimension d.
 * If there is a tie, break it with point::operator<().
 */
template <int Dim>
bool kd_tree<Dim>::smaller_in_dimension(const point<Dim>& first,
                                       const point<Dim>& second,
                                       int curDim) const
{
	if (first[curDim] < second[curDim])
		return true;
	if (first[curDim] == second[curDim] && first < second)
		return true;
    return false;
}


/**
 * Determines if a point is closer to the target point than another
 * reference point. Takes three points: target, current_best, and
 * potential, and returns whether or not potential is closer to
 * target than current_best.
 */
template <int Dim>
bool kd_tree<Dim>::should_replace(const point<Dim>& target,
                                 const point<Dim>& current_best,
                                 const point<Dim>& potential) const
{
	int potDist = 0;
	int curDist = 0;

	for (int i=0; i<Dim; ++i) {
		potDist += pow((potential[i] - target[i]), 2);
		curDist += pow((current_best[i] - target[i]), 2);
	}

	if (potDist < curDist) return true;
	else if (potDist == curDist && potential < current_best) return true;

    return false;
}


/**
 * Constructs a kd_tree from a vector of points, each having dimension Dim.
 */
template <int Dim>
kd_tree<Dim>::kd_tree(const std::vector<point<Dim>>& newpoints)
{
	points = newpoints;

	if (points.empty()) 
		return;
	
	indices.resize(points.size());
	left_child.resize(points.size());
	right_child.resize(points.size());

	for (int i=0; i<indices.size(); ++i) {
		indices[i] = i;
		left_child[i] = points.size();
		right_child[i] = points.size();
	}

	build_index(points, indices, 0, indices.size()-1, 0);

	points = build_tree(points, indices);

	

}

/**
 * Helper function for kd_tree().
 */
template <int Dim>
std::vector<point<Dim>> kd_tree<Dim>::build_tree(const std::vector<point<Dim>>& pts, std::vector<int>& idx)
{
	std::vector<point<Dim>> ret;
	ret.resize(pts.size());

	for (int i=0; i<pts.size(); i++) {
		ret[i] = pts[idx[i]];
	}

	return ret;
}

/**
 * Helper function for kd_tree().
 */
template <int Dim>
void kd_tree<Dim>::build_index(const std::vector<point<Dim>>& pts, std::vector<int>& idx, int left, int right, const int dim)
{
	if (left == right)
		return;

	int med = (left + right) / 2;
	int i = select(pts, idx, left, right, med, dim);

	if (med != left) {
		left_child[med] = (left + med-1)/2;
		build_index(pts, idx, left, med-1, (dim+1) % Dim);
	}

	right_child[med] = (med+1 + right)/2;
	build_index(pts, idx, med+1, right, (dim+1) % Dim);
	
}

/**
 * Selects pivot.
 */
template <int Dim>
int kd_tree<Dim>::select(const std::vector<point<Dim>>& pts, std::vector<int>& idx, int left, int right, int n, const int dim)
{
	if (left == right)
		return idx[left];
	while (true) {
		int pivot = n;
		pivot = partition(pts, idx, left, right, pivot, dim);
		if (n == pivot)
			return idx[n];
		else if (n < pivot)
			right = pivot - 1;
		else
			left = pivot + 1;
	}

}

/**
 * Partition into two with pivot.
 */
template <int Dim>
int kd_tree<Dim>::partition(const std::vector<point<Dim>>& pts, std::vector<int>& idx, int left, int right, int pivot, int dim)
{
	auto pivotVal = (pts.at(idx[pivot]));
	std::swap(idx[pivot], idx[right]);
	int ret = left;
	for (int i=left; i<right; ++i) {
		if (smaller_in_dimension(pts[idx[i]], pivotVal, dim)) {
			std::swap(idx[ret], idx[i]);
			ret++;
		}
	}
	std::swap(idx[right], idx[ret]);
	return ret;
}


/**
 * Finds the closest point to the parameter point in the kd_tree.
 */
template <int Dim>
point<Dim> kd_tree<Dim>::find_nearest_neighbor(const point<Dim>& query) const
{
	if (points.empty())
		return {};
	return find_nn((points.size()-1)/2, query, 0);

}

/**
 * Recursive helper function for find_nearest_neighbor().
 */
template <int Dim>
point<Dim> kd_tree<Dim>::find_nn(int idx, const point<Dim>& target, int dim) const
{
	point<Dim> child;
	point<Dim> ret;
	point<Dim> traverse;
	bool left;
	bool right;

	if (target[dim] < points[idx][dim]) {
			if (left_child[idx] == points.size()) {
				return points[idx];
			}
			child = find_nn(left_child[idx], target, (dim+1) % Dim);
			left = true;
			right = false;
	} else {
		if (right_child[idx] == points.size()) {
			return points[idx];
		}
			child = find_nn(right_child[idx], target, (dim+1) % Dim);
			right = true;
			left = false;
	}

	if (should_replace(target, points[idx], child)) {
		ret = child;
	}
	else {
		ret = points[idx];
	}


	if ((pow(target[dim]-points[idx][dim], 2) < distance(ret, target))
		|| ((pow(target[dim]-points[idx][dim], 2) == distance(ret, target))
			&& (points[idx] < ret))) {

		if (left && right_child[idx] != points.size()) {
			traverse = find_nn(right_child[idx], target, (dim+1) % Dim);
			if (should_replace(target, ret, traverse))
				ret = traverse;
		}
		else if (right && left_child[idx] != points.size()) {
			traverse = find_nn(left_child[idx], target, (dim+1) % Dim);
			if (should_replace(target, ret, traverse))
				ret = traverse;
		}


	return ret;

}

/**
 * Returns Eucledean distance btween two points.
 */
template <int Dim>
int kd_tree<Dim>::distance(const point<Dim>& pt1, const point<Dim>& pt2) const
{
	int sum = 0;
	for (int i=0; i<Dim; i++)
		sum += pow(pt1[i]-pt2[i], 2);
	return sum;
}

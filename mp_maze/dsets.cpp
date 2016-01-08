#include "dsets.h"


void dsets::add_elements (uint64_t num)
{
	auto old_size = arr_.size();
	arr_.resize(num + arr_.size());
	for (size_t i=0; i<num; i++)
		arr_[old_size+i] = -1;

}

uint64_t dsets::find (uint64_t elem)
{
	if (arr_[elem] < 0)
		return elem;
	return arr_[elem] = find(arr_[elem]);

}

void dsets::merge (uint64_t a, uint64_t b)
{
	auto root1 = find(a);
	auto root2 = find(b);
	if (root1 == root2) return;

	if (arr_[root1] <= arr_[root2]) {
		arr_[root1] += arr_[root2];
		arr_[root2] = root1;
	} else {
		arr_[root2] += arr_[root1];
		arr_[root1] = root2;
	}

}

/**
 * @file heap.cpp
 * Implementation of a heap class.
 */

#include <cmath>
#include <iostream>

using namespace std;

template <class T, class Compare>
size_t heap<T, Compare>::root() const
{
    return 0;
}

template <class T, class Compare>
size_t heap<T, Compare>::left_child(size_t idx) const
{
	return idx*2 +1;
}

template <class T, class Compare>
size_t heap<T, Compare>::right_child(size_t idx) const
{
	return idx*2 + 2;
}

template <class T, class Compare>
size_t heap<T, Compare>::parent(size_t idx) const
{
	return floor((idx-1)/2);
}

template <class T, class Compare>
bool heap<T, Compare>::has_child(size_t idx) const
{
	bool ret;
	if (elems_.size()-1 >= (idx * 2 + 1))
		ret = true;
	else 
		ret = false;
	return ret;
}

template <class T, class Compare>
size_t heap<T, Compare>::max_priority_child(size_t idx) const
{
	if (elems_.size()-1 >= right_child(idx)) {
		if (higher_priority_(elems_[left_child(idx)], elems_[right_child(idx)]))
			return left_child(idx);
		else
			return right_child(idx);
	} else if (elems_.size()-1 >= left_child(idx)) {
		return right_child(idx);
	} else
		return idx;
}

template <class T, class Compare>
void heap<T, Compare>::heapify_down(size_t idx)
{
	if ((!has_child(idx)) && (idx == 0))
		return;

	size_t l = left_child(idx);
	size_t r = right_child(idx);

	if (elems_.size()-1 >= r) {
		size_t max = max_priority_child(idx);

		if ((elems_[max] < elems_[idx])) {
			std::swap(elems_[idx], elems_[max]);
			heapify_down(max);
		}
	} else if (elems_.size()-1 >= l) {
		if ((elems_[l] < elems_[idx])) {
		
			std::swap(elems_[idx], elems_[l]);
			heapify_down(l);
		}
	}
	
	if (idx != 0)
		heapify_down(idx-1);
	else return;

}

template <class T, class Compare>
void heap<T, Compare>::heapify_up(size_t idx)
{
    if (idx == root())
        return;
    size_t parentIdx = parent(idx);
    if (higher_priority_(elems_[idx], elems_[parentIdx]))
    {
        std::swap(elems_[idx], elems_[parentIdx]);
        heapify_up(parentIdx);
    }
}

template <class T, class Compare>
heap<T, Compare>::heap()
{
}

template <class T, class Compare>
heap<T, Compare>::heap(const std::vector<T>& elems)
{
	elems_ = elems;
	size_t i;
	for (i=elems_.size()-1; !has_child(i); i--);
	cout << "heapify_down(" << i << ")" << endl;
	heapify_down(i);
}

template <class T, class Compare>
void heap<T, Compare>::pop()
{
	size_t s = elems_.size();
	
	if (s == 0) return;
	
	std::swap(elems_.at(0), elems_.at(s-1));
	elems_.pop_back();

	if (s-1 == 0) return;

	heapify_down(s-1);
}

template <class T, class Compare>
const T& heap<T, Compare>::peek() const
{
	if (elems_.size() == 0)
		return {};
	return elems_[0];
}

template <class T, class Compare>
void heap<T, Compare>::push(T elem)
{
	size_t s = elems_.size();
	elems_.resize(s+1);
	elems_.at(s) = elem;
	heapify_up(s);
}

template <class T, class Compare>
bool heap<T, Compare>::empty() const
{
	if (elems_.size() == 0)
		return true;
    return false;
}

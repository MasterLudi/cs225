#ifndef DSETS_H_
#define DSETS_H_

#include <vector>

class dsets
{
public :
	void add_elements (uint64_t num);
	uint64_t find (uint64_t elem);
	void merge (uint64_t a, uint64_t b);

private :
	std::vector<int64_t> arr_;

};
#endif

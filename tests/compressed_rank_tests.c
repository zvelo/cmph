#include "../src/compressed_rank.h"

#define DEBUG
#include "../src/debug.h"
#include <stdlib.h>

static inline void print_values(compressed_rank_t * cr, cmph_uint32 idx)
{
	register cmph_uint32 index;
	
	index = compressed_rank_query(cr, idx);
	fprintf(stderr, "Index[%u]\t= %u\n", idx, index);
}


static inline void print_values_packed(char * cr_packed, cmph_uint32 idx)
{
	register cmph_uint32 index;
	
	index = compressed_rank_query_packed(cr_packed, idx);
	fprintf(stderr, "Index[%u]\t= %u\n", idx, index);
}

/*
n = 20
Indices:         0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19
vector[]      = {0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}
nzeros = 12
zeroIndices[] = {0, 1, 2, 5, 7, 9, 11, 12, 13, 16, 17, 18}
*/
int main(int argc, char **argv)
{
	compressed_rank_t cr;
	cmph_uint32 i = 0;
	cmph_uint32 n = 12;
	cmph_uint32 nIndices = 20;
	cmph_uint32 keys_vec[] = {0, 1, 2, 5, 7, 9, 11, 12, 13, 16, 17, 18};
	char *buf = NULL;
	cmph_uint32 buflen = 0;
	char * cr_packed = NULL;
	cmph_uint32 cr_pack_size = 0;
	
	compressed_rank_init(&cr);
	compressed_rank_generate(&cr, keys_vec, n);
	fprintf(stderr, "Space usage = %u\n", compressed_rank_get_space_usage(&cr));
	for(i = 0; i < nIndices; i++)
	{
		print_values(&cr, i);
	}
	
	fprintf(stderr, "Dumping compressed rank structure\n");
	compressed_rank_dump(&cr, &buf, &buflen);
	
	compressed_rank_destroy(&cr);
	fprintf(stderr, "Loading compressed rank structure\n");
	
	compressed_rank_load(&cr, buf, buflen);
	for(i = 0; i < nIndices; i++)
	{
		print_values(&cr, i);
	}
	free(buf);
	
	cr_pack_size = compressed_rank_packed_size(&cr);
	
	cr_packed = (char *) calloc(cr_pack_size, sizeof(char));
	compressed_rank_pack(&cr, cr_packed);
	compressed_rank_destroy(&cr);
	
	fprintf(stderr, "Querying the packed compressed rank structure\n");
	for(i = 0; i < nIndices; i++)
	{
		print_values_packed(cr_packed, i);
	}
	
	free(cr_packed);
	return 0;
}

#include "../src/compressed_seq.h"

#define DEBUG
#include "../src/debug.h"
#include <stdlib.h>

static inline void print_values(compressed_seq_t * cs, cmph_uint32 idx)
{
	register cmph_uint32 index;
	
	index = compressed_seq_query(cs, idx);
	fprintf(stderr, "Index[%u]\t= %u\n", idx, index);
}


static inline void print_values_packed(char * cs_packed, cmph_uint32 idx)
{
	register cmph_uint32 index;
	
	index = compressed_seq_query_packed(cs_packed, idx);
	fprintf(stderr, "Index[%u]\t= %u\n", idx, index);
}

int main(int argc, char **argv)
{
	compressed_seq_t cs;
	cmph_uint32 i = 0;
	cmph_uint32 n = 20;
	cmph_uint32 keys_vec[] = {  0,   1,   1,   1,   2,   2,   2,   3,   5,   5,
	                            6,   6,   9,   9,   9,  12,  12,  13,  17,  1077};
	char *buf = NULL;
	cmph_uint32 buflen = 0;
	char * cs_packed = NULL;
	cmph_uint32 cs_pack_size = 0;
	
	compressed_seq_init(&cs);
	compressed_seq_generate(&cs, keys_vec, n);
	fprintf(stderr, "Space usage = %u\n", compressed_seq_get_space_usage(&cs));
	for(i = 0; i < n; i++)
	{
		print_values(&cs, i);
	}
	
	fprintf(stderr, "Dumping compressed seq structure\n");
	compressed_seq_dump(&cs, &buf, &buflen);
	
	compressed_seq_destroy(&cs);
	fprintf(stderr, "Loading compressed seq structure\n");
	
	compressed_seq_load(&cs, buf, buflen);
	for(i = 0; i < n; i++)
	{
		print_values(&cs, i);
	}
	free(buf);
	
	cs_pack_size = compressed_seq_packed_size(&cs);
	
	cs_packed = (char *) calloc(cs_pack_size, sizeof(char));
	compressed_seq_pack(&cs, cs_packed);
	compressed_seq_destroy(&cs);
	
	fprintf(stderr, "Querying the packed compressed seq structure\n");
	for(i = 0; i < n; i++)
	{
		print_values_packed(cs_packed, i);
	}
	
	free(cs_packed);
	return 0;
}

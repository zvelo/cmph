#include "../src/select.h"

#define DEBUG
#include "../src/debug.h"
#include <stdlib.h>

static inline void print_values(select_t * sel)
{
	register cmph_uint32 index;
	
	index = select_query(sel, 0);
	fprintf(stderr, "Index[0]\t= %u\n", index - 0);

	index = select_next_query(sel, index);
	fprintf(stderr, "Next Index\t= %u\n", index);

	index = select_query(sel, 1);
	fprintf(stderr, "Index[1]\t= %u\n", index - 1);

	index = select_next_query(sel, index);
	fprintf(stderr, "Next Index\t= %u\n", index);

	index = select_query(sel, 2);
	fprintf(stderr, "Index[2]\t= %u\n", index - 2);

	index = select_next_query(sel, index);
	fprintf(stderr, "Next Index\t= %u\n", index);

	index = select_query(sel, 3);
	fprintf(stderr, "Index[3]\t= %u\n", index - 3);
}


static inline void print_values_packed(char * sel_packed)
{
	register cmph_uint32 index;
	
	index = select_query_packed(sel_packed, 0);
	fprintf(stderr, "Index[0]\t= %u\n", index - 0);

	index = select_next_query_packed(sel_packed, index);
	fprintf(stderr, "Next Index\t= %u\n", index);

	index = select_query_packed(sel_packed, 1);
	fprintf(stderr, "Index[1]\t= %u\n", index - 1);

	index = select_next_query_packed(sel_packed, index);
	fprintf(stderr, "Next Index\t= %u\n", index);

	index = select_query_packed(sel_packed, 2);
	fprintf(stderr, "Index[2]\t= %u\n", index - 2);

	index = select_next_query_packed(sel_packed, index);
	fprintf(stderr, "Next Index\t= %u\n", index);

	index = select_query_packed(sel_packed, 3);
	fprintf(stderr, "Index[3]\t= %u\n", index - 3);
}

int main(int argc, char **argv)
{
	select_t sel;
	cmph_uint32 n = 4;
	cmph_uint32 keys_vec[4] = {0,1,2,3}; 
	cmph_uint32 m = keys_vec[3];
	char *buf = NULL;
	cmph_uint32 buflen = 0;
	char * select_packed = NULL;
	cmph_uint32 select_pack_size = 0;
	
	select_init(&sel);
	select_generate(&sel, keys_vec, n, m);
	fprintf(stderr, "Space usage = %u\n", select_get_space_usage(&sel));
	print_values(&sel);
	
	fprintf(stderr, "Dumping select structure\n");
	select_dump(&sel, &buf, &buflen);
	
	select_destroy(&sel);
	fprintf(stderr, "Loading select structure\n");
	
	select_load(&sel, buf, buflen);
	print_values(&sel);
	free(buf);;
	
	select_pack_size = select_packed_size(&sel);
	
	select_packed = (char *) calloc(select_pack_size, sizeof(char));
	select_pack(&sel, select_packed);
	select_destroy(&sel);
	
	fprintf(stderr, "Querying the packed select structure\n");
	print_values_packed(select_packed);
	
	free(select_packed);
	return 0;
}

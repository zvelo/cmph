#include <cmph.h>
#include <stdio.h>
#include <string.h>
 // Create minimal perfect hash function from in-disk keys using BDZ algorithm
int main(int argc, char **argv)
{   
	 //Open file with newline separated list of keys
	FILE * keys_fd = fopen("keys.txt", "r");
	cmph_t *hash = NULL;
	if (keys_fd == NULL) 
	{
	  fprintf(stderr, "File \"keys.txt\" not found\n");
	  exit(1);
	}	
	// Source of keys
	cmph_io_adapter_t *source = cmph_io_nlfile_adapter(keys_fd);

	cmph_config_t *config = cmph_config_new(source);
	cmph_config_set_algo(config, CMPH_BDZ);
	hash = cmph_new(config);
	cmph_config_destroy(config);
   
	//Find key
	const char *key = "jjjjjjjjjj";
	unsigned int id = cmph_search(hash, key, (cmph_uint32)strlen(key));
	fprintf(stderr, "Id:%u\n", id);
	//Destroy hash
	cmph_destroy(hash);
	cmph_io_nlfile_adapter_destroy(source);   
	fclose(keys_fd);
	return 0;
}

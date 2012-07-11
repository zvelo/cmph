#include <cmph.h>
#include <string.h>
// Create minimal perfect hash function from in-memory vector

#pragma pack(1)
typedef struct {
	cmph_uint32 id;
	char key[11];
	cmph_uint32 year;
} rec_t;
#pragma pack(0)

int main(int argc, char **argv)
{   
    // Creating a filled vector
    unsigned int i = 0;  
    rec_t vector[10] = {{1, "aaaaaaaaaa", 1999}, {2, "bbbbbbbbbb", 2000}, {3, "cccccccccc", 2001},
    			{4, "dddddddddd", 2002}, {5, "eeeeeeeeee", 2003}, {6, "ffffffffff", 2004},
    			{7, "gggggggggg", 2005}, {8, "hhhhhhhhhh", 2006}, {9, "iiiiiiiiii", 2007},
    			{10,"jjjjjjjjjj", 2008}};
    unsigned int nkeys = 10;
    FILE* mphf_fd = fopen("temp_struct_vector.mph", "w");
    // Source of keys
    cmph_io_adapter_t *source = cmph_io_struct_vector_adapter(vector, (cmph_uint32)sizeof(rec_t), (cmph_uint32)sizeof(cmph_uint32), 11, nkeys); 

    //Create minimal perfect hash function using the BDZ algorithm.
    cmph_config_t *config = cmph_config_new(source);
    cmph_config_set_algo(config, CMPH_BDZ);
    cmph_config_set_mphf_fd(config, mphf_fd);
    cmph_t *hash = cmph_new(config);
    cmph_config_destroy(config);
    cmph_dump(hash, mphf_fd); 
    cmph_destroy(hash);	
    fclose(mphf_fd);
   
    //Find key
    mphf_fd = fopen("temp_struct_vector.mph", "r");
    hash = cmph_load(mphf_fd);
    while (i < nkeys) {
      const char *key = vector[i].key;
      unsigned int id = cmph_search(hash, key, 11);
      fprintf(stderr, "key:%s -- hash:%u\n", key, id);
      i++;
    }
    
    //Destroy hash
    cmph_destroy(hash);
    cmph_io_vector_adapter_destroy(source);   
    fclose(mphf_fd);
    return 0;
}

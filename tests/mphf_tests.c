#ifdef WIN32
#include "../wingetopt.h"
#else
#include <getopt.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <cmph.h>

#ifdef WIN32
#define VERSION "0.8"
#else
#include "config.h"
#endif


void usage(const char *prg)
{
	fprintf(stderr, "usage: %s [-v] [-h] [-V] [-k nkeys] [-m file.mph]  keysfile\n", prg);   
}
void usage_long(const char *prg)
{
	fprintf(stderr, "usage: %s [-v] [-h] [-V] [-k nkeys] [-m file.mph] keysfile\n", prg);   
	fprintf(stderr, "Packed MPHFs testing tool\n\n"); 
	fprintf(stderr, "  -h\t print this help message\n");
	fprintf(stderr, "  -V\t print version number and exit\n");
	fprintf(stderr, "  -v\t increase verbosity (may be used multiple times)\n");
	fprintf(stderr, "  -k\t number of keys\n");
	fprintf(stderr, "  -m\t minimum perfect hash function file \n");
	fprintf(stderr, "  keysfile\t line separated file with keys\n");
}

int main(int argc, char **argv)
{
	char verbosity = 0;
	char *mphf_file = NULL;
	const char *keys_file = NULL;
	FILE *mphf_fd = stdout;
	FILE *keys_fd;
	cmph_uint32 nkeys = UINT_MAX;
	cmph_uint32 i = 0;
	cmph_t *mphf = NULL;
	cmph_io_adapter_t *source;
	while (1)
	{
		char ch = (char)getopt(argc, argv, "hVvk:m:");
		if (ch == -1) break;
		switch (ch)
		{
			case 'k':
			        {
					char *endptr;
					nkeys = (cmph_uint32) strtoul(optarg, &endptr, 10);
					if(*endptr != 0) {
						fprintf(stderr, "Invalid number of keys %s\n", optarg);
						exit(1);
					}
				}
				break;
			case 'm':
				mphf_file = strdup(optarg);
				break;
			case 'v':
				++verbosity;
				break;
			case 'V':
				printf("%s\n", VERSION);
				return 0;
			case 'h':
				usage_long(argv[0]);
				return 0;
			default:
				usage(argv[0]);
				return 1;
		}
	}

	if (optind != argc - 1)
	{
		usage(argv[0]);
		return 1;
	}
	keys_file = argv[optind];
  
	int ret = 0;
	if (mphf_file == NULL)
	{
		mphf_file = (char *)malloc(strlen(keys_file) + 5);
		memcpy(mphf_file, keys_file, strlen(keys_file));
		memcpy(mphf_file + strlen(keys_file), ".mph\0", (size_t)5);
	}	

	keys_fd = fopen(keys_file, "r");

	if (keys_fd == NULL)
	{
		fprintf(stderr, "Unable to open file %s: %s\n", keys_file, strerror(errno));
		return -1;
	}

	if(nkeys == UINT_MAX) source = cmph_io_nlfile_adapter(keys_fd);
	else source = cmph_io_nlnkfile_adapter(keys_fd, nkeys);

	cmph_uint8 * hashtable = NULL;
	mphf_fd = fopen(mphf_file, "r");
	if (mphf_fd == NULL)
	{
		fprintf(stderr, "Unable to open input file %s: %s\n", mphf_file, strerror(errno));
		free(mphf_file);
		return -1;
	}
	mphf = cmph_load(mphf_fd);
	fclose(mphf_fd);
	if (!mphf)
	{
		fprintf(stderr, "Unable to parser input file %s\n", mphf_file);
		free(mphf_file);
		return -1;
	}
	cmph_uint32 siz = cmph_size(mphf);
	hashtable = (cmph_uint8*)malloc(siz*sizeof(cmph_uint8));
	memset(hashtable, 0, (size_t)siz);
	//check all keys
	for (i = 0; i < source->nkeys; ++i)
	{
		cmph_uint32 h;
		char *buf;
		cmph_uint32 buflen = 0;
		source->read(source->data, &buf, &buflen);
		h = cmph_search(mphf, buf, buflen);
		if (!(h < siz))
		{
			fprintf(stderr, "Unknown key %*s in the input.\n", buflen, buf);
			ret = 1;
		} else if(hashtable[h])
		{
			fprintf(stderr, "Duplicated or unknown key %*s in the input\n", buflen, buf);
			ret = 1;
		} else hashtable[h] = 1;

		if (verbosity)
		{
			printf("%s -> %u\n", buf, h);
		}
		source->dispose(source->data, buf, buflen);
	}
		
	cmph_destroy(mphf);
	free(hashtable);

	fclose(keys_fd);
	free(mphf_file);
    cmph_io_nlfile_adapter_destroy(source);
	return ret;
  
}

bin_PROGRAMS = cmph
noinst_PROGRAMS = bm_numbers
lib_LTLIBRARIES = libcmph.la
include_HEADERS = cmph.h cmph_types.h cmph_time.h chd_ph.h
libcmph_la_SOURCES =  hash.h hash.c \
		      jenkins_hash.h jenkins_hash.c \
		      hash_state.h debug.h \
		      vstack.h vstack.c vqueue.h vqueue.c\
		      graph.h graph.c bitbool.h \
		      cmph.h cmph.c cmph_structs.h cmph_structs.c\
		      chm.h chm.c chm_structs.h \
		      bmz.h bmz.c bmz_structs.h \
                      bmz8.h bmz8.c bmz8_structs.h \
		      bdz.h bdz.c bdz_structs.h \
		      bdz_ph.h bdz_ph.c bdz_structs_ph.h \
		      brz.h brz.c brz_structs.h \
		      fch.h fch.c fch_structs.h \
		      fch_buckets.h fch_buckets.c \
		      chd.h chd.c chd_structs.h \
		      chd_ph.h chd_ph.c chd_structs_ph.h \
		      miller_rabin.h miller_rabin.c \
		      buffer_manager.h buffer_manager.c \
		      buffer_entry.h buffer_entry.c\
		      select.h select.c select_lookup_tables.h \
		      compressed_seq.h compressed_seq.c \
		      compressed_rank.h compressed_rank.c \
                      linear_string_map.h linear_string_map.c \
		      cmph_benchmark.h cmph_benchmark.c \
		      cmph_time.h

libcmph_la_LDFLAGS = -version-info 0:0:0

cmph_SOURCES = 	main.c wingetopt.h wingetopt.c
cmph_LDADD = libcmph.la

bm_numbers_SOURCES = bm_numbers.c
bm_numbers_LDADD = libcmph.la

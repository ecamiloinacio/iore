#**** User options ************************************************************
WITH_MPIIO_AFIO = true
WITH_OFSPROTO_AFIO = true
WITH_ORAGENFS_AFSB = false
WITH_LUSTRE_AFSB = false

#******************************************************************************
# Compile configurations
#******************************************************************************

#**** Directories *************************************************************
BINDIR = $(CURDIR)/bin
BUILDDIR = $(CURDIR)/build
BUILDDIRAFIO = $(BUILDDIR)/afio
BUILDDIRAFSB = $(BUILDDIR)/afsb
BUILDDIRCOMMON = $(BUILDDIR)/common
BUILDDIRPRNG = $(BUILDDIR)/prng
BUILDDIRSTEX = $(BUILDDIR)/stex
BUILDDIRTREC = $(BUILDDIR)/trec
BUILDDIRPARSER = $(BUILDDIR)/parser
BUILDDIRLIB = $(BUILDDIR)/lib
BUILDDIRLIBTINYMT = $(BUILDDIRLIB)/tinymt
BUILDDIRLIBJSONPARSER = $(BUILDDIRLIB)/json-parser
SRCDIR = $(CURDIR)/src
LIBDIR = $(CURDIR)/lib
INCDIR = $(CURDIR)/include
TESTSRCDIR = $(CURDIR)/test
TESTBINDIR = $(CURDIR)/test/bin
OUTDIRS = $(BUILDDIR) $(BUILDDIRAFIO) $(BUILDDIRAFSB) $(BUILDDIRCOMMON)
OUTDIRS += $(BUILDDIRPRNG) $(BUILDDIRSTEX) $(BUILDDIRTREC) $(BUILDDIRPARSER)
OUTDIRS += $(BUILDDIRLIB) $(BUILDDIRLIBTINYMT) $(BUILDDIRLIBJSONPARSER)
OUTDIRS += $(BINDIR) $(TESTBINDIR)

#**** Toolchain ***************************************************************
CC = mpicc

CFLAGS = -I $(INCDIR)
CFLAGS += -Wall -Wextra
CFLAGS += -I $(LIBDIR)/tinymt
CFLAGS += -I $(LIBDIR)/json-parser
CFLAGS += -std=gnu99
CFLAGS += -lm

ifeq ($(WITH_MPIIO_AFIO), true)
	CFLAGS += -DWITH_MPIIO_AFIO
endif

ifeq ($(WITH_OFSPROTO_AFIO), true)
	CFLAGS += -DWITH_OFSPROTO_AFIO
endif

ifeq ($(WITH_ORANGEFS_AFSB), true)
	CFLAGS += -DWITH_ORANGEFS_AFSB
endif

ifeq ($(WITH_LUSTRE_AFSB), true)
	CFLAGS += -DWITH_LUSTRE_AFSB
endif

#**** Variables ***************************************************************

OBJFILES = $(BUILDDIR)/%.o

# Abstract File I/O (AFIO)
AFIOT = $(BUILDDIRAFIO)/iore_afio_types.o
AFIO = $(BUILDDIRAFIO)/iore_afio.o 
AFIO += $(BUILDDIRAFIO)/iore_afio_posix.o
AFIO +=	$(BUILDDIRAFIO)/iore_afio_cstream.o
ifeq ($(WITH_MPIIO_AFIO), true)
	AFIO += $(BUILDDIRAFIO)/iore_afio_mpiio.o
endif
ifeq ($(WITH_OFSPROTO_AFIO), true)
	AFIO += $(BUILDDIRAFIO)/iore_afio_ofsproto.o
endif

# Abstract Storage File Backend (AFSB)
AFSBT = $(BUILDDIRAFSB)/iore_afsb_types.o
AFSB = $(BUILDDIRAFSB)/iore_afsb.o
ifeq ($(WITH_ORANGEFS_AFSB), true)
	AFSB += $(BUILDDIRAFSB)/iore_afsb_orangefs.o
endif
ifeq ($(WITH_LUSTRE_AFSB), true)
	AFSB += $(BUILDDIRAFSB)/iore_afsb_lustre.o
endif

# Pseudo-Random Number Generator (PRNG)
PRNGT = $(BUILDDIRPRNG)/iore_prng_types.o
PRNG = $(BUILDDIRPRNG)/iore_prng.o
PRNG += $(BUILDDIRPRNG)/iore_prng_unif.o 
PRNG += $(BUILDDIRPRNG)/iore_prng_norm.o
PRNG += $(BUILDDIRPRNG)/iore_prng_geom.o

# Statistics Exporter (STEX)
STEX = $(BUILDDIRSTEX)/iore_stex.o
STEX += $(BUILDDIRSTEX)/iore_stex_valid.o
STEX += $(BUILDDIRSTEX)/iore_stex_csv.o
STEXT = $(BUILDDIRSTEX)/iore_stex_types.o

# Time Recorder (TREC)
TRECT = $(BUILDDIRTREC)/iore_trec_types.o
TREC = $(BUILDDIRTREC)/iore_trec.o

# Parser
PRSR = $(BUILDDIRPARSER)/iore_parser.o
PRSR += $(BUILDDIRPARSER)/iore_parser_file.o
PRSR += $(BUILDDIRPARSER)/iore_parser_file_json.o

# Common
CTX = $(BUILDDIRCOMMON)/iore_ctx.o
DICT = $(BUILDDIRCOMMON)/iore_dict.o
ERROR = $(BUILDDIRCOMMON)/iore_error.o
FILE = $(BUILDDIRCOMMON)/iore_file.o
UTIL = $(BUILDDIRCOMMON)/iore_util.o

# Experiment Structures
EXP = $(BUILDDIR)/iore_experiment.o 
EXP += $(BUILDDIR)/iore_experiment_valid.o
EXPT = $(BUILDDIR)/iore_experiment_types.o
RUN = $(BUILDDIR)/iore_run.o
RUN += $(BUILDDIR)/iore_run_valid.o
RUNT = $(BUILDDIR)/iore_run_types.o
TEST = $(BUILDDIR)/iore_test.o
TEST += $(BUILDDIR)/iore_test_valid.o 
TESTT = $(BUILDDIR)/iore_test_types.o
WKLD = $(BUILDDIR)/iore_workload.o 
WKLD += $(BUILDDIR)/iore_workload_valid.o
WKLDT = $(BUILDDIR)/iore_workload_types.o

# Libs
TINYMT = $(BUILDDIRLIBTINYMT)/tinymt64.o
JSONPARSER = $(BUILDDIRLIBJSONPARSER)/json.o

# Main executable
IORE = $(BINDIR)/iore

# Tests executables
TEST_TREC = $(TESTBINDIR)/test_trec
TEST_STEX = $(TESTBINDIR)/test_stex
TEST_DICT = $(TESTBINDIR)/test_dict
TEST_PRNG = $(TESTBINDIR)/test_prng
TEST_WKLD = $(TESTBINDIR)/test_wkld
TEST_OSET = $(TESTBINDIR)/test_oset
TEST_DSET = $(TESTBINDIR)/test_dset
TEST_RUN = $(TESTBINDIR)/test_run
TEST_EXP = $(TESTBINDIR)/test_exp
TEST_AFIO_POSIX = $(TESTBINDIR)/test_afio_posix
TEST_AFIO_CSTREAM = $(TESTBINDIR)/test_afio_cstream
TEST_AFIO_MPIIO = $(TESTBINDIR)/test_afio_mpiio
TEST_PRSR_JSON = $(TESTBINDIR)/test_prsr_json

#**** Build targets ***********************************************************
.PHONY: all clean debug tests_clean

all: $(IORE)

clean:
	rm -rf $(BINDIR)/* $(BUILDDIR)/* $(TESTBINDIR)/*

debug: CFLAGS += -g
debug: all

tests_clean:
	rm -rf $(TESTBINDIR)/*

$(addsuffix /.marker, $(OUTDIRS)):
	mkdir -p $(dir $@)
	touch $@

$(OBJFILES): $(SRCDIR)/%.c
	$(CC) -c $(SRCDIR)/$*.c $(CFLAGS) -o $@

# Main executable
$(IORE): $(BINDIR)/.marker $(SRCDIR)/iore.c \
				$(CTX) $(EXP) $(TREC) $(PRSR) $(PRNG) $(STEX) $(AFIOT) \
				$(AFSBT) $(DICT) $(WKLD) $(EXPT) $(FILE) $(ERROR) \
				$(JSONPARSER) $(PRNGT) $(RUN) $(UTIL) $(STEXT) $(TESTT) \
				$(TINYMT) $(WKLDT) $(TEST) $(TRECT) $(AFIO) $(AFSB) $(RUNT)
	$(CC) $(SRCDIR)/iore.c \
		$(CTX) $(EXP) $(TREC) $(PRSR) $(PRNG) $(STEX) $(AFIOT) \
		$(AFSBT) $(DICT) $(WKLD) $(EXPT) $(FILE) $(ERROR) \
		$(JSONPARSER) $(PRNGT) $(RUN) $(UTIL) $(STEXT) $(TESTT) \
		$(TINYMT) $(WKLDT) $(TEST) $(TRECT) $(AFIO) $(AFSB) $(RUNT) \
		$(CFLAGS) -o $@

# AFIO
$(AFIO) $(AFIOT): $(BUILDDIRAFIO)/.marker
$(AFIO): $(INCDIR)/iore_afio.h
$(AFIOT): $(INCDIR)/iore_afio_types.h

# AFSB
$(AFSB) $(AFSBT): $(BUILDDIRAFSB)/.marker
$(AFSB): $(INCDIR)/iore_afsb.h
$(AFSBT): $(INCDIR)/iore_afsb_types.h

# PRNG
$(PRNG) $(PRNGT): $(BUILDDIRPRNG)/.marker 
$(PRNG): $(INCDIR)/iore_prng.h
$(PRNGT): $(INCDIR)/iore_prng_types.h

# STEX
$(STEX) $(STEXT): $(BUILDDIRSTEX)/.marker 
$(STEX): $(INCDIR)/iore_stex.h
$(STEXT): $(INCDIR)/iore_stex_types.h

# TREC
$(TREC) $(TRECT): $(BUILDDIRTREC)/.marker
$(TREC): $(INCDIR)/iore_trec.h
$(TRECT): $(INCDIR)/iore_trec_types.h

# Parser
$(PRSR): $(BUILDDIRPARSER)/.marker $(INCDIR)/iore_parser.h \
				$(INCDIR)/iore_parser_file.h

# Common
$(CTX) $(DICT) $(ERROR) $(FILE) $(UTIL): $(BUILDDIRCOMMON)/.marker
$(CTX): $(INCDIR)/iore_ctx.h
$(DICT): $(INCDIR)/iore_dict.h
$(ERROR): $(INCDIR)/iore_error.h
$(FILE): $(INCDIR)/iore_file.h
$(UTIL): $(INCDIR)/iore_util.h

# Experiment Structs
$(EXP) $(EXPT) $(RUN) $(TEST) $(TESTT) $(WKLD) $(WKLDT): $(BUILDDIR)/.marker
$(EXP): $(INCDIR)/iore_experiment.h
$(EXPT): $(INCDIR)/iore_experiment_types.h
$(RUN): $(INCDIR)/iore_run.h
$(RUNT): $(INCDIR)/iore_run_types.h
$(TEST): $(INCDIR)/iore_test.h
$(TESTT): $(INCDIR)/iore_test_types.h
$(WKLD): $(INCDIR)/iore_workload.h
$(WKLDT): $(INCDIR)/iore_workload_types.h

# Libs
$(TINYMT): $(BUILDDIRLIBTINYMT)/.marker $(LIBDIR)/tinymt/tinymt64.h
	$(CC) -c $(LIBDIR)/tinymt/tinymt64.c $(CFLAGS) -o $@
$(JSONPARSER): $(BUILDDIRLIBJSONPARSER)/.marker $(LIBDIR)/json-parser/json.h
	$(CC) -c $(LIBDIR)/json-parser/json.c $(CFLAGS) -o $@

### Tests
# TREC
$(TEST_TREC): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_trec.c \
				$(TREC) $(CTX) $(TESTT) $(TRECT) \
				$(AFIOT) $(AFSBT) $(UTIL) $(WKLDT) $(DICT) $(ERROR) $(PRNG) \
				$(PRNGT) $(TINYMT)
	$(CC) $(TESTSRCDIR)/test_iore_trec.c $(TREC) $(CTX) $(TESTT) $(TRECT) \
		$(AFIOT) $(AFSBT) $(UTIL) $(WKLDT) $(DICT) $(ERROR) $(PRNG) \
		$(PRNGT) $(TINYMT) \
		$(CFLAGS) -g -o $@

# STEX
$(TEST_STEX): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_stex.c \
				$(STEX) $(CTX) $(TREC) $(ERROR) $(TRECT) $(TESTT) \
				$(FILE) $(UTIL) $(WKLDT) $(AFIOT) $(AFSBT) $(DICT) \
				$(PRNG) $(PRNGT) $(TINYMT) $(STEXT)
	$(CC) $(TESTSRCDIR)/test_iore_stex.c $(STEX) $(CTX) \
		$(TREC) $(ERROR) $(TRECT) $(TESTT) \
		$(FILE) $(UTIL) $(WKLDT) $(AFIOT) $(AFSBT) $(DICT) \
		$(PRNG) $(PRNGT) $(TINYMT) $(STEXT) \
		$(CFLAGS) -g -o $@

# DICT
$(TEST_DICT): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_dict.c $(DICT) \
				$(UTIL) $(ERROR) $(PRNG) $(TINYMT) $(CTX) $(PRNGT)
	$(CC) $(TESTSRCDIR)/test_iore_dict.c $(DICT) \
		$(UTIL) $(ERROR) $(PRNG) $(TINYMT) $(CTX) $(PRNGT) \
		$(CFLAGS) -g -o $@

# PRNG
$(TEST_PRNG): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_prng.c $(PRNG) \
				$(CTX) $(DICT) $(ERROR) $(TINYMT) $(PRNGT) $(UTIL)
	$(CC) $(TESTSRCDIR)/test_iore_prng.c $(PRNG) \
		$(CTX) $(DICT) $(ERROR) $(TINYMT) $(PRNGT) $(UTIL) $(CFLAGS) -g -o $@

# Test Workload
$(TEST_WKLD): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_wkld.c $(WKLD) \
				$(WKLDT) $(CTX) $(ERROR) $(UTIL) $(PRNG) $(PRNGT) $(DICT) \
				$(TINYMT)
	$(CC) $(TESTSRCDIR)/test_iore_wkld.c $(WKLD) \
		$(WKLDT) $(CTX) $(ERROR) $(UTIL) $(PRNG) $(PRNGT) $(DICT) \
		$(TINYMT) \
		$(CFLAGS) -g -o $@

# Test Offset
$(TEST_OSET): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_test_oset.c \
				$(TEST) $(AFIO) $(AFSB) $(CTX) $(DICT) $(FILE) $(ERROR) \
				$(UTIL) $(PRNG) $(PRNGT) $(TREC) $(WKLD) $(TINYMT) $(TESTT) \
				$(AFIOT) $(AFSBT) $(WKLDT)
	$(CC) $(TESTSRCDIR)/test_iore_test_oset.c $(TEST) \
		$(AFIO) $(AFSB) $(CTX) $(DICT) $(FILE) $(ERROR) \
		$(UTIL) $(PRNG) $(PRNGT) $(TREC) $(WKLD) $(TINYMT) $(TESTT) \
		$(AFIOT) $(AFSBT) $(WKLDT) \
		$(CFLAGS) -g -o $@

# Test Dataset
$(TEST_DSET): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_test_dset.c \
				$(TEST) $(AFIO) $(AFSB) $(CTX) $(DICT) $(FILE) $(ERROR) \
				$(UTIL) $(PRNG) $(TREC) $(WKLD) $(TINYMT) $(PRNGT) $(TESTT) \
				$(AFIOT) $(AFSBT) $(WKLDT)
	$(CC) $(TESTSRCDIR)/test_iore_test_dset.c $(TEST) \
		$(AFIO) $(AFSB) $(CTX) $(DICT) $(FILE) $(ERROR) \
		$(UTIL) $(PRNG) $(TREC) $(WKLD) $(TINYMT) $(PRNGT) $(TESTT) \
		$(AFIOT) $(AFSBT) $(WKLDT) \
		$(CFLAGS) -g -o $@

# Run
$(TEST_RUN): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_run.c \
				$(RUN) $(TEST) $(AFIO) $(AFSB) $(CTX) $(DICT) $(FILE) \
				$(ERROR) $(UTIL) $(PRNG) $(TREC) $(WKLD) $(TINYMT) $(RUNT) \
				$(PRNGT) $(TESTT) $(AFIOT) $(AFSBT) $(WKLDT)
	$(CC) $(TESTSRCDIR)/test_iore_run.c $(RUN) $(TEST) \
		$(AFIO) $(AFSB) $(CTX) $(DICT) $(FILE) $(ERROR) \
		$(UTIL) $(PRNG) $(TREC) $(WKLD) $(TINYMT) $(RUNT) \
		$(PRNGT) $(TESTT) $(AFIOT) $(AFSBT) $(WKLDT) \
		$(CFLAGS) -g -o $@

# Experiment
$(TEST_EXP): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_experiment.c \
				$(EXP) $(RUN) $(TEST) $(AFIO) $(AFSB) $(CTX) $(DICT) $(FILE) \
				$(ERROR) $(UTIL) $(PRNG) $(TREC) $(WKLD) $(TINYMT) $(STEX) \
				$(TESTT) $(TRECT) $(RUNT) $(AFIOT) $(AFSBT) $(EXPT) $(PRNGT) \
				$(WKLDT) $(STEXT)
	$(CC) $(TESTSRCDIR)/test_iore_experiment.c $(EXP) $(RUN) $(TEST) \
		$(AFIO) $(AFSB) $(CTX) $(DICT) $(FILE) $(ERROR) \
		$(UTIL) $(PRNG) $(TREC) $(WKLD) $(TINYMT) $(STEX) $(TESTT) $(TRECT) \
		$(RUNT) $(AFIOT) $(AFSBT) $(EXPT) $(PRNGT) $(WKLDT) $(STEXT) \
		$(CFLAGS) -g -o $@

# AFIO POSIX
$(TEST_AFIO_POSIX): $(TESTBINDIR)/.marker $(TESTSRCDIR)/test_iore_afio_posix.c \
				$(TESTSRCDIR)/test_iore_afio.c $(AFIO) $(AFIOT) \
				$(CTX) $(DICT) $(WKLD) $(UTIL) $(ERROR) $(PRNG) $(TINYMT) \
				$(PRNGT) $(TESTT) $(WKLDT) $(AFSBT)
	$(CC) $(TESTSRCDIR)/test_iore_afio_posix.c \
		$(TESTSRCDIR)/test_iore_afio.c $(AFIO) $(AFIOT) \
		$(CTX) $(DICT) $(WKLD) $(UTIL) $(ERROR) $(PRNG) $(PRNGT) $(TINYMT) \
		$(TESTT) $(WKLDT) $(AFSBT) \
		$(CFLAGS) -g -o $@

# AFIO CSTREAM
$(TEST_AFIO_CSTREAM): $(TESTBINDIR)/.marker \
				$(TESTSRCDIR)/test_iore_afio_cstream.c \
				$(TESTSRCDIR)/test_iore_afio.c $(AFIO) $(AFIOT) \
				$(CTX) $(DICT) $(WKLD) $(UTIL) $(ERROR) $(PRNG) $(TINYMT) \
				$(PRNGT)
	$(CC) $(TESTSRCDIR)/test_iore_afio_cstream.c \
		$(TESTSRCDIR)/test_iore_afio.c $(AFIO) $(AFIOT) \
		$(CTX) $(DICT) $(WKLD) $(UTIL) $(ERROR) $(PRNG) $(PRNGT) $(TINYMT) \
		$(CFLAGS) -g -o $@

# AFIO MPIIO
$(TEST_AFIO_MPIIO): $(TESTBINDIR)/.marker \
				$(TESTSRCDIR)/test_iore_afio_mpiio.c \
				$(TESTSRCDIR)/test_iore_afio.c $(AFIO) $(AFIOT) \
				$(CTX) $(DICT) $(WKLD) $(UTIL) $(ERROR) $(PRNG) $(TINYMT) \
				$(PRNGT)
	$(CC) $(TESTSRCDIR)/test_iore_afio_mpiio.c \
		$(TESTSRCDIR)/test_iore_afio.c $(AFIO) $(AFIOT) \
		$(CTX) $(DICT) $(WKLD) $(UTIL) $(ERROR) $(PRNG) $(PRNGT) $(TINYMT) \
		$(CFLAGS) -g -o $@

# Parser JSON
$(TEST_PRSR_JSON): $(TESTBINDIR)/.marker \
				$(TESTSRCDIR)/test_iore_parser_file_json.c $(PRSR) \
				$(AFIOT) $(AFSBT) $(CTX) $(DICT) $(WKLD) $(WKLDT) $(EXPT) \
				$(FILE) $(ERROR) $(JSONPARSER) $(PRNG) $(PRNGT) $(STEXT) \
				$(TESTT) $(TINYMT) $(TRECT) $(RUN) $(UTIL) $(RUNT) $(TEST) \
				$(TREC) $(AFIO) $(AFSB) $(EXP) $(STEX)
	$(CC) $(TESTSRCDIR)/test_iore_parser_file_json.c $(PRSR) \
		$(AFIOT) $(AFSBT) $(CTX) $(DICT) $(WKLD) $(WKLDT) $(EXPT) \
		$(FILE) $(ERROR) $(JSONPARSER) $(PRNG) $(PRNGT) $(STEXT) \
		$(TESTT) $(TINYMT) $(TRECT) $(RUN) $(UTIL) $(RUNT) $(TEST) \
		$(TREC) $(AFIO) $(AFSB) $(EXP) $(STEX) \
		$(CFLAGS) -g -o $@

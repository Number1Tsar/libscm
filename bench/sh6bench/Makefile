#BENCH_OPTION:=$(BENCH_OPTION) -DDEBUG
#BENCH_OPTION:=$(BENCH_OPTION) -DPRINTLATENCY
#BENCH_OPTION:=$(BENCH_OPTION) -DPRINTTHROUGHPUT
#BENCH_OPTION:=$(BENCH_OPTION) -DPRINTMEMINFO
#BENCH_OPTION:=$(BENCH_OPTION) -DWITHOUT_LEAK
BENCH_OPTION:=$(BENCH_OPTION) -DCALL_COUNT=250
#BENCH_OPTION:=$(BENCH_OPTION) -DSYS_MULTI_THREAD -pthread

CC=gcc
CFLAGS=$(BENCH_OPTION) -O3
OBJECTDIR=build
DISTDIR=dist

all: sh6bench_malloc sh6bench_str sh6bench_stm sh6bench_strmc \
sh6bench_malloc_leak sh6bench_str_leak sh6bench_stm_leak sh6bench_strmc_leak

sh6bench_malloc_leak: $(OBJECTDIR)/sh6bench_leak.o
	mkdir -p $(DISTDIR)
	$(CC) $(CFLAGS) -o $(DISTDIR)/sh6benchMALLOC_LEAK $(OBJECTDIR)/sh6bench_leak.o

sh6bench_malloc: $(OBJECTDIR)/sh6bench.o
	mkdir -p $(DISTDIR)
	$(CC) $(CFLAGS) -o $(DISTDIR)/sh6benchMALLOC $(OBJECTDIR)/sh6bench.o

$(OBJECTDIR)/sh6bench_leak.o: sh6bench.c
	mkdir -p $(OBJECTDIR)
	$(CC) -c $(CFLAGS) -DMALLOC_ONLY -o $(OBJECTDIR)/sh6bench_leak.o sh6bench.c

$(OBJECTDIR)/sh6bench.o: sh6bench.c
	mkdir -p $(OBJECTDIR)
	$(CC) -c $(CFLAGS) -DWITHOUT_LEAK -DMALLOC_ONLY -o $(OBJECTDIR)/sh6bench.o sh6bench.c

$(OBJECTDIR)/sh6bench_str_leak.o: sh6bench.c
	mkdir -p $(OBJECTDIR)
	$(CC) -c $(CFLAGS) -I../../dist -DSTR_MALLOC -o $(OBJECTDIR)/sh6bench_str_leak.o sh6bench.c

$(OBJECTDIR)/sh6bench_str.o: sh6bench.c
	mkdir -p $(OBJECTDIR)
	$(CC) -c $(CFLAGS) -I../../dist -DWITHOUT_LEAK -DSTR_MALLOC -o $(OBJECTDIR)/sh6bench_str.o sh6bench.c

$(OBJECTDIR)/sh6bench_stm_leak.o: sh6bench.c
	mkdir -p $(OBJECTDIR)
	$(CC) -c $(CFLAGS) -I../../dist -DSTM_MALLOC -o $(OBJECTDIR)/sh6bench_stm_leak.o sh6bench.c

$(OBJECTDIR)/sh6bench_stm.o: sh6bench.c
	mkdir -p $(OBJECTDIR)
	$(CC) -c $(CFLAGS) -I../../dist -DWITHOUT_LEAK -DSTM_MALLOC -o $(OBJECTDIR)/sh6bench_stm.o sh6bench.c

$(OBJECTDIR)/sh6bench_strmc_leak.o: sh6bench.c
	mkdir -p $(OBJECTDIR)
	$(CC) -c $(CFLAGS) -I../../dist  -DSTRMC_MALLOC -o $(OBJECTDIR)/sh6bench_strmc_leak.o sh6bench.c

$(OBJECTDIR)/sh6bench_strmc.o: sh6bench.c
	mkdir -p $(OBJECTDIR)
	$(CC) -c $(CFLAGS) -I../../dist  -DWITHOUT_LEAK -DSTRMC_MALLOC -o $(OBJECTDIR)/sh6bench_strmc.o sh6bench.c

sh6bench_str_leak: ../../dist/libscm.so $(OBJECTDIR)/sh6bench_str_leak.o
	mkdir -p $(DISTDIR)
	$(CC) $(CFLAGS) $(OBJECTDIR)/sh6bench_str_leak.o -L../../dist -lscm -lpthread -o $(DISTDIR)/sh6benchSTR_LEAK
	
sh6bench_str: ../../dist/libscm.so $(OBJECTDIR)/sh6bench_str.o
	mkdir -p $(DISTDIR)
	$(CC) $(CFLAGS) $(OBJECTDIR)/sh6bench_str.o -L../../dist -lscm -lpthread -o $(DISTDIR)/sh6benchSTR
	
sh6bench_stm_leak: ../../dist/libscm.so $(OBJECTDIR)/sh6bench_stm_leak.o
	mkdir -p $(DISTDIR)
	$(CC) $(CFLAGS) $(OBJECTDIR)/sh6bench_stm_leak.o -L../../dist -lscm -lpthread -o $(DISTDIR)/sh6benchSTM_LEAK
	
sh6bench_stm: ../../dist/libscm.so $(OBJECTDIR)/sh6bench_stm.o
	mkdir -p $(DISTDIR)
	$(CC) $(CFLAGS) $(OBJECTDIR)/sh6bench_stm.o -L../../dist -lscm -lpthread -o $(DISTDIR)/sh6benchSTM
	
sh6bench_strmc_leak: ../../dist/libscm.so $(OBJECTDIR)/sh6bench_strmc_leak.o
	mkdir -p $(DISTDIR)
	$(CC) $(CFLAGS) $(OBJECTDIR)/sh6bench_strmc_leak.o -L../../dist -lscm -lpthread -o $(DISTDIR)/sh6benchSTRMC_LEAK
	
sh6bench_strmc: ../../dist/libscm.so $(OBJECTDIR)/sh6bench_strmc.o
	mkdir -p $(DISTDIR)
	$(CC) $(CFLAGS) $(OBJECTDIR)/sh6bench_strmc.o -L../../dist -lscm -lpthread -o $(DISTDIR)/sh6benchSTRMC

clean:
	rm -rf $(OBJECTDIR) $(DISTDIR)
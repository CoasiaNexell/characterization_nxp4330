######################################################################
#SUB_DIRS	:=
SUB_DIRS	+= main
SUB_DIRS	+= memtester
SUB_DIRS	+= cpu_sort
SUB_DIRS	+= cpuburn

all: compile_subdirs

compile_subdirs:
	@for dir in $(SUB_DIRS); do \
	make -C $$dir || exit $?;	\
	done

clean:
	@for dir in $(SUB_DIRS); do \
	make -C $$dir clean;		\
	done

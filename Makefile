.PHONY: all clean run debug help h ? docs

# my personal makefile, to do the following:
# 1. Compile all $(SRC_EXT) files with selected compiler, into separate object files, in $(BUILD_DIR)
# 2. Link all obj files we've just build against eachother
# 3. include any libs through ldlibs, prefer using $(shell pkg-config <name>) pattern
# 4. Offer options to debug segfaults, using adsan, usan, etc
# Do all of the above, but also make it pretty with fancy tput headers lol
all: run

EXE_DIR	:=bin
EXE_NAME :=test
EXE :=$(EXE_DIR)/$(EXE_NAME)


SRC_DIR	:=src
OBJ_DIR :=build

SRC_EXT	:=.c
OBJ_EXT :=.o


CC		:=	clang -std=c2x -Wall -Wimplicit-fallthrough  -Werror
CC		+= -Wno-unused-parameter -Wno-unused-variable -Wno-unused
SRC 	:=	$(wildcard $(SRC_DIR)/*$(SRC_EXT)) 
OBJS 	:= 	$(patsubst $(SRC_DIR)/%$(SRC_EXT),$(OBJ_DIR)/%$(OBJ_EXT),$(SRC))


# flags for the compiler only 
CFLAGS	:=-Iinclude $(shell pkg-config sdl3 --cflags)
CFLAGS	+=-Iinclude $(shell pkg-config sdl3-image --cflags)

LDFLAGS	:= 
LDLIBS	:= $(shell pkg-config sdl3 --libs)
LDLIBS  += $(shell pkg-config sdl3-image --libs)
ALLFLAGS:=


# libraries
CFLAGS+= 


CFLAGS	+=$(ALLFLAGS)
LDFLAGS+=$(ALLFLAGS)


$(OBJ_DIR)/%$(OBJ_EXT) : $(SRC_DIR)/%$(SRC_EXT)
	@$(ECHO_COMP_BANNER)
	$(CC) $(CFLAGS) -c $< -o $@ 

# build executable (linking obj -> bin)
$(EXE): $(EXE_DIR) $(OBJ_DIR) $(OBJS)
	@$(ECHO_LINK_BANNER)
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXE) $(LDLIBS)

# executing binary
run: $(EXE)
	@$(ECHO_EXE_BANNER)
	./$(EXE) $(ARGS)

	
#NOTE: AVOID PUTTING TARGETS IN CLEAN COMMAND! BE VERY SPECIFIC SO YOU DONT NUKE YOUR SOURCE FILES ON ACCIDENT
clean: 
	$(ECHO_CLEAN_BANNER)
	rm -f build/*.o src/*.o bin/*

# ------------ DEBUGGING ------------ #
debug: $(EXE)
	lldb -o run -- $(EXE) $(TERM_COLS) $(ARGS)

	
# Address san: lower overhead than thread-san, cleaner stack traces,
asan: CFLAGS  += -g -fsanitize=address -fno-omit-frame-pointer
asan: LDFLAGS += -fsanitize=address
asan: LDLIBS  += -fsanitize=address
asan: ASAN_ENV:= ASAN_OPTIONS=abort_on_error=1
asan: clean run 

usan: CFLAGS  += -fsanitize=undefined -fno-omit-frame-pointer -g
usan: LDFLAGS += -fsanitize=undefined 
usan: LDLIBS  += -fsanitize=undefined 
usan: clean run 


ausan: CFLAGS  += -fsanitize=address,undefined -fno-omit-frame-pointer -g
ausan: LDFLAGS += -fsanitize=address,undefined
ausan: LDLIBS  += -fsanitize=address,undefined
ausan: CFLAGS+= -fsanitize-address-use-after-scope
ausan: ASAN_ENV:= ASAN_OPTIONS=abort_on_error=1

ausan: CFLAGS  += -fsanitize=undefined -fno-omit-frame-pointer -g
ausan: LDFLAGS += -fsanitize=undefined 
ausan: LDLIBS  += -fsanitize=undefined 
ausan: clean run

tsan: CFLAGS  += -fsanitize=thread -fno-omit-frame-pointer 
tsan: LDFLAGS += -fsanitize=thread
tsan: LDLIBS  += -fsanitize=thread
tsan: clean run 

# ------------ EXTRAS ------------ #
$(OBJ_DIR):
	@mkdir -p $@

$(EXE_DIR):
	@mkdir -p $@

help h ? docs: HELP
.PHONY: HELP
HELP:
	
	@$(FMT_REV)
	@printf "===== MAKE VARIABLES =====\n"; $(FMT_RESET)
	@$(FMT_ALT1) 	printf "SRC      = $(SRC)\n" 
	@$(FMT_ALT2) 	printf "EXE_DIR  = $(EXE_DIR )\n"
	@$(FMT_ALT1)	printf "EXE_NAME = $(EXE_NAME)\n"
	@$(FMT_ALT2)	printf "EXE      = $(EXE)\n"
	@$(FMT_ALT1)	printf "SRC_DIR  = $(SRC_DIR)\n"
	@$(FMT_ALT2)	printf "OBJ_DIR  = $(OBJ_DIR)\n"
	@$(FMT_ALT1)	printf "SRC_EXT  = $(SRC_EXT)\n"
	@$(FMT_ALT2)	printf "OBJ_EXT  = $(OBJ_EXT)\n"
	@$(FMT_ALT1)	printf "CC     = $(CC)\n"
	@$(FMT_ALT2)	printf "SRC      = $(SRC)\n"
	@$(FMT_ALT1)	printf "OBJS     = $(OBJS)\n"
	@$(FMT_ALT2)	printf "CFLAGS  = $(CFLAGS)\n"
	@$(FMT_ALT1)	printf "LDFLAGS  = $(LDFLAGS)\n"
	@$(FMT_ALT2)	printf "LDLIBS   = $(LDLIBS)\n"
	@$(FMT_ALT1)	printf "ALLFLAGS = $(ALLFLAGS)\n"




# ------------ PRETTY PRINTING ------------ #
FMT_RESET	:=tput sgr0;
FMT_REDBANNER	:=tput rev; tput bold; tput setaf 1;
FMT_GREENBANNER	:=tput rev; tput bold; tput setaf 2;
FMT_YELLOWBANNER:=tput rev; tput bold; tput setaf 3;
FMT_REV		:=tput rev; tput bold;


FMT_ALT1	:= tput setaf 7;
FMT_ALT2	:= tput sgr0;


ECHO_LINK_BANNER := @$(FMT_YELLOWBANNER) 	printf " LINKING OBJ to BIN -> "; $(FMT_RESET) 	printf "\t"
ECHO_COMP_BANNER := @$(FMT_GREENBANNER)		printf " COMPILE SRC to OBJ -> "; $(FMT_RESET)	printf "\t"
ECHO_EXE_BANNER := @$(FMT_REDBANNER) 		printf " EXECUTING BINARY -> "; $(FMT_RESET)	printf "\t"
ECHO_CLEAN_BANNER:= @$(FMT_REV) 		printf " REMOVING EXEUTABLES AND OBJECT FILES... "; $(FMT_RESET)printf "\t"


# EXTRA MAKEFILE STUFF:
# :_______1-1_______: To exclude certain files from compilation:
# EXCLUDE_PAT	:= unit_tests.c
# SRC 		:= $(filter-out src/$(EXCLUDE_SRC), $(wildcard src/*.c))

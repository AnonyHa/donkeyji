############################################################################### 
# 
# Generic Makefile for C/C++ Program 
# 
# Author: whyglinux (whyglinux AT hotmail DOT com) 
# Date:   2006/03/04 

# Description: 
# The makefile searches in <SRCDIRS> directories for the source files 
# with extensions specified in <SOURCE_EXT>, then compiles the sources 
# and finally produces the <PROGRAM>, the executable file, by linking 
# the objectives. 

# Usage: 
#   $ make           compile and link the program. 
#   $ make objs      compile only (no linking. Rarely used). 
#   $ make clean     clean the objectives and dependencies. 
#   $ make cleanall  clean the objectives, dependencies and executable. 
#   $ make rebuild   rebuild the program. The same as make clean && make all. 
#============================================================================== 

## Customizing Section: adjust the following if necessary. 
##============================================================================= 

# The executable file name. 
# It must be specified. 
# PROGRAM   := a.out    # the executable name 
PROGRAM   := test

# The directories in which source files reside. 
# At least one path should be specified. 
# SRCDIRS   := .        # current directory 
SRCDIRS   := . dir1

# The source file types (headers excluded). 
# At least one type should be specified. 
# The valid suffixes are among of .c, .C, .cc, .cpp, .CPP, .c++, .cp, or .cxx. 
# SRCEXTS   := .c      # C program 
# SRCEXTS   := .cpp    # C++ program 
# SRCEXTS   := .c .cpp # C/C++ program 
SRCEXTS   := .c

# The flags used by the cpp (man cpp for more). 
# CPPFLAGS  := -Wall -Werror # show all warnings and take them as errors 
CPPFLAGS  :=  -Wall -g

# The compiling flags used only for C. 
# If it is a C++ program, no need to set these flags. 
# If it is a C and C++ merging program, set these flags for the C parts. 
LUA_PATH_INC = "D:\Program Files\Lua\5.1\include" 
CFLAGS    := -I $(LUA_PATH_INC)
CFLAGS    += 

# The compiling flags used only for C++. 
# If it is a C program, no need to set these flags. 
# If it is a C and C++ merging program, set these flags for the C++ parts. 
CXXFLAGS  := 
CXXFLAGS  += 

# The library and the link options ( C and C++ common). 
LUA_PATH_LIB = "D:\Program Files\Lua\5.1\lib"

LDFLAGS   := 
LDFLAGS   += -L $(LUA_PATH_LIB) -llua51

## Implict Section: change the following only when necessary. 
##============================================================================= 
# The C program compiler. Uncomment it to specify yours explicitly. 
CC      = gcc 

# The C++ program compiler. Uncomment it to specify yours explicitly. 
CXX     = g++ 

# Uncomment the 2 lines to compile C programs as C++ ones. 
#CC      = $(CXX) 
#CFLAGS  = $(CXXFLAGS) 

# The command used to delete file. 
#RM        = rm -f 

## Stable Section: usually no need to be changed. But you can add more. 
##============================================================================= 
SHELL   = /bin/sh 
SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS)))) 
OBJS    = $(foreach x,$(SRCEXTS), $(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES)))) 
DEPS    = $(patsubst %.o,%.d,$(OBJS)) 

.PHONY : all objs clean cleanall rebuild 

#all是默认目标，只需"make"即可，无需"make all"
all : $(PROGRAM)

# Rules for creating the dependency files (.d). 
#--------------------------------------------------- 
%.d : %.c 
	@$(CC) -MM -MD $(CFLAGS) $< -o $@

%.d : %.C 
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.cc 
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.cpp 
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.CPP 
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.c++ 
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.cp 
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@

%.d : %.cxx 
	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@

# Rules for producing the objects. 
#--------------------------------------------------- 
objs : $(OBJS)


# Rules for producing the executable. 
#---------------------------------------------- 
$(PROGRAM) : $(OBJS) 
ifeq ($(strip $(SRCEXTS)), .c)  # C file 
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
else                            # C++ file 
	$(CXX) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
endif 
#$(CC) -o $@ $(filter %.o, $+) $(LDFLAGS)
#$(CXX) -o $@ $(filter %.o, $+) $(LDFLAGS)

%.o : %.c 
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

%.o : %.C 
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.CPP
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.c++
	$(CXX -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

%.o : %.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

-include $(DEPS)

rebuild: clean all

clean :
	@$(RM) *.o *.d

cleanall: clean
	@$(RM) $(PROGRAM) $(PROGRAM).exe

### End of the Makefile ##  Suggestions are welcome  ## All rights reserved ### 
############################################################################### 

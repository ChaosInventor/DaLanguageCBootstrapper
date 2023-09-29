#------------------------------------------------------------------------------#
#	***
#	Make file meant for building everything in the bootstrapper, from tests
#	to executables and objects.
#
#	Below is an outline of the structure of the file, use your text editor's
#	find functionality to navigate to the sections, ex for vim: `/#1.` or
#	`/#2.3.2`.
#	***
#------------------------------------------------------------------------------#

#--------------------------------Table of contents-----------------------------#
#	1. Configurable variables
#		1.1 Binaries
#		1.2 Flags
#		1.3 Build directories
#	2. Static variables
#		2.1 Invocations
#		2.2 Files
#		2.3 Functions
#	3. Recipes
#		3.1 Phony recipes
#		3.2 Build directory recipes
#		3.3 Pattern recipes
#			3.3.1 File dependencies
#			3.3.2 Compilation
#	4. Includes
#------------------------------------------------------------------------------#



#------------------------------------------------------------------------------#
#1. Configurable variables
#
#	Configurable variables that should be overwritten using the environment.
# Not overwriting gives them sane defaults.
#------------------------------------------------------------------------------#

#------------------------------------------------------------------------------#
#1.1 Binaries
#
##	A list of programs and utilities used by the makefile.
#------------------------------------------------------------------------------#

#Command name for the C compiler.
CC ?= cc

#Command name for the rm utility.
RM ?= rm

#Command name for the mkdir utility.
MKDIR ?= mkdir

#Command name for the printf utility
PRINTF ?= printf

#Command name for a sed compatible program.
SED ?= sed

#Command name for an awk interpreter.
AWK ?= awk

#Command name for the tr utility.
TR ?= tr

#------------------------------------------------------------------------------#
#1.2 Flags
#------------------------------------------------------------------------------#

#The list of flags that are passed to each invocation of the C compiler.
#
#Must only include flags that do not impact which files the compiler generates.
#For example, #flags such line `-c`, `-M` must NOT be included in the list.
#
#Include directories are handled by another variable, `CCINCLUDES`, do not put
#`-I` flags here, put the include directories in `CCINCLUDES`.
CCFLAGS ?= -Wall -Wextra -pedantic -std=c99 -g -Og

#A list of directories that get `-I`ed with each invocation of the C compiler.
CCINCLUDES ?= include

#------------------------------------------------------------------------------#
#1.3 Build directories
#------------------------------------------------------------------------------#

#Directory where build artifacts are placed. See #3.2 section for the structure
#of this directory.
BUILD_DIR ?= build

#Directory where all `.c` files without a main function can be found.
#
#NOTE: Each source file here is **assumed** to have a one corresponding header,
#and that header to be implemented in that one source file only.
SOURCE_DIR ?= src

#Directory where all `.c` files with a main function that is supposed to act as
#a test can can be found.
TESTS_DIR ?= Tests

#Directory where all `.c` files with a main function that is supposed to as an
#interpreter for a specific DaLanguage instruction system top definition can be
#found.
INTERPRETERS_DIR ?= Interpreters


#------------------------------------------------------------------------------#
#2. Static variables
#
#	Setup of static variables, mostly meant as short hands. Derived from
#configurable variables. Do not overwrite externally.
#------------------------------------------------------------------------------#

#------------------------------------------------------------------------------#
#2.1 Invocations
#------------------------------------------------------------------------------#

#C complier short-hand invocation, more convenient to use.
CCINV := $(CC) $(CCFLAGS) $(addprefix -I,$(CCINCLUDES))

#------------------------------------------------------------------------------#
#2.2 Files
#------------------------------------------------------------------------------#

#All source files that need to be compiled.
sources := $(wildcard $(SOURCE_DIR)/*.c) $(wildcard $(TESTS_DIR)/*.c) $(wildcard $(INTERPRETERS_DIR)/*.c)

#All object files that need to be created. One for each source, they're all
#placed in the build directory's `obj` subdirectory. See #3.2
objects := $(addprefix $(BUILD_DIR)/obj/,$(subst .c,.o,$(sources)))

#All generated file dependencies for each source. See #3.3
sourcesDep := $(addprefix $(BUILD_DIR)/dep/,$(addsuffix .d,$(sources)))

#All source files that contain a main function and need to be compiled.
executableSources := $(wildcard $(TESTS_DIR)/*.c) $(wildcard $(INTERPRETERS_DIR)/*.c)

#All executables that need to be compiled, one for each source containing a main
#function. See #3.3
executables := $(addprefix $(BUILD_DIR)/bin/,$(basename $(executableSources)))

#All object dependencies for each executable object. See #3.3
executablesDep := $(addsuffix .d,$(addprefix $(BUILD_DIR)/dep/,$(basename $(executableSources))))

#------------------------------------------------------------------------------#
#2.3 Functions
#------------------------------------------------------------------------------#

#Used to redirect stdout and stderr of a program. The first argument is the name
#of the file the program is operating on, with a directory prefix. The directory
#prefix is assumed to have already been made.
#
#This function is mainly used to redirect the output of the compiler. See #3.3.2
redirect_log = 1> $(BUILD_DIR)/log/$(1).stdout.txt 2> $(BUILD_DIR)/log/$(1).stderr.txt


#------------------------------------------------------------------------------#
#3. Recipes
#------------------------------------------------------------------------------#

#------------------------------------------------------------------------------#
#3.1 Phony recipes
#------------------------------------------------------------------------------#

#The default recipe.
all: objects executables

#Removes the build directory.
clean:
	$(RM) -r $(BUILD_DIR)

#All objects.
objects: $(objects)

#All executables.
executables: $(executables)

.PHONY = all clean objects executables

#------------------------------------------------------------------------------#
#3.2 Build directory recipes
#------------------------------------------------------------------------------#

#The build directory is comprised of the following sub directories:
#	- bin -- Directory containing executables. Executables are created form
#	  each source file containing a main function. See #2.2 for the full
#	  list. Executables hold the base name(the name without the `.c`
#	  extension) of the source file containing the main function. A copy of
#	  the directory path of the source holding the main function is made in
#	  the `bin` directory under which the executable is placed.
#	- obj -- Directory where objects for each source are held. An object
#	  file is held for each source, named the exact same as the source
#	  except that the extension is `.o` instead of `.c` and placed under a
#	  copy of the directory path of the source file, all under the `obj`
#	  directory.
#	- dep -- Directory where makefile dependencies generated by the compiler
#	  are held. Each source file and executable has a dependency file, named
#	  the same as the file, with `.d` appended to the end. All dependency
#	  files are held under copies of their respective source file's
#	  directory paths.
#	- log -- Directory holding stdout and stderr outputs of compiler
#	  invocations used for compilation. Files are named the same as the
#	  outputs, where `.txt.stdout` is appended to the file holding stdout
#	  output, while `.txt.stderr` is appended to the file holding stderr
#	  output. Directory paths of the compiled files is also included. See
#	  #2.3 and #3.3.2 for more details.
$(BUILD_DIR):
	$(MKDIR) -p $@
$(BUILD_DIR)/bin: | $(BUILD_DIR)
	$(MKDIR) -p $@
$(BUILD_DIR)/obj: | $(BUILD_DIR)
	$(MKDIR) -p $@
$(BUILD_DIR)/dep: | $(BUILD_DIR)
	$(MKDIR) -p $@
$(BUILD_DIR)/log: | $(BUILD_DIR)
	$(MKDIR) -p $@

#------------------------------------------------------------------------------#
#3.3 Pattern recipes
#------------------------------------------------------------------------------#

#------------------------------------------------------------------------------#
#3.3.1 File dependencies
#------------------------------------------------------------------------------#

#Recipe that has the C compiler generate header dependencies for each source.
#This way, make will automatically remake the corresponding object file if any
#of the included headers or the source change.
#
#In order to make sure that the dependency file gets regenerated in case the
#source file or its headers are changed, it is put at the start of the file
#using echo. The recipe that the compiler writes is then appended, making the
#dependency file a target for the generated recipe.
#
#These files are later included. See section #4.
#
#Breakdown of recipe, line by line:
#
# 1. Stop if anything fails.
# 2. Make a copy of the directory path of the source in $(BUILD_DIR)/dep/, the
#    directory path we need is actually inside `%`.
# 3. Make the dependency file a target of the generated recipe so it is remade
#    if anything changes.
# 4. Give the object file the proper directory prefix, cc could do it, but it'd
#    require too many flags.
# 5. Generate the dependencies using the compiler, if the compiler gives an
#    error, empty the file by calling false, making the recipe fail and
#    truncating the dependency file. Dependency file needs to be truncated in
#    order to prevent make from including a malformed dependency file that
#    will render the current makefile unusable.
$(BUILD_DIR)/dep/%.c.d: %.c | $(BUILD_DIR)/dep
	@set -e;
	$(MKDIR) -p $(dir $@)
	$(PRINTF) '$@\\\n' > $@
	$(PRINTF) '$(dir $(subst /dep/,/obj/,$@))' >> $@
	$(CCINV) -M $< >> $@ || false > $@

#Recipe that uses the C compiler and some arcane unholy shell incantations to
#extract a list of objects needed to build an executable.
#
#The generate file has two lines, the first line contains the name of the
#executable in the build directory, followed by a colon and an escaped newline,
#creating a recipe. The second line contains a space separated list of object
#files needed to build the executable. The object files are inferred based on
#the headers the executable's source file includes.
#
#These files are later included, see #4, and used for building the executable,
#see #3.3.2.
#
#Breakdown of the recipe, line by line:
# 1. Stop if anything fails.
# 2. Make sure we have the source's directory path under `dep`.
# 3. Generate included user header files and put them in temporary file.
# 4. Make the executable file a target of the soon to be generated recipe and
#    start a new escaped line.
# 5. Replace each escaped newline in the temporary file dependencies with a space,
#    makes it easier to deal with later using awk. Put the result in a temporary
#    file.
# 6. Find all files that end in `.h` in the temporary file. Pipe them down. Each
#    file is now on a separate line. Pipe forward.
# 7. Strip the header file's directory prefix and replace it with the source
#    directory. Pipe output down.
# 8. Replace `.h` with `.o` in the headers list. We now have object file names.
# 9. Prefix the directory where these object files are built. We now have a list
#    of all the objects and their location that are needed to build the current
#    executable. Pipe onward.
# 10. Put the object files in a space separated list on a single line. Append
#     the output to the dependency file we're generating
# 11. Read the temporary file again and find the object file where the main
#     function is, since that one does not have a header. Stick the directory
#     prefix to it, remove and colon at the end and pipe for the last time.
#     TODO: Is cc guaranteed to put the object file before the colon? Might
#     cause trouble in some compilers.
# 12. Append the last object file to the generated list. The list in now
#     complete along with the dependency file.
# 13. Remove the temporary files.
$(BUILD_DIR)/dep/%.d: %.c | $(BUILD_DIR)/dep
	@set -e;
	$(MKDIR) -p $(dir $@)
	$(CCINV) -MM $< > $@.tmp.tmp
	$(PRINTF) '$(basename $(subst /dep/,/bin/,$@)):\\\n' > $@
	$(TR) '\\\n' ' ' < $@.tmp.tmp > $@.tmp
	$(AWK) -F' ' '{ for(i=1; i<=NF; i++) { if($$i ~ /.*\.h/) { print $$i } } }' < $@.tmp |\
	$(SED) -n 's,\(.*/\)*\(.*\.h\),$(SOURCE_DIR)/\2,gp' |\
	$(SED) -n 's/\.h/.o/gp' |\
	$(AWK) '{ for(i=1; i <= NF; i++) { print "$(BUILD_DIR)/obj/"$$i } }' |\
  	$(TR) '\n' ' ' >> $@
	$(AWK) -F' ' '{ for(i=1; i<=NF; i++) { if($$i ~ /.*\.o:/) { print "$(dir $(subst /dep/,/obj/,$@))"substr($$i, 1, length($$i) - 1) } } }' < $@.tmp |\
  	$(TR) '\n' ' ' >> $@
	$(RM) $@.tmp $@.tmp.tmp

#------------------------------------------------------------------------------#
#3.3.2 Compilation
#------------------------------------------------------------------------------#

#Recipe to compile an object file from a source, after the source's dependencies
#have been generated.
#
#Breakdown of the recipe, line by line:
# 1. Make sure we have a copy of the source's directory prefix under `obj`.
# 2. Compile the object file, see #2.1 for the variables.
$(BUILD_DIR)/obj/%.o: %.c $(BUILD_DIR)/dep/%.c.d | $(BUILD_DIR)/obj $(BUILD_DIR)/log
	$(MKDIR) -p $(dir $@)
	$(MKDIR) -p $(dir $(subst obj/,log/,$@))
	$(CCINV) -c $< -o $@ $(call redirect_log,$(subst $(BUILD_DIR)/obj/,,$@))

#Recipe to create an executable from it's object file dependencies.
#
#See #3.3.1 second recipe for the object dependency file.
#
#Breakdown of the recipe, line by line:
# 1. Make sure we have a copy of the executable's directory prefix under `bin`.
# 2. Compile using the object dependency file. Skip its first line, and use the
#    list in the second as the inputs. See #3.3.1 second recipe for the second
#    line contains.
$(BUILD_DIR)/bin/%: $(BUILD_DIR)/dep/%.d | $(BUILD_DIR)/bin $(BUILD_DIR)/log
	$(MKDIR) -p $(dir $@)
	$(MKDIR) -p $(dir $(subst bin/,log/,$@))
	$(CCINV) $$($(SED) -n '1!p' < $<) -o $@ $(call redirect_log,$(subst $(BUILD_DIR)/bin/,,$@))


#------------------------------------------------------------------------------#
#4. Includes
#------------------------------------------------------------------------------#

#Header dependencies for each source file. See section #3.3.1, the first recipe.
include $(sourcesDep)

#Object dependencies for executables. See section #3.3.1, second recipe.
include $(executablesDep)


#############
# Variables #
#############

SHELL = bash


# Compilación

CXX           := g++
CXXOPTS       := -ggdb -Wall -Wextra -std=gnu++0x #-Dnullptr=NULL -DUNICODE_AGENT_STRINGS

LDLIBS        := boost_program_options

clean-formats := $(clean-formats) o gch

sources       := $(wildcard *.cc)
objects       := $(sources:%.cc=%.o)

# Documentación

pandoc := pandoc

pandoc-opts :=            \
        --normalize       \
        --number-sections \
        --preserve-tabs   \
        --section-divs    \
        --standalone

pandoc-latex-opts := --latex-engine=xelatex -H doc/header.tex

doc-formats := txt html pdf

clean-formats := $(clean-formats) aux log out toc



###########
# Recetas #
###########

# Especiales

clean =                                        \
        shopt -s globstar;                     \
        rm -f $(clean-formats:%=./**.%) basico


# Compilación

build-executable = $(CXX) $(CXXOPTS) -o $@ $^ $(LDLIBS:%=-l%)
build-object     = $(CXX) $(CXXOPTS) -o $@ -c $<


# Documentación

pandoc-recipe =                                                    \
        sed < $<                                                   \
                -e 's/^@\(@[^@]\+\)*@$(suffix $@)\(@[^@]\+\)*@@//' \
                -e '/^@\(@[^@]\+\)\+@@/d'                          \
        | $(pandoc) $(pandocflags) -o $@

pandoc-latex-recipe = $(pandoc-recipe) $(pandoc-latex-args)



#####################
# Reglas especiales #
#####################

.PHONY: all doc clean

# Regla default
all: basico

doc: $(doc-formats:%=doc/README.%)

clean: ; $(clean)



#################
# Reglas patrón #
#################


# Compilación

# Archivos de objeto
%.o: %.cc ; $(build-object)


# Documentación

%.txt : %.md                ; $(pandoc-recipe)
%.html: %.md                ; $(pandoc-recipe)
%.pdf : %.md doc/header.tex ; $(pandoc-latex-recipe)
%.tex : %.md doc/header.tex ; $(pandoc-latex-recipe)



############################################
# Reglas cuyos targets son archivos reales #
############################################

# Cómo hacer el ejecutable principal:
basico: $(objects); $(build-executable)

# Dependencias de basico.o (todo lo necesario para compilar basico.cc)

#        .o       .cc Agent.hh TagAgent.hh
     main.o:  main.cc Agent.hh TagAgent.hh
    Agent.o: Agent.cc Agent.hh
 TagAgent.o: Agent.cc Agent.hh TagAgent.hh

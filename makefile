# **********************************************
# * Makefile para programas escritos em C     **
# *                                           **
# *  Autor: Ricky Lemes Habegger              **
# *                                           **
# **********************************************
# ==============================================

#-----------------------------------------------
#----              Variaveis                ----
#-----------------------------------------------

# Nome do projeto
ifndef NOME_PROJ
NOME_PROJ=pong
endif
ifndef MAIN_FILE
MAIN_FILE=espera-ocupada
endif
ifndef MAIN_DIR
MAIN_DIR=test/src/
endif

# PATHs
SOURCE_DIR=./main/src
INCLUDE_DIR=./main/include
LIB_DIR=
OBJ_DIR=./objetos
FILTER_OUT_SRC=
FILTER_OUT_INCLUDE=

# Compilador
CC=gcc

# headers de bibliotecas externas
INCLUDE_EXTERNO=

# Arquivos fonte
SOURCE_TYPE=.c
SOURCES=$(filter-out $(FILTER_OUT_SRC), $(wildcard $(SOURCE_DIR)/*$(SOURCE_TYPE)))

# Arquivo headers
HEADER_TYPE=.h
HEADERS=$(filter-out $(FILTER_OUT_INCLUDE), $(wildcard $(INCLUDE_DIR)/*$(HEADER_TYPE)))

# Objetos que serão gerados
OBJ=$(subst $(SOURCE_TYPE),.o,$(subst $(SOURCE_DIR),$(OBJ_DIR), $(subst $(SOURCE_DIR)/$(MAIN_FILE)$(SOURCE_TYPE),,$(SOURCES))))

FLAGS=-lpthread

# Flags para o compilador
CC_FLAGS=-c                     \
         -Wall                  \
		 -D_POSIX_C_SOURCE      \
		 # -Wextra                \
         # -pedantic              \
         #-pedantic-errors       \

# Ativa modo debug
ifeq ($(DEBUG), 1)
	CC_FLAGS+= -g \
			   -DDEBUG
endif

ifeq ($(CC), gcc)
	CC_FLAGS+= -std=c99
endif

# Comando para apagar arquivo não necessarios
RM = rm -f

#-----------------------------------------------
#----       Targets de Compilação           ----
#-----------------------------------------------

# Criar os objetos e executavel
all: diretorios $(NOME_PROJ)
	@ echo -e "\e[01mArquivo binario criado: \e[01;04;32m$(NOME_PROJ)\e[00m"

# linkar os objetos e gerar o executavel
$(NOME_PROJ): $(OBJ) $(OBJ_DIR)/$(MAIN_FILE).o
	@ echo -e " "
	@ echo -e "\e[01mCriando arquivo binario:\e[01;32m $@ \e[00m"
	@ $(CC) $^ -o $@ $(FLAGS)

# Compilar todas as sources
$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%$(SOURCE_TYPE) #$(INCLUDE_DIR)/%$(HEADER_TYPE)
	@ echo -e "Compilando: \e[00;31m $< \e[00m"
	@ $(CC) $< $(CC_FLAGS) -o $@ -I $(INCLUDE_DIR) $(INCLUDE_EXTERNO)

# Compilar a main
$(OBJ_DIR)/$(MAIN_FILE).o: $(MAIN_DIR)/$(MAIN_FILE)$(SOURCE_TYPE) $(HEADERS)
	@ echo -e "Compilando: \e[00;31m $< \e[00m"
	@ $(CC) $< $(CC_FLAGS) -o $@ -I $(INCLUDE_DIR) $(INCLUDE_EXTERNO)

# Criar os diretorios para objetos
diretorios:
	@ mkdir -p $(OBJ_DIR) $(LIB_DIR)

# Remover os objetos gerados na compilacao
clean:
	@ $(RM) $(NOME_PROJ) $(OBJ_DIR)/*.o  $(LIB_DIR)/*.a *~
	@ rmdir --ignore-fail-on-non-empty $(OBJ_DIR) $(LIB_DIR)

# Criar uma biblioteca estatica
lib: diretorios $(OBJ)
	@ echo -e "\e[01mCriando biblioteca estática:\e[01;32m $(NOME_PROJ) \e[00m"
	@ ar -rcs $(LIB_DIR)/lib$(NOME_PROJ).a $(OBJ)
	@ echo -e "\e[01mBiblioteca estática criada: \e[01;04;32m$(NOME_PROJ)\e[00m"

# Recompilar o programa
rebuild: clean all

# Instalacao (apenas para bibliotecas estaticas)
install: lib
	@ sudo cp -p $(LIB_DIR)/lib$(NOME_PROJ).a /usr/local/lib/
	@ sudo mkdir -p /usr/local/include/$(NOME_PROJ)
	@ sudo cp -p -r $(INCLUDE_DIR)/* /usr/local/include/$(NOME_PROJ)/
	@ sudo ldconfig
	@ echo -e "\e[01m\e[01;04;32m$(NOME_PROJ)\e[01;0m instalada.\e[00m"

# Desinstalacao (apenas para bibliotecas estaticas)
uninstall:
	@ sudo rm /usr/local/lib/lib$(NOME_PROJ).a
	@ sudo rm /usr/local/include/$(NOME_PROJ)/*
	@ sudo rmdir --ignore-fail-on-non-empty /usr/local/include/$(NOME_PROJ)
	@ sudo ldconfig
	@ echo -e "\e[01m\e[01;04;32m$(NOME_PROJ)\e[01;0m removida.\e[00m"

# Gerar o executavel e executar
run: all
	@ ./$(NOME_PROJ) 

test: all
	@ ./$(NOME_PROJ) > test_result.txt
	@ diff test_result.txt test/resources/$(MAIN_FILE).txt -s
	
# Evita ambiguidade com arquivo da source
.PHONY: all clean lib rebuild install uninstall diretorios

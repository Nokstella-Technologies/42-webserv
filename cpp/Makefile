NAME			=	webserver
UNITEST			=	unitest
CC				=	c++

CFLAGS			=	-Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address
CFLAGSTEST 		= 	-g3 -std=c++11

HEADER			=	-I src/header

HEADER_TEST 	=	-I test/header

CLASS			= 	Configuration.cpp  Server.cpp Routes.cpp SocketServer.cpp WebServer.cpp Request.cpp Response.cpp ResponseStatic.cpp
 
UTILS			= 	ends_with.cpp trim.cpp strtokS.cpp starts_with.cpp split.cpp 

SRC				=	main.cpp $(addprefix class/, $(CLASS)) $(addprefix utils/, $(UTILS))

TEST_FILES		=	test/main_test.cpp $(addprefix test/class/, $(CLASS:%.cpp=%_test.cpp)) $(addprefix test/utils/, $(UTILS:%.cpp=%_test.cpp))  $(addprefix src/class/, $(CLASS)) $(addprefix src/utils/, $(UTILS))

SOURCES_DIR		=	./src

OBJ_DIR			=	obj

SOURCES			=	$(addprefix $(SOURCES_DIR)/, $(SRC))

OBJS			=	$(SOURCES:$(SOURCES_DIR)/%.cpp=$(OBJ_DIR)/%.o)


$(OBJ_DIR)/%.o:		$(SOURCES_DIR)/%.cpp
					$(CC) $(CFLAGS) $(HEADER) -c $< -o $@

all:				$(NAME)

$(NAME):			$(OBJ_DIR) $(OBJS)
					$(CC) $(CFLAGS) $(HEADER) $(OBJS) -o $(NAME)

$(OBJ_DIR):
					mkdir -p $(OBJ_DIR)
					mkdir -p $(addprefix $(OBJ_DIR)/, class)
					mkdir -p $(addprefix $(OBJ_DIR)/, utils)

clean:
					rm -rf $(OBJ_DIR)


fclean:				clean
					rm -rf $(NAME)
					rm -rf $(UNITEST)

re:					fclean all


test: $(UNITEST)
	./$(UNITEST) 
	rm -rf $(UNITEST)

retest: fclean test

$(UNITEST):
	@$(CC) $(CFLAGSTEST) $(HEADER) $(HEADER_TEST) $(TEST_FILES) -o $(UNITEST)

.PHONY:	test all clean fclean re
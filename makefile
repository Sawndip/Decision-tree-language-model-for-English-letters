default: all

CC = g++

SRC_DIR = ./src/

# link math package
LINK = -lm

all: trees

trees: $(SRC_DIR)agglo_cluster.cpp $(SRC_DIR)bit_encode_decision_tree.cpp $(SRC_DIR)chou_tree.cpp $(SRC_DIR)client.cpp
	$(CC) $(SRC_DIR)agglo_cluster.cpp $(SRC_DIR)client.cpp $(SRC_DIR)bit_encode_decision_tree.cpp $(SRC_DIR)chou_tree.cpp -o test_tree $(LINK)


#include <iostream>
using namespace std;

#include <fstream>
#include <sstream>
#include <time.h>
#include <omp.h>
#include<pmmintrin.h>
#include<xmmintrin.h>
#include<immintrin.h>



#define NUM_THREADS 4
#define COL 1011
#define ROW1 539
#define ROW2 263
#define FILE1 "S:/Courses/ParallelProgramming/Data_Groebner/4_1011_539_263/1.txt"
#define FILE2 "S:/Courses/ParallelProgramming/Data_Groebner/4_1011_539_263/2.txt"
#define FILE3 "S:/Courses/ParallelProgramming/Data_Groebner/4_1011_539_263/3.txt"
#define FILEOUT1 "S:/Courses/ParallelProgramming/Data_Groebner/output/out1.txt"
#define FILEOUT2 "S:/Courses/ParallelProgramming/Data_Groebner/output/out2.txt"
/*
FILE11 "/home/sTest/data/Groebner/1_130_22_8/1.txt"
FILE12 "/home/sTest/data/Groebner/1_130_22_8/2.txt"
FILE13 "/home/sTest/data/Groebner/1_130_22_8/3.txt"
FILE21 "/home/sTest/data/Groebner/2_254_106_53/1.txt"
FILE22 "/home/sTest/data/Groebner/2_254_106_53/2.txt"
FILE23 "/home/sTest/data/Groebner/2_254_106_53/3.txt"
FILE31 "/home/sTest/data/Groebner/3_562_170_53/1.txt"
FILE32 "/home/sTest/data/Groebner/3_562_170_53/2.txt"
FILE33 "/home/sTest/data/Groebner/3_562_170_53/3.txt"
FILE41 "/home/sTest/data/Groebner/4_1011_539_263/1.txt"
FILE42 "/home/sTest/data/Groebner/4_1011_539_263/2.txt"
FILE43 "/home/sTest/data/Groebner/4_1011_539_263/3.txt"
FILE51 "/home/sTest/data/Groebner/5_2362_1226_453/1.txt"
FILE52 "/home/sTest/data/Groebner/5_2362_1226_453/2.txt"
FILE53 "/home/sTest/data/Groebner/5_2362_1226_453/3.txt"
FILE61 "/home/sTest/data/Groebner/6_3799_2759_1953/1.txt"
FILE62 "/home/sTest/data/Groebner/6_3799_2759_1953/2.txt"
FILE63 "/home/sTest/data/Groebner/6_3799_2759_1953/3.txt"
FILE71 "/home/sTest/data/Groebner/7_8399_6375_4535/1.txt"
FILE72 "/home/sTest/data/Groebner/7_8399_6375_4535/2.txt"
FILE73 "/home/sTest/data/Groebner/7_8399_6375_4535/3.txt"
FILE81 "/home/sTest/data/Groebner/8_23075_18748_14325/1.txt"
FILE82 "/home/sTest/data/Groebner/8_23075_18748_14325/2.txt"
FILE83 "/home/sTest/data/Groebner/8_23075_18748_14325/3.txt"
FILE91 "/home/sTest/data/Groebner/9_37960_29304_14921/1.txt"
FILE92 "/home/sTest/data/Groebner/9_37960_29304_14921/2.txt"
FILE93 "/home/sTest/data/Groebner/9_37960_29304_14921/3.txt"
FILEA1 "/home/sTest/data/Groebner/10_43577_39477_54274/1.txt"
FILEA2 "/home/sTest/data/Groebner/10_43577_39477_54274/2.txt"
FILEA3 "/home/sTest/data/Groebner/10_43577_39477_54274/3.txt"
FILEB1 "/home/sTest/data/Groebner/11_85401_5724_756/1.txt"
FILEB2 "/home/sTest/data/Groebner/11_85401_5724_756/2.txt"
FILEB3 "/home/sTest/data/Groebner/11_85401_5724_756/3.txt"
*/
struct Node {
	int index;
	Node* next;
	Node() {
		index = -1;
		next = nullptr;
	}
};
struct List {
	Node* head;
	Node* tail;
	List() {
		head = new Node();
		tail = new Node();
		head->next=tail;
	}
	int getFirstNum() {
		return head->next->index;
	}
	int doXor(Node* _head){
		//！！！xor应该只削一次，全部异或，而不是逮着一个使劲反复削
		//每次删除固定的，都要返回一个新的固定，然后由ori函数判断是否存在L1[first].
		//选择被消元行某一元素，考察固定下一个节点的index，依次遍历消元子行的每一个元素，分三种情况
		//如果消元子行的index始终大于被消元行的当前index，异或的结果就是插入对等数目的节点(因为两个"1"中间都是"0",所以碰上"1"异或结果才是1)
		//如果消元子行的index等于当前index，则异或结果为0，需要删除当前节点(pre->next=cur->next)
		//如果消元子行的index小于当前index，证明前一节点和当前节点之间没有可以消元的项，考察再下一个节点即可，若下一个还小，就再下一个
		//需要注意的是，当消元子遍历到末尾时，index为-1，一定小于固定节点的index，那么固定节点会依次往下找，直到找到tail异或结束
		Node* currentNode = head->next;
		Node* _currentNode = _head->next->next;
		while (currentNode->next->index != -1) {
			if(currentNode->next->index < _currentNode->index){
				//小于，比index大的消元子都要作为节点插入current之后，current->next之前
				while (currentNode->next->index < _currentNode->index) {
					Node* node = new Node();
					node->index = _currentNode->index;
					node->next = currentNode->next;
					currentNode->next = node;
					_currentNode = _currentNode->next;
				}

			}
			else if(currentNode ->next->index>_currentNode->index){
				//大于，跳过即可
				currentNode = currentNode->next;
			}
			else {
				//相等，异或为0，故要删除该节点
				Node* deletedNode = currentNode->next;
				currentNode->next = currentNode->next->next;
				_currentNode = _currentNode->next;
				delete deletedNode;
			}
		}
		//记得把第一个节点删掉，因为异或一定会归零
		Node* deletedNode = head->next;
		head->next = head->next->next;
		delete deletedNode;
		return head->next->index;
	}

};
void readFile(List* L1, List* L2) {

	ifstream myfile1(FILE1, ios::in);
	if (!myfile1.is_open()) {
		std::cout << "open file 1 falied" << endl;
	}
	string str;
	int row = 0;
	int index;
	while (getline(myfile1, str)) {
		stringstream ss;
		ss << str;
		if (!ss.eof()) {
			Node* currentNode;
			if (ss >> index) {
				row = index;
				Node* node = new Node;
				node->index = row;
				node->next = L1[row].tail;
				L1[row].head->next = node;
			}
			//current指向插入最新节点，它的下一个就是tail
			currentNode = L1[row].head->next;
			while (ss >> index){
				Node* node = new Node;
				node->index = index;
				node->next = L1[row].tail;
				currentNode->next = node;
				currentNode = node;
			}
		}
	}
	myfile1.close();

	ifstream myfile2(FILE2, ios::in);
	if (!myfile2.is_open()) {
		cout << "open file 2 falied" << endl;
	}
	row = 0;
	while (getline(myfile2, str)) {
		stringstream ss;
		ss << str;
		Node* currentNode;
		if (ss >> index) {
			Node* node = new Node;
			node->index = index;
			node->next = L2[row].tail;
			L2[row].head->next = node;
		}
		//current指向插入最新节点，它的下一个就是tail
		currentNode = L2[row].head->next;
		while (ss >> index) {
			Node* node = new Node;
			node->index = index;
			node->next = L2[row].tail;
			currentNode->next = node;
			currentNode = node;
		}
		row++;
	}
	myfile2.close();

}

void deMap(bool** M, int approach) {
	ofstream myfile;
	switch (approach) {
	case 1:myfile.open(FILEOUT1, ios::in); break;
	case 2:myfile.open(FILEOUT2, ios::in); break;
	default:break;
	}
	if (!myfile.is_open()) {
		cout << "open file out falied" << endl;
	}
	int num = 0;
	for (int i = 0; i < ROW2; i++) {
		for (int j = 0; j < COL; j++) {
			if (M[i][j])
				cout << j << " ";
		}
		cout << endl;
	}
	myfile.close();
}
void Ori(List* L1, List* L2) {
	for (int i = 0; i < ROW2; i++) {
		//遍历被消元行，若首项对应消元子存在，作异或操作，直到为空
		int first = L2[i].getFirstNum();
		while (first != -1) {
			//首项对应消元子为空，升格
			if (L1[first].head->next->index == -1) {
				L1[first] = L2[i];
				break;
			}
			//否则作一次异或，返回首项
			first = L2[i].doXor(L1[first].head);
		}
	}
}
int main() {
	List* L1 = new List[COL];
	List* L2 = new List[ROW2];

	clock_t  clockBegin, clockEnd;
	clockBegin = clock();
	readFile(L1,L2);
	clockEnd = clock();
	float ori_read = clockEnd - clockBegin;
	cout << "ori_read =  " << ori_read << "ms" << endl;
	Ori(L1,L2);
	clockEnd = clock();
	float ori = clockEnd - clockBegin;
	cout << "ori =  " << ori - ori_read << "ms" << endl;
	//deMap(M2, 2);

	return 0;
}
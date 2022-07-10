#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <omp.h>
#include<pmmintrin.h>
#include<xmmintrin.h>
#include<immintrin.h>

//#include <arm_neon.h>
using namespace std;

#define NUM_THREADS 4
#define cnt 1
#define COL 2362
#define ROW1 1226
#define ROW2 453
#define FILE1 "S:/Courses/ParallelProgramming/Data_Groebner/5_2362_1226_453/1.txt"
#define FILE2 "S:/Courses/ParallelProgramming/Data_Groebner/5_2362_1226_453/2.txt"
#define FILE3 "S:/Courses/ParallelProgramming/Data_Groebner/5_2362_1226_453/3.txt"
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
bool** M1;
bool** M2;

void readFile(bool** M1, bool** M2) {

	ifstream myfile1(FILE1, ios::in);
	if (!myfile1.is_open()) {
		cout << "open file 1 falied" << endl;
	}
	string str;
	int row = 0;
	int index;
	while (getline(myfile1, str)) {
		stringstream ss;
		ss << str;
		if (!ss.eof()) {
			if (ss >> index) {
				row = index;
				M1[row][index] = true;
			}
			while (ss >> index)
				M1[row][index] = true;
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
		if (!ss.eof()) {
			while (ss >> index)
				M2[row][index] = true;
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
void ori(bool** M1, bool** M2) {

	for (int i = 0; i < ROW2; i++) {
		for (int j = COL-1; j >=0; j--) {
			//如果有元素就处理
			if (M2[i][j]) {
				//存在消元子
				if(M1[j][j]){
					//第i行被消元行M[i][k]和第j行消元子M[j][k]从j开始依次异或
					for (int k = j; k >= 0; k--) {
						M2[i][k] = M1[j][k] != M2[i][k];
					}
				}
				//不存在消元子，升格为消元子,从M[i][j]到M[i][0]结束,拷贝到M[j][k],M[j][0]
				else {
					for (int k = j; k >= 0; k--) {
						M1[j][k] = M2[i][k];
					}
				}
			}
		}
	}
}
struct paramNode{
	int t_id;
	int j;
}
void doXor(void* param){
	paramNode* p = (paramNode*)param;
	int t_id = p->t_id;
	int j =  p->j;
	for (int k = j - t_id ; k >= 0; k--) {
		M2[i][k] = M1[j][k] != M2[i][k];
	}
}
void pthread(void* param, bool** M1, bool** M2) {
	pthread_t thread_handles[NUM_THREADS];
	paramNode param[NUM_THREADS];
	for (int i = 0; i < ROW2; i++) {
		for (int j = COL-1; j >=0; j--) {
			//如果有元素就处理
			if (M2[i][j]) {
				//存在消元子
				if(M1[j][j]){
					//第i行被消元行M[i][k]和第j行消元子M[j][k]从j开始依次异或
					//循环划分任务
					for(int t_id=0;t_id<NUM_THREADS;++t_id){
						paramNode[t_id].t_id = t_id;
						paramNode[t_id].j = j;
						pthread_create(&thread_handles[t_id], NULL, doXor, (void*)&paramNode[t_id]);
					}
					for(int t_id = 0;t_id<NUM_THREAD;++t_id)
						pthread_join(thread_handles[t_id], NULL);
		
				}
				//不存在消元子，升格为消元子,从M[i][j]到M[i][0]结束,拷贝到M[j][k],M[j][0]
				else {
					for (int k = j; k >= 0; k--) {
						M1[j][k] = M2[i][k];
					}
				}

			}
		}
	}
}
void simd(bitMap* mapG1, bitMap* mapG2) {
	    __m128 t1,t2;
    for (int i = 0; i < ROW2; i++) {
        for (int j = COL-1; j >=0; j--) {
            //如果有元素就处理
            if (M2[i][j]) {
                //存在消元子
                if(M1[j][j]){
                    int k;
                    //第i行被消元行M[i][k]和第j行消元子M[j][k]从j开始依次异或
                    for (k = j-16; k >= 0; k-=16) {
                        t1 = _mm_loadu_ps((const float*)&M1[j][k]);
                        t2 = _mm_loadu_ps((const float*)&M2[i][k]);
                        t1 = _mm_xor_ps(t1, t2);
                        _mm_storeu_ps((float*)&M2[i][k],t1);
                    }
                    for(k = k + 15;k>=0;k--){
                        M2[i][k] = M1[j][k] != M2[i][k];
                    }
                }
                //不存在消元子，升格为消元子,从M[i][j]到M[i][0]结束,拷贝到M[j][k],M[j][0]
                else {
                    for (int k = j; k >= 0; k--) {
                        M1[j][k] = M2[i][k];
                    }
                    //升格完毕，下一行
                    break;
                }
            }
        }
    }
}

void omp(bitMap* mapG1, bitMap* mapG2) {
	    for (int i = 0; i < ROW2; i++) {
        for (int j = COL-1; j >=0; j--) {
            //如果有元素就处理
            if (M2[i][j]) {
                //存在消元子
                if(M1[j][j]){
                    //第i行被消元行M[i][k]和第j行消元子M[j][k]从j开始依次异或
                    #pragma omp parallel for shedule(guided,500) num_threads(NUM_THREADS)
                    for (int k = j; k >= 0; k--) {
                        M2[i][k] = M1[j][k] != M2[i][k];
                    }
                }
                //不存在消元子，升格为消元子,从M[i][j]到M[i][0]结束,拷贝到M[j][k],M[j][0]
                else {
                    for (int k = j; k >= 0; k--) {
                        M1[j][k] = M2[i][k];
                    }
                    //升格完毕，下一行
                    break;
                }
            }
        }
    }
}
int main() {
	M1 = new bool* [COL];
	M2 = new bool* [ROW2];
	for (int i = 0; i < COL; i++) {
		M1[i] = new bool[COL];
		for (int j = 0; j < COL; j++)
			M1[i][j] = false;
	}
	for (int i = 0; i < ROW2; i++) {
		M2[i] = new bool[COL];
		for (int j = 0; j < COL; j++)
			M2[i][j] = false;
	}


	clock_t  clockBegin, clockEnd;
	clockBegin = clock();
	readFile(M1,M2);
	clockEnd = clock();
	float ori_read = clockEnd - clockBegin;
	cout << "ori_read =  " << ori_read << "ms" << endl;
	Ori(M1,M2);
	clockEnd = clock();
	float ori = clockEnd - clockBegin;
	cout << "ori =  " << ori-ori_read << "ms" << endl;
	//deMap(M2, 2);


	for (int i = 0; i < ROW1; i++) {
		delete[] M1[i];
	}
	for (int i = 0; i < ROW2; i++) {
		delete[] M2[i];
	}
	delete[] M1;
	delete[] M2;
	/*mapG1 = new bitMap[COL];
	for (int i = 0; i < COL; i++) {
		mapG1[i].setCol(COL);
		mapG1[i].clear();
	}
	mapG2 = new bitMap[ROW2];
	for (int i = 0; i < ROW2; i++) {
		mapG2[i].setCol(COL);
		mapG2[i].clear();
	}
	mapGO = new bitMap[ROW2];
	for (int i = 0; i < ROW2; i++) {
		mapGO[i].setCol(COL);
		mapGO[i].clear();
	}


	clockBegin = clock();
	for (int i = 0; i < cnt; ++i) {
		mapG1 = new bitMap[COL];
		for (int i = 0; i < COL; i++) {
			mapG1[i].setCol(COL);
			mapG1[i].clear();
		}
		mapG2 = new bitMap[ROW2];
		for (int i = 0; i < ROW2; i++) {
			mapG2[i].setCol(COL);
			mapG2[i].clear();
		}
		mapGO = new bitMap[ROW2];
		for (int i = 0; i < ROW2; i++) {
			mapGO[i].setCol(COL);
			mapGO[i].clear();
		}
		readFile(mapG1, mapG2, mapGO);
		clockEnd = clock();
		float omp_read = clockEnd - clockBegin;
		cout << "omp_read =  " << omp_read << "ms" << endl;
		Omp(mapG1, mapG2);
	}
	clockEnd = clock();
	float omp = clockEnd - clockBegin;
	cout << "ori_omp =  " << omp << "ms" << endl;
	deMap(mapG2, 2);
	cout << "Ratio = " << ori / omp << endl;
	delete[] mapG1;
	delete[] mapG2;
	delete[] mapGO;*/
	return 0;
}
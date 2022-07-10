%%writefile Grobner/grobner_dpcpp_host.cpp
//==============================================================
// Grobner: DPC++ Host
//==============================================================


#include <CL/sycl.hpp>
#include <getopt.h>
#include <ctime>
#include <chrono>
#include <fstream>
#include <sstream>
#include <time.h>
#include <omp.h>
#include<pmmintrin.h>
#include<xmmintrin.h>
#include<immintrin.h>

using namespace std;
using namespace sycl;

#define NUM_THREADS 8
#define COL 3799
#define ROW1 2759
#define ROW2 1953

#define FILE1 "Grobner/6_3799_2759_1953/1.txt"
#define FILE2 "Grobner/6_3799_2759_1953/2.txt"
#define FILE3 "Grobner/6_3799_2759_1953/3.txt"
#define FILEOUT1 "Grobner/output/output1.txt"
#define FILEOUT2 "Grobner/output/output2.txt"
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

//# Define vectors for matricies
std::vector<bool> M1(COL*COL, false);
std::vector<bool> M2(COL*ROW2, false);
    
void readFile(bool* M1,bool* M2) {

    ifstream myfile1(FILE1, ios::in);
    if (!myfile1.is_open()) {
        cout << "open file 1 falied" << '\n';
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
                M1[row*COL+index] = true;
            }
            while (ss >> index)
                M1[row*COL+index] = true;
        }
    }
    myfile1.close();

    ifstream myfile2(FILE2, ios::in);
    if (!myfile2.is_open()) {
        cout << "open file 2 falied" << '\n';
    }
    row = 0;
    while (getline(myfile2, str)) {
        stringstream ss;
        ss << str;
        if (!ss.eof()) {
            while (ss >> index)
                M2[row*COL+index] = true;
        }
        row++;
    }
    myfile2.close();
}
//# matrix multiplication kernel implementation in mm_dpcpp_*.cpp
void usm(bool* M1,bool* M2,queue q) {
    for (int i = 0; i < ROW2; i++) {
        for (int j = COL-1; j >=0; j--) {
            //如果有元素就处理
            if (M2[i*COL+j]) {
                //存在消元子
                if(M1[j*COL+j]){
                    //第i行被消元行M[i][k]和第j行消元子M[j][k]从j开始依次异或
                     q.parallel_for(range{(unsigned long)(j+1)}, [=](id<1> idx){ 
                         int k = idx[0] + j; // 等同于for(int k = j ; k>=0; k--)
                         M2[i*COL+k] = M1[j*COL+k] != M2[i*COL+k];
                     }); 
                }
                //不存在消元子，升格为消元子,从M[i][j]到M[i][0]结束,拷贝到M[j][k],M[j][0]
                else {
                    for (int k = j; k >= 0; k--) {
                        M1[j*COL+k] = M2[i*COL+k];
                    }
                    //升格完毕，下一行
                    break;
                }
            }
        }
    }
}
int main(int argc, char *argv[]) {
    gpu_selector selector;
    queue q(selector);
    bool* M1 = (bool *)malloc_shared(COL*COL* sizeof(bool), q);
    bool* M2 = (bool *)malloc_shared(COL*ROW2* sizeof(bool), q);
    //# get start time
    auto start = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    readFile(M1,M2);
    auto duration_read = std::chrono::high_resolution_clock::now().time_since_epoch().count() - start;
    std::cout << "Read Duration      : " << duration_read / 1e+9 << " seconds\n";
    usm(M1,M2,q);
    //# Call matrix multiplication kernel implementation
    auto duration_ori = std::chrono::high_resolution_clock::now().time_since_epoch().count() - start;
    std::cout << "Compute Duration      : " << duration_ori / 1e+9 << " seconds\n";
    deMap(M2,1);
}

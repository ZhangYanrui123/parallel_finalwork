#include <stdio.h>
#include <ifstream>
using namespace std;
//只进行了第6个文件的CUDA实验
#define FILE1 "1.txt"
#define FILE2 "2.txt"
#define FILEOUT "out.txt"

#define COL 3799
#define ROW1 2759
#define ROW2 1953
void init(bool* M1,bool* M2){
    for(int i = 0;i<COL*COL;i++){
        M1[i] = false;
    }
    for(int i = 0;i<COL*ROW2;i++){
        M2[i] = false;
    }
}

__global__ void xor_kernel(bool* M1,bool* M2,int j){
  int index = threadIdx.x + blockIdx.x * blockDim.x;
  int stride = blockDim.x * gridDim.x;

  for(int i = index; i < j; i += stride)
  {
    M2[i]^=M1[i]; 
  }
}
void read(bool* M1, bool* M2){
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
        M1[row*COL+index] = true;
      }
      while (ss >> index)
      M1[row*COL+index] = true;
      }
  }
  myfile1.close();

  row = 0;
  ifstream myfile2(FILE2, ios::in);
  if (!myfile2.is_open()) {
    cout << "open file 2 falied" << endl;
  }
  while (getline(myfile2, str)) {
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
    row++;
  }
  myfile2.close();
}

int main()
{
  //计时器
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  int deviceId;
  int numberOfSMs;

  cudaGetDevice(&deviceId);
  cudaDeviceGetAttribute(&numberOfSMs, cudaDevAttrMultiProcessorCount, deviceId);

  size_t size1 = COL *COL* sizeof(bool);
  size_t size2 = COL *ROW2* sizeof(bool);

  bool* M1;
  bool* M2;

  cudaMallocManaged(&M1, size1);
  cudaMallocManaged(&M2, size2);

  init(M1,M2);
  read(M1,M2);
  size_t threadsPerBlock;
  size_t numberOfBlocks;

  threadsPerBlock = 256;
  numberOfBlocks = 32 * numberOfSMs;

  cudaEventRecord(start,0);

  for (int i = 0; i < ROW2; i++) {
      for (int j = COL-1; j >=0; j--) {
          //如果有元素就处理
          if (M2[i][j]) {
          //存在消元子
              if(M1[j][j]){
              //第i行被消元行M[i][k]和第j行消元子M[j][k]从j开始依次异或
                  xor_kernel<<<numberOfBlocks,threadsPerBlock>>>(&M1[j],&M2[i],j);
                  //需要cpu停止，等待gpu任务完成
                  cudaDeviceSynchronize();
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
  cudaEventRecord(stop,0);
  //统计用时
  float elapsedTime;
  cudaEventElapsedTime(&elapsedTime, start, stop);
  cout<<"blocks = "<<numberOfBlocks<<",threads = "<<threadsPerBlock<<","<<elapsedTime<<" ms"<<"\n";
  cudaFree(M1);
  cudaFree(M2);
}

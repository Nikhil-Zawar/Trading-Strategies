#include<iostream>
#include<cstdlib>
using namespace std;

void strategize(){
    cout<<"the strategy here"<<endl;
}


int main(){
    const char* file_command = "python3 basic_strat_files.py SBIN 20 01/02/2023 21/02/2023";
    int files_generated = system(file_command);
    if(file_command == 0){
        cout<<"Failed : file generation did not happen from the python file";
    }else{
        strategize();
    }
    return 0;
}

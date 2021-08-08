#include <iostream>
#include <fstream>
#include <string>


class CountWord{
private:
    static const size_t MAX_BUFF_SIZE = 100000;
    std::string filepath, word, buf_end_word;
    int num;
    bool file_read_err;

    char* file_read(std::ifstream& file);
    void count_buf(char *);
    void count_between_two_buf(std::string next_buf_begain_word);

public:
    CountWord(std::string f, std::string s)
        :filepath{f}, word{s}, num{0}, file_read_err{false} {};
    int count();
};


char* CountWord::file_read(std::ifstream& file) {
    char* p  = new char[MAX_BUFF_SIZE];

    if (!file.readsome(p, MAX_BUFF_SIZE)){
        file_read_err = true;
        return nullptr;
    }

    file_read_err = false;
    return p;
}


int CountWord::count() {

    std::ifstream is (filepath, std::ifstream::binary);
    if (!is){
        std::cerr<< "file open error, filepath:" << filepath << std::endl;
        return num;
    }

    while (!file_read_err){
        char* buf = file_read(is);
        count_buf(buf);
        delete buf;
    }
    is.close();
    return num;
}

void CountWord::count_buf(char* buf) {
    if (nullptr == buf){
        return;
    }

    std::string buffer(buf);
    size_t pos = buffer.find(word, 0);
    while (std::string::npos != pos){
        num ++;
        pos = buffer.find(word, pos+1);
    }
    // There may be a word between the two buffers
    auto buf_begin_word = buffer.substr(0,word.length()-1);
    count_between_two_buf(buf_begin_word);
    buf_end_word = buffer.substr(buffer.size()-word.length()-1);
}

void CountWord::count_between_two_buf(std::string next_buf_begain_word) {
    std::string middle_part_buf = buf_end_word + next_buf_begain_word;
    size_t pos = next_buf_begain_word.find(word, 0);
    if (std::string::npos != pos){
        num ++;
    }
}

int main(){
    CountWord my{"data_131the.txt", "the"};
    std::cout << "count: " << my.count();
}

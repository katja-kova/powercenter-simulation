#include "fileinterface.h"

FileInterface::FileInterface(const std::string &_FPath, bool append) {
    this->filePATH = _FPath;
    this->append_Mode = append;
    if(_FPath != "") {
        std::ifstream tst(this->filePATH);
        if(!tst) {
            throw std::invalid_argument("cannot open file: " + _FPath + '\n');
        }
    }
}

void FileInterface::setFP(const std::string &_FPath) {
    this->filePATH = _FPath;
    std::ifstream tst(this->filePATH);
    if(!tst) {
        throw std::invalid_argument("cannot open file: " + _FPath + '\n');
    }
}

std::string FileInterface::fread() {
    std::string FileContent = "", line = "";
    std::ifstream file(this->filePATH.c_str());
    if(!file) {
        throw std::invalid_argument("cannot read file: " + this->filePATH + '\n');
    }
    while(std::getline(file,line)) {
        FileContent += line + '\n';
    }
    if(file.is_open()) {
        file.close();
    }
    return FileContent;
}

std::vector<std::string> FileInterface::flines(const char seperator) {
    std::vector<std::string> FileContent;
    std::string line = "";
    std::ifstream file(this->filePATH.c_str());
    if(!file) {
        throw std::invalid_argument("cannot read file: " + this->filePATH + '\n');
    }
    while(std::getline(file,line, seperator)) {
        FileContent.push_back(line);
    }
    if(file.is_open()) {
        file.close();
    }
    return FileContent;
}

void FileInterface::fwrite(std::string& _FContent) {
    if(!append_Mode) {
        clear();
    }
    std::ofstream file(this->filePATH.c_str(), std::ios::app);
    if(!file) {
        throw std::invalid_argument("cannot write file: " + this->filePATH + '\n');
    }
    file << _FContent;
}

void FileInterface::operator <<(std::string& _FContent) {
    this->fwrite(_FContent);
}

void FileInterface::clear() {
    std::ofstream file(this->filePATH.c_str(), std::ios::trunc);
    if(!file) {
        throw std::invalid_argument("cannot clear file: " + this->filePATH + '\n');
    }
}

void FileInterface::setAppend_Mode(bool value) {
    append_Mode = value;
}


#ifndef FILEINTERFACE_H
#define FILEINTERFACE_H

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

class FileInterface {
public:
    FileInterface(const std::string &_FPath = "", bool append = false);
    //FILE_INTERFACE
    void setFP(const std::string &_FPath);
    void setAppend_Mode(bool value);
    std::string fread();
    std::vector<std::string> flines(const char seperator = '\n');
    void fwrite(std::string& _FContent);
    void operator <<(std::string& _FContent);

    void clear();
private:
    std::string filePATH;
    bool append_Mode;
};

#endif // FILEINTERFACE_H


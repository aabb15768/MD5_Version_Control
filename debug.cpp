#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <algorithm>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <sstream>
#include <string>
#include <ctime>
#include "md5.h"
using namespace std;
#define COMMIT_COUNT 0
#define FILE_COUNT 1
#define ADD_COUNT 2
#define MODIFY_COUNT 3
#define COPY_COUNT 4
#define DELETE_COUNT 5
#define COMMIT_SIZE 6
typedef unsigned char       byte;


int getNumber(unsigned char byte, int position) // position in range 0-7
{
  int output = 0;
  if(position == 0){
    int one = byte >> 0 & 0x1;
    int two = byte >> 1 & 0x1;
    int three = byte >> 2 & 0x1;
    int four = byte >> 3 & 0x1;
    output = one + (two << 1) + (three << 2) + (four << 3);
  } else {
    int one = byte >> 4 & 0x1;
    int two = byte >> 5 & 0x1;
    int three = byte >> 6 & 0x1;
    int four = byte >> 7 & 0x1;
    output = one + (two << 1) + (three << 2) + (four << 3);
  }
  return output;
}

class File {
public:
  char name[256];
  string md5;
  char copiedFrom[256];
  bool isDelete = true;
};

bool compareName(const File &a,const File &b) {
    return (strcmp(a.name, b.name) < 0);
}

bool compareMd5(const File &a,const File &b) {
    return (a.md5 < b.md5);
}

bool compareCopiedFrom(const File &a,const File &b) {
    return (strcmp(a.copiedFrom, b.copiedFrom) < 0);
}


int main(int argc, char* argv[]) {
  ////////////////////////////DEBUG////////////////////////////////////
  ifstream is ("test_1/.loser_record", ifstream::binary);
  while(true) {
    uint32_t information[7];
    if(!is.read(reinterpret_cast<char *>(&information[COMMIT_COUNT]), sizeof(information[COMMIT_COUNT]))) {
      break;
    }
    cout << "commit " << information[COMMIT_COUNT] << "\n";
    for(int i = 1; i < 7; i++) {
      is.read(reinterpret_cast<char *>(&information[i]), sizeof(information[i]));
    }
    cout << "fileCount " << information[FILE_COUNT] << "\n";
    cout << "addCount " << information[ADD_COUNT] << "\n";
    cout << "modifyCount " << information[MODIFY_COUNT] << "\n";
    cout << "copyCount " << information[COPY_COUNT] << "\n";
    cout << "deleteCount " << information[DELETE_COUNT] << "\n";

    cout << "fileSize " << information[COMMIT_SIZE] << "\n";

    uint8_t sizeOfFile;
    cout << "[new_file]" << "\n";
    for(int i = 0; i < information[ADD_COUNT]; i++) {
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      char* fileName = new char[sizeOfFile+1];
      is.read(fileName, sizeOfFile);
      cout << fileName << "\n";
      delete [] fileName;
    }

    cout << "[modified]" << "\n";
    for(int i = 0; i < information[MODIFY_COUNT]; i++) {
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      char* fileName = new char[sizeOfFile+1];
      is.read(fileName, sizeOfFile);
      cout << fileName << "\n";
      delete [] fileName;
    }

    cout << "[deleted]" << "\n";
    for(int i = 0; i < information[DELETE_COUNT]; i++) {
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      char* fileName = new char[sizeOfFile+1];
      is.read(fileName, sizeOfFile);
      cout << fileName << "\n";
      delete [] fileName;
    }

    cout << "[copied_from]" << "\n";
    for(int i = 0; i < information[COPY_COUNT]; i++) {
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      char* fileName = new char[sizeOfFile+1];
      is.read(fileName, sizeOfFile);
      cout << fileName << "\n";
      delete [] fileName;
    }

    cout << "[copied_to]" << "\n";
    for(int i = 0; i < information[COPY_COUNT]; i++) {
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      char* fileName = new char[sizeOfFile+1];
      is.read(fileName, sizeOfFile);
      cout << fileName << "\n";
      delete [] fileName;
    }

    cout << "[deleted]" << "\n";
    for(int i = 0; i < information[DELETE_COUNT]; i++) {
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      char* fileName = new char[sizeOfFile+1];
      is.read(fileName, sizeOfFile);
      cout << fileName << "\n";
      delete [] fileName;
    }

    cout << "(MD5)" << "\n";
    for(int i = 0; i < information[FILE_COUNT]; i++) {
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      char* fileName = new char[sizeOfFile+1];
      is.read(fileName, sizeOfFile);
      cout << fileName << "\n";
      delete [] fileName;
      unsigned char temp[16];
      for(int j = 0; j < 16; j++) {
        is.read(reinterpret_cast<char *>(&temp[j]), 1);
        cout << hex << getNumber(temp[j], 1) << getNumber(temp[j], 0);
      }
      cout << "\n";
    }
    cout << dec;
  }
  return 0;

  if(strcmp(argv[1], "commit") == 0) {
    //////////////////////////////STATUS_INIT///////////////////////////////
    char pathOfRecord[550];
    strcpy(pathOfRecord, argv[2]);
    strcat(pathOfRecord, "/.loser_record");
    unsigned long long int commitNumber = 0;
    unsigned long long int accumuSize = 0;
    unsigned long long int totalSize;
    ifstream is (pathOfRecord, ifstream::binary);
    // fail
    if(is.fail()) {
      vector<File> fileSet;
      DIR *dir;
      struct dirent *ent;
      if ((dir = opendir (argv[2])) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
          if(strcmp(ent->d_name, ".") == 0) {
            continue;
          }
          if(strcmp(ent->d_name, "..") == 0) {
            continue;
          }
          if(strcmp(ent->d_name, ".DS_Store") == 0) {
            continue;
          }
          if(strcmp(ent->d_name, ".loser_record") == 0) {
            continue;
          }
          File file;
          strcpy(file.name, ent->d_name);
          char str[550];
          strcpy(str, argv[2]);
          strcat(str, "/");
          strcat(str, ent->d_name);
          ifstream is (str, ifstream::binary);
          is.seekg(0, is.end);
          int fileSize = is.tellg();
          string buffer(fileSize, '\0');
          is.seekg(0);
          is.read(&buffer[0], fileSize);
          MD5 md5(buffer);
          file.md5 = md5.md5();
          fileSet.push_back(file);
          //
          is.close();
        }
        closedir (dir);
      }

      sort(fileSet.begin(), fileSet.end(), compareMd5);
      stable_sort(fileSet.begin(), fileSet.end(), compareName);

      char fileToCreate[550];
      strcpy(fileToCreate, argv[2]);
      strcat(fileToCreate, "/");
      strcat(fileToCreate, ".loser_record");
      ofstream isWrite (fileToCreate, ofstream::binary);

      uint32_t commitNumber = 1;
      isWrite.write(reinterpret_cast<char *>(&commitNumber), sizeof(commitNumber));
      uint32_t fileNumber = fileSet.size();
      isWrite.write(reinterpret_cast<char *>(&fileNumber), sizeof(fileNumber));
      uint32_t addNumber = fileSet.size();
      isWrite.write(reinterpret_cast<char *>(&addNumber), sizeof(addNumber));
      uint32_t modifyNumber = 0;
      isWrite.write(reinterpret_cast<char *>(&modifyNumber), sizeof(modifyNumber));
      uint32_t copyNumber = 0;
      isWrite.write(reinterpret_cast<char *>(&copyNumber), sizeof(copyNumber));
      uint32_t deleteNumber = 0;
      isWrite.write(reinterpret_cast<char *>(&deleteNumber), sizeof(deleteNumber));
      uint32_t commitSize = 28;
      for(int i = 0; i < fileSet.size(); i++) {
        commitSize += 1;
        commitSize += strlen(fileSet[i].name);
      }
      for(int i = 0; i < fileSet.size(); i++) {
        commitSize += 1;
        commitSize += strlen(fileSet[i].name);
        commitSize += 16;
      }
      isWrite.write(reinterpret_cast<char *>(&commitSize), sizeof(commitSize));
      for(int i = 0; i < fileSet.size(); i++) {
        uint8_t sizeOfFileName = static_cast<uint8_t>(strlen(fileSet[i].name));
        isWrite.write(reinterpret_cast<char *>(&sizeOfFileName), sizeof(sizeOfFileName));
        isWrite.write(fileSet[i].name, sizeOfFileName);
      }
      for(int i = 0; i < fileSet.size(); i++) {
        uint8_t sizeOfFileName = static_cast<uint8_t>(strlen(fileSet[i].name));
        isWrite.write(reinterpret_cast<char *>(&sizeOfFileName), sizeof(sizeOfFileName));
        isWrite.write(fileSet[i].name, sizeOfFileName);
        for(int j = 0; j < 16; j++) {
          string output = fileSet[i].md5.substr(j*2, 2);
          uint8_t number = static_cast<uint8_t>(stoul(output, NULL, 16));
          isWrite.write(reinterpret_cast<char *>(&number), sizeof(number));
        }
      }
      isWrite.close();
      return 0;
    }
    // not fail
    is.seekg(0, is.end);
    totalSize = is.tellg();
    uint32_t currentSize = 0;
    unsigned long long int tempSize = 0;
    unsigned long long int targetSize = 0;
    for(int i = 24; i < totalSize; i+= currentSize) {
      commitNumber += 1;
      is.seekg(i);
      is.read(reinterpret_cast<char *>(&currentSize), sizeof(currentSize));
      tempSize = accumuSize + currentSize;
      if(tempSize >= totalSize) {
        targetSize = accumuSize;
      }
      accumuSize = tempSize;
    }
    is.seekg(targetSize);
    uint32_t information[7];
    for(int i = 0; i < 7; i++) {
      is.read(reinterpret_cast<char *>(&information[i]), sizeof(information[i]));
      targetSize += 4;
    }
    // ADD
    for(int i = 0; i < information[ADD_COUNT]; i++) {
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);
    }
    // MODIFY
    for(int i = 0; i < information[MODIFY_COUNT]; i++) {
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);
    }
    // COPY
    for(int i = 0; i < information[COPY_COUNT]; i++) {
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);

      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);
    }
    // DELETE
    for(int i = 0; i < information[DELETE_COUNT]; i++) {
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);
    }
    // MD5
    vector<File> lastCommit;
    for(int i = 0; i < information[FILE_COUNT]; i++) {
      File file;
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      char* fileName = new char[static_cast<int>(sizeOfFile)+1];
      memset(fileName, 0, static_cast<int>(sizeOfFile)+1);
      is.read(fileName, static_cast<int>(sizeOfFile));
      strcpy(file.name, fileName);
      delete [] fileName;
      unsigned char temp[16];
      stringstream stream;
      for(int j = 0; j < 16; j++) {
        is.read(reinterpret_cast<char *>(&temp[j]), 1);
        stream << hex << getNumber(temp[j], 1) << getNumber(temp[j], 0);
      }
      file.md5 = stream.str();
      stream << dec;
      stream.str("");
      lastCommit.push_back(file);
    }
    sort(lastCommit.begin(), lastCommit.end(), compareName);
    vector<File> lastCommitByMd5(lastCommit);
    stable_sort(lastCommitByMd5.begin(), lastCommitByMd5.end(), compareMd5);
    // for(int i = 0; i < lastCommitByMd5.size(); i++) {
    //   pair<vector<File>::iterator, vector<File>::iterator> bounds;
    //   bounds = equal_range(lastCommitByMd5.begin()+i, lastCommitByMd5.end(), lastCommitByMd5[i], compareMd5);
    //   sort(bounds.first, bounds.second, compareName);
    //   i = bounds.second - lastCommitByMd5.begin();
    // }
    is.close();
    //////////////////////////////STATUS_DIRECT///////////////////////////////
    vector<File> fileNew;
    vector<File> fileModify;
    vector<File> fileCopied;
    vector<File> fileAll;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (argv[2])) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        if(strcmp(ent->d_name, ".") == 0) {
          continue;
        }
        if(strcmp(ent->d_name, "..") == 0) {
          continue;
        }
        if(strcmp(ent->d_name, ".DS_Store") == 0) {
          continue;
        }
        if(strcmp(ent->d_name, ".loser_record") == 0) {
          continue;
        }
        File file;
        strcpy(file.name, ent->d_name);
        char str[550];
        strcpy(str, argv[2]);
        strcat(str, "/");
        strcat(str, ent->d_name);
        ifstream is (str, ifstream::binary);
        is.seekg(0, is.end);
        int fileSize = is.tellg();
        string buffer(fileSize, '\0');
        is.seekg(0);
        is.read(&buffer[0], fileSize);
        MD5 md5(buffer);
        file.md5 = md5.md5();
        //
        bool isNameSame = false;
        bool isMd5Same = false;
        pair<vector<File>::iterator, vector<File>::iterator> bounds;
        bounds = equal_range(lastCommit.begin(), lastCommit.end(), file, compareName);
        int indexName;
        if(bounds.first == bounds.second) {
          isNameSame = false;
        } else {
          isNameSame = true;
          indexName = bounds.first - lastCommit.begin();
          lastCommit[indexName].isDelete = false;
        }

        bounds = equal_range(lastCommitByMd5.begin(), lastCommitByMd5.end(), file, compareMd5);
        int indexMd5;
        if(bounds.first == bounds.second) {
          isMd5Same = false;
        } else {
          isMd5Same = true;
          indexMd5 = bounds.first - lastCommitByMd5.begin();
        }

        if(isNameSame) {
          if(!isMd5Same) {
            fileModify.push_back(file);
          }
        } else {
          if(isMd5Same) {
            strcpy(file.copiedFrom, lastCommitByMd5[indexMd5].name);
            fileCopied.push_back(file);
          } else {
            fileNew.push_back(file);
          }
        }
        fileAll.push_back(file);
        //
        is.close();
      }
      closedir (dir);
    }

    sort(fileNew.begin(), fileNew.end(), compareName);
    sort(fileModify.begin(), fileModify.end(), compareName);
    sort(fileCopied.begin(), fileCopied.end(), compareCopiedFrom);
    stable_sort(fileCopied.begin(), fileCopied.end(), compareName);
    sort(fileAll.begin(), fileAll.end(), compareMd5);
    stable_sort(fileAll.begin(), fileAll.end(), compareName);
    // final

    char fileToOpen[550];
    strcpy(fileToOpen, argv[2]);
    strcat(fileToOpen, "/");
    strcat(fileToOpen, ".loser_record");
    ofstream isWrite (fileToOpen, ofstream::binary | ofstream::app);
    // debug
    commitNumber = information[COMMIT_COUNT] + 1;
    isWrite.write(reinterpret_cast<char *>(&commitNumber), sizeof(commitNumber));
    uint32_t fileNumber = fileAll.size();
    isWrite.write(reinterpret_cast<char *>(&fileNumber), sizeof(fileNumber));
    uint32_t addNumber = fileNew.size();
    isWrite.write(reinterpret_cast<char *>(&addNumber), sizeof(addNumber));
    uint32_t modifyNumber = fileModify.size();
    isWrite.write(reinterpret_cast<char *>(&modifyNumber), sizeof(modifyNumber));
    uint32_t copyNumber = fileCopied.size();
    isWrite.write(reinterpret_cast<char *>(&copyNumber), sizeof(copyNumber));
    vector<File> fileDelete;
    for(int i = 0; i < lastCommit.size(); i++) {
      if(lastCommit[i].isDelete) {
        fileDelete.push_back(lastCommit[i]);
      }
    }
    uint32_t deleteNumber = fileDelete.size();
    isWrite.write(reinterpret_cast<char *>(&deleteNumber), sizeof(deleteNumber));
    uint32_t commitSize = 28;
    for(int i = 0; i < fileNew.size(); i++) {
      commitSize += 1;
      commitSize += strlen(fileNew[i].name);
    }
    for(int i = 0; i < fileModify.size(); i++) {
      commitSize += 1;
      commitSize += strlen(fileModify[i].name);
    }
    for(int i = 0; i < fileCopied.size(); i++) {
      commitSize += 1;
      commitSize += strlen(fileCopied[i].copiedFrom);
    }
    for(int i = 0; i < fileCopied.size(); i++) {
      commitSize += 1;
      commitSize += strlen(fileCopied[i].name);
    }
    for(int i = 0; i < fileDelete.size(); i++) {
      commitSize += 1;
      commitSize += strlen(fileDelete[i].name);
    }


    for(int i = 0; i < fileAll.size(); i++) {
      commitSize += 1;
      commitSize += strlen(fileAll[i].name);
      commitSize += 16;
    }
    isWrite.write(reinterpret_cast<char *>(&commitSize), sizeof(commitSize));

    for(int i = 0; i < fileNew.size(); i++) {
      uint8_t sizeOfFileName = static_cast<uint8_t>(strlen(fileNew[i].name));
      isWrite.write(reinterpret_cast<char *>(&sizeOfFileName), sizeof(sizeOfFileName));
      isWrite.write(fileNew[i].name, sizeOfFileName);
    }
    for(int i = 0; i < fileModify.size(); i++) {
      uint8_t sizeOfFileName = static_cast<uint8_t>(strlen(fileModify[i].name));
      isWrite.write(reinterpret_cast<char *>(&sizeOfFileName), sizeof(sizeOfFileName));
      isWrite.write(fileModify[i].name, sizeOfFileName);
    }
    for(int i = 0; i < fileCopied.size(); i++) {
      uint8_t sizeOfFileName = static_cast<uint8_t>(strlen(fileCopied[i].copiedFrom));
      isWrite.write(reinterpret_cast<char *>(&sizeOfFileName), sizeof(sizeOfFileName));
      isWrite.write(fileCopied[i].copiedFrom, sizeOfFileName);
      sizeOfFileName = static_cast<uint8_t>(strlen(fileCopied[i].name));
      isWrite.write(reinterpret_cast<char *>(&sizeOfFileName), sizeof(sizeOfFileName));
      isWrite.write(fileCopied[i].name, sizeOfFileName);
    }
    for(int i = 0; i < fileDelete.size(); i++) {
      uint8_t sizeOfFileName = static_cast<uint8_t>(strlen(fileDelete[i].name));
      isWrite.write(reinterpret_cast<char *>(&sizeOfFileName), sizeof(sizeOfFileName));
      isWrite.write(fileDelete[i].name, sizeOfFileName);
    }

    for(int i = 0; i < fileAll.size(); i++) {
      uint8_t sizeOfFileName = static_cast<uint8_t>(strlen(fileAll[i].name));
      isWrite.write(reinterpret_cast<char *>(&sizeOfFileName), sizeof(sizeOfFileName));
      isWrite.write(fileAll[i].name, sizeOfFileName);
      for(int j = 0; j < 16; j++) {
        string output = fileAll[i].md5.substr(j*2, 2);
        uint8_t number = static_cast<uint8_t>(stoul(output, NULL, 16));
        isWrite.write(reinterpret_cast<char *>(&number), sizeof(number));
      }
    }
    isWrite.close();
    return 0;
  }
  if(strcmp(argv[1], "status") == 0) {
    //////////////////////////////STATUS_INIT///////////////////////////////
    char pathOfRecord[550];
    strcpy(pathOfRecord, argv[2]);
    strcat(pathOfRecord, "/.loser_record");
    unsigned long long int commitNumber = 0;
    unsigned long long int accumuSize = 0;
    unsigned long long int totalSize;
    ifstream is (pathOfRecord, ifstream::binary);
    if(is.fail()) {
      vector<File> fileSet;
      DIR *dir;
      struct dirent *ent;
      if ((dir = opendir (argv[2])) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
          if(strcmp(ent->d_name, ".") == 0) {
            continue;
          }
          if(strcmp(ent->d_name, "..") == 0) {
            continue;
          }
          if(strcmp(ent->d_name, ".DS_Store") == 0) {
            continue;
          }
          if(strcmp(ent->d_name, ".loser_record") == 0) {
            continue;
          }
          File file;
          strcpy(file.name, ent->d_name);
          char str[550];
          strcpy(str, argv[2]);
          strcat(str, "/");
          strcat(str, ent->d_name);
          ifstream is (str, ifstream::binary);
          is.seekg(0, is.end);
          int fileSize = is.tellg();
          string buffer(fileSize, '\0');
          is.seekg(0);
          is.read(&buffer[0], fileSize);
          MD5 md5(buffer);
          file.md5 = md5.md5();
          fileSet.push_back(file);
          //
          is.close();
        }
        closedir (dir);
      }

      sort(fileSet.begin(), fileSet.end(), compareMd5);
      stable_sort(fileSet.begin(), fileSet.end(), compareName);
      printf("[new_file]\n");
      for(int i = 0; i < fileSet.size(); i++) {
        printf("%s\n", fileSet[i]. name);
      }
      printf("[modified]\n[copied]\n[deleted]\n");
      return 0;
    }

    is.seekg(0, is.end);
    totalSize = is.tellg();
    uint32_t currentSize = 0;
    unsigned long long int tempSize = 0;
    unsigned long long int targetSize = 0;
    for(int i = 24; i < totalSize; i+= currentSize) {
      commitNumber += 1;
      is.seekg(i);
      is.read(reinterpret_cast<char *>(&currentSize), sizeof(currentSize));
      tempSize = accumuSize + currentSize;
      if(tempSize >= totalSize) {
        targetSize = accumuSize;
      }
      accumuSize = tempSize;
    }
    is.seekg(targetSize);
    uint32_t information[7];
    for(int i = 0; i < 7; i++) {
      is.read(reinterpret_cast<char *>(&information[i]), sizeof(information[i]));
      targetSize += 4;
    }
    // ADD
    for(int i = 0; i < information[ADD_COUNT]; i++) {
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);
    }
    // MODIFY
    for(int i = 0; i < information[MODIFY_COUNT]; i++) {
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);
    }
    // COPY
    for(int i = 0; i < information[COPY_COUNT]; i++) {
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);

      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);
    }
    // DELETE
    for(int i = 0; i < information[DELETE_COUNT]; i++) {
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      targetSize = targetSize + 1 + static_cast<int>(sizeOfFile);
      is.seekg(targetSize);
    }
    // MD5
    vector<File> lastCommit;
    for(int i = 0; i < information[FILE_COUNT]; i++) {
      File file;
      uint8_t sizeOfFile;
      is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
      char* fileName = new char[static_cast<int>(sizeOfFile)+1];
      memset(fileName, 0, static_cast<int>(sizeOfFile)+1);
      is.read(fileName, static_cast<int>(sizeOfFile));
      strcpy(file.name, fileName);
      delete [] fileName;
      unsigned char temp[16];
      stringstream stream;
      for(int j = 0; j < 16; j++) {
        is.read(reinterpret_cast<char *>(&temp[j]), 1);
        stream << hex << getNumber(temp[j], 1) << getNumber(temp[j], 0);
      }
      file.md5 = stream.str();
      stream << dec;
      stream.str("");
      lastCommit.push_back(file);
    }
    sort(lastCommit.begin(), lastCommit.end(), compareName);
    vector<File> lastCommitByMd5(lastCommit);
    stable_sort(lastCommitByMd5.begin(), lastCommitByMd5.end(), compareMd5);
    // for(int i = 0; i < lastCommitByMd5.size(); i++) {
    //   pair<vector<File>::iterator, vector<File>::iterator> bounds;
    //   bounds = equal_range(lastCommitByMd5.begin()+i, lastCommitByMd5.end(), lastCommitByMd5[i], compareMd5);
    //   sort(bounds.first, bounds.second, compareName);
    //   i = bounds.second - lastCommitByMd5.begin();
    // }
    is.close();
    //////////////////////////////STATUS_DIRECT///////////////////////////////
    vector<File> fileNew;
    vector<File> fileModify;
    vector<File> fileCopied;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (argv[2])) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        if(strcmp(ent->d_name, ".") == 0) {
          continue;
        }
        if(strcmp(ent->d_name, "..") == 0) {
          continue;
        }
        if(strcmp(ent->d_name, ".DS_Store") == 0) {
          continue;
        }
        if(strcmp(ent->d_name, ".loser_record") == 0) {
          continue;
        }
        File file;
        strcpy(file.name, ent->d_name);
        char str[550];
        strcpy(str, argv[2]);
        strcat(str, "/");
        strcat(str, ent->d_name);
        ifstream is (str, ifstream::binary);
        is.seekg(0, is.end);
        int fileSize = is.tellg();
        string buffer(fileSize, '\0');
        is.seekg(0);
        is.read(&buffer[0], fileSize);
        MD5 md5(buffer);
        file.md5 = md5.md5();
        //
        bool isNameSame = false;
        bool isMd5Same = false;
        pair<vector<File>::iterator, vector<File>::iterator> bounds;
        bounds = equal_range(lastCommit.begin(), lastCommit.end(), file, compareName);
        int indexName;
        if(bounds.first == bounds.second) {
          isNameSame = false;
        } else {
          isNameSame = true;
          indexName = bounds.first - lastCommit.begin();
          lastCommit[indexName].isDelete = false;
        }

        bounds = equal_range(lastCommitByMd5.begin(), lastCommitByMd5.end(), file, compareMd5);
        int indexMd5;
        if(bounds.first == bounds.second) {
          isMd5Same = false;
        } else {
          isMd5Same = true;
          indexMd5 = bounds.first - lastCommitByMd5.begin();
        }

        if(isNameSame) {
          if(!isMd5Same) {
            fileModify.push_back(file);
          }
        } else {
          if(isMd5Same) {
            strcpy(file.copiedFrom, lastCommitByMd5[indexMd5].name);
            fileCopied.push_back(file);
          } else {
            fileNew.push_back(file);
          }
        }
        //
        is.close();
      }
      closedir (dir);
    }

    sort(fileNew.begin(), fileNew.end(), compareName);
    sort(fileModify.begin(), fileModify.end(), compareName);
    sort(fileCopied.begin(), fileCopied.end(), compareCopiedFrom);
    stable_sort(fileCopied.begin(), fileCopied.end(), compareName);
    printf("[new_file]\n");
    for(int i = 0; i < fileNew.size(); i++) {
      printf("%s\n", fileNew[i].name);
    }
    printf("[modified]\n");
    for(int i = 0; i < fileModify.size(); i++) {
      printf("%s\n", fileModify[i].name);
    }
    printf("[copied]\n");
    for(int i = 0; i < fileCopied.size(); i++) {
      printf("%s => %s\n", fileCopied[i].copiedFrom, fileCopied[i].name);
    }
    printf("[deleted]\n");
    for(int i = 0; i < lastCommit.size(); i++) {
      if(lastCommit[i].isDelete) {
        printf("%s\n", lastCommit[i].name);
      }
    }
    return 0;
  } else if (strcmp(argv[1], "log") == 0) {
    ////////////////////////////LOG///////////////////////////////
    char pathOfRecord[550];
    memset(pathOfRecord,0,sizeof(pathOfRecord));
    strcpy(pathOfRecord, argv[3]);
    strcat(pathOfRecord, "/.loser_record");
    long long int logNum = atoll(argv[2]);
    unsigned long long int commitNumber = 0;
    unsigned long long int accumuSize = 0;
    vector<unsigned long long int> sizeRecord;
    sizeRecord.push_back(0);
    unsigned long long int totalSize;
    ifstream is (pathOfRecord, ifstream::binary);
    if(is.fail()) {
      return 0;
    }
    is.seekg(0, is.end);
    totalSize = is.tellg();
    uint32_t currentSize = 0;
    for(int i = 24; i < totalSize; i+= currentSize) {
      commitNumber += 1;
      is.seekg(i);
      is.read(reinterpret_cast<char *>(&currentSize), sizeof(currentSize));
      accumuSize += currentSize;
      sizeRecord.push_back(accumuSize);
    }
    // deubg 1
    // for(int i = 0; i < sizeRecord.size(); i++) {
    //   cout << sizeRecord[i] << endl;
    // }
    // return 0;

    int lowerBound = (commitNumber-1-logNum);
    if(logNum >= commitNumber) {
      lowerBound = -1;
    }
    // debug 2
    // for(int i = commitNumber; i > lowerBound; i--) {
    //   cout << i << " " << sizeRecord[i] << endl;
    // }
    // return 0;
    // 3 3 4
    for(int i = (commitNumber-1); i > lowerBound; i--) {
      cout << "# commit " << i+1 << "\n";
      is.seekg(sizeRecord[i]);
      uint32_t information[7];
      for(int j = 0; j < 7; j++) {
        is.read(reinterpret_cast<char *>(&information[j]), sizeof(information[j]));
      }

      // uint8_t sizeOfFile;
      cout << "[new_file]\n";
      for(int j = 0; j < information[ADD_COUNT]; j++) {
        uint8_t sizeOfFile;
        is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
        char* fileName = new char[static_cast<int>(sizeOfFile)+1];
        // cout << sizeof(fileName) << endl;
        memset(fileName,0,static_cast<int>(sizeOfFile)+1);
        is.read(fileName, static_cast<int>(sizeOfFile));
        printf("%s\n", fileName);
        // cout << fileName << "\n";
        delete [] fileName;
      }

      cout << "[modified]\n";
      for(int j = 0; j < information[MODIFY_COUNT]; j++) {
        uint8_t sizeOfFile;
        is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
        char* fileName = new char[static_cast<int>(sizeOfFile)+1];
        memset(fileName,0,static_cast<int>(sizeOfFile)+1);
        is.read(fileName, static_cast<int>(sizeOfFile));
        printf("%s\n", fileName);
        // cout << fileName << "\n";
        delete [] fileName;
      }

      cout << "[copied]\n";
      for(int j = 0; j < information[COPY_COUNT]; j++) {
        uint8_t sizeOfFile;
        is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
        char* fileName = new char[static_cast<int>(sizeOfFile)+1];
        memset(fileName,0,static_cast<int>(sizeOfFile)+1);
        is.read(fileName, static_cast<int>(sizeOfFile));
        printf("%s => ", fileName);
        // cout << fileName << " => ";
        delete [] fileName;

        is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
        fileName = new char[static_cast<int>(sizeOfFile)+1];
        memset(fileName,0,static_cast<int>(sizeOfFile)+1);
        is.read(fileName, static_cast<int>(sizeOfFile));
        printf("%s\n", fileName);
        // cout << fileName << "\n";
      }

      cout << "[deleted]\n";
      for(int j = 0; j < information[DELETE_COUNT]; j++) {
        uint8_t sizeOfFile;
        is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
        char* fileName = new char[static_cast<int>(sizeOfFile)+1];
        memset(fileName,0,static_cast<int>(sizeOfFile)+1);
        is.read(fileName, static_cast<int>(sizeOfFile));
        printf("%s\n", fileName);
        // cout << fileName << "\n";
        delete [] fileName;
      }

      cout << "(MD5)\n";
      for(int j = 0; j < information[FILE_COUNT]; j++) {
        uint8_t sizeOfFile;
        is.read(reinterpret_cast<char *>(&sizeOfFile), sizeof(sizeOfFile));
        char* fileName = new char[sizeOfFile+1];
        memset(fileName,0,static_cast<int>(sizeOfFile)+1);
        is.read(fileName, static_cast<int>(sizeOfFile));
        printf("%s ", fileName);
        // cout << fileName << " ";
        delete [] fileName;
        unsigned char temp[16];
        for(int k = 0; k < 16; k++) {
          is.read(reinterpret_cast<char *>(&temp[k]), 1);
          cout << hex << getNumber(temp[k], 1) << getNumber(temp[k], 0);
        }
        cout << dec << "\n";
      }
      if(i == (lowerBound+1)) {
        continue;
      }
      cout << dec << "\n";
    }
    return 0;
  }

}

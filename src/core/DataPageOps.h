#ifndef DATAPAGE_OPS
#define DATAPAGE_OPS
#include "DiskManager.h"
#include "page.h"
#include "BPlusTree.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <map>
#include <string>
#include <vector>

#define TREE_ORDER 100

enum class DataType { StringType, IntType };

class DataPageOps {
private:
  std::string tableName;
  DiskManager diskManager;
  meta_page metaPage;
  BPlusTree <int, int>bPlusTree;
  std::string databaseName;

public:
  DataPageOps(const std::string &ptableName, int cacheSize = 2, const std::string &databaseName)
      : tableName(ptableName), diskManager(ptableName, cacheSize, databaseName), databaseName(databaseName), bPlusTree(TREE_ORDER){
    metaPage = diskManager.getMetaPage();
    // 从文件流实例化B+树
    std::string filename = "../data/" + tableName + ".idx";
    std::fstream file;
    file.open(filename, std::ios::in | std::ios::binary);
    if(!file.is_open()){
      cout<<"fail to open index file"<<endl;
    }
    bPlusTree = BPlusTree<int, int>(file, TREE_ORDER);
  };
  ~DataPageOps(){
    std::string filename = "../data/" + tableName + ".idx";
    std::fstream file;
    file.open(filename, std::ios::out | std::ios::binary);
    bPlusTree.serialize(file);
    file.close();
  };
  void insertRecord(std::string tableName, const std::vector<int> intDatas,
                    const std::vector<std::string> stringDatas) {
    int pageId;
    std::fstream &file = diskManager.getFile();
    data_page dataPage = {};
    if ((intDatas.size() + stringDatas.size()) != metaPage.colNum) {
      perror("not match!Insert fail!");
      return;
    }
    for (int i = 0; const auto &data : intDatas) {
      if (metaPage.colInfo[i].coltype ==
          static_cast<uint8_t>(DataType::IntType)) {
        std::array<char, MAX_NAME_LEN> colName;
        std::string scolName = metaPage.colInfo[i].colName;
        std::copy(scolName.begin(), scolName.end(), colName.begin());
        dataPage.intValues[colName] = data;
      }
      i++;
    }
    for (int i = 0; const auto &data : stringDatas) {
      if (metaPage.colInfo[i].coltype ==
          static_cast<uint8_t>(DataType::StringType)) {
        std::array<char, MAX_NAME_LEN> colName;
        std::string scolName = metaPage.colInfo[i].colName;
        std::copy(scolName.begin(), scolName.end(), colName.begin());
        const std::vector<uint8_t> &myVector =
            std::vector<uint8_t>(data.begin(), data.end());
        dataPage.stringValues[colName] = myVector;
      }
      i++;
    }
    if (metaPage.freePages.empty()) {
      file.seekg(0, std::ios::end);
      pageId = file.tellg() / PAGE_SIZE;
      metaPage.rowNum++; // tellg() 返回当前位置
    } else {
      // 有空闲页，从空闲页队列中取出
      pageId = metaPage.freePages.front();
      metaPage.freePages.pop();
    }
    std::string rawData = serializeDataPage(dataPage);
    const std::vector<char> &data =
        std::vector<char>(rawData.begin(), rawData.end());
    diskManager.writeNewPage(data);
    
    std::array <char, MAX_NAME_LEN> colName;
    std::string scolName = metaPage.primaryKey;
    std::copy(scolName.begin(), scolName.end(), colName.begin());
    int value = dataPage.intValues[colName];
    bPlusTree.insert(value, pageId);
  }

  void updateRecord(const std::string &tableName, std::string &columnCondition,
                    std::string &condition) {
    int pageId = 0;
    int type;
    int allPageNum = 0;
    int valueType = -1;
    int colType = 0;
    if (!condition.empty()) {
      
      size_t first = columnCondition.find(' ');
      std::string firstInfo = columnCondition.substr(0, first);
      size_t second = columnCondition.find(' ', first + 1);
      std::string secondInfo =
          columnCondition.substr(first + 1, second - first - 1);
      std::string thirdInfo = columnCondition.substr(second + 1);

      size_t firstSpace = condition.find(' ');
      std::string firstToken = condition.substr(0, firstSpace);
      size_t secondSpace = condition.find(' ', firstSpace + 1);
      std::string secondToken =
          condition.substr(firstSpace + 1, secondSpace - firstSpace - 1);
      std::string thirdToken = condition.substr(secondSpace + 1);

      // columnInfo:把列信息转换为array
      std::array<char, MAX_NAME_LEN> colName0;
      std::copy(firstInfo.begin(), firstInfo.end(), colName0.begin());
      const std::vector<uint8_t> &myVector0 =
          std::vector<uint8_t>(thirdInfo.begin(), thirdInfo.end());
      // where:把列信息转换为array
      std::array<char, MAX_NAME_LEN> colName;
      std::copy(firstToken.begin(), firstToken.end(), colName.begin());
      // where:解析op种类
      type = compareValue(secondToken);
      if (strcmp(firstToken.c_str(), metaPage.primaryKey) == 0) {
        // 是主键，要调用索引

        //}else{
        allPageNum = metaPage.rowNum;
        std::fstream &file = diskManager.getFile();
        if (file.is_open()) {
          std::cout << "open";
        }
        // 获取页码
        for (int i = 1; i <= allPageNum; i++) {
          // 计算数据页的起始位置
          std::streampos pageStart = static_cast<std::streampos>(i) * 4096;
          // 将文件指针移动到数据页的起始位置
          file.seekg(pageStart, std::ios::beg);

          data_page dataPage;
          std::string dataContent(static_cast<size_t>(4096), '\0');
          file.read(&dataContent[0], 4096);
          deserializeDataPage(dataPage, dataContent);

          if (dataPage.isdeleted == 0) {
            if (valueType == 0 || dataPage.stringValues.find(colName) !=
                                      dataPage.stringValues.end()) {
              valueType = 0; // 为字符串数值
              const std::vector<uint8_t> &myVector =
                  std::vector<uint8_t>(thirdToken.begin(), thirdToken.end());
              if (type == 0) {
                if (dataPage.stringValues[colName] == myVector) {
                  pageId = i;
                  if (colType == 0) {
                    dataPage.stringValues[colName0] = myVector0;
                  } else {
                    dataPage.intValues[colName0] = std::atoi(thirdInfo.c_str());
                  }
                  std::string rawData = serializeDataPage(dataPage);
                  const std::vector<char> &data =
                      std::vector<char>(rawData.begin(), rawData.end());
                  diskManager.updatePage(pageId, data);
                }
              } else if (type == -1) {
                if (dataPage.stringValues[colName] < myVector) {
                  pageId = i;
                  if (colType == 0) {
                    dataPage.stringValues[colName0] = myVector0;
                  } else {
                    dataPage.intValues[colName0] = std::atoi(thirdInfo.c_str());
                  }
                  std::string rawData = serializeDataPage(dataPage);
                  const std::vector<char> &data =
                      std::vector<char>(rawData.begin(), rawData.end());
                  diskManager.updatePage(pageId, data);
                }
              } else if (type == 1) {
                if (dataPage.stringValues[colName] > myVector) {
                  pageId = i;
                  if (colType == 0) {
                    dataPage.stringValues[colName0] = myVector0;
                  } else {
                    dataPage.intValues[colName0] = std::atoi(thirdInfo.c_str());
                  }
                  std::string rawData = serializeDataPage(dataPage);
                  const std::vector<char> &data =
                      std::vector<char>(rawData.begin(), rawData.end());
                  diskManager.updatePage(pageId, data);
                }
              }
            } else {
              // auto it2=dataPage.intValues.find(colName);
              // if(it2!=dataPage.intValues.end())
              //{
              if (type == 0) {
                if (dataPage.intValues[colName] ==
                    std::atoi(thirdToken.c_str())) {
                  pageId = i;
                  if (colType == 0) {
                    dataPage.stringValues[colName0] = myVector0;
                  } else {
                    dataPage.intValues[colName0] = std::atoi(thirdInfo.c_str());
                  }
                  std::string rawData = serializeDataPage(dataPage);
                  const std::vector<char> &data =
                      std::vector<char>(rawData.begin(), rawData.end());
                  diskManager.updatePage(pageId, data);
                }
              } else if (type == -1) {
                if (dataPage.intValues[colName] <
                    std::atoi(thirdToken.c_str())) {
                  pageId = i;
                  if (colType == 0) {
                    dataPage.stringValues[colName0] = myVector0;
                  } else {
                    dataPage.intValues[colName0] = std::atoi(thirdInfo.c_str());
                  }
                  std::string rawData = serializeDataPage(dataPage);
                  const std::vector<char> &data =
                      std::vector<char>(rawData.begin(), rawData.end());
                  diskManager.updatePage(pageId, data);
                }
              } else {
                if (dataPage.intValues[colName] >
                    std::atoi(thirdToken.c_str())) {
                  pageId = i;
                  if (colType == 0) {
                    dataPage.stringValues[colName0] = myVector0;
                  } else {
                    dataPage.intValues[colName0] = std::atoi(thirdInfo.c_str());
                  }
                  std::string rawData = serializeDataPage(dataPage);
                  const std::vector<char> &data =
                      std::vector<char>(rawData.begin(), rawData.end());
                  diskManager.updatePage(pageId, data);
                }
              }
              //}
            }
          }
        }
      }
      std::cout << pageId;
    }
  }

  void updateAll(const std::string &tableName, std::string &columnCondition) {
    int pageId = 0;
    int type;
    int allPageNum = 0;
    int valueType = -1;
    int colType = 0;

    size_t first = columnCondition.find(' ');
    std::string firstInfo = columnCondition.substr(0, first);
    size_t second = columnCondition.find(' ', first + 1);
    std::string secondInfo =
        columnCondition.substr(first + 1, second - first - 1);
    std::string thirdInfo = columnCondition.substr(second + 1);

    // columnInfo:把列信息转换为array
    std::array<char, MAX_NAME_LEN> colName0;
    std::copy(firstInfo.begin(), firstInfo.end(), colName0.begin());
    const std::vector<uint8_t> &myVector0 =
        std::vector<uint8_t>(thirdInfo.begin(), thirdInfo.end());

    allPageNum = metaPage.rowNum;
    std::fstream &file = diskManager.getFile();
    if (file.is_open()) {
      std::cout << "open";
    }
    // 获取页码
    for (int i = 1; i <= allPageNum; i++) {
      // 计算数据页的起始位置
      std::streampos pageStart = static_cast<std::streampos>(i) * 4096;
      // 将文件指针移动到数据页的起始位置
      file.seekg(pageStart, std::ios::beg);

      data_page dataPage;
      std::string dataContent(static_cast<size_t>(4096), '\0');
      file.read(&dataContent[0], 4096);
      deserializeDataPage(dataPage, dataContent);

      if (dataPage.isdeleted == 0) {
        pageId = i;
        if (colType == 0) {
          dataPage.stringValues[colName0] = myVector0;
        } else {
          dataPage.intValues[colName0] = std::atoi(thirdInfo.c_str());
        }
        std::string rawData = serializeDataPage(dataPage);
        const std::vector<char> &data =
            std::vector<char>(rawData.begin(), rawData.end());
        diskManager.updatePage(pageId, data);
      }
    }
    std::cout << pageId;
  }

  std::vector<std::map<std::string, std::string>>
  selectRecord(const std::string &tableName, std::vector<std::string> &colNames,
               std::string &condition) {
    std::vector<std::map<std::string, std::string>> recordSet = {};
    std::map<std::string, std::string> record = {};
    int valueType = -1;
    int colType = 0;
    int allPageNum;
    int type;
    int pageId = 0;
    allPageNum = metaPage.rowNum;
    std::fstream &file = diskManager.getFile();
    if (file.is_open()) {
      std::cout << "open";
    }
    size_t firstSpace = condition.find(' ');
    std::string firstToken = condition.substr(0, firstSpace);
    size_t secondSpace = condition.find(' ', firstSpace + 1);
    std::string secondToken =
        condition.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string thirdToken = condition.substr(secondSpace + 1);
    // where:把列信息转换为array
    std::array<char, MAX_NAME_LEN> colName;
    std::copy(firstToken.begin(), firstToken.end(), colName.begin());
    // where:解析op种类
    type = compareValue(secondToken);

    // 获取页码
    for (int i = 1; i <= allPageNum; i++) {
      // 计算数据页的起始位置
      std::streampos pageStart = static_cast<std::streampos>(i) * 4096;
      // 将文件指针移动到数据页的起始位置
      file.seekg(pageStart, std::ios::beg);

      data_page dataPage;
      std::string dataContent(static_cast<size_t>(4096), '\0');
      file.read(&dataContent[0], 4096);
      deserializeDataPage(dataPage, dataContent);
      if (dataPage.isdeleted == 0) {
        for (auto colName0 : colNames) // select信息
        {
          std::array<char, MAX_NAME_LEN> colNameArr;
          std::copy(colName0.begin(), colName0.end(), colNameArr.begin());
          if (valueType == 0 || dataPage.stringValues.find(colName) !=
                                    dataPage.stringValues.end()) {
            valueType = 0; // 为字符串数值
            const std::vector<uint8_t> &myVector =
                std::vector<uint8_t>(thirdToken.begin(), thirdToken.end());
            if (type == 0) {
              if (dataPage.stringValues[colName] == myVector) {
                if (colType == 0) {
                  std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                      dataPage.stringValues[colNameArr].end());
                  record.insert({colName0, colInfo});
                } else {
                  std::string colInfo =
                      std::to_string(dataPage.intValues[colNameArr]);
                  record.insert({colName0, colInfo});
                }

                std::cout << "find1";
              }
            } else if (type == -1) {
              if (dataPage.stringValues[colName] < myVector) {
                if (colType == 0) {
                  std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                      dataPage.stringValues[colNameArr].end());
                  record.insert({colName0, colInfo});
                } else {
                  std::string colInfo =
                      std::to_string(dataPage.intValues[colNameArr]);
                  record.insert({colName0, colInfo});
                }
              }
            } else if (type == 1) {
              if (dataPage.stringValues[colName] > myVector) {
                if (colType == 0) {
                  std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                      dataPage.stringValues[colNameArr].end());
                  record.insert({colName0, colInfo});
                } else {
                  std::string colInfo =
                      std::to_string(dataPage.intValues[colNameArr]);
                  record.insert({colName0, colInfo});
                }
              }
            }
          } else {
            // auto it2=dataPage.intValues.find(colName);
            // if(it2!=dataPage.intValues.end())
            //{
            if (type == 0) {
              if (dataPage.intValues[colName] ==
                  std::atoi(thirdToken.c_str())) {
                if (colType == 0) {
                  std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                      dataPage.stringValues[colNameArr].end());
                  record.insert({colName0, colInfo});
                } else {
                  std::string colInfo =
                      std::to_string(dataPage.intValues[colNameArr]);
                  record.insert({colName0, colInfo});
                }
                std::cout << "find2";
              }
            } else if (type == -1) {
              if (dataPage.intValues[colName] < std::atoi(thirdToken.c_str())) {
                if (colType == 0) {
                  std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                      dataPage.stringValues[colNameArr].end());
                  record.insert({colName0, colInfo});
                } else {
                  std::string colInfo =
                      std::to_string(dataPage.intValues[colNameArr]);
                  record.insert({colName0, colInfo});
                }
              }
            } else {

              if (dataPage.intValues[colName] > std::atoi(thirdToken.c_str())) {
                if (colType == 0) {
                  std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                      dataPage.stringValues[colNameArr].end());
                  record.insert({colName0, colInfo});
                } else {
                  std::string colInfo =
                      std::to_string(dataPage.intValues[colNameArr]);
                  record.insert({colName0, colInfo});
                }
              }
            }
            //}
          }
        }
      }
    }

    recordSet.push_back(record);

    return recordSet;
  }

  std::vector<std::map<std::string, std::string>>
  selectAllRow(const std::string &tableName,
               std::vector<std::string> &colNames) {
    std::vector<std::map<std::string, std::string>> recordSet = {};
    std::map<std::string, std::string> record = {};
    int valueType = -1;
    int allPageNum;
    int pageId = 0;
    int colType = 0;
    allPageNum = metaPage.rowNum;
    std::fstream &file = diskManager.getFile();
    if (file.is_open()) {
      std::cout << "open";
    }

    // 获取页码

    for (int i = 1; i <= allPageNum; i++) {
      // 计算数据页的起始位置
      std::streampos pageStart = static_cast<std::streampos>(i) * 4096;
      // 将文件指针移动到数据页的起始位置
      file.seekg(pageStart, std::ios::beg);

      data_page dataPage;
      std::string dataContent(static_cast<size_t>(4096), '\0');
      file.read(&dataContent[0], 4096);
      deserializeDataPage(dataPage, dataContent);
      if (dataPage.isdeleted == 0) {
        for (auto colName0 : colNames) // select信息
        {
          std::array<char, MAX_NAME_LEN> colNameArr;
          std::copy(colName0.begin(), colName0.end(), colNameArr.begin());
          if (colType == 0) {
            std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                dataPage.stringValues[colNameArr].end());
            record.insert({colName0, colInfo});
          } else {
            std::string colInfo =
                std::to_string(dataPage.intValues[colNameArr]);
            record.insert({colName0, colInfo});
          }
        }
      }
    }
    recordSet.push_back(record);

    return recordSet; // 元素为一行记录
  }

  std::vector<std::map<std::string, std::string>>
  selectAll(const std::string &tableName) {
    std::vector<std::map<std::string, std::string>> recordSet = {};
    std::map<std::string, std::string> record = {};
    int allPageNum;
    int pageId = 0;
    int colType = -1;
    allPageNum = metaPage.rowNum;
    std::fstream &file = diskManager.getFile();
    if (file.is_open()) {
      std::cout << "open";
    }
    // 获取页码
    // 获取页码
    for (int i = 1; i <= allPageNum; i++) {
      // 计算数据页的起始位置
      std::streampos pageStart = static_cast<std::streampos>(i) * 4096;
      // 将文件指针移动到数据页的起始位置
      file.seekg(pageStart, std::ios::beg);

      data_page dataPage;
      std::string dataContent(static_cast<size_t>(4096), '\0');
      file.read(&dataContent[0], 4096);
      deserializeDataPage(dataPage, dataContent);
      if (dataPage.isdeleted == 0) {
        for (auto col : metaPage.colInfo) {
          std::array<char, MAX_NAME_LEN> colName;
          std::copy(std::string(col.colName).begin(),
                    std::string(col.colName).end(), colName.begin());
          if (col.coltype == static_cast<uint8_t>(DataType::StringType)) {

            std::string colInfo(dataPage.stringValues[colName].begin(),
                                dataPage.stringValues[colName].end());
            record.insert({col.colName, colInfo});
          } else {
            std::string colInfo = std::to_string(dataPage.intValues[colName]);
            record.insert({col.colName, colInfo});
          }
        }
        recordSet.push_back(record);
      }
    }
    return recordSet; // 元素为一行记录
  }

  void deleteRecord(const std::string &tableName, std::string condition) {
    int pageId = 0;
    int type;
    int allPageNum = 0;
    int valueType = -1;
    if (!condition.empty()) {
      size_t firstSpace = condition.find(' ');
      std::string firstToken = condition.substr(0, firstSpace);
      size_t secondSpace = condition.find(' ', firstSpace + 1);
      std::string secondToken =
          condition.substr(firstSpace + 1, secondSpace - firstSpace - 1);
      std::string thirdToken = condition.substr(secondSpace + 1);

      // 解析op种类
      type = compareValue(secondToken);
      // 把列信息转换为array
      std::array<char, MAX_NAME_LEN> colName;
      std::copy(firstToken.begin(), firstToken.end(), colName.begin());
      if (strcmp(firstToken.c_str(), metaPage.primaryKey) == 0) {
        // 是主键，要调用索引

        //}else{
        allPageNum = metaPage.rowNum;
        std::fstream &file = diskManager.getFile();
        if (file.is_open()) {
          std::cout << "open";
        }
        // 获取页码
        for (int i = 1; i <= allPageNum; i++) {
          // 计算数据页的起始位置
          std::streampos pageStart = static_cast<std::streampos>(i) * 4096;
          // 将文件指针移动到数据页的起始位置
          file.seekg(pageStart, std::ios::beg);

          data_page dataPage;
          std::string dataContent(static_cast<size_t>(4096), '\0');
          file.read(&dataContent[0], 4096);
          deserializeDataPage(dataPage, dataContent);
          if (valueType == 0 || dataPage.stringValues.find(colName) !=
                                    dataPage.stringValues.end()) {
            valueType = 0; // 为字符串数值
            const std::vector<uint8_t> &myVector =
                std::vector<uint8_t>(thirdToken.begin(), thirdToken.end());
            if (type == 0) {
              if (dataPage.stringValues[colName] == myVector) {
                pageId = i;
                dataPage.isdeleted = 1;
                diskManager.deletePage(pageId);
                std::cout << "find1";
              }
            } else if (type == -1) {
              if (dataPage.stringValues[colName] < myVector) {
                pageId = i;
                dataPage.isdeleted = 1;
                diskManager.deletePage(pageId);
              }
            } else if (type == 1) {
              if (dataPage.stringValues[colName] > myVector) {
                pageId = i;
                dataPage.isdeleted = 1;
                diskManager.deletePage(pageId);
              }
            }
          } else {
            // auto it2=dataPage.intValues.find(colName);
            // if(it2!=dataPage.intValues.end())
            //{
            if (type == 0) {
              if (dataPage.intValues[colName] ==
                  std::atoi(thirdToken.c_str())) {
                pageId = i;
                dataPage.isdeleted = 1;
                diskManager.deletePage(pageId);
                std::cout << "find2";
              }
            } else if (type == -1) {
              if (dataPage.intValues[colName] < std::atoi(thirdToken.c_str())) {
                pageId = i;
                dataPage.isdeleted = 1;
                diskManager.deletePage(pageId);
                std::cout << "find2";
              }
            } else {

              if (dataPage.intValues[colName] > std::atoi(thirdToken.c_str())) {
                pageId = i;
                dataPage.isdeleted = 1;
                diskManager.deletePage(pageId);
                std::cout << "find2";
              }
            }
            //}
          }
        }
      }
      std::cout << pageId;
    }
  }

  void deleteAll(const std::string &tableName) {
    std::string filename = "../data/" + tableName + ".db";
    std::string idxfilename = "../data/" + tableName + ".idx";
    if (std::remove(filename.c_str()) != 0) {
      perror("Error deleting file");
    }
    std::remove(idxfilename.c_str());
    metaPage.rowNum = 0;
    metaPage.freePages = {};
    std::fstream file;
    file.open(filename, std::ios::out | std::ios::binary);
    file.seekg(0, std::ios_base::beg);
    std::string serializedMetaPage = serializeMetaPage(metaPage);
    file.write(serializedMetaPage.data(), serializedMetaPage.size());
    file.close();
  }

  meta_page &getMetaPage() { return metaPage; }

private:
  int compareValue(const std::string &secondToken) {
    int type;
    if (secondToken == "<") {
      type = -1;
    } else if (secondToken == ">") {
      type = 1;
    } else {
      type = 0;
    }
    return type;
  }
};

// 根据用户输入的内容自动转换为 DataType
DataType determineDataType(const std::string &userInput) {
  if (userInput.front() == '"' && userInput.back() == '"') {
    return DataType::StringType;
  } else {
    return DataType::IntType;
  }
}
#endif
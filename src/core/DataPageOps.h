#ifndef DATAPAGE_OPS
#define DATAPAGE_OPS
#include "BPlusTree.h"
#include "../parser/data.h"
#include "DiskManager.h"
#include "page.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <map>
#include <string>
#include <vector>
enum class DataType { StringType, IntType };

#define TREE_ORDER 100

#define CACHE_SIZE 100

class DataPageOps {
private:
  std::string tableName;
  DiskManager diskManager;
  meta_page metaPage;
  BPlusTree<int, int> bPlusTree;
  std::string databaseName;

public:
  DataPageOps(const std::string &ptableName, 
              std::string &databaseName)
      : tableName(ptableName), diskManager(ptableName, CACHE_SIZE, databaseName),
        databaseName(databaseName), bPlusTree(TREE_ORDER) {
    metaPage = diskManager.getMetaPage();
    // 从文件流实例化B+树
    std::string filename = "./data/" + databaseName + "/" + tableName + ".idx";
    std::fstream file;
    file.open(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
      std::cout << "fail to open index file" << std::endl;
    }
    bPlusTree.deserialize(file);
  }

  ~DataPageOps() {
    std::string filename = "./data/" + databaseName + "/" + tableName + ".idx";
    std::fstream file;
    file.open(filename, std::ios::out | std::ios::binary);
    bPlusTree.serialize(file);
    file.close();
  };
  

  void insertRecord(std::string tableName, std::vector<Data_Type> record) {
    int pageId;
    std::fstream &file = diskManager.getFile();
    std::string primaryKey(metaPage.primaryKey);
    data_page dataPage = {};
    int colNum = metaPage.colNum;

    for (int i = 0; i < colNum; i++) {

      if (isMatchType(int(metaPage.colInfo[i].coltype), record[i].flag) == 1) {
        return;
      } else {
        std::array<char, MAX_NAME_LEN> colName;
        std::string scolName = metaPage.colInfo[i].colName;
        std::copy(scolName.begin(), scolName.end(), colName.begin());
        if (record[i].flag == 1) {
          std::string stringData(record[i].data.str_data);
          const std::vector<uint8_t> &myVector =
              std::vector<uint8_t>(stringData.begin(), stringData.end());
          dataPage.stringValues[colName] = myVector;
        } else {
          // if (scolName == primaryKey) {
          //   int repeatPageId = bPlusTree.searchForData(record[i].data.num_int);
          //   if (repeatPageId != -1) {
          //     std::cout << "不允许重复主键！" << std::endl;
          //     return;
          //   }
          // }
          dataPage.intValues[colName] = record[i].data.num_int;
        }
      }
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
    std::array<char, MAX_NAME_LEN> colName;
    std::string scolName = metaPage.primaryKey;
    std::copy(scolName.begin(), scolName.end(), colName.begin());
    int value = dataPage.intValues[colName];
    bPlusTree.insert(value, pageId);
  }

  void updateRecord(const std::string &tableName, Condition &columnCondition,
                    Condition &condition) {
    int pageId = 0;
    int type = 0;
    int allPageNum = 0;
    int colType = 0;
    int valueType = 0;
    std::string primaryKey(metaPage.primaryKey);
    allPageNum = metaPage.rowNum;
    int isError = 0;
    int IntthirdToken;
    std::string SthirdToken;
    int IntthirdInfo;
    std::string SthirdInfo;
    std::string firstToken = condition.name;
    Relation relationOp = condition.relation_op;
    Data_Type conditionData = condition.data;

    // where:把列信息转换为array
    std::array<char, MAX_NAME_LEN> colName;
    std::copy(firstToken.begin(), firstToken.end(), colName.begin());

    // where:判断是否符合列类型
    if(isMatchType(metaPage.colInfoMap[colName], conditionData.flag)==1)
      return;

    std::string firstInfo = columnCondition.name;
    Data_Type columnConditionData = columnCondition.data;
    std::vector<uint8_t> myVector0;

    // set:把列信息转换为array
    std::array<char, MAX_NAME_LEN> colName0;
    std::copy(firstInfo.begin(), firstInfo.end(), colName0.begin());

    // set:判断是否符合列类型
    if(isMatchType(metaPage.colInfoMap[colName0], columnConditionData.flag)==1)
      return;

    if (conditionData.flag == 0) {
      IntthirdToken = conditionData.data.num_int;
    } else {
      SthirdToken = conditionData.data.str_data;
    }

    if (columnConditionData.flag == 0) {
      IntthirdInfo = columnConditionData.data.num_int;
    } else {
      SthirdInfo = columnConditionData.data.str_data;
      myVector0 = std::vector<uint8_t>(SthirdInfo.begin(), SthirdInfo.end());
    }

    valueType = (conditionData.flag == 0 ? 1 : 0);
    colType = (columnConditionData.flag == 0 ? 1 : 0);
    std::fstream &file = diskManager.getFile();
    // where:解析op种类
    type = compareValue(relationOp);

    if (firstToken == primaryKey) {
      // 是主键，要调用索引
      if (type == 0) {
        pageId = bPlusTree.search(IntthirdToken);
        std::streampos pageStart = static_cast<std::streampos>(pageId) * 4096;
        // 将文件指针移动到数据页的起始位置
        file.seekg(pageStart, std::ios::beg);
        data_page dataPage;
        std::string dataContent(static_cast<size_t>(4096), '\0');
        file.read(&dataContent[0], 4096);
        deserializeDataPage(dataPage, dataContent);
        if (colType == 0) {
          dataPage.stringValues[colName0] = myVector0;
        } else {
          if (firstInfo == primaryKey) // 修改的信息是主键则调用索引
          {
            int repeatPageId = bPlusTree.searchForData(IntthirdInfo);
            // 判断是否发生主键重复，未重复更新索引
            // if (pageId == repeatPageId || repeatPageId == -1) // 说明未重复
            // {
            //   bPlusTree.update(IntthirdInfo,
            //                    pageId); // 更新索引
            // } else {
            //   throw std::invalid_argument("not allow to repeat primaryKey");
            //   return;
            // }
          }
          dataPage.intValues[colName0] = IntthirdInfo;
        }
        std::string rawData = serializeDataPage(dataPage);
        const std::vector<char> &data =
            std::vector<char>(rawData.begin(), rawData.end());
        diskManager.updatePage(pageId, data);
      } else {
        std::vector<int> pageIds = {};
        if (type == 1) {
          pageIds = bPlusTree.findGreaterThan(IntthirdToken);
        } else {
          pageIds = bPlusTree.findLessThan(IntthirdToken);
        }
        for (auto id : pageIds) {
          pageId = id;
          std::streampos pageStart = static_cast<std::streampos>(pageId) * 4096;
          // 将文件指针移动到数据页的起始位置
          file.seekg(pageStart, std::ios::beg);
          data_page dataPage;
          std::string dataContent(static_cast<size_t>(4096), '\0');
          file.read(&dataContent[0], 4096);
          deserializeDataPage(dataPage, dataContent);
          if (colType == 0) {
            dataPage.stringValues[colName0] = myVector0;
          } else {
            if (firstInfo == primaryKey) // 修改的信息是主键则调用索引
            {
              int repeatPageId = bPlusTree.searchForData(IntthirdInfo);
              // 判断是否发生主键重复，未重复更新索引
              // if (pageId == repeatPageId || repeatPageId == -1) // 说明未重复
              // {
              //   bPlusTree.update(IntthirdInfo,
              //                    pageId); // 更新索引
              // } else {
              //   std::cout << "不允许重复主键！" << std::endl;
              //   return;
              // }
            }
            dataPage.intValues[colName0] = IntthirdInfo;
          }
          std::string rawData = serializeDataPage(dataPage);
          const std::vector<char> &data =
              std::vector<char>(rawData.begin(), rawData.end());
          diskManager.updatePage(pageId, data);
        }
      }

    } else {

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
          if (valueType == 0) {
            // 为字符串数值
            const std::vector<uint8_t> &myVector =
                std::vector<uint8_t>(SthirdToken.begin(), SthirdToken.end());
            if (type == 0) {
              if (dataPage.stringValues[colName] == myVector) {
                pageId = i;
                if (colType == 0) {
                  dataPage.stringValues[colName0] = myVector0;
                } else {
                  dataPage.intValues[colName0] = IntthirdInfo;
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
                  dataPage.intValues[colName0] = IntthirdInfo;
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
                  dataPage.intValues[colName0] = IntthirdInfo;
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
              std::cout  << "found";
              if (dataPage.intValues[colName] == IntthirdInfo) {
                pageId = i;
                if (colType == 0) {

                  dataPage.stringValues[colName0] = myVector0;
                } else {
                  if (firstInfo == primaryKey) // 修改的信息是主键则调用索引
                  {
                    int repeatPageId = bPlusTree.searchForData(IntthirdInfo);
                    // 判断是否发生主键重复，未重复更新索引
                    // if (pageId == repeatPageId ||
                    //     repeatPageId == -1) // 说明未重复
                    // {
                    //   bPlusTree.update(IntthirdInfo,
                    //                    pageId); // 更新索引
                    // } else {
                    //   std::cout << "不允许重复主键！" << std::endl;
                    //   return;
                    // }
                  }
                  dataPage.intValues[colName0] = IntthirdInfo;
                }
                std::string rawData = serializeDataPage(dataPage);
                const std::vector<char> &data =
                    std::vector<char>(rawData.begin(), rawData.end());
                diskManager.updatePage(pageId, data);
              }
            } else if (type == -1) {
              if (dataPage.intValues[colName] < IntthirdToken) {
                pageId = i;
                if (colType == 0) {
                  dataPage.stringValues[colName0] = myVector0;
                } else {
                  if (firstInfo == primaryKey) // 修改的信息是主键则调用索引
                  {
                    int repeatPageId = bPlusTree.searchForData(IntthirdInfo);
                    // 判断是否发生主键重复，未重复更新索引
                    // if (pageId == repeatPageId ||
                    //     repeatPageId == -1) // 说明未重复
                    // {
                    //   bPlusTree.update(IntthirdInfo,
                    //                    pageId); // 更新索引
                    // } else {
                    //   std::cout << "不允许重复主键！" << std::endl;
                    //   return;
                    // }
                  }
                  dataPage.intValues[colName0] = IntthirdInfo;
                }
                std::string rawData = serializeDataPage(dataPage);
                const std::vector<char> &data =
                    std::vector<char>(rawData.begin(), rawData.end());
                diskManager.updatePage(pageId, data);
              }
            } else {
              if (dataPage.intValues[colName] > IntthirdToken) {
                pageId = i;
                if (colType == 0) {
                  dataPage.stringValues[colName0] = myVector0;
                } else {
                  if (firstInfo == primaryKey) // 修改的信息是主键则调用索引
                  {
                    int repeatPageId = bPlusTree.searchForData(IntthirdInfo);
                    // 判断是否发生主键重复，未重复更新索引
                    // if (pageId == repeatPageId ||
                    //     repeatPageId == -1) // 说明未重复
                    // {
                    //   bPlusTree.update(IntthirdInfo,
                    //                    pageId); // 更新索引
                    // } else {
                    //   std::cout << "不允许重复主键！" << std::endl;
                    //   return;
                    // }
                  }
                  dataPage.intValues[colName0] = IntthirdInfo;
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

  void updateAll(const std::string &tableName, Condition &columnCondition) {
    int pageId = 0;
    int type = 0;
    int allPageNum = 0;
    int colType = 0;
    allPageNum = metaPage.rowNum;
    int isError = 0;
    int IntthirdInfo;
    std::string primaryKey = metaPage.primaryKey;
    std::string SthirdInfo;

    std::string firstInfo = columnCondition.name;
    Data_Type columnConditionData = columnCondition.data;
    std::vector<uint8_t> myVector0;

    // set:把列信息转换为array
    std::array<char, MAX_NAME_LEN> colName0;
    std::copy(firstInfo.begin(), firstInfo.end(), colName0.begin());

    // set:判断是否符合列类型
    if(isMatchType(metaPage.colInfoMap[colName0], columnConditionData.flag)==1)return;

    if (columnConditionData.flag == 0) {
      IntthirdInfo = columnConditionData.data.num_int;
    } else {
      SthirdInfo = columnConditionData.data.str_data;
      myVector0 = std::vector<uint8_t>(SthirdInfo.begin(), SthirdInfo.end());
    }
    colType = (columnConditionData.flag == 0 ? 1 : 0);

    std::fstream &file = diskManager.getFile();

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
          if (firstInfo == primaryKey) // 修改的信息是主键则调用索引
          {
            int repeatPageId = bPlusTree.searchForData(IntthirdInfo);
            // 判断是否发生主键重复，未重复更新索引
            // if (pageId == repeatPageId || repeatPageId == -1) // 说明未重复
            // {
            //   bPlusTree.update(IntthirdInfo,
            //                    pageId); // 更新索引
            // } else {
            //   std::cout << "不允许重复主键" << std::endl;
            // }
          }
          dataPage.intValues[colName0] = IntthirdInfo;
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
               Condition &condition) {
    std::vector<std::map<std::string, std::string>> recordSet = {};

    int pageId = 0;
    int type = 0;
    int allPageNum = 0;
    std::string primaryKey = metaPage.primaryKey;
    int valueType = 0;
    allPageNum = metaPage.rowNum;
    int isError = 0;
    int IntthirdToken;
    std::string SthirdToken;
    std::string firstToken = condition.name;
    Relation relationOp = condition.relation_op;
    Data_Type conditionData = condition.data;
    std::fstream &file = diskManager.getFile();

    // where:把列信息转换为array
    std::array<char, MAX_NAME_LEN> colName;
    std::copy(firstToken.begin(), firstToken.end(), colName.begin());

    // where:判断是否符合列类型
    if(isMatchType(metaPage.colInfoMap[colName], conditionData.flag)==1)return {};

    if (conditionData.flag == 0) {
      IntthirdToken = conditionData.data.num_int;
    } else {
      SthirdToken = conditionData.data.str_data;
    }
    valueType = (conditionData.flag == 0 ? 1 : 0);
    type = compareValue(relationOp);
    if (firstToken == primaryKey) {
      if (type == 0) {
        std::map<std::string, std::string> record = {};
        pageId = bPlusTree.search(IntthirdToken);
        std::streampos pageStart = static_cast<std::streampos>(pageId) * 4096;
        // 将文件指针移动到数据页的起始位置
        file.seekg(pageStart, std::ios::beg);
        data_page dataPage;
        std::string dataContent(static_cast<size_t>(4096), '\0');
        file.read(&dataContent[0], 4096);
        deserializeDataPage(dataPage, dataContent);
        for (auto colName0 : colNames) {
          std::array<char, MAX_NAME_LEN> colNameArr;
          std::copy(colName0.begin(), colName0.end(), colNameArr.begin());
          if (metaPage.colInfoMap[colNameArr] == 0) {
            std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                dataPage.stringValues[colNameArr].end());
            record.insert({colName0, colInfo});
          } else {
            std::string colInfo =
                std::to_string(dataPage.intValues[colNameArr]);
            record.insert({colName0, colInfo});
          }
        }
        recordSet.push_back(record);
      } else {
        std::vector<int> pageIds = {};
        if (type == 1) {
          pageIds = bPlusTree.findGreaterThan(IntthirdToken);
        } else {
          pageIds = bPlusTree.findLessThan(IntthirdToken);
        }
        for (auto Id : pageIds) {
          pageId = Id;
          std::streampos pageStart = static_cast<std::streampos>(pageId) * 4096;
          // 将文件指针移动到数据页的起始位置
          file.seekg(pageStart, std::ios::beg);
          data_page dataPage;
          std::string dataContent(static_cast<size_t>(4096), '\0');
          file.read(&dataContent[0], 4096);
          deserializeDataPage(dataPage, dataContent);
          std::map<std::string, std::string> record = {};
          for (auto colName0 : colNames) {
            std::array<char, MAX_NAME_LEN> colNameArr;
            std::copy(colName0.begin(), colName0.end(), colNameArr.begin());
            if (metaPage.colInfoMap[colNameArr] == 0) {
              std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                  dataPage.stringValues[colNameArr].end());
              record.insert({colName0, colInfo});
            } else {
              std::string colInfo =
                  std::to_string(dataPage.intValues[colNameArr]);
              record.insert({colName0, colInfo});
            }
          }
          recordSet.push_back(record);
        }
      }
    } else {
    
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
        std::map<std::string, std::string> record = {};
        for (auto colName0 : colNames) // select信息
        {
          std::array<char, MAX_NAME_LEN> colNameArr;
          std::copy(colName0.begin(), colName0.end(), colNameArr.begin());
          if (valueType == 0) {

            valueType = 0; // 为字符串数值
            const std::vector<uint8_t> &myVector =
                std::vector<uint8_t>(SthirdToken.begin(), SthirdToken.end());
            if (type == 0) {
              if (dataPage.stringValues[colName] == myVector) {
                if (metaPage.colInfoMap[colNameArr] == 0) {
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
                if (metaPage.colInfoMap[colNameArr] == 0) {
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
                if (metaPage.colInfoMap[colNameArr] == 0) {
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
              if (dataPage.intValues[colName] == IntthirdToken) {
                if (metaPage.colInfoMap[colNameArr] == 0) {
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
              if (dataPage.intValues[colName] < IntthirdToken) {
                if (metaPage.colInfoMap[colNameArr] == 0) {
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

              if (dataPage.intValues[colName] > IntthirdToken) {
                if (metaPage.colInfoMap[colNameArr] == 0) {
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
        recordSet.push_back(record);
      }
    }
    }
    return recordSet;
  }

  std::vector<std::map<std::string, std::string>>
  selectAllRow(const std::string &tableName,
               std::vector<std::string> &colNames) {
    std::vector<std::map<std::string, std::string>> recordSet = {};
    int valueType = -1;
    int allPageNum;
    int pageId = 0;
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
        std::map<std::string, std::string> record = {};
        for (auto colName0 : colNames) // select信息
        {
          std::array<char, MAX_NAME_LEN> colNameArr;
          std::copy(colName0.begin(), colName0.end(), colNameArr.begin());
          if (metaPage.colInfoMap[colNameArr] == 0) {
            std::string colInfo(dataPage.stringValues[colNameArr].begin(),
                                dataPage.stringValues[colNameArr].end());
            record.insert({colName0, colInfo});
          } else {
            std::string colInfo =
                std::to_string(dataPage.intValues[colNameArr]);
            record.insert({colName0, colInfo});
          }
        }
        recordSet.push_back(record);
      }
    }

    return recordSet; // 元素为一行记录
  }

  std::vector<std::map<std::string, std::string>>
  selectAll(const std::string &tableName) {
    std::vector<std::map<std::string, std::string>> recordSet = {};
    int allPageNum;
    int pageId = 0;
    int colType = 1;
    allPageNum = metaPage.rowNum;
    std::fstream &file = diskManager.getFile();
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
        std::map<std::string, std::string> record =
            {}; // Create a new map for each record

        for (int i = 0; i < metaPage.colNum; i++) {
          auto col = metaPage.colInfo[i];
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
    return recordSet;
  }

  void deleteRecord(const std::string &tableName, Condition &condition) {
    int pageId = 0;
    int type = 0;
    int allPageNum = metaPage.rowNum;
    int valueType = 0;
    int isError = 0;
    std::fstream &file = diskManager.getFile();
    int IntthirdToken;
    std::string primaryKey(metaPage.primaryKey);
    std::string SthirdToken;
    std::string firstToken = condition.name;
    Relation relationOp = condition.relation_op;
    Data_Type conditionData = condition.data;
    // 解析op种类
    type = compareValue(relationOp);
    // where:把列信息转换为array
    std::array<char, MAX_NAME_LEN> colName;
    std::copy(firstToken.begin(), firstToken.end(), colName.begin());

    // where:判断是否符合列类型
    if(isMatchType(metaPage.colInfoMap[colName], conditionData.flag)==1)return;

    if (conditionData.flag == 0) {
      IntthirdToken = conditionData.data.num_int;
    } else {
      SthirdToken = conditionData.data.str_data;
    }
    valueType = (conditionData.flag == 0 ? 1 : 0);
    if (firstToken == primaryKey) {
      if (type == 0) {
        pageId = bPlusTree.search(IntthirdToken);
        std::streampos pageStart = static_cast<std::streampos>(pageId) * 4096;
        // 将文件指针移动到数据页的起始位置
        file.seekg(pageStart, std::ios::beg);
        data_page dataPage;
        std::string dataContent(static_cast<size_t>(4096), '\0');
        file.read(&dataContent[0], 4096);
        deserializeDataPage(dataPage, dataContent);
        dataPage.isdeleted = 1;
        diskManager.deletePage(pageId);
        bPlusTree.remove(IntthirdToken);
      } else {
        std::vector<int> pageIds = {};
        if (type == 1) {
          pageIds = bPlusTree.findGreaterThan(IntthirdToken);
        } else {
          pageIds = bPlusTree.findLessThan(IntthirdToken);
        }
        for (auto Id : pageIds) {
          pageId = Id;
          std::streampos pageStart = static_cast<std::streampos>(pageId) * 4096;
          // 将文件指针移动到数据页的起始位置
          file.seekg(pageStart, std::ios::beg);
          data_page dataPage;
          std::string dataContent(static_cast<size_t>(4096), '\0');
          file.read(&dataContent[0], 4096);
          deserializeDataPage(dataPage, dataContent);
          dataPage.isdeleted = 1;
          diskManager.deletePage(pageId);
          bPlusTree.remove(IntthirdToken);
        }
      }
    } else {
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
        if (valueType == 0) {
          const std::vector<uint8_t> &myVector =
              std::vector<uint8_t>(SthirdToken.begin(), SthirdToken.end());
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
              std::cout << "error";
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
            if (dataPage.intValues[colName] == IntthirdToken) {
              pageId = i;
              dataPage.isdeleted = 1;
              diskManager.deletePage(pageId);
              bPlusTree.remove(IntthirdToken);
              std::cout << "find2";
            }
          } else if (type == -1) {
            if (dataPage.intValues[colName] < IntthirdToken) {
              pageId = i;
              dataPage.isdeleted = 1;
              diskManager.deletePage(pageId);
              bPlusTree.remove(IntthirdToken);
              std::cout << "find2";
            }
          } else {

            if (dataPage.intValues[colName] > IntthirdToken) {
              pageId = i;
              dataPage.isdeleted = 1;
              diskManager.deletePage(pageId);
              bPlusTree.remove(IntthirdToken);
              std::cout << "find2";
            }
          }
          //}
        }
      }
    }
    std::cout << pageId;
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
  int compareValue(const Relation relationOp) {
    int type;
    if (relationOp == Relation::LESS) {
      type = -1;
    } else if (relationOp == Relation::GREAT) {
      type = 1;
    } else {
      type = 0;
    }
    return type;
  }
  // 判断是否符合字段类型
  int isMatchType(int a, int b) {
    int error = 0;
    if ((a == 0 && b == 0) || (a == 1 && b == 1)) {
      error = 0;
      std::cout << "类型不匹配！" << std::endl;
    }
    return error;
  }
};

#endif
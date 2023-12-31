/**
 * @file DiskManager.h
 * @brief 磁盘管理器 跟踪和管理空闲页
 * @version 0.1
 * @date 2023-12-5
 */
#ifndef DISKMANAGER_
#define DISKMANAGER_
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <fstream>
#include "BufferManager.h"
#include"page.h"
#include <cstring>

#define PATH_PREFIX "./data/"

class DiskManager {
public:
    DiskManager(const std::string& table_name, int cache_size, const std::string& database_name) 
    : filename(PATH_PREFIX+database_name+"/"+table_name + ".db"), bufferManager(cache_size, file) {
        openFile(file, filename);
    }
    ~DiskManager()
    {  
        UpdateMetaPage();
    }

    // 使用 BufferManager 读取页面
    std::vector<char> readPage(int page_id) {
        return bufferManager.getPage(page_id);
    }

    // 删除一个页面 需要将页面加入空闲页队列
    void deletePage(int page_id)
    {   
        clearPage(page_id);
        metaPage.freePages.push(page_id);
    }

    // 写入一个新页面
    void writeNewPage(const std::vector<char>& page) {
        int page_id = newPage();
        writePage(page_id, page);
    }

    // 更新一个页面
    void updatePage(int page_id, const std::vector<char>& page) {
        writePage(page_id, page);
    }


      void clearPage(int page_id)
    {
        auto emptyPage = std::vector<char>(PAGE_SIZE, '\0');
        writePage(page_id, emptyPage);
    }

    meta_page& getMetaPage()
    {
        return metaPage;
    }

    std::fstream& getFile()
    {   
        return file;
    }


private:
    std::string filename;
    std::fstream file;
    BufferManager bufferManager;
    meta_page metaPage;

    // 打开文件
    void openFile(std::fstream& file, const std::string& filename) {
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if(file.is_open()){
            loadMetaPage();
            std::cout<<metaPage.rowNum;
            std::cout<<metaPage.colNum;
            std::cout<<metaPage.primaryKey;
            std::cout<<metaPage.tableName;
            return;
        }
            
        if (!file.is_open()) {
            // 清除所有错误标志
            file.clear();

            
            // 创建文件
            file.open(filename, std::ios::out | std::ios::binary);
            metaPage={};
            std::strncpy(metaPage.tableName, filename.c_str(), MAX_NAME_LEN - 1);
            
            if (!file) {
                throw std::runtime_error("Unable to create file: " + filename);
            }

            // 填充4K的元数据区域
            std::vector<char> metadata(PAGE_SIZE, '\0'); // 使用'\0'初始化元数据区
            file.write(metadata.data(), PAGE_SIZE);
            if (!file) {
                throw std::runtime_error("Failed to write metadata to file: " + filename);
            }

            // 关闭并重新以读写模式打开文件
            file.close();
            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
            if (!file) {
                throw std::runtime_error("Unable to open file after metadata write: " + filename);
            }
        }
    }

    // 使用 BufferManager 写入页面
    void writePage(int page_id, const std::vector<char>& page) {
        bufferManager.writePage(page_id, page);
    }

    // 新建一个页面
    int newPage() {
        int page_id;
        if (metaPage.freePages.empty()) {
            // 没有空闲页，新建一个
            file.seekg(0, std::ios::end);
            page_id = file.tellg()/ PAGE_SIZE;
            metaPage.rowNum++; // tellg() 返回当前位置
        } else {
            // 有空闲页，从空闲页队列中取出
            page_id = metaPage.freePages.front();
            metaPage.freePages.pop();
        }
    
        return page_id;
    }

  
    void loadMetaPage()
    {   
        file.seekg(0, std::ios_base::beg);
        file.seekg(0, std::ios_base::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios_base::beg);
        std::string metaContent(static_cast<size_t>(fileSize), '\0');
        file.read(&metaContent[0], fileSize);

        deserializeMetaPage(metaPage, metaContent);
    }

    void UpdateMetaPage()
    {
        file.seekg(0, std::ios_base::beg);
        std::string serializedMetaPage = serializeMetaPage(metaPage);
        file.write(serializedMetaPage.data(), serializedMetaPage.size());
    }
};

#endif
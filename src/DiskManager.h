/**
 * @file DiskManager.h
 * @brief 磁盘管理器 跟踪和管理空闲页
 * @version 0.1
 * @date 2023-12-5
 */

#include <string>
#include <queue>
#include <fstream>
#include "BufferManager.h"

#define PATH_PREFIX "../data/"

class DiskManager {
public:
    DiskManager(const std::string& table_name, int cache_size) 
    : filename(PATH_PREFIX + table_name + ".db"), bufferManager(cache_size, file) {
        openFile(file, filename);
    }

    // 使用 BufferManager 读取页面
    std::vector<char> readPage(int page_id) {
        return bufferManager.getPage(page_id);
    }

    // 删除一个页面 需要将页面加入空闲页队列
    void deletePage(int page_id);

    // 写入一个新页面
    void writeNewPage(const std::vector<char>& page) {
        int page_id = newPage();
        writePage(page_id, page);
    }

    // 更新一个页面
    void updatePage(int page_id, const std::vector<char>& page) {
        writePage(page_id, page);
    }

private:
    std::string filename;
    std::fstream file;
    BufferManager bufferManager;
    std::queue<int> freePages; // 空闲页队列

    // 打开文件
    void openFile(std::fstream& file, const std::string& filename) {
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

        if (!file.is_open()) {
            // 清除所有错误标志
            file.clear();

            // 创建文件
            file.open(filename, std::ios::out | std::ios::binary);
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
        if (freePages.empty()) {
            // 没有空闲页，新建一个
            file.seekg(0, std::ios::end);
            page_id = file.tellg() / PAGE_SIZE; // tellg() 返回当前位置
        } else {
            // 有空闲页，从空闲页队列中取出
            page_id = freePages.front();
            freePages.pop();
        }
        return page_id;
    }
};


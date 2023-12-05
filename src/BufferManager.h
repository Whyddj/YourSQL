/**
 * @file BufferManager.h
 * @author why
 * @brief 缓存管理器 负责缓存的管理
 * @version 0.1
 * @date 2023-12-5
 *
 *
 */
#include <unordered_map>
#include <vector>

const int PAGE_SIZE = 4096; // 假设页大小为4096字节

struct Page {
    char data[PAGE_SIZE];
    // 可以包含其他元数据
};

class BufferManager {
    std::unordered_map<int, Page> buffer; // 以页号为键存储页

public:
    Page* getPage(int pageID) {
        auto it = buffer.find(pageID);
        if (it != buffer.end()) {
            // 找到了，直接返回内存中的页
            return &(it->second);
        } else {
            // 未找到，从磁盘加载页到缓冲区
            Page newPage;
            // ... 从磁盘读取数据填充newPage ...
            buffer[pageID] = newPage;
            return &(buffer[pageID]);
        }
    }
};
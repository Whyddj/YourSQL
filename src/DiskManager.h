/**
 * @file DiskManager.h
 * @author why
 * @brief 磁盘管理器 跟踪和管理空闲页
 * @version 0.1
 * @date 2023-12-5
 *
 *
 */
#include <iostream>
#include <queue>
#include <unordered_map>

// TODO
class DiskManager {
    std::queue<int> freePages; // 空闲页队列
    std::unordered_map<int, char*> pages; // 页号到页内容的映射
    int nextPageID = 0; // 下一个可用的页号

public:
    int allocatePage() {
        if (!freePages.empty()) {
            int pageID = freePages.front();
            freePages.pop();
            return pageID;
        }
        return nextPageID++;
    }

    void freePage(int pageID) {
        freePages.push(pageID);
        // 可能还需要清除或初始化页的内容
    }

    // TODO
};



/**
 * @file BufferManager.h
 * @author why
 * @brief 缓存管理器 负责缓存的管理
 * @version 0.2
 * @date 2023-12-5
 *
 *
 */
#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <list>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <fstream>
#include <stdexcept>
#include <iostream>
// #include "../util/util.h"

#define PAGE_SIZE 4096

// 定义一些颜色代码 方便调试
// const std::string RED = "\033[31m";     // 红色
// const std::string GREEN = "\033[32m";   // 绿色
// const std::string YELLOW = "\033[33m";  // 黄色
// const std::string RESET = "\033[0m";    // 重置颜色

/**
 * @brief 缓存管理器
 * @details 负责缓存的管理
 */
class BufferManager {
public:
    BufferManager(int capacity, std::fstream& file) : capacity(capacity), file(file) {
        // std::cout << YELLOW << "[BufferManager--DEBUG] BufferManager created with capacity: " << capacity  << RESET << std::endl;
        // util::log(util::DEBUG, "BufferManager: created with capacity: ", capacity);
    }
    
    ~BufferManager() {
        // std::cout << YELLOW << "[BufferManager--DEBUG] BufferManager destructing. Writing dirty pages back to disk."  << RESET << std::endl;
        // util::log(util::DEBUG, "BufferManager: destructing. Writing dirty pages back to disk.");
        // 将所有脏页写回磁盘
        for (auto& p : cache) {
            if (p.second.isDirty) {
                // std::cout << YELLOW << "[BufferManager--DEBUG] Writing dirty page " << p.first << " to disk."  << RESET << std::endl;
                // util::log(util::DEBUG, "BufferManager: Writing dirty page ", p.first, " to disk.");
                writePageToDisk(p.first, p.second.data);
            }
        }
        file.close();
    }

    // 获取页面
    std::vector<char> getPage(int page_id) {
        // std::lock_guard<std::mutex> lock(mutex);
        // std::cout << YELLOW << "[BufferManager--DEBUG] Requesting page " << page_id  << RESET << std::endl;
        // util::log(util::DEBUG, "BufferManager: Requesting page ", page_id);

        if (cache.find(page_id) != cache.end()) {
            // std::cout << YELLOW << "[BufferManager--DEBUG] Page " << page_id << " found in cache."  << RESET << std::endl;
            // util::log(util::DEBUG, "BufferManager: Page ", page_id, " found in cache.");
            // 页面在缓存中，移动到LRU列表的前端
            touch(page_id);
            return cache[page_id].data;
        } else {
            // 页面不在缓存中，需要从磁盘加载
            // std::cout << YELLOW << "[BufferManager--DEBUG] Page " << page_id << " not in cache. Loading from disk."  << RESET << std::endl;
            // util::log(util::DEBUG, "BufferManager: Page ", page_id, " not in cache. Loading from disk.");
            try {
                loadPageFromDisk(page_id);
                return cache[page_id].data;
            } catch (const std::exception& e) {
                // std::cerr << "Failed to load page " << page_id << " from disk: " << e.what() << std::endl;
                // util::log(util::ERROR, "BufferManager: Failed to load page ", page_id, " from disk: ", e.what());
                throw std::runtime_error("Failed to load page from disk: " + std::string(e.what()));
            }
        }
    }

    const std::vector<char>& getPageConst(int page_id) {
        // std::lock_guard<std::mutex> lock(mutex);
        // std::cout << YELLOW << "[BufferManager--DEBUG] Requesting page " << page_id  << RESET << std::endl;
        // util::log(util::DEBUG, "BufferManager: Requesting page ", page_id);

        if (cache.find(page_id) != cache.end()) {
            // std::cout << YELLOW << "[BufferManager--DEBUG] Page " << page_id << " found in cache."  << RESET << std::endl;
            // util::log(util::DEBUG, "BufferManager: Page ", page_id, " found in cache.");
            // 页面在缓存中，移动到LRU列表的前端
            touch(page_id);
            return cache[page_id].data;
        } else {
            // 页面不在缓存中，需要从磁盘加载
            // std::cout << YELLOW << "[BufferManager--DEBUG] Page " << page_id << " not in cache. Loading from disk."  << RESET << std::endl;
            // util::log(util::DEBUG, "BufferManager: Page ", page_id, " not in cache. Loading from disk.");
            try {
                loadPageFromDisk(page_id);
                return cache[page_id].data;
            } catch (const std::exception& e) {
                // std::cerr << "Failed to load page " << page_id << " from disk: " << e.what() << std::endl;
                // util::log(util::ERROR, "BufferManager: Failed to load page ", page_id, " from disk: ", e.what());
                throw std::runtime_error("Failed to load page from disk: " + std::string(e.what()));
            }
        }
    }


    // 写入页面
    void writePage(int page_id, const std::vector<char>& data) {
        // std::cout << YELLOW << "[BufferManager--DEBUG] Writing page " << page_id  << RESET << std::endl;
        // util::log(util::DEBUG, "BufferManager: Writing page ", page_id);
        // std::lock_guard<std::mutex> lock(mutex);
        // 如果页面在缓存中，直接写入
        if (cache.find(page_id) != cache.end()) {
            // std::cout << YELLOW << "[BufferManager--DEBUG] Page " << page_id << " updated in cache."  << RESET << std::endl;
            // util::log(util::DEBUG, "BufferManager: Page ", page_id, " updated in cache.");
            cache[page_id] = {data, true};
            touch(page_id);
            return;
        }
        // 如果页面不在缓存中，新建一个页面
        if (cache.size() == capacity) {
            // std::cout << YELLOW << "[BufferManager--DEBUG] Cache is full. Evicting a page."  << RESET << std::endl;
            // util::log(util::DEBUG, "BufferManager: Cache is full. Evicting a page.");
            evictPage(); // 如果缓存满了，先移除一个页面
        }
        cache[page_id] = {data, true};
        lru.push_front(page_id);
        // 如果页面是新建的，需要写入磁盘
        // writePageToDisk(page_id, data);
        if (page_id == file.tellg() / PAGE_SIZE) {
            // std::cout << YELLOW << "[BufferManager--DEBUG] Writing new page " << page_id << " to disk."  << RESET << std::endl;
            // util::log(util::DEBUG, "BufferManager: Writing new page ", page_id, " to disk.");
            writePageToDisk(page_id, data);
        }

        // std::cout << YELLOW << "[BufferManager--DEBUG] Page " << page_id << " added to cache. Cache's size = " << cache.size() << RESET << std::endl;
        // util::log(util::DEBUG, "BufferManager: Page ", page_id, " added to cache. Cache's size = ", cache.size());
    }

private:
    struct CacheEntry {
        std::vector<char> data;
        bool isDirty;
    };

    int capacity;
    std::list<int> lru; // 用于LRU页面替换策略的列表
    std::unordered_map<int, CacheEntry> cache; // 页面ID到CacheEntry的映射
    std::fstream& file;
    // std::mutex mutex; // 线程安全锁

    // 将页面移动到LRU列表的前端
    void touch(int page_id) {
        lru.remove(page_id);
        lru.push_front(page_id);
    }

    // 移除一个缓存
    void evictPage() {
        int old_page_id = lru.back();
        if (cache[old_page_id].isDirty) {
            // 如果是脏页，需要先写回磁盘
            writePageToDisk(old_page_id, cache[old_page_id].data);
        }
        cache.erase(old_page_id);
        lru.pop_back();
    }

    // 从磁盘加载页面
    void loadPageFromDisk(int page_id) {
        std::vector<char> data(PAGE_SIZE);
        file.seekg(page_id * PAGE_SIZE);
        if (!file.read(data.data(), PAGE_SIZE)) {
            throw std::runtime_error("Failed to read page from disk");
        }
        if (cache.size() == capacity) {
            // std::cout << YELLOW << "[BufferManager--DEBUG] Cache is full. Evicting a page."  << RESET << std::endl;
            // util::log(util::DEBUG, "BufferManager: Cache is full. Evicting a page.");
            evictPage(); // 如果缓存满了，先移除一个页面
        }
        cache[page_id] = {data, false};
        lru.push_front(page_id);
    }

    // 将页面写回磁盘
    void writePageToDisk(int page_id, const std::vector<char>& data) {
        file.seekp(page_id * PAGE_SIZE);
        if (!file.write(data.data(), PAGE_SIZE)) {
            throw std::runtime_error("Failed to write page to disk");
        }
        cache[page_id].isDirty = false;
    }
};

#endif // BUFFERMANAGER_H
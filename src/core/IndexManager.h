/**
 * IndexManager.h
 * @author why
 * @date 2023.12.28
 * @brief 管理索引
*/

#include <ios>
#include <sstream>
#include <string>
#include <queue>
#include <fstream>
#include "BufferManager.h"
#include <cstring>

#define PATH_PREFIX "../data/"
#define MAX_NAME_LEN 20
#define INDEX_VERSION 1
#define KEY_SIZE 4
#define VALUE_SIZE 4
#define TREE_ORDER 4

#include <cstring>
#include <vector>
#include <queue>
#include "../util/util.h"

struct IndexMetadataPage {
    static const std::size_t MaxIndexNameLength = 255; // 索引名的最大长度

    uint32_t indexVersion; // 索引的版本
    uint32_t rootPageId; // B+树根节点的页号
    uint32_t treeOrder; // B+树的阶数
    uint32_t keySize; // 键的大小
    uint32_t valueSize; // 值的大小
    std::queue<int> freePages; // 空闲页队列
    bool isValid; // 元数据页是否有效

    IndexMetadataPage()
        : indexVersion(INDEX_VERSION), rootPageId(0), treeOrder(TREE_ORDER), keySize(KEY_SIZE), 
          valueSize(VALUE_SIZE), isValid(false) {
    }

    // 将元数据页的内容序列化为字节流
    void serialize(std::vector<char>& buffer) {
        buffer.clear();
        
        buffer.reserve(sizeof(indexVersion) + sizeof(rootPageId) + sizeof(treeOrder) +
                       sizeof(keySize) + sizeof(valueSize) + sizeof(size_t) + 
                       freePages.size() * sizeof(int) + sizeof(isValid));

        // serializeIntegral(buffer, indexVersion);
        util::serializeIntegral(buffer, indexVersion);
        util::serializeIntegral(buffer, rootPageId);
        util::serializeIntegral(buffer, treeOrder);
        util::serializeIntegral(buffer, keySize);
        util::serializeIntegral(buffer, valueSize);


        size_t queueSize = freePages.size();
        util::serializeIntegral(buffer, queueSize);

        std::queue<int> tempQueue = freePages; // 复制一份空闲页队列
        while (!tempQueue.empty()) {
            int pageId = tempQueue.front();
            util::serializeIntegral(buffer, pageId);
            tempQueue.pop();
        }

        buffer.push_back(static_cast<char>(isValid)); // static_cast<char>将bool转换为char
    }

    // 从字节流反序列化元数据页的内容
    void deserialize(const std::vector<char>& buffer) {
        size_t pos = 0;

        indexVersion = util::deserializeIntegral<uint32_t>(buffer, pos);
        rootPageId = util::deserializeIntegral<uint32_t>(buffer, pos);
        treeOrder = util::deserializeIntegral<uint32_t>(buffer, pos);
        keySize = util::deserializeIntegral<uint32_t>(buffer, pos);
        valueSize = util::deserializeIntegral<uint32_t>(buffer, pos);

        size_t queueSize = util::deserializeIntegral<size_t>(buffer, pos);

        for (size_t i = 0; i < queueSize; ++i) {
            int pageId = util::deserializeIntegral<int>(buffer, pos);
            freePages.push(pageId);
        }

        isValid = buffer[pos++] != 0;
    }

};

struct IndexPage {
    bool dirty; // 是否被修改过
    std::vector<uint32_t> keys; // 当前节点的key
    std::vector<uint32_t> values; // 当前节点的value, 只有叶子节点才有
    std::vector<uint32_t> childrenPageIds; // 子节点的页号，而不是指针
    uint32_t parentPageId; // 父节点的页号
    uint32_t nextPageId; // 下一个节点的页号，只有叶子节点才有
    uint32_t prevPageId; // 上一个节点的页号，只有叶子节点才有
    uint32_t pageId; // 当前节点的页号
    uint32_t size; // 键的数量
    bool isLeaf; // 是否是叶子节点

    IndexPage() : dirty(true), parentPageId(0), nextPageId(0), prevPageId(0), pageId(0), size(0), isLeaf(false) {
    }

    IndexPage(const std::vector<char>& buffer) {
        deserialize(buffer);
    }

    ~IndexPage() {
    }

    // 将页面的内容序列化为字节流
    void serialize(std::vector<char>& buffer) {
        buffer.clear();
        buffer.reserve(sizeof(dirty) + sizeof(size) + sizeof(parentPageId) + sizeof(nextPageId) +
                       sizeof(prevPageId) + sizeof(pageId) + sizeof(isLeaf) +
                       keys.size() * sizeof(uint32_t) + values.size() * sizeof(uint32_t) +
                       childrenPageIds.size() * sizeof(uint32_t));

        util::serializeIntegral(buffer, size);
        util::serializeIntegral(buffer, parentPageId);
        util::serializeIntegral(buffer, nextPageId);
        util::serializeIntegral(buffer, prevPageId);
        util::serializeIntegral(buffer, pageId);
        buffer.push_back(static_cast<char>(isLeaf));

        util::serializeIntegral(buffer, keys.size());
        for (size_t i = 0; i < keys.size(); ++i) {
            util::serializeIntegral(buffer, keys[i]);
        }

        util::serializeIntegral(buffer, values.size());
        for (size_t i = 0; i < values.size(); ++i) {
            util::serializeIntegral(buffer, values[i]);
        }

        util::serializeIntegral(buffer, childrenPageIds.size());
        for (size_t i = 0; i < childrenPageIds.size(); ++i) {
            util::serializeIntegral(buffer, childrenPageIds[i]);
        }
    }

    // 从字节流反序列化页面的内容
    void deserialize(const std::vector<char>& buffer) {
        size_t pos = 0;

        dirty = false;
        size = util::deserializeIntegral<uint32_t>(buffer, pos);
        parentPageId = util::deserializeIntegral<uint32_t>(buffer, pos);
        nextPageId = util::deserializeIntegral<uint32_t>(buffer, pos);
        prevPageId = util::deserializeIntegral<uint32_t>(buffer, pos);
        pageId = util::deserializeIntegral<uint32_t>(buffer, pos);
        isLeaf = buffer[pos++] != 0;

        uint32_t keySize = util::deserializeIntegral<uint32_t>(buffer, pos);
        keys.resize(keySize);
        for (size_t i = 0; i < keySize; ++i) {
            keys[i] = util::deserializeIntegral<uint32_t>(buffer, pos);
        }

        uint32_t valueSize = util::deserializeIntegral<uint32_t>(buffer, pos);
        values.resize(valueSize);
        for (size_t i = 0; i < valueSize; ++i) {
            values[i] = util::deserializeIntegral<uint32_t>(buffer, pos);
        }

        uint32_t childrenSize = util::deserializeIntegral<uint32_t>(buffer, pos);
        childrenPageIds.resize(childrenSize);
        for (size_t i = 0; i < childrenSize; ++i) {
            childrenPageIds[i] = util::deserializeIntegral<uint32_t>(buffer, pos);
        }
    }

};

class IndexManager {
public:
    IndexManager(const std::string& table_name, int cache_size) 
    : filename(PATH_PREFIX + table_name + ".idx"), cacheSize(cache_size) {
        openFile(file, filename);
    }
    ~IndexManager() {
        flushCache();
    }

    // 删除一个页面 需要将页面加入空闲页队列
    void deletePage(int page_id) {
        metaPage.freePages.push(page_id);
    }

    // 写入一个新页面

    // 加入新节点
    void insertNode(uint32_t key, u_int32_t value) {

    }

    // 删除节点
    void deleteNode(uint32_t key) {

    }

    // 查找节点
    IndexPage findNode(uint32_t key) {

    }


private:
    std::string filename;
    std::fstream file;
    IndexMetadataPage metaPage;
    std::vector<char> metaPageData;
    IndexPage rootPage;
    std::unordered_map<int, std::pair<IndexPage, std::list<int>::iterator>> cache; // 页面缓存
    std::list<int> lruKeys; // 用于跟踪最近最少使用的页号
    size_t cacheSize; // 缓存大小

    void flushCache() {
        for (auto& pair : cache) {
            int page_id = pair.first;
            IndexPage& page = pair.second.first;
            std::vector<char> page_data;
            page.serialize(page_data);
            writePageToDisk(page_id, page_data);
        }
        util::log(util::DEBUG, "IndexManager: flushed cache");
        metaPage.serialize(metaPageData);
        writePageToDisk(0, metaPageData);
        util::log(util::DEBUG, "IndexManager: flushed metadata page");
    }

    IndexPage loadPageFromDisk(int page_id) {
        util::log(util::DEBUG, "IndexManager: loading page ", page_id, " from disk");
        std::vector<char> page_data(PAGE_SIZE, 0);
        file.seekg(page_id * PAGE_SIZE);
        file.read(page_data.data(), PAGE_SIZE);
        IndexPage page(page_data);
        return page;
    }

    void evictPage() {
        if (lruKeys.empty()) return;

        int oldPageId = lruKeys.back();
        std::vector<char> pageData;
        cache[oldPageId].first.serialize(pageData);
        writePageToDisk(oldPageId, pageData);
        lruKeys.pop_back();
        cache.erase(oldPageId);
    }

    IndexPage& getPage(int page_id) {
        util::log(util::DEBUG, "IndexManager: getting page ", page_id);
        auto it = cache.find(page_id);
        if (it != cache.end()) {
            // 如果找到页面，将其移动到LRU列表的前端
            lruKeys.splice(lruKeys.begin(), lruKeys, it->second.second);
            return it->second.first;
        }

        // 加载页面
        IndexPage newPage = loadPageFromDisk(page_id);

        // 如果缓存已满，移除最近最少使用的项
        if (cache.size() >= cacheSize) {
            evictPage();
        }

        // 添加新页面到缓存和LRU列表
        auto newIt = cache.emplace(page_id, std::make_pair(std::move(newPage), lruKeys.begin())).first;
        lruKeys.push_front(page_id);
        newIt->second.second = lruKeys.begin();
        return newIt->second.first;
    }

    void writePage(int page_id, IndexPage& page) {
        // 如果页面在缓存中，直接写入
        if (cache.find(page_id) != cache.end()) {
            cache[page_id].first = page;
            lruKeys.splice(lruKeys.begin(), lruKeys, cache[page_id].second); // 将页面移动到LRU列表的前端
            return;
        }

        // 如果页面不在缓存中，新建一个页面

        // 如果页面是新建的，写入磁盘
        if (page_id == file.tellg() / PAGE_SIZE) {
            std::vector<char> page_data;
            page.serialize(page_data);
            writePageToDisk(page_id, page_data);
        }
    }

    void writePageToDisk(int page_id, const std::vector<char>& data) {
        file.seekp(page_id * PAGE_SIZE);
        file.write(data.data(), data.size());
    }

    void split(IndexPage& node) {

    }

    IndexPage& find_leaf(uint32_t key) {

    }

    void remove_from_leaf(IndexPage& node, uint32_t key) {

    }

    void borrow_from_left_leaf(IndexPage& node, int i) {

    }

    void borrow_from_right_leaf(IndexPage& node, int i) {

    }

    void merge_leaf(IndexPage& left, IndexPage& right, int i) {

    }

    void adjust(IndexPage& node) {

    }

    void borrow_from_left(IndexPage& node, int i) {

    }

    void borrow_from_right(IndexPage& node, int i) {

    }

    void merge(IndexPage& left, IndexPage& right, int i) {

    }



    // 打开文件
    void openFile(std::fstream& file, const std::string& filename) {
        util::log(util::DEBUG, "IndexManager: opening file: ", filename);
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if(file.is_open()) {
            loadMetaPage();
            loadRootPage();
        } else {
            createFile();
        }
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }
    }

    // 创建文件并写入元数据页，根节点页
    void createFile() {
        util::log(util::DEBUG, "IndexManager: creating file: ", filename);
        // 清除所有错误标志
        file.clear();

        // 创建文件
        file.open(filename, std::ios::out | std::ios::binary);
        if (!file) {
            throw std::runtime_error("Unable to create file: " + filename);
        }

        // 填充4K的元数据区域
        metaPage = IndexMetadataPage();
        metaPage.rootPageId = 1;
        std::vector<char> metadata(PAGE_SIZE); // 使用'\0'初始化元数据区
        metaPage.serialize(metadata);
        if (!file.write(metadata.data(), PAGE_SIZE)) {
            throw std::runtime_error("Failed to write metadata to file: " + filename);
        }

        // 填充4K的根节点区域
        rootPage = IndexPage();
        std::vector<char> root(PAGE_SIZE);
        rootPage.serialize(root);
        if (!file.write(root.data(), PAGE_SIZE)) {
            throw std::runtime_error("Failed to write root page to file: " + filename);
        }

        // 关闭并重新以读写模式打开文件
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) {
            throw std::runtime_error("Unable to open file after metadata write: " + filename);
        }
    }

    void loadMetaPage() {
        util::log(util::DEBUG, "IndexManager: loading metadata page");
        metaPageData.resize(PAGE_SIZE);
        file.seekg(0);
        file.read(metaPageData.data(), PAGE_SIZE);
        metaPage.deserialize(metaPageData);
        util::log(util::DEBUG, "IndexManager: metadata page's root page id: ", metaPage.rootPageId);
        util::log(util::DEBUG, "IndexManager: metadata page's tree order: ", metaPage.treeOrder);
    }

    void loadRootPage() {
        util::log(util::DEBUG, "IndexManager: loading root page");
        rootPage = getPage(metaPage.rootPageId);
        util::log(util::DEBUG, "IndexManager: root page's size: ", rootPage.size);
        util::log(util::DEBUG, "IndexManager: root page's parent page id: ", rootPage.parentPageId);
    }

    // 新建一个页面
    int newPage() {
        int page_id;
        if (metaPage.freePages.empty()) {
            // 没有空闲页，新建一个
            file.seekg(0, std::ios::end);
            page_id = file.tellg()/ PAGE_SIZE; // tellg() 返回当前位置
        } else {
            // 有空闲页，从空闲页队列中取出
            page_id = metaPage.freePages.front();
            metaPage.freePages.pop();
        }
        // metaPage.rowNum++;
        return page_id;
    }
};


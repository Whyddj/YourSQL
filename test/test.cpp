//test.cpp
#include <iostream>
#include "../src/core/BPlusTree.h"
// #include "../src/core/DiskManager.h"
// #include "../src/core/IndexManager.h"
// #include "../src/core/DatabaseOperations.h"
#include <chrono>
#include <fstream>

using namespace std;
// 一些单元测试
void insertTest();
void InsertRomoveTest();
void randomInsertTest();
void searchTest();
void testReadWrite();
void IndexManagerTest();
void testSerize();
void databaseOpTest();

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    // insertTest();
    // randomInsertTest();
    // insertTest2();
    // searchTest();

    // testReadWrite();
    // IndexManagerTest();
    testSerize();
    // InsertRomoveTest();
    // databaseOpTest();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by function: "
              << duration.count() << " microseconds" << std::endl;

    

    return 0;
}

// void databaseOpTest() {
//     DatabaseOperations* databaseOperations = new DatabaseOperations();
//     databaseOperations->createDatabase("test");
//     databaseOperations->useDatabase("test");
//     databaseOperations->createDatabase("test2");
//     databaseOperations->useDatabase("test2");
//     databaseOperations->dropDatabase("test");
//     databaseOperations->createDatabase("test3");
//     databaseOperations->createDatabase("test4");
//     databaseOperations->showDatabases();
// }

void testSerize() {
    BPlusTree<uint32_t, uint32_t> tree(100);
    // 插入一些数据
    srand(time(NULL));
    for (int i = 0; i < 221088; i++) {
        int key = rand() % 1000000;
        tree.insert(key, key);
    }
    tree.insert(100, 100);

    // 序列化树到文件
    std::ofstream outFile("bplustree.dat", std::ios::binary);
    tree.serialize(outFile);
    outFile.close();
    // tree.print();

    // 创建一个新的 B+树实例来进行反序列化
    BPlusTree<uint32_t, uint32_t> newTree(100);

    // 从文件反序列化树
    std::ifstream inFile("bplustree.dat", std::ios::binary);
    newTree.deserialize(inFile);
    inFile.close();
    // newTree.print();

    auto start = std::chrono::high_resolution_clock::now();
    // 测试搜索
    std::cout << newTree.search(100) << std::endl;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop-start);
    std::cout << "Time taken by search 100: "
              << duration.count() << " microseconds" << std::endl;
}

// void IndexManagerTest() {
//     IndexManager indexManager = IndexManager("test_idx", 2);
// }

// void testReadWrite() {
//     std::string tableName;
//     int cacheSize = 2; // 假设的缓存大小
//     std::cout << "Enter file name for Disk Manager: ";
//     std::cin >> tableName;

//     DiskManager diskManager(tableName, cacheSize);

//     bool running = true;
//     while (running) {
//         std::cout << "Choose an option:\n";
//         std::cout << "1. Write new page\n";
//         std::cout << "2. Read a page\n";
//         std::cout << "3. Write a specific page\n";
//         std::cout << "4. delete a specific page\n";
//         std::cout << "5. Exit\n";
//         std::cout << "Enter choice (1-4): ";

//         int choice;
//         std::cin >> choice;

//         switch (choice) {
//             case 1: {
//                 std::string inputString;
//                 std::cout << "Enter content to write on the page: ";
//                 std::cin.ignore(); // 忽略前一个换行符
//                 std::getline(std::cin, inputString); // 读入整行作为字符串

//                 // 创建一个大小为4096的向量，并用输入的字符串初始化它
//                 std::vector<char> writeData(4096, '\0'); // 初始填充为空字符
//                 std::copy(inputString.begin(), inputString.end(), writeData.begin());

//                 diskManager.writeNewPage(writeData);
//                 std::cout << "Page written with provided content.\n";
//                 break;
//             }
//             case 2: {
//                 int pageId;
//                 std::cout << "Enter page ID to read: ";
//                 std::cin >> pageId;
//                 try {
//                     auto readData = diskManager.readPage(pageId);
//                     std::cout << "Data on page " << pageId << ": [";
//                     for (char c : readData) std::cout << c;
//                     std::cout << "]\n";
//                 } catch (const std::exception& e) {
//                     std::cout << "Failed to read page: " << e.what() << "\n";
//                 }
//                 break;
//             }
//             case 3: {
//                 int pageId;
//                 std::cout << "Enter page ID to write: ";
//                 std::cin >> pageId;
//                 std::string inputString;
//                 std::cout << "Enter content to write on the page: ";
//                 std::cin.ignore(); // 忽略前一个换行符
//                 std::getline(std::cin, inputString); // 读入整行作为字符串


//                  // 创建一个大小为4096的向量，并用输入的字符串初始化它

//                 std::vector<char> writeData(4096, '\0'); // 初始填充为空字符
//                 std::copy(inputString.begin(), inputString.end(), writeData.begin());

//                 diskManager.updatePage(pageId, writeData);
//                 std::cout << "Page written with provided content.\n";
//                 break;
//             }

//              case 4: {
//                 int pageId;
//                 std::cout << "Enter page ID to delete: ";
//                 std::cin >> pageId;
//                 diskManager.deletePage(pageId);
//                 std::cout << "Page " << pageId << " deleted.\n";
//                 break;
//             }
//             case 5:

//                 running = false;
//                 break;
//             default:
//                 std::cout << "Invalid choice. Please try again.\n";
//                 break;
//         }
//     }
// }

void searchTest() {
    BPlusTree<int, string> tree(3, {{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}, {5, "e"}, {6, "f"}, {7, "g"}, {8, "h"}, {9, "i"}, {10, "j"}, {11, "k"}, {12, "l"}, {13, "m"}, {14, "n"}, {15, "o"}, {16, "p"}, {17, "q"}, {18, "r"}, {19, "s"}, {20, "t"}, {21, "u"}, {22, "v"}, {23, "w"}, {24, "x"}, {25, "y"}, {26, "z"}});
    std::cout << tree.search(12) << std::endl;
    std::cout << tree.search(13) << std::endl;
    std::cout << tree.search(26) << std::endl;
}

void insertTest() {
    BPlusTree<int, int> tree(100);
    // 生成一些随机数插入
    srand(time(NULL));
    for (int i = 0; i < 500; i++) {
        int key = rand() % 1000;
        tree.insert(key, key);
    }



    tree.print();
}

void InsertRomoveTest() {
    BPlusTree<int, int> tree(3, {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {7, 7}, {8, 8}, {9, 9}, {10, 10}});
    tree.remove(4);
    tree.remove(2);
    tree.remove(9);
}
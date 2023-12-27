//test.cpp
#include <iostream>
#include "BPlusTree.h"
#include "DiskManager.h"

using namespace std;

void insertTest();
void randomInsertTest();
void insertTest2();
void searchTest();

void testReadWrite();

int main() {
    // insertTest();
    // randomInsertTest();
    // insertTest2();
    // searchTest();

    testReadWrite();
    return 0;
}

void testReadWrite() {
    std::string tableName;
    int cacheSize = 2; // 假设的缓存大小
    std::cout << "Enter file name for Disk Manager: ";
    std::cin >> tableName;

    DiskManager diskManager(tableName, cacheSize);

    bool running = true;
    while (running) {
        std::cout << "Choose an option:\n";
        std::cout << "1. Write new page\n";
        std::cout << "2. Read a page\n";
        std::cout << "3. Write a specific page\n";
        std::cout << "4. delete a specific page\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter choice (1-4): ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string inputString;
                std::cout << "Enter content to write on the page: ";
                std::cin.ignore(); // 忽略前一个换行符
                std::getline(std::cin, inputString); // 读入整行作为字符串

                // 创建一个大小为4096的向量，并用输入的字符串初始化它
                std::vector<char> writeData(4096, '\0'); // 初始填充为空字符
                std::copy(inputString.begin(), inputString.end(), writeData.begin());

                diskManager.writeNewPage(writeData);
                std::cout << "Page written with provided content.\n";
                break;
            }
            case 2: {
                int pageId;
                std::cout << "Enter page ID to read: ";
                std::cin >> pageId;
                try {
                    auto readData = diskManager.readPage(pageId);
                    std::cout << "Data on page " << pageId << ": [";
                    for (char c : readData) std::cout << c;
                    std::cout << "]\n";
                } catch (const std::exception& e) {
                    std::cout << "Failed to read page: " << e.what() << "\n";
                }
                break;
            }
            case 3: {
                int pageId;
                std::cout << "Enter page ID to write: ";
                std::cin >> pageId;
                std::string inputString;
                std::cout << "Enter content to write on the page: ";
                std::cin.ignore(); // 忽略前一个换行符
                std::getline(std::cin, inputString); // 读入整行作为字符串

                 // 创建一个大小为4096的向量，并用输入的字符串初始化它
                std::vector<char> writeData(4096, '\0'); // 初始填充为空字符
                std::copy(inputString.begin(), inputString.end(), writeData.begin());

                diskManager.updatePage(pageId, writeData);
                std::cout << "Page written with provided content.\n";
                break;
            }
             case 4: {
                int pageId;
                std::cout << "Enter page ID to delete: ";
                std::cin >> pageId;
                diskManager.deletePage(pageId);
                std::cout << "Page " << pageId << " deleted.\n";
                break;
            }
            case 5:
                running = false;
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
}

void searchTest() {
    BPlusTree<int, string> tree(3, {{1, "a"}, {2, "b"}, {3, "c"}, {4, "d"}, {5, "e"}, {6, "f"}, {7, "g"}, {8, "h"}, {9, "i"}, {10, "j"}, {11, "k"}, {12, "l"}, {13, "m"}, {14, "n"}, {15, "o"}, {16, "p"}, {17, "q"}, {18, "r"}, {19, "s"}, {20, "t"}, {21, "u"}, {22, "v"}, {23, "w"}, {24, "x"}, {25, "y"}, {26, "z"}});
    std::cout << tree.search(12) << std::endl;
    std::cout << tree.search(13) << std::endl;
    std::cout << tree.search(26) << std::endl;
}

void insertTest2() {
    BPlusTree<int, int> tree(3);
    tree.insert(1, 1);
    tree.print();
    tree.insert(2, 2);
    tree.print();
    tree.insert(3, 3);
    tree.print();
    tree.insert(4, 4);
    tree.print();
    tree.insert(5, 5);
    tree.print();
    tree.insert(6, 6);
    tree.print();
    tree.insert(7, 7);
    tree.print();
    tree.insert(8, 8);
    tree.print();
    tree.insert(9, 9);
    tree.print();
    tree.insert(10, 10);
    tree.print();
    tree.remove(6);
    tree.print();
    tree.remove(5);
    tree.print();
    tree.remove(4);
    tree.print();
    tree.remove(3);
    tree.print();
    tree.remove(2);
    tree.print();
    tree.remove(1);
    tree.print();
    tree.insert(11, 11);
    tree.print();
    tree.insert(12, 12);
    tree.print();
    tree.insert(13, 13);
    tree.print();
    tree.insert(14, 14);
    tree.print();
    tree.insert(15, 15);
    tree.print();
    tree.insert(16, 16);
    tree.insert(17, 17);
    tree.insert(18, 18);
    tree.insert(19, 19);
    tree.insert(20, 20);
    tree.insert(21, 21);
    tree.insert(22, 22);
    tree.insert(23, 23);
    tree.insert(24, 24);
    tree.insert(25, 25);
    tree.insert(26, 26);
    tree.insert(27, 27);
    tree.insert(28, 28);
    tree.insert(29, 29);
    tree.insert(30, 30);
    tree.insert(31, 31);
    tree.insert(32, 32);
    tree.insert(33, 33);
    tree.insert(34, 34);
    tree.insert(35, 35);
    tree.insert(36, 36);
    tree.insert(37, 37);
    tree.insert(38, 38);
    tree.insert(39, 39);
    tree.insert(40, 40);
    tree.insert(41, 41);
    tree.insert(42, 42);
    tree.insert(43, 43);
    tree.insert(44, 44);
    tree.insert(45, 45);
    tree.insert(46, 46);
    tree.insert(47, 47);
    tree.insert(48, 48);
    tree.insert(49, 49);
    tree.insert(50, 50);
    tree.insert(51, 51);
    tree.insert(52, 52);
    tree.insert(53, 53);
    tree.insert(54, 54);

    tree.print();
}

void insertTest() {
    BPlusTree<int, int> tree(3);
    tree.insert(1, 1);
    tree.print();
    tree.insert(2, 2);
    tree.print();
    tree.insert(3, 3);
    tree.print();
    tree.insert(4, 4);
    tree.print();
    tree.insert(5, 5);
    tree.print();
    tree.insert(6, 6);
    tree.print();
    tree.insert(7, 7);
    tree.print();
    tree.insert(8, 8);
    tree.print();
    tree.insert(9, 9);
    tree.print();
    tree.insert(10, 10);
    tree.print();
}

void randomInsertTest() {
    BPlusTree<int, int> tree(3);
    tree.insert(1, 1);
    tree.print();
    tree.insert(5, 2);
    tree.print();
    tree.insert(9, 3);
    tree.print();
    tree.insert(4, 4);
    tree.print();
    tree.insert(43, 5);
    tree.print();
    tree.insert(6, 6);
    tree.print();
    tree.insert(7, 7);
    tree.print();
    tree.insert(14, 8);
    tree.print();
    tree.insert(22, 9);
    tree.print();
    tree.insert(10, 10);
    tree.print();
    // tree.insert(11, 11);
    // tree.print();
    // tree.insert(12, 12);
    // tree.print();
    // tree.insert(10, 13);
    // tree.print();
    // tree.insert(14, 14);
    // tree.print();
    // tree.insert(15, 15);
    // tree.print();
    // tree.insert(16, 16);
    // tree.print();
    // tree.insert(17, 17);
    // tree.print();
    // tree.insert(18, 18);
    // tree.print();
    // tree.insert(19, 19);
    // tree.print();
    // tree.insert(32, 20);
    // tree.print();
    // tree.insert(21, 21);
    // tree.print();
    // tree.insert(22, 22);
    // tree.print();
    // tree.insert(23, 23);
    // tree.print();
    // tree.insert(2, 24);
    // tree.print();
    // tree.insert(25, 25);
    // tree.print();
    // tree.insert(6, 26);
    // tree.print();
    // tree.insert(27, 27);
    // tree.print();
    // tree.insert(14, 28);
    // tree.print();
    // tree.insert(29, 29);
    // tree.print();
}
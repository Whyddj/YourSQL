/**
 * @file BPlusTree.h
 * @author why
 * @brief B+树的实现
 * @version 0.1
 * @date 2023-12-3
 *
 *
 */



#include <vector>
#include <iostream>

/**
 * @brief B+树的实现
*/
template <typename key_t, typename value_t>
class BPlusTree {
public:
    static constexpr int min_degree = 3; // 最小阶数

    /**
     * @brief B+树默认构造函数
    */
    BPlusTree(int degree) : degree(degree), root(new Node(true)) {
        if (degree < min_degree) {
            throw std::invalid_argument("Degree must be greater than 2");
        }
    }
    
    /**
     * @brief B+树带有初始化列表的构造函数
    */
    BPlusTree(int degree, std::initializer_list<std::pair<const key_t, value_t>> items) 
        : BPlusTree(degree) {
        for (const auto& [key, value] : items) { // C++17的结构化绑定
            this->insert(key, value);
        }
    }

    ~BPlusTree() {
        // removeAll();
        delete root;
    }

    /**
     * @brief 向B+树中插入一个键值对
    */
    void insert(key_t key, value_t value) {
        Node* node = this->root;

        // 找到叶子节点
        while (!node->is_leaf) {
            int i = 0;
            while (i < node->size && key > node->keys[i]) {
                i++;
            }
            node = node->children[i];
        }

        int i = 0;
        while (i < node->size && key >= node->keys[i]) {
            i++;
        }
        node->keys.insert(node->keys.begin() + i, key); // begin()返回的是迭代器，指向第一个元素的位置
        node->values.insert(node->values.begin() + i, value);
        node->size++;

        if (node->size == this->degree) {
            this->split(node);
        }
    }

    /**
     * @brief 从B+树中删除一个键值对
    */
    void remove(key_t key) {
        auto leaf = find_leaf(key);

        remove_from_leaf(leaf, key);

        // 如果是根节点，直接返回
        if (leaf == root) {
            return;
        }

        if (leaf->size >= degree / 2) {
            return;
        }

        int i = 0;
        while (i < leaf->parent->size + 1 && leaf->parent->children[i] != leaf) {
            i++;
        }
        // 如果左兄弟节点的大小大于阶数的一半，从左兄弟节点借一个key
        if (i > 0 && leaf->parent->children[i - 1]->size > degree / 2) {
            borrow_from_left_leaf(leaf, i);
        }
        // 如果右兄弟节点的大小大于阶数的一半，从右兄弟节点借一个key
        else if (i + 1 < leaf->parent->size + 1 && leaf->parent->children[i + 1]->size > degree / 2) {
            borrow_from_right_leaf(leaf, i);
        }
        // 否则合并
        else {
            if (i > 0) {
                // 防止 heap-use-after-free
                auto sibling = leaf->parent->children[i - 1];
                merge_leaf(sibling, leaf, i - 1);
                leaf = sibling;
            }
            else {
                merge_leaf(leaf, leaf->parent->children[i + 1], i);
            }
        }

        if (leaf->parent == root) {
            if (leaf->parent->size == 0) {
                root = leaf->parent->children[0];
                delete leaf->parent;
            } else return;
        }
        else if (leaf->parent->size + 1 < (degree + 1) / 2) {
            adjust(leaf->parent);
        }

    }

    /**
     * @brief 从B+树中删除所有键值对
    */
    void removeAll(){
        auto node = this->root;
        while (!node->is_leaf) {
            node = node->children[0];
        }
        while (node) {
            auto next = node->next;
            delete node;
            node = next;
        }
        this->root = new Node(true);
    }

    /**
     * @brief 从B+树中查找一个键值对
    */
    value_t search(const key_t& key) {
        auto node = find_leaf(key);
        int i = 0;
        while (i < node->size && key > node->keys[i]) {
            i++;
        }
        if (node->keys[i] != key) {
            // throw "Key not found";
            // return nullptr;
            throw std::invalid_argument("Key not found");
        }
        return node->values[i];
    }

    /**
     * @brief 打印B+树的结构
    */
    void print() {
        std::vector<Node*> nodes;
        nodes.push_back(this->root);
        int level = 0; // 添加一个变量来跟踪树的层级

        while (!nodes.empty()) {
            std::cout << "Level " << level << ": ";
            std::vector<Node*> next_nodes;
            for (int i = 0; i < nodes.size(); i++) {
                Node* node = nodes[i];
                // 打印节点类型（内部节点或叶子节点）
                std::cout << (node->is_leaf ? "Leaf[" : "Internal[");
                for (int j = 0; j < node->size; j++) {
                    std::cout << node->keys[j] << (j < node->size - 1 ? ", " : "");
                }
                std::cout << "] ";
                // 如果不是叶子节点，打印子节点的信息
                if (!node->is_leaf) {
                    std::cout << "-> {";
                    for (int j = 0; j < node->children.size(); j++) {
                        next_nodes.push_back(node->children[j]);
                        std::cout << "(" << node->children[j]->keys[0] << ")"; // 以子节点的第一个键作为标识
                        if (j < node->children.size() - 1) std::cout << ", ";
                    }
                    std::cout << "} ";
                }
            }
            std::cout << std::endl;
            nodes = next_nodes;
            level++; // 增加层级
        }
        std::cout << std::endl;
    }


private:
    /**
     * @brief B+树的节点定义
    */
    struct Node {
        bool is_leaf = false; // 是否是叶子节点
        int size = 0;   // 当前节点的大小, 即当前节点有多少个key
        std::vector<key_t> keys; // 当前节点的key
        std::vector<value_t> values; // 当前节点的value, 只有叶子节点才有
        std::vector<Node*> children; // 当前节点的子节点
        Node* parent = nullptr; // 当前节点的父节点
        Node* next = nullptr; // 当前节点的下一个节点，只有叶子节点才有
        Node* prev = nullptr; // 当前节点的上一个节点，只有叶子节点才有

        explicit Node(bool isLeaf = false) : is_leaf(isLeaf) {} // explicit关键字防止隐式转换
    };

    Node* root;
    int degree; // B+树的阶数，即一个节点最多有多少个子节点

    void split(Node* node){
        auto parent = node->parent;
        auto sibling = new Node(node->is_leaf);

        // 将node的后一半key和value移动到sibling中 
        sibling->keys.assign(node->keys.end() - node->size / 2, node->keys.end());
        if (node->is_leaf) {
            sibling->values.assign(node->values.end() - node->size / 2, node->values.end());
        }
        sibling->size = node->size / 2;
        node->size -= sibling->size;
        node->keys.resize(node->size);
        if (node->is_leaf) {
            node->values.resize(node->size);
        }

        // 如果node是叶子节点，还需要处理next和prev指针
        if (node->is_leaf) {
            sibling->next = node->next;
            if (node->next != nullptr) { // 如果node不是最后一个叶子节点
                node->next->prev = sibling;
            }
            node->next = sibling;
            sibling->prev = node;
        }

        // 将node的后一半children移动到sibling中
        if (!node->is_leaf) {
            sibling->children.assign(node->children.begin() + node->size, node->children.end());
            for (auto& child : sibling->children) {
                child->parent = sibling;
            }
            node->children.resize(node->size);
        }

        // 将node的中间key和value插入到parent中
        if (!parent || node == this->root) { // 如果node是根节点
            parent = new Node();
            this->root = parent;
            parent->children.push_back(node);
            node->parent = parent;
        }

        int i = 0;
        while (i < parent->size && node->keys[node->size - 1] >= parent->keys[i]) {
            i++;
        }
        parent->keys.insert(parent->keys.begin() + i, node->keys[node->size - 1]);
        // parent->values.insert(parent->values.begin() + i, node->values[node->size - 1]);
        parent->size++;
        // 如果node不是叶子节点，还需要将最后一个键值对从node中删除
        if (!node->is_leaf) {
            node->keys.pop_back();
            node->size--;
        }

        // 将sibling插入到parent中
        parent->children.insert(parent->children.begin() + i + 1, sibling);
        sibling->parent = parent;

        // 如果parent的大小超过了阶数，需要继续分裂
        if (parent->size == this->degree) {
            this->split(parent);
        }
    }
    
    Node* find_leaf(key_t key) {
        auto node = this->root;
        // 找到叶子节点
        while (!node->is_leaf) {
            int i = 0;
            while (i < node->size && key > node->keys[i]) {
                i++;
            }
            node = node->children[i];
        }
        // 如果key不在叶子节点中，抛出异常
        if (node->keys[node->size - 1] < key || node->keys[0] > key) {
            throw std::invalid_argument("Key not found");
        }
        return node;
    }

    // 从叶子节点中删除key
    void remove_from_leaf(Node* leaf, const key_t& key) {
        int i = 0;
        while (i < leaf->size && key > leaf->keys[i]) {
            i++;
        }
        if (leaf->keys[i] != key) {
            throw std::invalid_argument("Key not found in leaf");
        }
        leaf->keys.erase(leaf->keys.begin() + i);
        leaf->values.erase(leaf->values.begin() + i);
        leaf->size--;
    }

    // 叶子节点从左兄弟节点借一个key
    void borrow_from_left_leaf(Node* leaf, int i) {
        auto sibling = leaf->parent->children[i - 1];
        leaf->keys.insert(leaf->keys.begin(), sibling->keys[sibling->size - 1]);
        leaf->values.insert(leaf->values.begin(), sibling->values[sibling->size - 1]);
        leaf->size++;
        sibling->keys.erase(sibling->keys.end() - 1);
        sibling->values.erase(sibling->values.end() - 1);
        sibling->size--;
        leaf->parent->keys[i - 1] = sibling->keys[sibling->size - 1];
    }

    // 叶子节点从右兄弟节点借一个key
    void borrow_from_right_leaf(Node* leaf, int i) {
        auto sibling = leaf->parent->children[i + 1];
        leaf->keys.insert(leaf->keys.end(), sibling->keys[0]);
        leaf->values.insert(leaf->values.end(), sibling->values[0]);
        leaf->size++;
        sibling->keys.erase(sibling->keys.begin());
        sibling->values.erase(sibling->values.begin());
        sibling->size--;
        leaf->parent->keys[i] = leaf->keys[leaf->size - 1];
    }

    // 叶子节点合并 right合并到left
    void merge_leaf(Node* left, Node* right, int i) {
        left->keys.insert(left->keys.end(), right->keys.begin(), right->keys.end());
        left->values.insert(left->values.end(), right->values.begin(), right->values.end());
        left->size += right->size;
        left->next = right->next;
        if (right->next != nullptr) {
            right->next->prev = left;
        }
        left->parent->keys.erase(left->parent->keys.begin() + i);
        left->parent->children.erase(left->parent->children.begin() + i + 1);
        left->parent->size--;
        delete right;
    }

    // 调整非叶子节点
    void adjust(Node* node) {
        int i = 0;
        while (i < node->parent->size + 1 && node->parent->children[i] != node) {
            i++;
        }
        if (i > 0 && node->parent->children[i - 1]->size + 1 > (degree + 1) / 2) {
            borrow_from_left(node, i);
        }
        else if (i + 1 < node->parent->size + 1 && node->parent->children[i + 1]->size + 1 > (degree + 1) / 2) {
            borrow_from_right(node, i);
        }
        else {
            if (i > 0) {
                Node* sibling = node->parent->children[i - 1];
                merge(sibling, node, i); // 合并到左兄弟节点
                node = sibling;
            }
            else {
                merge(node, node->parent->children[i + 1], i);
            }
        }
        
        if (node->parent == root) {
            if (node->parent->size == 0) {
                root = node->parent->children[0];
                delete node->parent;
            } else return;
        }
        else if (node->parent->size + 1 < (degree + 1) / 2) {
            adjust(node->parent);
        }
    }

    // 非叶子节点从左兄弟节点借一个key
    void borrow_from_left(Node* node, int i) {
        auto sibling = node->parent->children[i - 1];
        node->keys.insert(node->keys.begin(), node->parent->keys[i - 1]);
        node->size++;
        node->parent->keys[i - 1] = sibling->keys[sibling->size - 1];
        sibling->keys.erase(sibling->keys.end() - 1);
        sibling->size--;
        
        node->children.insert(node->children.begin(), sibling->children[sibling->size]);
        node->children[0]->parent = node;
        sibling->children.erase(sibling->children.end() - 1);
    }

    // 非叶子节点从右兄弟节点借一个key
    void borrow_from_right(Node* node, int i) {
        auto sibling = node->parent->children[i + 1];
        node->keys.insert(node->keys.end(), node->parent->keys[i]);
        node->size++;
        node->parent->keys[i] = sibling->keys[0];
        sibling->keys.erase(sibling->keys.begin());
        sibling->size--;
        
        node->children.insert(node->children.end(), sibling->children[0]);
        node->children[node->size]->parent = node;
        sibling->children.erase(sibling->children.begin());
    }

    // 非叶子节点合并 right合并到left
    void merge(Node* left, Node* right, int i) {
        left->keys.insert(left->keys.end(), left->children[left->size]->keys.back()); // 将子节点的最后一个key插入到left中
        left->size++;
        left->keys.insert(left->keys.end(), right->keys.begin(), right->keys.end());
        left->children.insert(left->children.end(), right->children.begin(), right->children.end());
        for (auto& child : left->children) {
            if (child) {
                child->parent = left;
            }
        }
        left->size += right->size;
        left->parent->keys.erase(left->parent->keys.begin() + i);
        left->parent->children.erase(left->parent->children.begin() + i + 1);
        left->parent->size--;
        delete right;
    }
};
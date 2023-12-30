#ifndef BP_TREE_H
#define BP_TREE_H

#include <vector>
#include <iostream>
#include <memory>

/**
 * @brief B+树的实现，使用智能指针进行优化
*/
template <typename key_t, typename value_t>
class BPlusTree {
public:
    static constexpr int min_degree = 3; // 最小阶数

    /**
     * @brief B+树默认构造函数
    */
    BPlusTree(int degree) : degree(degree), root(std::make_unique<Node>(true)) {
        if (degree < min_degree) {
            throw std::invalid_argument("Degree must be greater than 2");
        }
    }
    
    /**
     * @brief B+树带有初始化列表的构造函数
    */
    BPlusTree(int degree, std::initializer_list<std::pair<const key_t, value_t>> items) 
        : BPlusTree(degree) {
        for (const auto& [key, value] : items) {
            this->insert(key, value);
        }
    }

    /**
     * @brief B+树从文件中读取的构造函数
    */
    BPlusTree(std::istream& in, int degree) : BPlusTree(degree) {
        key_t key;
        value_t value;
        while (in.read(reinterpret_cast<char*>(&key), sizeof(key_t)) &&
            in.read(reinterpret_cast<char*>(&value), sizeof(value_t))) {
            insert(key, value);
        }
    }

    ~BPlusTree() {
        removeAll();
    }

    void serialize(std::ostream& out) {
        Node* current = find_leftmost_leaf();
        while (current != nullptr) {
            for (int i = 0; i < current->size; ++i) {
                out.write(reinterpret_cast<const char*>(&current->keys[i]), sizeof(key_t));
                out.write(reinterpret_cast<const char*>(&current->values[i]), sizeof(value_t));
            }
            current = current->next;
        }
    }

    void deserialize(std::istream& in) {
        key_t key;
        value_t value;
        while (in.read(reinterpret_cast<char*>(&key), sizeof(key_t)) &&
            in.read(reinterpret_cast<char*>(&value), sizeof(value_t))) {
            insert(key, value);
        }
    }

    /**
     * @brief 向B+树中插入一个键值对
    */
    void insert(key_t key, value_t value) {
        Node* node = this->root.get();

        // 找到叶子节点
        while (!node->is_leaf) {
            int i = 0;
            while (i < node->size && key > node->keys[i]) {
                i++;
            }
            node = node->children[i].get();
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
        if (leaf == root.get()) {
            return;
        }

        if (leaf->size >= degree / 2) {
            return;
        }

        int i = 0;
        while (i < leaf->parent->size + 1 && leaf->parent->children[i].get() != leaf) {
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
                auto sibling = leaf->parent->children[i - 1].get();
                merge_leaf(sibling, leaf, i - 1);
                leaf = sibling;
            }
            else {
                merge_leaf(leaf, leaf->parent->children[i + 1].get(), i);
            }
        }

        if (leaf->parent == root.get()) {
            if (leaf->parent->size == 0) {
                root = std::move(leaf->parent->children[0]); // 更新根节点
            } else return;
        }
        else if (leaf->parent->size + 1 < (degree + 1) / 2) {
            adjust(leaf->parent);
        }

    }

    /**
     * @brief 从B+树中删除所有键值对
    */
    void removeAll() {
        // 通过重置根节点来删除所有节点
        root = std::make_unique<Node>(true);
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
        nodes.push_back(this->root.get());
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
                        next_nodes.push_back(node->children[j].get());
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
        std::vector<std::unique_ptr<Node>> children; // 当前节点的子节点
        Node* parent = nullptr; // 当前节点的父节点
        Node* next = nullptr; // 当前节点的下一个节点，只有叶子节点才有
        Node* prev = nullptr; // 当前节点的上一个节点，只有叶子节点才有

        explicit Node(bool isLeaf = false) : is_leaf(isLeaf) {} // explicit关键字防止隐式转换
    };

    std::unique_ptr<Node> root;
    int degree; // B+树的阶数，即一个节点最多有多少个子节点

    Node* find_leftmost_leaf() {
        Node* current = root.get(); // 从根节点开始
        while (current && !current->is_leaf) {
            // 持续向下遍历到第一个子节点，即最左侧的子节点
            current = current->children[0].get();
        }
        return current; // 返回找到的叶子节点
    }


    void split(Node* node){
        auto parent = node->parent;
        auto sibling = std::make_unique<Node>(node->is_leaf); // 使用make_unique创建新节点

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
            if (node->next != nullptr) {
                node->next->prev = sibling.get();
            }
            node->next = sibling.get();
            sibling->prev = node;
        }

        // 将node的后一半children移动到sibling中
        if (!node->is_leaf) {
            sibling->children.assign(std::make_move_iterator(node->children.begin() + node->size),
                                    std::make_move_iterator(node->children.end()));
            for (auto& child : sibling->children) {
                child->parent = sibling.get();
            }
            node->children.resize(node->size);
        }

        if (!parent || node == root.get()) {
            auto newRoot = std::make_unique<Node>(); // 创建新的根节点
            newRoot->children.push_back(std::move(root)); // 将旧的根节点作为新根节点的子节点
            node->parent = newRoot.get(); // 更新旧根节点的父指针
            root = std::move(newRoot); // 更新根指针
        }

        int i = 0;
        while (i < parent->size && node->keys[node->size - 1] >= parent->keys[i]) {
            i++;
        }
        parent->keys.insert(parent->keys.begin() + i, node->keys[node->size - 1]);
        parent->size++;

        if (!node->is_leaf) {
            node->keys.pop_back();
            node->size--;
        }

        // 将sibling插入到parent中
        sibling->parent = parent;
        parent->children.insert(parent->children.begin() + i + 1, std::move(sibling)); // 使用move插入

        // 如果parent的大小超过了阶数，需要继续分裂
        if (parent->size == this->degree) {
            this->split(parent);
        }
    }
    
    Node* find_leaf(key_t key) {
        auto node = this->root.get();
        // 找到叶子节点
        while (!node->is_leaf) {
            int i = 0;
            while (i < node->size && key > node->keys[i]) {
                i++;
            }
            node = node->children[i].get();
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
        auto sibling = leaf->parent->children[i - 1].get();
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
        auto sibling = leaf->parent->children[i + 1].get();
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
        // 合并keys和values
        left->keys.insert(left->keys.end(), right->keys.begin(), right->keys.end());
        left->values.insert(left->values.end(), right->values.begin(), right->values.end());
        left->size += right->size;

        // 更新链表指针
        left->next = right->next;
        if (right->next != nullptr) {
            right->next->prev = left;
        }

        // 更新父节点的keys和children
        left->parent->keys.erase(left->parent->keys.begin() + i);
        left->parent->children.erase(left->parent->children.begin() + i + 1);
        left->parent->size--;
        // 注意：不需要手动删除right节点，因为当我们从children中移除它时，智能指针将自动处理它的销毁
    }


    // 调整非叶子节点
    void adjust(Node* node) {
        int i = 0;
        while (i < node->parent->size + 1 && node->parent->children[i].get() != node) {
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
                Node* sibling = node->parent->children[i - 1].get();
                merge(sibling, node, i); // 合并到左兄弟节点
                node = sibling;
            }
            else {
                merge(node, node->parent->children[i + 1].get(), i);
            }
        }
        
        if (node->parent == root.get()) {
            if (node->parent->size == 0) {
                root = std::move(node->parent->children[0]); // 更新根节点
            } else return;
        }
        else if (node->parent->size + 1 < (degree + 1) / 2) {
            adjust(node->parent);
        }
    }


    // 非叶子节点从左兄弟节点借一个key
    void borrow_from_left(Node* node, int i) {
        auto& sibling = node->parent->children[i - 1];
        node->keys.insert(node->keys.begin(), node->parent->keys[i - 1]);
        node->size++;
        node->parent->keys[i - 1] = sibling->keys.back();
        sibling->keys.pop_back();
        sibling->size--;

        node->children.insert(node->children.begin(), std::move(sibling->children.back()));
        node->children.front()->parent = node;
        sibling->children.pop_back();
    }

    // 非叶子节点从右兄弟节点借一个key
    void borrow_from_right(Node* node, int i) {
        auto& sibling = node->parent->children[i + 1];
        node->keys.push_back(node->parent->keys[i]);
        node->size++;
        node->parent->keys[i] = sibling->keys.front();
        sibling->keys.erase(sibling->keys.begin());
        sibling->size--;

        node->children.push_back(std::move(sibling->children.front()));
        node->children.back()->parent = node;
        sibling->children.erase(sibling->children.begin());
    }


    // 非叶子节点合并 right合并到left
    void merge(Node* left, Node* right, int i) {
        left->keys.push_back(left->children[left->size]->keys.back());
        left->size++;
        left->keys.insert(left->keys.end(), right->keys.begin(), right->keys.end());
        left->children.insert(left->children.end(),
                            std::make_move_iterator(right->children.begin()),
                            std::make_move_iterator(right->children.end()));

        for (auto& child : left->children) {
            if (child) {
                child->parent = left;
            }
        }
        left->size += right->size;
        left->parent->keys.erase(left->parent->keys.begin() + i);
        left->parent->children.erase(left->parent->children.begin() + i + 1);
        left->parent->size--;
        // 注意：不需要手动删除right节点，智能指针将自动处理它的销毁
    }
};

#endif // BP_TREE_H
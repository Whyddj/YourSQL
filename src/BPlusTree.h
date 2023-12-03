#include <vector>
#include <iostream>

template <typename key_t, typename value_t>
class BPlusTree {
public:
    BPlusTree(int degree) {
        this->degree = degree;
        this->root = new Node(true);
    }
    ~BPlusTree() {
        delete this->root;
    }

    void insert(key_t key, value_t value) {
        Node* node = this->root;

        // 找到叶子节点
        while (!node->is_leaf) {
            int i = 0;
            while (i < node->size && key >= node->keys[i]) {
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
    void remove(key_t key);
    value_t search(key_t key);

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
    }


private:
    struct Node {
        bool is_leaf; // 是否是叶子节点
        int size;   // 当前节点的大小, 即当前节点有多少个key
        std::vector<key_t> keys; // 当前节点的key
        std::vector<value_t> values; // 当前节点的value, 只有叶子节点才有
        std::vector<Node*> children; // 当前节点的子节点
        Node* parent; // 当前节点的父节点
        Node* next; // 当前节点的下一个节点，只有叶子节点才有
        Node* prev; // 当前节点的上一个节点，只有叶子节点才有

        Node(bool is_leaf = false){
            this->is_leaf = is_leaf;
            this->size = 0;
            this->parent = nullptr;
            this->next = nullptr;
            this->prev = nullptr;
        }
        ~Node() {
            for (int i = 0; i < this->children.size(); i++) {
                delete this->children[i];
            }
        }
    };

    Node* root;
    int degree; // B+树的阶数，即一个节点最多有多少个子节点

    void split(Node* node){
        Node* parent = node->parent;
        Node* sibling = new Node(node->is_leaf);

        // 将node的后一半key和value移动到sibling中 
        sibling->keys.insert(sibling->keys.begin(), node->keys.end() - node->size / 2, node->keys.end());
        sibling->values.insert(sibling->values.begin(), node->values.end() - node->size / 2, node->values.end());
        sibling->size = node->size / 2;
        node->size -= sibling->size;
        node->keys.erase(node->keys.end() - sibling->size, node->keys.end());
        node->values.erase(node->values.end() - sibling->size, node->values.end());

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
            sibling->children.insert(sibling->children.begin(), node->children.begin() + node->size, node->children.end());
            for (int i = 0; i < sibling->children.size(); i++) {
                sibling->children[i]->parent = sibling;
            }
            node->children.erase(node->children.begin() + node->size, node->children.end());
        }

        // 将node的中间key和value插入到parent中
        if (parent == nullptr) { // 如果node是根节点
            parent = new Node();
            this->root = parent;
            parent->children.push_back(node);
        }
        int i = 0;
        while (i < parent->size && node->keys[node->size - 1] >= parent->keys[i]) {
            i++;
        }
        parent->keys.insert(parent->keys.begin() + i, node->keys[node->size - 1]);
        parent->values.insert(parent->values.begin() + i, node->values[node->size - 1]);
        parent->size++;

        // 将sibling插入到parent中
        parent->children.insert(parent->children.begin() + i + 1, sibling);
        sibling->parent = parent;

        // 如果parent的大小超过了阶数，需要继续分裂
        if (parent->size == this->degree) {
            this->split(parent);
        }
    }
    
};
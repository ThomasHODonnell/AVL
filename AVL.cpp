#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <omp.h>
#include <vector>

using namespace std;

class Node {
public:
    int key, data = 0, size = 1, height = 1, bf = 0;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;

    Node(int key) {
        this->key = key;
    }

    int nHeight() {
        if (this == nullptr) return 0;
        return this->height;
    }

    int nSize() {
        if (this == nullptr) return 0;
        return this->size;
    }
};

class AVL {
public:
    Node* root = nullptr;

    int height() {
        if (root == nullptr) return 0;
        return root->height;
    }

    int size() {
        if (root == nullptr) return 0;
        return root->size;
    }

    Node* search(int key) {
        return searchrec(key, root);
    }

    Node* searchrec(int key, Node* x) {
        if (x == nullptr) return nullptr;
        if (key == x->key) return x;
        else if (key < x->key) return searchrec(key, x->left);
        else return searchrec(key, x->right);
    }

    void insert(Node* x) {
        if (root == nullptr) root = x;
        else insertnode(x, root);
    }

    void insertnode(Node* x, Node* y) { // update height??
        if (x->key == y->key) return; // ignore duplicate key
        if (y == nullptr) return;
        if (x->key < y->key) {
            // Fill this
            if (y->left != nullptr) return insertnode(x,y->left);
            y -> left = x; 
            x -> parent = y; 
            if (y->right==nullptr) y->height = y->height+1;
        }
        if (x->key > y->key) {
            // Fill this
            if (y->right != nullptr) return insertnode(x,y->right);
            y -> right = x; 
            x -> parent = y; 
            if (y->left==nullptr) y->height = y->height+1;
        }
        fixme(x);
    }

    void fixme(Node* x) {
        //fix x if its balance factor is violated
        recalc(x); // recalculate what's new height and bf of x are
        // Uncomment one of the below two statements
    }

    void cases(Node* x) {
        //if (x == root)  return;
        // 4 cases of rotations // Fill them up
        if (x->bf == -2 && x->right->bf == -1)      rrotate(x);   // right rotation
        else if (x->bf == -2 && x->right->bf == 1)  dlrotate(x);   // dogleg left
        else if (x->bf == 2 && x->left->bf == -1)  drrotate(x);  // dogleg right
        else if (x->bf == 2 && x->left->bf == 1)   lrotate(x);  // left rotation
    }

    void Transplant(Node* x, Node* y) {
        // Put node y in place of x in the tree
        // x along with its subtree will be orphaned from the tree
        // Assumes x is not nullptr
        if (x->parent == nullptr) root = y;
        else if (x->parent->left == x) x->parent->left = y;
        else x->parent->right = y;
        if (y != nullptr) y->parent = x->parent;
    }

    void rrotate(Node* x) {
        Node* y = x->right;
        if (x == root) {
            root = y;
            x->right = y->left;
            root->left = x;
            x->parent = y;
            y->parent=nullptr;
        }
        else {
            if (x->parent->right==x) {
                x->parent->right=y;
                y->parent = x->parent; 
                x->parent = y;
                x->right=y->left;
                y->left=x;
            }
            else {
                x->parent->left=y;
                y->parent = x->parent; 
                x->parent = y;
                x->right=y->left;
                y->left = x;
            }
        }
        recalc(x);
    }

     void lrotate(Node* x) {
        Node* y = x->left;
        if (x == root) {
            root = y;
            x->left = y->right;
            root->right = x;
            x->parent = y;
            y->parent=nullptr;
        }
        else {
            if (x->parent->left==x) {
                x->parent->left=y;
                y->parent = x->parent; 
                x->parent = y;
                x->left=y->right;
                y->right=x;
            }
            else {
                x->parent->right=y;
                y->parent = x->parent; 
                x->parent = y;
                x->left=y->right;
                y->right = x;
            }
        }
        recalc(x);
    }

    void dlrotate(Node* x) {
        Node* y = x->right;
        Node* z = y->left;
        z->right=y;
        y->parent=z;
        x->right=z;
        z->parent=x;
        y->left=nullptr;
        rrotate(x);
    }

    void drrotate (Node* x) {
        Node* y = x->left;
        Node* z = y->right;
        z->left=y;
        y->parent=z;
        x->left=z;
        z->parent=x;
        y->right=nullptr;
        //recalc(y);
        lrotate(x);
    }

    Node* select(Node* x, int i) {
        // Check for the condition if x == nullptr 
        // Compare the size of the left child of x with i
        // Fill this
        if (x == nullptr) return nullptr; // update to throw exeption
        if (x->left->size >= i) return select(x->left,i);
        if (x->left->size + 1 == i) return x;
        return select(x->right,i-1-(x->left->size));
    }

    // int rank(Node* x, int key) {
    //     if(x->rank>key)
	//     	rank(key, x->left);	
	// else if(x->rank<key)
	// {	rank(key, x->right);	}
	// else if(x->rank==key)
	// {	return x->key;	}
	// else
	// 	return -1;
    // }

    Node* tallchild(Node* x) {
        if (x == nullptr) return nullptr;
        else if (x->left == nullptr) return x->right;
        else if (x->right == nullptr) return x->left;
        else if (x->left->height > x->right->height) return x->left;
        else return x->right;
    }

    void recalc(Node* x) {
        // Recalculate x's height, size, and bf 
        // Assumes its children are correctly recalculated
        if (x == nullptr) return;
        if ((x->left != nullptr) && (x->right != nullptr)) {
            x->height = 1 + max(x->left->height, x->right->height);
            x->size = 1 + x->left->size + x->right->size;
            x->bf = x->left->height - x->right->height;
        }
        else if (x->right != nullptr) { 
            x->height = 1 + x->right->height;
            x->size = 1 + x->right->size;
            x->bf = -x->right->height;
        }
        else if (x->left != nullptr) { 
            x->height = 1 + x->left->height;
            x->size = 1 + x->left->size;
            x->bf = x->left->height;
        }
        else {
            x->height = 1;
            x->size = 1;
            x->bf = 0;
        }
        // if  ((x->bf == -2 || x->bf == 2)) cases(x);   // comment out if BST
        // else 
        if (x != root) recalc(x->parent); 
        //if 
    }

    void inorder(Node* p) {
        // Helpful for debugging
        if (p == nullptr) return;
        cout << " (";
        inorder(p->left);
        cout << p->key;
        inorder(p->right);
        cout << ") ";
    }

    void displayRight (Node*b) {
            while(b!=nullptr) {
                cout << "Key:" << b->key << "\t";
                cout << "Height:" << b->height << "\t"; 
                cout << "BF:" << b->bf << "\t";
                cout << "Size:" << b->size << "\n";
                b = b->right;
                }
    }
    void displayLeft (Node*b) {
            while(b!=nullptr) {
                cout << "Key:" << b->key << "\t";
                cout << "Height:" << b->height << "\t"; 
                cout << "BF:" << b->bf << "\t";
                cout << "Size:" << b->size << "\n";
                b = b->left;
                }
    }
};

    AVL b;

int main() {

    int n = 1000000;
    auto startTime = chrono::high_resolution_clock::now();
    for (int i = 1; i <= n; i++) {
        // Uncomment one of the below two statements
        Node* p = new Node((i * 7637 + 571) % n + 1); // random inserts
        //Node* p = new Node(i); // sequential inserts
        //cout << p->key<< endl;
        b.insert(p);
    }

    auto endTime = chrono::high_resolution_clock::now();

    cout << "n = " << b.size() << " h = " << b.height();
    cout << " h/log n = " << b.height() / (log(b.size()) / log(2)) << endl;
    cout << "Total Time = " << chrono::duration_cast<chrono::microseconds>(endTime - startTime).count() / 1000.0 << " micsec";
    cout << " Avg time per ins " << chrono::duration_cast<chrono::microseconds>(endTime - startTime).count() / 1000.0 / b.size() << " micsec" << endl;
    // Above 4 lines will be suppressed (commented out for Gradescope submission)

    //Node* a = b.select(b.root, n / 2); // This; line is for Gradescope output
    //cout << a->key << endl;
    // Choose n = 10000 with sequential insert mode 
    // with AVL tree

    return 0;
}

void fixup(Node* x) {
    // Needed for delete
    // Walks up the path from the point of deletion x and fixes every node
    Node* y = x;
    while (y != nullptr) { b.fixme(y); y = y->parent; }
}

void deleteNode(Node* x) {
    /* Case 1:  x is a leaf */
    if (x->left == nullptr && x->right == nullptr) {
        b.Transplant(x, nullptr);
        fixup(x->parent);
    }
    /* Case 2:  x has one child */
    else if (x->left == nullptr) {
        b.Transplant(x, x->right);
        fixup(x->right);
    }
    else if (x->right == nullptr) {
        b.Transplant(x, x->left);
        fixup(x->left);
    }
    else {  /* Case 3: x has two children */
        Node* y = x->right;
        while (y->left != nullptr) y = y->left; // get successor
        if (y->parent == x) { // Subcase 3.1
            b.Transplant(x, y);
            y->left = x->left;
            x->left->parent = y;
            fixup(y);
        }
        else { // Subcase 3.2
            b.Transplant(y, y->right);
            b.Transplant(x, y);
            y->right = x->right; y->left = x->left;
            x->right->parent = x->left->parent = y;
            b.recalc(y);
            fixup(y->right);
        }
    }
}

void deleteKey(int key) {
    Node* x = b.search(key);
    if (x != nullptr) deleteNode(x);
}




//
//  link_chain.cpp
//
//  Created by Zongming Liu on 2022/1/20.
//

#include "link_chain.hpp"
#include "util/log_util.hpp"

namespace mn {

LinkNode::LinkNode() {
    pre_ = nullptr;
    next_ = nullptr;
}

LinkNode::LinkNode(std::vector<float> content) {
    content_ = content;
    pre_ = nullptr;
    next_ = nullptr;
}

LinkNode::~LinkNode() {
//    MLOGD("LinkNode Destruct");
}

LinkChain::LinkChain() {
    first_ = last_ = nullptr;
    length_ = 0;
}

LinkChain::~LinkChain() {
    while (this->first_) {
        LinkNode* tmp = this->first_;
        LinkNode* next = this->first_->next_;

        this->first_ = next;
        delete tmp;
    }
}

// todo: check leak;
void LinkChain::PushNode(std::vector<float> content) {
    LinkNode* node = new LinkNode(content);
    this->length_++;
    if (this->length_ == 1) {
        this->last_ = this->first_ = node;
        return;
    }
    
    LinkNode* current = this->last_;
    while (current) {
        if (this->SortNode(node, current)) {
            if (this->first_ == current) {
                current->pre_ = node;
                node->next_ = current;
                this->first_ = node;
                return;
            } else {
                current = current->pre_;
            }
        } else {
            this->InsertNode(current, node);
            if (current == this->last_) {
                this->last_ = node;
            }
            return;
        }
    }
}
    
void LinkChain::RemoveNode(LinkNode* node) {
    LinkNode* current = this->first_;
    if (this->Length() > 0 && node) {
        this->length_--;
        if (current == node) {
            this->first_ = current->next_;
            if (this->first_) {
                delete first_->pre_;
                first_->pre_ = nullptr;
            }
        } else if ((current = this->last_) == node) {
            this->last_ = current->pre_;
            if (last_) {
                delete last_->next_;
                last_->next_ = nullptr;
            }
        } else {
            LinkNode* pre = node->pre_;
            LinkNode* next = node->next_;
            pre->next_ = next;
            if (next) {
                next->pre_ = pre;
            }
            
            delete node;
        }
    } else {
        return ;
    }
}

//a -> b   to    a -> next -> b
void LinkChain::InsertNode(LinkNode* a, LinkNode* next) {
    LinkNode* b = a->next_;
    a->next_ = next;
    next->pre_ = a;
    next->next_ = b;
    if (b) {
        b->pre_ = next;
    }
}

bool LinkChain::SortNode(LinkNode* node, LinkNode* current_node) {
    auto node_context = node->GetContext();
    auto current_context = current_node->GetContext();
    return node_context[0] - current_context[0] <= 0;
}

}

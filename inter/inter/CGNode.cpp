//
//  CGNode.cpp
//  proj
//
//  Created by Chao Chen on 2014-03-28.
//  Copyright (c) 2014 Chao Chen. All rights reserved.
//

#include "CGNode.h"
#include <iostream>
using namespace std;


CGNode::CGNode(string name) : t_support(0){
    this->name = name;
}

void CGNode::setChild(CGNode *node){
    if(this->children_pair.find(node) == this->children_pair.end()){
        //cout<<"set friend: "<<node->getname()<<" to "<<this->getname()<<endl;
        this->children_pair[node] = this->children.size();
        this->children.push_back(node);
    }
}

bool CGNode::hasChildren(){
    if(!this->children.empty()){
        return true;
    }
    else{
        return false;
    }
}
void CGNode::setFriend(CGNode *node){
    if(this->friends_pair.find(node) == this->friends_pair.end()){
        this->friends_pair[node] = this->friends.size();
        this->friends.push_back(node);
        if(node->hasChildren()){
            for(int i = 0; i < node->children.size();i++){
                this->setFriend(node->children[i]);
            }
        }
    }
}

void CGNode::setParent(CGNode *parent){
    this->parent = parent;
}

int CGNode::T_Support(){
    return this->t_support;
}

int CGNode::friends_index(CGNode* node){
    map<CGNode*, unsigned long>::iterator it = this->friends_pair.find(node);
    if( it != this->friends_pair.end()){
        return (int)it->second;
    }else{
        return -1;
    }
}

void CGNode::T_Support_Increment(){
    this->t_support++;
}

string CGNode::getname(){
    return this->name;
}

vector<CGNode*> CGNode::getChildren(){
    return this->children;
}

vector<CGNode*> CGNode::getFriends(){
    return this->friends;
}
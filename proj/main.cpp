//
//  main.cpp
//  proj
//
//  Created by Chao Chen on 2014-03-28.
//  Copyright (c) 2014 Chao Chen. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <regex>
#include "CGNode.h"
using namespace std;

map<string,CGNode*> graph;
map< pair<CGNode*, CGNode*>, int > pair_map;
vector<CGNode*> CG;
bool null_func_parent;
CGNode* current_parent;

int support(CGNode* A, CGNode* B){
    map< pair<CGNode*, CGNode*>, int >::iterator it;
    pair<CGNode*, CGNode*> pair1(A,B);
    pair<CGNode*, CGNode*> pair2(B,A);
    it = pair_map.find(pair1);
    if(it == pair_map.end()){
        it = pair_map.find(pair2);
    }
    if(it == pair_map.end()){
        return 0;
    }
    else{
        return it->second;
    }
}

void print_friend(CGNode* node){
    cout<<node->getname()<<" has friends:"<<endl;
    for(int i = 0;i<node->getFriends().size();i++){
        cout<<"\t"<<node->getFriends()[i]->getname()<<endl;
    }
}

void print_allFriend(){
    for(int i = 0; i < CG.size(); i++){
        print_friend(CG[i]);
    }
}

void print_CG(){
    for(int i = 0; i < CG.size(); i++){
        cout<<CG[i]->getname()<<endl;
        if(!CG[i]->getChildren().empty()){
            for(int j=0; j < CG[i]->getChildren().size(); j++){
                cout<<"\t"<<CG[i]->getChildren()[j]->getname()<<" support: "<<
                CG[i]->getChildren()[j]->T_Support()<<endl;
                if(j == CG[i]->getChildren().size()-1){
                    break;
                }
                for(int k=j+1; k<CG[i]->getChildren().size();k++){
                    cout<<"\t\t"<<"support({ "<<CG[i]->getChildren()[j]->getname()<<", "<<CG[i]->getChildren()[k]->getname()
                    <<" )} is "<<support(CG[i]->getChildren()[j], CG[i]->getChildren()[k])<<endl;
                }
            }
        }
    }
}

float Confidence(CGNode* A, CGNode* B, int pair_support){
    float percentage = (float)pair_support/(float)A->T_Support();
    float confidence = (float)100*percentage;
    cout.precision(2);
    return confidence;
}

void printing_bug(CGNode* A, CGNode* B, CGNode* parent, int pair_support, float confidence){
    string a,b,c;
    a = A->getname();
    b = B->getname();
    if(a.compare(b)>0){
        c = a;
        a = b;
        b = c;
    }
    cout.precision(2);
    cout<<"bug: "<<A->getname()<<" in "<<parent->getname()<<", pair: ("<<a
    <<", "<<b<<"), support: "<<pair_support<<", confidence: "<<fixed<<confidence<<"%"<<endl;
}

void finding_bug(int T_SUPPORT, int T_CONFIDENCE){
    for(int i = 0; i < CG.size(); i++){
        if(!CG[i]->getChildren().empty()){
            for(int j=0; j < CG[i]->getChildren().size(); j++){
                bool friends[CG[i]->getChildren()[j]->getFriends().size()];
                for(int a = 0; a < CG[i]->getChildren()[j]->getFriends().size(); a++){
                    friends[a] = false;
                }
                for(int k=0; k<CG[i]->getChildren().size();k++){
                    if( k == j ) continue;
                    int index = CG[i]->getChildren()[j]->friends_index(CG[i]->getChildren()[k]);
                    if(index > -1){
                        friends[index] = true;
                    }
                }
                for(int m = 0; m < CG[i]->getChildren()[j]->getFriends().size(); m++){
                    int pair_support = support(CG[i]->getChildren()[j],CG[i]->getChildren()[j]->getFriends()[m]);
                    //cout<<friends[m]<<" "<<CG[i]->getChildren()[j]->getname()<<" and "<<CG[i]->getChildren()[j]->getFriends()[m]->getname()<<" support: "<<pair_support<<" in "<<CG[i]->getname()<<endl;
                    if(!friends[m] && pair_support>=T_SUPPORT){
                        float confidence = Confidence(CG[i]->getChildren()[j], CG[i]->getChildren()[j]->getFriends()[m],pair_support);
                        //cout<<"conf: "<<confidence<<endl;
                        if(T_CONFIDENCE <= confidence){
                            //print bug
                            printing_bug(CG[i]->getChildren()[j],CG[i]->getChildren()[j]->getFriends()[m],CG[i],pair_support,confidence);
                        }
                    }
                }
            }
        }
    }
}



void counting_support(){
    for(int i = 0; i < CG.size(); i++){
        if(!CG[i]->getChildren().empty()){
            map< pair<CGNode*, CGNode*>, int > temp_map;
            for(int j=0; j < CG[i]->getChildren().size(); j++){
                //count the support to every single child
                CG[i]->getChildren()[j]->T_Support_Increment();
                if(j == CG[i]->getChildren().size()-1){
                    break;
                }
                //calculate support to every pair in parent i
                for(int k = j+1; k < CG[i]->getChildren().size(); k++ ){
                    CG[i]->getChildren()[j]->setFriend(CG[i]->getChildren()[k]);//be friends with each other
                    CG[i]->getChildren()[k]->setFriend(CG[i]->getChildren()[j]);
                    map< pair<CGNode*, CGNode*>, int >::iterator it;
                    pair<CGNode*, CGNode*> pair1(CG[i]->getChildren()[j],CG[i]->getChildren()[k]);
                    pair<CGNode*, CGNode*> pair2(CG[i]->getChildren()[k],CG[i]->getChildren()[j]);
                    it = temp_map.find(pair1);
                    if(it == temp_map.end()){
                        it = temp_map.find(pair2);
                    }
                    if(it==temp_map.end()){
                        temp_map[pair1] = 1;
                    }
                    else{
                        continue;
                    }
                    it = pair_map.find(pair1);
                    if(it == pair_map.end()){
                        it = pair_map.find(pair2);
                    }
                    if(it == pair_map.end()){
                        pair_map[pair1] = 1;
                    }
                    else{
                        //cout<<CG[i]->getChildren()[j]->getname()<<" "<<CG[i]->getChildren()[k]->getname()<<
                        //" support increase in"<<CG[i]->getname()<<endl;
                        it->second++;
                    }
                }
            }
            //end
        }
    }
}


void createCallGraph(string line){
    string parent("Call graph node for function:");
    string child("calls function");
    string null_function("Call graph node <<null function>>");
    string name;
    CGNode* node;
    CGNode* child_node;
    map<string,CGNode*>::iterator it;
    if(line.size()<10){
        return;
    }
    if(line.compare(0,null_function.size(),null_function)==0){
        null_func_parent = true;
    }
    else if((int)line.find(parent)>-1){
        unsigned long pos =line.find(" '");
        unsigned long pos2 =line.find("'<<");
        pos += 2;
        name = line.substr(pos,pos2-pos);
        //check if there is already such a node in call graph
        //if not, create a node and put into call graph
        it = graph.find(name);
        if(it==graph.end()){
            node = new CGNode(name);
            graph[name] = node;
            CG.push_back(node);
        }
        else{
            node = it->second;
        }
        
        current_parent = node;
        null_func_parent = false;
    }
    else if((int)line.find(child)>11){
        unsigned long pos =line.find(" '");
        pos += 2;
        name = line.substr(pos,line.size()-pos-1);
        it = graph.find(name);
        if(it == graph.end()){
            child_node = new CGNode(name);
            graph[name] = child_node;
            CG.push_back(child_node);
        }
        else{
            child_node = it->second;
        }
        current_parent->setChild(child_node);
    }else;
    
}



int main(int argc, const char * argv[])
{
    int T_SUPPORT, T_CONFIDENCE;
    T_SUPPORT = 3;
    T_CONFIDENCE = 65;
    string filename;
    switch (argc) {
        case 4:
            istringstream(argv[2])>>T_SUPPORT;
            istringstream(argv[3])>>T_CONFIDENCE;
        case 2:
            filename = argv[1];
            break;
        default:
            cerr<<"arguments number is wrong!"<<endl;
            return 1;
    }
    //cout<<"file: "<<filename<<" support: "<<T_SUPPORT<<" confidence: "<<T_CONFIDENCE<<endl;
    null_func_parent = false;
    string line;
    ifstream myfile;
    myfile.open(filename);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            //cout<<line<<endl;
            createCallGraph(line);
        }
        myfile.close();
    }
    else{
        cout<<"unable to open the file"<<endl;
    }
    counting_support();
    //print_CG();
    finding_bug(T_SUPPORT, T_CONFIDENCE);
    //print_allFriend();
    return 0;
}






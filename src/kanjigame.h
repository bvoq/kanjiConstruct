//
//  kanjigame.h
//  kanjiTrees
//
//  Created by kdkdk on 26.12.18.
//

#ifndef kanjigame_h
#define kanjigame_h


struct kanjiPointerCompare {
    bool operator() (const Kanji* lhs, const Kanji* rhs) const {
        return lhs->no < rhs->no;
    }
};

vector<Kanji*> kanjiTree;
set<Kanji*, kanjiPointerCompare> kanjiTreeSet;
set<Kanji*, kanjiPointerCompare> selectedKanji;

void removeSelectedKanji(Kanji* p) {
    selectedKanji.erase(p);
    p->selected = false;
}
void insertSelectedKanji(Kanji* p) {
    cout << "selected: " << p->repres.front().translation;
    selectedKanji.insert(p);
    p->selected = true;
}

void initKanjiGame() {
    kanjiTree.clear();
    //add the first 5 undividable elements
    for(int i=0;i<kanjis.size();++i) {
        if(kanjis[i].consists.size() == 0 && i != 1764) {
            kanjis[i].known = true;
            kanjiTree.push_back(&kanjis[i]);
            kanjiTreeSet.insert(&kanjis[i]);
        }
    }
}

int checkMatch() {
    vector<int> tolookup;
    for(const Kanji* a : selectedKanji)
        tolookup.push_back(a->no);
    //should be sorted by set condition
    for(auto range = lookuptable.equal_range(tolookup);
        range.first != range.second;
        range.first++) {
        if(kanjis[range.first->second].known == false) { //using == false for clarity
            return range.first->second;
        }
    }
    return -1;
}

void drawKanjiGame() {
    int scale = getScale() - 4*charW(); // get some edge space of 2*charW()
    int h = charW() + 2*charWSmall();
    int cx = (ofGetWidth() - scale)/2 + scale/2 - h;
    int cy = (ofGetHeight() - scale)/2 + scale/2 - h/2.;
    int acx = (ofGetWidth() - scale)/2 + scale/2.-h/2.;
    int acy = (ofGetHeight() - scale)/2 + scale/2 - 3*h/4.;
    
    
    int mx = ofGetAppPtr()->mouseX;
    int my = ofGetAppPtr()->mouseY;
    
    int currentI = 0, layerI = 0;
    vector<int> sizeOfLayer = {45,43,39,18,18,18,18,18,18,18,18,18,18};
    
    for(int i=0;i<kanjiTree.size();++i) {
        float angle = (i-currentI)*2*M_PI/(MIN(kanjiTree.size()-currentI,sizeOfLayer[layerI]));
        float radius = scale/2. - layerI * charW()*3;
        
        int px = cx + radius*sin(angle);
        int py = cy + radius*cos(angle) + charW();
        
        bool canDisplay = true;
        if(selectedKanji.size() > 0 && selectedKanji.count(kanjiTree[i])==0) {
            bool foundOne = false;
            for(int e : kanjiTree[i]->canConstruct) {
                int countNo = 0;
                for(int f : kanjis[e].consists) {
                    if(kanjiTreeSet.count(&kanjis[f]) == 0) {
                        countNo = 0;
                        break;
                    }
                    if(selectedKanji.count(&kanjis[f])>0) countNo++;
                }
                if(countNo == selectedKanji.size()) {
                    foundOne = true;
                    break;
                }
            }
            if(!foundOne) canDisplay = false;
            //restart loop
        }
        
        if(canDisplay) {
            int w = printEntireKanjiWithMeaningCompact(px,py, kanjiTree[i]->no) - px;
            
            ofRectangle box(px-charWSmall(), py-h-charWSmall(), w+2*charWSmall(),h+2*charWSmall());
            if(kanjiTree[i]->selected) {
                ofFill();
                ofSetColor(255,0,0,100);
                ofDrawRectangle(box);
                
                ofFill();
                ofSetColor(0,0,0);
                ofDrawLine((box.getMinX()+box.getMaxX())/2,(box.getMinY()+box.getMaxY())/2,acx,acy);
            }
            
            if(mx >= box.getMinX() && mx <= box.getMaxX() && my >= box.getMinY() && my <= box.getMaxY()) {
                ofNoFill();
                ofSetColor(255,0,0);
                ofDrawRectangle(box);
                if(mouseFirstPressed) {
                    if(kanjiTree[i]->selected)
                        removeSelectedKanji(kanjiTree[i]);
                    else if (selectedKanji.size() < 20)  // can only have up to 20 selected
                        insertSelectedKanji(kanjiTree[i]);
                }
                
            }
        }
        
        if(((i+1)-currentI) % sizeOfLayer[layerI] == 0) {
            currentI = i+1;
            layerI++;
        }
    }
    
    int checkIndex = checkMatch();
    if(checkIndex != -1) {
        
        ofFill();
        ofSetColor(0);
        
        int w = printEntireKanjiWithMeaningCompact(cx,cy, checkIndex) - cx;
        
        ofRectangle box(cx-charWSmall(), cy-h-charWSmall(), w+2*charWSmall(),h+2*charWSmall());
        ofSetColor(0,255,0,100);
        ofDrawRectangle(box);
        
        if(mx >= box.getMinX() && mx <= box.getMaxX() && my >= box.getMinY() && my <= box.getMaxY()) {
            ofSetColor(0,255,0,0);
            if(mouseFirstPressed) {
                for(Kanji* e : selectedKanji) {
                    e->discoveredBuiltFrom.push_back(&kanjis[checkIndex]);
                }
                kanjiTree.push_back(&kanjis[checkIndex]);
                kanjiTreeSet.insert(&kanjis[checkIndex]);
                kanjis[checkIndex].known = true;
            }
        }
        
        //print it twice so it's over green
        printEntireKanjiWithMeaningCompact(cx,cy, checkIndex) - cx;
        
    }
    
}

#endif /* kanjigame_h */

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
set<Kanji*, kanjiPointerCompare> selectedKanji;

void removeSelectedKanji(Kanji* p) {
    selectedKanji.erase(p);
    p->selected = false;
}
void insertSelectedKanji(Kanji* p) {
    selectedKanji.insert(p);
    p->selected = true;
}

void initKanjiGame() {
    kanjiTree.clear();
    //add the first 5 undividable elements
    for(int i=0;i<kanjis.size() && kanjiTree.size() < 50;++i) {
        if(kanjis[i].consists.size() == 0) {
            kanjis[i].known = true;
            kanjiTree.push_back(&kanjis[i]);
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
    
    for(int i=0;i<kanjiTree.size();++i) {
        float angle = i*2*M_PI/kanjiTree.size();
        int px = cx + scale/2.*sin(angle);
        int py = cy + scale/2.*cos(angle) + charW();
        
        
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
                else if (selectedKanji.size() < 5)  // can only have up to 5 selected
                    insertSelectedKanji(kanjiTree[i]);
            }
            
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
                kanjiTree.push_back(&kanjis[checkIndex]);
                kanjis[checkIndex].known = true;
            }
        }
        
        //print it twice so it's over green
        printEntireKanjiWithMeaningCompact(cx,cy, checkIndex) - cx;
        
    }

}

#endif /* kanjigame_h */

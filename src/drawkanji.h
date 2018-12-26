//
//  drawkanji.h
//  kanjiTrees
//
//  Created by kdkdk on 26.12.18.
//

#ifndef drawkanji_h
#define drawkanji_h

//draws upwar
int printKanjiWithMeaning(int x, int y, int no, int repr) {
    //ofDrawRectangle(x,y,10,10);
    
    assert(no < kanjis.size());
    assert(repr < kanjis[no].repres.size());
    if(x < -100 || y < - 100 || x > ofGetWindowWidth() + 100 || y > ofGetWindowHeight() + 100) return x;
    int totalw = 0, totalh = charW();
    
    ofSetColor(0);
    int rightmost = x;
    int sw = fontSmall.stringWidth(kanjis[no].repres[repr].translation);
    int sh = fontSmall.stringHeight(kanjis[no].repres[repr].translation);
    int w=0,h=0;
    if(kanjis[no].repres[repr].hasUnicodeString) {
        ofSetColor(0);
        w = font.stringWidth(kanjis[no].repres[repr].kanjiStr);
        h = font.stringHeight(kanjis[no].repres[repr].kanjiStr);
        if(sw > w) font.drawString(kanjis[no].repres[repr].kanjiStr, x+(sw-w)/2, y);
        else font.drawString(kanjis[no].repres[repr].kanjiStr, x, y);
        
    } else {
        ofSetColor(255);
        //does not work well with the images, as they are not cropped
        //if(sw > w) kanjis[no].repres[repr].kanjiImg.draw(x+(sw-w)/2,y-charW(),charW(),charW());
        kanjis[no].repres[repr].kanjiImg.draw(x,y-charW(),charW(),charW());
        w = charW();
        h = charW();
    }
    
    ofSetColor(0);
    
    if(sw > w) fontSmall.drawString(kanjis[no].repres[repr].translation, x,y-h);
    else fontSmall.drawString(kanjis[no].repres[repr].translation, x+(w-sw)/2,y-h);
    
    return x+max(w,sw);
}

int printEntireKanjiWithMeaning(int x, int y, int no) {
    for(int j=0; j < kanjis[no].repres.size(); ++j) {
        x = printKanjiWithMeaning(x,y,no,j);
    }
    return x;
}

int printKanjiWithMeaningCompact(int x, int y, int no, int repr) {
    //ofDrawRectangle(x,y,10,10);
    
    assert(no < kanjis.size());
    assert(repr < kanjis[no].repres.size());
    if(x < -100 || y < - 100 || x > ofGetWindowWidth() + 100 || y > ofGetWindowHeight() + 100) return x;
    int totalw = 0, totalh = charW();
    
    ofSetColor(0);
    int rightmost = x;
    int w=0,h=0;
    if(kanjis[no].repres[repr].hasUnicodeString) {
        ofSetColor(0);
        w = font.stringWidth(kanjis[no].repres[repr].kanjiStr);
        h = font.stringHeight(kanjis[no].repres[repr].kanjiStr);
        font.drawString(kanjis[no].repres[repr].kanjiStr, x, y);
        
    } else {
        ofSetColor(255);
        kanjis[no].repres[repr].kanjiImg.draw(x,y-charW(),charW(),charW());
        w = charW();
        h = charW();
    }

    
    string str = kanjis[no].repres[repr].translation;
    
    int sw = fontSmall.stringWidth(str);
    int sh = fontSmall.stringHeight(str);

    // clip the string in two lines if too big.
    //cout << "yea for: " << sw << " " << sh << " " << str << endl;

    if(sw > w && sw > charW()) {
        //search for space first
        int mid = str.size()/2;
        string str1, str2;
        for(int i=0;i<mid/2;++i) {
            if(!(str[mid+i] >= 'A' && str[mid+i] <= 'Z')&&!(str[mid+i] >= 'a' && str[mid+i] <= 'z') ) {
                str1 = str.substr(0,mid+i);
                if(str[mid+i] == ' ') str2 = str.substr(mid+i+1);
                else str2 = str.substr(mid+i);
                break;
            }
            if(!(str[mid-i] >= 'A' && str[mid-i] <= 'Z')&&!(str[mid-i] >= 'a' && str[mid-i] <= 'z') ) {
                str1 = str.substr(0,mid-i);
                if(str[mid-i] == ' ') str2 = str.substr(mid-i+1);
                else str2 = str.substr(mid-i);
                break;
            }
        }
        
        if(str1.size() == 0) {
        for(int i=0;i<mid/2;++i) {
            if(str[mid+i] != 'a' && str[mid+i] != 'e' && str[mid+i] != 'i' && str[mid+i] != 'o' && str[mid+i] != 'u') {
                if(str[mid+i+1] == 'a' || str[mid+i+1] == 'e' || str[mid+i+1] == 'i' || str[mid+i+1] == 'o' || str[mid+i+1] == 'u') {
                    str1 = str.substr(0,mid+i) + "-";
                    str2 = str.substr(mid+i);
                    break;
                }
            }
            if(str[mid-i] != 'a' && str[mid-i] != 'e' && str[mid-i] != 'i' && str[mid-i] != 'o' && str[mid-i] != 'u') {
                if(str[mid-i+1] == 'a' || str[mid-i+1] == 'e' || str[mid-i+1] == 'i' || str[mid-i+1] == 'o' || str[mid-i+1] == 'u') {
                    str1 = str.substr(0,mid-i) + "-";
                    str2 = str.substr(mid-i);
                    break;
                }
            }
        }
        }
        ofSetColor(0);
        int sw1 = fontSmall.stringWidth(str1);
        int sw2 = fontSmall.stringWidth(str2);
        if(sw1 > w) fontSmall.drawString(str1, x,y-h-sh);
        else fontSmall.drawString(str1, x+(w-sw1)/2,y-h-sh);
        
        if(sw2 > w) fontSmall.drawString(str2, x,y-h);
        else fontSmall.drawString(str2, x+(w-sw2)/2,y-h);
        sw = max(sw1,sw2);
    }
    else {
        ofSetColor(0);
        if(sw > w) fontSmall.drawString(kanjis[no].repres[repr].translation, x,y-h);
        else fontSmall.drawString(kanjis[no].repres[repr].translation, x+(w-sw)/2,y-h);
    }
    

    
    return x+max(w,sw);
}

int printEntireKanjiWithMeaningCompact(int x, int y, int no) {
    for(int j=0; j < kanjis[no].repres.size(); ++j) {
        x = printKanjiWithMeaningCompact(x,y,no,j);
    }
    return x;
}


int offsetY = 0;
void printDictionary() {
    for(int i = 0; i < kanjis.size(); ++i) {
        int initX = 100, initY = charW()*2*i+offsetY;
        if(initY < -100 || initY > ofGetWindowHeight() + 100) continue;
        
        for(int j=0; j < kanjis[i].repres.size(); ++j) {
            initX = printKanjiWithMeaningCompact(initX,initY,i,j);
        }
        if(kanjis[i].consists.size() != 0) {
            font.drawString("   consists of: ", initX,initY);
            initX = font.getStringBoundingBox("   consists of: ",initX,initY).getRight();
        }
        for(int j = 0; j < kanjis[i].consists.size(); ++j) {
            initX = printKanjiWithMeaningCompact(initX,initY,kanjis[i].consists[j],kanjis[i].consistsWhichMutant[j]);
        }
    }
}

#endif /* drawkanji_h */

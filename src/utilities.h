//
//  utilities.h
//  kanjiTrees
//
//  Created by kdkdk on 26.12.18.
//

#ifndef utilities_h
#define utilities_h

float getScale() {
    int w = ofGetWidth();
    int h = ofGetHeight();
    return min(w,h);
}

inline float charW() {
    return 48; //fixed
}

inline float charWSmall() {
    return 12; //fixed
}


ofTrueTypeFont font;
ofTrueTypeFont fontSmall;
void initFont() {
    float scale = getScale();
    
    vector<string> possibleFonts = {
        "Othutome_font_ver2/OthutomeFont_Ver2.ttf"
    };
    ofTrueTypeFontSettings settings(possibleFonts[0],charW());
    settings.antialiased = true;
    settings.addRanges({
        ofUnicode::Latin1Supplement,
        ofUnicode::Hiragana,
        ofUnicode::Katakana,
        ofUnicode::CJKUnified
        
    });
    
    settings.addRanges(ofAlphabet::Emoji);
    settings.addRanges(ofAlphabet::Japanese);
    
    font.load(settings);
    
    ofTrueTypeFontSettings smallsettings(possibleFonts[7],charWSmall());
    smallsettings.antialiased = true;
    smallsettings.addRanges({
        ofUnicode::Latin1Supplement,
        ofUnicode::Hiragana,
        ofUnicode::Katakana
    });
    
    fontSmall.load(smallsettings);
}

#endif /* utilities_h */

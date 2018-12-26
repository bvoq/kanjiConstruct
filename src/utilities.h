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
        "antique-kaku/font_1_ant-kaku.ttf",
        "aozoramincho-readme-ttf/AozoraMinchoRegular.ttf",
        "honoka-marugo/font_1_honokamarugo_1.1.ttf",
        "JapanSansFont-free/ウェブフォント用/JapanSans70.ttf",
        "Kosugi_Maru/KosugiMaru-Regular.ttf",
        "KTEGAKI115/KTEGAKI.ttf",
        "Othutome_font_ver2/OthutomeFont_Ver2.ttf",
        "rnd_css_20151231/omake/nukamiso_2004_beta09.ttf",
        "SistersFs/SistersFL.ttf"
    };
    ofTrueTypeFontSettings settings(possibleFonts[7],charW());
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

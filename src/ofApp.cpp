#include "ofApp.h"

bool compareAccordingToFirstDigits(const string & a, const string & b) {
    auto posa1 = a.find("/"), posa2 = a.find("-");
    auto posb1 = b.find("/"), posb2 = b.find("-");
    int no1 = stoi( a.substr(posa1+1,posa2-posa1-1) );
    int no2 = stoi( b.substr(posb1+1,posb2-posb1-1) );
    if(no1 != no2) return no1 < no2;
    else return a < b;
}


struct KanjiSymbol {
    bool hasUnicodeString;
    string kanjiStr; //if no image, it's the kanji, elsewise it's the adress of the image of the kanji
    ofImage kanjiImg;
    string translation;
};
struct Kanji {
    string loc; //location of html in file system
    int no;
    vector<int> consists; //consists out of the following numbers
    vector<int> consistsWhichMutant;
    
    vector<KanjiSymbol> repres;
    //bool kanjiDamageOriginal;
    //ofImage kanjiImg; //only loaded if original
};

//superstitious transformation, it's correct but ehh.. yee.
int noToArr(int i) {
    if(i >= 666) i--;
    if(i >= 584) i--;
    i--; // zero indexed
    return i;
}

vector<Kanji> kanjis;
ofTrueTypeFont font;
ofTrueTypeFont fontSmall;

bool operator<(const Kanji & a, const Kanji & b) {
    return a.no < b.no;
}



vector<Kanji> initKanjis(string path) {
    ofDirectory dir(path);
    //only show png files
    dir.allowExt("html");
    //populate the directory object
    dir.listDir();
    
    //vector<string> kanjis = dir.getSorted();
    
    kanjis.resize(dir.size());
    //go through and print out all the paths
    for(int i = 0; i < dir.size(); i++) {
        kanjis[i].loc = dir.getPath(i);
        int a = kanjis[i].loc.find_last_of("/");
        int b = kanjis[i].loc.find("-",a); //start searching from a.
        int c = -1;
        assert(kanjis[i].loc.substr(kanjis[i].loc.size()-5) == ".html");
        if (kanjis[i].loc[kanjis[i].loc.size()-6] == '-') c = kanjis[i].loc.size()-6;
        else c = kanjis[i].loc.size()-5;
        
        KanjiSymbol mainSymbol;
        mainSymbol.translation = kanjis[i].loc.substr(b+1,c-b-1);
        
        assert(a != kanjis[i].loc.size());
        assert(b != kanjis[i].loc.size());
        cout << kanjis[i].loc << endl;
        assert(b-a-1 > 0);
        kanjis[i].no = noToArr( stoi( kanjis[i].loc.substr(a+1,b-a-1) ) ); // go to zero for simplicity
        cout << kanjis[i].no << endl;
        
        
        string fullhtml = ofBufferFromFile(kanjis[i].loc).getText();
        
        //Get the kanji symbol
        size_t startkanjisymbol = fullhtml.find("kanji_character");
        size_t endkanjisymbol = fullhtml.find("</span>", startkanjisymbol);
        
        string kanjisymbol = fullhtml.substr(startkanjisymbol + 17, endkanjisymbol-(startkanjisymbol+17));
        mainSymbol.kanjiStr = kanjisymbol;
        //this is either a kanji or a kanjidamage original. the original has to be loaded as an image.
        mainSymbol.hasUnicodeString = !(kanjisymbol.size() > 4 && (kanjisymbol.substr(0,4) == "<img"));
        
        if(!mainSymbol.hasUnicodeString) {
            size_t startofsrcpath = kanjisymbol.find("radsmall");
            size_t endofscrpath = kanjisymbol.find("\"",startofsrcpath+6);
            string imgpath = kanjisymbol.substr(startofsrcpath,endofscrpath-(startofsrcpath));
            cout << "loading: " << imgpath << endl;
            mainSymbol.kanjiStr = imgpath;
            mainSymbol.kanjiImg.load(imgpath);
        }
        
        kanjis[i].repres.push_back(mainSymbol);
        
        //Lastly add mutants if there are any
        auto mutantstartpos = fullhtml.find("<h2>Mutants</h2>");
        if(mutantstartpos < fullhtml.size()) {
            auto mutantendpos = fullhtml.find("</table>");
            auto mutantit = mutantstartpos;
            while ((mutantit = fullhtml.find("<td>", mutantit+1) ) < mutantendpos) {
                KanjiSymbol mutant;
                auto endofmutant = fullhtml.find("</td>", mutantit);
                string mutantStr = fullhtml.substr(mutantit+4,endofmutant-(mutantit+4));
                if(mutantStr.size() > 4 && mutantStr.substr(0,4) == "<img") {
                    mutant.hasUnicodeString = false;
                    auto mutantImgStart = fullhtml.find("radREALLYsmall",mutantit);
                    auto mutantImgEnd = fullhtml.find("\"",mutantImgStart);
                    //string mutantImg = "radsmall" + fullhtml.substr(mutantImgStart+14,mutantImgEnd-(mutantImgStart+14));
                    string mutantImg = fullhtml.substr(mutantImgStart,mutantImgEnd-mutantImgStart);
                    mutant.kanjiStr = mutantImg;
                    cout << "loading " << mutantImg << endl;
                    mutant.kanjiImg.load(mutantImg);
                } else {
                    mutant.hasUnicodeString = true;
                    mutant.kanjiStr = mutantStr;
                }
                mutantit = fullhtml.find("<td>", mutantit+1);
                auto endofmutanttranslation = fullhtml.find("</td>",mutantit);
                string mutantTranslation = fullhtml.substr(mutantit+4,endofmutanttranslation-(mutantit+4));
                mutant.translation = mutantTranslation;
                kanjis[i].repres.push_back(mutant);
            }
        }
            
    }
    sort(kanjis.begin(),kanjis.end());
    for(int i = 0; i < kanjis.size(); ++i) {
        string fullhtml = ofBufferFromFile(kanjis[i].loc).getText();
        
        // Get the kanjis out of which it consists:
        size_t startconfig = fullhtml.find("</h1>"); //immediately after this follows the config
        size_t endconfig = fullhtml.find("</div>",startconfig); //end of config
        size_t arefmention = startconfig;
        
        while ((arefmention = fullhtml.find("<a href=\"", arefmention+1) ) < endconfig) {
            size_t untilaref = fullhtml.find("-", arefmention);
            
            string builtfromstr = fullhtml.substr(arefmention+9,untilaref-(arefmention+9));
            //skip all special tag information
            if(builtfromstr.size() > 7 && builtfromstr.substr(0,7) == "../tags") continue; //skip
            
            int builtfrom = noToArr( stoi( builtfromstr ) );
            kanjis[i].consists.push_back(builtfrom);
            
            size_t exactComponent = fullhtml.find("component\">", arefmention);
            size_t endOfComponent = fullhtml.find("</a>", exactComponent);
            string componentStr = fullhtml.substr(exactComponent+11,endOfComponent-(exactComponent+11));
            
            if(componentStr.size() > 4 && componentStr.substr(0,4) == "<img") {
                size_t startofimgcomponent = fullhtml.find("radsmall",exactComponent);
                size_t endofimgcomponent = fullhtml.find("\"", startofimgcomponent);
                string imgstr = fullhtml.substr(startofimgcomponent,endofimgcomponent-startofimgcomponent);
                size_t minusseperator = imgstr.find_last_of("-"); //mmmm might not be good idea cus water-secondarg
                string imgname = imgstr.substr(9,minusseperator-9);

                bool foundimg = false;
                for(int q=0;q<kanjis[builtfrom].repres.size(); ++q) {
                    size_t startofotherimgcomponent = kanjis[builtfrom].repres[q].kanjiStr.find_last_of("/");
                    size_t endofotherimgcomponent = kanjis[builtfrom].repres[q].kanjiStr.find_last_of("-");
                    string otherimgname = kanjis[builtfrom].repres[q].kanjiStr.substr(startofotherimgcomponent+1, endofotherimgcomponent - (startofotherimgcomponent+1));
                    if(imgname == otherimgname) {
                        foundimg = true;
                        assert(!kanjis[builtfrom].repres[q].hasUnicodeString);
                        if(kanjis[builtfrom].repres[q].kanjiStr != imgstr) {
                            cout << "loading better res " << imgstr << endl;
                            kanjis[builtfrom].repres[q].kanjiImg.load(imgstr); //load higher res picture
                            kanjis[builtfrom].repres[q].kanjiStr = imgstr;
                        }
                        kanjis[i].consistsWhichMutant.push_back(q);
                        break;
                    }
                }
                assert(foundimg);
            } else {
                for(int q=0;q<kanjis[builtfrom].repres.size(); ++q) {
                    if(kanjis[builtfrom].repres[q].kanjiStr == componentStr) {
                        kanjis[i].consistsWhichMutant.push_back(q);
                        break;
                    }
                }
            }
        }
    }
    
    for(int i = 0; i < kanjis.size(); ++i) {
        cout << kanjis[i].no << " " << kanjis[i].repres.front().translation << endl;
    }
    int counter = 0;
    for(int i = 0; i < kanjis.size(); ++i) {
        if(kanjis[i].consists.size() == 0) {
            cout << kanjis[i].no << " " << kanjis[i].repres.front().translation << endl;
            counter++;
        }
    }
    cout << counter << endl;
    return kanjis;
}

float getScale() {
    int w = ofGetWindowWidth();
    int h = ofGetWindowHeight();
    return min(w,h);
}

inline float charW() {
    return 48; //fixed
}

inline float charWSmall() {
    return 12; //fixed
}

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

    /*
    settings.ranges = {
        ofUnicode::Latin1Supplement,
        ofUnicode::Hiragana,
        ofUnicode::Katakana,
        ofUnicode::CJKUnified
    };*/
    
    
    /*
     ofTtfSettings fontsettings("Othutome_font_ver2/OthutomeFont_Ver2.ttf", 16);
     //fontsettings.aliased = true;
     fontsettings.add({
     ofUnicode::Latin1Supplement,
     ofUnicode::Hiragana,
     ofUnicode::Katakana,
     ofUnicode::CJKUnified
     });
     fontsettings.addRange(ofUnicode::CJKUnified); //chinese, japanese, korean
     */
    
    //fontsettings.addRanges(ofAlphabet::Chinese);
    //fontsettings.addRanges(ofAlphabet::Latin);
    //fontsettings.addRange(ofUnicode::Latin1Supplement);
    //fontsettings.addRange(ofUnicode::LatinExtendedAdditional);
    //fontsettings.addRange(ofUnicode::Hiragana);
    //fontsettings.addRange(ofUnicode::Katakana);
    //fontsettings.addRange(ofUnicode::KatakanaPhoneticExtensions);
    //fontsettings.addRange(ofUnicode::KatakanaHalfAndFullwidthForms);
    
}

//draws upwar
int printKanji(int x, int y, int no, int repr) {
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
        kanjis[no].repres[repr].kanjiImg.draw(x,y-charW(),charW(),charW());
        w = charW();
        h = charW();
    }
    
    ofSetColor(0);
    if(sw > w) fontSmall.drawString(kanjis[no].repres[repr].translation, x,y-h);
    else fontSmall.drawString(kanjis[no].repres[repr].translation, x+(w-sw)/2,y-h);

    return x+max(w,sw);
}

int timesinceresize = 0;
int offsetY = 0;

void printDictionary() {
    for(int i = 0; i < kanjis.size(); ++i) {
        int initX = 100, initY = charW()*2*i+offsetY;
        if(initY < -100 || initY > ofGetWindowHeight() + 100) continue;
        
        for(int j=0; j < kanjis[i].repres.size(); ++j) {
            initX = printKanji(initX,initY,i,j);
        }
        if(kanjis[i].consists.size() != 0) {
            font.drawString("   consists of: ", initX,initY);
            initX = font.getStringBoundingBox("   consists of: ",initX,initY).getRight();
        }
        for(int j = 0; j < kanjis[i].consists.size(); ++j) {
            initX = printKanji(initX,initY,kanjis[i].consists[j],kanjis[i].consistsWhichMutant[j]);
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    // init kanjis
    initKanjis("kanjiList");
    // init font
    initFont();
    string path = "kanjiList";
    
}

//--------------------------------------------------------------
void ofApp::update(){
    if(timesinceresize != 0 && ofGetElapsedTimeMillis() - timesinceresize > 2000) {
        timesinceresize = 0;
        //initFont();
    }
}

//--------------------------------------------------------------


void ofApp::draw(){
    ofBackground(255);
    ofFill();
    ofSetColor(0,0,0);
    printDictionary();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_UP) {
        offsetY += 100;
    }
    if(key == OF_KEY_DOWN) {
        offsetY -= 100;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    timesinceresize = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

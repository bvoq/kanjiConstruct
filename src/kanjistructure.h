//
//  kanjistructure.h
//  kanjiTrees
//
//  Created by kdkdk on 26.12.18.
//

#ifndef kanjistructure_h
#define kanjistructure_h

/*
bool compareAccordingToFirstDigits(const string & a, const string & b) {
    auto posa1 = a.find("/"), posa2 = a.find("-");
    auto posb1 = b.find("/"), posb2 = b.find("-");
    int no1 = stoi( a.substr(posa1+1,posa2-posa1-1) );
    int no2 = stoi( b.substr(posb1+1,posb2-posb1-1) );
    if(no1 != no2) return no1 < no2;
    else return a < b;
}*/


struct KanjiSymbol {
    bool hasUnicodeString;
    string kanjiStr; //if no image, it's the kanji, elsewise it's the adress of the image of the kanji
    ofImage kanjiImg;
    string translation;
    string str1, str2; // in case the translation is too big it is usually split among multiple
};
struct Kanji {
    string loc; //location of html in file system
    int no;
    vector<int> consists; //consists out of the following numbers
    vector<int> consistsWhichMutant;
    
    vector<int> canConstruct;
    
    vector<KanjiSymbol> repres;
    
    //for the game
    bool selected = false;
    bool expanded = false;
    bool known = false;
    vector<Kanji*> discoveredBuiltFrom;
};

multimap<vector<int>, int> lookuptable;

bool operator<(const Kanji & a, const Kanji & b) {
    return a.no < b.no;
}

//superstitious transformation, it's correct but ehh.. yee.
int noToArr(int i) {
    if(i >= 666) i--;
    if(i >= 584) i--;
    i--; // zero indexed
    return i;
}

string replStr(string str) {
    string ostr = "";
    for(int i=0; i<str.size(); ++i) {
        if(i+4 <= str.size() && str.substr(i,4) == "&lt;") {
            ostr += "<";
            i+=3;
            continue;
        }
        ostr.push_back(str[i]);
    }
    return ostr;
}

vector<Kanji> kanjis;
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

        assert(b-a-1 > 0);
        kanjis[i].no = noToArr( stoi( kanjis[i].loc.substr(a+1,b-a-1) ) ); // go to zero for simplicity
        
        cout << "loading " << kanjis[i].loc << endl;
        string fullhtml = ofBufferFromFile(kanjis[i].loc).getText();
        
        //find better translation
        size_t starttranslationsymbol = fullhtml.find("'translation'>");
        size_t endtranslationsymbol = fullhtml.find("</span>",starttranslationsymbol);
        mainSymbol.translation = fullhtml.substr(starttranslationsymbol+14,endtranslationsymbol-(starttranslationsymbol+14));
        
        //Get the kanji symbol
        size_t startkanjisymbol = fullhtml.find("kanji_character");
        size_t endkanjisymbol = fullhtml.find("</span>", startkanjisymbol);
        
        string kanjisymbol = fullhtml.substr(startkanjisymbol + 17, endkanjisymbol-(startkanjisymbol+17));
        mainSymbol.kanjiStr = replStr(kanjisymbol);
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
                    mutant.kanjiStr = replStr(mutantImg);
                    cout << "loading " << mutantImg << endl;
                    mutant.kanjiImg.load(mutantImg);
                } else {
                    mutant.hasUnicodeString = true;
                    mutant.kanjiStr = replStr(mutantStr);
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
            if(builtfrom == 1764) continue; // SKIP THE GLITCH Kanji
            kanjis[i].consists.push_back(builtfrom);
            kanjis[builtfrom].canConstruct.push_back(i);
            
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
        
        
        //add it to the lookup table
        if(kanjis[i].consists.size() > 0) {
            vector<int> sortedconsists = kanjis[i].consists;
            sort(sortedconsists.begin(),sortedconsists.end());
            lookuptable.insert(make_pair(sortedconsists, i));
        }
    }
    
    for(int i = 0; i < kanjis.size(); ++i) {
        cout << kanjis[i].no << " " << kanjis[i].repres.front().translation << endl;
    }
    int counter = 0;
    for(int i = 0; i < kanjis.size(); ++i) {
        if(kanjis[i].consists.size() == 0) {
            cout << kanjis[i].no << " " << kanjis[i].repres.front().translation << " " << kanjis[i].loc << endl;
            counter++;
        }
    }
    cout << counter << endl;
    return kanjis;
}


#endif /* kanjistructure_h */

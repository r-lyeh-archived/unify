/* Unify is a C++11 function to normalize resouce identificators.
   - rlyeh, zlib/libpng licensed.

   Unify transforms any physical resource string to a unified string,
   called UID (Unified ID). Any absolute, relative, virtual and/or networks
   paths, URI, URL or ID will transform to an UID. Basically `unify(src)` does
   a string transformation from given string to a sorted `[a-zA-Z0-9-]+` pattern,
   which is guarnateed to remain inmutable (on a high degree) on code, even if
   physical source is altered externally.

   ## Features
   - Unified folder/asset separators.
   - Unified absolute, relative, virtual and remote paths.
   - Unified uppercases, lowercases, whitespaces and hyphens.
   - Unified extensions.
   - Unified typos on double extensions and double punctuations.
   - Unified typos on many diacritics.
   - Unified AoS (OO) and SoA (ECS) disk layouts.
   - Unified plurals as well (if using English words).
   - Unified SOV, SVO, VSO, VOS, OVS, OSV subject/verb/object language topologies.
   - Unified tagging (useful when globbing and deploying files and/or directories)
   - Unified consistency. Reunification as a lossless process.
   - Unify is header-only, cross-platform and standalone.
   - Unify is ZLIB/LibPNG licensed.

   ## Public API
   // Convert anything to an uid.
   // If pointer is valid, any tag found during parsing will be pushed into vector.
   string unify( const string &uri, vector<string> *tags_found = 0 );

   ## Samples
   You better inspect the test suite at bottom of file.
*/

#pragma once

// Public API

#include <string>
#include <vector>

#define UNIFY_VERSION "1.0.0" /* (2015/08/18) Initial version */

namespace {
// convert anything to an UID
// additionally, push back any tags found (if `tags != nullptr`)
std::string unify( const std::string &uri, std::vector<std::string> *tags = 0 );
}

// Private API following, and tests at bottom of file

#include <stdio.h>
#include <algorithm>
#include <cctype>
#include <deque>
#include <map>
#include <string>
#include <vector>
#include <cstring>

namespace {

std::string unify( const std::string &uri, std::vector<std::string> *tags ) {
    // taken from https://github.com/r-lyeh/wire {
    struct string {
        static void left_of( std::string &self, const std::string &substring ) {
            std::string::size_type pos = self.find( substring );
            if( pos != std::string::npos ) self = self.substr(0, pos);
        }
        static std::deque< std::string > tokenize( const std::string &self, const std::string &delimiters ) {
            std::string map( 256, '\0' );
            for( const unsigned char &ch : delimiters )
                map[ ch ] = '\1';
            std::deque< std::string > tokens(1);
            for( const unsigned char &ch : self ) {
                /**/ if( !map.at(ch)          ) tokens.back().push_back( char(ch) );
                else if( tokens.back().size() ) tokens.push_back( std::string() );
            }
            while( tokens.size() && !tokens.back().size() ) tokens.pop_back();
            return tokens;
        }
        static void replace( std::string &self, const std::string &target, const std::string &replacement ) {
            size_t found = 0;
            while( ( found = self.find( target, found ) ) != std::string::npos ) {
                self.replace( found, target.length(), replacement );
                found += replacement.length();
            }
        }
        static void replace( std::string &self, const std::map< std::string, std::string > &replacements ) {
            std::string out;
            for( size_t end = self.size(), i = end -end; i < end; ) {
                bool found = false;
                size_t match_length = 0;
                std::map< std::string, std::string >::const_reverse_iterator it;
                for( it = replacements.rbegin(); !found && it != replacements.rend(); ++it ) {
                    const std::string &target = it->first;
                    const std::string &replacement = it->second;
                    if( match_length != target.size() ) {
                        match_length = target.size();
                    }
                    if( self.size() - i >= target.size() )
                    if( !std::memcmp( &self.at(int(i)), &target.at(0), (int)match_length ) ) {
                        i += target.size();
                        out += replacement;
                        found = true;
                    }
                }
                if( !found )
                    out += self.at(int(i++));
            }
           self = out;
        }
        static void lowercase( std::string &s ) {
            std::transform( s.begin(), s.end(), s.begin(), (int(*)(int)) std::tolower );
        }
    };
    // }

    std::string tmp = uri;

    // 1) @todo latinization/romanization here (proper utf8)
    // [...]

    // 2) @todo remove diacritics here (proper utf8)
    std::map<std::string, std::string> diacritics = {
        {"ä", "a"},         {"Ä", "A"}, 
        {"â", "a"},         {"Â", "A"}, 
        {"á", "a"},         {"Á", "A"}, 
        {"à", "a"},         {"À", "A"}, 
        {"ã", "a"},         {"Ã", "A"}, 
        {"ë", "e"},         {"Ë", "E"}, 
        {"ê", "e"},         {"Ê", "E"}, 
        {"é", "e"},         {"É", "E"}, 
        {"è", "e"},         {"È", "E"}, 
        //{"~e", "e"},      {"~E", "E"}, 
        {"ï", "i"},         {"Ï", "I"}, 
        {"î", "i"},         {"Î", "I"}, 
        {"í", "i"},         {"Í", "I"}, 
        {"ì", "i"},         {"Ì", "I"}, 
        //{"~i", "i"},      {"~I", "I"}, 
        {"ö", "o"},         {"Ö", "O"}, 
        {"ô", "o"},         {"Ô", "O"}, 
        {"ó", "o"},         {"Ó", "O"}, 
        {"ò", "o"},         {"Ò", "O"}, 
        {"õ", "o"},         {"Õ", "O"}, 
        {"ü", "u"},         {"Ü", "U"}, 
        {"û", "u"},         {"Û", "U"}, 
        {"ú", "u"},         {"Ú", "U"}, 
        {"ù", "u"},         {"Ù", "U"}, 
        //{"~u", "u"},      {"~U", "U"}, 
        //{"¨n", "n"},      {"¨N", "N"}, 
        //{"^n", "n"},      {"^N", "N"}, 
        //{"´n", "n"},      {"´N", "N"}, 
        //{"`n", "n"},      {"`N", "N"}, 
        {"ñ", "n"},         {"Ñ", "N"}, 
    };
    string::replace( tmp, diacritics );

    // 3) @todo unescape url here
    // [...]

    // 4) remove url options (if any)
    tmp = tmp.substr( 0, tmp.find_first_of('?') );

    // 5) lowercase contents
    string::lowercase( tmp );

    // 6) strip tags like #tag-123 #tag_456 or #xbox360=yes
    int dst = 0;
    for( auto end = tmp.size(), it = end - end; it < end; ) {
        auto &chr = tmp[it];
        if( chr != '#' ) tmp[dst++] = tmp[it++];
        else {
            std::string tag;
            while( ++it < end ) {
                tag += (chr = tmp[it]);
                if( chr == '-' || chr == '=' || chr == '_' ) continue;
                if( chr >= '0' && chr <= '9' ) continue;
                if( chr >= 'a' && chr <= 'z' ) continue;
                if( tags ) {
                    tags->push_back( "#" + tag.substr( 0, tag.size() - 1 ) );
                }
                break;
            }
        }
    }
    tmp.resize(dst);

    // 7) split path up to 2nd level
    auto split = string::tokenize(tmp, "\\/");
    while( split.size() > 2 ) split.pop_front();

    // 8) trim extension, whitespaces and punctuations
    tmp.clear();
    std::map<std::string, std::string> whitespaces = {
        {" ", "-"},
        {"_", "-"},
        {",", "-"},
        {"|", "-"},
        {";", "-"},
        {":", "-"},
        {"(", "-"},
        {")", "-"},
        {"[", "-"},
        {"]", "-"},
    };
    for( auto &s : split ) {
        string::left_of(s, ".");
        string::replace(s, whitespaces);
        tmp = tmp + "-" + s;
    }

    //  9) split, sort, and join stems
    // 10) fix aos/soa plural
    split = string::tokenize(tmp, "-");
    std::sort( split.begin(), split.end() );
    tmp.clear();
    for( auto &s: split ) {
        if( !s.empty() && s.back() == 's' ) {
            s.pop_back();
        }
        tmp = tmp + "-" + s;
    }

    // 11) remove lead separator
    return &tmp[1];
}

}

#ifdef UNIFY_TESTS

// UID, hypothetical sugar class {
struct UID : public std::string {
    UID() : std::string()
    {}
    template<typename T>
    UID( const T &t ) : std::string(unify(t))
    {}
    UID( const UID &t ) : std::string(t)
    {}
    template<typename T>
    bool operator<( const T &t ) const {
        const std::string &self = *this;
        return self < (std::string)UID(t);
    }
    template<typename T>
    bool operator==( const T &t ) const {
        const std::string &self = *this;
        return self == (std::string)UID(t);
    }
    template<typename T>
    bool operator!=( const T &t ) const {
        const std::string &self = *this;
        return self != (std::string)UID(t);
    }
    operator const std::string() const {
        return *this;
    }
    operator std::string() {
        return *this;
    }
};
// }

// Simple filesystem dictionary, hypothetical sugar class {
struct disk {
    std::map< std::string, std::string > map;
    void add( const std::string &uri ) {
        map[ unify(uri) ] = uri;
    };
    std::string lookup( const std::string &uid_or_uri ) const {
        auto find = map.find( unify(uid_or_uri) );
        return find == map.end() ? std::string() : find->second;
    };
};
// }

#include <cassert>
#include <iostream>

int main(int argc, const char **argv) {

    // unified folder/asset separators
    std::string test = unify("folder\\asset");
    assert( test == unify("folder/asset") );
    assert( test == unify("folder-asset") );
    assert( test == unify("folder|asset") );
    assert( test == unify("folder:asset") );
    assert( test == unify("folder;asset") );
    assert( test == unify("folder,asset") );
    assert( test == unify("[folder]asset") );
    assert( test == unify("asset(folder)") );
    // -> asset-folder

    // unified absolute, relative, virtual and remote paths
    test = unify("~home/game/folder/asset.jpg");
    assert( test == unify("~user/game1/folder/asset.jpg") );
    assert( test == unify("~mark/game2/folder/asset.jpg") );
    assert( test == unify("~john/game3/data/folder/asset.jpg") );
    assert( test == unify("../folder/asset.jpg") );
    assert( test == unify("C:\\data\\folder\\asset.jpg") );
    assert( test == unify("C:/game/data/folder/asset.jpg") );
    assert( test == unify("data.zip/data/folder/asset.jpg") );
    assert( test == unify("virtual.rar/folder/asset.jpg") );
    assert( test == unify("http://web.domain.com%20/folder/asset.jpg?blabla=123&abc=123#qwe") );
    // -> asset-folder

    // unified uppercases, lowercases, whitespaces and hyphens
    assert( unify("mesh/main-character") == "character-main-mesh" );
    assert( unify("mesh/main_character") == "character-main-mesh" );
    assert( unify("mesh/Main Character") == "character-main-mesh" );
    assert( unify("mesh / Main  character ") == "character-main-mesh" );
    // -> character-main-mesh

    // unified extensions
    assert( unify("music/theme.ogg") == "music-theme" );
    assert( unify("music/theme.wav") == "music-theme" );
    assert( unify("ui/logo.png") == "logo-ui" );
    assert( unify("ui/logo.webp") == "logo-ui" );
    // -> music-theme, -> logo-ui

    // unified typos on double extensions and double punctuations
    assert( unify("game/logo.bmp.png") == unify("game/logo.bmp") );
    assert( unify("game/logo.png") == unify("game/logo..png") );
    // -> game-logo

    // unified typos on many diacritics
    assert( unify("âñimátïón/wàlk") == unify("animation/walk") );
    // -> animation-walk

    // unified AoS (OO) and SoA (ECS) disk layouts
    // unified plurals as well (if using English words)
    assert( unify("sounds/kid")  == unify("kid/sound") );
    assert( unify("sprites/kid") == unify("kid/sprite") );
    assert( unify("sounds/car")  == unify("car/sound") );
    assert( unify("sprites/car") == unify("car/sprite") );
    // -> car-sound, car-sprite, kid-sound, kid-sprite

    // unified SOV, SVO, VSO, VOS, OVS, OSV subject/verb/object language topologies
    test = unify("player-joins-scene.intro");
    assert( test == unify("player-scene-join.intro") );
    assert( test == unify("join-player-scene.intro") );
    assert( test == unify("join-scene-player.intro") );
    assert( test == unify("scene-join-player.intro") );
    assert( test == unify("scene-player-join.intro") );
    // -> join-player-scene

    // unified tagging (useful when globbing and deploying files and/or directories)
    test = unify("splash/logo");
    assert( unify("/splash/#win32/logo") == test );
    assert( unify("splash #mobile/logo #win32=always.png") == test );
    // -> logo-splash

    // unified consistency. reunification as a lossless process
    assert( unify( unify("roses-are-red") ) == unify("roses-are-red") );
    // -> are-red-rose

    // demo: transparent UID conversion
    // UID stands for Unified Identifier.
    // UID is inmutable on code always, even if changed on disk.
    // UID can be converted from paths, URLs, URIs, and IDs
    {
        UID test = "game\\logo.bmp";
        assert( test == "game-logo" );
        test = "logo/game";
        assert( test == "game-logo" );
        test = "~home/game/folder/asset.jpg";
        assert( test == "~user/game1/folder/asset.jpg" );
        assert( test == "~mark/game2/folder/asset.jpg" );
        assert( test == "~john/game3/data/folder/asset.jpg" );
        assert( test == "../folder/asset.jpg" );
        assert( test == "C:\\data\\folder\\asset.jpg" );
        assert( test == "C:/game/data/folder/asset.jpg" );
        assert( test == "data.zip/data/folder/asset.jpg" );
        assert( test == "virtual.rar/folder/asset.jpg" );
        assert( test == "http://web.domain.com%20/folder/asset.jpg?blabla=123&abc=123#qwe" );
    }

    // demo: insertion & lookup from a virtual filesystem
    {
        disk d;
        d.add("./local/file.txt");
        d.add("./data/game/icon.png");
        d.add("./songs/main_theme.ogg");
        d.add("./game.zip/json #win32/inventory.json");
        d.add("./game.zip/logos #win32/big.webp");
        d.add("./game.zip/logos #mobile/small.png");
        /* pseudocode :
        for( all mounted filesystems ) {
            for( all monitored files in subdirs ) {
                d.add( file.full_path );
            }
        } */
        assert( d.lookup("local/file") == "./local/file.txt" );
        assert( d.lookup("local-file") == "./local/file.txt" );
        assert( d.lookup("file-local") == "./local/file.txt" );
        assert( d.lookup("../file/local") == "./local/file.txt" );
        assert( d.lookup("game/icon") == "./data/game/icon.png" );
        assert( d.lookup("game-icon") == "./data/game/icon.png" );
        assert( d.lookup("icon/game") == "./data/game/icon.png" );
        assert( d.lookup("icon-game") == "./data/game/icon.png" );
        assert( d.lookup("songs/main-theme") == "./songs/main_theme.ogg" );
        assert( d.lookup("inventory-json") == "./game.zip/json #win32/inventory.json" );
        assert( d.lookup("logos-big") == "./game.zip/logos #win32/big.webp" );
        assert( d.lookup("logos-big") == "./game.zip/logos #win32/big.webp" );
    }

    std::cout << "All ok." << std::endl;
}

#endif

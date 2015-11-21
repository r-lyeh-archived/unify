Unify :link: <a href="https://travis-ci.org/r-lyeh/unify"><img src="https://api.travis-ci.org/r-lyeh/unify.svg?branch=master" align="right" /></a>
=====

**Unify** is a C++11 function to normalize resouce identificators.

Unify transforms any physical resource string to a unified string, called UID (Unified ID). Any absolute, relative, virtual and/or networks paths, URI, URL or ID will transform to an UID. Basically `unify(src)` does a string transformation from given string to a sorted `[a-zA-Z0-9-]+` pattern, which is guaranteed to remain inmutable (on a high degree) on code, even if physical source is altered externally.

## Features
- [x] Unified folder/asset separators.
- [x] Unified absolute, relative, virtual and remote paths.
- [x] Unified uppercases, lowercases, whitespaces and hyphens.
- [x] Unified extensions.
- [x] Unified typos on double extensions and double punctuations.
- [x] Unified typos on many diacritics.
- [x] Unified AoS (OO) and SoA (ECS) disk layouts.
- [x] Unified plurals as well (if using English words).
- [x] Unified SOV, SVO, VSO, VOS, OVS, OSV subject/verb/object language topologies.
- [x] Unified tagging (useful when globbing and deploying files and/or directories)
- [x] Unified consistency. Reunification as a lossless process.
- [x] Unify is header-only, cross-platform and standalone.
- [x] Unify is ZLIB/LibPNG licensed.

## Public API
```c++
// Convert anything to an UID
// Additionally, if tags != null pushback all parsed tags found
string unify( const string &uri, vector<string> *tags = 0 );
```

## Quick tutorial TL;DR
```c++
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
```

## Showcase
- John is a lead coder and must finish his game at time.
- When dealing with asset identifiers and filesystems, John just wants to write the identifiers once and forget about them during development (even after deployment!). 
- To achieve this:
  - John names every asset in the game with an identifier.
  - John embeds Unify over a thin disk wrapper.
- From this point:
  - Everybody in the production team can `#tag` files and folders from now as much as needed (or even `#tag=with-attributes`). These tags will be automatically removed from identifers once unified and will help John later, specially when globbing files and prior to deploy a release (more on this later).
  - Whitespaces, underscores, uppercase characters, diacritics, double extensions (and a large list of mistakes) will be fixed automatically.
  - Artists will be able to change (on a high degree) the asset names and/or folders on disk at any the time without telling the coders (just because artists are always that consistent).
  - Programmers will be able to transcode files to different formats without telling their technical directors: like going from .etc1 (desktop) to .pvr (iOS) (just because programmers are always that efficent).
  - Technical directors will be able to encrypt, repack and compress the filesystem into a completely different filesystem 2 hours before the release without telling the CEO (just because directors are always that security concerned).
  - And CEOs just can still take that lovely bath on the beach with no interruptions.
- This is actually John's wrapper:

```c++
#include <map>
#include <string>
#include "unify.hpp"

// Simple filesystem dictionary
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

int main() {
    disk d;
    d.add("./local/file.txt");
    d.add("./data/game/icon.png");
    d.add("./songs/main_theme.ogg");
    d.add("./game.zip/json #win32/inventory.json");
    d.add("./game.zip/logos #win32/big.webp");
    d.add("./game.zip/logos #mobile/small.png");
    /* pseudocode:
    for( all mounted filesystems ) {
        for( all monitored files in subdirs ) {
            d.add( file.full_path );
        }
    } */
    // these virtual<->physical uris are now equivalent
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
```

- As a bonus side, John packages and updates DLCs in a breeze:
```
tools/7za a -tzip common.zip *#all*
tools/7za a -tzip win32.zip *#w32*
tools/7za a -tzip iphones.zip *#iphone*
tools/7za a -tzip xmas.zip *#xmas*
tools/7za a -tzip halloween.zip *#halloween*
```

- John's DLC updating code is roughly similar to:
```
wget -N http://website.com/dlc/common.zip
wget -N http://website.com/dlc/xmas.zip
wget -N http://website.com/dlc/halloween.zip
[ "$PROFILE" == "WIN32"  ] && wget -N http://website.com/dlc/win32.zip
[ "$PROFILE" == "IPHONE" ] && wget -N http://website.com/dlc/iphones.zip
```

- John compares asset optimizations from rev3 to rev4
```
dir *#3* && dir *#4*
```

- John checks how big are all textures on iphone5 build
```
dir *#iphone5*#textures*
```

## Appendix: On transformation
Transformation on reference implementation performs as follows:

1. Latinization (utf8)
2. Remove diacritics (utf8)
3. Unescape URL (utf8)
4. Remove url options (if any)
5. Lowercase contents
6. Strip tags in `#tag-123`, `#tag_456` or `#xbox360=yes` format.
7. Split path up to 2nd level.
8. Trim extensions and punctuators (if any).
9. Replace whitespaces with `-` hyphens.
9. Split string into tokens (with `-` hyphen separator).
10. Sort tokens array.
11. For every token, fix aos/soa plural (if any).
12. Join stems with `-` hyphen separator.

## Appendix: Full tagging proposal
```
name (#platforms)(#factories)(#contexts)(#alias)(#type)(#version)
```

A possible proposal for a family of optional tags for any UID, that would be:
- applied to `#platforms` device targets, and
- loaded into `#factories` program consumers, and
- used in `#contexts` program scenarios, and
- referred to as `#aliases` identification aliases, and
- kind of `#type` file format, and
- located in `#versions` revision numbers.

Note: there are no reserved keywords in **Unify**. Name tags are **application/project/company dependant** and have to be defined in anticipation.

## Changelog
- v1.0.1 (2015/11/21): Disabled diacritics for now. Also, x18 times faster
- v1.0.0 (2015/08/18): Initial commit

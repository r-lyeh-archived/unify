#include "unify.hpp"

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

#include <cassert>

int main() {
    // demo: transparent UID conversion
    // UID stands for Unified Identifier.
    // UID is inmutable on code always, even if changed on disk.
    // UID can be converted from paths, URLs, URIs, and IDs
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
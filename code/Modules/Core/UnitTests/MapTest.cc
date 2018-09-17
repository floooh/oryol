//------------------------------------------------------------------------------
//  MapTest.cc
//  Test Dictionary functionality
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/Map.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"

using namespace Oryol;

TEST(MapTest) {

    // test simple insertion of unique elements
    Map<int, int> map;
    CHECK(map.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(map.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(map.Size() == 0);
    CHECK(map.Empty());
    CHECK(map.Capacity() == 0);
    CHECK(!map.Contains(1));
    map.Add(0, 0);
    map.Add(3, 3);
    map.Add(8, 8);
    map.Add(6, 6);
    map.Add(4, 4);
    map.Add(1, 1);
    map.Add(2, 2);
    map.Add(7, 7);
    map.Add(5, 5);
    CHECK(map.Size() == 9);
    CHECK(map.Capacity() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(!map.Empty());
    CHECK(map.Contains(4));
    CHECK(!map.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map[i] == i);
    }
    
    // copy construct
    Map<int, int> map1(map);
    CHECK(map1.Size() == 9);
    CHECK(map1.Capacity() == 9); // copy trims
    CHECK(!map1.Empty());
    CHECK(map1.Contains(4));
    CHECK(!map1.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map1[i] == i);
    }
    
    // copy-assign
    Map<int, int> map2;
    map2 = map;
    CHECK(map2.Size() == 9);
    CHECK(map2.Capacity() == 9);    // copy trims
    CHECK(!map2.Empty());
    CHECK(map2.Contains(4));
    CHECK(!map2.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map2[i] == i);
    }
    
    // trim and clear
    map.Trim();
    CHECK(map.Size() == 9);
    CHECK(map.Capacity() == 9);
    CHECK(!map.Empty());
    CHECK(map.Contains(4));
    CHECK(!map.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map[i] == i);
    }
    map.Clear();
    CHECK(map.Size() == 0);
    CHECK(map.Capacity() == 9);
    CHECK(map.Empty());
    CHECK(!map.Contains(2));
    
    // move-assign
    map = std::move(map2);
    CHECK(map2.Size() == 0);
    CHECK(map2.Capacity() == 0);
    CHECK(map.Size() == 9);
    CHECK(map.Capacity() == 9);    // copy trims
    CHECK(!map.Empty());
    CHECK(map.Contains(4));
    CHECK(!map.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map[i] == i);
    }
    
    // index accessor functions
    for (int i = 0; i < 9; i++) {
        CHECK(map.KeyAtIndex(i) == i);
        CHECK(map.ValueAtIndex(i) == i);
    }
    
    // modify values
    map[5] = 6;
    CHECK(map[5] == 6);
    map.ValueAtIndex(7) = 6;
    CHECK(map[7] == 6);
    
    // check FindDuplicate
    CHECK(InvalidIndex == map.FindDuplicate(0));
    map.Add(5, 10);
    map.Add(5, 11);
    CHECK(5 == map.FindDuplicate(0));
    // test erase (single and duplicates)
    CHECK(map.Contains(5));
    CHECK(map.Size() == 11);
    map.Erase(5);   // this should remove all duplicates
    CHECK(!map.Contains(5));
    CHECK(map.Size() == 8);
    CHECK(InvalidIndex == map.FindDuplicate(0));
    
    // erase at front and back
    map.Erase(0);
    CHECK(map.Size() == 7);
    CHECK(!map.Contains(0));
    map.Erase(8);
    CHECK(map.Size() == 6);
    CHECK(!map.Contains(8));
    
    // test bulk add
    Map<int, int> map4;
    map4.BeginBulk();
    for (int i = 31; i >= 0; i--) {
        KeyValuePair<int, int> kvp(i, i);
        map4.AddBulk(kvp);
    }
    map4.EndBulk();
    for (int i = 0; i < 32; i++) {
        CHECK(map4.KeyAtIndex(i) == i);
        CHECK(map4.ValueAtIndex(i) == i);
        CHECK(map4[i] == i);
    }
    
    // FindIndex
    CHECK(map4.FindIndex(0) == 0);
    CHECK(map4.FindIndex(31) == 31);
    CHECK(map4.FindIndex(16) == 16);
    CHECK(map4.FindIndex(100) == InvalidIndex);
    
    // test InsertUnique
    CHECK(map4.AddUnique(32, 32));
    CHECK(!map4.AddUnique(32, 33));
    CHECK(map4[32] == 32);
    int index = map4.FindIndex(32);
    CHECK(InvalidIndex == map4.FindDuplicate(index));

    // test for https://github.com/floooh/oryol/issues/271
    Map<int32_t, String> testMap;
    for (int32_t counter = 0; counter < 20; ++counter) {
        testMap.Add(counter, "testString");
        testMap.Erase(counter);
    }

    // test removing the first element
    Map<int, int> map5;
    map5.Add(0, 0);
    map5.Add(1, 1);
    map5.Add(2, 2);
    map5.Add(3, 3);
    map5.Add(4, 4);
    map5.Add(5, 5);
    CHECK(map5.KeyAtIndex(0) == 0);
    CHECK(map5.KeyAtIndex(1) == 1);
    int key = map5.KeyAtIndex(0);
    map5.Erase(key);
    CHECK(map5.KeyAtIndex(0) == 1);
}

namespace MapTest {

class Locator {
public:
    static const uint32_t NonSharedSignature = 0xFFFFFFFF;
    static const uint32_t DefaultSignature = 0xFFFFFFFE;

    Locator();
    Locator(const StringAtom& location);
    Locator(const char* location);
    Locator(const StringAtom& location, uint32_t signature);
    Locator(const char* location, uint32_t signature);

    static Locator NonShared();
    static Locator NonShared(const StringAtom& location);

    Locator(const Locator& rhs);
    void operator=(const Locator& rhs);

    bool operator==(const Locator& rhs) const;
    bool operator!=(const Locator& rhs) const;
    bool operator<(const Locator& rhs) const;

    bool IsShared() const;
    bool HasValidLocation() const;
    const StringAtom& Location() const;
    uint32_t Signature() const;

private:
    StringAtom location;
    uint32_t signature;
};

inline Locator::Locator(const StringAtom& loc) :
location(loc),
signature(DefaultSignature) {
    // empty
}

inline Locator::Locator(const char* loc) :
location(loc),
signature(DefaultSignature) {
    // empty
}

inline Locator::Locator(const StringAtom& loc, uint32_t sig) :
location(loc),
signature(sig) {
    // empty
}

inline Locator::Locator(const char* loc, uint32_t sig) :
location(loc),
signature(sig) {
    // empty
}

inline Locator Locator::NonShared() {
    return Locator(StringAtom(), NonSharedSignature);
}

inline Locator Locator::NonShared(const StringAtom& loc) {
    return Locator(loc, NonSharedSignature);
}

inline Locator::Locator() :
signature(DefaultSignature) {
    // empty
}

inline Locator::Locator(const Locator& rhs) :
location(rhs.location),
signature(rhs.signature) {
    // empty
}

inline void Locator::operator=(const Locator& rhs) {
    this->location = rhs.location;
    this->signature = rhs.signature;
}

inline bool Locator::operator==(const Locator& rhs) const {
    return (this->location == rhs.location) && (this->signature == rhs.signature);
}

inline bool Locator::operator!=(const Locator& rhs) const {
    return (this->location != rhs.location) || (this->signature != rhs.signature);
}

inline bool Locator::operator<(const Locator& rhs) const {
    if (this->location == rhs.location) {
        return this->signature < rhs.signature;
    }
    else {
        return this->location < rhs.location;
    }
}

inline bool Locator::IsShared() const {
    return NonSharedSignature != this->signature;
}

inline bool Locator::HasValidLocation() const {
    return this->location.IsValid();
}

inline const StringAtom& Locator::Location() const {
    return this->location;
}

inline uint32_t Locator::Signature() const {
    return this->signature;
}

TEST(MapTest2) {
    Locator l0("IMUIShader");
    Locator l1("LambertShader");
    Locator l2("model", 1);
    Locator l3("model", 2);
    Map<Locator, int> map;
    map.Add(l0, 2);
    map.Add(l1, 4);
    map.Add(l2, 6);
    map.Add(l3, 7);
    CHECK(map.KeyAtIndex(0) == l0);
    CHECK(map.KeyAtIndex(1) == l1);
    CHECK(map.KeyAtIndex(2) == l2);
    CHECK(map.KeyAtIndex(3) == l3);
    CHECK(map.ValueAtIndex(0) == 2);
    CHECK(map.ValueAtIndex(1) == 4);
    CHECK(map.ValueAtIndex(2) == 6);
    CHECK(map.ValueAtIndex(3) == 7);
    map.Erase(l0);
    CHECK(map.KeyAtIndex(0) == l1);
    CHECK(map.KeyAtIndex(1) == l2);
    CHECK(map.KeyAtIndex(2) == l3);
    CHECK(map.ValueAtIndex(0) == 4);
    CHECK(map.ValueAtIndex(1) == 6);
    CHECK(map.ValueAtIndex(2) == 7);
}

} // namespace MapTest

//
// Created by Sam Bock on 8/9/16.
//

#include <catch.hpp>
#include <containers/Talea.h>
#include <containers/TaleaGrid.h>
#include <thread>
#include <Noto.h>

TEST_CASE("TaleaInitialization","[talea]") {

    Talea<int,true> talea(3, VoxelCoord(1,2,3));

    REQUIRE(talea(4,3,2) == 3);

    talea.set(4,3,2, 9);

    REQUIRE(talea(4,3,2) == 9);
    REQUIRE(talea(5,3,2) == 3);
    REQUIRE(talea(TaleaDim-1,TaleaDim-1,TaleaDim-1) == 3);
    REQUIRE(talea(0,0,0) == 3);
}

TEST_CASE("TaleaModificationTracking","[talea]") {

    Talea<int,true> talea(3, VoxelCoord(1,2,3));

    talea.set(4,3,2,9);
    REQUIRE(talea.modificationCount == 0); // modification tracking not yet enabled, shouldn't count
    talea.trackingEnabled = true;

    talea.set(3,3,3,3);
    REQUIRE(talea.modificationCount == 0); // setting to default value is not a change

    talea.set(4,4,4,4);
    REQUIRE(talea.modificationCount == 1); // set to non-default, should increment
}

TEST_CASE("TaleaWriteTracking","[talea]") {
    Talea<int,false> talea(3, VoxelCoord(1,2,3));

    talea.set(4,3,2,9);
    REQUIRE(talea.modificationCount == 0);

    talea.trackingEnabled = true;

    talea.set(4,3,2,9);
    REQUIRE(talea.modificationCount == 1);
}

TEST_CASE("TaleaGridReadWrite","[talea]") {
    TaleaGrid<int,true,256> grid(3);

    auto& tal = grid.getTaleaRW(5,5,5);
    REQUIRE(tal.position == VoxelCoord(0,0,0));

    REQUIRE(tal(1,1,1) == 3);
    tal.set(1,1,1, 5);

    auto& tal2 = grid.getTaleaRO(2,2,2);
    REQUIRE(tal2.position == VoxelCoord(0,0,0));
    REQUIRE(tal2(1,1,1) == 5);

    REQUIRE(grid(1,1,1) == 5);
    REQUIRE(grid(0,0,0) == 3);
    REQUIRE(grid(-5,-5,-5) == 3);

    auto& tal3 = grid.getTaleaRW(-10,-10,-10);

    REQUIRE(tal3.position == VoxelCoord(-TaleaDim,-TaleaDim,-TaleaDim));

    auto& tal4 = grid.getTaleaRO(-50,-50,-50);

    REQUIRE(tal4.isSentinel);

    auto& tal5 = grid.getTaleaRW(-1000,-1000,-1000);

    REQUIRE(tal5(4,5,6) == 3);
    tal5.set(5,6,7, 20);
    REQUIRE(tal5(5,6,7) == 20);

    REQUIRE(grid(tal5.position.x + 5, tal5.position.y + 6, tal5.position.z + 7) == 20);

    auto iter = grid.iter();
    std::vector<Talea<int,true>*> allTaleae;
    while (iter.hasNext()) {
        allTaleae.push_back(iter.next());
    }

    // two in core taleae, one in non main taleae
    REQUIRE(allTaleae.size() == 3);
}

TEST_CASE("TaleaGridConcurrency","[talea][concurrency]") {
    TaleaGrid<int,true,256> grid(0);
    bool failureEncountered = false;

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.push_back(std::thread([&](){
            Arx::Map<VoxelCoord,int> desiredValues;
            for (int j = 0; j < 1000; ++j) {
                int rx = rand() % 300;
                int ry = rand() % 300;
                int rz = rand() % 256;

                int nv = rand()%32 + 1;
                grid.set(rx,ry,rz, nv);
                desiredValues.put(VoxelCoord(rx,ry,rz), nv);
            }

            auto iter = desiredValues.begin();
            while (iter != desiredValues.end()) {
                int foundValue = grid(iter->first);
                if (foundValue == 0) {
                    failureEncountered = true;
                }
                iter++;
            }
        }));
    }

    for (std::thread& thread : threads) {
        thread.join();
    }

    REQUIRE(!failureEncountered);
}


#ifndef BLOCKY_BLOCKS_H
#define BLOCKY_BLOCKS_H

enum BlockType
{
    EMPTY,
    AIR,
    DIRT,
    GRASS,
    STONE,
    TARGET
};

struct BlockContainer
{
    BlockType type;
    int count;
};

#endif //BLOCKY_BLOCKS_H

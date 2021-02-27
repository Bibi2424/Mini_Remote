#ifndef MENU_H
#define MENU_H

#define COLOR(r, g, b)  ((((b) >> 3) << (3+8)) | (((g) >> 2) << 5) | ((r) >> 3))
#define BLACK   COLOR(0, 0, 0)
#define BLUE    COLOR(0, 0, 255)
#define RED     COLOR(255, 0, 0)
#define GREEN   COLOR(0, 255, 0)
#define CYAN    COLOR(0, 255, 255)
#define MAGENTA COLOR(255, 255, 0)
#define YELLOW  COLOR(255, 255, 0)
#define WHITE   COLOR(255, 255, 255)

typedef enum {
    ROOT,
    SUBMENU,
    INT,
    CUSTOM
}MENU_TYPE;

typedef struct menu_item_t {
    struct menu_item_t *parent;
    struct menu_item_t *child;
    MENU_TYPE type;
    void* content;
}menu_item_t;


extern void menu_init(void);


#endif
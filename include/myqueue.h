//Struct definitions
struct measurement {
        int timestamp;
        int value;
};
typedef struct measurement measurement;
struct node {
        struct measurement m;
        struct node *next;
};
typedef struct node node;

struct myqueue {
        int size;
        node * head;
};
typedef struct myqueue myqueue;

//function prototypes
void myqueueinsert(myqueue * queue, measurement m);
measurement myqueueremove(myqueue * queue);
myqueue * createQueue();

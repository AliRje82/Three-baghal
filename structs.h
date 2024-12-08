struct grocery
{
    char *name;
    int count;
};

struct userInfo
{
    struct grocery **groceries;
    int user_id;
    int order_id;
    int budget;
    int n;
};

struct item{
    double score;
    double price;
    char *name;
};

struct recipt
{
    struct item **items;
    int n;
};



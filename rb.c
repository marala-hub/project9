#include "DS.h"

#define RED 1
#define BLACK 0

static int isRed(Elm *node);
static Elm *rotateLeft(Elm *h);
static Elm *rotateRight(Elm *h);
static void flipColors(Elm *h);
static Elm *put(Elm *h, const Student *px);
static Elm *moveRedLeft(Elm *h);
static Elm *moveRedRight(Elm *h);
static Elm *balance(Elm *h);
static Elm *min(Elm *x);
static Elm *deleteMin(Elm *h);
static Elm *delete(Elm *h, const char id[]);

/*
  Хоёр оюутны мэдээллийг id-аар харьцуулах функц
*/
int less(const Student *a, const Student *b)
{
    return strcmp(a->id, b->id) < 0;
}

static int isRed(Elm *node)
{
    if (node == NULL) return 0;
    return node->color == RED;
}

static Elm *rotateLeft(Elm *h)
{
    Elm *x = h->R;
    h->R = x->L;
    x->L = h;
    x->color = h->color;
    h->color = RED;
    return x;
}

static Elm *rotateRight(Elm *h)
{
    Elm *x = h->L;
    h->L = x->R;
    x->R = h;
    x->color = h->color;
    h->color = RED;
    return x;
}

/* Эцэг болон хүүхдүүдийн өнгийг эсрэгээр нь хөрвүүлнэ.
   Оруулахад: хар эцэг + 2 улаан хүүхэд -> улаан эцэг + 2 хар хүүхэд
   Устгахад:  улаан эцэг + 2 хар хүүхэд -> хар эцэг + 2 улаан хүүхэд */
static void flipColors(Elm *h)
{
    h->color ^= 1;
    if (h->L) h->L->color ^= 1;
    if (h->R) h->R->color ^= 1;
}

static Elm *put(Elm *h, const Student *px)
{
    if (h == NULL) {
        Elm *newNode = (Elm *)malloc(sizeof(Elm));
        newNode->x = *px;
        newNode->color = RED;
        newNode->L = NULL;
        newNode->R = NULL;
        return newNode;
    }

    int cmp = strcmp(px->id, h->x.id);
    if (cmp < 0) {
        h->L = put(h->L, px);
    } else if (cmp > 0) {
        h->R = put(h->R, px);
    } else {
        h->x = *px; /* Ижил ID байвал утгыг шинэчилнэ */
    }

    if (isRed(h->R) && !isRed(h->L)) h = rotateLeft(h);
    if (isRed(h->L) && isRed(h->L->L)) h = rotateRight(h);
    if (isRed(h->L) && isRed(h->R)) flipColors(h);

    return h;
}

/*
  `ptree`-ийн зааж байгаа модонд `px` утгыг оруулна.
 */
void rb_put(RBT *ptree, const Student *px)
{
    ptree->root = put(ptree->root, px);
    ptree->root->color = BLACK;
}

/*
  `ptree`-ийн зааж байгаа модноос `id`-аар хайн олдсон оройн `Elm*` хаягийг буцаана.
 */
Elm *rb_get(const RBT *ptree, const char id[])
{
    Elm *curr = ptree->root;
    while (curr != NULL) {
        int cmp = strcmp(id, curr->x.id);
        if (cmp < 0) {
            curr = curr->L;
        } else if (cmp > 0) {
            curr = curr->R;
        } else {
            return curr;
        }
    }
    return NULL;
}

static Elm *moveRedLeft(Elm *h)
{
    flipColors(h);
    if (h->R && isRed(h->R->L)) {
        h->R = rotateRight(h->R);
        h = rotateLeft(h);
        flipColors(h);
    }
    return h;
}

static Elm *moveRedRight(Elm *h)
{
    flipColors(h);
    if (h->L && isRed(h->L->L)) {
        h = rotateRight(h);
        flipColors(h);
    }
    return h;
}

static Elm *balance(Elm *h)
{
    if (isRed(h->R) && !isRed(h->L)) h = rotateLeft(h);
    if (isRed(h->L) && isRed(h->L->L)) h = rotateRight(h);
    if (isRed(h->L) && isRed(h->R)) flipColors(h);
    return h;
}

static Elm *min(Elm *x)
{
    while (x->L != NULL) x = x->L;
    return x;
}

static Elm *deleteMin(Elm *h)
{
    if (h->L == NULL) {
        free(h);
        return NULL;
    }
    if (!isRed(h->L) && !isRed(h->L->L))
        h = moveRedLeft(h);
    h->L = deleteMin(h->L);
    return balance(h);
}

/* Устгах id модонд заавал байгаа гэж үзнэ (rb_del дотор шалгана) */
static Elm *delete(Elm *h, const char id[])
{
    if (strcmp(id, h->x.id) < 0) {
        if (h->L != NULL && !isRed(h->L) && !isRed(h->L->L))
            h = moveRedLeft(h);
        h->L = delete(h->L, id);
    } else {
        if (isRed(h->L))
            h = rotateRight(h);

        /* rotateRight-ийн дараа h өөрчлөгдсөн тул дахин харьцуулна */
        if (strcmp(id, h->x.id) == 0 && h->R == NULL) {
            free(h);
            return NULL;
        }

        if (h->R != NULL && !isRed(h->R) && !isRed(h->R->L))
            h = moveRedRight(h);

        /* moveRedRight-ийн дараа бас дахин харьцуулна */
        if (strcmp(id, h->x.id) == 0) {
            Elm *m = min(h->R);
            h->x = m->x;
            h->R = deleteMin(h->R);
        } else {
            h->R = delete(h->R, id);
        }
    }
    return balance(h);
}

/*
  Устгах функц: ТМноос `id`-аар хайж олоод устгана.
 */
void rb_del(RBT *ptree, const char id[])
{
    if (rb_get(ptree, id) == NULL) return;

    /* Үндсийн хоёр хүүхэд хар бол үндсийг түр улаан болгоно */
    if (!isRed(ptree->root->L) && !isRed(ptree->root->R))
        ptree->root->color = RED;

    ptree->root = delete(ptree->root, id);
    if (ptree->root != NULL)
        ptree->root->color = BLACK;
}

void print(Elm *p)
{
    if (p)
        printf("%s %s %d %.1f\n", p->x.name, p->x.id, p->x.age, p->x.gpa);
    else
        printf("None\n");
}

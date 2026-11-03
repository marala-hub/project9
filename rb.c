#include "DS.h"
#define RED 1
#define BLACK 0

static int isRed(Elm *node);
static Elm *rotateleft(Elm *h);
static Elm *rotateRight(Elm *h);
static void flipColors(Elm *h);
static Elm *put(Elm *h, const Student *px);
static Elm  *min(Elm *x);
static Elm *deleteMin(Elm *h);
static Elm *delete(Elm *h, const char id[]);
static void clearTree(Elm *h);

/*
  Хоёр оюутны мэдээллийн хооронд нь харьцуулах функц
*/
int less(const Student *a, const Student *b)
{
	return strcmp(a->id, b->id)<0;
}
static int isRed(Elm *node)
{
if (node ==NULL) return 0;
return node->color==RED;
}
static Elm *rotateLeft(Elm *h)
{
Elm *x=h->R;
h->R=x->L;
x->L=h;
x->color=h->color;
h->color=RED;
return x;
}
static void flipColors(Elm *h)
{
    h->color = RED;
    if (h->L) h->L->color = BLACK;
    if (h->R) h->R->color = BLACK;
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
        h->x = *px; // Ижил ID байвал утгыг шинэчилнэ
    }

    if (isRed(h->R) && !isRed(h->L)) h = rotateLeft(h);
    if (isRed(h->L) && isRed(h->L->L)) h = rotateRight(h);
    if (isRed(h->L) && isRed(h->R)) flipColors(h);

    return h;
}


/*
  `ptree`-ийн зааж байгаа модонд `x` утгыг оруулна.
  Мод хоосон байсан бол `ptree->root` хаяг өөрчлөгдөж шинээр орсон оройг заана.
  Хэрэв мод тэнцвэрээ алдсан бол тохирох тэнцвэржүүлэх үйлдлүүдийг хийнэ.
 */
void rb_put(RBT *ptree, const Student *px)
{
        ptree->root=put(ptree->root, px);
	if(ptree->root !=NULL){
		ptree->root->color=BLACK;
	}
}

/*
  `ptree`-ийн зааж байгаа модноос `x` утгыг хайн олдсон оройн `Elm*` хаягийг буцаана.
  Олдохгүй бол `NULL` хаягийг буцаана.
  Мод дандаа ялгаатай элементүүд хадгална гэж үзэж болно.
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
    if (isRed(h->R)) h = rotateLeft(h);
    if (isRed(h->L) && isRed(h->L->L)) h = rotateRight(h);
    if (isRed(h->L) && isRed(h->R)) flipColors(h);
    return h;
}

static Elm *min(Elm *x)
{
    if (x->L == NULL) return x;
    else return min(x->L);
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

static Elm *delete(Elm *h, const char id[])
{
    if (h == NULL) return NULL;

    if (strcmp(id, h->x.id) < 0) {
        if (h->L != NULL) {
            if (!isRed(h->L) && !isRed(h->L->L))
                h = moveRedLeft(h);
            h->L = delete(h->L, id);
        }
    } else {
        if (isRed(h->L))
            h = rotateRight(h);
        if (strcmp(id, h->x.id) == 0 && (h->R == NULL)) {
            free(h);
            return NULL;
        }
        if (h->R != NULL) {
            if (!isRed(h->R) && !isRed(h->R->L))
                h = moveRedRight(h);
            if (strcmp(id, h->x.id) == 0) {
                Elm *x = min(h->R);
                h->x = x->x;
                h->R = deleteMin(h->R);
            } else {
                h->R = delete(h->R, id);
            }
        }
    }
    return balance(h);
}

/*
  Устгах функц: ТМноос `x` утгыг хайж олоод устгана.
  Олдохгүй бол юу ч хийхгүй.
  Хэрэв мод тэнцвэрээ алдсан бол тохирох тэнцвэржүүлэх үйлдлүүдийг хийнэ.
 */
void rb_del(RBT *ptree, const char id[])
{
        if (rb_get(ptree, id) != NULL) {
        ptree->root = delete(ptree->root, id);
        if (ptree->root != NULL) {
            ptree->root->color = BLACK;
        }
    }
}

void print(Elm *p)
{
	if (p)
		printf("%s %s %d %.1f\n", p->x.name, p->x.id, p->x.age, p->x.gpa);
	else
		printf("None\n");
}

/* 
 * tkCanvLayout.c --
 *
 *      This module provides Layouting support for canvases,
 *      including the "layout" widget command plus a few utility
 *      procedures used for layouting icons.
 *
 * Copyright (c) 1993 by Sven Delmas
 * All rights reserved.
 * See the file COPYRIGHT for the copyright notes.
 *
 */

#ifndef lint
static char *AtFSid = "$Header: tkCanvLayout.c[1.8] Wed Jun  7 11:29:17 1995 sven@(none) accessed $";
#endif

#include <stdio.h>
#include "tkInt.h"
#include "tkCanvas.h"
#include "tkPort.h"

/*
 * This value is added to the line length in pixels, so the distance
 * between two nodes can be calculated correctly.
 */

#define LINE_INCREMENT 10

/*
 * Turn on/off debugging.
 */

#define DEBUGGING 0
#define DEBUG_LAYOUT 0
#define DEBUG_PLACE 0
#define TESTING 0

/*
 * A more portable random
 */

#define RANDOM() rand()
#define SRANDOM(a) srand((unsigned int) a)

/*
 * The datastructures used for layouting.
 */

/*
 * These datas/variables are used by the ISI layouter.
 */


/*
 * These datas/variables are used by the tree layouter.
 */

typedef struct {
  double tmpX;                        /* A temporary x position. */
  double tmpY;                        /* A temporary y position. */
} TreeData;

static int treeDataNum;               /* The number of tree nodes. */
static TreeData **treeData;           /* The list of tree datas. */

#define TREE_TMP_X_POS(node)           ((TreeData *) node->data)->tmpX
#define SET_TREE_TMP_X_POS(node, pos)  ((TreeData *) node->data)->tmpX=pos
#define TREE_TMP_Y_POS(node)           ((TreeData *) node->data)->tmpY
#define SET_TREE_TMP_Y_POS(node, pos)  ((TreeData *) node->data)->tmpY=pos
#if DEBUGGING
#define DEBUG_PRINT_TREE_NODE_POS(node, s) TkCanvLayoutDebugging(node, s, 1)
#else
#define DEBUG_PRINT_TREE_NODE_POS(node, s)
#endif

/*
 * These datas/variables are used by the random layouter.
 */

static int keepRandomPositions;       /* Don't change the position of */
                                      /* already placed icons when */
                                      /* layouting with the random */
                                      /* placer. */
static int maxX, maxY;                /* Maximal X and Y coordinates */
                                      /* for the random placer. */


/*
 * A topologically ordered node. Stored in the global array topList
 * (0 to topListNum).
 */

typedef struct {
  struct Nodes *nodePtr;
} Topology;

/*
 * A successor node of a node. Stored in a array attached to the
 * specific node.
 */

typedef struct {
  int ignore;                         /* Ignore this successor (edge). */
  int visited;                        /* This edge was visited. */
  struct Nodes *nodePtr;              /* A pointer to the node struct. */
} Succ;

/*
 * A parent node of a node. Stored in a array attached to the
 * specific node.
 */

typedef struct {
  int ignore;                         /* Ignore this parent (edge). */
  int visited;                        /* This edge was visited. */
  struct Nodes *nodePtr;              /* A pointer to the node struct. */
} Parent;

/*
 * A graph node. Stored in a global array named nodes
 * (0 to nodeNum).
 */

typedef struct Nodes {
  Tk_Item *itemPtr;                   /* Pointer to the canvas item. */
  int ignore;                         /* Ignore this node. */
  int visited;                        /* This node was already
                                       * visited/layouted. */
  double x;                           /* The calculated x position. */
  double y;                           /* The calculated y position. */
  double width;                       /* The nodes width. */
  double height;                      /* The nodes height. */
  int parentNum;                      /* The number of parent nodes. */
  Parent **parent;                    /* The array of parent nodes. */
  int succNum;                        /* The number of successor nodes. */
  Succ **succ;                        /* The array of successor nodes. */
  char *data;                         /* Special data attached to
                                       * this node. The contents
                                       * depend from the layouting
                                       * algorithms. */
} Node;

/*
 * Global variables used for layouting.
 */

static int nodeNum;                   /* The number of graph nodes. */
static Node **nodes;                  /* The list of graph nodes. */

static int topListNum;                /* The current node number. */
static Topology **topList;            /* The sorted nodes. */

static int computeiconsize;           /* Use the biggest icon size. */
static int elementsPerLine;           /* How many elements per line. */
static int edgeHeight;                /* The standard height of an edge. */
static int edgeOrder;                 /* Set the edges to the layout order. */
static int edgeWidth;                 /* The standard width of an edge. */
static int iconHeight, iconWidth;     /* The standard icon size. */
static int iconSpaceV;                /* The vertical space between icons. */
static int iconSpaceH;                /* The horizontal space between icons. */
static int graphOrder;                /* Ordering... 0 = LR, 1 = TD. */
static int posX1, posY1, posX2, posY2;/* Coordinates */

static long rootId;                   /* The root id for graph. */
static long xOffset;                  /* The x offset for the placing. */
static long yOffset;                  /* The y offset for the placing. */

static char *graphName;               /* The name of the graph. */
static char *idlist;                  /* The list of ids to layout. */
static char *sortcommand;             /* The Tcl procedure called for */
                                      /* sorting. */
static char convertBuffer[100];       /* Convert numbers to strings.*/

static double maxXPosition;           /* Maximal X and Y coordinates */
static double maxYPosition;           /* for the random placer. */

Node *rootNode;                       /* The root node. */

void (*createDataNode)();             /* Create additional data. */

int layoutTypesNum;                   /* The number of layout types. */
char **layoutTypes;                   /* The types of items that will
                                         be layouted. */

/*
 * Defines that hide internal functionality.
 */

/*
 * Get and set the edge ignore flag. Edges which are ignored will not
 * be traversed. The first parameter is the edge, and the second
 * parameter (if you call the setting) is the new value of the
 * ignore flag.
 */

#define IGNORE_EDGE(edge)             edge->ignore
#define SET_IGNORE_EDGE(edge,mode)    edge->ignore=mode

/*
 * Get and set the edge visited flag. This flag is usually set to true
 * when the edge was visited. The first parameter is the edge, and the
 * second parameter (if you call the setting) is the new value of the
 * visited flag.
 */

#define VISITED_EDGE(edge)            edge->visited
#define SET_VISITED_EDGE(edge,mode)   edge->visited=mode

/*
 * Get and set the node ignore flag. Nodes which are ignored will not
 * be traversed, and they are not placed/layouted. The first parameter
 * is the node, and the second parameter (if you call the setting) is
 * the new value of the ignore flag.
 */

#define IGNORE_NODE(node)             node->ignore
#define SET_IGNORE_NODE(node,mode)    node->ignore=mode
#define RESET_IGNORE_NODE(i)          for (i = 0; i < nodeNum; i++) nodes[i]->ignore=0;

/*
 * Get and set the node visited flag. This flag is usually set to true
 * when the node was visited. Currently this flag is mainly used for the
 * topological sorting. The first parameter is the node, and the
 * second parameter (if you call the setting) is the new value of the
 * visited flag.
 */

#define VISITED_NODE(node)            node->visited
#define SET_VISITED_NODE(node,mode)   node->visited=mode
#define RESET_VISITED_NODE(i)         for (i = 0; i < nodeNum; i++) nodes[i]->visited=0;

/*
 * Get and set the number of parent nodes. The first parameter is the
 * node, and the second parameter (if you call the setting) is the new
 * number of parents.
 */

#define PARENT_NUM(node)              node->parentNum
#define SET_PARENT_NUM(node,num)      node->parentNum=num

/*
 * Get and set the number of successor nodes. The first parameter is
 * the node, and the second parameter (if you call the setting) is the
 * new number of successors.
 */

#define SUCC_NUM(node)                node->succNum
#define SET_SUCC_NUM(node,num)        node->succNum=num

/*
 * Get and set the node item. This item is the corresponding canvas
 * item for this node. The first parameter is the node, and the second
 * parameter (if you call the setting) is the new canvas item pointer.
 */

#define NODE_ITEM(node)               node->itemPtr
#define SET_NODE_ITEM(node,item)      node->itemPtr=item

/*
 * Get and set the node x position. This is the value for the final
 * placing of the node. The first parameter is the node, and the
 * second parameter (if you call the setting) is the new x position.
 */

#define NODE_X_POS(node)              node->x
#define SET_NODE_X_POS(node,pos)      node->x=pos

/*
 * Get and set the node y position. This is the value for the final
 * placing of the node. The first parameter is the node, and the
 * second parameter (if you call the setting) is the new y position.
 */

#define NODE_Y_POS(node)              node->y
#define SET_NODE_Y_POS(node,pos)      node->y=pos

/*
 * Get and set the nodes width. The parameter specifies the node
 * whose width and height will be returned.
 */

#define NODE_WIDTH(node)              node->width
#define SET_NODE_WIDTH(node,size)     node->width=size

/*
 * Get and set the nodes height. The parameter specifies the node
 * whose width and height will be returned.
 */

#define NODE_HEIGHT(node)             node->height
#define SET_NODE_HEIGHT(node,size)    node->height=size

/*
 * Return the parent/successor edge/node for the specified node. The
 * second parameter is a integer counter that is used as index for the
 * parent/successor array. Usually this index is generated by a
 * FOR_ALL_* macro.
 */

#define PARENT_EDGE(node, i)          node->parent[i]
#define PARENT_NODE(node, i)          node->parent[i]->nodePtr
#define SUCC_EDGE(node, i)            node->succ[i]
#define SUCC_NODE(node, i)            node->succ[i]->nodePtr
#define DUMMY_NODE(node)              (node->itemPtr == (Tk_Item *) NULL)

/*
 * Return the node specified by the integer counter passed to this
 * macro. The nodes in the array are topologically ordered (beginning
 * with the index 0. The index is usually created by the macro
 * FOR_ALL_TOP_NODES.
 */

#define TOP_NODE(i)                   topList[i]->nodePtr

/*
 * Walk through all nodes that are currently defined. The only
 * parameter is the integer counter that is used to index the array.
 */

#define FOR_ALL_NODES(i)              for (i = 0; i < nodeNum; i++)

/*
 * Walk through all parents/successors of the specified node. The
 * second parameter is the integer counter that is used to index the
 * array.
 */

#define FOR_ALL_PARENTS(node, i)      for (i = 0; i < node->parentNum; i++)
#define FOR_ALL_SUCCS(node, i)        for (i = 0; i < node->succNum; i++)

/*
 * Walk through all nodes in topological order. The only parameter is
 * the integer counter that is used to index the array. This call
 * requires a call of the topological order function, before it can be
 * used.
 */

#define FOR_ALL_TOP_NODES(i)          for (i = 0; i < topListNum; i++)

/*
 * Walk through all canvas items. The first parameter is the canvas
 * pointer that should be scanned, and the second parameter is the
 * item pointer that contains the current canvas item.
 */

#define FOR_ALL_CANVAS_ITEMS(canv,i)  for (i = canv->firstItemPtr; i != NULL; i = i->nextPtr)

/*
 * Get the canvas item parameters inidcated by the type of the macro.
 * The only parameter is the canvas item pointer.
 */

#define CANVAS_ITEM_ID(i)             i->id
#define CANVAS_ITEM_TYPE(i)           i->typePtr->name
#define CANVAS_ITEM_X_POS(i)          i->x1
#define CANVAS_ITEM_Y_POS(i)          i->y1
#define CANVAS_ITEM_HEIGHT(i)         (i->y2 - i->y1)
#define CANVAS_ITEM_WIDTH(i)          (i->x2 - i->x1)

/*
 * Debugging macros.
 */

#if DEBUGGING
#define DEBUG_PRINT_NODE_POS(node, s) TkCanvLayoutDebugging(node, s, 0)
#define DEBUG_PRINT_STRING(s1, s2)    fprintf(stderr, "%s %s<\n", s2, s1);
#else
#define DEBUG_PRINT_NODE_POS(node, s)
#define DEBUG_PRINT_STRING(s1, s2)
#endif

/*
 * To create/insert a dummy node call the standard node creation
 * command (as shown below), and pass a from and a to node pointer.
 * The dummy node will be inserted between these nodes.
 */
/*
  TkCanvLayoutCreateNode(canvasPtr, interp, NULL,
                         nodes[counter], nodes[counter2]);
*/

/*
 * Forward declarations for procedures defined later in this file:
 */

static void TkCanvLayoutISISetX     _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp,
                                                 Node *currentNode));
static void TkCanvLayoutISISetY     _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp,
                                                 Node *currentNode));
static void TkCanvLayoutCreateTreeNode _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                    Tcl_Interp *interp,
                                                    Node *currentNode));
static void TkCanvLayoutTreeSetX    _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp,
                                                 Node *currentNode));
static void TkCanvLayoutTreeSetY    _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp,
                                                 Node *currentNode));
static void TkCanvLayoutCreateNode  _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp,
                                                 Tk_Item *itemPtr,
                                                 Node *fromNode,
                                                 Node *toNode));
static int TkCanvLayoutBuildGraph   _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp));
static void TkCanvLayoutFreeGraph   _ANSI_ARGS_(());
static Node *TkCanvLayoutGraphRoot  _ANSI_ARGS_((TkCanvas *canvasPtr));
static void TkCanvLayoutGraphSortTopological _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                          Tcl_Interp *interp,
                                                          Node *currentNode));
static int TkCanvLayoutGraphPlaceNodes _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                    Tcl_Interp *interp));
static int TkCanvLayoutGraphPlaceEdges _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                    Tcl_Interp *interp));
static int TkCanvLayoutEdgeWidth    _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp));
static int TkCanvLayoutEdge         _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp,
                                                 Tk_Item *curPtr,
                                                 Node *fromNode,
                                                 Node *toNode));
static int TkCanvLayoutISI          _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp));
static int TkCanvLayoutMatrix       _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp));
static int TkCanvLayoutRandom       _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp));
static int TkCanvLayoutTree         _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp));
int TkCanvLayoutCmd                 _ANSI_ARGS_((TkCanvas *canvasPtr,
                                                 Tcl_Interp *interp,
                                                 char *graph,
                                                 int argc,
                                                 char **argv));
#if DEBUGGING

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutDebugging --
 *
 *        This procedure is invoked to print debugging informations.
 *
 * Results:
 *        None
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

/* ARGSUSED */
static void
TkCanvLayoutDebugging(currentNode, string, type)
     Node *currentNode;                /* This is the current node. */
     char *string;
     int type;
{
  double tmpX, tmpY;
  
  if (graphOrder) {
    /* Place nodes top down. */
    tmpX = xOffset - NODE_HEIGHT(rootNode);
    tmpY = yOffset - NODE_WIDTH(rootNode);
  } else {
    /* Place nodes left to right. */
    tmpX = xOffset - NODE_WIDTH(rootNode);
    tmpY = yOffset - NODE_HEIGHT(rootNode);
  }
  
  if (!DUMMY_NODE(currentNode)) {
    fprintf(stderr, "%-6s Node %-3ld: x=%-g y=%-g x=%-g y=%-g\n",
            string, CANVAS_ITEM_ID(NODE_ITEM(currentNode)),
            NODE_X_POS(currentNode) + tmpX,
            NODE_Y_POS(currentNode) + tmpY,
            NODE_X_POS(currentNode),
            NODE_Y_POS(currentNode));
  } else {
    fprintf(stderr, "%-6s Node dummy: x=%-g y=%-g x=%-g y=%-g\n",
            string, NODE_X_POS(currentNode) + tmpX,
            NODE_Y_POS(currentNode) + tmpY,
            NODE_X_POS(currentNode),
            NODE_Y_POS(currentNode));
  }
  switch (type) {
  case 1:
    fprintf(stderr,
            "                 x=%-g y=%-g x=%-g y=%-g\n",
            TREE_TMP_X_POS(currentNode) + tmpX,
            TREE_TMP_Y_POS(currentNode) + tmpY,
            TREE_TMP_X_POS(currentNode),
            TREE_TMP_Y_POS(currentNode));
    break;
  default:
    break;
  }
}
#endif

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutISISetX --
 *
 *        This procedure is invoked to calculate the x ISI position.
 *
 * Results:
 *        None
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

/* ARGSUSED */
static void
TkCanvLayoutISISetX(canvasPtr, interp, currentNode)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
     Node *currentNode;                /* This is the current node. */
{
  int counter, visitedAllChilds = 1;
  
  if (IGNORE_NODE(currentNode)) {
    return;
  }
  if (VISITED_NODE(currentNode)) {
    return;
  }
  
  FOR_ALL_SUCCS(currentNode, counter) {
    /* Are there un layouted children ? */
    if (IGNORE_EDGE(SUCC_EDGE(currentNode, counter))) {
      continue;
    }
    if (IGNORE_NODE(SUCC_NODE(currentNode, counter))) {
      continue;
    }
    if (!VISITED_NODE(SUCC_NODE(currentNode, counter))) {
      visitedAllChilds = 0;
    }
  }
  
  SET_VISITED_NODE(currentNode, 1);  /* Used to prevent infinite recursion */
  if (!visitedAllChilds) {
    FOR_ALL_SUCCS(currentNode, counter) {
      /* Layout the children of this node. */
      if (IGNORE_EDGE(SUCC_EDGE(currentNode, counter))) {
        continue;
      }
      if (IGNORE_NODE(SUCC_NODE(currentNode, counter))) {
        continue;
      }
      if (!VISITED_NODE(SUCC_NODE(currentNode, counter))) {
        TkCanvLayoutISISetX(canvasPtr, interp,
                            SUCC_NODE(currentNode, counter));
      }
    }
    DEBUG_PRINT_NODE_POS(currentNode, "SetX");
    if (SUCC_NUM(currentNode) == 1) {
      SET_NODE_X_POS(currentNode,
                     NODE_X_POS(SUCC_NODE(currentNode, 0)));
    } else {
      SET_NODE_X_POS(currentNode,
                     NODE_X_POS(SUCC_NODE(currentNode, 0)) +
                     ((NODE_X_POS(SUCC_NODE(currentNode,
                                            SUCC_NUM(currentNode)-1)) -
                       NODE_X_POS(SUCC_NODE(currentNode, 0))) / 2));
    }
  } else {
    /* Set the x position of the current node. */
    DEBUG_PRINT_NODE_POS(currentNode, "SetX");
    if (graphOrder) {
      /* Place nodes top down. */
      SET_NODE_X_POS(currentNode, maxXPosition);
      maxXPosition = NODE_X_POS(currentNode) + iconSpaceH +
        edgeWidth + NODE_WIDTH(currentNode);
    } else {
      /* Place nodes left to right. */
      SET_NODE_X_POS(currentNode, maxXPosition);
      maxXPosition = NODE_X_POS(currentNode) + iconSpaceV +
        edgeHeight + NODE_HEIGHT(currentNode);
    }
  }
  SET_VISITED_NODE(currentNode, 1);
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutISISetY --
 *
 *        This procedure is invoked to calculate the y ISI position. 
 *
 * Results:
 *        None
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

/* ARGSUSED */
static void
TkCanvLayoutISISetY(canvasPtr, interp, currentNode)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
     Node *currentNode;                /* This is the current node. */
{
  int counter;
  double tmpMaxY;
  
  /* Was this node already layouted ? */
  if (IGNORE_NODE(currentNode)) {
    return;
  }
  if (VISITED_NODE(currentNode)) {
    return;
  }
  
  SET_VISITED_NODE(currentNode, 1);  /* Used to prevent infinite recursion */
  if (PARENT_NUM(currentNode) != 0) {
    FOR_ALL_PARENTS(currentNode, counter) {
      /* Are there un layouted parents ? */
      if (IGNORE_EDGE(PARENT_EDGE(currentNode, counter))) {
        continue;
      }
      if (IGNORE_NODE(PARENT_NODE(currentNode, counter))) {
        continue;
      }
      if (!VISITED_NODE(PARENT_NODE(currentNode, counter))) {
        TkCanvLayoutISISetY(canvasPtr, interp,
                            PARENT_NODE(currentNode, counter));
      }
    }
    tmpMaxY = 0;
    FOR_ALL_PARENTS(currentNode, counter) {
      if (graphOrder) {
        if (NODE_Y_POS(PARENT_NODE(currentNode, counter)) +
            NODE_HEIGHT(PARENT_NODE(currentNode, counter)) > tmpMaxY) {
          tmpMaxY = NODE_Y_POS(PARENT_NODE(currentNode, counter)) +
            NODE_HEIGHT(PARENT_NODE(currentNode, counter));
        }
      } else {
        if (NODE_Y_POS(PARENT_NODE(currentNode, counter)) +
            NODE_WIDTH(PARENT_NODE(currentNode, counter)) > tmpMaxY) {
          tmpMaxY = NODE_Y_POS(PARENT_NODE(currentNode, counter)) +
            NODE_WIDTH(PARENT_NODE(currentNode, counter));
        }
      }
    }
    DEBUG_PRINT_NODE_POS(currentNode, "SetY");
    if (graphOrder) {
      /* Place nodes top down. */
      SET_NODE_Y_POS(currentNode, tmpMaxY + edgeHeight + iconSpaceV);
    } else {
      /* Place nodes left to right. */
      SET_NODE_Y_POS(currentNode, tmpMaxY + edgeWidth + iconSpaceH);
    }
  } else {
    DEBUG_PRINT_NODE_POS(currentNode, "SetY");
    if (graphOrder) {
      /* Place nodes top down. */
      SET_NODE_Y_POS(currentNode, 0);
    } else {
      /* Place nodes left to right. */
      SET_NODE_Y_POS(currentNode, 0);
    }
  }
  SET_VISITED_NODE(currentNode, 1);
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutCreateTreeNode --
 *
 *        This procedure is invoked to create a new tree node. A pointer
 *        to this procedure is stored in createDataNode.
 *
 * Results:
 *        None
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static void
TkCanvLayoutCreateTreeNode(canvasPtr, interp, currentNode)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
     Node *currentNode;                /* This is the current node. */
{
  TreeData *tmpTreeNode;
  
  /* create layout specific data slots. */
  treeDataNum++;
  if (treeData == NULL) {
    treeData = (TreeData **) ckalloc(treeDataNum * sizeof(TreeData *));
  } else {
    treeData = (TreeData **) realloc((char *) treeData,
                                     treeDataNum * sizeof(TreeData *));
  }
  tmpTreeNode = (TreeData *) ckalloc(sizeof(TreeData));
  tmpTreeNode->tmpX = -1;
  tmpTreeNode->tmpY = -1;
  treeData[treeDataNum-1] = tmpTreeNode;
  currentNode->data = (char *) treeData[treeDataNum-1];
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutTreeSetX --
 *
 *        This procedure is invoked to calculate the x tree position.
 *        The procedure is called for all icons in the topological
 *        order. 
 *
 * Results:
 *        None
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static void
TkCanvLayoutTreeSetX(canvasPtr, interp, currentNode)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
     Node *currentNode;                /* This is the current node. */
{
  int counter;
  
  /* Was this node already layouted ? */
  if (TREE_TMP_X_POS(currentNode) != -1 || IGNORE_NODE(currentNode)) {
    return;
  }
  if (DUMMY_NODE(currentNode)) {
    return;
  }
  
  SET_VISITED_NODE(currentNode, 1);
  if (PARENT_NUM(currentNode) > 0 &&
      VISITED_NODE(PARENT_NODE(currentNode, 0))) {
    /* There are parents, and the parent was already visited. This */
    /* means that this node is the first child we layout at this */
    /* level. That means it occurs at the same level as the parent. */
    SET_VISITED_NODE(PARENT_NODE(currentNode, 0), 0);
    SET_TREE_TMP_X_POS(currentNode,
                       TREE_TMP_X_POS(PARENT_NODE(currentNode, 0)));
  } else {
    /* Append the icon to the current maximal x position. It is not */
    /* the first child of the parent. */
    SET_TREE_TMP_X_POS(currentNode, maxXPosition);
  }
  
  /* Set the x position of the current node. If the order is top down, */
  /* we use the maximum edge width. */
  if (graphOrder) {
    /* Place nodes top down. */
    SET_NODE_X_POS(currentNode, TREE_TMP_X_POS(currentNode));
    /* Do we have a new maximal x position ? */
    if (NODE_X_POS(currentNode) + iconSpaceH + edgeWidth +
        NODE_WIDTH(currentNode) > maxXPosition) {
      maxXPosition = NODE_X_POS(currentNode) + iconSpaceH +
        edgeWidth + NODE_WIDTH(currentNode);
    }
  } else {
    /* Place nodes left to right. */
    SET_NODE_X_POS(currentNode, TREE_TMP_X_POS(currentNode));
    /* Do we have a new maximal x position ? */
    if ((NODE_X_POS(currentNode) + iconSpaceV + edgeHeight +
         NODE_HEIGHT(currentNode)) > maxXPosition) {
      maxXPosition = NODE_X_POS(currentNode) + iconSpaceV +
        edgeHeight + NODE_HEIGHT(currentNode);
    }
  }
  
  /* Walk through all successors. */
  FOR_ALL_SUCCS(currentNode, counter) {
    /* Layout the children of this node. */
    if (IGNORE_NODE(SUCC_EDGE(currentNode, counter))) {
      continue;
    }
    TkCanvLayoutTreeSetX(canvasPtr, interp,
                         SUCC_NODE(currentNode, counter));
  }
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutTreeSetY --
 *
 *        This procedure is invoked to calculate the y tree position. 
 *
 * Results:
 *        None
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static void
TkCanvLayoutTreeSetY(canvasPtr, interp, currentNode)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
     Node *currentNode;                /* This is the current node. */
{
  int counter;
  double tmpMaxY;
  
  if (IGNORE_NODE(currentNode)) {
    return;
  }
  if (DUMMY_NODE(currentNode)) {
    return;
  }
  if (VISITED_NODE(currentNode)) {
    return;
  }
  
  SET_VISITED_NODE(currentNode, 1);
  tmpMaxY = 0;
  /* Walk through all parents. */
  FOR_ALL_PARENTS(currentNode, counter) {
    /* Find the parent of this node that has the greatest Y. This way */
    /* the graph is always growing to the Y direction. */
    if (IGNORE_NODE(PARENT_EDGE(currentNode, counter)) ||
        IGNORE_NODE(PARENT_NODE(currentNode, counter))) {
      continue;
    }
    if (graphOrder) {
      /* Place nodes top down. */
      if (TREE_TMP_Y_POS(PARENT_NODE(currentNode, counter)) +
          NODE_HEIGHT(PARENT_NODE(currentNode, counter)) +
          edgeHeight + iconSpaceV > tmpMaxY) {
        tmpMaxY = TREE_TMP_Y_POS(PARENT_NODE(currentNode, counter)) +
          NODE_HEIGHT(PARENT_NODE(currentNode, counter)) + edgeHeight +
            iconSpaceV;
      }
    } else {
      if (TREE_TMP_Y_POS(PARENT_NODE(currentNode, counter)) +
          NODE_WIDTH(PARENT_NODE(currentNode, counter)) +
          edgeWidth + iconSpaceH > tmpMaxY) {
        tmpMaxY = TREE_TMP_Y_POS(PARENT_NODE(currentNode, counter)) +
          NODE_WIDTH(PARENT_NODE(currentNode, counter)) + edgeWidth +
            iconSpaceH;
      }
    }
  }
  
  /* Set the y position of the current node. */
  if (graphOrder) {
    /* Place nodes top down. */
    SET_NODE_Y_POS(currentNode, tmpMaxY);
    /* Keep the maximal y position, this way we can later calculate the */
    /* correct Y position for children of this widget. */
    SET_TREE_TMP_Y_POS(currentNode, NODE_Y_POS(currentNode));
  } else {
    /* Place nodes left to right. */
    SET_NODE_Y_POS(currentNode, tmpMaxY);
    /* Keep the maximal y position, this way we can later calculate the */
    /* correct Y position for children of this widget. */
    SET_TREE_TMP_Y_POS(currentNode, NODE_Y_POS(currentNode));
  }
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutCreateNode --
 *
 *        This procedure is invoked to create a new node. Optionally the
 *        procedure can be used to create dummy nodes. In that case the
 *        fromNode and toNode parameters are specified.
 *
 * Results:
 *        None
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static void
TkCanvLayoutCreateNode(canvasPtr, interp, itemPtr, fromNode, toNode)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
     Tk_Item *itemPtr;                 /* The corresponding canvas item. */
     Node *fromNode;                   /* This is the from node. */
     Node *toNode;                     /* This is the to node. */
{
  int counter1 = 0, counter2 = 0, counter3 = 0, counter4 = 0, found = 0;
  Node *tmpNode;
  Parent *tmpParent;
  Succ *tmpSucc;
  
  nodeNum++;
  if (nodes == NULL) {
    nodes = (Node **) ckalloc(nodeNum * sizeof(Node *));
  } else {
    nodes = (Node **) realloc((char *) nodes,
                              nodeNum * sizeof(Node *));
  }
  tmpNode = (Node *) ckalloc(sizeof(Node));
  SET_NODE_ITEM(tmpNode, itemPtr);
  SET_IGNORE_NODE(tmpNode, 0);
  SET_VISITED_NODE(tmpNode, 0);
  SET_NODE_X_POS(tmpNode, 0);
  SET_NODE_Y_POS(tmpNode, 0);
  if (!DUMMY_NODE(tmpNode)) {
    SET_NODE_WIDTH(tmpNode, CANVAS_ITEM_WIDTH(itemPtr));
  } else {
    SET_NODE_WIDTH(tmpNode, 1);
  }
  if (!DUMMY_NODE(tmpNode)) {
    SET_NODE_HEIGHT(tmpNode, CANVAS_ITEM_HEIGHT(itemPtr));
  } else {
    SET_NODE_HEIGHT(tmpNode, 1);
  }
  SET_PARENT_NUM(tmpNode, 0);
  tmpNode->parent = (Parent **) NULL;
  SET_SUCC_NUM(tmpNode, 0);
  tmpNode->succ = (Succ **) NULL;
  nodes[nodeNum-1] = tmpNode;
  
  /* Create the specific data slot. */
  if (createDataNode != NULL) {
    (*createDataNode)(canvasPtr, interp, nodes[nodeNum-1]);
  }
  
  /* Insert the dummy node. */
  if (fromNode != (Node *) NULL && toNode != (Node *) NULL) {
    FOR_ALL_NODES(counter1) {
      if (nodes[counter1] == fromNode) {
        FOR_ALL_SUCCS(nodes[counter1], counter2) {
          if (SUCC_NODE(nodes[counter1], counter2) == toNode) {
            found++;
            break;
          }
        }
        break;
      }
    }
    FOR_ALL_NODES(counter3) {
      if (nodes[counter3] == toNode) {
        FOR_ALL_PARENTS(nodes[counter3], counter4) {
          if (PARENT_NODE(nodes[counter3], counter4) == fromNode) {
            found++;
            break;
          }
        }
        break;
      }
    }
    if (found == 2) {
      DEBUG_PRINT_NODE_POS(nodes[counter1], "Dummy insert from");
      DEBUG_PRINT_NODE_POS(nodes[counter3], "Dummy insert to");
      SET_PARENT_NUM(tmpNode, 1);
      SET_SUCC_NUM(tmpNode, 1);
      SET_NODE_X_POS(tmpNode, 10);
      SET_NODE_Y_POS(tmpNode, 10);
      nodes[nodeNum-1]->parent = (Parent **) 
        ckalloc(nodes[nodeNum-1]->parentNum * sizeof(Parent *));
      tmpParent = (Parent *) ckalloc(sizeof(Parent));
      SET_IGNORE_EDGE(tmpParent, 0);
      SET_VISITED_EDGE(tmpParent, 0);
      tmpParent->nodePtr = nodes[counter1];
      nodes[nodeNum-1]->parent[0] = tmpParent;
      nodes[nodeNum-1]->succ = (Succ **) 
        ckalloc(nodes[nodeNum-1]->succNum * sizeof(Succ *));
      tmpSucc = (Succ *) ckalloc(sizeof(Succ));
      SET_IGNORE_EDGE(tmpSucc, 0);
      SET_VISITED_EDGE(tmpSucc, 0);
      tmpSucc->nodePtr = nodes[counter3];
      nodes[nodeNum-1]->succ[0] = tmpSucc;
      nodes[counter3]->parent[counter4]->nodePtr = tmpNode;
      nodes[counter1]->succ[counter2]->nodePtr = tmpNode;
    }
  }
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutBuildGraph --
 *
 *        This procedure is invoked to create the internal
 *        graph structure used for layouting.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static int
TkCanvLayoutBuildGraph(canvasPtr, interp)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
{
  register Tk_Item *itemPtr;
  int argc2 = 0, argc3 = 0, argc4 = 0, argc5 = 0,
      isGraphMember, counter, counter1, counter2;
  char **argv2, **argv3, **argv4, **argv5;
  Parent *tmpParent;
  Succ *tmpSucc;
  
  /* Walk through all canvas items to create node slots. */
  FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
    /* Create node slot for icon. */
    if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "icon") == 0) {
      TkCanvLayoutCreateNode(canvasPtr, interp, itemPtr, NULL, NULL);
    }
  }
  
  /* Walk through all nodes to compute the parents/successors */
  FOR_ALL_NODES(counter) {
    /* Find the greatest icon dimensions. */
    if (NODE_WIDTH(nodes[counter]) > iconWidth) {
      iconWidth = NODE_WIDTH(nodes[counter]);
    }
    if (NODE_HEIGHT(nodes[counter]) > iconHeight) {
      iconHeight = NODE_HEIGHT(nodes[counter]);
    }
    
    /* Ignore dummy nodes. */
    if (DUMMY_NODE(nodes[counter])) {
      continue;
    }
    /* Read the edge list of this node. */
    Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
                     NODE_ITEM(nodes[counter])->typePtr->configSpecs,
                     (char *) NODE_ITEM(nodes[counter]), "-edges", 0);
    if (Tcl_SplitList(canvasPtr->interp, canvasPtr->interp->result,
                      &argc2, &argv2) != TCL_OK) {
      Tcl_AppendResult(interp, "error during layouting: ",
                       "reading parameters", (char *) NULL);
      return TCL_ERROR;
    }
    /* Get the actual value of -edges, and split it up. */
    if (Tcl_SplitList(canvasPtr->interp, argv2[4],
                      &argc3, &argv3) != TCL_OK) {
      Tcl_AppendResult(interp, "error during layouting: ",
                       "reading parameters", (char *) NULL);
      ckfree((char *) argv2);
      return TCL_ERROR;
    }
    
    if (graphName != NULL) {
      isGraphMember = 0;
    } else {
      isGraphMember = 1;
    }
    /* Walk through all edges in the -edges list. */
    for (counter1 = 0; counter1 < argc3; counter1++) {
      /* Get the icon and edge ids for this edge.*/
      if (Tcl_SplitList(canvasPtr->interp, argv3[counter1],
                        &argc4, &argv4) != TCL_OK) {
        Tcl_AppendResult(interp, "error during layouting: ",
                         "reading parameters", (char *) NULL);
        ckfree((char *) argv3);
        ckfree((char *) argv2);
        return TCL_ERROR;
      }
      
      if (graphName != NULL) {
        /* Read the edge graphname of this edge node. */
        FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
          /* Create node slot for icon. */
          if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "edge") == 0) {
            if (CANVAS_ITEM_ID(itemPtr) == atol(argv4[2])) {
              Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
                               itemPtr->typePtr->configSpecs,
                               (char *) itemPtr, "-graphname", 0);
              if (Tcl_SplitList(canvasPtr->interp,
                                canvasPtr->interp->result,
                                &argc5, &argv5) != TCL_OK) {
                Tcl_AppendResult(interp, "error during layouting: ",
                                 "reading parameters", (char *) NULL);
                ckfree((char *) argv4);
                ckfree((char *) argv3);
                ckfree((char *) argv2);
                return TCL_ERROR;
              }
              if (strcmp(argv5[4], graphName) == 0) {
                isGraphMember = 1;
                ckfree((char *) argv5);
                break;
              }
              ckfree((char *) argv5);
            }
          }
        }
      }
      
      /* Is the current node a child ? */
      if (CANVAS_ITEM_ID(NODE_ITEM(nodes[counter])) == atol(argv4[1])) {
        /* Walk through all nodes. */
        FOR_ALL_NODES(counter2) {
          /* Ignore dummy nodes. */
          if (DUMMY_NODE(nodes[counter2])) {
            continue;
          }
          if (CANVAS_ITEM_ID(NODE_ITEM(nodes[counter2])) == atol(argv4[0])) {
            nodes[counter]->parentNum++;
            if (nodes[counter]->parent == NULL) {
              nodes[counter]->parent = (Parent **) 
                ckalloc(nodes[counter]->parentNum * sizeof(Parent *));
            } else {
              nodes[counter]->parent = (Parent **) 
                realloc((char *) nodes[counter]->parent,
                        nodes[counter]->parentNum * sizeof(Parent *));
            }
            tmpParent = (Parent *) ckalloc(sizeof(Parent));
            SET_IGNORE_EDGE(tmpParent, 0);
            SET_VISITED_EDGE(tmpParent, 0);
            tmpParent->nodePtr = nodes[counter2];
            nodes[counter]->parent[nodes[counter]->parentNum-1] =
              tmpParent;
          }
        }
      }
      /* Is the current node a parent ? */
      if (CANVAS_ITEM_ID(NODE_ITEM(nodes[counter])) == atol(argv4[0])) {
        /* Walk through all nodes. */
        FOR_ALL_NODES(counter2) {
          /* Ignore dummy nodes. */
          if (DUMMY_NODE(nodes[counter2])) {
            continue;
          }
          if (CANVAS_ITEM_ID(NODE_ITEM(nodes[counter2])) == atol(argv4[1])) {
            nodes[counter]->succNum++;
            if (nodes[counter]->succ == NULL) {
              nodes[counter]->succ = (Succ **) 
                ckalloc(nodes[counter]->succNum * sizeof(Succ *));
            } else {
              nodes[counter]->succ = (Succ **) 
                realloc((char *) nodes[counter]->succ,
                        nodes[counter]->succNum * sizeof(Succ *));
            }
            tmpSucc = (Succ *) ckalloc(sizeof(Succ));
            SET_IGNORE_EDGE(tmpSucc, 0);
            SET_VISITED_EDGE(tmpSucc, 0);
            tmpSucc->nodePtr = nodes[counter2];
            nodes[counter]->succ[nodes[counter]->succNum-1] = tmpSucc;
          }
        }
      }
      ckfree((char *) argv4);
    }
    if (!isGraphMember) {
      SET_IGNORE_NODE(nodes[counter], 1);
    }
    ckfree((char *) argv3);
    ckfree((char *) argv2);
  }
  return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutFreeGraph --
 *
 *        This procedure is invoked to free the graph structures
 *        used for layouting.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static void
TkCanvLayoutFreeGraph()
{
  int counter, counter2;

  /* Free allocated memory. */
  FOR_ALL_NODES(counter) {
    FOR_ALL_PARENTS(nodes[counter], counter2) {
      ckfree((char *) PARENT_EDGE(nodes[counter], counter2));
    }
    ckfree((char *) nodes[counter]->parent);
    FOR_ALL_SUCCS(nodes[counter], counter2) {
      ckfree((char *) SUCC_EDGE(nodes[counter], counter2));
    }
    ckfree((char *) nodes[counter]->succ);
    ckfree((char *) nodes[counter]);
  }
  ckfree((char *) nodes);
  FOR_ALL_TOP_NODES(counter) {
    ckfree((char *) topList[counter]);
  }
  ckfree((char *) topList);
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutGraphRoot --
 *
 *        This procedure is invoked to find the root of a graph.
 *
 * Results:
 *        The root node.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static Node *
TkCanvLayoutGraphRoot(canvasPtr)
     TkCanvas *canvasPtr;               /* Information about canvas widget. */
{
  int optimalRootNum = -10000, minParentNum = -1, maxSuccNum = -1,
      counter, counter1, counter2, counter3;
  Node *tmpRootNode = (Node *) NULL;

  /* Find a root node. This node has no parents. In case we do not */
  /* have such a node... find the node with the smallest number of */
  /* parents. */
  if (rootId != -1) {
    FOR_ALL_NODES(counter) {
      /* Ignore dummy nodes. */
      if (DUMMY_NODE(nodes[counter])) {
        continue;
      }
      /* Ignore ignored nodes. */
      if (IGNORE_NODE(nodes[counter])) {
        continue;
      }
      if (rootId == CANVAS_ITEM_ID(NODE_ITEM(nodes[counter]))) {
        tmpRootNode = nodes[counter];
      }
    }
  } else {
    /* We try to find the node with the most children and no parent. */
    /* This node will become root. */
    FOR_ALL_NODES(counter) {
      if (IGNORE_NODE(nodes[counter])) {
        continue;
      }
      if (SUCC_NUM(nodes[counter]) > 0 &&
          PARENT_NUM(nodes[counter]) == 0 && 
          SUCC_NUM(nodes[counter]) > maxSuccNum) {
        tmpRootNode = nodes[counter];
        maxSuccNum = SUCC_NUM(nodes[counter]);
      }
    }
    if (tmpRootNode == (Node *) NULL) {
      /* We try to find the node with the most children and the least */
      /* parents. This node will become root. */
      FOR_ALL_NODES(counter) {
        if (IGNORE_NODE(nodes[counter])) {
          continue;
        }
        if ((SUCC_NUM(nodes[counter]) > 0 &&
             optimalRootNum <= (SUCC_NUM(nodes[counter]) -
                                PARENT_NUM(nodes[counter])) &&
             (minParentNum > PARENT_NUM(nodes[counter]) ||
              (minParentNum == PARENT_NUM(nodes[counter]) &&
               maxSuccNum < SUCC_NUM(nodes[counter])))) ||
            optimalRootNum == -10000) {
          tmpRootNode = nodes[counter];
          minParentNum = PARENT_NUM(nodes[counter]);
          maxSuccNum = SUCC_NUM(nodes[counter]);
          if (SUCC_NUM(nodes[counter]) > 0) {
            optimalRootNum =
              (SUCC_NUM(nodes[counter]) - PARENT_NUM(nodes[counter]));
          }
        }
      }
    }
  }
  
  /* No nodes... so abort the search. */
  if (tmpRootNode == NULL) {
    return NULL;
  }
  
  /* There is no node with no parents. So use the node with the */
  /* smallest number of parents, and ignore the edges leading to this */
  /* node. */
  if (PARENT_NUM(tmpRootNode) != 0) {
    for (counter1 = 0; counter1 < PARENT_NUM(tmpRootNode); counter1++) {
      SET_IGNORE_NODE(PARENT_EDGE(tmpRootNode, counter1), 1);
      FOR_ALL_NODES(counter2) {
        /* Ignore dummy nodes. */
        if (DUMMY_NODE(nodes[counter2])) {
          continue;
        }
        if (NODE_ITEM(nodes[counter2]) ==
            NODE_ITEM(PARENT_NODE(tmpRootNode, counter1))) {
          FOR_ALL_SUCCS(nodes[counter2], counter3) {
            if (NODE_ITEM(SUCC_NODE(nodes[counter2], counter3)) ==
                NODE_ITEM(tmpRootNode)) {
              SET_IGNORE_NODE(SUCC_EDGE(nodes[counter2], counter3), 1);
            }
          }
        }
      }
    }
  }
  return tmpRootNode;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutGraphSortTopological --
 *
 *        This procedure is invoked to sort a graph topological. 
 *
 * Results:
 *        None
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static void
TkCanvLayoutGraphSortTopological(canvasPtr, interp, currentNode)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
     Node *currentNode;                /* This is the current node. */
{
  int counter;
  Topology *tmpTopology;
  
  if (VISITED_NODE(currentNode) || IGNORE_NODE(currentNode)) {
    return;
  }
  
  /* Append the current node to the list of topologically sorted */
  /* nodes. */
  topListNum++;
  if (topList == NULL) {
    topList = (Topology **) ckalloc(topListNum * sizeof(Topology *));
  } else {
    topList = (Topology **) realloc((char *) topList,
                                    topListNum * sizeof(Topology *));
  }
  tmpTopology = (Topology *) ckalloc(sizeof(Topology));
  tmpTopology->nodePtr = currentNode;
  topList[topListNum-1] = tmpTopology;
  
  SET_VISITED_NODE(currentNode, 1);
  /* Walk through all successors. */
  FOR_ALL_SUCCS(currentNode, counter) {
    if (IGNORE_EDGE(SUCC_EDGE(currentNode, counter))) {
      continue;
    }
    if (IGNORE_NODE(SUCC_NODE(currentNode, counter))) {
      continue;
    }
    if (VISITED_NODE(SUCC_NODE(currentNode, counter))) {
      SET_IGNORE_EDGE(SUCC_EDGE(currentNode, counter), 1);
      continue;
    }
    TkCanvLayoutGraphSortTopological(canvasPtr, interp,
                                     SUCC_NODE(currentNode, counter));
  }
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutGraphPlaceNodes --
 *
 *        This procedure is invoked to actually place the graph nodes.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static int
TkCanvLayoutGraphPlaceNodes(canvasPtr, interp)
     TkCanvas *canvasPtr;              /* Information about canvas
                                        * widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
{
  int argc2 = 0, counter;
  double tmpX, tmpY;
  char **argv2;
  
  SRANDOM(getpid() + time((time_t *) NULL));
  FOR_ALL_NODES(counter) {
    if (IGNORE_NODE(nodes[counter])) {
      continue;
    }
    if (NODE_X_POS(nodes[counter]) != -1 &&
        NODE_Y_POS(nodes[counter]) != -1) {
      if (graphOrder) {
        /* Place nodes top down. */
        tmpX = NODE_X_POS(nodes[counter]);
        tmpY = NODE_Y_POS(nodes[counter]);
      } else {
        /* Place nodes left to right. */
        tmpX = NODE_Y_POS(nodes[counter]);
        tmpY = NODE_X_POS(nodes[counter]);
      }
    } else {
      /* are we allowed to place the icon ? */
      if (keepRandomPositions &&
          NODE_X_POS(nodes[counter]) > 0 &&
          NODE_Y_POS(nodes[counter]) > 0) {
        continue;
      }
      tmpX = (long) (RANDOM() % maxX) - NODE_WIDTH(nodes[counter]);
      tmpY = (long) (RANDOM() % maxY) - NODE_HEIGHT(nodes[counter]);
    }
    if (tmpX < 0) {
      tmpX = 0;
    }
    if (tmpY < 0) {
      tmpY = 0;
    }
    if (!DUMMY_NODE(nodes[counter])) {
      sprintf(convertBuffer, "%g %g ", tmpX + xOffset,
              tmpY + yOffset);
      if (Tcl_SplitList(canvasPtr->interp, convertBuffer,
                        &argc2, &argv2) != TCL_OK) {
        Tcl_AppendResult(interp, "error during layouting: ",
                         "splitting geometry", (char *) NULL);
        return TCL_ERROR;
      }
      (*NODE_ITEM(nodes[counter])->typePtr->coordProc)
        (interp, (Tk_Canvas) canvasPtr,
         NODE_ITEM(nodes[counter]), argc2, argv2);
      ckfree((char *) argv2);
    }
  }
  return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutGraphPlaceEdges --
 *
 *        This procedure is invoked to relayout all edges.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static int
TkCanvLayoutGraphPlaceEdges(canvasPtr, interp)
     TkCanvas *canvasPtr;              /* Information about canvas
                                        * widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
{
  register Tk_Item *itemPtr;
  
  /* scan through all items. */
  FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
    /* layout edges. */
    if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "edge") == 0) {
      TkCanvLayoutEdge(canvasPtr, interp, itemPtr, NULL, NULL);
    }
  }
  return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutBuildGraphToString --
 *
 *      This procedure is invoked to create a string representing 
 *      the current graph in the canvas. The string follows this
 *      format:
 *      {{NodeList} {EdgeList}}
 *        NodeList = {Node Node Node ...}
 *          Node     = {NodeCanvasId x y width height {ConnectionList}}
 *            ConnectionList = {Connection Connection ...}
 *              Connection      = {FromCanvasId ToCanvasId EdgeCanvasId}
 *        EdgeList = {Edge Edge Edge ...}
 *          Edge     = {EdgeCanvasId FromCanvasId ToCanvasId}
 *
 * Results:
 *        The created string.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
char *
TkCanvLayoutBuildGraphToString(canvasPtr, interp)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
{
  register Tk_Item *itemPtr;
  int argc2 = 0;
  char **argv2, *result;
  Tcl_DString tmpString, iconString, edgeString, resultString;
  
  Tcl_DStringInit(&resultString);
  Tcl_DStringInit(&iconString);
  /* Walk through all canvas items to create icon string. */
  FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
    /* Create node string for icon. */
    Tcl_DStringInit(&tmpString);
    if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "icon") == 0) {
      sprintf(convertBuffer, "%ld %d %d %d %d",
              (long) CANVAS_ITEM_ID(itemPtr),
              CANVAS_ITEM_X_POS(itemPtr), CANVAS_ITEM_Y_POS(itemPtr),
              CANVAS_ITEM_WIDTH(itemPtr), CANVAS_ITEM_HEIGHT(itemPtr));
      Tcl_DStringAppend(&tmpString, convertBuffer, -1);
      /* Read the edge list of this node. */
      Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
                       itemPtr->typePtr->configSpecs,
                       (char *) itemPtr, "-edges", 0);
      if (Tcl_SplitList(canvasPtr->interp, canvasPtr->interp->result,
                        &argc2, &argv2) != TCL_OK) {
        Tcl_AppendResult(interp, "error during layouting: ",
                         "reading parameters", (char *) NULL);
        return (char *) NULL;
      }
      Tcl_DStringAppendElement(&tmpString, argv2[4]);
      Tcl_DStringAppendElement(&iconString, tmpString.string);
      ckfree((char *) argv2);
    }
    Tcl_DStringFree(&tmpString);
  }
  
  Tcl_DStringInit(&edgeString);
  /* Walk through all canvas items to create edge string. */
  FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
    /* Create node string for edge. */
    Tcl_DStringInit(&tmpString);
    if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "edge") == 0) {
      sprintf(convertBuffer, "%ld ", (long) CANVAS_ITEM_ID(itemPtr));
      Tcl_DStringAppend(&tmpString, convertBuffer, -1);
      Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
                       itemPtr->typePtr->configSpecs,
                       (char *) itemPtr, "-from", 0);
      if (Tcl_SplitList(canvasPtr->interp,
                        canvasPtr->interp->result,
                        &argc2, &argv2) != TCL_OK) {
        return (char *) NULL;
      }
      Tcl_DStringAppend(&tmpString, argv2[4], -1);
      ckfree((char *) argv2);
      Tcl_DStringAppend(&tmpString, " ", -1);
      Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
                       itemPtr->typePtr->configSpecs,
                       (char *) itemPtr, "-to", 0);
      if (Tcl_SplitList(canvasPtr->interp,
                        canvasPtr->interp->result,
                        &argc2, &argv2) != TCL_OK) {
        return (char *) NULL;
      }
      Tcl_DStringAppend(&tmpString, argv2[4], -1);
      ckfree((char *) argv2);
      Tcl_DStringAppendElement(&edgeString, tmpString.string);
    }
    Tcl_DStringFree(&tmpString);
  }
  
  Tcl_DStringAppendElement(&resultString, iconString.string);
  Tcl_DStringAppendElement(&resultString, edgeString.string);
  result = ckalloc((size_t) Tcl_DStringLength(&resultString) + 2);
  strcpy(result, resultString.string);
  Tcl_DStringFree(&resultString);
  Tcl_DStringFree(&iconString);
  Tcl_DStringFree(&edgeString);
  return result;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutGraphPlaceNodesFromString --
 *
 *      This procedure is invoked to actually place the graph nodes
 *      and the edges. The nodes and edges are specified by a string
 *      following this format:
 *      {{NodeList} {EdgeList}}
 *        NodeList = {Node Node Node ...}
 *          Node   ={NodeCanvasId x y width height}
 *        EdgeList = {Edge Edge Edge ...}
 *          Edge   ={EdgeCanvasId x1 y1 x2 y2 ....}
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
int
TkCanvLayoutGraphPlaceNodesFromString(canvasPtr, interp, nodeString)
     TkCanvas *canvasPtr;              /* Information about canvas
                                        * widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
     char *nodeString;
{
  register Tk_Item *itemPtr;
  int argc2 = 0, argc3 = 0, argc4 = 0, elementsCounter = 0, counter;
  long elementId;
  char **argv2, **lists, **elements, **element;
  
  if (Tcl_SplitList(canvasPtr->interp, nodeString,
                    &argc3, &lists) != TCL_OK) {
    Tcl_AppendResult(interp, "error during layouting: ",
                     "splitting place string", (char *) NULL);
    return TCL_ERROR;
  }
  
  /* Place the nodes */
  if (Tcl_SplitList(canvasPtr->interp, lists[0],
                    &elementsCounter, &elements) != TCL_OK) {
    Tcl_AppendResult(interp, "error during layouting: ",
                     "splitting place string", (char *) NULL);
    ckfree((char *) lists);
    return TCL_ERROR;
  }
  
  for (counter = 0; counter < elementsCounter; counter++) {
    if (Tcl_SplitList(canvasPtr->interp, elements[counter],
                      &argc4, &element) != TCL_OK) {
      Tcl_AppendResult(interp, "error during layouting: ",
                       "splitting place string", (char *) NULL);
      ckfree((char *) elements);
      ckfree((char *) lists);
      return TCL_ERROR;
    }
    /* Here we perform the actual placing. */
    sprintf(convertBuffer, "%ld %ld ", atol(element[1]), atol(element[2]));
    if (Tcl_SplitList(canvasPtr->interp, convertBuffer,
                      &argc2, &argv2) != TCL_OK) {
      Tcl_AppendResult(interp, "error during layouting: ",
                       "splitting geometry", (char *) NULL);
      ckfree((char *) element);
      ckfree((char *) elements);
      ckfree((char *) lists);
      return TCL_ERROR;
    }
    elementId = atol(element[0]);
    /* Walk through all canvas items. */
    FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
      /* Is the canvas item a icon, and does it have the correct id? */
      if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "icon") == 0 &&
          CANVAS_ITEM_ID(itemPtr) == elementId) {
        (*itemPtr->typePtr->coordProc)
          (interp, (Tk_Canvas) canvasPtr, itemPtr,
           argc2, argv2);
        break;
      }
    }
    ckfree((char *) argv2);
    ckfree((char *) element);
  }
  ckfree((char *) elements);
  
  /* Place the edges */
  if (Tcl_SplitList(canvasPtr->interp, lists[1],
                    &elementsCounter, &elements) != TCL_OK) {
    Tcl_AppendResult(interp, "error during layouting: ",
                     "splitting place string", (char *) NULL);
    ckfree((char *) lists);
    return TCL_ERROR;
  }
  
  for (counter = 0; counter < elementsCounter; counter++) {
    if (Tcl_SplitList(canvasPtr->interp, elements[counter],
                      &argc4, &element) != TCL_OK) {
      Tcl_AppendResult(interp, "error during layouting: ",
                       "splitting place string", (char *) NULL);
      ckfree((char *) elements);
      ckfree((char *) lists);
      return TCL_ERROR;
    }
    /* Here we perform the actual placing. */
    elementId = atol(element[0]);
    /* Walk through all canvas items. */
    FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
      /* Is the canvas item a icon, and does it have the correct id? */
      if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "edge") == 0 &&
          CANVAS_ITEM_ID(itemPtr) == elementId) {
        (*itemPtr->typePtr->coordProc)
          (interp, (Tk_Canvas) canvasPtr, itemPtr,
           argc2 - 1, argv2 + 1);
        break;
      }
    }
    ckfree((char *) element);
  }
  ckfree((char *) elements);
  
  ckfree((char *) lists);
  return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutEdgeWidth --
 *
 *        This procedure is invoked to find the widest edge. Widest
 *        means the edge with the maximal x expansion.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static int
TkCanvLayoutEdgeWidth(canvasPtr, interp)
     TkCanvas *canvasPtr;              /* Information about canvas widget. */
     Tcl_Interp *interp;               /* Current interpreter. */
{
  register Tk_Item *itemPtr;
  int argc2; 
  char **argv2;
  
  if (edgeWidth == 0) {
    /* Walk through all items. */
    FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
      if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "edge") == 0) {
        /* Read the text height of this node. */
        Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
                         itemPtr->typePtr->configSpecs,
                         (char *) itemPtr, "-textheight", 0);
        if (Tcl_SplitList(canvasPtr->interp, canvasPtr->interp->result,
                          &argc2, &argv2) != TCL_OK) {
          Tcl_AppendResult(interp, "error during layouting: ",
                           "reading parameters", (char *) NULL);
          return TCL_ERROR;
        }
        if (atol(argv2[4]) + LINE_INCREMENT > edgeHeight) {
          edgeHeight = atol(argv2[4]) + LINE_INCREMENT;
        }
        ckfree((char *) argv2);
        /* Read the text width of this node. */
        Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
                         itemPtr->typePtr->configSpecs,
                         (char *) itemPtr, "-textwidth", 0);
        if (Tcl_SplitList(canvasPtr->interp, canvasPtr->interp->result,
                          &argc2, &argv2) != TCL_OK) {
          Tcl_AppendResult(interp, "error during layouting: ",
                           "reading parameters", (char *) NULL);
          return TCL_ERROR;
        }
        if (atol(argv2[4]) + LINE_INCREMENT > edgeWidth) {
          edgeWidth = atol(argv2[4]) + LINE_INCREMENT;
        }
        ckfree((char *) argv2);
      }
    }
  }
  return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutEdge --
 *
 *        This procedure is invoked to adjust the edge to the new
 *        locations of the connected nodes. This algorithm only works
 *        for simple edges.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static int
TkCanvLayoutEdge(canvasPtr, interp, curPtr, fromNode, toNode)
     TkCanvas *canvasPtr;                /* Information about canvas widget. */
     Tcl_Interp *interp;                 /* Current interpreter. */
     Tk_Item *curPtr;                    /* Current edge. */
     Node *fromNode;                     /* Source node. */
     Node *toNode;                       /* Target node. */
{
  register Tk_Item *itemPtr = NULL, *fromPtr = NULL, *toPtr = NULL;
  int argc2 = 0, result = TCL_OK, fromId, toId, counter = 0;
  char **argv2;
  Tcl_DString command, tmpPoints;
  
  if (fromNode == (Node *) NULL && toNode == (Node *) NULL) {
    /* Get "from" id of the edge */
    Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
                     curPtr->typePtr->configSpecs,
                     (char *) curPtr, "-from", 0);
    if (Tcl_SplitList(canvasPtr->interp,
                      canvasPtr->interp->result,
                      &argc2, &argv2) != TCL_OK) {
      return TCL_ERROR;
    }
    fromId = atol(argv2[4]);
    ckfree((char *) argv2);
    
    /* Get "to" id of the edge */
    Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
                     curPtr->typePtr->configSpecs,
                     (char *) curPtr, "-to", 0);
    if (Tcl_SplitList(canvasPtr->interp,
                      canvasPtr->interp->result,
                      &argc2, &argv2) != TCL_OK) {
      return TCL_ERROR;
    }
    toId = atol(argv2[4]);
    ckfree((char *) argv2);
    
    /* Match the numeric id value to a concrete item pointer. */
    FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
      if (fromId == CANVAS_ITEM_ID(itemPtr)) {
        fromPtr = itemPtr;
      }
      if (toId == CANVAS_ITEM_ID(itemPtr)) {
        toPtr = itemPtr;
      }
    }
  }
  
  /* Is this a regular edge ? */
  if (fromPtr != NULL && toPtr != NULL) {
    Tcl_DStringInit(&tmpPoints);
    Tcl_DStringInit(&command);
    Tcl_DStringAppend(&command, Tk_PathName(canvasPtr->tkwin), -1);
    Tcl_DStringAppend(&command, " coords ", -1);
    sprintf(convertBuffer, "%ld", (long) CANVAS_ITEM_ID(curPtr));
    Tcl_DStringAppend(&command, convertBuffer, -1);
    Tcl_Eval(interp, command.string);
    if (Tcl_SplitList(canvasPtr->interp, interp->result,
                      &argc2, &argv2) != TCL_OK) {
      return TCL_ERROR;
    }
    Tcl_DStringFree(&command);
    
    /* find the points between the first and the last point */
    if (argc2 >= 4) {
      for (counter = 2; counter < argc2 - 2; counter++) {
        sprintf(convertBuffer, "%ld %ld ", atol(argv2[counter++]),
                atol(argv2[counter]));
        Tcl_DStringAppend(&tmpPoints, convertBuffer, -1);
      }
    }
    ckfree((char *) argv2);
    
    /* calculate the various node anchors. */
    if (fromPtr->x1 > fromPtr->x2) {
      posX1 = fromPtr->x1 + ((fromPtr->x2 - fromPtr->x1) / 2);
    } else {
      posX1 = fromPtr->x2 + ((fromPtr->x1 - fromPtr->x2) / 2);
    }
    if (fromPtr->y1 > fromPtr->y2) {
      posY1 = fromPtr->y1 + ((fromPtr->y2 - fromPtr->y1) / 2);
    } else {
      posY1 = fromPtr->y2 + ((fromPtr->y1 - fromPtr->y2) / 2);
    }
    if (toPtr->x1 > toPtr->x2) {
      posX2 = toPtr->x1 + ((toPtr->x2 - toPtr->x1) / 2);
    } else {
      posX2 = toPtr->x2 + ((toPtr->x1 - toPtr->x2) / 2);
    }
    if (toPtr->y1 > toPtr->y2) {
      posY2 = toPtr->y1 + ((toPtr->y2 - toPtr->y1) / 2);
    } else {
      posY2 = toPtr->y2 + ((toPtr->y1 - toPtr->y2) / 2);
    }
    
    if (edgeOrder) {
      /* Place the edges according to the graph order... only along
       * the generale layout direction. */
      if (graphOrder) {
        /* Place nodes top down. */
        if (fromPtr->y2 <= toPtr->y1) {
          sprintf(convertBuffer, "%d %d %s %d %d ",
                  posX1, fromPtr->y2, tmpPoints.string,
                  posX2, toPtr->y1);
        } else {
          sprintf(convertBuffer, "%d %d %s %d %d ",
                  posX1, fromPtr->y1, tmpPoints.string,
                  posX2, toPtr->y2);
        }
      } else {
        /* Place nodes left to right. */
        if (fromPtr->x2 < toPtr->x1) {
          sprintf(convertBuffer, "%d %d %s %d %d ",
                  fromPtr->x2, posY1, tmpPoints.string,
                  toPtr->x1, posY2);
        } else {
          sprintf(convertBuffer, "%d %d %s %d %d ",
                  fromPtr->x1, posY1, tmpPoints.string,
                  toPtr->x2, posY2);
        }
      }
    } else {
      /* Place the edges so that they use the shorts distance. */
      if (fromPtr->y2 <= toPtr->y1) {
        /* from is above to */
        if (fromPtr->x2 <= toPtr->x1) {
          /* from is left from to */
          if (graphOrder) {
            /* Place nodes top down. */
            sprintf(convertBuffer, "%d %d %s %d %d ",
                    posX1, fromPtr->y2, tmpPoints.string,
                    posX2, toPtr->y1);
          } else {
            sprintf(convertBuffer, "%d %d %s %d %d ",
                    fromPtr->x2, posY1, tmpPoints.string,
                    toPtr->x1, posY2);
          }
        } else {
          if (fromPtr->x1 >= toPtr->x2) {
            /* from is right from to */
            if (graphOrder) {
              /* Place nodes top down. */
              sprintf(convertBuffer, "%d %d %s %d %d ",
                      posX1, fromPtr->y2, tmpPoints.string,
                      posX2, toPtr->y1);
            } else {
              sprintf(convertBuffer, "%d %d %s %d %d ",
                      fromPtr->x1, posY1, tmpPoints.string,
                      toPtr->x2, posY2);
            }
          } else {
            /* from is at same level as to */
            sprintf(convertBuffer, "%d %d %s %d %d ",
                    posX1, fromPtr->y2, tmpPoints.string,
                    posX2, toPtr->y1);
          }
        }
      } else {
        if (fromPtr->y1 >= toPtr->y2) {
          /* from is below to */
          if (fromPtr->x2 <= toPtr->x1) {
            /* from is left from to */
            if (graphOrder) {
              /* Place nodes top down. */
              sprintf(convertBuffer, "%d %d %s %d %d ",
                      posX1, fromPtr->y1, tmpPoints.string,
                      posX2, toPtr->y2);
            } else {
              sprintf(convertBuffer, "%d %d %s %d %d ",
                      fromPtr->x2, posY1, tmpPoints.string,
                      toPtr->x1, posY2);
            }
          } else {
            if (fromPtr->x1 >= toPtr->x2) {
              /* from is right from to */
              if (graphOrder) {
                /* Place nodes top down. */
                sprintf(convertBuffer, "%d %d %s %d %d ",
                        posX1, fromPtr->y1, tmpPoints.string,
                        posX2, toPtr->y2);
              } else {
                sprintf(convertBuffer, "%d %d %s %d %d ",
                        fromPtr->x1, posY1, tmpPoints.string, 
                        toPtr->x2, posY2);
              }
            } else {
              /* from is at same level as to */
              sprintf(convertBuffer, "%d %d %s %d %d ",
                      posX1, fromPtr->y1, tmpPoints.string,
                      posX2, toPtr->y2);
            }
          }
        } else {
          /* from is at same level as to */
          if (fromPtr->x2 <= toPtr->x1) {
            /* from is left from to */
            sprintf(convertBuffer, "%d %d %s %d %d ",
                    fromPtr->x2, posY1, tmpPoints.string,
                    toPtr->x1, posY2);
          } else {
            if (fromPtr->x1 > toPtr->x2) {
              /* from is right from to */
              sprintf(convertBuffer, "%d %d %s %d %d ",
                      fromPtr->x1, posY1, tmpPoints.string,
                      toPtr->x2, posY2);
            } else {
              if (fromPtr->x1 <= toPtr->x1) {
                /* from is partially left from to */
                sprintf(convertBuffer, "%d %d %s %d %d ",
                        fromPtr->x2, posY1, tmpPoints.string,
                        toPtr->x1, posY2);
              } else {
                /* from is partially right from to */
                sprintf(convertBuffer, "%d %d %s %d %d ",
                        fromPtr->x1, posY1, tmpPoints.string,
                        toPtr->x2, posY2);
              }
            }
          }
        }
      }
    }
    Tcl_DStringFree(&tmpPoints);
    
    /* Set new coordinates */
    if (Tcl_SplitList(canvasPtr->interp, convertBuffer,
                      &argc2, &argv2) != TCL_OK) {
      return TCL_ERROR;
    }
    Tcl_ResetResult(canvasPtr->interp);
    result = (*curPtr->typePtr->coordProc)
      (interp, (Tk_Canvas) canvasPtr, curPtr, argc2, argv2);
    ckfree((char *) argv2);
  }
  return result;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutISI --
 *
 *        This procedure is invoked to place icons with ISI.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static int
TkCanvLayoutISI(canvasPtr, interp)
     TkCanvas *canvasPtr;                /* Information about canvas widget. */
     Tcl_Interp *interp;                 /* Current interpreter. */
{
  int counter, result = TCL_OK;
  
  maxXPosition = 0;
  maxYPosition = 0;
  nodeNum = 0;
  nodes = (Node **) NULL;
  topListNum = 0;
  topList = (Topology **) NULL;
  rootNode = (Node *) NULL;
  createDataNode = NULL;
  
  /* Find the widest/highest Edge. */
  TkCanvLayoutEdgeWidth(canvasPtr, interp);
  
  /* Build the internal graph structure. */
  if (TkCanvLayoutBuildGraph(canvasPtr, interp) != TCL_OK) {
    return TCL_ERROR;
  }
  
  /* Find root of the graph. */
  if ((rootNode = TkCanvLayoutGraphRoot(canvasPtr)) == NULL) {
    Tcl_AppendResult(interp, "error during layouting: ",
                     "no root node", (char *) NULL);
    return TCL_ERROR;
  }
  
  DEBUG_PRINT_NODE_POS(rootNode, "RootNode");
  /* Sort the graph topological. */
  TkCanvLayoutGraphSortTopological(canvasPtr, interp, rootNode);
  FOR_ALL_NODES(counter) {
    if (PARENT_NUM(nodes[counter]) == 0) {
      TkCanvLayoutGraphSortTopological(canvasPtr, interp, nodes[counter]);
    }
  }
  
  /* Calculate the position values. */
  RESET_VISITED_NODE(counter);
  FOR_ALL_NODES(counter) {
    if (PARENT_NUM(nodes[counter]) == 0) {
      TkCanvLayoutISISetX(canvasPtr, interp, nodes[counter]);
    }
  }
  RESET_VISITED_NODE(counter);
  FOR_ALL_NODES(counter) {
    if (SUCC_NUM(nodes[counter]) == 0) {
      TkCanvLayoutISISetY(canvasPtr, interp, nodes[counter]);
    }
  }
  
  /* Place the graph items. */
  if (TkCanvLayoutGraphPlaceNodes(canvasPtr, interp) != TCL_OK) {
    TkCanvLayoutFreeGraph();
    return TCL_ERROR;
  }
  
  /* Free the internal graph structure. */
  TkCanvLayoutFreeGraph();
  
  return result;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutMatrix --
 *
 *        This procedure is invoked to place icons as matrix.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static int
TkCanvLayoutMatrix(canvasPtr, interp)
     TkCanvas *canvasPtr;                /* Information about canvas widget. */
     Tcl_Interp *interp;                 /* Current interpreter. */
{
  register Tk_Item *itemPtr = NULL, *curPtr = NULL;
  int argc2 = 0, argc3 = 0, result = TCL_OK, greatestX = 10,
    greatestY = 10, greatestHeight = 0, columnCounter = 0,
    tmpIconWidth = 0, offset = 0, counter, correctType;
  char *sortResult = (char *) NULL, **argv2, **argv3;

  createDataNode = NULL;
  
  /* Initialize the sort id list. */
  if (Tcl_SetVar(interp, "tkIconLocalList", " ", 0) == NULL) {
    Tcl_AppendResult(canvasPtr->interp, "error during layouting: ",
                     "initializing tkIconLocalList",
                     (char *) NULL);
    return TCL_ERROR;
  }
  
  /* Scan through all canvas items. */
  FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
    /* Add icons to the sort id list. */
    correctType = 0;
    for (counter = 0; counter < layoutTypesNum; counter++) {
      if (strcmp(CANVAS_ITEM_TYPE(itemPtr),
                 *(layoutTypes + counter)) == 0 &&
          strcmp(CANVAS_ITEM_TYPE(itemPtr), "line") != 0 &&
          strcmp(CANVAS_ITEM_TYPE(itemPtr), "polygon") != 0) {
        correctType = 1;
      }
    }
    if (correctType) {
      /* Find the greatest icon dimensions. */
      if (computeiconsize) {
        if (CANVAS_ITEM_WIDTH(itemPtr) > iconWidth) {
          iconWidth = CANVAS_ITEM_WIDTH(itemPtr);
        }
        if (CANVAS_ITEM_HEIGHT(itemPtr) > iconHeight) {
          iconHeight = CANVAS_ITEM_HEIGHT(itemPtr);
        }
      }
      sprintf(convertBuffer, "%ld", (long) CANVAS_ITEM_ID(itemPtr));
      if (Tcl_SetVar(interp, "tkIconLocalList", convertBuffer,
                     TCL_APPEND_VALUE | TCL_LIST_ELEMENT) == NULL) {
        Tcl_UnsetVar(interp, "tkIconLocalList", 0);
        Tcl_AppendResult(canvasPtr->interp, "error during layouting: ",
                         "appending to tkIconLocalList",
                         (char *) NULL);
        return TCL_ERROR;
      }
    }
  }

  /* Do we have to sort ? */
  if (sortcommand != (char *) NULL && strlen(sortcommand) > (size_t) 0) {
    sortResult = Tcl_GetVar(interp, "tkIconLocalList",
                            TCL_LEAVE_ERR_MSG);
    /* Sort the list. */
    if (Tcl_VarEval(interp, sortcommand, " ",
                    Tk_PathName(canvasPtr->tkwin), " {",
                    sortResult, "}", (char *) NULL) != TCL_OK) {
      Tcl_UnsetVar(interp, "tkIconLocalList", 0);
      Tcl_AppendResult(canvasPtr->interp, "error during layouting: ",
                       "sorting", (char *) NULL);
      return TCL_ERROR;
    }
    if (Tcl_SetVar(interp, "tkIconLocalList", interp->result,
                   0) == NULL) {
      Tcl_AppendResult(canvasPtr->interp, "error during layouting: ",
                       "updating tkIconLocalList",
                       (char *) NULL);
      return TCL_ERROR;
    }
  }
  /* Get the list of ids to layout. */
  sortResult = Tcl_GetVar(interp, "tkIconLocalList", TCL_LEAVE_ERR_MSG);
  if (Tcl_SplitList(canvasPtr->interp, sortResult,
                    &argc2, &argv2) != TCL_OK) {
    Tcl_UnsetVar(interp, "tkIconLocalList", 0);
    Tcl_AppendResult(canvasPtr->interp, "error during layouting: ",
                     "retrieving tkIconLocalList",
                     (char *) NULL);
    return TCL_ERROR;
  }
  
  /* Walk through the list of ids. */
  for (counter = 0; counter < argc2; counter++) {
    /* Match the id from the list to a concrete itemPtr. */
    curPtr = NULL;
    FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
      if (CANVAS_ITEM_ID(itemPtr) == atol(argv2[counter])) {
        curPtr = itemPtr;
        break;
      }
    }
    if (curPtr == NULL) {
      continue;
    }
    
    if (iconWidth == 0) {
      tmpIconWidth = CANVAS_ITEM_WIDTH(curPtr);
      offset = ((CANVAS_ITEM_WIDTH(curPtr)) / 2) -
        ((CANVAS_ITEM_WIDTH(curPtr)) / 2);
    } else {
      tmpIconWidth = iconWidth;
      offset = (iconWidth / 2) - ((CANVAS_ITEM_WIDTH(curPtr)) / 2);
    }
    /* Is this the highest icon so far ? */
    if (CANVAS_ITEM_HEIGHT(curPtr) > greatestHeight) {
      greatestHeight = CANVAS_ITEM_HEIGHT(curPtr);
    }
    
    /* Place icon on the current line. */
    if ((greatestX + tmpIconWidth < maxX || columnCounter == 0) &&
        (elementsPerLine == 0 || columnCounter < elementsPerLine)) {
      if (strcmp(CANVAS_ITEM_TYPE(curPtr), "icon") == 0 ||
          strcmp(CANVAS_ITEM_TYPE(curPtr), "bitmap") == 0 ||
          strcmp(CANVAS_ITEM_TYPE(curPtr), "text") == 0 ||
          strcmp(CANVAS_ITEM_TYPE(curPtr), "window") == 0) {
        sprintf(convertBuffer, "%ld %ld ",
                offset + greatestX + xOffset, greatestY + yOffset);
      } else {
        if (strcmp(CANVAS_ITEM_TYPE(curPtr), "arc") == 0 ||
            strcmp(CANVAS_ITEM_TYPE(curPtr), "oval") == 0 ||
            strcmp(CANVAS_ITEM_TYPE(curPtr), "rectangle") == 0) {
          sprintf(convertBuffer, "%ld %ld %ld %ld ",
                  offset + greatestX + xOffset, greatestY + yOffset,
                  offset + greatestX + xOffset + CANVAS_ITEM_WIDTH(curPtr),
                  greatestY + yOffset + CANVAS_ITEM_HEIGHT(curPtr));
        }
      }
      greatestX += (tmpIconWidth + iconSpaceH);
      columnCounter++;
      
      if (Tcl_SplitList(canvasPtr->interp, convertBuffer,
                        &argc3, &argv3) != TCL_OK) {
        Tcl_UnsetVar(interp, "tkIconLocalList", 0);
        Tcl_AppendResult(canvasPtr->interp, "error during layouting: ",
                         "splitting geometry", (char *) NULL);
        ckfree((char *) argv2);
        return TCL_ERROR;
      }
      (*curPtr->typePtr->coordProc)
        (interp, (Tk_Canvas) canvasPtr, curPtr, argc3, argv3);
      ckfree((char *) argv3);
    } else {
      /* Place icon on the next line. */
      if (iconHeight > 0) {
        greatestY += (iconHeight + iconSpaceV);
      } else {
        greatestY += (greatestHeight + iconSpaceV);
      }
      if (strcmp(CANVAS_ITEM_TYPE(curPtr), "icon") == 0 ||
          strcmp(CANVAS_ITEM_TYPE(curPtr), "bitmap") == 0 ||
          strcmp(CANVAS_ITEM_TYPE(curPtr), "text") == 0 ||
          strcmp(CANVAS_ITEM_TYPE(curPtr), "window") == 0) {
        sprintf(convertBuffer, "%ld %ld ",
                10 + offset + xOffset, greatestY + yOffset);
      } else {
        if (strcmp(CANVAS_ITEM_TYPE(curPtr), "arc") == 0 ||
            strcmp(CANVAS_ITEM_TYPE(curPtr), "oval") == 0 ||
            strcmp(CANVAS_ITEM_TYPE(curPtr), "rectangle") == 0) {
          sprintf(convertBuffer, "%ld %ld %ld %ld ",
                  10 + offset + xOffset, greatestY + yOffset,
                  10 + offset + xOffset + CANVAS_ITEM_WIDTH(curPtr),
                  greatestY + yOffset + CANVAS_ITEM_HEIGHT(curPtr));
        }
      }
      greatestHeight = CANVAS_ITEM_HEIGHT(curPtr);
      greatestX = 10 + tmpIconWidth + iconSpaceH;
      columnCounter = 1;
      
      if (Tcl_SplitList(canvasPtr->interp, convertBuffer,
                        &argc3, &argv3) != TCL_OK) {
        Tcl_UnsetVar(interp, "tkIconLocalList", 0);
        Tcl_AppendResult(canvasPtr->interp, "error during layouting: ",
                         "splitting geometry", (char *) NULL);
        ckfree((char *) argv2);
        return TCL_ERROR;
      }
      (*curPtr->typePtr->coordProc)
        (interp, (Tk_Canvas) canvasPtr, curPtr, argc3, argv3);
      ckfree((char *) argv3);
    }
  }
  
  ckfree((char *) argv2);
  Tcl_UnsetVar(interp, "tkIconLocalList", 0);
  return result;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutRandom --
 *
 *        This procedure is invoked to place icons randomly.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static int
TkCanvLayoutRandom(canvasPtr, interp)
     TkCanvas *canvasPtr;                /* Information about canvas widget. */
     Tcl_Interp *interp;                 /* Current interpreter. */
{
  register Tk_Item *itemPtr = NULL;
  int argc2 = 0, argc3 = 0, result = TCL_OK, counter, correctType;
  long tmpX, tmpY;
  char **argv2, **argv3;
  
  createDataNode = NULL;
  SRANDOM(getpid() + time((time_t *) NULL));
  
  if (idlist != NULL) {
    if (Tcl_SplitList(canvasPtr->interp, idlist,
                      &argc2, &argv2) == TCL_OK) {
      /* Walk through all ids in the passed list. */
      for (counter = 0; counter < argc2; counter++) {
        /* Walk through all canvas ids. */
        FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
          /* Is this canvas id the current idlist element. */
          if (CANVAS_ITEM_ID(itemPtr) == atol(argv2[counter])) {
            /* are we allowed to place the icon ? */
            if (keepRandomPositions &&
                CANVAS_ITEM_X_POS(itemPtr) > 0 &&
                CANVAS_ITEM_Y_POS(itemPtr) > 0) {
              continue;
            }
            /* Randomly place this object. */
            tmpX = (long) (RANDOM() % maxX) - CANVAS_ITEM_WIDTH(itemPtr);
            tmpY = (long) (RANDOM() % maxY) - CANVAS_ITEM_HEIGHT(itemPtr);
            if (tmpX <= 0) {
              tmpX = 1;
            }
            if (tmpY <= 0) {
              tmpY = 1;
            }
            if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "icon") == 0 ||
                strcmp(CANVAS_ITEM_TYPE(itemPtr), "bitmap") == 0 ||
                strcmp(CANVAS_ITEM_TYPE(itemPtr), "text") == 0 ||
                strcmp(CANVAS_ITEM_TYPE(itemPtr), "window") == 0) {
              sprintf(convertBuffer, "%ld %ld ",
                      tmpX + xOffset, tmpY + yOffset);
            } else {
              if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "arc") == 0 ||
                  strcmp(CANVAS_ITEM_TYPE(itemPtr), "oval") == 0 ||
                  strcmp(CANVAS_ITEM_TYPE(itemPtr), "rectangle") == 0) {
                sprintf(convertBuffer, "%ld %ld %ld %ld ",
                        tmpX + xOffset, tmpY + yOffset,
                        tmpX + xOffset + CANVAS_ITEM_WIDTH(itemPtr),
                        tmpY + yOffset + CANVAS_ITEM_HEIGHT(itemPtr));
              }
            }
            if (Tcl_SplitList(canvasPtr->interp, convertBuffer,
                              &argc3, &argv3) != TCL_OK) {
              Tcl_AppendResult(canvasPtr->interp,
                               "error during layouting: ",
                               "splitting geometry", (char *) NULL);
              ckfree((char *) argv2);
              return TCL_ERROR;
            }
            (*itemPtr->typePtr->coordProc)
              (interp, (Tk_Canvas) canvasPtr, itemPtr, 
               argc3, argv3);
            ckfree((char *) argv3);
            break;
          }
        }
      }
      ckfree((char *) argv2);
    }
  } else {
    /* Walk through all canvas ids. */
    FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
      /* Randomly place icons. */
      correctType = 0;
      /* Add icons to the sort id list. */
      for (counter = 0; counter < layoutTypesNum; counter++) {
        if (strcmp(CANVAS_ITEM_TYPE(itemPtr),
                   *(layoutTypes + counter)) == 0 &&
            strcmp(CANVAS_ITEM_TYPE(itemPtr), "line") != 0 &&
            strcmp(CANVAS_ITEM_TYPE(itemPtr), "polygon") != 0) {
          correctType = 1;
        }
      }
      if (correctType) {
        /* are we allowed to place the icon ? */
        if (keepRandomPositions &&
            CANVAS_ITEM_X_POS(itemPtr) > 0 &&
            CANVAS_ITEM_Y_POS(itemPtr) > 0) {
          continue;
        }
        tmpX = (long) (RANDOM() % maxX) - CANVAS_ITEM_WIDTH(itemPtr);
        tmpY = (long) (RANDOM() % maxY) - CANVAS_ITEM_HEIGHT(itemPtr);
        if (tmpX <= 0) {
          tmpX = 1;
        }
        if (tmpY <= 0) {
          tmpY = 1;
        }
        if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "icon") == 0 ||
            strcmp(CANVAS_ITEM_TYPE(itemPtr), "bitmap") == 0 ||
            strcmp(CANVAS_ITEM_TYPE(itemPtr), "text") == 0 ||
            strcmp(CANVAS_ITEM_TYPE(itemPtr), "window") == 0) {
          sprintf(convertBuffer, "%ld %ld ",
                  tmpX + xOffset, tmpY + yOffset);
        } else {
          if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "arc") == 0 ||
              strcmp(CANVAS_ITEM_TYPE(itemPtr), "oval") == 0 ||
              strcmp(CANVAS_ITEM_TYPE(itemPtr), "rectangle") == 0) {
            sprintf(convertBuffer, "%ld %ld %ld %ld ",
                    tmpX + xOffset, tmpY + yOffset,
                    tmpX + xOffset + CANVAS_ITEM_WIDTH(itemPtr),
                    tmpY + yOffset + CANVAS_ITEM_HEIGHT(itemPtr));
          }
        }
        if (Tcl_SplitList(canvasPtr->interp, convertBuffer,
                          &argc3, &argv3) != TCL_OK) {
          Tcl_AppendResult(canvasPtr->interp,
                           "error during layouting: ",
                           "splitting geometry", (char *) NULL);
          return TCL_ERROR;
        }
        (*itemPtr->typePtr->coordProc)
          (interp, (Tk_Canvas) canvasPtr, itemPtr, argc3, argv3);
        ckfree((char *) argv3);
      }
    }
  }
  return result;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutTree --
 *
 *        This procedure is invoked to place icons as tree.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static int
TkCanvLayoutTree(canvasPtr, interp)
     TkCanvas *canvasPtr;                /* Information about canvas widget. */
     Tcl_Interp *interp;                 /* Current interpreter. */
{
  int result = TCL_OK, counter;
  
  maxXPosition = 0;
  maxYPosition = 0;
  nodeNum = 0;
  nodes = (Node **) NULL;
  topListNum = 0;
  topList = (Topology **) NULL;
  treeData = (TreeData **) NULL;
  treeDataNum = 0;
  rootNode = (Node *) NULL;
  createDataNode = TkCanvLayoutCreateTreeNode;
  
  /* Find the widest/highest Edge. */
  TkCanvLayoutEdgeWidth(canvasPtr, interp);
  
  /* Build the internal graph structure. */
  if (TkCanvLayoutBuildGraph(canvasPtr, interp) != TCL_OK) {
    return TCL_ERROR;
  }
  
  /* Find root of the graph. */
  if ((rootNode = TkCanvLayoutGraphRoot(canvasPtr)) == NULL) {
    Tcl_AppendResult(interp, "error during layouting: ",
                     "no root node", (char *) NULL);
    return TCL_ERROR;
  }
  
  /* Sort the graph topological. */
  TkCanvLayoutGraphSortTopological(canvasPtr, interp, rootNode);
  FOR_ALL_NODES(counter) {
    if (PARENT_NUM(nodes[counter]) == 0) {
      TkCanvLayoutGraphSortTopological(canvasPtr, interp, nodes[counter]);
    }
  }
  
  /* Calculate the position values. */
  RESET_VISITED_NODE(counter);
  TkCanvLayoutTreeSetX(canvasPtr, interp, rootNode);
  FOR_ALL_NODES(counter) {
    if (PARENT_NUM(nodes[counter]) == 0) {
      TkCanvLayoutTreeSetX(canvasPtr, interp, nodes[counter]);
    }
  }
  RESET_VISITED_NODE(counter);
  FOR_ALL_TOP_NODES(counter) {
    TkCanvLayoutTreeSetY(canvasPtr, interp, TOP_NODE(counter));
  }
  
  /* Place the graph items. */
  if (TkCanvLayoutGraphPlaceNodes(canvasPtr, interp) != TCL_OK) {
    TkCanvLayoutFreeGraph();
    return TCL_ERROR;
  }
  
  /* free layout specific data slots. */
  for (counter = 0; counter < treeDataNum; counter++) {
    ckfree((char *) treeData[counter]);
  }
  ckfree((char *) treeData);
  
  /* Free the internal graph structure. */
  TkCanvLayoutFreeGraph();
  
  return result;
}

/*
 *--------------------------------------------------------------
 *
 * TkCanvLayoutCmd --
 *
 *        This procedure is invoked to process the "layout" options
 *        of the widget command for canvas widgets. See the user
 *        documentation for details on what it does.
 *
 * Results:
 *        A standard Tcl result.
 *
 * Side effects:
 *        See the user documentation.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
int
TkCanvLayoutCmd(canvasPtr, interp, graph, argc, argv)
     TkCanvas *canvasPtr;                /* Information about canvas widget. */
     Tcl_Interp *interp;                 /* Current interpreter. */
     char *graph;                        /* The graph name. */
     int argc;                           /* Number of arguments. */
     char **argv;                        /* Argument strings.  Caller has
                                          * already parsed this command enough
                                          * to know that argv[1] is
                                          * "postscript". */
{
  register Tk_Item *itemPtr;
  int argc2, result = TCL_OK, updateEdges = 0, counter, correctType; 
  size_t length = 0;
  char c, **argv2;
  Tcl_DString tmpString;
  
  /* Initialize global data. */
  computeiconsize = 0;
  edgeOrder = 0;
  edgeHeight = 2;
  edgeWidth = 0;
  elementsPerLine = 0;
  graphName = graph;
  iconHeight = 0;
  iconSpaceH = 5;
  iconSpaceV = 5;
  iconWidth = 0;
  keepRandomPositions = 0;
  maxX = 0;
  maxY = 0;
  graphOrder = 0;
  rootId = -1;
  xOffset = 4;
  yOffset = 4;
  idlist = (char *) NULL;
  sortcommand = (char *) NULL;
  Tcl_DStringInit(&tmpString);
  Tcl_DStringAppend(&tmpString, "icon", -1);
  if (Tcl_SplitList(canvasPtr->interp, tmpString.string,
                    &layoutTypesNum, &layoutTypes) != TCL_OK) {
    Tcl_AppendResult(interp, "error during layouting: ",
                     "reading parameters", (char *) NULL);
    return TCL_ERROR;
  }
  Tcl_DStringFree(&tmpString);
  
  /* Parse parameter list. */
  for (counter = 3; counter < argc; counter++) {
    if (strcmp(argv[counter], "-computeiconsize") == 0) {
      counter++;
      if (counter <= argc) {
        computeiconsize = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-edgeorder") == 0) {
      counter++;
      if (counter <= argc) {
        edgeOrder = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-edgeheight") == 0) {
      counter++;
      if (counter <= argc) {
        edgeHeight = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-edgewidth") == 0) {
      counter++;
      if (counter <= argc) {
        edgeWidth = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-elementsperline") == 0) {
      counter++;
      if (counter <= argc) {
        elementsPerLine = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-iconheight") == 0) {
      counter++;
      if (counter <= argc) {
        iconHeight = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-iconspaceh") == 0) {
      counter++;
      if (counter <= argc) {
        iconSpaceH = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-iconspacev") == 0) {
      counter++;
      if (counter <= argc) {
        iconSpaceV = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-iconwidth") == 0) {
      counter++;
      if (counter <= argc) {
        iconWidth = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-idlist") == 0) {
      counter++;
      if (counter <= argc) {
        idlist = argv[counter];
      }
    } else if (strcmp(argv[counter], "-keeprandompositions") == 0) {
      counter++;
      if (counter <= argc) {
        keepRandomPositions = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-layouttypes") == 0) {
      counter++;
      if (counter <= argc) {
        ckfree((char *) layoutTypes);
        if (Tcl_SplitList(canvasPtr->interp, argv[counter],
                          &layoutTypesNum, &layoutTypes) != TCL_OK) {
          Tcl_AppendResult(interp, "error during layouting: ",
                           "reading parameters", (char *) NULL);
          return TCL_ERROR;
        }
      }
    } else if (strcmp(argv[counter], "-maxx") == 0) {
      counter++;
      if (counter <= argc) {
        maxX = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-maxy") == 0) {
      counter++;
      if (counter <= argc) {
        maxY = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-graphorder") == 0) {
      counter++;
      if (counter <= argc) {
        graphOrder = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-rootid") == 0) {
      counter++;
      if (counter <= argc) {
        rootId = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-sortcommand") == 0) {
      counter++;
      if (counter <= argc) {
        sortcommand = argv[counter];
      }
    } else if (strcmp(argv[counter], "-xoffset") == 0) {
      counter++;
      if (counter <= argc) {
        xOffset = atol(argv[counter]);
      }
    } else if (strcmp(argv[counter], "-yoffset") == 0) {
      counter++;
      if (counter <= argc) {
        yOffset = atol(argv[counter]);
      }
    } else {
      Tcl_AppendResult(canvasPtr->interp, "wrong option:  \"",
                       argv[counter], "\"", (char *) NULL);
      ckfree((char *) layoutTypes);
      return TCL_ERROR;
    }
  }
  
  if (maxX == 0) {
    maxX = Tk_Width(canvasPtr->tkwin) - xOffset;
    if (maxX < 2) {
      maxX = Tk_ReqWidth(canvasPtr->tkwin) - xOffset;
    }
  }
  if (maxX <= 0) {
    maxX = 100;
  }
  if (maxY == 0) {
    maxY = Tk_Height(canvasPtr->tkwin) - yOffset;
    if (maxY < 2) {
      maxY = Tk_ReqHeight(canvasPtr->tkwin) - yOffset;
    }
  }
  if (maxY <= 0) {
    maxY = 100;
  }
  
  c = argv[2][0];
  length = strlen(argv[2]);
  if ((c == 'e') && (strncmp(argv[2], "edges", length) == 0)) {
    if (argc < 3 || argc % 2 == 0) {
      Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
                       Tk_PathName(canvasPtr->tkwin),
                       " layout edges ?option value?",
                       (char *) NULL);
      ckfree((char *) layoutTypes);
      return TCL_ERROR;
    }
    
    updateEdges = 1;
  } else if ((c == 'i') && (strncmp(argv[2], "isi", length) == 0)) {
    if (argc < 3 || argc % 2 == 0) {
      Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
                       Tk_PathName(canvasPtr->tkwin),
                       " layout isi ?option value?",
                       (char *) NULL);
      ckfree((char *) layoutTypes);
      return TCL_ERROR;
    }
    
    /* Place icons. */
    TkCanvLayoutISI(canvasPtr, interp);
    updateEdges = 1;
  } else if ((c == 'm') && (strncmp(argv[2], "matrix", length) == 0)) {
    if (argc < 3 || argc % 2 == 0) {
      Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
                       Tk_PathName(canvasPtr->tkwin), " layout",
                       " matrix ?option value?\"", (char *) NULL);
      ckfree((char *) layoutTypes);
      return TCL_ERROR;
    }
    
    /* Place icons. */
    TkCanvLayoutMatrix(canvasPtr, interp);
    updateEdges = 1;
  } else if ((c == 'r') && (strncmp(argv[2], "random", length) == 0)) {
    if (argc < 3 || argc % 2 == 0) {
      Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
                       Tk_PathName(canvasPtr->tkwin),
                       " layout random ?option value?",
                       (char *) NULL);
      ckfree((char *) layoutTypes);
      return TCL_ERROR;
    }
    
    /* Place icons. */
    TkCanvLayoutRandom(canvasPtr, interp);
    updateEdges = 1;
  } else if ((c == 't') && (strncmp(argv[2], "tree", length) == 0)) {
    if (argc < 3 || argc % 2 == 0) {
      Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
                       Tk_PathName(canvasPtr->tkwin),
                       " layout tree ?option value?",
                       (char *) NULL);
      ckfree((char *) layoutTypes);
      return TCL_ERROR;
    }
    
    /* Place icons. */
    TkCanvLayoutTree(canvasPtr, interp);
    updateEdges = 1;
  } else if ((c == 't') && (strncmp(argv[2], "testing", length) == 0)) {
    /* 
     * Add new layout mechanisms here. Simply write your own functions,
     * and activate them with a "if" statement like below.
     */
    char *graphString;
    
    if (argc < 3 || argc % 2 == 0) {
      Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
                       Tk_PathName(canvasPtr->tkwin),
                       " layout testing ?option value?",
                       (char *) NULL);
      ckfree((char *) layoutTypes);
      return TCL_ERROR;
    }
    
    /* Place icons. */
    graphString = TkCanvLayoutBuildGraphToString(canvasPtr, interp);
    fprintf(stderr, "result %s\n", graphString);
    TkCanvLayoutGraphPlaceNodesFromString(canvasPtr, interp, graphString);
    ckfree((char *) graphString);
  } else {
    if (argc < 3 || argc % 2 == 0) {
      Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
                       Tk_PathName(canvasPtr->tkwin),
                       " layout item ?option value?",
                       (char *) NULL);
      ckfree((char *) layoutTypes);
      return TCL_ERROR;
    }
    
    if (idlist != NULL) {
      if (Tcl_SplitList(canvasPtr->interp, idlist,
                        &argc2, &argv2) == TCL_OK) {
        /* walk through all ids in the passed list. */
        for (counter = 0; counter < argc2; counter++) {
          /* walk through all canvas ids. */
          FOR_ALL_CANVAS_ITEMS(canvasPtr, itemPtr) {
            /* is this canvas id the current idlist element. */
            if (CANVAS_ITEM_ID(itemPtr) == atol(argv2[counter])) {
              if (strcmp(CANVAS_ITEM_TYPE(itemPtr), "edge") == 0) {
                TkCanvLayoutEdge(canvasPtr, interp, itemPtr, NULL, NULL);
              } else {
                correctType = 0;
                /* Add icons to the sort id list. */
                for (counter = 0; counter < layoutTypesNum; counter++) {
                  if (strcmp(CANVAS_ITEM_TYPE(itemPtr),
                             *(layoutTypes + counter)) == 0 &&
                      strcmp(CANVAS_ITEM_TYPE(itemPtr), "line") != 0 &&
                      strcmp(CANVAS_ITEM_TYPE(itemPtr), "polygon") != 0) {
                    correctType = 1;
                  }
                }
                if (correctType) {
                  TkCanvLayoutRandom(canvasPtr, interp);
                }
              }
            }
          }
        }
        ckfree((char *) argv2);
      }
    }
  }
  
  if (updateEdges) {
    TkCanvLayoutGraphPlaceEdges(canvasPtr, interp);
  }
  
  /* Free the layout types. */
  ckfree((char *) layoutTypes);
  
  Tcl_ResetResult(canvasPtr->interp);
  return result;
}

/* eof */


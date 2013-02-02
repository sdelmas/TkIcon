/* 
 * tkIcon.c --
 *
 *      This module implements functionality required for sending
 *      X events to windows and do other Xlib stuff.
 *
 * Copyright (c) 1995 by Sven Delmas
 * All rights reserved.
 * See the file COPYRIGHT for the copyright notes.
 *
 */

#if defined(USE_TKICON)

#ifndef lint
static char *AtFSid = "$Header: tkIcon.c[1.6] Wed Jul 19 17:30:18 1995 sven@(none) accessed $";
#endif /* not lint */

#include "tclInt.h"
#include "tkPort.h"
#include "tkCanvas.h"
#include "tkInt.h"

/*
 * A data structure of the following type is kept for each
 * graph that currently exists for this process:
 */ 
typedef struct tkIcon *TkIconPtr;
typedef struct tkIcon {
  char *graphName;              /* Graph name.  Malloc-ed. */
  TkCanvas *canvasPtr;          /* The canvas connected to the
                                 * graph. */
  TkIconPtr nextPtr;            /* we just keep a simple
                                 * linked list of all graphs, as
                                 * there will not be that many. */
} TkIcon;

/*
 * The start point for the linked list of graph
 * data structures.
 */
static TkIconPtr listStart = NULL;

extern int TkCanvLayoutCmd _ANSI_ARGS_((TkCanvas *canvasPtr,
                                        Tcl_Interp *interp,
                                        char *graph,
                                        int argc,
                                        char **argv));
extern void TkIconEdge_Init _ANSI_ARGS_(());
extern void TkIconIcon_Init _ANSI_ARGS_(());

/*
 *----------------------------------------------------------------------
 *
 * TkGraph_GraphCmd --
 *
 *      This procedure gives access to one specific graph.
 *
 * Results:
 *      Returns a standard Tcl completion code, and leaves an error
 *      message in interp->result if an error occurs.
 *
 * Side effects:
 *      Depends on the performed action.
 *
 *----------------------------------------------------------------------
 */
int
TkGraph_GraphCmd(clientData, interp, argc, argv)
     ClientData clientData;     /* Main window associated with
                                 * interpreter. */
     Tcl_Interp *interp;        /* Current interpreter. */
     int argc;                  /* Number of arguments. */
     char **argv;               /* Argument strings. */
{
  char c;
  int result = TCL_OK;
  size_t length = 0;
  register TkIconPtr tkIconPtr = (TkIconPtr) clientData;
  Tk_Item *itemPtr = NULL;
  TkCanvas *canvasPtr;

  if (argc < 2) {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, "wrong # args: should be \"",
                     argv[0], " command ?arg arg....?\"",
                     (char *) NULL);
    return TCL_ERROR;
  }

  if (tkIconPtr->canvasPtr == NULL) {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, "\"", argv[0], "\": this graph was not",
                     " attached to a canvas widget", (char *) NULL);
    return TCL_ERROR;
  }
  canvasPtr = tkIconPtr->canvasPtr;

  c = argv[1][0];
  length = strlen(argv[1]);
  if ((c == 'e') && (strncmp(argv[1], "edgeinfo", length) == 0)) {
    int argc2, argc3, argc4, counter, appendIt;
    char **argv2, **argv3, **argv4, convertBuffer[20];
    Tcl_DString resultStr, command;
    
    if (argc < 3) {
      Tcl_AppendResult(interp, "wrong # args:  should be \"", argv[0], 
                       " edgeinfo command ?arg arg....?\"",
                       (char *) NULL);
      goto error;
    }
    
    Tcl_DStringInit(&resultStr);
    if (argc == 3) {
      if (!strcmp(argv[2], "ids")) {
        for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
             itemPtr = itemPtr->nextPtr) {
          if (!strcmp(itemPtr->typePtr->name, "edge")) {
            sprintf(convertBuffer, "%d", itemPtr->id);
            Tcl_DStringAppendElement(&resultStr, convertBuffer);
          }
        }
      } else if (!strcmp(argv[2], "names")) {
        for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
             itemPtr = itemPtr->nextPtr) {
          if (!strcmp(itemPtr->typePtr->name, "edge")) {
            result = Tk_ConfigureInfo(interp, canvasPtr->tkwin,
                                      itemPtr->typePtr->configSpecs, 
                                      (char *) itemPtr, "-name", 0);
            if (Tcl_SplitList(interp, interp->result,
                              &argc2, &argv2) != TCL_OK) {
              goto edgeinfo_error;
            }
            Tcl_DStringAppendElement(&resultStr, argv2[4]);
            ckfree((char *) argv2);
          }
        }
      } else if (!strcmp(argv[2], "graphs")) {
        for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
             itemPtr = itemPtr->nextPtr) {
          if (!strcmp(itemPtr->typePtr->name, "edge")) {
            result = Tk_ConfigureInfo(interp, canvasPtr->tkwin,
                                      itemPtr->typePtr->configSpecs, 
                                      (char *) itemPtr,
                                      "-graphname", 0);
            if (Tcl_SplitList(interp, interp->result,
                              &argc2, &argv2) != TCL_OK) {
              goto edgeinfo_error;
            }
            if (Tcl_SplitList(interp, resultStr.string,
                              &argc3, &argv3) != TCL_OK) {
              ckfree((char *) argv2);
              goto edgeinfo_error;
            }
            appendIt = 1;
            for (counter = 0; counter < argc3; counter++) {
              if (!strcmp(argv3[counter], argv2[4])) {
                appendIt = 0;
              }
            }
            if (appendIt) {
              Tcl_DStringAppendElement(&resultStr, argv2[4]);
            }
            ckfree((char *) argv3);
            ckfree((char *) argv2);
          }
        }
      }
    } else {
      if (argc == 4) {
        if (!strcmp(argv[2], "ids")) {
          for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
               itemPtr = itemPtr->nextPtr) {
            result = Tk_ConfigureInfo(interp, canvasPtr->tkwin,
                                      itemPtr->typePtr->configSpecs, 
                                      (char *) itemPtr,
                                      "-graphname", 0);
            if (Tcl_SplitList(interp, interp->result,
                              &argc2, &argv2) != TCL_OK) {
              goto edgeinfo_error;
            }
            if (!strcmp(itemPtr->typePtr->name, "edge") &&
                !strcmp(argv2[4], argv[3])) {
              sprintf(convertBuffer, "%d", itemPtr->id);
              Tcl_DStringAppendElement(&resultStr, convertBuffer);
            }
            ckfree((char *) argv2);
          }
        } else if (!strcmp(argv[2], "names")) {
          for (itemPtr = canvasPtr->firstItemPtr;
               itemPtr != NULL;
               itemPtr = itemPtr->nextPtr) {
            result = Tk_ConfigureInfo(interp, canvasPtr->tkwin,
                                      itemPtr->typePtr->configSpecs, 
                                      (char *) itemPtr,
                                      "-graphname", 0);
            if (Tcl_SplitList(interp, interp->result,
                              &argc2, &argv2) != TCL_OK) {
              goto edgeinfo_error;
            }
            if (!strcmp(itemPtr->typePtr->name, "edge") &&
                !strcmp(argv2[4], argv[3])) {
              result = Tk_ConfigureInfo(interp, canvasPtr->tkwin,
                                        itemPtr->typePtr->configSpecs, 
                                        (char *) itemPtr,
                                        "-name", 0);
              if (Tcl_SplitList(interp, interp->result,
                                &argc3, &argv3) != TCL_OK) {
                ckfree((char *) argv2);
                goto edgeinfo_error;
              }
              Tcl_DStringAppendElement(&resultStr, argv3[4]);
              ckfree((char *) argv3);
            }
            ckfree((char *) argv2);
          }
        }
      } else {
        if (!strcmp(argv[2], "id")) {
          for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
               itemPtr = itemPtr->nextPtr) {
            if (strcmp(itemPtr->typePtr->name, "icon") == 0) {
              result = Tk_ConfigureInfo(interp,
                                        canvasPtr->tkwin,
                                        itemPtr->typePtr->configSpecs, 
                                        (char *) itemPtr,
                                        "-edges", 0);
              if (Tcl_SplitList(interp, interp->result,
                                &argc2, &argv2) != TCL_OK) {
                goto edgeinfo_error;
              }
              if (Tcl_SplitList(interp, argv2[4],
                                &argc3, &argv3) != TCL_OK) {
                ckfree((char *) argv2);
                goto edgeinfo_error;
              }
              Tcl_DStringInit(&command);
              Tcl_DStringAppend(&command, "lsearch -glob {", -1);
              Tcl_DStringAppend(&command, argv2[4], -1);
              Tcl_DStringAppend(&command, "} {", -1);
              Tcl_DStringAppend(&command, argv[4], -1);
              Tcl_DStringAppend(&command, " ", -1);
              Tcl_DStringAppend(&command, argv[5], -1);
              Tcl_DStringAppend(&command, " *}", -1);
              Tcl_Eval(interp, command.string);
              if (strcmp(interp->result, "-1")) {
                if (Tcl_SplitList(interp,
                                  argv3[atol(interp->result)],
                                  &argc4, &argv4) != TCL_OK) {
                  ckfree((char *) argv3);
                  ckfree((char *) argv2);
                  goto edgeinfo_error;
                }
                Tcl_DStringAppend(&resultStr, argv4[2], -1);
                ckfree((char *) argv4);
                ckfree((char *) argv3);
                ckfree((char *) argv2);
                goto edgeinfo_finished;
              }
              Tcl_DStringFree(&command);
              ckfree((char *) argv3);
              ckfree((char *) argv2);
            }
          }
        }
      }
    }
    
  edgeinfo_finished:
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, resultStr.string, (char *) NULL);
    Tcl_DStringFree(&resultStr);
    goto done;
    
  edgeinfo_error:
    Tcl_ResetResult(interp);
    goto error;
        
  } else if ((c == 'i') && (strncmp(argv[1], "iconinfo", length) == 0)) {
    int argc2;
    char **argv2, convertBuffer[20];
    Tcl_DString resultStr;
    
    if (argc < 3) {
      Tcl_AppendResult(interp, "wrong # args:  should be \"", argv[0],
                       " iconinfo command ?options?\"", (char *) NULL);
      goto error;
    }
    
    Tcl_DStringInit(&resultStr);
    if (argc == 3) {
      if (!strcmp(argv[2], "ids")) {
        for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
             itemPtr = itemPtr->nextPtr) {
          if (!strcmp(itemPtr->typePtr->name, "icon")) {
            sprintf(convertBuffer, "%d", itemPtr->id);
            Tcl_DStringAppendElement(&resultStr, convertBuffer);
          }
        }
      } else if (!strcmp(argv[2], "names")) {
        for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
             itemPtr = itemPtr->nextPtr) {
          if (!strcmp(itemPtr->typePtr->name, "icon")) {
            result = Tk_ConfigureInfo(interp, canvasPtr->tkwin,
                                      itemPtr->typePtr->configSpecs, 
                                      (char *) itemPtr, "-name", 0);
            if (Tcl_SplitList(interp, interp->result,
                              &argc2, &argv2) != TCL_OK) {
              goto iconinfo_error;
            }
            Tcl_DStringAppendElement(&resultStr, argv2[4]);
            ckfree((char *) argv2);
          }
        }
      }
    } else {
      if (!strcmp(argv[2], "id")) {
        for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
             itemPtr = itemPtr->nextPtr) {
          if (strcmp(itemPtr->typePtr->name, "icon") == 0) {
            result = Tk_ConfigureInfo(interp, canvasPtr->tkwin,
                                      itemPtr->typePtr->configSpecs, 
                                      (char *) itemPtr, "-name", 0);
            if (Tcl_SplitList(interp, interp->result,
                              &argc2, &argv2) != TCL_OK) {
              goto iconinfo_error;
            }
            if (!strcmp(argv2[4], argv[3])) {
              sprintf(convertBuffer, "%d", itemPtr->id);
              Tcl_DStringAppend(&resultStr, convertBuffer, -1);
              ckfree((char *) argv2);
              goto iconinfo_finished;
            }
            ckfree((char *) argv2);
          }
        }
      }
    }
    
  iconinfo_finished:
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, resultStr.string, (char *) NULL);
    Tcl_DStringFree(&resultStr);
    goto done;
    
  iconinfo_error:
    Tcl_ResetResult(interp);
    goto error;
    
  } else if ((c == 'l') && (strncmp(argv[1], "layout", length) == 0)) {
    int gotAny = 0, gotAnyScroll = 0;
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0, xScroll2 = 0, yScroll2 = 0;
    char convertBuffer[20];
    Tcl_DString buffer;
    
    if (argc == 2) {
      Tcl_AppendResult(interp, "wrong # args:  should be \"",
                       argv[0], " layout type ?-option value?\"",
                       (char *) NULL);
      goto error;
    }
    
    for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
         itemPtr = itemPtr->nextPtr) {
      if (!gotAny) {
        x1 = itemPtr->x1;
        y1 = itemPtr->y1;
        x2 = itemPtr->x2;
        y2 = itemPtr->y2;
        gotAny = 1;
      } else {
        if (itemPtr->x1 < x1) {
          x1 = itemPtr->x1;
        }
        if (itemPtr->y1 < y1) {
          y1 = itemPtr->y1;
        }
        if (itemPtr->x2 > x2) {
          x2 = itemPtr->x2;
        }
        if (itemPtr->y2 > y2) {
          y2 = itemPtr->y2;
        }
      }
    }
    result = TkCanvLayoutCmd(canvasPtr, interp,
                             tkIconPtr->graphName, argc, argv);
    if (result != TCL_OK) {
      goto error;
    }
    for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
         itemPtr = itemPtr->nextPtr) {
      if (!gotAny) {
        x1 = itemPtr->x1;
        y1 = itemPtr->y1;
        x2 = itemPtr->x2;
        y2 = itemPtr->y2;
        gotAny = 1;
      } else {
        if (itemPtr->x1 < x1) {
          x1 = itemPtr->x1;
        }
        if (itemPtr->y1 < y1) {
          y1 = itemPtr->y1;
        }
        if (itemPtr->x2 > x2) {
          x2 = itemPtr->x2;
        }
        if (itemPtr->y2 > y2) {
          y2 = itemPtr->y2;
        }
      }
      if (!gotAnyScroll) {
        xScroll2 = itemPtr->x2;
        yScroll2 = itemPtr->y2;
        gotAnyScroll = 1;
      } else {
        if (itemPtr->x2 > xScroll2) {
          xScroll2 = itemPtr->x2;
        }
        if (itemPtr->y2 > yScroll2) {
          yScroll2 = itemPtr->y2;
        }
      }
    }
    if (gotAny) {
      Tk_CanvasEventuallyRedraw((Tk_Canvas) canvasPtr, x1, y1, x2, y2);
    }
    Tcl_DStringInit(&buffer);
    Tcl_DStringAppend(&buffer,
                      ((TkWindow *) (canvasPtr->tkwin))->pathName, -1);
    Tcl_DStringAppend(&buffer, " config -scrollregion {0c 0c ", -1);
    sprintf(convertBuffer, "%d", xScroll2 + 20);
    Tcl_DStringAppend(&buffer, convertBuffer, -1);
    Tcl_DStringAppend(&buffer, " ", -1);
    sprintf(convertBuffer, "%d", yScroll2 + 20);
    Tcl_DStringAppend(&buffer, convertBuffer, -1);
    Tcl_DStringAppend(&buffer, " }", -1);
    Tcl_Eval(interp, buffer.string);
    Tcl_DStringFree(&buffer);
  } else {
    Tcl_AppendResult(interp, "bad minor command \"", argv[0],
                     "\":  must be edgeinfo, iconinfo, or layout",
                     (char *) NULL);  
    goto error;
  }
  
 done:
  return result;
  
 error:
  return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TkGraph_CreateGraph --
 *
 *      This procedure creates a new graph structure if required.
 *
 * Results:
 *      Returns a standard Tcl completion code, and leaves an error
 *      message in interp->result if an error occurs.
 *
 * Side effects:
 *      Depends on the performed action.
 *
 *----------------------------------------------------------------------
 */
int
TkGraph_CreateGraph(interp, graphName, canvasPtr)
     Tcl_Interp *interp;        /* Current interpreter. */
     char *graphName;           /* The graph names. */
     TkCanvas *canvasPtr;       /* The canvas attached to the graph. */
{
  int result = TCL_OK;
  Tcl_HashEntry *hPtr;
  Tcl_HashSearch search;
  register Interp *iPtr = (Interp *) interp;
  register TkIconPtr tkIconPtr, tmpIconPtr;

  /* check if we have to create a new graph entry */
  for (tmpIconPtr = listStart; tmpIconPtr != NULL; 
       tmpIconPtr = tmpIconPtr->nextPtr) {
    if (!strcmp(graphName, tmpIconPtr->graphName)) {
      goto done;
    }
  }

  for (hPtr = Tcl_FirstHashEntry(&iPtr->commandTable, &search);
       hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
    char *name = Tcl_GetHashKey(&iPtr->commandTable, hPtr);
    if (!strcmp(name, graphName)) {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, "tkgraph error: the command: \"",
                       graphName, "\" already exists", (char *) NULL);
      goto error;
    }
  }

  /* initialize the graph data structure. */
  tkIconPtr = (TkIconPtr) ckalloc(sizeof(TkIcon));
  tkIconPtr->graphName = ckalloc(strlen(graphName) + 2);
  strcpy(tkIconPtr->graphName, graphName);
  tkIconPtr->canvasPtr = canvasPtr;
  tkIconPtr->nextPtr = NULL;
  
  /* append the data structure to the graph list. */
  if (listStart == NULL) {
    listStart = tkIconPtr;
  } else {
    for (tmpIconPtr = listStart; tmpIconPtr->nextPtr != NULL; 
         tmpIconPtr = tmpIconPtr->nextPtr);
    tmpIconPtr->nextPtr = tkIconPtr;
  }

  /* create the new graph command */
  Tcl_CreateCommand(interp, graphName, TkGraph_GraphCmd,
                    (ClientData) tkIconPtr, (void (*)()) NULL);

 done:
  return result;
  
 error:
  return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TkGraph_Cmd --
 *
 *      This procedure gives access to the graph handling.
 *
 * Results:
 *      Returns a standard Tcl completion code, and leaves an error
 *      message in interp->result if an error occurs.
 *
 * Side effects:
 *      Depends on the performed action.
 *
 *----------------------------------------------------------------------
 */
int
TkGraph_Cmd(clientData, interp, argc, argv)
     ClientData clientData;     /* Main window associated with
                                 * interpreter. */
     Tcl_Interp *interp;        /* Current interpreter. */
     int argc;                  /* Number of arguments. */
     char **argv;               /* Argument strings. */
{
  char c;
  int result = TCL_OK;
  size_t length = 0;
  register TkIconPtr tmpIconPtr;
  Tcl_CmdInfo info;
  TkCanvas *canvasPtr = NULL;

  if (argc < 2) {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, "wrong # args: should be \"",
                     argv[0], " command ?arg arg....?\"",
                     (char *) NULL);
    return TCL_ERROR;
  }

  c = argv[1][0];
  length = strlen(argv[1]);
  if ((c == 'c') && (strncmp(argv[1], "create", length) == 0)) {
    if (argc < 3) {
      Tcl_AppendResult(interp, "wrong # args:  should be \"", argv[0], 
                       " create graphName ?arg arg....?\"",
                       (char *) NULL);
      goto error;
    }

    if (argc == 5 && !strcmp(argv[3], "-canvas")) {
      if (Tcl_GetCommandInfo(interp, argv[4], &info) != 0) {
        canvasPtr = (TkCanvas *) info.clientData;     
      }
    }
    if (TkGraph_CreateGraph(interp, argv[2], canvasPtr) != TCL_OK) {
      goto error;
    }
  } else if ((c == 'n') && (strncmp(argv[1], "names", length) == 0)) {
    Tcl_DString resultStr;
    
    if (argc < 2) {
      Tcl_AppendResult(interp, "wrong # args:  should be \"", argv[0], 
                       " names\"", (char *) NULL);
      goto error;
    }
    
    /* scan through the graphs, and append the names */
    Tcl_DStringInit(&resultStr);
    for (tmpIconPtr = listStart; tmpIconPtr != NULL; 
         tmpIconPtr = tmpIconPtr->nextPtr) {
      Tcl_DStringAppendElement(&resultStr, tmpIconPtr->graphName);
    }
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, resultStr.string, (char *) NULL);
    Tcl_DStringFree(&resultStr);
    goto done;
  }
  
 done:
  return result;
  
 error:
  return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TkIcon_Init --
 *
 *      This procedure performs application-specific initialization.
 *      Most applications, especially those that incorporate additional
 *      packages, will have their own version of this procedure.
 *
 * Results:
 *      Returns a standard Tcl completion code, and leaves an error
 *      message in interp->result if an error occurs.
 *
 * Side effects:
 *      Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */
int
TkIcon_Init(interp)
     Tcl_Interp *interp;         /* Interpreter for application. */
{
  /*
   * Call Tcl_CreateCommand for application-specific commands.
   */
  Tcl_CreateCommand(interp, "tkgraph", TkGraph_Cmd,
                    (ClientData) Tk_MainWindow(interp),
                    (Tcl_CmdDeleteProc *) NULL);
  TkIconEdge_Init();
  TkIconIcon_Init();
  
  return TCL_OK;
}

#endif

/* eof */


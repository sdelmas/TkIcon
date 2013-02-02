/* 
 * tkCanvIcon.c --
 *
 *	This file implements icon items for canvas widgets.
 *
 * Copyright (c) 1995 by Sven Delmas
 * All rights reserved.
 * See the file COPYRIGHT for the copyright notes. The file
 * "license.terms" is part of the Tk distribution and is
 * included in the file COPYRIGHT.
 *
 *
 * This source is based upon the file tkCanvImg.c from:
 *
 * John Ousterhout
 *
 * Copyright (c) 1994 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef lint
static char *AtFSid = "$Header: /usr1/master/RCS/TclTk/ccode/TkIcon/tkCanvIcon.c,v 2.0.0.1 1995/10/27 16:11:20 sachin Exp $";
#endif

#include <stdio.h>
#include "tkInt.h"
#include "tkPort.h"
#include "tkCanvas.h"

/* Sven Delmas */
/*
 * Map the original canvas image names to the new
 * canvas icon names. This code was derived from 
 * the image item, so to make adaption to new
 * image items easier the basic code is exactly 
 * the same.
 */
#include "tkIcon.h"

#define ImageItem IconItem
#define tkImageType tkIconType
#define imgPtr iconPtr
#define ComputeImageBbox ComputeIconBbox
#define ConfigureImage ConfigureIcon
#define CreateImage CreateIcon
#define DeleteImage DeleteIcon
#define DisplayImage DisplayIcon
#define ImageCoords IconCoords
#define ImageToArea IconToArea
#define ImageToPoint IconToPoint
#define ScaleImage ScaleIcon
#define TranslateImage TranslateIcon
#define ImageChangedProc IconChangeProc

/*
 * The distance between image and text.
 */

#define IMAGE_OFFSET 2
/* End Sven Delmas */

/*
 * The structure below defines the record for each image item.
 */

typedef struct ImageItem  {
    Tk_Item header;		/* Generic stuff that's the same for all
				 * types.  MUST BE FIRST IN STRUCTURE. */
    Tk_Canvas canvas;		/* Canvas containing the image. */
    double x, y;		/* Coordinates of positioning point for
				 * image. */
    char *imageString;		/* String describing -image option (malloc-ed).
				 * NULL means no image right now. */
    Tk_Image image;		/* Image to display in window, or NULL if
				 * no image at present. */

/* Sven Delmas */
    int borderwidth;		/* Width of outline (in pixels). */
    XColor *bgColor;            /* Background color to use for icon. */
    char *command;		/* Command for item, usually evaluated
                                 * on double-click. */
    char *dropCommand;		/* Drop command for item, usually */
				/* evaluated on drop. */
    char *edges;	        /* Edges bound to this image, this is a
				 * list of thw form {{fromId toId
				 * edgeId}...}. */
    XColor *fgColor;            /* Foreground color to use for icon. */
    XFontStruct *fontPtr;	/* Font for drawing text. */
    char *layoutInfo;           /* This option holds permanent
				 * layouting information. */
    char *menu1;	        /* Standard menu for item, usually
				 * activated with button-3. */
    char *menu2;		/* Alternative menu for item, usually
				 * activated with meta-button-3. */
    char *menu3;		/* Alternative menu for item, usually
				 * activated with control-button-3. */
    char *name;		        /* Name for item. */
    XColor *outlineColor;	/* Color for outline.  NULL means don't
				 * draw outline. */
    char *sortString;		/* The string that is used for
                                 * sorting, usually the same value as
                                 * stored in -name. */
    char *state;		/* State of item, this value is used
				 * to represent the selection status
				 * (normal, selected). */    
    int relief;			/* 3-d effect: TK_RELIEF_RAISED, etc. */
    
    char *graphName;	        /* String contains the graphName
                                 * to which the item is connected
				 * option (malloc-ed). NULL means
                                 * no graph connection. */
    char *statusImageString;	/* String describing -statusimage */
				/* option (malloc-ed).
				 * NULL means no image right now. */
    Tk_Image statusImage;	/* Status image to display in window,
				 * or NULL if no image at present. */
    char *text;			/* Text for item. */
    Tk_Anchor textAnchor;	/* Where to anchor text relative to
				 * image. */

    GC gc;			/* Graphics context to use for drawing
				 * text on screen. */
    GC invertedGc;		/* Graphics context to use for drawing
				 * text on screen. */
    GC outlineGc;		/* Graphics context for outline. */
    int imageX;                 /* Coordinates of positioning point */
    int imageY; 	        /* for text. */
    int textX;                  /* Coordinates of positioning point */
    int textY;	                /* for text. */
    int textWidth;              /* Width and height of the text. */
    int textHeight;
    int width;
/* End Sven Delmas */
} ImageItem;

/*
 * Information used for parsing configuration specs:
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_COLOR, "-background", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(ImageItem, bgColor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_INT, "-borderwidth", (char *) NULL, (char *) NULL,
	"0", Tk_Offset(ImageItem, borderwidth), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_STRING, "-command", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, command), 0},
    {TK_CONFIG_STRING, "-dropcommand", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, dropCommand), 0},
    {TK_CONFIG_STRING, "-edges", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, edges), 0},
    {TK_CONFIG_FONT, "-font", (char *) NULL, (char *) NULL,
	"-Adobe-Helvetica-Bold-R-Normal--*-120-*",
	Tk_Offset(ImageItem, fontPtr), 0},
    {TK_CONFIG_COLOR, "-foreground", (char *) NULL, (char *) NULL,
	"black", Tk_Offset(ImageItem, fgColor), 0},
    {TK_CONFIG_STRING, "-graphname", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, graphName), TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-image", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(ImageItem, imageString), TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-layoutinfo", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, layoutInfo), 0},
    {TK_CONFIG_STRING, "-menu1", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, menu1), 0},
    {TK_CONFIG_STRING, "-menu2", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, menu2), 0},
    {TK_CONFIG_STRING, "-menu3", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, menu3), 0},
    {TK_CONFIG_STRING, "-name", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, name), 0},
    {TK_CONFIG_COLOR, "-outline", (char *) NULL, (char *) NULL,
	"black", Tk_Offset(ImageItem, outlineColor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	"flat", Tk_Offset(ImageItem, relief), 0},
    {TK_CONFIG_STRING, "-sortstring", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, sortString), 0},
    {TK_CONFIG_STRING, "-state", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, state), 0},
    {TK_CONFIG_STRING, "-status", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, statusImageString), TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-statusimage", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(ImageItem, statusImage),
	TK_CONFIG_NULL_OK},
    {TK_CONFIG_CUSTOM, "-tags", (char *) NULL, (char *) NULL,
	(char *) NULL, 0, TK_CONFIG_NULL_OK, &tk_CanvasTagsOption},
    {TK_CONFIG_ANCHOR, "-textanchor", (char *) NULL, (char *) NULL,
	"center", Tk_Offset(ImageItem, textAnchor),
	TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_STRING, "-text", (char *) NULL, (char *) NULL,
	"", Tk_Offset(ImageItem, text), 0},
    {TK_CONFIG_INT, "-width", (char *) NULL, (char *) NULL,
	"0", Tk_Offset(ImageItem, width), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_DOUBLE, "-x", (char *) NULL, (char *) NULL,
	"0", Tk_Offset(ImageItem, x), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_DOUBLE, "-y", (char *) NULL, (char *) NULL,
	"0", Tk_Offset(ImageItem, y), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Prototypes for procedures defined in this file:
 */

static void		ImageChangedProc _ANSI_ARGS_((ClientData clientData,
			    int x, int y, int width, int height, int imgWidth,
			    int imgHeight));
static int		ImageCoords _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
			    char **argv));
static int		ImageToArea _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, double *rectPtr));
static double		ImageToPoint _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, double *coordPtr));
static void		ComputeImageBbox _ANSI_ARGS_((Tk_Canvas canvas,
			    ImageItem *imgPtr));
static int		ConfigureImage _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
			    char **argv, int flags));
static int		CreateImage _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Canvas canvas, struct Tk_Item *itemPtr,
			    int argc, char **argv));
static void		DeleteImage _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, Display *display));
static void		DisplayImage _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, Display *display, Drawable dst,
			    int x, int y, int width, int height));
static void		ScaleImage _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, double originX, double originY,
			    double scaleX, double scaleY));
static void		TranslateImage _ANSI_ARGS_((Tk_Canvas canvas,
			    Tk_Item *itemPtr, double deltaX, double deltaY));

extern int TkGraph_CreateGraph _ANSI_ARGS_((Tcl_Interp *interp,
                                            char *graphName,
                                            TkCanvas *canvasPtr));
/*
 * The structures below defines the image item type in terms of
 * procedures that can be invoked by generic item code.
 */

Tk_ItemType tkImageType = {
    "icon",				/* name */
    sizeof(ImageItem),			/* itemSize */
    CreateImage,			/* createProc */
    configSpecs,			/* configSpecs */
    ConfigureImage,			/* configureProc */
    ImageCoords,			/* coordProc */
    DeleteImage,			/* deleteProc */
    DisplayImage,			/* displayProc */
    0,					/* alwaysRedraw */
    ImageToPoint,			/* pointProc */
    ImageToArea,			/* areaProc */
    (Tk_ItemPostscriptProc *) NULL,	/* postscriptProc */
    ScaleImage,				/* scaleProc */
    TranslateImage,			/* translateProc */
    (Tk_ItemIndexProc *) NULL,		/* indexProc */
    (Tk_ItemCursorProc *) NULL,		/* icursorProc */
    (Tk_ItemSelectionProc *) NULL,	/* selectionProc */
    (Tk_ItemInsertProc *) NULL,		/* insertProc */
    (Tk_ItemDCharsProc *) NULL,		/* dTextProc */
    (Tk_ItemType *) NULL		/* nextPtr */
};

/*
 *--------------------------------------------------------------
 *
 * CreateImage --
 *
 *	This procedure is invoked to create a new image
 *	item in a canvas.
 *
 * Results:
 *	A standard Tcl return value.  If an error occurred in
 *	creating the item, then an error message is left in
 *	interp->result;  in this case itemPtr is left uninitialized,
 *	so it can be safely freed by the caller.
 *
 * Side effects:
 *	A new image item is created.
 *
 *--------------------------------------------------------------
 */

static int
CreateImage(interp, canvas, itemPtr, argc, argv)
    Tcl_Interp *interp;			/* Interpreter for error reporting. */
    Tk_Canvas canvas;			/* Canvas to hold new item. */
    Tk_Item *itemPtr;			/* Record to hold new item;  header
					 * has been initialized by caller. */
    int argc;				/* Number of arguments in argv. */
    char **argv;			/* Arguments describing rectangle. */
{
    ImageItem *imgPtr = (ImageItem *) itemPtr;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		Tk_PathName(Tk_CanvasTkwin(canvas)), " create ",
		itemPtr->typePtr->name, " x y ?options?\"",
		(char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Initialize item's record.
     */

    imgPtr->canvas = canvas;
    imgPtr->imageString = NULL;
    imgPtr->image = NULL;

/* Sven Delmas */
    imgPtr->bgColor = NULL;
    imgPtr->borderwidth = 0;
    imgPtr->command = NULL;
    imgPtr->dropCommand = NULL;
    imgPtr->edges = NULL;
    imgPtr->fgColor = NULL;
    imgPtr->fontPtr = NULL;
    imgPtr->graphName = NULL;
    imgPtr->layoutInfo = NULL;
    imgPtr->menu1 = NULL;
    imgPtr->menu2 = NULL;
    imgPtr->menu3 = NULL;
    imgPtr->name = NULL;
    imgPtr->outlineColor = NULL;
    imgPtr->relief = TK_RELIEF_FLAT;
    imgPtr->sortString = NULL;
    imgPtr->state = NULL;
    imgPtr->statusImageString = NULL;
    imgPtr->statusImage = NULL;
    imgPtr->text = NULL;
    imgPtr->textAnchor = TK_ANCHOR_S;
    imgPtr->width = 0;
    imgPtr->gc = None;
    imgPtr->invertedGc = None;
    imgPtr->outlineGc = None;
/* End Sven Delmas */

    /*
     * Process the arguments to fill in the item record.
     */
/* Sven Delmas */
    if (argc >= 2) {
	/* did we get a default x and y position as first two parameters ? */
	if ((argv[0][0] == '-' && isdigit(argv[0][1])) ||
	    isdigit(argv[0][0])) {
	    /* the first two parameters are numbers... so use them as x and */
	    /* y coordinates. */
	    if ((Tk_CanvasGetCoord(interp, canvas, argv[0], &imgPtr->x)
		 != TCL_OK) ||
		(Tk_CanvasGetCoord(interp, canvas, argv[1], &imgPtr->y)
		 != TCL_OK)) {
		return TCL_ERROR;
	    }
      
	    if (ConfigureImage(interp, canvas, itemPtr,
				argc-2, argv+2, 0) != TCL_OK) {
		DeleteImage(canvas, itemPtr,
			     Tk_Display(Tk_CanvasTkwin(canvas)));
		return TCL_ERROR;
	    }
	} else {
	    if (ConfigureImage(interp, canvas, itemPtr,
				argc, argv, 0) != TCL_OK) {
		DeleteImage(canvas, itemPtr,
			     Tk_Display(Tk_CanvasTkwin(canvas)));
		return TCL_ERROR;
	    }
	}
    }
/* End Sven Delmas */

    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * ImageCoords --
 *
 *	This procedure is invoked to process the "coords" widget
 *	command on image items.  See the user documentation for
 *	details on what it does.
 *
 * Results:
 *	Returns TCL_OK or TCL_ERROR, and sets interp->result.
 *
 * Side effects:
 *	The coordinates for the given item may be changed.
 *
 *--------------------------------------------------------------
 */

static int
ImageCoords(interp, canvas, itemPtr, argc, argv)
    Tcl_Interp *interp;			/* Used for error reporting. */
    Tk_Canvas canvas;			/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item whose coordinates are to be
					 * read or modified. */
    int argc;				/* Number of coordinates supplied in
					 * argv. */
    char **argv;			/* Array of coordinates: x1, y1,
					 * x2, y2, ... */
{
    ImageItem *imgPtr = (ImageItem *) itemPtr;
    char x[TCL_DOUBLE_SPACE], y[TCL_DOUBLE_SPACE];

    if (argc == 0) {
	Tcl_PrintDouble(interp, imgPtr->x, x);
	Tcl_PrintDouble(interp, imgPtr->y, y);
	Tcl_AppendResult(interp, x, " ", y, (char *) NULL);
    } else if (argc == 2) {
	if ((Tk_CanvasGetCoord(interp, canvas, argv[0], &imgPtr->x) != TCL_OK)
		|| (Tk_CanvasGetCoord(interp, canvas, argv[1],
		    &imgPtr->y) != TCL_OK)) {
	    return TCL_ERROR;
	}
	ComputeImageBbox(canvas, imgPtr);
    } else {
	sprintf(interp->result,
		"wrong # coordinates:  expected 0 or 2, got %d", argc);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * ConfigureImage --
 *
 *	This procedure is invoked to configure various aspects
 *	of an image item, such as its anchor position.
 *
 * Results:
 *	A standard Tcl result code.  If an error occurs, then
 *	an error message is left in interp->result.
 *
 * Side effects:
 *	Configuration information may be set for itemPtr.
 *
 *--------------------------------------------------------------
 */

static int
ConfigureImage(interp, canvas, itemPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    Tk_Canvas canvas;		/* Canvas containing itemPtr. */
    Tk_Item *itemPtr;		/* Image item to reconfigure. */
    int argc;			/* Number of elements in argv.  */
    char **argv;		/* Arguments describing things to configure. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    ImageItem *imgPtr = (ImageItem *) itemPtr;
    Tk_Window tkwin;
    Tk_Image image;

/* Sven Delmas */
    XGCValues gcValues;
    GC newGC;
    unsigned long mask;
    TkCanvas *canvasPtr = (TkCanvas *) canvas;

    /* no negative borders */
    if (imgPtr->borderwidth < 0) {
	imgPtr->borderwidth = 0;
    }

    /*
     * Free the old image for the widget, if there was one, then
     * configure the widget and (re-)get the image, if there is
     * one.
     */

    if (imgPtr->image != NULL) {
	Tk_FreeImage(imgPtr->image);
	imgPtr->image = NULL;
    }
    if (imgPtr->statusImage != NULL) {
	Tk_FreeImage(imgPtr->statusImage);
	imgPtr->statusImage = NULL;
    }
/* End Sven Delmas */

    tkwin = Tk_CanvasTkwin(canvas);
    if (Tk_ConfigureWidget(interp, tkwin, configSpecs, argc,
	    argv, (char *) imgPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

/* Sven Delmas */
    /*
     * attach the icon to a specific graph
     */
    if (imgPtr->graphName != NULL) {
      if (TkGraph_CreateGraph(interp, imgPtr->graphName,
			      canvasPtr) != TCL_OK) {
	return TCL_ERROR;
      }
    }
/* End Sven Delmas */

    if (imgPtr->imageString != NULL) {
	image = Tk_GetImage(interp, tkwin, imgPtr->imageString,
		ImageChangedProc, (ClientData) imgPtr);
	if (image == NULL) {
	    return TCL_ERROR;
	}
    } else {
	image = NULL;
    }
    if (imgPtr->image != NULL) {
	Tk_FreeImage(imgPtr->image);
    }
    imgPtr->image = image;

/* Sven Delmas */
    if (imgPtr->statusImageString != NULL) {
	image = Tk_GetImage(interp, tkwin, imgPtr->statusImageString,
		ImageChangedProc, (ClientData) imgPtr);
	if (image == NULL) {
	    return TCL_ERROR;
	}
    } else {
	image = NULL;
    }
    if (imgPtr->statusImage != NULL) {
	Tk_FreeImage(imgPtr->statusImage);
    }
    imgPtr->statusImage = image;

    /*
     * A few of the options require additional processing, such as those
     * that determine the graphics context.
     */

    gcValues.foreground = imgPtr->fgColor->pixel;
    mask = GCForeground | GCBackground;
    if (imgPtr->bgColor != NULL) {
	gcValues.background = imgPtr->bgColor->pixel;
    } else {
	gcValues.background = Tk_3DBorderColor(canvasPtr->bgBorder)->pixel;
    }
    if ((imgPtr->fgColor != NULL) && (imgPtr->fontPtr != NULL)) {
	gcValues.font = imgPtr->fontPtr->fid;
	mask |= GCFont;
    }
    newGC = Tk_GetGC(tkwin, mask, &gcValues);
    if (imgPtr->gc != None) {
	Tk_FreeGC(Tk_Display(tkwin), imgPtr->gc);
    }
    imgPtr->gc = newGC;

    /* the inverted display gc */
    gcValues.background = imgPtr->fgColor->pixel;
    mask = GCForeground | GCBackground;
    if (imgPtr->bgColor != NULL) {
	gcValues.foreground = imgPtr->bgColor->pixel;
    } else {
	gcValues.foreground = Tk_3DBorderColor(canvasPtr->bgBorder)->pixel;
    }
    if ((imgPtr->fgColor != NULL) && (imgPtr->fontPtr != NULL)) {
	gcValues.font = imgPtr->fontPtr->fid;
	mask |= GCFont;
    }
    newGC = Tk_GetGC(tkwin, mask, &gcValues);
    if (imgPtr->invertedGc != None) {
	Tk_FreeGC(Tk_Display(tkwin), imgPtr->invertedGc);
    }
    imgPtr->invertedGc = newGC;

    /* the outline color gc */
    if (imgPtr->outlineColor == NULL) {
	newGC = None;
    } else {
	gcValues.foreground = imgPtr->outlineColor->pixel;
	gcValues.cap_style = CapProjecting;
	gcValues.line_width = imgPtr->borderwidth;
	mask = GCForeground|GCCapStyle|GCLineWidth;
	newGC = Tk_GetGC(tkwin, mask, &gcValues);
    }
    if (imgPtr->outlineGc != None) {
	Tk_FreeGC(Tk_Display(tkwin), imgPtr->outlineGc);
    }
    imgPtr->outlineGc = newGC;
/* End Sven Delmas */

    ComputeImageBbox(canvas, imgPtr);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * DeleteImage --
 *
 *	This procedure is called to clean up the data structure
 *	associated with a image item.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Resources associated with itemPtr are released.
 *
 *--------------------------------------------------------------
 */

static void
DeleteImage(canvas, itemPtr, display)
    Tk_Canvas canvas;			/* Info about overall canvas widget. */
    Tk_Item *itemPtr;			/* Item that is being deleted. */
    Display *display;			/* Display containing window for
					 * canvas. */
{
    ImageItem *imgPtr = (ImageItem *) itemPtr;

    if (imgPtr->imageString != NULL) {
	ckfree(imgPtr->imageString);
    }
    if (imgPtr->image != NULL) {
	Tk_FreeImage(imgPtr->image);
    }

/* Sven Delmas */
    if (imgPtr->bgColor != NULL) {
	Tk_FreeColor(imgPtr->bgColor);
    }
    if (imgPtr->command != NULL) {
	ckfree(imgPtr->command);
    }
    if (imgPtr->dropCommand != NULL) {
	ckfree(imgPtr->dropCommand);
    }
    if (imgPtr->edges != NULL) {
	ckfree(imgPtr->edges);
    }
    if (imgPtr->fgColor != NULL) {
	Tk_FreeColor(imgPtr->fgColor);
    }
    if (imgPtr->fontPtr != NULL) {
	Tk_FreeFontStruct(imgPtr->fontPtr);
    }
    if (imgPtr->graphName != NULL) {
	ckfree(imgPtr->graphName);
    }
    if (imgPtr->layoutInfo != NULL) {
	ckfree(imgPtr->layoutInfo);
    }
    if (imgPtr->menu1 != NULL) {
	ckfree(imgPtr->menu1);
    }
    if (imgPtr->menu2 != NULL) {
	ckfree(imgPtr->menu2);
    }
    if (imgPtr->menu3 != NULL) {
	ckfree(imgPtr->menu3);
    }
    if (imgPtr->name != NULL) {
	ckfree(imgPtr->name);
    }
    if (imgPtr->outlineColor != NULL) {
	Tk_FreeColor(imgPtr->outlineColor);
    }
    if (imgPtr->sortString != NULL) {
	ckfree(imgPtr->sortString);
    }
    if (imgPtr->state != NULL) {
	ckfree(imgPtr->state);
    }
    if (imgPtr->statusImageString != NULL) {
	ckfree(imgPtr->statusImageString);
    }
    if (imgPtr->statusImage != NULL) {
	Tk_FreeImage(imgPtr->statusImage);
    }
    if (imgPtr->text != NULL) {
	ckfree(imgPtr->text);
    }
    if (imgPtr->gc != NULL) {
	Tk_FreeGC(display, imgPtr->gc);
    }
    if (imgPtr->invertedGc != NULL) {
	Tk_FreeGC(display, imgPtr->invertedGc);
    }
    if (imgPtr->outlineGc != NULL) {
	Tk_FreeGC(display, imgPtr->outlineGc);
    }
/* End Sven Delmas */
}

/*
 *--------------------------------------------------------------
 *
 * ComputeImageBbox --
 *
 *	This procedure is invoked to compute the bounding box of
 *	all the pixels that may be drawn as part of a image item.
 *	This procedure is where the child image's placement is
 *	computed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The fields x1, y1, x2, and y2 are updated in the header
 *	for itemPtr.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static void
ComputeImageBbox(canvas, imgPtr)
    Tk_Canvas canvas;			/* Canvas that contains item. */
    ImageItem *imgPtr;			/* Item whose bbox is to be
					 * recomputed. */
{
    int width, height, imgWidth, imgHeight;
    int x, y;

/* Sven Delmas */
    int textWidth = 0;
    int lineHeight, lineWidth, numChars, ascent, descent;
/* End Sven Delmas */

    x = imgPtr->x + ((imgPtr->x >= 0) ? 0.5 : - 0.5);
    y = imgPtr->y + ((imgPtr->y >= 0) ? 0.5 : - 0.5);

    if (imgPtr->image == None && imgPtr->text == NULL) {
	imgPtr->header.x1 = imgPtr->header.x2 = x;
	imgPtr->header.y1 = imgPtr->header.y2 = y;
	return;
    }

    /*
     * Compute location and size of image, using anchor information.
     */

/* Sven Delmas */
    if (imgPtr->image != None) {
	Tk_SizeOfImage(imgPtr->image, &imgWidth, &imgHeight);
        width = imgWidth;
	height = imgHeight;
    } else {
	imgWidth = 0;
	imgHeight = 0;
	width = 0;
	height = 0;
    }
    ascent = imgPtr->fontPtr->ascent;
    descent = imgPtr->fontPtr->descent;
    lineHeight = imgPtr->fontPtr->ascent + imgPtr->fontPtr->descent;
    if (imgPtr->width != 0 &&
	imgPtr->width < (int) strlen(imgPtr->text)) {
	textWidth = imgPtr->width;
    } else {
	textWidth = strlen(imgPtr->text);
    }
    numChars = TkMeasureChars(imgPtr->fontPtr, imgPtr->text,
			      textWidth, 0, 10000000, 0,
			      TK_AT_LEAST_ONE, &lineWidth);
    switch (imgPtr->textAnchor) {
    case TK_ANCHOR_N:
	imgPtr->textY = y + lineHeight + imgPtr->borderwidth -
            IMAGE_OFFSET;
	imgPtr->imageY = y + lineHeight + IMAGE_OFFSET +
	    IMAGE_OFFSET + imgPtr->borderwidth;
	if (imgWidth > lineWidth) {
	    width = imgWidth;
	    imgPtr->textX = x + (imgWidth/2) -
		(lineWidth/2) + imgPtr->borderwidth;
	    imgPtr->imageX = x + imgPtr->borderwidth;
	} else {
	    width = lineWidth;
	    imgPtr->textX = x + imgPtr->borderwidth;
	    imgPtr->imageX = x + (lineWidth/2) -
		(imgWidth/2) + imgPtr->borderwidth;
	}
	height += lineHeight + IMAGE_OFFSET + IMAGE_OFFSET;
	break;
    case TK_ANCHOR_NW:
	imgPtr->textX = x + imgPtr->borderwidth;
	imgPtr->imageX = x + imgPtr->borderwidth;
	imgPtr->textY = y + lineHeight + imgPtr->borderwidth -
            IMAGE_OFFSET;
	imgPtr->imageY = y + lineHeight + IMAGE_OFFSET +
	    IMAGE_OFFSET + imgPtr->borderwidth;
	if (imgWidth > lineWidth) {
	    width = imgWidth;
	} else {
	    width = lineWidth;
	}
	height += lineHeight + IMAGE_OFFSET + IMAGE_OFFSET;
	break;
    case TK_ANCHOR_NE:
	imgPtr->textY = y + lineHeight + imgPtr->borderwidth -
            IMAGE_OFFSET;
	imgPtr->imageY = y + lineHeight + IMAGE_OFFSET +
	    IMAGE_OFFSET + imgPtr->borderwidth;
	if (imgWidth > lineWidth) {
	    width = imgWidth;
	    imgPtr->textX = x + (imgWidth - lineWidth) +
		imgPtr->borderwidth;
	    imgPtr->imageX = x + imgPtr->borderwidth;
	} else {
	    width = lineWidth;
	    imgPtr->textX = x + imgPtr->borderwidth;
	    imgPtr->imageX = x + (lineWidth - imgWidth) +
		imgPtr->borderwidth;
	}
	height += lineHeight + IMAGE_OFFSET + IMAGE_OFFSET;
	break;
    case TK_ANCHOR_S:
	imgPtr->textY = y + height + ascent + IMAGE_OFFSET +
	    imgPtr->borderwidth;
	imgPtr->imageY = y + imgPtr->borderwidth;
	if (imgWidth > lineWidth) {
	    width = imgWidth;
	    imgPtr->textX = x + (imgWidth/2) -
		(lineWidth/2) + imgPtr->borderwidth;
	    imgPtr->imageX = x + imgPtr->borderwidth;
	} else {
	    width = lineWidth;
	    imgPtr->textX = x + imgPtr->borderwidth;
	    imgPtr->imageX = x + (lineWidth/2) -
		(imgWidth/2) + imgPtr->borderwidth;
	}
	height += lineHeight + IMAGE_OFFSET + IMAGE_OFFSET;
	break;
    case TK_ANCHOR_SW:
	imgPtr->textX = x + imgPtr->borderwidth;
	imgPtr->imageX = x + imgPtr->borderwidth;
	imgPtr->textY = y + height + ascent + IMAGE_OFFSET +
	    imgPtr->borderwidth;
	imgPtr->imageY = y + imgPtr->borderwidth;
	if (imgWidth > lineWidth) {
	    width = imgWidth;
	} else {
	    width = lineWidth;
	}
	height += lineHeight + IMAGE_OFFSET + IMAGE_OFFSET;
	break;
    case TK_ANCHOR_SE:
	imgPtr->textY = y + height + ascent + IMAGE_OFFSET +
	    imgPtr->borderwidth;
	imgPtr->imageY = y + imgPtr->borderwidth;
	if (imgWidth > lineWidth) {
	    width = imgWidth;
	    imgPtr->textX = x + (imgWidth - lineWidth) +
		imgPtr->borderwidth;
	    imgPtr->imageX = x + imgPtr->borderwidth;
	} else {
	    width = lineWidth;
	    imgPtr->textX = x + imgPtr->borderwidth;
	    imgPtr->imageX = x + (lineWidth - imgWidth) +
		imgPtr->borderwidth;
	}
	height += lineHeight + IMAGE_OFFSET + IMAGE_OFFSET;
	break;
    case TK_ANCHOR_W:
	imgPtr->textY = y + (height/2) + descent +
	    imgPtr->borderwidth;
	imgPtr->imageY = y + imgPtr->borderwidth;
	imgPtr->textX = x + imgPtr->borderwidth;
	imgPtr->imageX = x + lineWidth + IMAGE_OFFSET +
	    imgPtr->borderwidth;
	width += lineWidth + IMAGE_OFFSET;
	break;
    case TK_ANCHOR_E:
	imgPtr->textY = y + (height/2) + descent +
	    imgPtr->borderwidth;
	imgPtr->imageY = y + imgPtr->borderwidth;
	imgPtr->textX = x + width + IMAGE_OFFSET +
	    imgPtr->borderwidth;
	imgPtr->imageX = x + imgPtr->borderwidth;
	width += lineWidth + IMAGE_OFFSET;
	break;
    case TK_ANCHOR_CENTER:
	if (imgWidth > lineWidth) {
	    width = imgWidth;
	    imgPtr->textX = x + (imgWidth/2) -
		(lineWidth/2) + imgPtr->borderwidth;
	    imgPtr->imageX = x + imgPtr->borderwidth;
	} else {
	    width = lineWidth;
	    imgPtr->textX = x + imgPtr->borderwidth;
	    imgPtr->imageX = x + (lineWidth/2) -
		(imgWidth/2) + imgPtr->borderwidth;
	}
	if (imgHeight > lineHeight) {
	    height = imgHeight;
	    imgPtr->textY = y + (imgHeight/2) +
		descent + imgPtr->borderwidth;
	    imgPtr->imageY = y + imgPtr->borderwidth;
	} else {
	    height = lineHeight;
	    imgPtr->textY = y + lineHeight - descent +
		imgPtr->borderwidth;
	    imgPtr->imageY = y + (lineHeight/2) -
		(imgHeight/2) + imgPtr->borderwidth;
	}
	break;
    }
/* End Sven Delmas */

    /*
     * Store the information in the item header.
     */

    imgPtr->header.x1 = x;
    imgPtr->header.y1 = y;

/* Sven Delmas */
    imgPtr->header.x2 = x + width + imgPtr->borderwidth +
	imgPtr->borderwidth;
    imgPtr->header.y2 = y + height + imgPtr->borderwidth +
	imgPtr->borderwidth;
    imgPtr->textWidth = lineWidth;
    imgPtr->textHeight = lineHeight;
/* End Sven Delmas */
}

/*
 *--------------------------------------------------------------
 *
 * DisplayImage --
 *
 *	This procedure is invoked to draw a image item in a given
 *	drawable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	ItemPtr is drawn in drawable using the transformation
 *	information in canvas.
 *
 *--------------------------------------------------------------
 */

static void
DisplayImage(canvas, itemPtr, display, drawable, x, y, width, height)
    Tk_Canvas canvas;			/* Canvas that contains item. */
    Tk_Item *itemPtr;			/* Item to be displayed. */
    Display *display;			/* Display on which to draw item. */
    Drawable drawable;			/* Pixmap or window in which to draw
					 * item. */
    int x, y, width, height;		/* Describes region of canvas that
					 * must be redisplayed (not used). */
{
    ImageItem *imgPtr = (ImageItem *) itemPtr;
    short drawableX, drawableY;

/* Sven Delmas */
    int textWidth;
    TkCanvas *canvasPtr = (TkCanvas *) canvas;

    /* display image */
    if (imgPtr->image != NULL) {
        Tk_CanvasDrawableCoords(canvas, (double) x, (double) y,
				&drawableX, &drawableY);
	Tk_RedrawImage(imgPtr->image, x - imgPtr->header.x1,
                       y - imgPtr->header.y1, width, height,
                       drawable, drawableX, drawableY);
    }

    /* display status image */
    if (imgPtr->statusImage != None) {
        Tk_CanvasDrawableCoords(canvas, (double) x, (double) y,
				&drawableX, &drawableY);
	Tk_RedrawImage(imgPtr->statusImage, x - imgPtr->header.x1,
                       y - imgPtr->header.y1, width, height,
                       drawable, drawableX, drawableY);
    }

    /* display text */
    if (imgPtr->text != NULL) {
        Tk_CanvasDrawableCoords(canvas, (double) imgPtr->textX,
				(double) imgPtr->textY,
				&drawableX, &drawableY);
	if (imgPtr->width != 0 &&
	    imgPtr->width < (int) strlen(imgPtr->text)) {
	    textWidth = imgPtr->width;
	} else {
	    textWidth = strlen(imgPtr->text);
	}
	if (strcmp(imgPtr->state, "selected") == 0) {
	    XFillRectangle(display, drawable, imgPtr->gc, drawableX,
			   drawableY - imgPtr->fontPtr->ascent -
			   IMAGE_OFFSET, (unsigned int) imgPtr->textWidth,
			   (unsigned int) imgPtr->textHeight +
			   IMAGE_OFFSET + IMAGE_OFFSET);
	    TkDisplayChars(display, drawable, imgPtr->invertedGc,
			   imgPtr->fontPtr, imgPtr->text, textWidth,
			   drawableX, drawableY, 0, 0);
	} else {
	    XFillRectangle(display, drawable, imgPtr->invertedGc, drawableX,
			   drawableY - imgPtr->fontPtr->ascent -
			   IMAGE_OFFSET, (unsigned int) imgPtr->textWidth,
			   (unsigned int) imgPtr->textHeight +
			   IMAGE_OFFSET + IMAGE_OFFSET);
	    TkDisplayChars(display, drawable, imgPtr->gc,
			   imgPtr->fontPtr, imgPtr->text, textWidth,
			   drawableX, drawableY, 0, 0);
	}
    }

    /* draw border */
    if (imgPtr->borderwidth > 0) {
        Tk_CanvasDrawableCoords(canvas, (double) imgPtr->header.x1,
				(double) imgPtr->header.y1,
				&drawableX, &drawableY);
	if (imgPtr->relief == TK_RELIEF_FLAT) {
	    if (imgPtr->outlineGc != None) {
		XDrawRectangle(display, drawable, imgPtr->outlineGc,
			       drawableX + (imgPtr->borderwidth / 2),
			       drawableY + (imgPtr->borderwidth / 2),
			       (unsigned int) (imgPtr->header.x2 -
					       imgPtr->header.x1) -
			       imgPtr->borderwidth,
			       (unsigned int) (imgPtr->header.y2 -
					       imgPtr->header.y1) -
			       imgPtr->borderwidth);
	    }
	} else {
	    Tk_Draw3DRectangle(Tk_CanvasTkwin(canvas), drawable,
			       canvasPtr->bgBorder, drawableX, drawableY,
			       (imgPtr->header.x2 - imgPtr->header.x1),
			       (imgPtr->header.y2 - imgPtr->header.y1),
			       imgPtr->borderwidth, imgPtr->relief);
	}
    }
/* End Sven Delmas */
}

/*
 *--------------------------------------------------------------
 *
 * ImageToPoint --
 *
 *	Computes the distance from a given point to a given
 *	rectangle, in canvas units.
 *
 * Results:
 *	The return value is 0 if the point whose x and y coordinates
 *	are coordPtr[0] and coordPtr[1] is inside the image.  If the
 *	point isn't inside the image then the return value is the
 *	distance from the point to the image.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static double
ImageToPoint(canvas, itemPtr, coordPtr)
    Tk_Canvas canvas;		/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against point. */
    double *coordPtr;		/* Pointer to x and y coordinates. */
{
    ImageItem *imgPtr = (ImageItem *) itemPtr;
    double x1, x2, y1, y2, xDiff, yDiff;

    x1 = imgPtr->header.x1;
    y1 = imgPtr->header.y1;
    x2 = imgPtr->header.x2;
    y2 = imgPtr->header.y2;

    /*
     * Point is outside rectangle.
     */

    if (coordPtr[0] < x1) {
	xDiff = x1 - coordPtr[0];
    } else if (coordPtr[0] > x2)  {
	xDiff = coordPtr[0] - x2;
    } else {
	xDiff = 0;
    }

    if (coordPtr[1] < y1) {
	yDiff = y1 - coordPtr[1];
    } else if (coordPtr[1] > y2)  {
	yDiff = coordPtr[1] - y2;
    } else {
	yDiff = 0;
    }

    return hypot(xDiff, yDiff);
}

/*
 *--------------------------------------------------------------
 *
 * ImageToArea --
 *
 *	This procedure is called to determine whether an item
 *	lies entirely inside, entirely outside, or overlapping
 *	a given rectangle.
 *
 * Results:
 *	-1 is returned if the item is entirely outside the area
 *	given by rectPtr, 0 if it overlaps, and 1 if it is entirely
 *	inside the given area.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static int
ImageToArea(canvas, itemPtr, rectPtr)
    Tk_Canvas canvas;		/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against rectangle. */
    double *rectPtr;		/* Pointer to array of four coordinates
				 * (x1, y1, x2, y2) describing rectangular
				 * area.  */
{
    ImageItem *imgPtr = (ImageItem *) itemPtr;

    if ((rectPtr[2] <= imgPtr->header.x1)
	    || (rectPtr[0] >= imgPtr->header.x2)
	    || (rectPtr[3] <= imgPtr->header.y1)
	    || (rectPtr[1] >= imgPtr->header.y2)) {
	return -1;
    }
    if ((rectPtr[0] <= imgPtr->header.x1)
	    && (rectPtr[1] <= imgPtr->header.y1)
	    && (rectPtr[2] >= imgPtr->header.x2)
	    && (rectPtr[3] >= imgPtr->header.y2)) {
	return 1;
    }
    return 0;
}

/*
 *--------------------------------------------------------------
 *
 * ScaleImage --
 *
 *	This procedure is invoked to rescale an item.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The item referred to by itemPtr is rescaled so that the
 *	following transformation is applied to all point coordinates:
 *		x' = originX + scaleX*(x-originX)
 *		y' = originY + scaleY*(y-originY)
 *
 *--------------------------------------------------------------
 */

static void
ScaleImage(canvas, itemPtr, originX, originY, scaleX, scaleY)
    Tk_Canvas canvas;			/* Canvas containing rectangle. */
    Tk_Item *itemPtr;			/* Rectangle to be scaled. */
    double originX, originY;		/* Origin about which to scale rect. */
    double scaleX;			/* Amount to scale in X direction. */
    double scaleY;			/* Amount to scale in Y direction. */
{
    ImageItem *imgPtr = (ImageItem *) itemPtr;

    imgPtr->x = originX + scaleX*(imgPtr->x - originX);
    imgPtr->y = originY + scaleY*(imgPtr->y - originY);
    ComputeImageBbox(canvas, imgPtr);
}

/*
 *--------------------------------------------------------------
 *
 * TranslateImage --
 *
 *	This procedure is called to move an item by a given amount.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The position of the item is offset by (xDelta, yDelta), and
 *	the bounding box is updated in the generic part of the item
 *	structure.
 *
 *--------------------------------------------------------------
 */

static void
TranslateImage(canvas, itemPtr, deltaX, deltaY)
    Tk_Canvas canvas;			/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item that is being moved. */
    double deltaX, deltaY;		/* Amount by which item is to be
					 * moved. */
{
    ImageItem *imgPtr = (ImageItem *) itemPtr;

    imgPtr->x += deltaX;
    imgPtr->y += deltaY;
    ComputeImageBbox(canvas, imgPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ImageChangedProc --
 *
 *	This procedure is invoked by the image code whenever the manager
 *	for an image does something that affects the image's size or
 *	how it is displayed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Arranges for the canvas to get redisplayed.
 *
 *----------------------------------------------------------------------
 */

static void
ImageChangedProc(clientData, x, y, width, height, imgWidth, imgHeight)
    ClientData clientData;		/* Pointer to canvas item for image. */
    int x, y;				/* Upper left pixel (within image)
					 * that must be redisplayed. */
    int width, height;			/* Dimensions of area to redisplay
					 * (may be <= 0). */
    int imgWidth, imgHeight;		/* New dimensions of image. */
{
    ImageItem *imgPtr = (ImageItem *) clientData;

    /*
     * If the image's size changed and it's not anchored at its
     * northwest corner then just redisplay the entire area of the
     * image.  This is a bit over-conservative, but we need to do
     * something because a size change also means a position change.
     */

    if (((imgPtr->header.x2 - imgPtr->header.x1) != imgWidth)
	    || ((imgPtr->header.y2 - imgPtr->header.y1) != imgHeight)) {
	x = y = 0;
	width = imgWidth;
	height = imgHeight;
	Tk_CanvasEventuallyRedraw(imgPtr->canvas, imgPtr->header.x1,
		imgPtr->header.y1, imgPtr->header.x2, imgPtr->header.y2);
    } 
    ComputeImageBbox(imgPtr->canvas, imgPtr);
    Tk_CanvasEventuallyRedraw(imgPtr->canvas, imgPtr->header.x1 + x,
	    imgPtr->header.y1 + y, (int) (imgPtr->header.x1 + x + width),
	    (int) (imgPtr->header.y1 + y + height));
}

/* Sven Delmas */
/*
 *----------------------------------------------------------------------
 *
 * TkIconIcon_Init --
 *
 *      This procedure initializes the icon item.
 *
 * Results:
 *      None
 *
 * Side effects:
 *      Adds the new item type to the canvas widget.
 *
 *----------------------------------------------------------------------
 */
void
TkIconIcon_Init ()
{
    Tk_CreateItemType((Tk_ItemType *) &tkImageType);
}
/* End Sven Delmas */

/* eof */


